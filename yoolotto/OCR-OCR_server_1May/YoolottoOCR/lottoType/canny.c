/*
 * canny.c
 *
 *  Created on: Oct 30, 2014
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include  "../headers/Image.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include  "../headers/Functions.h"


#define DEFAULT_GAUSS_TRUNCATE  4.0




FImage sobelDx, sobelDy;
float *kernel;
int ksize;

//float* makeKernel(float sigma, float truncate);
void makeKernel(float sigma, float truncate);
void convolveVertical(LPFImage image, float *kernel, int ksize);
void convolveHorizontal(LPFImage image, float *kernel, int ksize);
void  FSobelX(LPFImage image);
void  FSobelY(LPFImage image);
void fImageClone(LPFImage clone, LPFImage img);
//void cpyFImage(LPFImage imgOut, LPFImage img);
void freeFImage(LPFImage image);
void newFImage(LPFImage imageOut, int width, int height);
void computeSuppressed(LPFImage dxImage, LPFImage dyImage, LPFImage magsOut, LPFImage magnitudes);
void processImageSobel(LPFImage output, LPFImage dx, LPFImage dy);
float computeHighThreshold(LPFImage magnitudes);
void normaliseFImage(LPFImage img);
void zero(LPFImage img);
void thresholdingTracker(LPFImage magnitude, LPFImage output, float low, float high);
void analyseImageSobel(LPFImage image);
void imageProcessFGaussianConvolve(LPFImage image, float sigma);


float lowThresh = -1;
float highThresh = -1;
float threshRatio = 0.4f;
extern float sigma;
//float sigma = 1;


	void cannyProcessImageSobel(LPFImage image, float sigma) {


		FImage clone; clone.pixels = NULL;
		fImageClone(&clone, image);

		analyseImageSobel(&clone);

		freeFImage(&clone);

		processImageSobel(image, &sobelDx, &sobelDy);

	}


	void processImageSobel(LPFImage output, LPFImage dx, LPFImage dy) {
		FImage tmpMags;
		newFImage(&tmpMags,  dx->width, dx->height);


		FImage magnitudes;
		computeSuppressed(dx,  dy, &tmpMags, &magnitudes);

		normaliseFImage(&magnitudes);


		float low = lowThresh;
		float high = highThresh;
		if (high < 0) {
			// if high has not been set we use a similar approach to matlab to
			// estimate the thresholds
			high = computeHighThreshold(&tmpMags);
			low = threshRatio * high;
		}

//		printf("low= %e, high= %e\n",low, high);
//		fflush(stdout);
		freeFImage(&tmpMags);
		thresholdingTracker(&magnitudes, output, low, high);
		freeFImage(&magnitudes);

	}

void histogramCanny(LPFImage img, double* histogram, int nbins){
//	this.histogram = new Histogram(nbins);
	int r, c, bin;
//	histogram[0] = nbins;
	for (r = 0; r < img->height; r++)	{
		for (c = 0; c < img->width; c++)	{
			bin = (int) (img->pixels[r*img->width+c] * nbins);
			if (bin > (nbins - 1))
				bin = nbins - 1;
			histogram[bin]++;
		}
	}
}
	float computeHighThreshold(LPFImage magnitudes) {
	    int nbins = 64;
		double hist[nbins];
		int i;
		float cumSum = 0;

		for(i=0;i<nbins;i++)
			hist[i] = 0;
		histogramCanny(magnitudes, hist, nbins);
//		final Histogram hist = HistogramAnalyser.getHistogram(magnitudes, 64);


		for (i = 0; i < 64; i++) {
			if (cumSum > 0.7 * magnitudes->width * magnitudes->height) {
				return ((float)i / (float)64);
			}
			cumSum += hist[i];
		}

		return 1;
	}
	void addPixel(LPPIXELS *lpList, LPPIXEL lpixel){

		LPPIXELS lpTmp, lpCandidate;

		lpCandidate = yl_calloc("SWT9", 1, sizeof(PIXEL));
		if(lpCandidate == NULL ) {
			printf("thresholdingTracker: out of memory! x= %d y= %d\n ",lpixel->x, lpixel->y);return;
		}
		lpCandidate->p.x = lpixel->x;lpCandidate->p.y = lpixel->y;lpCandidate->next = NULL;

		if(*lpList == NULL){
			*lpList =  lpCandidate; return;
		}
		lpTmp = *lpList;
		while(lpTmp->next != NULL){
			lpTmp = lpTmp->next;
		}
		lpTmp->next = lpCandidate;
	}
	void addPixelXY(LPPIXELS *lpList, int x, int y){

		LPPIXELS lpTmp, lpCandidate;

		lpCandidate = yl_calloc("SWT9", 1, sizeof(PIXEL));
		if(lpCandidate == NULL ) {
			printf("thresholdingTracker: out of memory! x= %d y= %d\n ",x, y);return;
		}
		lpCandidate->p.x = x;lpCandidate->p.y = y;lpCandidate->next = NULL;

		if(*lpList == NULL){
			*lpList =  lpCandidate; return;
		}
		lpTmp = *lpList;
		while(lpTmp->next != NULL){
			lpTmp = lpTmp->next;
		}
		lpTmp->next = lpCandidate;
	}

	int isEmptyPixelList(LPPIXELS lpList){
		if(lpList == NULL) return 1;
		return 0;
	}
	int pollFirst(LPPIXELS *lpList, LPPIXEL pixelOut){
		LPPIXELS lpTmp= *lpList;
		if(lpTmp == NULL) return 0;
		pixelOut->x = lpTmp->p.x;
		pixelOut->y = lpTmp->p.y;
//		if(lpTmp->next != NULL)
		   *lpList = lpTmp->next;
		yl_free(lpTmp);
		return 1;

	}

	void thresholdingTracker(LPFImage magnitude, LPFImage output, float low, float high) {
		LPPIXELS lpCandidates = NULL;
		PIXEL current, newPixel;
		int ret;
		int y, x;
		zero(output);
//		printf("magnitude->height= %d, magnitude->width= %d\n", magnitude->height, magnitude->width);
//		fflush(stdout);

		for (y = 0; y < magnitude->height; y++) {
			for (x = 0; x < magnitude->width; x++) {
				if (magnitude->pixels[y*magnitude->width+x] >= high && output->pixels[y*magnitude->width+x] != 1) {

					newPixel.x = x;newPixel.y = y;
					addPixel(&lpCandidates, &newPixel);

//					candidates.add(new Pixel(x, y));
					while (!isEmptyPixelList(lpCandidates)) {
						 ret = pollFirst(&lpCandidates, &current);

                         if (ret == 0 || current.x < 0 || current.x > magnitude->width || current.y < 0 || current.y > magnitude->height)
//						if (current.x < 0 || current.x > magnitude.width || current.y < 0 || current.y > magnitude.height)
							continue;
						if (output->pixels[current.y*output->width+current.x] == 1)
							continue;
						if (magnitude->pixels[current.y*magnitude->width+current.x] < low)
							continue;

						output->pixels[current.y*output->width+current.x] = 1;
						newPixel.x = x - 1;newPixel.y = y - 1;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x;newPixel.y = y - 1;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x + 1;newPixel.y = y - 1;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x - 1;newPixel.y = y;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x + 1;newPixel.y = y;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x - 1;newPixel.y = y + 1;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x;newPixel.y = y + 1;addPixel(&lpCandidates, &newPixel);
						newPixel.x = x + 1;newPixel.y = y + 1;addPixel(&lpCandidates, &newPixel);
//						candidates.add(new Pixel(x - 1, y - 1));
//						candidates.add(new Pixel(x, y - 1));
//						candidates.add(new Pixel(x + 1, y - 1));
//						candidates.add(new Pixel(x - 1, y));
//						candidates.add(new Pixel(x + 1, y));
//						candidates.add(new Pixel(x - 1, y + 1));
//						candidates.add(new Pixel(x, y + 1));
//						candidates.add(new Pixel(x + 1, y + 1));
					}
				}
			}
		}
//	    printf("isEmptyPixelList(lpCandidates)= %d",isEmptyPixelList(lpCandidates));
//		freeCandidatesList(lpCandidates);
	}


	void computeSuppressed(LPFImage dxImage, LPFImage dyImage, LPFImage magsOut, LPFImage magnitudes) {
//		final float[][] diffx = dxImage.pixels;
//		final float[][] diffy = dyImage.pixels;
//		final int width = dxImage.width;
//		final int height = dxImage.height;
//		final float[][] mag = magsOut == null ? new float[height][width] : magsOut.pixels;

		float *diffx = dxImage->pixels;
		float *diffy = dyImage->pixels;
		int width = dxImage->width;
		int height = dxImage->height;
		float *mag = magsOut->pixels;
		int y, x, yx;
		int dx, dy;
		float a1, a2, b1, b2, A, B, point, val;

		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++) {
				yx = y*width+x;
				mag[yx] = (float) sqrt(diffx[yx] * diffx[yx] + diffy[yx] * diffy[yx]);
			}

//		FImage outimg = new FImage(width, height);
//		final float[][] output = outimg.pixels;
		newFImage(magnitudes,  width, height);
		float *output = magnitudes->pixels;

		for (y = 1; y < height - 1; y++) {
			for (x = 1; x < width - 1; x++) {
				yx = y*width+x;
				if (diffx[yx] > 0)
					dx = 1;
				else
					dx = -1;

				if (diffy[yx] > 0)
					dy = -1;
				else
					dy = 1;

				if (fabs(diffx[yx]) > fabs(diffy[yx])){
					a1 = mag[yx + dx];
					a2 = mag[(y - dy)*width+x + dx];
					b1 = mag[yx - dx];
					b2 = mag[(y + dy)*width+ x - dx];
					A = (fabs(diffx[yx]) - fabs(diffy[yx])) * a1 + fabs(diffy[yx]) * a2;
					B = (fabs(diffx[yx]) - fabs(diffy[yx])) * b1 + fabs(diffy[yx]) * b2;
					point = mag[yx] * fabs(diffx[yx]);
					if (point >= A && point > B) {
						val = fabs(diffx[yx]);
						output[yx] = val;
					}
					else {
						val = 0;
						output[yx] = val;
					}
				}
				else
				{
					a1 = mag[(y - dy)*width+x];
					a2 = mag[(y - dy)*width+x + dx];
					b1 = mag[(y + dy)*width+x];
					b2 = mag[(y + dy)*width+x - dx];
					A = (fabs(diffy[yx]) - fabs(diffx[yx])) * a1 + fabs(diffx[yx]) * a2;
					B = (fabs(diffy[yx]) - fabs(diffx[yx])) * b1 + fabs(diffx[yx]) * b2;
					point = mag[yx] * fabs(diffy[yx]);
					if (point >= A && point > B) {
						val = fabs(diffy[yx]);
						output[yx] = val;
					}
					else {
						val = 0;
						output[yx] = val;
					}
				}
			}
		}

//		return outimg;
	}

	void analyseImageSobel(LPFImage image) {
		FImage tmp;

		fImageClone(&tmp, image);
		if(sigma > 0)
		    imageProcessFGaussianConvolve(&tmp, sigma);
//		tmp = image;

		sobelDx.pixels = NULL;
		sobelDy.pixels = NULL;
		fImageClone(&sobelDx, &tmp);
		fImageClone(&sobelDy, &tmp);

		freeFImage(&tmp);

		FSobelX(&sobelDx);
		FSobelY(&sobelDy);
	}

	void imageProcessFGaussianConvolve(LPFImage image, float sigma){

		float truncate = DEFAULT_GAUSS_TRUNCATE;
		 makeKernel(sigma, truncate);

		 convolveHorizontal(image, kernel, ksize);
		 convolveVertical(image, kernel, ksize);

         yl_free(kernel);

	}

	void makeKernel(float sigma, float truncate) {
		int i;
		float sum, x;

		ksize = 1;
		if (sigma == 0){
			kernel = yl_calloc("SWT1", ksize, sizeof(float));
			*kernel = 1;
			return;
		}
	// The kernel is truncated at truncate sigmas from center.
		 ksize = (int) (2.0f * truncate * sigma + 1.0f);
		if (ksize % 2 == 0)
			ksize++; // size must be odd

		kernel = yl_calloc("SWT1", ksize, sizeof(float));

		// build kernel
		sum = 0.0;
		for ( i = 0; i < ksize; i++) {
			  x = i - ksize / 2;
			  kernel[i] = (float) exp(-x * x / (2.0 * sigma * sigma));
			  sum += kernel[i];
		}

		// normalise area to 1
		for (i = 0; i < ksize; i++) {
			kernel[i] /= sum;
		}

	}

	/**
	 * Convolve the image in the horizontal direction with the kernel. Edge
	 * effects are handled by duplicating the edge pixels.
	 *
	 * @param image
	 *            the image to convolve.
	 * @param kernel
	 *            the convolution kernel.
	 */
	void convolveHorizontal(LPFImage image,float *kernel, int ksize) {
		 int halfsize = ksize / 2;
		 int r, i, j, jj, l, c;
		 int buffL = image->width + ksize;
		 float buffer[buffL];
		 float sum;
		 int w =image->width;
//		final float buffer[] = new float[image.width + kernel.length];

		for (r = 0; r < image->height; r++) {
			for (i = 0; i < halfsize; i++)
				buffer[i] = image->pixels[w*r]; //[r][0];

			for (i = 0; i < w; i++)
				buffer[halfsize + i] = image->pixels[w*r+i]; //[r][i];
			for (i = 0; i < halfsize; i++)
				buffer[halfsize + w + i] = image->pixels[w*r+w - 1]; //[r][image.width - 1];

			l = buffL - ksize;
			for (i = 0; i < l; i++) {
				sum = 0.0;
				for (j = 0, jj = ksize - 1; j < ksize; j++, jj--)
					sum += buffer[i + j] * kernel[jj];

				buffer[i] = sum;
			}
			// end convolveBuffer(buffer, kernel);

			for (c = 0; c < w; c++)
				image->pixels[w*r+c] = buffer[c]; //[r][c]
		}
	}

	/**
	 * Convolve the image in the vertical direction with the kernel. Edge
	 * effects are handled by duplicating the edge pixels.
	 *
	 * @param image
	 *            the image to convolve.
	 * @param kernel
	 *            the convolution kernel.
	 */
	void convolveVertical(LPFImage image, float *kernel, int ksize) {
		 int halfsize = ksize / 2;
		 int r, i, j, jj, l, c;
		 int buffL = image->height + ksize;
		 float buffer[buffL];
		 float sum;
		 int w =image->width;
//		final float buffer[] = new float[image.height + kernel.length];


		for (c = 0; c < w; c++) {
			for (i = 0; i < halfsize; i++)
				buffer[i] = image->pixels[c]; //[0][c];
			for (i = 0; i < image->height; i++)
				buffer[halfsize + i] = image->pixels[w*i+c]; //[i][c];
			for (i = 0; i < halfsize; i++)
				buffer[halfsize + image->height + i] = image->pixels[w*(image->height - 1)+c];  //[image->height - 1][c];

			// convolveBuffer(buffer, kernel);
			l = buffL - ksize;
			for (i = 0; i < l; i++) {
				sum = 0.0;
				for (j = 0, jj = ksize - 1; j < ksize; j++, jj--)
					sum += buffer[i + j] * kernel[jj];
				buffer[i] = sum;
			}
			// end convolveBuffer(buffer, kernel);

			for (r = 0; r < image->height; r++)
				image->pixels[w*r+c] = buffer[r]; // [r][c]
		}
	}

	void  FSobelX(LPFImage image){
//	  FImage KERNEL_X;
//	  float kPixels[9] = {1,0,-1,2,0,-2,1,0,-1};
//	  KERNEL_X.height = 3;KERNEL_X.width = 3;KERNEL_X.pixels = kPixels;
	  int nrows = 3;
	  int i;
	  float row[] = {-0.7071067811865475,0.0,0.7071067811865475};
	  float col[] = {-0.40824829046386313,-0.8164965809277259,-0.408248290463863};
	  float factor = (float) sqrt(3.464101615137754);
	  for ( i = 0; i < nrows; i++) {
			row[i] *= factor;
			col[i] *= factor;
	  }

	  convolveHorizontal(image, row, nrows);
	  convolveVertical(image, col, nrows);


	}

	void  FSobelY(LPFImage image){
	  int nrows = 3;
	  int i;
	  float row[] = {-0.40824829046386313,-0.8164965809277261,-0.4082482904638631};
	  float col[] = {-0.7071067811865474,0.0,0.7071067811865476};
	  float factor = (float) sqrt(3.464101615137754);
	  for ( i = 0; i < nrows; i++) {
			row[i] *= factor;
			col[i] *= factor;
	  }
	  convolveHorizontal(image, row, nrows);
	  convolveVertical(image, col, nrows);

	}

	void freeFImage(LPFImage image){

	   if(image->pixels != NULL){
           yl_free(image->pixels);
           image->pixels = NULL;
	   }

	}

