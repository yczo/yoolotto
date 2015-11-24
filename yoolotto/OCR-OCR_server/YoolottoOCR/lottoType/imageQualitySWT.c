/*
 * imageQualitySWT.c
 *
 *  Created on: Oct 29, 2014
 *      Author: Wicek
 */

/*
package org.openimaj.image.processing.edges;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

import org.openimaj.citation.annotation.Reference;
import org.openimaj.citation.annotation.ReferenceType;
import org.openimaj.image.FImage;
import org.openimaj.image.ImageUtilities;
import org.openimaj.image.analysis.algorithm.EdgeDirectionCoherenceVector;
import org.openimaj.image.pixel.Pixel;
import org.openimaj.image.pixel.util.LineIterators;
import org.openimaj.image.processing.convolution.FSobel;
import org.openimaj.image.processor.SinglebandImageProcessor;
import org.openimaj.math.geometry.line.Line2d;
import org.openimaj.math.util.FloatArrayStatsUtils;
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include  "HashTable_Lib.h"
#include  "../headers/Image.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include  "../headers/Functions.h"
#include  "../headers/ocr.h"

/**
 * Implementation of the Stroke Width Transform.
 * <p>
 * The Stroke Width Transform detects strokes and their respective widths from
 * an image. This implementation contains a number of enhancements to improve
 * the quality of the detected strokes, based on ideas from <a
 * href="http://libccv.org/lib/ccv-swt/">LibCCV</a> implementation:
 * <ul>
 * <li>We search around the stroke in a small window for endpoints.</li>
 * <li>We search around the endpoint in a small window for matching gradients.</li>
 * <li>In addition to the stroke along the gradient, we also stroke at +/-45
 * degrees from this.</li>
 * </ul>
 *
 * @author Jonathon Hare (jsh2@ecs.soton.ac.uk)
 *
 */
//public class StrokeWidthTransform implements SinglebandImageProcessor<Float, FImage> {
	int edgeSearchRegionL = 5;
	int gradSearchRegionL = 9;
	int connect8L = 8;

	int edgeSearchRegion[5][5] = { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
	int gradSearchRegion[9][9] = {
			{ 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } };
	int connect8[8][8] = {
		{ -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } };




//	private final CannyEdgeDetector canny;
#define	POSITIVE_INFINITY 2000000000.0
#define	INT_MAX_VALUE  400000000
#define	CLOCKWISE 0
#define	COUNTER_CLOCKWISE 1
#define	COLLINEAR 2

#define	UPPER_RIGHT 0
#define	UPPER_LEFT 1
#define	 LOWER_LEFT 2
#define	 LOWER_RIGHT 3
#define	 SIGMA  0.00000000001
#define	 ANGLE_0DEG_IN_RADS 0
#define	 ANGLE_90DEG_IN_RADS  M_PI / 2
#define	 ANGLE_180DEG_IN_RADS  M_PI
#define	 ANGLE_270DEG_IN_RADS  3 * M_PI / 2
#define	 ANGLE_360DEG_IN_RADS  2 * M_PI

	int direction;
	int maxStrokeWidth = 70;
	float lowThresh;
	float highThresh;
	float sigma;

	int xIt;
	int yIt;
	float dxIt;
	float dyIt;
	int sxIt;
	int syIt;
	float errIt;
	PIXEL pIt;
    int MaxSizeRay;
    int totSizeRays, totSizeComponents, totPixelsListForComponents;
    int totNumbLetters, totNumbLines, maxNumbPairs;
    float  BYTE_TO_FLOAT_LUT[256];
	extern FImage sobelDx, sobelDy;


//	   void traceRay(LPFImage edges, LPFImage dx, LPFImage dy, int
//				detectDark, LPFImage output, float gradDirection,
//				int x, int y, LPPIXELSLIST *rays, int xx, int xy, int yx, int yy);

//	   void addPixelXY(LPPIXELS *lpList, int x, int y);
//	void saveLogFile(char *str1, char *str2, char *fName);
//    void setNewImageName(char *imgName, char *imgNInp, int loop);
	   void getColor24(LPIMGDATA img, int xn, int yn, LPBYTE R, LPBYTE G, LPBYTE B);
	   void convertToFImage(LPIMGDATA  image, LPFImage img);
	   void cannyProcessImageSobel(LPFImage image, float sigma);
	   BYTE getPixelCompress(LPBYTE ptr, int xn, int yn, int wPad, int height);
//	   void medianFilter(LPFImage output, LPPIXELSLIST *rays);
	   void freeFImage(LPFImage image);
	   int supercoverAsList(int x1, int y1, int x2, int y2, LPPIXEL *ray);
		void generateRays(LPFImage edges, LPFImage dx, LPFImage dy, int detectDark, LPFImage output, LPPIXELSLIST rays);
	   void traceRay(LPFImage edges, LPFImage dx, LPFImage dy, int
					detectDark, LPFImage output, float gradDirection,
					int x, int y, LPPIXELSLIST rays, int xx, int xy, int yx, int yy);

		   void medianFilter(LPFImage output, LPPIXELSLIST rays);
		   void freeRays(LPPIXELSLIST *rays);
			void findComponents(LPFImage image, PIXELSLIST *components);
//			LPPIXEL forestMakeSet(h_dictionary data, h_dictionary counts, LPPIXEL p, LPPIXELS *pixelsTmp, int w);
			LPPIXEL forestMakeSet(NODE *ptrNodes, int w, int h, LPPIXEL p, LPPIXEL pixelsTmp);
			void addPixelToList(LPPIXEL p,  LPPIXELS *pixelsTmp);
			int getPixelsSize(LPPIXELS ps);
			void freePixels(LPPIXELS *ps);
			void switchRays(LPPIXELSLIST lpRays, int i, int jMin);
//			LPPIXEL findPixelInForest(h_dictionary data, LPPIXEL x, int w);
			LPPIXEL findPixelInForest(NODE *ptrNodes, int w, int h, LPPIXEL x);
			void setPixelSWT(LPPIXEL xO, LPPIXEL xI);
			int pixelsSame(PIXEL x1, PIXEL x2);
			void  getSubsets(NODE *ptrNodes, int w, int h, LPPIXEL pixelsTmp, PIXELSLIST *components);
		    double getArrMean(float *arr, int size);
	        double getArrVariance(float *arr, int size);
	        double getArrPercentile(float *arr, int size, int perc);
	        void movePixelsValueToArr(float *arr,  LPPIXEL lpP, int size, LPFImage swt);
	        double calculateOrientatedBoundingBoxAspectRatio(LPPIXEL lpP, int size, LPFImage swt);
			void filterComponents(PIXELSLIST *components, LPFImage swt, LPFImage  img, LPLETTER letters);
			void Poly4_minimumBoundingRectangle(PIXEL *poly, int polySize, ROTATED_RECT *rotR, int printFlg);
		    LPPIXEL getLowestPoint2d(LPPIXEL Points, int size);
		    void copyPixels(LPPIXEL PointsOutp, LPPIXEL PointsInp, int size);
		    void sortPointsWithComparator(LPPIXEL points, int lo, int hi, LPPIXEL pLowest);
		    int getConvexHull(LPPIXEL poly, int polySize, LPPIXEL* polyOut, int *polySizeOut);
		 	void calculateRegularBoundingBox(LPPIXEL poly, int size, LETT_RECT* rectOut);
		 	void setTestLetters(LPLETTER lett);
			void extractLines(LPLETTER letters, LPLINELETTERS *lines, int *numLines);
 	        void printPair(LPLETTERPAIR p);
 	        void cpyLetters(LPLETTER lO, LPLETTER lI, int size);
 	  	    void computeBounds(LPLETTER letters, int size, LETT_RECT* rectOut);
 	  	    void extractWords(LPLINELETTERS oneLine, LPWORDCAND words, int *numWords);
 	       void printLetter(LPLETTER l);

	void setPixelSWT(LPPIXEL xO, LPPIXEL xI){
		xO->x = xI->x;xO->y = xI->y;
	}
	void setPixelXY(LPPIXEL xO, int x, int y){
		xO->x = x;xO->y = y;
	}

	void setPixelFimage(float *ptr,  int w, int h, int x, int y, int v){
		long nY;
		if(y< h) {
		   nY=y*w;
		   ptr[x+nY] = v;
		}
	}
	float getPixelFimage(float *ptr,  int w, int h, int x, int y){
		long nY;
		float v = 0;
		if(y< h) {
		   nY=y*w;
		   v = ptr[x+nY];
		}
		return v;
	}

	void convertToFImage(LPIMGDATA  image, LPFImage img) {
		int size, wPad;
		int x, y, v;
		float *ptr;
		if(image->width <= 10 || image->height <= 10) return;
		size = image->width*image->height;
		ptr = yl_calloc("SWT1", size, sizeof(float));
        if(ptr == NULL) {
	       	printf("ERROR memory problem: convertToFImage\n"); return;
	       }

		img->width = image->width;
		img->height = image->height;
		img->pixels = ptr;

		wPad = ((image->width+31)/32)*4;

		for(y=0;y<img->height;y++){
			for(x=0;x<img->width;x++){
				v = getPixelCompress(image->pixPtr, x, y, wPad, image->height);
				setPixelFimage(img->pixels,  img->width, img->height, x, y, v);
			}
		}
	}

	void setPixelFimageFromRGB(float *ptr,  int w, int h, int x, int y, BYTE R, BYTE G, BYTE B){
		float fpix = 0.299 * R + 0.587 * G + 0.114 * B;
		long nY;
//		if(y == 200 && x == 0)
//		     printf("R= %d, G= %d B=%d, fpix= %f\n",R,G,B, BYTE_TO_FLOAT_LUT[(int)fpix]);
		if(y< h) {
		   nY=y*w;
		   ptr[x+nY] = BYTE_TO_FLOAT_LUT[(int)fpix];
		}
	}

	void set_BYTE_TO_FLOAT_LUT(){
		int i;
  	   for (i = 0; i < 256; i++)
		  BYTE_TO_FLOAT_LUT[i] = i / (float)255;
	}

	void convertBMP24ToFImage(LPIMGDATA  image, LPFImage img) {
		int size, wPad;
		int x, y;
		float *ptr;
		BYTE R, G, B;

		if(image->width <= 10 || image->height <= 10) {
			printf("ERROR image->width= %d, image->height= %d\n",image->width, image->height); return;
		}
		size = image->width*image->height;
		ptr = yl_calloc("SWT1", size, sizeof(float));
        if(ptr == NULL) {
	       	printf("ERROR memory problem: convertToFImage\n"); return;
	       }

		img->width = image->width;
		img->height = image->height;
		img->pixels = ptr;

		wPad = ((image->width+31)/32)*4;
		set_BYTE_TO_FLOAT_LUT();

		for(y=0;y<img->height;y++){
			for(x=0;x<img->width;x++){
				getColor24(image, x, y, &R, &G, &B);
//				if(y == 200 && x == 0)
//				     printf("R= %d, G= %d B=%d\n",R,G,B);

				setPixelFimageFromRGB(img->pixels,  img->width, img->height, x, y, R, G, B);
//				v = getPixelCompress(image->pixPtr, x, y, wPad, image->height);
//				setPixelFimage(img->pixels,  img->width, img->height, x, y, v);
			}
		}
	}

	void fImageClone(LPFImage clone, LPFImage img){
		int size;
		int i;
		float *ptr;

		if(img->width <= 10 || img->height <= 10) return;

		size = img->width*img->height;
		ptr = yl_calloc("SWT2", size, sizeof(float));
        if(ptr == NULL) {
	       	printf("ERROR memory problem: fImageClone\n"); return;
	       }

		clone->width = img->width;
		clone->height = img->height;
		clone->pixels = ptr;

		for(i=0;i<size;i++)
		    ptr[i] = img->pixels[i];
	}
	void newFImage(LPFImage imageOut, int width, int height) {
		int size;
//		int i;
		float *ptr;

		if(width <= 0 || height <= 0) return;

		size = width*height;
		ptr = yl_calloc("SWT2", size, sizeof(float));
        if(ptr == NULL) {
	       	printf("ERROR memory problem: newFImage\n"); return;
        }
		imageOut->width = width;
		imageOut->height = height;
		imageOut->pixels = ptr;
	}

	void cpyFImage(LPFImage imgOut, LPFImage img){
		int size;
		int i;
//		float *ptr;

		if(img->width <= 0 || img->height <= 0) return;

		size = img->width*img->height;
		for(i=0;i<size;i++)
			imgOut->pixels[i] = img->pixels[i];
	}

  float maxFImage(LPFImage img){
		int r, c;
		float max = -10000000;;

		for (r = 0; r < img->height; r++)
			for (c = 0; c < img->width; c++)
				if (max < img->pixels[r*img->width+c])
					max = img->pixels[r*img->width+c];
		return max;
	}

  float minFImage(LPFImage img){
		int r, c;
		float min = 10000000;

		for (r = 0; r < img->height; r++)
			for (c = 0; c < img->width; c++)
				if (min > img->pixels[r*img->width+c])
					min = img->pixels[r*img->width+c];

		return min;
	}
  void zero(LPFImage img){
	 int r, c;
		for (r = 0; r < img->height; r++){
			for (c = 0; c < img->width; c++){
				img->pixels[r*img->width+c] = 0;
			}
		}
	}
	void fill(LPFImage img, float color)	{
		int r, c;
		for (r = 0; r < img->height; r++)
			for (c = 0; c < img->width; c++)
				img->pixels[r*img->width+c] = color;

	}

	void normaliseFImage(LPFImage img){
		float min = minFImage(img);
		float max = maxFImage(img);
		int r, c;

		for (r = 0; r < img->height; r++)	{
			for (c = 0; c < img->width; c++)		{
				img->pixels[r*img->width+c] = (img->pixels[r*img->width+c] - min) / (max - min);
			}
		}
	}

	/**
	 * Construct the SWT with the given sigma for smoothing in the Canny phase
	 * The Canny thresholds are chosen automatically.
	 *
	 * @param direction
	 *            direction of the SWT; true for dark on light, false for light
	 *            on dark.
	 * @param sigma
	 *            the amount of initial blurring
	 */
	void setParams_StrokeWidthTransform(int directionInp, float sigmaInp) {
		direction = directionInp;
		sigma = sigmaInp;
//		canny = new CannyEdgeDetector(sigmaInp);
	}

	/**
	 * Construct with all Canny parameters set manually.
	 *
	 * @param direction
	 *            direction of the SWT; true for dark on light, false for light
	 *            on dark.
	 *
	 * @param lowThresh
	 *            lower hysteresis threshold.
	 * @param highThresh
	 *            upper hysteresis threshold.
	 * @param sigma
	 *            the amount of initial blurring.
	 */
	void setParams_StrokeWidthTransform2(int directionInp, float lowThreshInp, float highThreshInp, float sigmaInp) {
		direction = directionInp;
		lowThresh = lowThreshInp;
		highThresh =highThreshInp;
		sigma = sigmaInp;

//		canny = new CannyEdgeDetector(lowThresh, highThresh, sigmaInp);
	}

	int processImageSWT(LPIMGDATA  image) {
		int retValue = 0;
		FImage swt, img;
		FImage edges;
//		int x, y;
//		char buff[2000];


		convertToFImage(image, &img);
//		convertBMP24ToFImage(image, &img);
/*
		{
//		  char buff[2000];
		  int x, y;
			for(y=200;y<250;y++){
//				buff[0] = 0;
				for(x=0;x<20;x++){
					printf("%6.4f, ",img.pixels[y*img.width+x]);
				}
				printf("\n");
//     	        saveLogFile(buff, "\n", "log_SWT.txt");
			}
		}
*/
//		printf("convertBMP24ToFImage, img.width= %d, img.height= %d\n", img.width, img.height);
//		fflush(stdout);


		fImageClone(&edges, &img);
		fImageClone(&swt, &img);


//		freeFImage(&swt);
//		freeFImage(&edges);
//		freeFImage(&img);

//		{int x, y;
//	    for(x=1;x<10000;x++) {
//	  	  for(y=1;y<100000;y++){
//	  		  double a;a = sqrt((double)x);
//	  	  }
//	    }
//	    return -1;
//		}

		cannyProcessImageSobel(&edges, sigma);



//		printf("cannyProcessImageSobel\n");
//		fflush(stdout);


		fill(&swt, POSITIVE_INFINITY);

//		printf("fill\n");
//		fflush(stdout);

		{
		   PIXELSLIST *rays;
		   rays = (LPPIXELSLIST)yl_calloc("SWT2", edges.width*edges.height, sizeof(PIXELSLIST));
	        if(rays == NULL) {
		       	printf("ERROR memory problem: rays\n"); return retValue;
	        }

		    totSizeRays = 0;
			generateRays(&edges, &sobelDx, &sobelDy, direction, &swt, rays);

//			printf("0 medianFilter; totSizeRays= %d\n", totSizeRays);
//			fflush(stdout);

        if(totSizeRays > 0)
		    medianFilter(&swt, rays);

//		printf("1 medianFilter\n");
//		fflush(stdout);

		freeRays(&rays);
		freeFImage(&edges);
		freeFImage(&sobelDx);
		freeFImage(&sobelDy);

//					printf("medianFilter\n");
//					fflush(stdout);


		{
		   PIXELSLIST *components;
		   components = (LPPIXELSLIST)yl_calloc("SWT2", swt.width*swt.height, sizeof(PIXELSLIST));
	        if(components == NULL) {
		       	printf("ERROR memory problem: components\n"); return retValue;
	        }

		    totSizeComponents = 0;
	        findComponents(&swt, components);

//	       printf("totSizeComponents= %d\n",totSizeComponents);
//		   fflush(stdout);
/*
	       for(i=0;i<totSizeComponents;i++){
	    	   lpP = components[i].lpPixel;
	    	   printf("component= %d, size= %d\n",i, components[i].size);
	    	   for(k=0;k<components[i].size;k++){
	    	      printf("pixel[ %d ]= (%d,%d,%f),",k, lpP[k].x, lpP[k].y, swt.pixels[lpP[k].y*swt.width+lpP[k].x]);
	    	   }
	    	   printf("\n");
	    	   if(i > 5) break;
	       }
*/





/*
				{int x, y, i;
				freeFImage(&swt);
				freeFImage(&img);
			       for(i=0;i<totSizeComponents;i++){
			    	   if(components[i].lpPixel != NULL)
			    		   yl_free(components[i].lpPixel);
			       }
		           	yl_free(components);
			    for(x=1;x<10000;x++) {
			  	  for(y=1;y<100000;y++){
			  		  double a;a = sqrt((double)x);
			  	  }
			    }
			    return -1;
				}
*/


	       {LPLETTER letters;
	       LPLINELETTERS lines;
	       int numLines, i;
	           letters = (LPLETTER)yl_calloc("SWT2", totSizeComponents, sizeof(LETTER));
		       if(letters == NULL) {
			       	printf("ERROR memory problem: letters\n"); return retValue;
		       }

	           totNumbLetters = 0;
	           filterComponents(components, &swt, &img, letters);

		       for(i=0;i<totSizeComponents;i++){
		    	   if(components[i].lpPixel != NULL)
		    		   yl_free(components[i].lpPixel);
		       }
	           	yl_free(components);

//	           printf(" totNumbLetters= %d\n",totNumbLetters);
//           	fflush(stdout);
           	totNumbLines = 0;

           	extractLines(letters, &lines, &numLines);

//     	    printf("numLines= %d\n",numLines);
//           	fflush(stdout);


     	    {int line;
     	     int count = 0;
     	     float ratio;
     	     int maxLett = 0;
	          for(line=0;line<numLines;line++){
//	        	  printf("line= %d, letters= %d\n",line, lines[line].size);
	        	  if(maxLett< lines[line].size) maxLett = lines[line].size;
	        	  count +=lines [line].size;
	          }
	          ratio = 100;
	          if(count > 0) {
	        	  ratio = (float)totNumbLetters/(float)count;
//        	     printf(" count letters= %d, tot/cc= %5.2f\n",count, ratio);
	          }
//	          else
//	          printf(" count letters= %d, tot/cc= NAN\n",count);
	          { float thRatio1 = 2.5, thRatio2 = 5;
	            int thLines1 = 5, thLines2 = 10, thMaxL = 4;
	            int printLog = 0;
//	            char buff[2000];
	              if(ratio > thRatio2) printLog = 1;
	              if(ratio > thRatio1 && numLines < thLines1) printLog = 1;
	              if(ratio > thRatio1 && numLines < thLines2 && maxLett < thMaxL) printLog = 1;
	              if(printLog)
	            	  retValue = -1;

//	              printLog = 1;
//	              if(printLog){
//            	     sprintf(buff,"image= %s, totNumbLetters= %d, count letters= %d, ratio= %5.2f, numLines= %d, maxLett= %d\n",image->szIMGName, totNumbLetters, count, ratio, numLines, maxLett);
//            	     saveLogFile("", buff, "log_SWT.txt");
//	              }
	          }

     	    }


     	   // 	    saveLogFile("", "\n\n", "log_SWT.txt");

     	   // 	    sprintf(buff,"image= %s, sigma= %5.2f, direction= %d-------------------------------------------\n",image->szIMGName, sigma, direction);
     	   // 	    saveLogFile("", buff, "log_SWT.txt");


     	     {int line;
   	             for(line=0;line<totNumbLetters;line++){
  	                if(letters[line].lpPixel !=NULL)
  	    	          yl_free(letters[line].lpPixel);
   	             }

      	        for(line=0;line<numLines;line++){
     	            if(lines[line].lpL !=NULL)
     	    	       yl_free(lines[line].lpL);
      	        }
      	     }
     	     yl_free(lines);
     	     yl_free(letters);


	       }

		}

	   }

		freeFImage(&swt);
		freeFImage(&img);
		return retValue;
	}
	void copyRect(LETT_RECT *rO, LETT_RECT *rI){
		rO->x = rI->x; rO->y = rI->y;
		rO->width = rI->width;rO->height = rI->height;
	}
	void copyPair(LETT_RECT *rO, LETT_RECT *rI){
		rO->x = rI->x; rO->y = rI->y;
		rO->width = rI->width;rO->height = rI->height;
	}

	void cpyLetter(LPLETTER lO, LPLETTER lI){
		lO->lpPixel = yl_calloc("SWT55", lI->size, sizeof(PIXEL));
	    if(lO->lpPixel == NULL) {
	    	printf("cpyLetter; yl_calloc: out of memory!\n ");return;
	    }
	    lO->size = lI->size;
	    copyPixels(lO->lpPixel, lI->lpPixel, lI->size);
	    lO->medianStrokeWidth = lI->medianStrokeWidth;
	    copyRect(&lO->regularBoundingBox, &lI->regularBoundingBox);

	    setPixelSWT(&lO->centroid, &lI->centroid);
	    lO->averageBrightness = lI->averageBrightness;
	}

	void cpyPair(LETTERPAIR *lPairO, LETTERPAIR *lPairI){
		cpyLetter(&lPairO->lett1, &lPairI->lett1);
		cpyLetter(&lPairO->lett2, &lPairI->lett2);
	}
	void cpyLetters(LPLETTER lO, LPLETTER lI, int size){
	    int i;
	    for(i=0;i<size;i++)
	       cpyLetter(&lO[i], &lI[i]);
	}

	int sameAllPixels(PIXEL *p1, PIXEL *p2, int size){
		int i;
		for(i=0;i<size;i++)
			if(!pixelsSame(p1[i], p2[i] )) return 0;
		return 1;
	}
	int sameRect(LETT_RECT *r1,LETT_RECT *r2){


			if(r1->x != r2->x || r1->y != r2->y ||
			   r1->width != r2->width || r1->height != r2->height) return 0;
		return 1;
	}

	int sameletter(LPLETTER l1, LPLETTER l2){
		if(l1->medianStrokeWidth != l2->medianStrokeWidth) return 0;
		if(l1->averageBrightness != l2->averageBrightness) return 0;
		if(l1->size != l2->size) return 0;
		if(!sameRect(&l1->regularBoundingBox, &l2->regularBoundingBox)) return 0;
		if(!sameAllPixels(l1->lpPixel, l2->lpPixel, l1->size)) return 0;

		return 1;
	}

	int samePair(LETTERPAIR *lPair, LPLETTER l1, LPLETTER l2){
		if(!sameletter(&lPair->lett1,l1)) return 0;
		if(!sameletter(&lPair->lett2,l2)) return 0;
		return 1;
	}
	int noSamePair(LETTERPAIR *lPairs, int totLetterPairs, LPLETTER l1, LPLETTER l2){
		int i;
		for(i=0;i<totLetterPairs;i++){
			if(samePair(&lPairs[i], l1, l2)) return 0;
		}
		return 1;
	}

	void addNextLettersPair(LETTERPAIR *lPairs, int *totLetterPairs, LPLETTER l1, LPLETTER l2){
		if(noSamePair(lPairs, *totLetterPairs, l1, l2)){
	   	   cpyLetter(&lPairs[*totLetterPairs].lett1, l1);
		   cpyLetter(&lPairs[*totLetterPairs].lett2, l2);
		   (*totLetterPairs)++;
		}
	}
	void createLetterPairs(LPLETTER letters, LETTERPAIR *lPairs, int *totLetterPairs) {
		LETTER *l1, *l2;
		double distance;
		int numLetters = totNumbLetters;
		int i, j, oy;

		for (j = 0; j < numLetters; j++) {
			 l1 = &letters[j];
			for (i = j + 1; i < numLetters; i++) {
				 l2 = &letters[i];
				// similar stroke width (median ratio < 2)
				if (max(l1->medianStrokeWidth, l2->medianStrokeWidth)
						/ min(l1->medianStrokeWidth, l2->medianStrokeWidth) > 2.0)
					continue;
				// similar height
				if (max(l1->regularBoundingBox.height, l2->regularBoundingBox.height)
						/ min(l1->regularBoundingBox.height, l2->regularBoundingBox.height) > 2.0)
					continue;
				// similar color
				if (fabs(l1->averageBrightness - l2->averageBrightness) > 0.12)
					continue;

				// small distance between
				distance = l1->centroid.x - l2->centroid.x;
				if (fabs(distance) > 3 * max(l1->regularBoundingBox.width, l2->regularBoundingBox.width))
					continue;
				// approximately level
				oy = (int) (min(l1->regularBoundingBox.y + l1->regularBoundingBox.height,
						l2->regularBoundingBox.y + l2->regularBoundingBox.height) - max(l1->regularBoundingBox.y,
						l2->regularBoundingBox.y));
				if (oy * 1.3 < min(l1->regularBoundingBox.height, l2->regularBoundingBox.height))
					continue;
				// tests passed... merge
                if(*totLetterPairs<maxNumbPairs-1)
				  addNextLettersPair(lPairs, totLetterPairs, l1, l2);
//				pairs.add(new Pair<LetterCandidate>(l1, l2));
			}
		}
	}
//	List<LineCandidate>

	void computeDelta(LPLETTER l1, LPLETTER l2, PIXEL *pOut) {
		LETT_RECT frect = l1->regularBoundingBox;
		LETT_RECT srect = l2->regularBoundingBox;
		int dx = (int) (frect.x - srect.x + (frect.width - srect.width) / 2);
		int dy = (int) (frect.y - srect.y + (frect.height - srect.height) / 2);
		setPixelXY(pOut, dx, dy);
	}


	int comparatorComparePairs(LPLETTERPAIR vi, LPLETTERPAIR vj){
		PIXEL pair1d;
		PIXEL pair2d;
		int tn, td;
		computeDelta(&vi->lett1, &vi->lett2, &pair1d);
		computeDelta(&vj->lett1, &vj->lett2, &pair2d);

		if (sameletter(&vi->lett1, &vj->lett1) || sameletter(&vi->lett2, &vj->lett2)){
			tn = pair1d.y * pair2d.x - pair1d.x * pair2d.y;
			td = pair1d.x * pair2d.x + pair1d.y * pair2d.y;
			// share the same end, opposite direction
			if (tn * 7 < -td * 4 && tn * 7 > td * 4)
				return 0;
		} else if (sameletter(&vi->lett1, &vj->lett2)
				|| sameletter(&vi->lett2, &vj->lett1)) 		{
			          tn = pair1d.y * pair2d.x - pair1d.x * pair2d.y;
			          td = pair1d.x * pair2d.x + pair1d.y * pair2d.y;
			// share the other end, same direction
			if (tn * 7 < td * 4 && tn * 7 > -td * 4)
				return 0;
		}
		return 1;
	}


int findPairInForest(NODEPAIR *ptrNodes, int i) {
		 if(i == ptrNodes[i].parent)
				 return i;
	 return findPairInForest(ptrNodes,  ptrNodes[i].parent);
	}


	void forestUnionPairs(NODEPAIR *ptrNodes,  LPLETTERPAIR x, int i, LPLETTERPAIR y, int j){
//		LPLETTERPAIR xRoot, yRoot;
		int   xiRoot = findPairInForest(ptrNodes, i);
		int   yiRoot = findPairInForest(ptrNodes, j);
		NODEPAIR *xNode, *yNode;
		if (xiRoot == yiRoot)
			 return;
		 xNode = &ptrNodes[xiRoot];
		 yNode = &ptrNodes[yiRoot];

		 if (xNode->rank < yNode->rank) {
			xNode->parent = yiRoot;
//			counts.adjustValue(yRoot, counts.remove(xRoot));
				return;
			} else if (xNode->rank > yNode->rank) {
				yNode->parent = xiRoot;
//				counts.adjustValue(xRoot, counts.remove(yRoot));
				return;
			} else {
				yNode->parent = xiRoot;
				xNode->rank++;
//				counts.adjustValue(xRoot, counts.remove(yRoot));
//			   return xRoot;
			}

	}
	void addAllPairNodes(NODEPAIR *ptrNodes, int size){
		int i;
		for(i=0;i<size;i++){
			ptrNodes[i].parent = i;
			ptrNodes[i].rank = 0;
		}
	}

	 float maxValue(float *arr, int size) {
		int i;
		float max;
		if (size < 0)
			return 0;
		max = arr[0];
		for (i = 1; i < size; i++) {
			if (arr[i] > max) {
				max = arr[i];
			}
		}
		return max;
	}
	 float minValue(float *arr, int size) {
		int i;
		float min;
		if (size < 0)
			return 0;
		min = arr[0];
		for (i = 1; i < size; i++) {
			if (arr[i] < min) {
				min = arr[i];
			}
		}
		return min;
	}
	void makeHistogram(float *data, int size, int numBins, float min, float max, int *histData) {
		int c, h;
		float d;

		// Calculate histogram
		for (c = 0; c < size; c++) {
			d = (data[c] - min) / (max - min);
			h = (int) (d * (numBins - 1));
			histData[h]++;
		}
	}
	float computeThresholdFromHistogram(int *histData, int numBins, int total) {
		float sum = 0;
		int t;
		float sumB = 0;
		int wB = 0;
		int wF = 0;
		float varMax = 0;
		float threshold = 0;
		float varBetween;
		float mB, mF;

		for (t = 0; t < numBins; t++)
			sum += t * histData[t];

		for ( t = 0; t < numBins; t++) {
			wB += histData[t]; // Weight Background
			if (wB == 0)
				continue;

			wF = total - wB; // Weight Foreground
			if (wF == 0)
				break;

			sumB += (t * histData[t]);

			mB = sumB / wB; // Mean Background
			mF = (sum - sumB) / wF; // Mean Foreground

			// Calculate Between Class Variance
			varBetween = (float) wB * (float) wF * (mB - mF) * (mB - mF);

			// Check if new maximum found
			if (varBetween > varMax) {
				varMax = varBetween;
				threshold = t;
			}
		}
		return threshold / (numBins - 1);
	}
	float calculateThreshold(float *data, int size, int numBins) {
		float min = minValue(data, size);
		float max = maxValue(data, size);
		float t;
		int histData[numBins];

		makeHistogram(data, size, numBins, min, max, histData);

		t = computeThresholdFromHistogram(histData, numBins, size);
//		printf("min= %f, max= %f, t= %f\n",min, max, t);

		return (t+ min);
	}
	void calculateSpacing(LPLETTER letters, int size, float *spacings){
		int i;
//		for (i = 0; i < size; i++) {
//		    printLetter(&letters[i]);printf("\n");
//		}
		for (i = 0; i < size-1; i++) {

			spacings[i] = letters[i+1].regularBoundingBox.x-(letters[i].regularBoundingBox.x+letters[i].regularBoundingBox.width);
//			printf("spacings[ %d ]= %f\n",i, spacings[i]);
		}

	}

	void extractWords(LPLINELETTERS oneLine, LPWORDCAND words, int *numWords) {
//		final List<WordCandidate> words = new ArrayList<WordCandidate>();
		LPLETTER letters = oneLine->lpL;
		int size = oneLine->size;
		int spacingslength= size-1;
		// Collect inter-word spacings
		float spacings[spacingslength];
		float threshold;
		LETTER wLetters[size];
		int wordLetters = 0;
		int wordNum = 0;
		int i;

		calculateSpacing(oneLine->lpL, size, spacings);
		// use Otsu's method to find the optimal threshold
		threshold = calculateThreshold(spacings, spacingslength,10);
//		printf("threshold= %f\n",threshold);

//		words.add(word);
		for (i = 0; i < spacingslength; i++) {
  		    cpyLetter(&wLetters[wordLetters++], &letters[i]);
//			word.letters.add(line.letters.get(i));
			if (spacings[i] < threshold) {
			    cpyLetter(&wLetters[wordLetters++], &letters[i+1]);
//				word.letters.add(line.letters.get(i + 1));
			} else {
				words[wordNum].lpL = (LPLETTER)yl_calloc("SWT2", wordLetters, sizeof(LETTER));
		        if(words[wordNum].lpL == NULL) {
				       	printf("ERROR memory problem: extractWords\n"); return;
			       }
		        cpyLetters(words[wordNum].lpL, wLetters, wordLetters);
				words[wordNum].size = wordLetters;
				wordNum++;
				wordLetters = 0;
				cpyLetter(&wLetters[wordLetters++], &letters[i+1]);
			}
		}
		if(wordLetters > 0) {
		   words[wordNum].lpL = (LPLETTER)yl_calloc("SWT2", wordLetters, sizeof(LETTER));
           if(words[wordNum].lpL == NULL) {
		       	printf("ERROR memory problem: extractWords\n"); return;
	       }
           cpyLetters(words[wordNum].lpL, wLetters, wordLetters);
		   words[wordNum].size = wordLetters;
		   wordNum++;
		   wordLetters = 0;
		}
		*numWords = wordNum;
		for (i = 0; i < wordNum; i++) {

 	  	   computeBounds(words[i].lpL, words[i].size, &words[i].regularBoundingBox);

		}
	}


    void partitionSubsetsWithComparator(LPLETTERPAIR lPairs, int totLetterPairs, NODEPAIR *ptrNodes){
//    	LPLETTERPAIR pairsTmp = NULL;
        int i, j;
        LPLETTERPAIR vi, vj;

//        pairsTmp = (LETTERPAIR)yl_calloc("SWT_NODELIST2", totLetterPairs, sizeof(LETTERPAIR));

//        setNodesInit(ptrNodes, img->width, img->height); ?????
         addAllPairNodes(ptrNodes, totLetterPairs);

//  printf("addAllPairNodes\n");
//   	  for(i=0;i<totLetterPairs;i++){
//   		  printf("NODE i=%d, (%d, %d)\n",i,ptrNodes[i].parent,ptrNodes[i].rank);
//   	  }
//   	  printf("\n");

		for (i = 0; i < totLetterPairs; i++) {
			 vi = &lPairs[i];
			for (j = i + 1; j < totLetterPairs; j++) {
				vj = &lPairs[j];
				if (comparatorComparePairs(vi, vj) == 0)
					forestUnionPairs(ptrNodes, vi, i, vj, j);
//				  forestUnion(ptrNodes, pairsTmp, vi, vj);
			}
		}

	}

    void printPixel(PIXEL *p){
    	printf("(%d,%d), ",p->x,p->y);
    }
      void printRECT(LETT_RECT *r){
    	  printf("[%10.2f,%10.2f,%10.2f,%10.2f], ",r->x,r->y, r->width, r->height);
      }
      void printLetter(LPLETTER l){
    	  printRECT(&l->regularBoundingBox);printf("averageBrightness= %f, medianStrokeWidth= %f. ",l->averageBrightness,l->medianStrokeWidth);
    	  printPixel(&l->centroid);

      }
     void printLetterOnlyR(LPLETTER l){
    	  printRECT(&l->regularBoundingBox);

      }

      void printPair(LPLETTERPAIR p){
    	  printf("[  ");printLetterOnlyR(&p->lett1);printLetterOnlyR(&p->lett2);printf(" ]");
      }

      void createLinesP(LINECHAIN *linesTmp, LPLETTERPAIR lPairs, LPNODEPAIR ptrNodes, int size, int *numLines){
          int i, j;
          int link[size], idx;
          LPLETTERPAIR line;
          *numLines = 0;
    	  for(i=0;i<size;i++){
    		  if(ptrNodes[i].parent<0) continue;
    		  idx = 0;link[idx++] = i;
    		  for(j=i+1;j<size;j++){
    			  if(ptrNodes[i].parent == ptrNodes[j].parent){
    				  link[idx++] = j;ptrNodes[j].parent = -1;
    			  }
    		  }
    		  if(idx>0){
    			  line = (LPLETTERPAIR)yl_calloc("SWT2", idx, sizeof(LETTERPAIR));
    		      if(line == NULL) {
    		        	printf("ERROR memory problem: line\n"); return;
    		        }
    		      linesTmp[*numLines].p = line;
    		      linesTmp[*numLines].size = idx;
        		  for(j=0;j<idx;j++){
        			 cpyPair(&line[j], &lPairs[link[j]]);
        		  }
        		  (*numLines)++;
    		  }
    	  }
      }
      void copyLettersToLine(LPLINELETTERS lines, LINECHAIN *linesTmp, int numLines){
          int i, j, line, size;
          LPLETTER letters;
          for(line=0;line<numLines;line++){
        	  j = 0;
        	  size = linesTmp[line].size*2;
        	  letters = (LPLETTER)yl_calloc("SWT2", size, sizeof(LETTER));
    	      if(letters == NULL) {
    	        	printf("ERROR memory problem: letters\n"); return;
   	          }
        	  for(i=0;i<linesTmp[line].size;i++){
        		  cpyLetter(&letters[j++], &linesTmp[line].p[i].lett1);
        		  cpyLetter(&letters[j++], &linesTmp[line].p[i].lett2);
              }

        	  lines[line].lpL = letters;
        	  lines[line].size = size;
          }

      }
     //-------------------------------------------
	  void extractLines(LPLETTER letters, LPLINELETTERS *lines, int *numLines) {
		  LPLETTERPAIR lPairs;
		  int totLetterPairs = 0;
          NODEPAIR *ptrNodes;
          LPLINECHAIN linesTmp;
//          int i, j;
/*
  printf("totNumbLetters= %d\n",totNumbLetters);
	  for(i=0;i<totNumbLetters;i++){
			  printLetter(&letters[i]);
			  printf("\n");
	  }
	  printf("\n");
*/
          maxNumbPairs = 2*totNumbLetters;
		  lPairs = (LPLETTERPAIR)yl_calloc("SWT2", maxNumbPairs, sizeof(LETTERPAIR));
	      if(lPairs == NULL) {
	        	printf("ERROR memory problem: lPairs\n"); return;
	        }


		  createLetterPairs(letters, lPairs, &totLetterPairs);

//   	    printf("createLetterPairs;totNumbLetters= %d, totLetterPairs= %d\n",totNumbLetters, totLetterPairs);
//       	fflush(stdout);

/*
	  for(i=0;i<totLetterPairs;i++){
		      printPair(&lPairs[i]);
		      printf("\n");
	  }
	  printf("\n");
*/
          ptrNodes = (NODEPAIR *)yl_calloc("SWT_NODELIST2", totLetterPairs, sizeof(NODEPAIR));
	      if(ptrNodes == NULL) {
	        	printf("ERROR memory problem: ptrNodes\n"); return;
	        }

		  partitionSubsetsWithComparator(lPairs, totLetterPairs, ptrNodes);

//	   	    printf("partitionSubsetsWithComparator\n");
//	       	fflush(stdout);

	      //totPixelsListForComponents = 0;
//	  for(i=0;i<totLetterPairs;i++){
//		  printf("NODE i=%d, (%d, %d)\n",i,ptrNodes[i].parent,ptrNodes[i].rank);
//	  }
//	  printf("\n");

		  linesTmp = (LPLINECHAIN)yl_calloc("SWT_NODELIST2", totLetterPairs, sizeof(LINECHAIN));
          if(linesTmp == NULL) {
        	printf("ERROR memory problem: linesTmp\n"); return;
           }
          *lines = (LPLINELETTERS)yl_calloc("SWT_NODELIST2", totLetterPairs, sizeof(LINELETTERS));
          if(*lines == NULL) {
        	printf("ERROR memory problem: linesTmp\n"); return;
           }

	   createLinesP(linesTmp, lPairs, ptrNodes, totLetterPairs, numLines);

// 	    printf("createLinesP\n");
//     	fflush(stdout);


/*
	   {int i, j;
 	      for(i=0;i<*numLines;i++){
  	 	     printf("linesTmp;line= %d ----------\n", i);
 	 	      for(j=0;j<linesTmp[i].size;j++){
		       printPair(&linesTmp[i].p[j]);
 	         }
 	 	     printf("\n");
 	      }
	   }
*/
	   copyLettersToLine(*lines, linesTmp, *numLines);
// 	    printf("copyLettersToLine\n");
//     	fflush(stdout);

/*
	   {int i, j;
	   LPLINELETTERS linesT;
	       linesT =*lines;

 	      for(i=0;i<*numLines;i++){
   	 	     printf("lines;line= %d ----------\n", i);

 	 	      for(j=0;j<linesT[i].size;j++){
 	 	    	printLetter(&linesT[i].lpL[j]);
 	         }
 	 	     printf("\n");
 	      }
	   }
*/
	   {int i;
 	      for(i=0;i<*numLines;i++){
	          if(linesTmp[i].p !=NULL)
	    	     yl_free(linesTmp[i].p);
 	      }
 	   }
//	    printf("yl_free(linesTmp[i].p)\n");
//    	fflush(stdout);


/*
	   {
		   LPLETTERPAIR oneL;
	   printf("numLines= %d\n",*numLines);
	 	  for(i=0;i<*numLines;i++){
	   printf("line= %d\n",i);
	      oneL = linesTmp[i].p;
	 		 for(j=0;j<linesTmp[i].size;j++){
	 			printPair(&oneL[j]);
	 		 }
	 		printf("\n");
	 	  }
	 	  printf("\n");
	   }
*/


       yl_free(lPairs);
       yl_free(ptrNodes);
       yl_free(linesTmp);
	}

     double calculateMoment(LPPIXEL cc, int size, int p, int q, double xc, double yc) {
    	 double mpq = 0;
    	 int i;
		if (p == 0 && q == 0)
			return size;

		for(i=0;i<size;i++){
			mpq += pow(cc[i].x - xc, p) * pow(cc[i].y - yc, q);
		}
		return mpq;
	}

	 void calculateCentroid(LPPIXEL cc, int size, double* centroid) {
		double m00 =  calculateMoment(cc, size, 0, 0, 0, 0);
		centroid[0] = calculateMoment(cc, size, 1, 0, 0, 0) / m00;
		centroid[1] = calculateMoment(cc, size, 0, 1, 0, 0) / m00;

	}

	void calculateCentroidPixel(LPPIXEL cc, int size, PIXEL *centroid) {
		double centroidD[2];
		calculateCentroid(cc, size, centroidD);
		centroid->x = (int)round(centroidD[0]);centroid->y = (int)round(centroidD[1]);
	}

	void addNewLetter(LPPIXEL cc, int size, LPLETTER letters, float  median, LPFImage img){
		LPLETTER lett = &letters[totNumbLetters];


		totNumbLetters++;
		lett->lpPixel = yl_calloc("SWT55", size, sizeof(PIXEL));
	    if(lett->lpPixel == NULL) {
	    	printf("addNewLetter; yl_calloc: out of memory! totNumbLetters= %d\n ", totNumbLetters);return;
	    }
	    copyPixels(lett->lpPixel, cc, size);
//		this.cc = cc;
	    lett->medianStrokeWidth = median;
//		this.medianStrokeWidth = medianStrokeWidth;

	 	calculateRegularBoundingBox(cc, size, &lett->regularBoundingBox);
		calculateCentroidPixel(cc, size, &lett->centroid);

 	   {float arr[size];
 	          movePixelsValueToArr(arr, cc, size, img);
 	         lett->averageBrightness = (float)getArrMean(arr, size);
 	   }

	}

		void filterComponents(PIXELSLIST *components, LPFImage swt, LPFImage img, LPLETTER letters) {
			LPPIXEL lpP;
			int size, i;
			double mean;
			double variance;
			double median;
			double aspect;
			LETT_RECT bb;
			float diameter;
//			char buff[2000];

//			final List<LetterCandidate> output = new ArrayList<LetterCandidate>();

//			final DescriptiveStatistics stats = new DescriptiveStatistics();
		       for(i=0;i<totSizeComponents;i++){
		    	   lpP = components[i].lpPixel;
//		    	   size = components[i].size;
		    	   size = lpP[0].x-1;
//   	   printf("i= %d, size = %d\n", i, size);
//	   fflush(stdout);
// 	     sprintf(buff,"i= %d, size = %d\n", i, size);
//		saveLogFile("", buff, "log_SWT.txt");
					if (size < 150)
						continue;

		    	   {float arr[size];
//                     int k;
/*
if(i < 20 && size < 20) {
    	   printf("component= %d, size= %d\n",i, size);
    	   for(k=1;k<size+1;k++){
    	      printf("pixel[ %d ]= (%d,%d,%f),",k, lpP[k].x, lpP[k].y, swt->pixels[lpP[k].y*swt->width+lpP[k].x]);
    	   }
    	   printf("\n");
}
*/
//   if(i == 9) {
//	   printf("component= %d, size= %d\n",i, size);
//	   for(k=1;k<size+1;k++){
//	      printf("pixel[ %d ]= (%d,%d,%f),\n",k, lpP[k].x, lpP[k].y, swt->pixels[lpP[k].y*swt->width+lpP[k].x]);
//	   }
//	   printf("\n");

//        }
		    	      movePixelsValueToArr(arr, &lpP[1], size, swt);

//		       	   for(k=0;k<size;k++){
//		       	      printf("arr[ %d ]= (%f),",k, arr[k]);
//		       	   }
//		       	printf("\n");

				      mean = getArrMean(arr, size);
				      variance = getArrVariance(arr, size);
				      median = getArrPercentile(arr, size, 50);

//      if(i == 9) {
//        printf("mean = %f, variance= %f, median= %f\n", mean, variance, median);
//	    fflush(stdout);
//      }
//	      sprintf(buff,"mean = %f, variance= %f, 0.5*mean= %f, median= %f\n", mean, variance, 0.5 * mean, median);
//   	      saveLogFile("", buff, "log_SWT.txt");


		    	   }
					// test variance of stroke width
					if (variance > 0.5 * mean)
						continue;
					// test aspect ratio
//if( i == 1210) {
//    printf("i = %d\n", i);
//   fflush(stdout);

					aspect = calculateOrientatedBoundingBoxAspectRatio(&lpP[1], size, swt);
//   if(i == 9) {
//     printf("aspect = %f\n", aspect);
//     fflush(stdout);
//   }
//				      sprintf(buff,"aspect = %f\n", aspect);
//			   	      saveLogFile("", buff, "log_SWT.txt");

					if (aspect < 0.1 || aspect > 10)
						continue;
//}

					// test diameter
				 	calculateRegularBoundingBox(&lpP[1], size, &bb);
//					bb = cc.calculateRegularBoundingBox();
					diameter = max(bb.width, bb.height);

//	     printf("diameter = %f\n", diameter);
//	     fflush(stdout);

//			      sprintf(buff,"diameter = %f, diameter / median= %f\n", diameter, diameter / median);
//		   	      saveLogFile("", buff, "log_SWT.txt");

					if (diameter / median > 10)
						continue;

					// check occlusion

					// check height

//					if (size < 150)
//						continue;

//					 printf("mean = %f, variance= %f, median= %f, aspect = %f, diameter = %f\n", mean, variance, median, aspect, diameter);
//					 fflush(stdout);
//				      sprintf(buff,"mean = %f, variance= %f, median= %f, aspect = %f, diameter = %f\n", mean, variance, median, aspect, diameter);
//			   	      saveLogFile("", buff, "log_SWT.txt");

					addNewLetter(&lpP[1], size, letters, (float) median, img);


//					if(i>4) break;

		       }
		}


	 PIXEL* topLeftMostPixel(LPPIXEL lpP, int size) {
		int top = (int)INT_MAX_VALUE;
		PIXEL *pix = NULL;
		int i;
		for(i=0;i<size;i++){
			if (lpP[i].y < top) {
				top = lpP[i].y;
				pix = &lpP[i];
			}
		}
		for(i=0;i<size;i++){
			if (lpP[i].y == top) {
				if (lpP[i].x < pix->x)
				     pix = &lpP[i];
			}
		}

		return pix;
	}
	 int pixelsCcontains( LPPIXEL lpP, int size, PIXEL *target) {
          int i;
			for(i=0;i<size;i++){
				if(pixelsSame(lpP[i], target[0])) return 1;
			}
			return 0;
	 }

	 void nextEdgePixelACW4(LPPIXEL lpP, int size, PIXEL *current, int dir, PIXEL *next){
		 int startdir = (dir + 3) % 4;
		 PIXEL target[4];
		 int i;

		 setPixelSWT(next, current);
		 setPixelXY(&target[0], current->x + 1, current->y);
		 setPixelXY(&target[1], current->x, current->y-1);
		 setPixelXY(&target[2], current->x-1, current->y);
		 setPixelXY(&target[3], current->x, current->y+1);
			for (i = 0; i < 4; i++) {
				int dir = startdir + i;
				if (dir >= 4)
					dir -= 4;
				if (pixelsCcontains( lpP, size, &target[dir])) {
					setPixelSWT(next, &target[dir]);return;
				}
			}
	 }


	 int code4(PIXEL *current, PIXEL *next){
			if (current->x - 1 == next->x)
				return 2;
			if (current->y + 1 == next->y)
				return 3;
			if (current->x + 1 == next->x)
				return 0;
			if (current->y - 1 == next->y)
				return 1;
			return -1;
	 }
    int Poly4_getInnerBoundary(LPPIXEL lpP, int size, LPFImage swt, PIXEL* p, int pSize){
			PIXEL *start = topLeftMostPixel(lpP, size);
			PIXEL current;
			PIXEL next;
			int dir, newSize;


//	       printf("start.x = %d, start.y= %d\n", start->x,start->y);
//		   fflush(stdout);


			setPixelSWT(&current, start);
			    newSize = 0;
				dir = 3;
				while (1) {

//				       printf("current.x = %d, current.y= %d,  ", current.x,current.y);
//					   fflush(stdout);

					nextEdgePixelACW4(lpP, size, &current, dir, &next);

//			       printf("next.x = %d, next.y= %d, *pSize= %d, ", next.x,next.y, pSize);
//				   fflush(stdout);

					if (newSize >= 2 && pixelsSame(next, p[1]) && pixelsSame(current, *start))
						break;
					dir = code4(&current, &next);

//			       printf("dir= %d, ", dir);
//				   fflush(stdout);
				    if(newSize > pSize) {
				      printf("getInnerBoundary; newSize = %d, pSize= %d !!!!!!!!!!\n", newSize, pSize);
		    	      fflush(stdout);return newSize;
				       }

					setPixelSWT(&p[newSize], &current);newSize++;
					setPixelSWT(&current, &next);

//				       printf("*pSize= %d\n", *pSize);
//					   fflush(stdout);


				}
//		       printf("end: pSize= %d, p[1].x= %d, p[1].y= %d\n", *pSize, p[1].x, p[1].y);
//			   fflush(stdout);

				return newSize;
	    }

    LPPIXEL getLowestPoint2d(LPPIXEL Points, int size){
    	LPPIXEL lowest, temp;
    	int i;
    	lowest = &Points[0];
		for (i = 1; i < size; i++) {
	    	 temp = &Points[i];
			if (temp->y < lowest->y || (temp->y == lowest->y && temp->x < lowest->x)) {
				lowest = temp;
			}
		}
		return lowest;
    }

    int ComparatorPoint2d(PIXEL *a, PIXEL *b, PIXEL *lowest){
		double thetaA, thetaB;
		double distanceA, distanceB;

		if (pixelsSame(*a, *b))
			return 0;
		thetaA = atan2(a->y - lowest->y, a->x - lowest->x);
		thetaB = atan2(b->y - lowest->y, b->x - lowest->x);
		if (thetaA < thetaB) {
			return -1;
		}
		else if (thetaA > thetaB) {
			return 1;
		}
		else {
			// collinear with the 'lowest' Point2d, let the Point2d
			// closest to it come first
			distanceA = sqrt(((lowest->x - a->x) * (lowest->x - a->x))+
					((lowest->y - a->y) * (lowest->y - a->y)));
			distanceB = sqrt(((lowest->x - b->x) * (lowest->x - b->x))+
					((lowest->y - b->y) * (lowest->y - b->y)));

			if (distanceA < distanceB) {
				return -1;
			}
			else {
				return 1;
			}
		}
    }
    void sortPointsWithComparator(LPPIXEL points, int lo, int hi, LPPIXEL pLowest){
        int i = lo;
        int j = hi;
        int m = (lo+hi)/2;
        PIXEL x;
    	setPixelSWT(&x, &points[m]);

        do {
          while ( ComparatorPoint2d(&points[i], &x, pLowest)< 0 )
            i++;

          while ( ComparatorPoint2d(&points[j], &x, pLowest)> 0 )
            j--;

          if (i<=j) {
            PIXEL tmp;
            setPixelSWT(&tmp, &points[j]);
            setPixelSWT(&points[j], &points[i]);
            setPixelSWT(&points[i], &tmp);
            i++;
            j--;
          }
        } while (i <= j);

        // recursion
        if (lo < j)
        	sortPointsWithComparator(points, lo, j, pLowest);

        if (i < hi)
        	sortPointsWithComparator(points, i, hi, pLowest);
    }

    void copyPixels(LPPIXEL PointsOutp, LPPIXEL PointsInp, int size){
    	int i;
    	for(i=0;i<size;i++)
    		setPixelSWT(&PointsOutp[i], &PointsInp[i]);
    }
  void getSortedPoint2dSet(LPPIXEL Points, int size) {
	     PIXEL lowest;
		 PIXEL *pLowest = getLowestPoint2d(Points, size);
		 setPixelSWT(&lowest, pLowest);
//        copyPixels(PointsOutp, PointsInp, size);

//	printf("lowest= (%d,%d), size-1=%d\n",pLowest->x,pLowest->y, size-1);
		 sortPointsWithComparator(Points, 0, size-1, &lowest);

/*
		{
			int i;
			printf("-----------------------------------------------\n");
			for(i=0;i<size;i++)
				printf("(%d,%d),",PointsInp[i].x,PointsInp[i].y);
			printf("\n");
//			printf("PointsInp=");
			for(i=0;i<size;i++)
				printf("(%d,%d),",PointsOutp[i].x,PointsOutp[i].y);
			printf("\n");

		}
*/
	}
  int getSizeForSortedPixels(LPPIXEL poly, int polySize){
	  int i, size = 1;
	     for(i = 1;i<polySize;i++){
		     if(!pixelsSame(poly[i-1], poly[i]))
				size++;
	     }
	     return size;
  }
  void copySortetPixels(LPPIXEL polyOut, LPPIXEL poly, int polySize){
	  int i, k = 0;
		 setPixelSWT(&polyOut[k], &poly[0]);k++;
	     for(i = 1;i<polySize;i++){
		     if(!pixelsSame(poly[i-1], poly[i])){
		    	 setPixelSWT(&polyOut[k], &poly[i]);k++;
		     }
	     }
  }

 int getTurn(PIXEL* a, PIXEL* b, PIXEL* c) {
		double crossProduct = ((b->x - a->x) * (c->y - a->y)) -
				((b->y - a->y) * (c->x - a->x));

		if (crossProduct > 0) {
			return COUNTER_CLOCKWISE;
		}
		else if (crossProduct < 0) {
			return CLOCKWISE;
		}
		else {
			return COLLINEAR;
		}
 }

  int areAllCollinear(LPPIXEL points, int size){
	PIXEL a, b, c;
	int i;

		if (size < 2)
			return 1;
		 a = points[0];
		 b = points[1];
		 for (i = 2; i < size; i++) {
			  c = points[i];
			  if (getTurn(&a, &b, &c) != COLLINEAR) {
					return 0;
				}
			}
		return 1;

  }
  int stackPush(PIXEL *stack, int size, int stackHead, PIXEL *value){
     int h = stackHead+1;
     if(h < size ){
    	 setPixelSWT(&stack[h], value);
     }else
    	 printf("stack size= %d; current head = %d\n",size,stackHead);
     return h;
  }
  int  stackPop(PIXEL *stack, int size, int stackHead, PIXEL *value){
	  int h = stackHead;
	  if(h>=0)
	     setPixelSWT(value, &stack[h]);
	  h--;
	  return h;
  }
  int  stackPeek(PIXEL *stack, int size, int stackHead, PIXEL *value){
	  if(stackHead>= 0 && stackHead<size) {
	     setPixelSWT(value, &stack[stackHead]);
	     return 1;
	  }
	  return 0;
  }
 void printStack(PIXEL *stack, int stackHead){
	 int k;
	   printf("stack-------\n");
	for(k=0;k<=stackHead;k++)
		printf("(%d,%d),",stack[k].x,stack[k].y);
	printf("\n");
 }

 int getConvexHull(LPPIXEL poly, int polySize, LPPIXEL* polyOut, int *polySizeOut) {

        LPPIXEL sorted;
        int sortedSize = 0;

	    getSortedPoint2dSet(poly, polySize);
	    sortedSize = getSizeForSortedPixels(poly, polySize);
        sorted = (LPPIXEL)yl_calloc("SWT55", sortedSize, sizeof(PIXEL));
	    if(sorted == NULL) {
	    	printf("yl_calloc: out of memory!\n ");fflush(stdout);return -1;
	    }
	    copySortetPixels(sorted, poly, polySize);

/*
		{
			int i;
			printf("sorted -----------------------------------------------\n");
			for(i=0;i<sortedSize;i++)
				printf("(%d,%d),",sorted[i].x,sorted[i].y);
			printf("\n");
			fflush(stdout);

		}
*/



		if (sortedSize <= 3) {
			*polyOut = poly;
			*polySizeOut = polySize;
			return 0;
		}

		if (areAllCollinear(sorted, sortedSize)) {
			*polyOut = poly;
			*polySizeOut = polySize;
			return 0;
		}
// stack
		{
			int   stackSize = sortedSize+1;
			PIXEL stack[stackSize];
			int stackHead = -1, ret, turn;
			PIXEL head;
			PIXEL middle;
			PIXEL tail;


			int i;
			stackHead = stackPush(stack, stackSize, stackHead, &sorted[0]);
			stackHead = stackPush(stack, stackSize, stackHead, &sorted[1]);

//	printf("stackHead= %d, stackSize= %d\n",stackHead, stackSize);
//	printStack(stack, stackHead);
//	fflush(stdout);


			for (i = 2; i < sortedSize; i++) {
				head = sorted[i];
				stackHead = stackPop(stack, stackSize, stackHead, &middle);
				ret = stackPeek(stack, stackSize, stackHead, &tail);

//printf("i= %d, head= (%d,%d), middle= (%d,%d), tail = (%d,%d), stackHead = %d\n",i,head.x,head.y,middle.x,middle.y,tail.x,tail.y,stackHead);
//printStack(stack, stackHead);
//fflush(stdout);

				turn = getTurn(&tail, &middle, &head);
//	printf("turn= %d,\n",turn);
//	fflush(stdout);
				switch (turn) {
				case COUNTER_CLOCKWISE:
					stackHead = stackPush(stack, stackSize, stackHead, &middle);
					stackHead = stackPush(stack, stackSize, stackHead, &head);
				     //if(stackHead >= sortedSize ){
//				    	 printf("4 stack size= %d; current head = %d\n",sortedSize,stackHead);
//				    	 fflush(stdout);
//				     return -1;
//				     }
//		 	printf("3. stackHead= %d\n",stackHead);
//		    printStack(stack, stackHead);
//	        fflush(stdout);

					break;
				case CLOCKWISE:
					i--;
					break;
				case COLLINEAR:
					stackHead = stackPush(stack, stackSize, stackHead, &head);
					break;
				}
			}
			// close the hull
			stackHead = stackPush(stack, stackSize, stackHead, &sorted[0]);

//		 	printf("6. stackHead= %d\n",stackHead);
//	    	printStack(stack, stackHead);
//	    	fflush(stdout);

	        yl_free(sorted);

//	 	printf("7. stackHead= %d\n",stackHead);
//    	printStack(stack, stackHead);
//    	fflush(stdout);

	        *polySizeOut = stackHead+1;
	        *polyOut = yl_calloc("SWT55", *polySizeOut, sizeof(PIXEL));
		    if(*polyOut == NULL) {
		    	printf("polyOut;yl_calloc: out of memory!\n ");fflush(stdout);return -1;
		    }
		    copySortetPixels(*polyOut, stack, *polySizeOut);

		}
	    return 0;
	}

 	void calculateRegularBoundingBox(LPPIXEL poly, int size, LETT_RECT* rectOut) {
	float xmin = POSITIVE_INFINITY, xmax = -POSITIVE_INFINITY, ymin = POSITIVE_INFINITY, ymax = -POSITIVE_INFINITY;
		int i;
		float px, py;
//		PIXEL *ppp;
//		for (pp = 0; pp < size+1 pp++)	{
		for (i=0; i<size;i++)	{
			px = poly[i].x;
			py = poly[i].y;
			if (px < xmin)
				xmin = px;
			if (px > xmax)
				xmax = px;
			if (py < ymin)
				ymin = py;
			if (py > ymax)
				ymax = py;
		}
		rectOut->x = xmin;rectOut->y = ymin;
		rectOut->width = xmax - xmin;rectOut->height = ymax - ymin;
//		return new Rectangle(xmin, ymin, xmax - xmin, ymax - ymin);
	}
	void computeBounds(LPLETTER letters, int size, LETT_RECT* rectOut) {
		float minx = POSITIVE_INFINITY;float miny = POSITIVE_INFINITY;
		float maxx = 0;float maxy = 0;
		LETT_RECT *r;
		int i;

		for (i=0; i<size;i++)	{

//			final Rectangle r = letter.cc.calculateRegularBoundingBox();
			r = &letters[i].regularBoundingBox;

			if (r->x < minx)
				minx = r->x;
			if (r->y < miny)
				miny = r->y;
			if (r->x + r->width > maxx)
				maxx = r->x + r->width;
			if (r->y + r->height > maxy)
				maxy = r->y + r->height;
		}
		rectOut->x = minx;rectOut->y = miny;
		rectOut->width = maxx - minx;rectOut->height = maxy - miny;
	}

	void RotatedRectangle(ROTATED_RECT *regRectOut, LETT_RECT *regRect, float rotation) {
		regRectOut->width = regRect->width;
		regRectOut->height = regRect->height;
		regRectOut->cx = regRect->x + regRect->width / 2;
		regRectOut->cy = regRect->y + regRect->height / 2;
		regRectOut->rotation = rotation;
	}
	int getIndex(PIXEL* convexHull, int size, int corner) {
		int index = 0, i, change;
		PIXEL point, temp;
		float px, tx;
		float py, ty;

		setPixelSWT(&point, &convexHull[0]);
		px = point.x;py = point.y;
		for (i = 1; i < size - 1; i++) {
			setPixelSWT(&temp, &convexHull[i]);
			change = 0;
			tx = temp.x;ty = temp.y;

			switch (corner) {
			case UPPER_RIGHT:
				if((tx > px || (tx == px && ty > py))) change = 1;
				else change = 0;
				break;
			case UPPER_LEFT:
				if((ty > py || (ty == py && tx < px))) change = 1;
				else change = 0;
				break;
			case LOWER_LEFT:
				if((tx < px || (tx == px && ty < py))) change = 1;
				else change = 0;
				break;
			case LOWER_RIGHT:
				if((ty < py || (ty == py && tx > px)))  change = 1;
				else change = 0;
				break;
			}
			if (change) {
				index = i;
				px = tx;
				py = ty;
			}
		}
		return index;
	}
 void Caliper(PIXEL *convexHull, int size, int index, double ang, CALIPER *Cal){

	 LPPIXEL p;
     p = yl_calloc("SWT55", size, sizeof(PIXEL));
     if(p == NULL) {
	    	printf("Caliper; yl_calloc: out of memory!\n ");return;
    }
	 copyPixels(p, convexHull, size);
	 Cal->convexHull = p;Cal->size = size;Cal->currentAngle = ang;Cal->pointIndex = index;
 }
 void freeCaliper(CALIPER* Cal){
	 if(Cal->convexHull != NULL)
		 yl_free(Cal->convexHull);
 }

	int isVertical(CALIPER *Cal) {
		if((fabs(Cal->currentAngle - ANGLE_90DEG_IN_RADS) < SIGMA)
				|| (fabs(Cal->currentAngle - ANGLE_270DEG_IN_RADS) < SIGMA))
			return 1;
		return 0;
	}
	int isHorizontal(CALIPER *Cal) {
		if((fabs(Cal->currentAngle) < SIGMA) || (fabs(Cal->currentAngle - ANGLE_180DEG_IN_RADS) < SIGMA))
			return 1;
		return 0;
	}
	double getSlope(double currentAngle) {
		return tan(currentAngle);
	}

	double getConstant(CALIPER *Cal) {
		PIXEL*p = &(Cal->convexHull[Cal->pointIndex]);
		return ( p->y - (getSlope(Cal->currentAngle) * p->x) );
	}

	void getIntersection(CALIPER *this, CALIPER *that, PIXEL* pOut) {

		// the x-intercept of 'this' and 'that': x = ((c2 - c1) / (m1 - m2))
		double x;
		// the y-intercept of 'this' and 'that', given 'x': (m*x) + c
		double y;
		if (isVertical(this)) {
			x = this->convexHull[this->pointIndex].x;
		}
		else if (isHorizontal(this)) {
			x = that->convexHull[that->pointIndex].x;
		}
		else {
			x = (getConstant(that) - getConstant(this)) / (getSlope(this->currentAngle) - getSlope(that->currentAngle));
		}

		if (isVertical(this)) {
			y = getConstant(that);
		}
		else if (isHorizontal(this)) {
			y = getConstant(this);
		}
		else {
			y = (getSlope(this->currentAngle) * x) + getConstant(this);
		}
		pOut->x = x;pOut->y = y;
	}
	double getArea(LPPIXEL rectangle) {

		double deltaXAB = rectangle[0].x - rectangle[1].x;
		double deltaYAB = rectangle[0].y - rectangle[1].y;

		double deltaXBC = rectangle[1].x - rectangle[2].x;
		double deltaYBC = rectangle[1].y - rectangle[2].y;

		double lengthAB = sqrt((deltaXAB * deltaXAB) + (deltaYAB * deltaYAB));
		double lengthBC = sqrt((deltaXBC * deltaXBC) + (deltaYBC * deltaYBC));

		return lengthAB * lengthBC;
	}
	double getAngleNextPoint(CALIPER *Cal) {
		PIXEL p1;
		PIXEL p2;
		double deltaX, deltaY, angle;

   	     setPixelSWT(&p1, &Cal->convexHull[Cal->pointIndex]);
   	     setPixelSWT(&p2, &Cal->convexHull[(Cal->pointIndex+1)%Cal->size]);

		 deltaX = p2.x - p1.x;
		 deltaY = p2.y - p1.y;
		 angle = atan2(deltaY, deltaX);

		return (angle < 0 ? ANGLE_360DEG_IN_RADS + angle : angle);
	}


	double getDeltaAngleNextPoint(CALIPER *Cal) {

		double angle = getAngleNextPoint(Cal);
		angle = angle < 0 ? ANGLE_360DEG_IN_RADS + angle - Cal->currentAngle : angle - Cal->currentAngle;
		return (angle < 0 ? ANGLE_360DEG_IN_RADS : angle);
	}
	double getSmallestTheta(CALIPER *I, CALIPER *J, CALIPER *K, CALIPER *L) {

		double thetaI = getDeltaAngleNextPoint(I);
		double thetaJ = getDeltaAngleNextPoint(J);
		double thetaK = getDeltaAngleNextPoint(K);
		double thetaL = getDeltaAngleNextPoint(L);

		if (thetaI <= thetaJ && thetaI <= thetaK && thetaI <= thetaL) {
			return thetaI;
		}
		else if (thetaJ <= thetaK && thetaJ <= thetaL) {
			return thetaJ;
		}
		else if (thetaK <= thetaL) {
			return thetaK;
		}
		else {
			return thetaL;
		}
	}
	void rotateBy(CALIPER *Cal, double angle) {

		if (getDeltaAngleNextPoint(Cal) == angle) {
			Cal->pointIndex++;
		}

		Cal->currentAngle += angle;
	}
	void makeRotated(LPPIXEL rectangle, double angle, ROTATED_RECT *rotR) {
		double deltaXAB = rectangle[0].x - rectangle[1].x;
		double deltaYAB = rectangle[0].y - rectangle[1].y;
		double deltaXBC = rectangle[1].x - rectangle[2].x;
		double deltaYBC = rectangle[1].y - rectangle[2].y;

		double lengthAB = sqrt((deltaXAB * deltaXAB) + (deltaYAB * deltaYAB));
		double lengthBC = sqrt((deltaXBC * deltaXBC) + (deltaYBC * deltaYBC));

		double cx = (rectangle[0].x + rectangle[1].x + rectangle[2].x + rectangle[3].x) / 4;
		double cy = (rectangle[0].y + rectangle[1].y + rectangle[2].y + rectangle[3].y) / 4;
		rotR->cx = (float) cx;
		rotR->cy = (float) cy;
		rotR->width = (float) lengthAB;
		rotR->height = (float) lengthBC;
		rotR->rotation = (float) angle;
	}

 void Poly4_minimumBoundingRectangle(PIXEL *poly, int polySize, ROTATED_RECT *rotR, int printFlg) {
	  LPPIXEL convexHull; int convexHullSize;
	  LETT_RECT r;


/*
	  if(printFlg){
		  printf("polySize= %d\n", polySize);
		{
			int i;
			printf("poly -----------------------------------------------\n");
			for(i=0;i<polySize;i++)
				printf("(%d,%d),",poly[i].x,poly[i].y);
			printf("\n");

		}
	  }
*/
	  getConvexHull(poly, polySize, &convexHull, &convexHullSize);

/*
  if(printFlg){

	  printf("convexHullSize= %d\n", convexHullSize);
		{
			int i;
			printf("convexHull -----------------------------------------------\n");
			for(i=0;i<convexHullSize;i++)
				printf("(%d,%d),",convexHull[i].x,convexHull[i].y);
			printf("\n");

		}
  }
*/

		if (convexHullSize < 3) {
			calculateRegularBoundingBox(convexHull, convexHullSize, &r);
			RotatedRectangle(rotR, &r, 0);
//			return new RotatedRectangle(convexHull.calculateRegularBoundingBox(polyOut, &polySizeOut), 0);
		}
		{
		LPPIXEL minimum = NULL;
		PIXEL minimumRectangle[4];
		double minimumAngle = 0;
		double area = POSITIVE_INFINITY;
		CALIPER I, J, K, L;
		PIXEL rectangle[4];
		double tempArea;
		double smallestTheta;
          I.convexHull = NULL;J.convexHull = NULL;
	      K.convexHull = NULL;L.convexHull = NULL;

		  Caliper(convexHull, convexHullSize, getIndex(convexHull, convexHullSize, UPPER_RIGHT), ANGLE_90DEG_IN_RADS, &I);
		  Caliper(convexHull, convexHullSize, getIndex(convexHull, convexHullSize, UPPER_LEFT), ANGLE_180DEG_IN_RADS, &J);
		  Caliper(convexHull, convexHullSize, getIndex(convexHull, convexHullSize, LOWER_LEFT), ANGLE_270DEG_IN_RADS, &K);
		  Caliper(convexHull, convexHullSize, getIndex(convexHull, convexHullSize, LOWER_RIGHT), ANGLE_0DEG_IN_RADS, &L);

		 while (L.currentAngle < ANGLE_90DEG_IN_RADS) {
			     getIntersection(&L, &I, &rectangle[0]);
	    		 getIntersection(&I, &J, &rectangle[1]);
	    		 getIntersection(&J, &K, &rectangle[2]);
	    		 getIntersection(&K, &L, &rectangle[3]);

				tempArea = getArea(rectangle);
				if (minimum == NULL || tempArea < area) {
		            copyPixels(minimumRectangle, rectangle, 4);
					minimum = minimumRectangle;
					minimumAngle = L.currentAngle;
					area = tempArea;
				}
				smallestTheta = getSmallestTheta(&I, &J, &K, &L);
				rotateBy(&I, smallestTheta);
				rotateBy(&J, smallestTheta);
				rotateBy(&K, smallestTheta);
				rotateBy(&L, smallestTheta);
			}

		  freeCaliper(&I);freeCaliper(&J);freeCaliper(&K);freeCaliper(&L);
		  makeRotated(minimum, minimumAngle, rotR);
		}
 }
		double calculateOrientatedBoundingBoxAspectRatio(LPPIXEL lpP, int size, LPFImage swt){
		    PIXEL* poly, *polyNew;
		    int polySizeInit = 3*size;
		    int newPolySize;
		    ROTATED_RECT rotR;
		    int printFlg = 0;
		    poly = yl_calloc("SWT55", polySizeInit, sizeof(PIXEL));
/*
	  if(size == 9 && lpP[0].x==470 && lpP[0].y == 1){
		    {int i;
	    	   printf("size= %d\n", size);
	    	   for(i=0;i<size;i++){
	    	      printf("(%d,%d), ",lpP[i].x, lpP[i].y);
	    	   }
	    	   printf("\n");
			   fflush(stdout);
		    }
		    printFlg = 1;
	  }
*/

		    newPolySize = Poly4_getInnerBoundary(lpP, size, swt, poly, polySizeInit);


	         polyNew = yl_calloc("SWT55", newPolySize, sizeof(PIXEL));
	         copyPixels(polyNew, poly, newPolySize);
	         yl_free(poly);


/*
		    {int i;
	    	   printf("polySize= %d\n", size);
	    	   for(i=0;i<polySize;i++){
	    	      printf("(%d,%d), ",poly[i].x, poly[i].y);
	    	   }
	    	   printf("\n");
			   fflush(stdout);
		    }

*/
		    Poly4_minimumBoundingRectangle(polyNew, newPolySize, &rotR, printFlg);

//	   printf("Rectangle= (%f, %f, %f, %f, %f), ",rotR.cx, rotR.cy, rotR.width,rotR.height,rotR.rotation);
//	   fflush(stdout);

	         yl_free(polyNew);


//	    		   printf("Rectangle= (%f, %f, %f, %f, %f), ",rotR.cx, rotR.cy, rotR.width,rotR.height,rotR.rotation);

//		    	   printf("\n");
//  		   printf("rotR.width= %f, rotR.height= %f\n",rotR.width,rotR.height);
//		   fflush(stdout);

            if(rotR.height < SIGMA || rotR.width < SIGMA)
            	return 1000;
	         return rotR.height / rotR.width;


		}
      void movePixelsValueToArr(float *arr,  LPPIXEL lpP, int size, LPFImage swt){
      	   int location;
      	   int i;
  		    for (i = 0; i < size; i++) {
  		    	location = lpP[i].y*swt->width+lpP[i].x;
  		    	arr[i] = swt->pixels[location];
  		    }

      }
     double getArrMean(float *arr, int size){
      	   double mean = 0;
      	   int count = 1, i;

            if(size == 0) return mean;
            mean = arr[0];
   		    for (i = 1; i < size; i++) {
   		    	count++;
   		    	mean = mean + (arr[i] - mean) / count;
   		    }
   		return mean;
   	}

    double getArrVariance(float *arr, int size){
   	    int count = 1, i;
		double oldMean;
		double newMean;
		double x;
		double var = 0;

			if (size < 2) return var;

			oldMean = arr[0];
			newMean = arr[0];

			for (i = 1; i < size; i++) {
				count++;
				x = arr[i];
				newMean = oldMean + (x - oldMean) / count;
				var = var + (x - oldMean) * (x - newMean);
				oldMean = newMean;
			}

			return var / (count - 1);
		}
	void qSortArr(float *arr, int lo, int hi) {
    int i = lo;
    int j = hi;

    float x = arr[(lo+hi)/2];

    //partition
    do {
      while ( arr[i] < x )
        i++;

      while ( arr[j] > x )
        j--;

      if (i<=j) {
        float tmp = arr[j];
        arr[j] = arr[i];
        arr[i] = tmp;
        i++;
        j--;
      }
    } while (i <= j);

    // recursion
    if (lo < j)
    	qSortArr(arr, lo, j);

    if (i < hi)
    	qSortArr(arr, i, hi);
  }

    double getArrPercentile(float *arr, int size, int perc){
    	int idx = (int)((float)(size-1)*((float)perc/100.0) +0.5);
    	if(idx>size-1) idx=size-1;
    	qSortArr(arr, 0, size-1);
        return (double)arr[idx];
    }

//		public static double std(double[][] arr) {
//			return Math.sqrt(var(arr));
//		}

    void generateRays(LPFImage edges, LPFImage dx, LPFImage dy, int detectDark, LPFImage output, LPPIXELSLIST rays) {
//        PIXELSLIST pList;
//    	int size;
        int y, x;
//        int i, j;
		float gradDirection = detectDark ? -1 : 1;

//		int tSizePrev, stop;


		for (y = 0; y < output->height; y++) {
//			tSizePrev = totSizeRays;
			for (x = 0; x < output->width; x++) {
				if (edges->pixels[y*edges->width+x] > 0) {
					traceRay(edges, dx, dy, detectDark, output, gradDirection, x, y, rays, 1, 0, 0, 1);
					traceRay(edges, dx, dy, detectDark, output, gradDirection, x, y, rays, 1, 1, -1, 1);
					traceRay(edges, dx, dy, detectDark, output, gradDirection, x, y, rays, 1, -1, 1, 1);
				}
			}
//		   printf("y= %d, tSizePrev= %d, totSizeRays= %d\n", y, tSizePrev, totSizeRays);fflush(stdout);

/*
			if(y>42 &&y<44 && totSizeRays>0) {
			   printf("y= %d, tSizePrev= %d, totSizeRays= %d\n", y, tSizePrev, totSizeRays);fflush(stdout);
			   stop = totSizeRays; //if(stop-tSizePrev> 20) stop = tSizePrev + 20;
			  for(j=tSizePrev;j<stop;j++) {
				  printf("j=%d, ",j);
			     for(i=0;i<rays[j].size;i++)
				    printf("i=%d, (%d,%d); ", i, rays[j].lpPixel[i].x,rays[j].lpPixel[i].y);
			     printf("\n");fflush(stdout);
			  }

			}
*/
		}
//		printf("size= %d\n", totSizeRays);fflush(stdout);
    }

    void startPixelIterator(int x0, int y0, float fdx, float fdy){
	   xIt = x0;
	   yIt = y0;
	   dxIt = fabs(fdx);
	   dyIt = fabs(fdy);
	   sxIt = (fdx > 0) ? 1 : -1;
	   syIt = (fdy > 0) ? 1 : -1;
	   errIt = dxIt - dyIt;
//	   Pixel p = new Pixel();
    }
    void pixelIteratorNext(){
		pIt.x = xIt;
		pIt.y = yIt;
		float e2 = 2 * errIt;
		if (e2 > -dyIt) {
			errIt = errIt - dyIt;
			xIt = xIt + sxIt;
		}
		if (e2 < dxIt) {
			errIt = errIt + dxIt;
			yIt = yIt + syIt;
		}
    }
  	double distance(LPPIXEL p1, LPPIXEL p2) {
		return sqrt((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y));
	}
   void traceRay(LPFImage edges, LPFImage dx, LPFImage dy, int
			detectDark, LPFImage output, float gradDirection,
			int x, int y, LPPIXELSLIST rays, int xx, int xy, int yx, int yy){

	    LPPIXEL ray;
	    int sizeRay;
		float gradX = (xx * dx->pixels[y*dx->width+x] + xy * dy->pixels[y*dx->width+x]) * gradDirection;
		float gradY = (yy * dy->pixels[y*dx->width+x] + yx * dx->pixels[y*dx->width+x]) * gradDirection;
		float startGradX, startGradY, currentGradX, currentGradY, tn, td, length;
		PIXEL start, current, end, p;
		int currentX, currentY, endOk, found;
		int i, j;


		startPixelIterator(x, y, gradX, gradY);
	    pixelIteratorNext();
		start.x = pIt.x;start.y = pIt.y;


		for (j = 0; j < maxStrokeWidth; j++) {
		    pixelIteratorNext();
		    current.x = pIt.x;current.y = pIt.y;
			// check bounds
			if (current.x < 1 || current.x >= output->width - 1 || current.y < 1 || current.y >= output->height - 1) {
				break;
			}
			if (fabs(current.x - start.x) < 2 && fabs(current.y - start.y) < 2)
				continue;

//			Pixel end = null;
			endOk = 0;
			for (i = 0; i < edgeSearchRegionL; i++) {
				currentX = current.x + edgeSearchRegion[i][0];
				currentY = current.y + edgeSearchRegion[i][1];

				if (edges->pixels[currentY*edges->width+currentX] > 0) {
					end.x = currentX; end.y = currentY;
					endOk = 1;
					break;
				}
			}
			if (endOk > 0) {
				// edge found; now search for matching gradient in surrounding
				// area
				found = 0;

				startGradX = dx->pixels[start.y*dx->width+start.x];
				startGradY = dy->pixels[start.y*dy->width+start.x];

				for (i = 0; i < gradSearchRegionL; i++) {
					currentX = end.x + gradSearchRegion[i][0];
					currentY = end.y + gradSearchRegion[i][1];

					currentGradX = dx->pixels[currentY*dx->width+currentX];
					currentGradY = dy->pixels[currentY*dy->width+currentX];
					tn = startGradY * currentGradX - startGradX * currentGradY;
					td = startGradX * currentGradX + startGradY * currentGradY;
					if (tn * 7 < -td * 4 && tn * 7 > td * 4) {
						found = 1;
						break;
					}
				}

				// if we found an opposite grad, then we fill in the ray using
				// the supercover to select all pixels that the ray passes
				// through
				if (found) {
					length = (float)distance(&start, &end);
					ray = NULL;
//if(y==1) {printf("start.x= %d, start.y= %d, end.x= %d, end.y= %d\n", start.x, start.y,end.x, end.y);}
//					sizeRay = supercoverAsList(start.x, start.y, end.x, end.y, &ray, y);
					sizeRay = supercoverAsList(start.x, start.y, end.x, end.y, &ray);
//		printf("sizeRay= %d", sizeRay);
					if(MaxSizeRay < sizeRay) MaxSizeRay = sizeRay;
//printf("sizeRay= %d, MaxSizeRay= %d\n", sizeRay, MaxSizeRay);
					for (i = 0; i < sizeRay; i++) {
						p = ray[i];
						output->pixels[p.y*output->width+p.x] = min(length, output->pixels[p.y*output->width+p.x]);
					}
                    rays[totSizeRays].lpPixel = ray;rays[totSizeRays].size = sizeRay;totSizeRays++;
				}

				break;
			}

		}

   	}
/*
	void supercoverAsList(int x1, int y1, int x2, int y2, LPPIXELS *ray) {

		int ystep, xstep; // the step on y and x axis
		int error; // the error accumulated during the increment
		int errorprev; // *vision the previous value of the error variable
		int y = y1, x = x1; // the line points
		int ddy, ddx; // compulsory variables: the double values of dy and dx
		int dx = x2 - x1;
		int dy = y2 - y1;
		int i;
//		PIXEL pixel;
//		pixel.x = x1;pixel.y = y1;
		addPixelXY(ray, x1, y1);// first point
//		size++;
		// NB the last point can't be here, because of its previous point (which
		// has to be verified)
		if (dy < 0) {
			ystep = -1;
			dy = -dy;
		} else
			ystep = 1;
		if (dx < 0) {
			xstep = -1;
			dx = -dx;
		} else
			xstep = 1;

		ddy = 2 * dy; // work with double values for full precision
		ddx = 2 * dx;
		if (ddx >= ddy) { // first octant (0 <= slope <= 1)
			// compulsory initialization (even for errorprev, needed when
			// dx==dy)
			errorprev = error = dx; // start in the middle of the square
			for (i = 0; i < dx; i++) { // do not use the first point
											// (already
											// done)
				x += xstep;
				error += ddy;
				if (error > ddx) { // increment y if AFTER the middle ( > )
					y += ystep;
					error -= ddx;
					// three cases (octant == right->right-top for directions
					// below):
					if (error + errorprev < ddx) // bottom square also
					    addPixelXY(ray, x, y - ystep);
					else if (error + errorprev > ddx) // left square also
					    addPixelXY(ray, x - xstep, y);
					else { // corner: bottom and left squares also
					    addPixelXY(ray, x, y - ystep);
						addPixelXY(ray, x - xstep, y);
					}
				}
				addPixelXY(ray, x, y);
				errorprev = error;
			}
		} else { // the same as above
			errorprev = error = dy;
			for (i = 0; i < dy; i++) {
				y += ystep;
				error += ddx;
				if (error > ddy) {
					x += xstep;
					error -= ddy;
					if (error + errorprev < ddy)
						addPixelXY(ray, x - xstep, y);
					else if (error + errorprev > ddy)
						addPixelXY(ray, x, y - ystep);
					else {
						addPixelXY(ray, x - xstep, y);
					    addPixelXY(ray, x, y - ystep);
					}
				}
				addPixelXY(ray, x, y);
				errorprev = error;
			}
		}
	}
*/
//	int supercoverAsList(int x1, int y1, int x2, int y2, LPPIXEL *ray, int print) {
	int supercoverAsList(int x1, int y1, int x2, int y2, LPPIXEL *ray) {

		int ystep, xstep; // the step on y and x axis
		int error; // the error accumulated during the increment
		int errorprev; // *vision the previous value of the error variable
		int y = y1, x = x1; // the line points
		int ddy, ddx; // compulsory variables: the double values of dy and dx
		int dx = x2 - x1;
		int dy = y2 - y1;
		int i;
		PIXEL pixelTab[1000];
		int size = 0;
//		pixel.x = x1;pixel.y = y1;
		pixelTab[0].x = x1;pixelTab[0].y = y1;
//	if(print==1) {printf("pixelTab[0].x= %d, pixelTab[0].y= %d\n", pixelTab[0].x, pixelTab[0].y);}

		size++;
		// NB the last point can't be here, because of its previous point (which
		// has to be verified)
		if (dy < 0) {
			ystep = -1;
			dy = -dy;
		} else
			ystep = 1;
		if (dx < 0) {
			xstep = -1;
			dx = -dx;
		} else
			xstep = 1;

		ddy = 2 * dy; // work with double values for full precision
		ddx = 2 * dx;
		if (ddx >= ddy) { // first octant (0 <= slope <= 1)
			// compulsory initialization (even for errorprev, needed when
			// dx==dy)
			errorprev = error = dx; // start in the middle of the square
			for (i = 0; i < dx; i++) { // do not use the first point
											// (already
											// done)
				x += xstep;
				error += ddy;
				if (error > ddx) { // increment y if AFTER the middle ( > )
					y += ystep;
					error -= ddx;
					// three cases (octant == right->right-top for directions
					// below):
					if (error + errorprev < ddx) // bottom square also
//					    addPixelXY(ray, x, y - ystep);
					    {pixelTab[size].x = x;pixelTab[size].y = y - ystep;size++;}
					else if (error + errorprev > ddx) // left square also
//					    addPixelXY(ray, x - xstep, y);
					{pixelTab[size].x = x- xstep;pixelTab[size].y = y;size++;}
					else { // corner: bottom and left squares also
//					    addPixelXY(ray, x, y - ystep);
//						addPixelXY(ray, x - xstep, y);
						pixelTab[size].x = x;pixelTab[size].y = y - ystep;size++;
						pixelTab[size].x = x- xstep;pixelTab[size].y = y;size++;
					}
				}
//				addPixelXY(ray, x, y);
				pixelTab[size].x = x;pixelTab[size].y = y;size++;
				errorprev = error;
			}
		} else { // the same as above
			errorprev = error = dy;
			for (i = 0; i < dy; i++) {
				y += ystep;
				error += ddx;
				if (error > ddy) {
					x += xstep;
					error -= ddy;
					if (error + errorprev < ddy)
//						addPixelXY(ray, x - xstep, y);
					{pixelTab[size].x = x- xstep;pixelTab[size].y = y;size++;}
					else if (error + errorprev > ddy)
//						addPixelXY(ray, x, y - ystep);
				    {pixelTab[size].x = x;pixelTab[size].y = y - ystep;size++;}
					else {
//						addPixelXY(ray, x - xstep, y);
//					    addPixelXY(ray, x, y - ystep);
						pixelTab[size].x = x- xstep;pixelTab[size].y = y;size++;
						pixelTab[size].x = x;pixelTab[size].y = y - ystep;size++;
					}
				}
//				addPixelXY(ray, x, y);
				pixelTab[size].x = x;pixelTab[size].y = y;size++;
				errorprev = error;
			}
		}
		{LPPIXEL lpRay;
		*ray = (LPPIXEL)yl_calloc("SWT15", size, sizeof(PIXEL));
		lpRay = *ray;
		for(i=0;i<size;i++){
			lpRay[i].x = pixelTab[i].x;lpRay[i].y = pixelTab[i].y;
		}
		}
		return size;
	}

/*
	void switchRays(LPPIXELSLIST lpTmpMin, LPPIXELSLIST lpTmpMinPrev, LPPIXELSLIST lpTmp, LPPIXELSLIST lpTmpPrev){
		LPPIXELSLIST lpTN;
		lpTmpPrev->next = lpTmpMin;
		lpTN = lpTmpMin->next;
		lpTmpMin->next = lpTmp->next;
		lpTmpMinPrev->next =  lpTmp;
		lpTmp->next = lpTN;
	}
	void switchRays2(LPPIXELSLIST2 lpTmpMin, LPPIXELSLIST2 lpTmpMinPrev, LPPIXELSLIST2 lpTmp, LPPIXELSLIST2 lpTmpPrev){
		LPPIXELSLIST2 lpTN;
		lpTmpPrev->next = lpTmpMin;
		lpTN = lpTmpMin->next;
		lpTmpMin->next = lpTmp->next;
		lpTmpMinPrev->next =  lpTmp;
		lpTmp->next = lpTN;

	}

	void sortPixList(LPPIXELSLIST *rays){
  		LPPIXELSLIST lpTmp, lpTmpPrev, lpTmp1, lpTmpMin, lpTmpMinPrev;
  		int maxSize;
  		lpTmp = *rays;
  		lpTmpPrev = NULL;
  		while(lpTmp != NULL){
  			maxSize = lpTmp->size;
  			lpTmpMin = lpTmp;
  			lpTmpMinPrev = lpTmp;
  			lpTmp1 = lpTmp->next;
  			while(lpTmp1 != NULL){
  				if(lpTmp1->size < maxSize){
  					maxSize = lpTmp1->size;
  					lpTmpMin = lpTmp1;
  				}
  				lpTmpMinPrev = lpTmp1;
  				lpTmp1 = lpTmp1->next;
  			}
  			if(lpTmpMin != lpTmp){
  				switchRays(lpTmpMin, lpTmpMinPrev, lpTmp, lpTmpPrev);
  			}
  			lpTmpPrev = lpTmp;
  			lpTmp= lpTmp->next;
  		}

	}
	void sortPixList2(LPPIXELSLIST2 *rays){
  		LPPIXELSLIST2 lpTmp, lpTmpPrev, lpTmp1, lpTmpMin, lpTmpMinPrev;
  		int maxSize;
  		lpTmp = *rays;
  		lpTmpPrev = NULL;
  		while(lpTmp != NULL){
  			maxSize = lpTmp->size;
  			lpTmpMin = lpTmp;
  			lpTmpMinPrev = lpTmp;
  			lpTmp1 = lpTmp->next;
  			while(lpTmp1 != NULL){
  				if(lpTmp1->size < maxSize){
  					maxSize = lpTmp1->size;
  					lpTmpMin = lpTmp1;
  				}
  				lpTmpMinPrev = lpTmp1;
  				lpTmp1 = lpTmp1->next;
  			}
  			if(lpTmpMin != lpTmp){
  				switchRays2(lpTmpMin, lpTmpMinPrev, lpTmp, lpTmpPrev);
  			}
  			lpTmpPrev = lpTmp;
  			lpTmp= lpTmp->next;
  		}
	}
*/
	void switchRays(LPPIXELSLIST lpRays, int i, int jMin){
		PIXELSLIST rayTmp;
		rayTmp.lpPixel = lpRays[i].lpPixel;
		rayTmp.size = lpRays[i].size;
		lpRays[i].lpPixel = lpRays[jMin].lpPixel;
		lpRays[i].size = lpRays[jMin].size;
		lpRays[jMin].lpPixel = rayTmp.lpPixel;
		lpRays[jMin].size = rayTmp.size;
	}
	void sortPixList(LPPIXELSLIST rays){
  		LPPIXELSLIST lpTmp;
  		int maxSize;
  		int i, j, jMin;
  		lpTmp = rays;

			for(i=0;i<totSizeRays;i++){
				maxSize = lpTmp[i].size;
				jMin = i;
	  			for(j=i+1;j<totSizeRays;j++){
	  				if(lpTmp[j].size <maxSize) jMin = j;
	  			}
                if(jMin !=i){
                	switchRays(lpTmp, i, jMin);
                }

			}

	}
/*
	int GetLastPixelListSize(LPPIXELSLIST *rays){
  		LPPIXELSLIST lpTmp;

		if(*rays == NULL) return 0;
		lpTmp = *rays;
		while(lpTmp->next != NULL)
			lpTmp = lpTmp->next;

		return lpTmp->size;
	}
	int GetLastPixelListSize2(LPPIXELSLIST2 *rays){
  		LPPIXELSLIST2 lpTmp;

		if(*rays == NULL) return 0;
		lpTmp = *rays;
		while(lpTmp->next != NULL)
			lpTmp = lpTmp->next;

		return lpTmp->size;
	}
*/
	float quickSelect(float *arr, int n, int low, int high) {
		high--; // make high inclusive

		int middle, ll, hh;
		float tmp = 0;
		int median = (low + high) / 2;

		while (1) {
			if (high <= low) /* One element only */
				return arr[median];

			if (high == low + 1) { /* Two elements only */
				if (arr[low] > arr[high]) {
					tmp = arr[low];
					arr[low] = arr[high];
					arr[high] = tmp;
				}
				return arr[median];
			}

			/* Find median of low, middle and high items; swap into position low */
			middle = (low + high) / 2;
			if (arr[middle] > arr[high]) {
				tmp = arr[middle];
				arr[middle] = arr[high];
				arr[high] = tmp;
				;
			}
			if (arr[low] > arr[high]) {
				tmp = arr[low];
				arr[low] = arr[high];
				arr[high] = tmp;
			}
			if (arr[middle] > arr[low]) {
				tmp = arr[low];
				arr[low] = arr[middle];
				arr[middle] = tmp;
			}

			/* Swap low item (now in position middle) into position (low+1) */
			tmp = arr[middle];
			arr[middle] = arr[low + 1];
			arr[low + 1] = tmp;

			/* Nibble from each end towards middle, swapping items when stuck */
			ll = low + 1;
			hh = high;
			for (;;) {
				do
					ll++;
				while (arr[low] > arr[ll]);
				do
					hh--;
				while (arr[hh] > arr[low]);

				if (hh < ll)
					break;

				tmp = arr[ll];
				arr[ll] = arr[hh];
				arr[hh] = tmp;
			}

			/* Swap middle item (in position low) back into correct position */
			tmp = arr[low];
			arr[low] = arr[hh];
			arr[hh] = tmp;

			/* Re-set active partition */
			if (hh <= median)
				low = ll;
			if (hh >= median)
				high = hh - 1;
		}
		return 0;
	}

    float median(float *arr, int start, int stop, int len) {
		int median = len/ 2;

		if (len % 2 == 0) {
			float a = quickSelect(arr, median, start, stop);
			float b = quickSelect(arr, median - 1, start, stop);

			return (a + b) / 2;
		}
		return quickSelect(arr, median, start, stop);
	}
/*
	void medianFilter(LPFImage output, LPPIXELSLIST *rays) {
		LPPIXELSLIST lpTmp;
		LPPIXELS lpPix;
		PIXEL pixel;
		float med;
		int i, length;

		if (rays == NULL)
			return;
        sortPixList(rays);
        int maxSize = GetLastPixelListSize(rays);

        {
		float working[maxSize];
		lpTmp = *rays;
		while(lpTmp != NULL){
//		for (final List<Pixel> ray : rays) {
			length = lpTmp->size;
			lpPix = lpTmp->pList;
			while(lpPix != NULL){
//			for (i = 0; i < length; i++) {
				 pixel = lpPix->p;
				 working[i] = output->pixels[pixel.y*output->width+pixel.x];
			}
			med = median(working, 0, length, length);
			lpPix = lpTmp->pList;
			while(lpPix != NULL){
//			for (i = 0; i < length; i++) {
				 pixel = lpPix->p;
				if (output->pixels[pixel.y*output->width+pixel.x] > med)
					output->pixels[pixel.y*output->width+pixel.x] = med;
			}
		}
        }
	}

	void medianFilter2(LPFImage output, LPPIXELSLIST2 *rays) {
		LPPIXELSLIST2 lpTmp;
		LPPIXEL lpPix;
		PIXEL pixel;
		float med;
		int i, length;

		if (rays == NULL)
			return;
        sortPixList2(rays);
        int maxSize = GetLastPixelListSize2(rays);

        {
		float working[maxSize];
		lpTmp = *rays;
		while(lpTmp != NULL){
//		for (final List<Pixel> ray : rays) {
			length = lpTmp->size;

			lpPix = lpTmp->lpPixel;
			for (i = 0; i < length; i++) {
				 pixel = lpPix[i];
				 working[i] = output->pixels[pixel.y*output->width+pixel.x];
			}
			med = median(working, 0, length, length);
			lpPix = lpTmp->lpPixel;
			for (i = 0; i < length; i++) {
				 pixel = lpPix[i];
					if (output->pixels[pixel.y*output->width+pixel.x] > med)
						output->pixels[pixel.y*output->width+pixel.x] = med;
			}
		  }
        }
	}
*/
	void qSortPixList(LPPIXELSLIST rays, int lo, int hi) {
    int i = lo;
    int j = hi;


    int x = rays[(lo+hi)/2].size;

    //partition
    do {
      while ( rays[i].size < x )
        i++;

      while ( rays[j].size > x )
        j--;

      if (i<=j) {
      	switchRays(rays, i, j);
        i++;
        j--;
      }
    } while (i <= j);

    // recursion
    if (lo < j)
    	qSortPixList(rays, lo, j);

    if (i < hi)
    	qSortPixList(rays, i, hi);
  }


	void medianFilter(LPFImage output, LPPIXELSLIST rays) {
		LPPIXELSLIST lpTmp;
		LPPIXEL lpPix;
		PIXEL pixel;
		float med;
		int i, k, length;

		if (rays == NULL)
			return;
//        sortPixList3(rays);
        qSortPixList(rays, 0, totSizeRays-1);

        int maxSize =  rays[totSizeRays-1].size;
//		for (k = 0; k < totSizeRays; k++) {
//			printf("k=%d, size=%d; ",k,rays[k].size);fflush(stdout);
//			if(k%20==0) printf("\n");
//		}
//        printf("maxSize= %d, totSizeRays= %d\n",maxSize, totSizeRays);fflush(stdout);
        {
		float working[maxSize];
		lpTmp = rays;

		for (k = 0; k < totSizeRays; k++) {
//        printf("k= %d\n",k);fflush(stdout);
//		while(lpTmp != NULL){
			length = lpTmp[k].size;
			lpPix = lpTmp[k].lpPixel;

			if(length > maxSize) {printf("length= %d, maxSize= %d",length,maxSize);fflush(stdout);}

			for (i = 0; i < length; i++) {
				 pixel = lpPix[i];
				 working[i] = output->pixels[pixel.y*output->width+pixel.x];
			}
			med = median(working, 0, length, length);
			lpPix = lpTmp[k].lpPixel;
			for (i = 0; i < length; i++) {
				 pixel = lpPix[i];
					if (output->pixels[pixel.y*output->width+pixel.x] > med)
						output->pixels[pixel.y*output->width+pixel.x] = med;
			}
		  }
        }
	}
/*
  int getRaysSize(LPPIXELSLIST *rays){
	 LPPIXELSLIST lpTmp;
	 int size = 0;
	 lpTmp = *rays;
	 while(lpTmp != NULL){
	 	 size++;lpTmp = lpTmp->next;
	 }
     return size;
  }
  int getRaysSize2(LPPIXELSLIST2 *rays){
	 LPPIXELSLIST2 lpTmp;
	 int size = 0;
	 lpTmp = *rays;
	 while(lpTmp != NULL){
	 	 size++;lpTmp = lpTmp->next;
	 }
     return size;
  }
*/
	void freeRays(LPPIXELSLIST *rays){
		LPPIXELSLIST lpTmp;
		int k;
		lpTmp = *rays;
		for (k = 0; k < totSizeRays; k++) {
			if(lpTmp[k].lpPixel != NULL){
				yl_free(lpTmp[k].lpPixel); lpTmp[k].lpPixel = NULL;
			}
		}
        yl_free(lpTmp);
	}



	void setNodesInit(NODE *ptrNodes, int w, int h){
		int x, y;
		for (y = 0; y < h; y++) {
//if(y%100==0) {printf("setNodesInit; y= %d, img->height= %d\n", y, h);fflush(stdout);}
			for (x = 0; x < w; x++) {
				ptrNodes[y*w+x].rank = -7;
			}
		}
	}
	LPPIXEL findPixelInForest(NODE *ptrNodes, int w, int h, LPPIXEL p) {
//		 LPNODE xNode;
		 int location = p->y*w+p->x;
		 if(ptrNodes[location].rank<0) return NULL;

		 if (pixelsSame(*p, ptrNodes[location].parent))
			 return p;

		 return findPixelInForest(ptrNodes,  w, h, &ptrNodes[location].parent);
	}

   LPPIXEL forestUnion(NODE *ptrNodes, int w, int h, LPPIXEL p1, LPPIXEL p2) {
				 LPPIXEL p1Root = findPixelInForest(ptrNodes, w, h, p1);
				 LPPIXEL p2Root = findPixelInForest(ptrNodes, w, h, p2);
//				 LPNODE xNode, yNode;
				 int locationx = p1->y*w+p1->x;
				 int locationy = p2->y*w+p2->x;

				 if (p1Root == NULL || p2Root == NULL || pixelsSame(*p1Root, *p2Root))
					return NULL;

				 if(ptrNodes[locationx].rank<0) return NULL;
				 if(ptrNodes[locationy].rank<0) return NULL;

				// x and y are not already in same set. Merge them.
				if (ptrNodes[locationx].rank < ptrNodes[locationy].rank) {
					setPixelSWT(&(ptrNodes[locationx].parent), p2Root);

					return p2Root;
				} else if (ptrNodes[locationx].rank > ptrNodes[locationy].rank) {
					setPixelSWT(&(ptrNodes[locationy].parent), p1Root);

					return p1Root;
				} else {
					setPixelSWT(&(ptrNodes[locationy].parent), p1Root);
					ptrNodes[locationx].rank++;

					return p1Root;
				}
			}

	LPPIXEL forestMakeSet(NODE *ptrNodes, int w, int h, LPPIXEL p, LPPIXEL pixelsTmp){
        int location = p->y*w+p->x;
		if(ptrNodes[location].rank>=0)
			return NULL;
		setPixelSWT(&ptrNodes[location].parent, p);ptrNodes[location].rank = 0;

//	    addPixelToList(p, pixelsTmp);
		setPixelSWT(&(pixelsTmp[totPixelsListForComponents]), p);
		totPixelsListForComponents++;
        return p;
	}
	void moveDataToNewBlock(LPPIXEL reprSetOut, LPPIXEL reprSetInp, int size){
		int i;
		for(i=1;i<=reprSetInp[0].x;i++){
			reprSetOut[i].x = reprSetInp[i].x;reprSetOut[i].y = reprSetInp[i].y;
		}
		reprSetOut[0].x = reprSetInp[0].x;
		reprSetOut[0].y = size;

	}

	LPPIXEL realocateBlock(LPPIXEL reprSet, int oneBlock){
		int size = reprSet[0].y+oneBlock;
		LPPIXEL reprSet2;
			reprSet2 = (LPPIXEL)yl_calloc("SWT25", size+1, sizeof(PIXEL));
			if(reprSet2 == NULL){
				printf("memory problem: reprSet\n"); return NULL;
			}
//printf("new size= %d\n", size);
         moveDataToNewBlock(reprSet2, reprSet, size);
//    printf("new idx= %d, new block= %d\n", reprSet2[0].x,reprSet2[0].y);
         yl_free(reprSet);
         return reprSet2;
	}

	void  getSubsets(NODE *ptrNodes, int w, int h, LPPIXEL pixelsTmp, PIXELSLIST *components) {
		LPPIXEL repr;
		h_status status;
		int i, lastIdx;
		LPPIXEL reprSet;
		PIXELW pW;
		int oneBlock = 200;
		int totSizeReprSets = 0;
		LPPIXEL reprSets;

		totSizeComponents = 0;

	    h_dictionary set = h_new_dictionary( DICT_TYPE_PIXEL, w*h);
	    h_set_object_copy_function(set, (h_object_copy_function) copyDocument);
	    reprSets = (LPPIXEL)yl_calloc("SWT25", w*h, sizeof(PIXEL));

	    	for(i=0;i<totPixelsListForComponents;i++){
	    	 repr = findPixelInForest(ptrNodes, w, h, &pixelsTmp[i]);
	    	 pW.x = repr->x;pW.y = repr->y;pW.w = w;
	    	 reprSet = h_get_object_for_key(set, &pW, 0);
	 		if(reprSet == NULL){
	 			reprSet = (LPPIXEL)yl_calloc("SWT25", oneBlock+1, sizeof(PIXEL));
	 			if(reprSet == NULL){
	 				printf("memory problem: reprSet\n"); return;
	 			}
	 			reprSet[0].x = 1;reprSet[0].y = oneBlock;
	 			reprSet[reprSet[0].x].x = pixelsTmp[i].x;reprSet[reprSet[0].x].y = pixelsTmp[i].y;
	 			reprSet[0].x++;
	 		    status = h_add_object_for_key(
	 		                set,
	 		                &pW,   //  Key
	 		                reprSet,  //  Object
	 		                &lastIdx
	 		                );
	              setPixelSWT(&(reprSets[totSizeReprSets]), repr);totSizeReprSets++;
	 		    if ( status != H_SUCCESS ){
	 		    	printf( "(ERROR in h_add_object_for_key. Error=%d\n", (int) status );
	 		    	return;
	 		    }
	 		}else{
	 			if(reprSet[0].x >= reprSet[0].y){
	 				reprSet= realocateBlock(reprSet, oneBlock);
		 			if(reprSet == NULL){
		 				printf("memory problem: reallock reprSet\n"); return;
		 			}
		 			h_update_object_for_key(set, &pW, 0, reprSet);
	 			}
	 			reprSet[reprSet[0].x].x = pixelsTmp[i].x;reprSet[reprSet[0].x].y = pixelsTmp[i].y;
	 			reprSet[0].x++;
	 		}
	    }
	 	  {int  k, size;
	    	for(i=0;i<totSizeReprSets;i++){
		    	 pW.x = reprSets[i].x;pW.y = reprSets[i].y;pW.w = w;
		    	 reprSet = h_get_object_for_key(set, &pW, 0);

		    	 size  = reprSet[0].x;
		 		 {LPPIXEL lpP;
			 	       components[i].lpPixel = (LPPIXEL)yl_calloc("SWT15", size, sizeof(PIXEL));
			 		   lpP = components[i].lpPixel;
			 			if(lpP == NULL){
			 				printf("memory problem: components[ %d ].lpPixel\n", i); return;
			 			}

			 		   for(k=0;k<size;k++){
			 			  setPixelSWT(&lpP[k], &(reprSet[k]));
			 		   }
			 		  components[i].size = size;
		 		}
		 		totSizeComponents++;
	    	}
	 	  }

	    	for(i=0;i<totSizeReprSets;i++){
		    	 pW.x = reprSets[i].x;pW.y = reprSets[i].y;pW.w = w;
		    	 reprSet = h_get_object_for_key(set, &pW, 0);
		    	 if(reprSet != NULL)
		    		 yl_free(reprSet);
	    	}
//		   status =  h_clean_dictionary(set);
		   status =  h_free_Fixeddictionary(set);
		   if ( status != H_SUCCESS ){
			    	printf( "(ERROR in h_add_object_for_key. Error=%d\n", (int) status );
			    	return;
		   }

         yl_free(reprSets);


	}
	void addPixelToList(LPPIXEL p,  LPPIXELS *pixelsTmp){
		LPPIXELS tmp, t;

		tmp= (LPPIXELS)yl_calloc("SWT2", 1, sizeof(PIXELS));
		tmp->p.x = p->x;tmp->p.y = p->y;tmp->next = NULL;
		if(*pixelsTmp == NULL){
			*pixelsTmp = tmp;
			return;
		}
		t = *pixelsTmp;
		while(t->next != NULL) t= t->next;
		t->next = tmp;

	}

	void findComponents(LPFImage img, PIXELSLIST *components ) {
		PIXEL current;
		PIXEL next;
//		int count = 0;
		int x, y, i;
		int xx, yy;
		float currentValue, value;
	    LPPIXEL pixelsTmp = NULL;
        NODE *ptrNodes;

        ptrNodes = (LPNODE)yl_calloc("SWT_NODELIST2", img->width*img->height, sizeof(NODE));
        if(ptrNodes == NULL) {
        	printf("ERROR memory problem: ptrNodes\n"); return;
        }
        pixelsTmp = (LPPIXEL)yl_calloc("SWT_NODELIST2", img->width*img->height, sizeof(PIXEL));
        if(pixelsTmp == NULL) {
        	printf("ERROR memory problem: pixelsTmp\n"); return;
        }

        totPixelsListForComponents = 0;

        setNodesInit(ptrNodes, img->width, img->height);
		for (y = 0; y < img->height; y++) {

//if(y%100==0) {printf("y= %d, img->height= %d\n", y, img->height);fflush(stdout);}

			for (x = 0; x < img->width; x++) {
				currentValue = img->pixels[y*img->width+x];
				if (currentValue > 0 && currentValue < POSITIVE_INFINITY) {
					current.x = x;
					current.y = y;
					forestMakeSet(ptrNodes, img->width, img->height, &current, pixelsTmp);

					for (i = 0; i < connect8L; i++) {
						xx = x + connect8[i][0];
						yy = y + connect8[i][1];

						if (xx >= 0 && xx < img->width - 1 && yy >= 0 && yy < img->height - 1) {
							 value = img->pixels[yy*img->width+xx];

							if (value > 0 && value < POSITIVE_INFINITY) {
								next.x = xx;
								next.y = yy;

								forestMakeSet(ptrNodes, img->width, img->height, &next, pixelsTmp);

								// if (Math.max(currentValue, value) /
								// Math.min(currentValue, value) < 3)
								forestUnion(ptrNodes, img->width, img->height, &current, &next);
							}
						}
					}

				}

			}
		}


		{
		 getSubsets(ptrNodes, img->width, img->height, pixelsTmp, components);
//		  printf("getSubsets\n");
//		  fflush(stdout);

		 yl_free(ptrNodes);
         yl_free(pixelsTmp);

		}


	}


	int pixelsSame(PIXEL x1, PIXEL x2){
		if(x1.x != x2.x) return 0;
		if(x1.y != x2.y) return 0;
		return 1;
	}
	LPPIXEL findPixelInForestJ(h_dictionary data, LPPIXEL x, int w) {
		 PIXELW pW;
		 LPNODE xNode;
		 pW.x = x->x;pW.y = x->y;pW.w = w;
		 xNode = h_get_object_for_key(data, &pW, 0);

		if (xNode == NULL)
			return NULL;

		if (pixelsSame(*x, xNode->parent))
			return x;

		xNode->parent = *findPixelInForestJ(data, &(xNode->parent),  w);

		return &(xNode->parent);
	}

   int getPixelsSize(LPPIXELS ps){
	   LPPIXELS t = ps;
	   int size = 0;
	   while(t!=NULL){
		   size++;
		   t = t->next;
	   }
	   return size;
   }

	void freePixels(LPPIXELS *ps){
	LPPIXELS t = *ps, t1;
	   while(t!=NULL){
		   t1 = t;
		   t = t->next;
		   yl_free(t1);
	   }
       *ps = NULL;
	}
void getMapValues_24(LPBYTE imgPtr, int biWidth, int biWidthPad, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void convert24BMPtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void ImageEdgeTest(LPIMGDATA imgInp,  int imgIdx, EDGEINFO* edgeInfo );
/*
void printTestYellow24BMP(LPBYTE ptrInp, int biWidthPadInp, int biHeightInp,
		                   int idxW, int idxH){
	int xn, yn;
	long nYInp, nXInp;
	int sh = 3;
	LPBYTE ptrI;

	int r, g, b;
	int drb, drg;
	int ret = 0;

	yn =idxH;
	nYInp=(biHeightInp-1-yn)*biWidthPadInp;
	xn = idxW;
	nXInp = xn*sh;
	ptrI = ptrInp+nYInp+nXInp;

	b =*ptrI;
	g =*(ptrI+1);
	r =*(ptrI+2);
	drb = r-b;
	drg = r-g;
//	{char buff[2000];
//       sprintf(buff,"h=%d, w= %d, r= %d, g= %d, b= %d, drb= %d, drg= %d\n",idxH, idxW, r, g, b, drb, drg);
//	   saveLogFile("", buff, "log_yellow.txt");
//	}
//	if(drb > 20 && drg<20 && r > 60 && b < 90) {
//		setWhitePTR(ptrI);

//	}
//    return ret;
}

void filterYellow_24BMP(LPIMGDATA img){

		int idxW, idxH;
		int biWidthInp, biWidthPadInp, biHeightInp;
		int flgHor;
		int dd, ret;

//char buff[500];
//		Red1Idx = 0; Red2Idx = 0;

		biWidthInp=img->bi.biWidth;
		biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
		biHeightInp = img->bi.biHeight;


		flgHor = 0;
	    if(biWidthInp > biHeightInp)  flgHor = 1;

//	    dd = biHeightInp/2;
		for(idxH=0;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<biWidthInp;idxW++){
				printTestYellow24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
//				ret = testRed24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
			}
		}


}
*/
void pre_ProcessColorImageBMP24Only(LPIMGDATA img, LPIMGDATA imgOut, int byteSteps, int US_state, EDGEINFO *lpEdgeInfo, int *iterOut){
		LPBYTE imgPtr = img->pixPtr;
		int    biWidthPad, biSizeImage;
		int *  lpImgMap_m;
		int *  lpImgMap_s;
		int    grid = 50;
		int    mapSize;
		float kFactor;
	    IMGDATA *imgOutTmp = NULL;
	    float kFhh;
	    int  tooDark, iter = 0;
	    int cVth, cHth, veryDark_th;
	//    int cVth2, cHth2;
	    EDGEINFO *edgeInfoTmp;
	//    int ccVPrev, ccHPrev;

//	    char imgName[100];
//	    char buff[2000];

		if(byteSteps <= 0) byteSteps = 1;
		biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
		biSizeImage = biWidthPad*img->bi.biHeight;
		mapSize = ((img->bi.biWidth/grid)+1)*((img->bi.biHeight/grid)+1);


		lpImgMap_m = yl_calloc("PP1", mapSize, sizeof(int));
		if(lpImgMap_m == NULL) {
			sprintf(img->error,"pre_ProcessColorImageBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, mapSize, strerror(errno));
			return;
		}
		lpImgMap_s = yl_calloc("PP2", mapSize, sizeof(int));
		if(lpImgMap_s == NULL) {
			sprintf(img->error,"pre_ProcessColorImageBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, mapSize, strerror(errno));
			return;
		}
		kFhh = 0.0;

	//    printf("img->bi.biBitCount= %d\n",img->bi.biBitCount);
	//    fflush(stdout);
//		filterYellow_24BMP(img);


		switch(img->bi.biBitCount) {
		case 24:
			getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);

	//		cVth = 300; cHth = 140;
	//		if(img->bi.biWidth > img->bi.biHeight) {
	//			cVth = 140; cHth = 300;
	//		}
		    cVth = 200; cHth = 40;
		    if(img->bi.biWidth > img->bi.biHeight) {
		    	cVth = 40; cHth = 200;
		    }
			veryDark_th = 1300;
            if(*iterOut < 0) {
		       iter = 0; kFhh = 0.1;kFactor = 0.0;
		       imgOutTmp = imgOut;edgeInfoTmp = lpEdgeInfo;
			   while(iter < 6) {
			     kFactor += kFhh;
//			 		printf("iter= %d\n", iter);
//			 		fflush(stdout);

			     if(imgOutTmp->ptr != NULL){

//			 		printf("yl_free(imgOutTmp->ptr)\n");
//			 		fflush(stdout);

		 	        yl_free(imgOutTmp->ptr);
		 	       imgOutTmp->ptr = NULL;
			     }
		         convert24BMPtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
			     tooDark = 0;
			     ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);

			     if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
			     if(edgeInfoTmp->ccV + edgeInfoTmp->ccH > veryDark_th) tooDark = -7;

//		       printf("24; 0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//		       fflush(stdout);
//         setNewImageName(imgName, img->szIMGName, iter);
//         saveBMPbuffer (imgOutTmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//         sprintf(buff,"24; 0;---iter= %d; kFactor= %e\n",iter, kFactor);
//	 	 saveLogFile("", buff, "log_SWT.txt");


			     if(tooDark <= 0) break;
		          iter++;
			   }
            }else{
           	  if(*iterOut < 3) {
 		         iter = *iterOut+2; kFhh = 0.1;
 		         kFactor = (iter+1)*kFhh;
           	  }else{
  		         iter = (*iterOut)/2; kFhh = 0.1;
  		         kFactor = (iter+1)*kFhh;
           	  }

		       imgOutTmp = imgOut;edgeInfoTmp = lpEdgeInfo;

//			     kFactor += kFhh;
			     if(imgOutTmp->ptr != NULL){
		 	        yl_free(imgOutTmp->ptr);
		 	       imgOutTmp->ptr = NULL;
			     }
		         convert24BMPtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
			     tooDark = 0;
			     ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);

			     if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
			     if(edgeInfoTmp->ccV + edgeInfoTmp->ccH > veryDark_th) tooDark = -7;

	//	       printf("24; 0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	//	       fflush(stdout);
//        setNewImageName(imgName, img->szIMGName, iter+70);
//        saveBMPbuffer (imgOutTmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//        sprintf(buff,"24; 0;---iter= %d; kFactor= %e\n",iter, kFactor);
//	 	 saveLogFile("", buff, "log_SWT.txt");
            }


			break;
		default:
			sprintf(img->error,"image type not supported yet \n");

			break;
		}
		*iterOut = iter;
		yl_free(lpImgMap_m);
		yl_free(lpImgMap_s);
		if(img->error[0] != 0){
			printf("img->error= %s\n",img->error);
			return;
		}

	//	imgOutTmp = imgOut;
	//   printf("imgOutTmp->bi.biWidth= %d; imgOutTmp->bi.biHeight= %d\n", imgOutTmp->bi.biWidth, imgOutTmp->bi.biHeight);

		if(imgOutTmp->bi.biWidth > imgOutTmp->bi.biHeight) {
	//		printf("rotate-------------------\n");
		    rotate(imgOutTmp, 90);
		}


	}


int ImageTest_Quality(LPIMGDATA img,  int byteSteps, LPIMGDATA imgOut){
	int retValue = -1;
//	IMGDATA imgOut;
	EDGEINFO edgeInfo;
	EDGEINFO *lpEdgeInfo;
	int iter = -1;

	char imgName[100];
//	char buff[2000];

//	   imgOut.ptr = NULL;
//	   strcpy(imgOut.szIMGName,img->szIMGName);


	  lpEdgeInfo = &edgeInfo;
//	  pre_ProcessColorImage_getImagesForOCR(img, &imgOut, byteSteps, US_state, lpEdgeInfo);
	  pre_ProcessColorImageBMP24Only(img, imgOut, byteSteps, US_State, lpEdgeInfo, &iter);
	  retValue = -1;

//		printf("pre_ProcessColorImageBMP24Only\n");
//		fflush(stdout);

	    setParams_StrokeWidthTransform(1, 2/* float sigmaInp */);

//	   setNewImageName(imgName, img->szIMGName, 7);
//	   saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


	    retValue = processImageSWT(imgOut);
//	    retValue = processImageSWT(img);

	    if(retValue < 0){

//     sprintf(buff,"1;---iter= %d, retValue= %d;\n",iter, retValue);
//     saveLogFile("", buff, "log_SWT.txt");

	  	  pre_ProcessColorImageBMP24Only(img, imgOut, byteSteps, US_State, lpEdgeInfo, &iter);
	  	  retValue = -1;
  	      setParams_StrokeWidthTransform(1, 2/* float sigmaInp */);
  	      retValue = processImageSWT(imgOut);

//     sprintf(buff,"2;---iter= %d, retValue= %d;\n",iter, retValue);
// 	 saveLogFile("", buff, "log_SWT.txt");

	  //	    retValue = processImageSWT(img);

	    }
//	 	if(imgOut.ptr != NULL) {
//	 	   yl_free(imgOut.ptr);
//	 	   imgOut.ptr = NULL;
//	 	}

	return retValue;
	}

	void setTestLetter(LPLETTER lett, LETT_RECT *r, float avb, float medStr, LPPIXEL pix){

	    lett[0].regularBoundingBox.x = r->x; lett[0].regularBoundingBox.y = r->y;lett[0].regularBoundingBox.width = r->width;lett[0].regularBoundingBox.height=r->height;
	    lett[0].averageBrightness = avb;lett[0].medianStrokeWidth = medStr;
	    lett[0].centroid.x = pix->x;lett[0].centroid.y = pix->y;lett[0].lpPixel = NULL;
	}
    void setTestLetters(LPLETTER lett){
    	LETT_RECT r;
    	PIXEL pix;


    	r.x=47.0; r.y=369.0; r.width=34.0; r.height=23.0; pix.x=63; pix.y=382;
    	setTestLetter(&lett[0], &r, 0.122621566, 7.615773, &pix);
    	r.x=218.0; r.y=201.0; r.width=28.0; r.height=25.0; pix.x=232; pix.y=214;
    	setTestLetter(&lett[1], &r, 0.22222222, 5.0990195, &pix);
    	r.x=249.0; r.y=1.0; r.width=18.0; r.height=19.0; pix.x=257; pix.y=10;
    	setTestLetter(&lett[2], &r, 0.08040201, 9.0, &pix);
    	r.x=849.0; r.y=150.0; r.width=23.0; r.height=26.0; pix.x=859; pix.y=162;
    	setTestLetter(&lett[3], &r, 0.2651515, 5.0990195, &pix);
    	r.x=1188.0; r.y=967.0; r.width=34.0; r.height=51.0; pix.x=1205; pix.y=990;
    	setTestLetter(&lett[4], &r, 0.092987806, 10.816654, &pix);
    	r.x=250.0; r.y=49.0; r.width=14.0; r.height=18.0; pix.x=256; pix.y=58;
    	setTestLetter(&lett[5], &r, 0.105882354, 9.219544, &pix);
    	r.x=1064.0; r.y=765.0; r.width=33.0; r.height=7.0; pix.x=1083; pix.y=769;
    	setTestLetter(&lett[6], &r, 0.294686, 4.0, &pix);
    	r.x=270.0; r.y=1.0; r.width=13.0; r.height=18.0; pix.x=277; pix.y=10;
    	setTestLetter(&lett[7], &r, 0.13636364, 6.4031243, &pix);
    	r.x=667.0; r.y=1508.0; r.width=6.0; r.height=40.0; pix.x=670; pix.y=1528;
    	setTestLetter(&lett[8], &r, 0.15611814, 4.0, &pix);
    	r.x=174.0; r.y=232.0; r.width=30.0; r.height=25.0; pix.x=190; pix.y=245;
    	setTestLetter(&lett[9], &r, 0.19502868, 5.3851647, &pix);
    	r.x=514.0; r.y=81.0; r.width=27.0; r.height=14.0; pix.x=528; pix.y=87;
    	setTestLetter(&lett[10], &r, 0.86627907, 3.6055512, &pix);
    	r.x=283.0; r.y=260.0; r.width=21.0; r.height=31.0; pix.x=291; pix.y=273;
    	setTestLetter(&lett[11], &r, 0.07788945, 7.28011, &pix);
    	r.x=986.0; r.y=1046.0; r.width=14.0; r.height=54.0; pix.x=996; pix.y=1069;
    	setTestLetter(&lett[12], &r, 0.17391305, 7.0, &pix);
    	r.x=404.0; r.y=155.0; r.width=19.0; r.height=12.0; pix.x=413; pix.y=160;
    	setTestLetter(&lett[13], &r, 0.17880794, 7.615773, &pix);
    	r.x=815.0; r.y=746.0; r.width=35.0; r.height=5.0; pix.x=832; pix.y=748;
    	setTestLetter(&lett[14], &r, 0.47428572, 4.0, &pix);
    	r.x=972.0; r.y=764.0; r.width=34.0; r.height=6.0; pix.x=990; pix.y=767;
    	setTestLetter(&lett[15], &r, 0.51111114, 4.0, &pix);
    	r.x=1185.0; r.y=306.0; r.width=23.0; r.height=31.0; pix.x=1196; pix.y=324;
    	setTestLetter(&lett[16], &r, 0.1045082, 7.2111025, &pix);
    	r.x=871.0; r.y=6.0; r.width=22.0; r.height=27.0; pix.x=882; pix.y=21;
    	setTestLetter(&lett[17], &r, 0.87677723, 3.1622777, &pix);
    	r.x=492.0; r.y=459.0; r.width=35.0; r.height=34.0; pix.x=509; pix.y=476;
    	setTestLetter(&lett[18], &r, 0.17474303, 5.656854, &pix);
    	r.x=136.0; r.y=230.0; r.width=29.0; r.height=26.0; pix.x=150; pix.y=244;
    	setTestLetter(&lett[19], &r, 0.17636684, 7.0, &pix);
    	r.x=942.0; r.y=474.0; r.width=57.0; r.height=46.0; pix.x=970; pix.y=499;
    	setTestLetter(&lett[20], &r, 0.108606555, 8.5440035, &pix);
    	r.x=756.0; r.y=284.0; r.width=21.0; r.height=33.0; pix.x=766; pix.y=301;
    	setTestLetter(&lett[21], &r, 0.11382114, 7.2111025, &pix);
    	r.x=44.0; r.y=99.0; r.width=61.0; r.height=24.0; pix.x=75; pix.y=108;
    	setTestLetter(&lett[22], &r, 0.16881444, 7.0, &pix);
    	r.x=140.0; r.y=198.0; r.width=30.0; r.height=25.0; pix.x=155; pix.y=210;
    	setTestLetter(&lett[23], &r, 0.15889464, 7.0, &pix);
    	r.x=537.0; r.y=501.0; r.width=34.0; r.height=35.0; pix.x=555; pix.y=519;
    	setTestLetter(&lett[24], &r, 0.124503314, 7.071068, &pix);
    	r.x=212.0; r.y=233.0; r.width=30.0; r.height=26.0; pix.x=228; pix.y=247;
    	setTestLetter(&lett[25], &r, 0.17857143, 6.0, &pix);
    	r.x=337.0; r.y=1437.0; r.width=58.0; r.height=115.0; pix.x=356; pix.y=1485;
    	setTestLetter(&lett[26], &r, 0.074699864, 11.7046995, &pix);
    	r.x=925.0; r.y=763.0; r.width=44.0; r.height=13.0; pix.x=948; pix.y=769;
    	setTestLetter(&lett[27], &r, 0.18895349, 6.4031243, &pix);
    	r.x=987.0; r.y=1010.0; r.width=12.0; r.height=26.0; pix.x=994; pix.y=1023;
    	setTestLetter(&lett[28], &r, 0.09663866, 6.3245554, &pix);
    	r.x=179.0; r.y=200.0; r.width=29.0; r.height=25.0; pix.x=194; pix.y=212;
    	setTestLetter(&lett[29], &r, 0.22954091, 5.3851647, &pix);
    	r.x=1162.0; r.y=304.0; r.width=18.0; r.height=31.0; pix.x=1169; pix.y=317;
    	setTestLetter(&lett[30], &r, 0.112391934, 7.0, &pix);
    	r.x=140.0; r.y=1.0; r.width=28.0; r.height=13.0; pix.x=155; pix.y=7;
    	setTestLetter(&lett[31], &r, 0.63461536, 3.0, &pix);
    	r.x=1015.0; r.y=765.0; r.width=33.0; r.height=6.0; pix.x=1032; pix.y=768;
    	setTestLetter(&lett[32], &r, 0.5265958, 4.0, &pix);
    	r.x=306.0; r.y=262.0; r.width=23.0; r.height=30.0; pix.x=319; pix.y=277;
    	setTestLetter(&lett[33], &r, 0.059101656, 8.485281, &pix);
    	r.x=570.0; r.y=66.0; r.width=18.0; r.height=20.0; pix.x=578; pix.y=75;
    	setTestLetter(&lett[34], &r, 0.9378882, 3.6055512, &pix);
    	totNumbLetters = 35;
    }

