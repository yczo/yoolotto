/*
 * PreProcessing.c
 *
 *  Created on: Sep 28, 2012
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include  "../headers/Functions.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"

void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
void getMapValues_24(LPBYTE imgPtr, int biWidth, int biWidthPad, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGBA(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGB565(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void convert24BMPtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertRGBAtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);

void convertRGB565To_mono_BMPLum_Next(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int idxW, int idxH, int grid, int gridW, int gridH, int tH);
void convertRGB565RedToWhite(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                     int idxW, int idxH, int grid, int gridW, int gridH, int th);


//void convertRGB565toMonochromeGridCA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid);
void cleanTicket(LPIMGDATA img, int US_state);
void cleanTicketCA(LPIMGDATA img);
void cleanTicketNY(LPIMGDATA img);
void smearing(LPIMGDATA imgInp, LPIMGDATA imgOut, int US_state);
//void createEdgeMonochrom(LPIMGDATA imgInp, LPIMGDATA img);
void createEdgeMonochrom(LPIMGDATA imgInp, LPIMGDATA img, int ddX, int ddY);
void addEdgeMonochrom(LPIMGDATA img, LPIMGDATA imgEdge);
void addEdgeMonochromCenter(LPIMGDATA img, LPIMGDATA imgEdge);
//int getRotationAngle(LPIMGDATA imgOut, LPTICKETDATA ticketData, int rotateId);
void saveAllLinesPtr(LPBYTE hpFieldHPtr, int width, int height);

void filterPARed24BMP(LPIMGDATA img, int grid, int side);
void filterNYRed24BMP(LPIMGDATA img,  int grid, int side);

void filterPARedRGB565(LPIMGDATA img, int grid, int side);
void filterPARedRGBA(LPIMGDATA img, int grid, int side);

void filterCARed24BMP(LPIMGDATA img, int grid);
void filterCARedRGB565(LPIMGDATA img, int grid);
void filterCARedRGBA(LPIMGDATA img, int grid);



void convertRGB565toMonochromeGrid2(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convert24BMPtoMonochromeGridSmall(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1);
void convertRGB565toMonochromeGridSmall(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1);
void convertRGBAtoMonochromeGridSmall_faster(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1);
void convert24BMPtoMonochromeGrid2_CA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR);
void convertRGBAtoMonochromeGrid2_CA_faster(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR);

void convertRGB565toMonochromeGrid2_CA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR);
void convertRGB565toMonochromeGrid_sOff(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);

void convertRGB565toMonochromeGridFinal(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor, float *kTab, int gridK);

void convertRGB565toMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertBMPTo_mono_BMP_init(LPIMGDATA img, LPIMGDATA imgOutp);
void convertBMPTo_mono_BMP_initSmall(LPIMGDATA img, LPIMGDATA imgOutp, int biWidthOut, int biHeightOut);
int byteMassXY(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2);
void filterOrangeRGBA(LPIMGDATA img, int grid);
void filterOrangeBMP(LPIMGDATA img, int grid);
void filterOrangeRGB565(LPIMGDATA img, int grid);

void filterRedLeftRGBA(LPIMGDATA img);
void filterRedLeftRGB565(LPIMGDATA img);
void filterRedLeft_24BMP(LPIMGDATA img);

int computeLuminosity(int r, int g, int b);

void TestImageAfterConversionCABMP(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid);
void TestImageAfterConversionCARGB565(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid);
void TestImageAfterConversionCARGBA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid);

int byteMassY(LPBYTE ptrPix, int widthImg, int hightImg, int y1, int x1, int x2);
int byteMass(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int y1, int y2);

void cleanSidesForScratchOff(LPIMGDATA imgOut);

void cutTopImage(LPIMGDATA img, int top);
void testForSmallImg(LPIMGDATA img);
//void testImageTH_L_R_RGBA(LPIMGDATA img,  int grid, float *kOutL_R);
//void testImageTH_L_R_RGB565(LPIMGDATA img,  int grid, float *kOutL_R);

//#define H_SIZE_PREPROC  2000
#define eRGB565_R  0xf800 ///< Mask for blue	(1111100000000000)
#define eRGB565_G  0x07e0 ///< Mask for green	(0000011111100000)
#define eRGB565_B  0x001f ///< Mask for red	(0000000000011111)
#define whiteRGB565  0xffff ///< Mask for white	(1111111111111111)

int hHistPreProc[H_SIZE_PREPROC];
int vHistPreProc[H_SIZE_PREPROC];
#define maxElemRed  300
int Red1Idx, Red2Idx;
int y1Red[maxElemRed], x1Red[maxElemRed], freq1Red[maxElemRed];
int y2Red[maxElemRed], x2Red[maxElemRed], freq2Red[maxElemRed];

//#define BYTES_STEP 1

//void saveLog(char *str1, char *str2);
//char buffLog[500];
void filterRedLeftBC_24BMP(LPIMGDATA img);
void JustSmearing(LPIMGDATA imgInp, LPIMGDATA imgOut, int pix);
void ImageEdgeTest(LPIMGDATA imgInp,  int imgIdx, EDGEINFO* edgeInfo );

BYTE getPixelCompress(LPBYTE ptr, int xn, int yn, int wPad, int height){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;
	BYTE b;

	yn=height-1-yn;
	nY=yn*wPad;
	nX = (long)(xn/8);
	ptrP = ptr+nY+nX;

	mask=LEFT_BIT;
	mask >>=(xn%8);

	b=(~(*ptrP))&mask;
	if(b!=0) {
		return(0);
	}
	else {
		return(1);
	}
}

void setPixelDecompress(LPBYTE ptr,  int wPad, int height, int x, int y, int color){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;

	y=height-1-y;
	nY=y*wPad;
	nX = (long)(x/8);
	ptrP = ptr+nY+nX;

	mask=LEFT_BIT;
	mask >>=(x%8);
	if(color!=0){
		*ptrP = *ptrP | mask;
	}
	else {
		*ptrP = *ptrP & (~mask);
	}
}
void setNewImageName(char *imgName, char *imgNInp, int loop){

	strcpy(imgName, imgNInp);
	imgName[strlen(imgName)-4] = 0;
	strcat(imgName, "_");
    sprintf(&imgName[strlen(imgName)], "%d",loop);
    strcat(imgName, ".bmp");

}

void pre_ProcessColorImageBMP(LPIMGDATA img, LPIMGDATA imgOut, int byteSteps, LPTICKETDATA ticketData, int US_state, int scratchOff){
	LPBYTE imgPtr = img->pixPtr;
	int    biWidthPad, biSizeImage;
	int *  lpImgMap_m;
	int *  lpImgMap_s;
	int    grid = 50;
	int    mapSize;
	IMGDATA imgOut2;
	float kFactor = 0.17;

//	  time_t mytime1, mytime2, mytime3;
//	  struct tm *mytm1;
//	  struct tm *mytm2;
//	  struct tm *mytm3;


//	float kFactorL_R[4];

//	int    rotAng;

//	    char   buff[500];


	if(byteSteps <= 0) byteSteps = 1;
	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biSizeImage = biWidthPad*img->bi.biHeight;
	mapSize = ((img->bi.biWidth/grid)+1)*((img->bi.biHeight/grid)+1);

//	printf("-------biWidthPad= %d, biSizeImage= %d, mapSize= %d \n", biWidthPad, biSizeImage, mapSize);

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

	switch(img->bi.biBitCount) {
	case 24:
		switch(US_state)
		{
		   case 0:kFactor = 0.15;break;
		   case 1:filterPARed24BMP(img,  grid, 0);break;
//		   case 2:filterCARed24BMP(img,  grid);kFactor = 0.20;break;
		   case 2:kFactor = 0.30;break;
		   case 3:kFactor = 0.25;break;
		}

//		getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps);
		switch(US_state){
		   case 2:
//			   filterOrangeBMP(img, grid);
				getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 1);
			   TestImageAfterConversionCABMP(img, imgOut, lpImgMap_m, lpImgMap_s, grid);
			   break;
		   case 3:
				getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
				filterRedLeft_24BMP(img);
				convert24BMPtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   break;

		   default:
				getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			    convert24BMPtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
                break;
		}
//		getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps);
//	    convert24BMPtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

		break;
	case 32:
		switch(US_state)
		{
		   case 0:kFactor = 0.15;break;
		   case 1:filterPARedRGBA(img,  grid, 0);break;
	//	   case 2:filterCARedRGBA(img,  grid);kFactor = 0.20;break;
		   case 2:kFactor = 0.30;break;
		   case 3:kFactor = 0.25;break;
//		   case 3:filterPARedRGBA(img,  grid, 1);break;
		}



//		getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps);
		kFactor = 0.25;
		switch(US_state){
		   case 2:
				getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 1);
			    TestImageAfterConversionCARGBA(img, imgOut, lpImgMap_m, lpImgMap_s, grid);
		       break;
		   case 3:
			   getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   filterRedLeftRGBA(img);
			   convertRGBAtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   break;
		   default:
			   getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
//			   filterPARedRGBA(img,  grid, 1);
//			   filterRedLeftRGBA(img);
			   convertRGBAtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
                break;
		}

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");


		break;
	case 565:
		switch(US_state)
		{
		   case 0:kFactor = 0.17;break;
		   case 1:filterPARedRGB565(img,  grid, 0);break;
//		   case 2:filterCARedRGB565(img,  grid);kFactor = 0.45;break;
		   case 2:kFactor = 0.45;break;
		   case 3:kFactor = 0.35;break;
//		   case 3:filterPARedRGB565(img,  grid, 1);break;
		}

//		getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps);
		switch(US_state){
		   case 2:
				getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 1);
//			   filterOrangeRGB565(img, grid);
			   TestImageAfterConversionCARGB565(img, imgOut, lpImgMap_m, lpImgMap_s, grid);
			   break;
		   case 3:
			   getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   filterRedLeftRGB565(img);
			   convertRGB565toMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   break;

		   default:
//			    kFactorL_R[0] = kFactor; kFactorL_R[1] = kFactor; kFactorL_R[2] = kFactor; kFactorL_R[3] = kFactor;
			   getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   if(scratchOff == 0)
			       convertRGB565toMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   else{
				   kFactor = 0.08;
				   convertRGB565toMonochromeGrid_sOff(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   }
                break;
		}

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

//	    saveBMPbuffer (imgOut->ptr, "addEdge.bmp", imgOut->szOutPath);
		break;

	default:
		sprintf(img->error,"image type not supported yet \n");

		break;
	}

	yl_free(lpImgMap_m);
	yl_free(lpImgMap_s);
	if(img->error[0] != 0)
		return;

//    if(imgOut->ptr == NULL)
//        printf("\n pre_   imgOut == NULL\n");
//    else
//        printf("\n pre_   imgOut != NULL\n");

	if(imgOut->bi.biWidth > imgOut->bi.biHeight) {
//		rotAng = getRotationAngle(imgOut, ticketData, 1);
//		rotate(imgOut, rotAng);

		rotate(imgOut, 90);
	}

if(scratchOff == 0) {

   switch(US_state){
   case 0:
   case 1: cleanTicket(imgOut, US_state);break;
//   case 2: cleanTicketCA(imgOut);break;
//   case 2: cleanTicket(imgOut, US_state);break;
//   case 3: cleanTicketNY(imgOut);break;

   }
//}

	  smearing(imgOut, &imgOut2, US_state);

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

//	saveBMPbuffer (imgOut2.ptr, "gradient.bmp", imgOut->szOutPath);
	if(US_state != 2) {
	   createEdgeMonochrom(imgOut, &imgOut2, 20, 50);
	   addEdgeMonochrom(imgOut, &imgOut2);
	}
	else{
		createEdgeMonochrom(imgOut, &imgOut2, 250, 250);
		addEdgeMonochromCenter(imgOut, &imgOut2);
	}

	if(US_state == 3) {
		testForSmallImg(imgOut);
//		cutTopImage(imgOut,  400);
//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");
	}


	if(imgOut2.ptr != NULL) {
		yl_free(imgOut2.ptr); // memory leak
		imgOut2.ptr = NULL;
	}
  }
/*
else{

//	cleanSidesForScratchOff(imgOut);
//	 saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

  }
*/
  kFactor = biSizeImage; // to avoid compiler warning
}

int getLuminosity_RGBA(LPBYTE ptrI){
	int r, g, b;

    g =*(ptrI+1);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
    r =*ptrI;
    b =*(ptrI+2);
#else
   b =*ptrI;
   r =*(ptrI+2);
#endif
   return(computeLuminosity(r, g, b));
}

int darknessTest2(LPIMGDATA imgOut, int mode) {
	int biWidthPadOut, biHeightOut;
	int x1, x2, y1, y2, i;
	float mass[4];


	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	x1 = 0;
	x2 = 200;
	y1 = 0;
	y2 = 200;
	if(x2>= imgOut->bi.biWidth) x2 = imgOut->bi.biWidth-1;
	if(y2>= biHeightOut) y2 = biHeightOut-1;
	mass[0] = byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, y1, y2);

	x1 = imgOut->bi.biWidth-201;
	x2 = imgOut->bi.biWidth-1;
	y1 = 0;
	y2 = 200;
	if(x1< 0) x1 = 0;
	if(y2>= biHeightOut) y2 = biHeightOut-1;
	mass[1] = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, y1, y2);

	x1 = imgOut->bi.biWidth-201;
	x2 = imgOut->bi.biWidth-1;
	y1 = biHeightOut-201;
	y2 = biHeightOut-1;
	if(x1< 0) x1 = 0;
	if(y1< 0) y1 = 0;
	mass[2] = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, y1, y2);

	x1 = 0;
	x2 = 200;
	y1 = biHeightOut-201;
	y2 = biHeightOut-1;
	if(x2>= imgOut->bi.biWidth) x2 = imgOut->bi.biWidth-1;
	if(y1< 0) y1 = 0;
	mass[3] = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, y1, y2);

	for(i=0;i<4;i++)
		mass[i] /= (float)(200*200);
//printf(" mass[0]= %e, mass[1]= %e, mass[2]= %e, mass[3]= %e\n",mass[0],mass[1],mass[2],mass[3]);
	if(mode == 0) {
	  for(i=0;i<4;i++){
           if(mass[i]> 0.1) return 1;
	  }
	}
	if(mode > 0) {
	  for(i=0;i<4;i++){
           if(mass[i]> 0.01) return 1;
	  }
	}

    return 0;
}
float getNextDataForMonoLocalTH2(LPBYTE pixPtr, int biWidth, int biHeight, int x, int pixX, int y, int pixY, int flgHor);
void setWhitePixelsX(LPBYTE ptr, int w, int h, int x1, int x2, int x12, int x22, int y1, int y2);
void cleanBlackSide(LPIMGDATA imgOut, int k1, int k2){
	int biWidthPadOut, biHeightOut;

	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	if(imgOut->bi.biWidth > imgOut->bi.biHeight){
		setWhitePixelsX(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, imgOut->bi.biWidth-1, 0, imgOut->bi.biWidth-1, k1, k2);
	}else{
		setWhitePixelsX(imgOut->pixPtr, biWidthPadOut, biHeightOut, k1, k2, k1, k2, 0, biHeightOut-1);
	}
}

void getHistogramForBlack(LPIMGDATA imgOut, float *hist) {
	int biWidthPadOut, biHeightOut;
	int x1, x2, y1, y2;
	int xC, yC;
	int loop;


	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	xC = (imgOut->bi.biWidth*imgOut->bi.biBitCount)/2;
	yC = imgOut->bi.biHeight/2;

	if(imgOut->bi.biWidth > imgOut->bi.biHeight){
	      x1 = xC-xC/2;
		  x2 = xC+xC/2;
		  y1 = 0;
		  y2 = imgOut->bi.biHeight-1;
//		  histHTest_X1_X1(imgOut->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1, y2, hist);
		  for(loop= y1;loop<y2;loop++){
			  if(loop <y1+350 || loop > y2-350)
                 hist[loop+1] = getNextDataForMonoLocalTH2(imgOut->pixPtr, imgOut->bi.biWidth, biHeightOut, x1, x2-x1, loop, 1, 1);
			  else
				  hist[loop+1] = 0;
		  }
		  hist[0] = loop;
	}else{
		  x1 = 0;
		  x2 = imgOut->bi.biWidth-1;
		  y1 = yC-yC/2;
		  y2 = yC+yC/2;
//		 histVPtrPix(imgOut->pixPtr, biWidthPadOut, biHeightOut, H_SIZE_PREPROC, x1, x2, y1, y2, hist);
		  for(loop= x1;loop<x2;loop++){
			  if(loop <x1+350 || loop > x2-350)
                 hist[loop+1] = getNextDataForMonoLocalTH2(imgOut->pixPtr, imgOut->bi.biWidth, biHeightOut, loop, 1, y1, y2-y1, 0);
			  else
				  hist[loop+1] = 0;
		  }
		  hist[0] = loop;

	}
	loop = biWidthPadOut; // to avoid compiler warning
}


float getNextDataForMonoLocalTH2(LPBYTE pixPtr, int biWidth, int biHeight, int x, int pixX, int y, int pixY, int flgHor){
	int iX, iY;
	int xn, yn;
	float tD1;
	int L1, L2, L3;
	int itD1;
	int cc;
	int wPad = ((biWidth+31)/32)*4;


//	y = idxH*grid;
//	x = idxW*grid;

	tD1 = 0;
	cc = 0;
	itD1 = 0;
	for(iY=0;iY<pixY;iY++){
		yn =y+iY;
		for(iX=0;iX<pixX;iX++){
			xn = x+iX;
			L1 = getPixelCompress(pixPtr, xn, yn, wPad, biHeight);
			L2 = L1;
			if(flgHor==1) {
				if(iX+2 < pixX) {
			      L2 = getPixelCompress(pixPtr, xn+1, yn, wPad, biHeight);
			      L3 = getPixelCompress(pixPtr, xn+2, yn, wPad, biHeight);
				}
			}
			else{
				if(iY+2 < pixY) {
				  L2 = getPixelCompress(pixPtr, xn, yn+1, wPad, biHeight);
				  L3 = getPixelCompress(pixPtr, xn, yn+2, wPad, biHeight);
				}
			}

			if(L2-L1 != 0 && L3-L2 != 0) itD1++;
			cc++;
		}
	}
	if(cc>0)
		tD1 = (float)itD1/(float)cc;
   return tD1;
}
float getNextDataForMonoLocalTHSmall(LPBYTE pixPtr, int biWidth, int biHeight, int flgHor){
	int iX, iY;
	int xn, yn;
	float tD1;
	int L1, L2, L3;
	int itD1;
	int cc;
	int wPad = ((biWidth+31)/32)*4;


	tD1 = 0;
	cc = 0;
	itD1 = 0;
	for(iY=0;iY<biHeight-2;iY++){
		yn =iY;
		for(iX=0;iX<biWidth-2;iX++){
			xn = iX;
			L1 = getPixelCompress(pixPtr, xn, yn, wPad, biHeight);
			L2 = L1;
			if(flgHor==1) {
			      L2 = getPixelCompress(pixPtr, xn+1, yn, wPad, biHeight);
			      L3 = getPixelCompress(pixPtr, xn+2, yn, wPad, biHeight);
			}
			else{
				  L2 = getPixelCompress(pixPtr, xn, yn+1, wPad, biHeight);
				  L3 = getPixelCompress(pixPtr, xn, yn+2, wPad, biHeight);
			}

			if(L2-L1 != 0 && L3-L2 != 0) itD1++;
			cc++;
		}
	}
	if(cc>0)
		tD1 = (float)itD1/(float)cc;
   return tD1;
}

void scaleTD(float *tD, int n){
	float max = 0;
	int i;
	for(i=0;i<n;i++){
		if(max < tD[i]) max = tD[i];
	}
	if(max > 0) {
	   for(i=0;i<n;i++){
		   tD[i] = tD[i]/max;
	   }
	}
}
void saveTD(float *tD, int n, char *szIMGName, char *suff){
	char fileName[500];
    FILE *f;
    char buff[200];
    int i;

	strcpy(fileName, "c:\\Users\\Wicek\\OCR\\output\\");
	strcat(fileName, szIMGName);
	fileName[strlen(fileName)-4] = 0;
	strcat(fileName, "_");
	strcat(fileName, suff);
    strcat(fileName, ".csv");

    f = fopen(fileName, "a");
    if(f == NULL) {
    	printf("savelog (\"%s\") failed: %s\n",fileName, strerror(errno));
    	return;
    }

    strcpy(buff, "loop,TD\n");
    fprintf (f, "%s", buff);

	for(i=0;i<n;i++){
		sprintf(buff,"%i,%e\n",i,tD[i]);
		fprintf (f, "%s", buff);
	}
   fclose (f);
}
void calculateKbordersRGBASmall(float *tD0, int n, float mass, int *k){
	int i;
    float dd_0, max;
    int iMax, start;
	*k = 0;
	if(n<2) return;

//printf("  mass= %e\n", mass);
    max = 0;iMax = tD0[0];
	for(i=1;i<n-1;i++){
		if(max < tD0[i]) {max = tD0[i];iMax = i;}
	}
	start = 0;
    if(iMax < 3) start = iMax;

	for(i=start;i<n-1;i++){
		dd_0 = tD0[i]-tD0[i+1];
//		printf(" i= %d, dd0= %e, \n",i,dd_0);
		if(dd_0 < 0.001 &&  mass > 0.05) {*k = i; return;}
//		printf("2 i= %d, dd0= %e, \n",i,dd_0);
	}
	for(i=start;i<n-2;i++){
		dd_0 = tD0[i]-tD0[i+1];
//		printf(" i= %d, dd0= %e, \n",i,dd_0);
		if(dd_0 < 0.03) {*k = i; return;}
//		printf("2 i= %d, dd0= %e, \n",i,dd_0);
	}
	i = n-2;
	dd_0 = tD0[i]-tD0[i+1];
//printf(" i= %d, dd0= %e, mass= %e\n",i,dd_0, mass);
	if(dd_0 < 0.025 && mass > 0.05) {*k = i;return;}
	if(dd_0 < 0.035 && mass > 0.07) *k = i;

//	printf(" k= %d \n",k);
}

void calculateKbordersRGBASmall_2(float *tD0, int n, int *k){
	int i;
    float dd_0 ;
	*k = 0;
	if(n<2) return;
	for(i=0;i<n-1;i++){
		dd_0 = tD0[i]-tD0[i+1];
//		printf(" i= %d, dd0= %e, \n",i,dd_0);
		if(dd_0 < 0.0015) {*k = i; return;}
//		printf("2 i= %d, dd0= %e, \n",i,dd_0);
	}
}

void setDPI(LPIMGDATA img, LPIMGDATA imgOut){
//	LPBITMAPINFOHEADER bi;

//	   imgOut->bi.biXPelsPerMeter=400;
//	   imgOut->bi.biYPelsPerMeter=400;
	   convertDPI_in_BMP(imgOut, 300, 300);
/*
		bf = (LPBITMAPFILEHEADER)(imgOut->ptr);
		bi = (LPBITMAPINFOHEADER)(imgOut->ptr + sizeof (BITMAPFILEHEADER));



		set4Bytes(bf->bfSize, imgOut->bf.bfSize);

		set4Bytes(bi->biWidth, imgOut->bi.biWidth);
		set4Bytes(bi->biHeight, imgOut->bi.biHeight);

		set4Bytes(bi->biSizeImage, imgOut->bi.biSizeImage);
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
*/

//   if( 1 || imgOut->bi.biWidth > 1400 && imgOut->bi.biHeight > 1400)
/*
   if( imgOut->bi.biWidth > 1400 && imgOut->bi.biHeight > 5400)
	   convertDPI_in_BMP(imgOut, 300, 300);
   else{
	   imgOut->bi.biXPelsPerMeter=300;
	   imgOut->bi.biYPelsPerMeter=300;
	   bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
	   set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
	   set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
}
*/
}
void TestImageAfterConversionCABMP(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid){
   int biWidthInp, biHeightInp;
   int biWidthOut, biHeightOut, biWidthPadOut;
   int loop, loopTimes = 5;
   float tD_0[loopTimes*2];

   float mass = 0, td0;
   float massL, massR, ratio;
   float k1,  kG_FinalL, kG_FinalR;
   int ik1, ikG_Final;
   float kTab[loopTimes*2];
   int flgBlack;
   float startLoop = 0.1;
   float kF = startLoop;
	int flgHor;
   int xC_1, xC_2;
   int yC_1, yC_2;
   float div;

   int shiftX = 250;
   int shiftY = 200;


//   char imgName[100];


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;
	flgHor = 0;
    if(biWidthInp > biHeightInp)  flgHor = 1;

// right side

     xC_1 = biWidthInp/2 - biWidthInp/6;
     xC_2 = biWidthInp/2 + biWidthInp/6;
     yC_1 = biHeightInp/2 - biHeightInp/7;
     yC_2 = biHeightInp/2 + biHeightInp/7;

     if(flgHor == 1) {
         xC_1 += shiftY;xC_2 += shiftY;
         yC_1 -= shiftX;yC_2 -= shiftX;
     }else{
    	  xC_1 += shiftX;xC_2 += shiftX;
	      yC_1 += shiftY;  yC_2 += shiftY;
    }

  	    	    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
  	    	    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
  	    	    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
  	    	    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;

  	    		biWidthOut  = xC_2- xC_1+1;
  	    		biHeightOut = yC_2- yC_1+1;

	convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;


	{
		div = biWidthOut*biHeightOut;
		kF = startLoop;


	        if(biWidthInp > biHeightInp)  flgHor = 1;
		    for(loop=0;loop<loopTimes;loop++){
		    	kTab[loop] = kF;
		    	convert24BMPtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
	                tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);
	                if(loop == loopTimes-2)
	       	                mass = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

		    	kF += 0.15;
//		    	setNewImageName(imgName, img->szIMGName, loop);
//		    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
		    }

    }
//	printf("tD_0[0]= %e\n",tD_0[0]);
    td0 = tD_0[0];

          scaleTD(tD_0, loopTimes);
//          saveTD(tD_0, loopTimes, img->szIMGName, "00");

          calculateKbordersRGBASmall(tD_0, loopTimes, mass, &ik1);
          if(ik1 < 2 && td0 > 0.0032) ik1++;
          k1 = kTab[ik1];
          if(ik1 > 0)
              k1 -= 0.1;
          if(k1 < 0) k1 = 0.5;

// 	printf("ik1= %d, k1=%e\n",ik1, k1);


        kF = k1;
        ikG_Final = -1;
    	loop = 0;
    	while(loop < loopTimes*2-1){
	    	kTab[loop] = kF;
//   	printf("kF= %e\n",kF);
	    	convert24BMPtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
             tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);

	    	kF += 0.05;
//		    	setNewImageName(imgName, img->szIMGName, loop);
//		    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	    	loop++;
	    }
      	td0 = tD_0[0];
          scaleTD(tD_0, loop);
//	              saveTD(tD_0, loop, img->szIMGName, "L");
          calculateKbordersRGBASmall_2(tD_0, loop, &ikG_Final);

//	           	printf("ikG_Final=%d\n",ikG_Final);
          if(ikG_Final < 2 && td0 > 0.004) ikG_Final++;
       	  kG_FinalR = kTab[ikG_Final];
//     	printf("ikG_Final=%d\n",ikG_Final);
//     	printf("kG_Final=%e\n",kG_Final);

// left side
    	convert24BMPtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
        massR = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

       		       xC_1 = biWidthInp/2 - biWidthInp/6;
       		       xC_2 = biWidthInp/2 + biWidthInp/6;
       		       yC_1 = biHeightInp/2 - biHeightInp/7;
       		       yC_2 = biHeightInp/2 + biHeightInp/7;

       		    if(flgHor == 1) {
       		    	xC_1 += shiftY;xC_2 += shiftY;
       		    	yC_1 += shiftX;yC_2 += shiftX;
       		    }else{
       		    	xC_1 -= shiftX;xC_2 -= shiftX;
       		    	yC_1 += shiftY;yC_2 += shiftY;
       		    }
       		    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
       		    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
       		    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
       		    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;
       			biWidthOut  = xC_2- xC_1+1;
       			biHeightOut = yC_2- yC_1+1;

       			div = biWidthOut*biHeightOut;

       		   	if(imgOut->ptr != NULL) {
       		    		yl_free(imgOut->ptr); // memory leak
       		   		  imgOut->ptr = NULL;
       		   	}
       			convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
       			biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;

       	    	convert24BMPtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
       	        massL = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

       	//      	setNewImageName(imgName, img->szIMGName, 44);
       	//    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

       	        ratio = massL/massR;
       	//                printf("---------ratio=%e\n",massL/massR);
       	        kG_FinalL = kG_FinalR;
       	        if(kG_FinalR < 0.6) {
       	           if(ratio> 1.15) kG_FinalL = kG_FinalR + 0.1;
       	           if(ratio> 1.4) kG_FinalL = kG_FinalR + 0.2;
       	        }

       	//                printf("---------kG_FinalL=%e, kG_FinalR = %e\n",kG_FinalL, kG_FinalR);


   	if(imgOut->ptr != NULL) {
    		yl_free(imgOut->ptr); // memory leak
   		  imgOut->ptr = NULL;
   	}
  convertBMPTo_mono_BMP_init(img, imgOut);
  convert24BMPtoMonochromeGrid2_CA(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalL, kG_FinalR);

 flgBlack= 0;
 flgBlack = darknessTest2(imgOut, 1);
// printf("flgBlack= %d\n",flgBlack);

	if(flgBlack > 0) {
// cleaning black
		 float hist[5000];
		 float av;
		 int kCut;
//		 char buff[500];
		 int kk, kkC, flg, j;
		 getHistogramForBlack(imgOut, hist);

		 for(kk=1;kk<hist[0]-4;kk++){
			 av = (hist[kk]+hist[kk+1]+hist[kk+2]+hist[kk+3])/4;
			 hist[kk] = av;

//			 sprintf(buff," hist[ %d ]= %e\n",kk, hist[kk]);
//			 saveLog("-----------------------------------", buff);
		 }
//		 kkC = hist[0]/2;
//		 kkC3 = kkC/3;
		 flg = 0;
		 for(kk=0;kk<100;kk++){
			 if(hist[kk] < 0.00000001) flg++;
			 else flg = 0;
			 if(flg > 20) break;
		 }
		 if(flg < 20) {
		    kkC = 0;
		    if(hist[0] > 300) kkC = 300;
		    for(kk=kkC;kk>16;kk--){
			 if(hist[kk] > 0.00999) {
				 flg = 0;
				 for(j=0;j<15;j++){
					 if(hist[kk-j] < 0.00999) {flg= 1;break;}
				 }
				 if(flg == 0) {
					 kCut = kk; if(kCut < 300 ) kCut += 22;
					 cleanBlackSide(imgOut, 0, kCut);break;
				 }
				 kk -= j;
			 }
		   }
		 }
		 flg = 0;
		 for(kk=hist[0];kk>hist[0]-100;kk--){
			 if(hist[kk] < 0.00000001) flg++;
			 else flg = 0;
			 if(flg > 20) break;
		 }
		 if(flg < 20) {
		    kkC = hist[0];
		    if(hist[0] > 300) kkC = hist[0]-300;
		    for(kk=kkC;kk<hist[0]-16;kk++){
			 if(hist[kk] > 0.00999) {
				 flg = 0;
				 for(j=0;j<15;j++){
					 if(hist[kk+j] < 0.00999) {flg= 1;break;}
				 }

//printf(" kk= %d, flg= %d, kkC= %d, kkC3= %d\n",kk, flg, kkC, kkC3);
				 if(flg == 0) {
					 kCut = kk; if(hist[0]-kCut < 300 ) kCut -= 22;
					 cleanBlackSide(imgOut, kCut, hist[0]);break;
				 }
				 kk += j;
			 }
		    }
		 }

}

 setDPI(img, imgOut);
//setNewImageName(imgName, img->szIMGName, 0);
//saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
}


void TestImageAfterConversionCARGB565(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid){
	 int biWidthInp, biHeightInp;
	 int biWidthOut, biHeightOut, biWidthPadOut;
	 int loop, loopTimes = 5;
	 float tD_0[loopTimes*2];

	 float mass = 0, td0;
	 float massL, massR, ratio;
	 float k1,  kG_FinalL, kG_FinalR;
	 int ik1, ikG_Final;
	 float kTab[loopTimes*2];
	 int flgBlack;
	 float startLoop = 0.1;
	 float kF = startLoop;
	 int flgHor;
	 int xC_1, xC_2;
	 int yC_1, yC_2;
	 float div;

	 int shiftX = 250;
	 int shiftY = 200;

//	 char imgName[100];



		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;
		flgHor = 0;
	    if(biWidthInp > biHeightInp)  flgHor = 1;


 // right side

	    	       xC_1 = biWidthInp/2 - biWidthInp/6;
	    	       xC_2 = biWidthInp/2 + biWidthInp/6;
	    	       yC_1 = biHeightInp/2 - biHeightInp/7;
	    	       yC_2 = biHeightInp/2 + biHeightInp/7;

	    		    if(flgHor == 1) {
	    		       xC_1 += shiftY;xC_2 += shiftY;
	    		       yC_1 -= shiftX;yC_2 -= shiftX;
	    		    }else{
	    		    	xC_1 += shiftX;xC_2 += shiftX;
	    		        yC_1 += shiftY;  yC_2 += shiftY;
	    		    }

	    	    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
	    	    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
	    	    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
	    	    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;

	    		biWidthOut  = xC_2- xC_1+1;
	    		biHeightOut = yC_2- yC_1+1;

//	    printf(" biWidthOut= %d, biHeightOut= %d\n",biWidthOut, biHeightOut);

	    		convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
	    		biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;

	    		{
	    			div = biWidthOut*biHeightOut;
	    			kF = startLoop;
	    		        if(biWidthInp > biHeightInp)  flgHor = 1;
	    			    for(loop=0;loop<loopTimes;loop++){
	    			    	kTab[loop] = kF;
	    			    	convertRGB565toMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
	    		                tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);
	    		                if(loop == loopTimes-2)
	    		       	                mass = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

	    			    	kF += 0.15;
//	    			    	setNewImageName(imgName, img->szIMGName, loop);
//	    			    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	    			    }

	    	    }
//	    		printf("tD_0[0]= %e\n",tD_0[0]);
	    		      td0 = tD_0[0];
	    	          scaleTD(tD_0, loopTimes);
//	    	          saveTD(tD_0, loopTimes, img->szIMGName, "RR");

	    	          calculateKbordersRGBASmall(tD_0, loopTimes, mass, &ik1);
//    	          printf("ik1= %d\n",ik1);
	    	          if(ik1 < 2 && td0 > 0.0032) ik1++;
//	    	          if(ik1 < 2 && td0 > 0.006) ik1++;
	    	          k1 = kTab[ik1];
	    	          if(ik1 > 0)
	    	              k1 -= 0.1;
	    	          if(k1 < 0) k1 = 0.5;
//	              printf("right ik1= %d, k1= %e\n",ik1, k1);



	            kF = k1;
	            ikG_Final = -1;
	          	loop = 0;
	          	while(loop < loopTimes*2-1){
	      	    	kTab[loop] = kF;
	      	    	convertRGB565toMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
	                 tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);

	      	    	kF += 0.05;
//	      		    	setNewImageName(imgName, img->szIMGName, loop);
//	      		    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	      	    	loop++;
	      	    }
//	          	printf("tD_0[0]= %e\n",tD_0[0]);
	          	td0 = tD_0[0];
	              scaleTD(tD_0, loop);
//	              saveTD(tD_0, loop, img->szIMGName, "L");
	              calculateKbordersRGBASmall_2(tD_0, loop, &ikG_Final);


//	           	printf("ikG_Final=%d\n",ikG_Final);
//	           	if(ikG_Final < 3) ikG_Final++;
    	          if(ikG_Final < 2 && td0 > 0.004) ikG_Final++;
//    	          if(ikG_Final < 2 && td0 > 0.006) ikG_Final++;
	           	kG_FinalR = kTab[ikG_Final];
//	           	printf("ikG_Final=%d, kG_FinalL=%e\n",ikG_Final,kG_FinalR);

// left side
		    	convertRGB565toMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
                massR = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

	    	       xC_1 = biWidthInp/2 - biWidthInp/6;
	    	       xC_2 = biWidthInp/2 + biWidthInp/6;
	    	       yC_1 = biHeightInp/2 - biHeightInp/7;
	    	       yC_2 = biHeightInp/2 + biHeightInp/7;

	    	    if(flgHor == 1) {
	    	    	xC_1 += shiftY;xC_2 += shiftY;
	    	    	yC_1 += shiftX;yC_2 += shiftX;
	    	    }else{
	    	    	xC_1 -= shiftX;xC_2 -= shiftX;
	    	    	yC_1 += shiftY;yC_2 += shiftY;
	    	    }
	    	    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
	    	    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
	    	    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
	    	    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;
	    		biWidthOut  = xC_2- xC_1+1;
	    		biHeightOut = yC_2- yC_1+1;

	    	   	if(imgOut->ptr != NULL) {
	    	    		yl_free(imgOut->ptr); // memory leak
	    	   		  imgOut->ptr = NULL;
	    	   	}
	    		convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
	    		biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;

	    		div = biWidthOut*biHeightOut;
		    	convertRGB565toMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
                massL = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

                ratio = massL/massR;
//                printf("---------ratio=%e\n",massL/massR);
                kG_FinalL = kG_FinalR;
                if(kG_FinalR < 0.6) {
                   if(ratio> 1.15) kG_FinalL = kG_FinalR + 0.1;
                   if(ratio> 1.4) kG_FinalL = kG_FinalR + 0.2;
//                   if(ratio> 1.8) kG_FinalL = kG_FinalR + 0.24;
//                   if(ratio> 2) kG_FinalL = kG_FinalR + 0.2;
                }

//                printf("---------kG_FinalL=%e, kG_FinalR = %e\n",kG_FinalL, kG_FinalR);




     	if(imgOut->ptr != NULL) {
      		yl_free(imgOut->ptr); // memory leak
     		  imgOut->ptr = NULL;
     	}
          convertBMPTo_mono_BMP_init(img, imgOut);
          convertRGB565toMonochromeGrid2_CA(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalL, kG_FinalR);

          flgBlack= 0;
          flgBlack = darknessTest2(imgOut, 1);
//          printf("flgBlack= %d\n",flgBlack);

         	if(flgBlack > 0) {
         // cleaning black
         		 float hist[5000];
         		 float av;
         		 int kCut;
         //		 char buff[500];
         		 int kk, kkC, flg, j;
         		 getHistogramForBlack(imgOut, hist);

         		 for(kk=1;kk<hist[0]-4;kk++){
         			 av = (hist[kk]+hist[kk+1]+hist[kk+2]+hist[kk+3])/4;
         			 hist[kk] = av;

         //			 sprintf(buff," hist[ %d ]= %e\n",kk, hist[kk]);
         //			 saveLog("-----------------------------------", buff);
         		 }
         //		 kkC = hist[0]/2;
         //		 kkC3 = kkC/3;
         		 flg = 0;
         		 for(kk=0;kk<100;kk++){
         			 if(hist[kk] < 0.00000001) flg++;
         			 else flg = 0;
         			 if(flg > 20) break;
         		 }
         		 if(flg < 20) {
         		    kkC = 0;
         		    if(hist[0] > 300) kkC = 300;
         		    for(kk=kkC;kk>16;kk--){
         			 if(hist[kk] > 0.00999) {
         				 flg = 0;
         				 for(j=0;j<15;j++){
         					 if(hist[kk-j] < 0.00999) {flg= 1;break;}
         				 }
         				 if(flg == 0) {
         					 kCut = kk; if(kCut < 300 ) kCut += 22;
         					 cleanBlackSide(imgOut, 0, kCut);break;
         				 }
         				 kk -= j;
         			 }
         		   }
         		 }
         		 flg = 0;
         		 for(kk=hist[0];kk>hist[0]-100;kk--){
         			 if(hist[kk] < 0.00000001) flg++;
         			 else flg = 0;
         			 if(flg > 20) break;
         		 }
         		 if(flg < 20) {
         		    kkC = hist[0];
         		    if(hist[0] > 300) kkC = hist[0]-300;
         		    for(kk=kkC;kk<hist[0]-16;kk++){
         			 if(hist[kk] > 0.00999) {
         				 flg = 0;
         				 for(j=0;j<15;j++){
         					 if(hist[kk+j] < 0.00999) {flg= 1;break;}
         				 }

         //printf(" kk= %d, flg= %d, kkC= %d, kkC3= %d\n",kk, flg, kkC, kkC3);
         				 if(flg == 0) {
         					 kCut = kk; if(hist[0]-kCut < 300 ) kCut -= 22;
         					 cleanBlackSide(imgOut, kCut, hist[0]);break;
         				 }
         				 kk += j;
         			 }
         		    }
         		 }

         }

          setDPI(img, imgOut);
//          setNewImageName(imgName, img->szIMGName, 77);
//          saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

}

void TestImageAfterConversionCARGBA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid){
   int biWidthInp, biHeightInp;
   int biWidthOut, biHeightOut, biWidthPadOut;
   int loop, loopTimes = 5;
   float tD_0[loopTimes*2];

   float mass = 0, td0;
   float massL, massR, ratio;
   float k1,  kG_FinalL, kG_FinalR;
   int ik1, ikG_Final;
   float kTab[loopTimes*2];
   int flgBlack;
   float startLoop = 0.1;
   float kF = startLoop;
	int flgHor;
   int xC_1, xC_2;
   int yC_1, yC_2;
   float div;

   int shiftX = 250;
   int shiftY = 200;



//   char imgName[100];


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;
	flgHor = 0;
    if(biWidthInp > biHeightInp)  flgHor = 1;

 // right side

      xC_1 = biWidthInp/2 - biWidthInp/6;
      xC_2 = biWidthInp/2 + biWidthInp/6;
      yC_1 = biHeightInp/2 - biHeightInp/7;
      yC_2 = biHeightInp/2 + biHeightInp/7;

      if(flgHor == 1) {
          xC_1 += shiftY;xC_2 += shiftY;
          yC_1 -= shiftX;yC_2 -= shiftX;
      }else{
     	  xC_1 += shiftX;xC_2 += shiftX;
 	      yC_1 += shiftY;  yC_2 += shiftY;
     }

   	    	    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
   	    	    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
   	    	    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
   	    	    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;

   	    		biWidthOut  = xC_2- xC_1+1;
   	    		biHeightOut = yC_2- yC_1+1;

	convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;

	{
		div = biWidthOut*biHeightOut;
		kF = startLoop;


	        if(biWidthInp > biHeightInp)  flgHor = 1;
		    for(loop=0;loop<loopTimes;loop++){
		    	kTab[loop] = kF;
//		    	convertRGBAtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
		    	convertRGBAtoMonochromeGridSmall_faster(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
	                tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);
	                if(loop == loopTimes-2)
	       	                mass = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

		    	kF += 0.15;
//		    	setNewImageName(imgName, img->szIMGName, loop);
//		    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
		    }

    }
//	printf("tD_0[0]= %e\n",tD_0[0]);
    td0 = tD_0[0];
    scaleTD(tD_0, loopTimes);
//	    	          saveTD(tD_0, loopTimes, img->szIMGName, "RR");
    calculateKbordersRGBASmall(tD_0, loopTimes, mass, &ik1);
//    	          printf("ik1= %d\n",ik1);
    if(ik1 < 2 && td0 > 0.0032) ik1++;
//	    	          if(ik1 < 2 && td0 > 0.006) ik1++;
    k1 = kTab[ik1];
    if(ik1 > 0)
        k1 -= 0.1;
    if(k1 < 0) k1 = 0.5;
// 	printf("ik1= %d, k1=%e\n",ik1, k1);

        kF = k1;
        ikG_Final = -1;
    	loop = 0;
    	while(loop < loopTimes*2-1){
	    	kTab[loop] = kF;
//printf("kF=%e\n",kF);
//	    	convertRGBAtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
	    	convertRGBAtoMonochromeGridSmall_faster(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kF, xC_1, yC_1 );
             tD_0[loop] = getNextDataForMonoLocalTHSmall(imgOut->pixPtr, biWidthOut, biHeightOut, flgHor);

	    	kF += 0.05;
//		    	setNewImageName(imgName, img->szIMGName, loop);
//		    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	    	loop++;
	    }

      	td0 = tD_0[0];
          scaleTD(tD_0, loop);
//	              saveTD(tD_0, loop, img->szIMGName, "L");
          calculateKbordersRGBASmall_2(tD_0, loop, &ikG_Final);

//	           	printf("ikG_Final=%d\n",ikG_Final);
          if(ikG_Final < 2 && td0 > 0.004) ikG_Final++;
       	  kG_FinalR = kTab[ikG_Final];
//	           	printf("ikG_Final=%d, kG_FinalL=%e\n",ikG_Final,kG_FinalR);

// left side
//    	convertRGBAtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
    	convertRGBAtoMonochromeGridSmall_faster(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
        massR = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

//          	setNewImageName(imgName, img->szIMGName, 22);
//   	    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


	       xC_1 = biWidthInp/2 - biWidthInp/6;
	       xC_2 = biWidthInp/2 + biWidthInp/6;
	       yC_1 = biHeightInp/2 - biHeightInp/7;
	       yC_2 = biHeightInp/2 + biHeightInp/7;

	    if(flgHor == 1) {
	    	xC_1 += shiftY;xC_2 += shiftY;
	    	yC_1 += shiftX;yC_2 += shiftX;
	    }else{
	    	xC_1 -= shiftX;xC_2 -= shiftX;
	    	yC_1 += shiftY;yC_2 += shiftY;
	    }
	    if(xC_1<0) xC_1 = 0;if(xC_1>=biWidthInp) xC_1 = biWidthInp-1;
	    if(xC_2<0) xC_2 = 0;if(xC_2>=biWidthInp) xC_2 = biWidthInp-1;
	    if(yC_1<0) yC_1 = 0;if(yC_1>=biHeightInp) yC_1 = biHeightInp-1;
	    if(yC_2<0) yC_2 = 0;if(yC_2>=biHeightInp) yC_2 = biHeightInp-1;
		biWidthOut  = xC_2- xC_1+1;
		biHeightOut = yC_2- yC_1+1;

		div = biWidthOut*biHeightOut;

	   	if(imgOut->ptr != NULL) {
	    		yl_free(imgOut->ptr); // memory leak
	   		  imgOut->ptr = NULL;
	   	}
		convertBMPTo_mono_BMP_initSmall(img, imgOut, biWidthOut, biHeightOut);
		biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;

//    	convertRGBAtoMonochromeGridSmall(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
    	convertRGBAtoMonochromeGridSmall_faster(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalR, xC_1, yC_1 );
        massL = (float)byteMassXY(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, biWidthOut-1, 0, biHeightOut-1)/div;

//      	setNewImageName(imgName, img->szIMGName, 44);
//    	saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

        ratio = massL/massR;
//                printf("---------ratio=%e\n",massL/massR);
        kG_FinalL = kG_FinalR;
        if(kG_FinalR < 0.6) {
           if(ratio> 1.15) kG_FinalL = kG_FinalR + 0.1;
           if(ratio> 1.4) kG_FinalL = kG_FinalR + 0.2;
        }

//                printf("---------kG_FinalL=%e, kG_FinalR = %e\n",kG_FinalL, kG_FinalR);

   	if(imgOut->ptr != NULL) {
    		yl_free(imgOut->ptr); // memory leak
   		  imgOut->ptr = NULL;
   	}
  convertBMPTo_mono_BMP_init(img, imgOut);
  convertRGBAtoMonochromeGrid2_CA_faster(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kG_FinalL, kG_FinalR);

 flgBlack= 0;
 flgBlack = darknessTest2(imgOut, 1);
// printf("flgBlack= %d\n",flgBlack);

	if(flgBlack > 0) {
// cleaning black
		 float hist[5000];
		 float av;
		 int kCut;
//		 char buff[500];
		 int kk, kkC, flg, j;
		 getHistogramForBlack(imgOut, hist);

		 for(kk=1;kk<hist[0]-4;kk++){
			 av = (hist[kk]+hist[kk+1]+hist[kk+2]+hist[kk+3])/4;
			 hist[kk] = av;

//			 sprintf(buff," hist[ %d ]= %e\n",kk, hist[kk]);
//			 saveLog("-----------------------------------", buff);
		 }
//		 kkC = hist[0]/2;
//		 kkC3 = kkC/3;
		 flg = 0;
		 for(kk=0;kk<100;kk++){
			 if(hist[kk] < 0.00000001) flg++;
			 else flg = 0;
			 if(flg > 20) break;
		 }
		 if(flg < 20) {
		    kkC = 0;
		    if(hist[0] > 300) kkC = 300;
		    for(kk=kkC;kk>16;kk--){
			 if(hist[kk] > 0.00999) {
				 flg = 0;
				 for(j=0;j<15;j++){
					 if(hist[kk-j] < 0.00999) {flg= 1;break;}
				 }
				 if(flg == 0) {
					 kCut = kk; if(kCut < 300 ) kCut += 22;
					 cleanBlackSide(imgOut, 0, kCut);break;
				 }
				 kk -= j;
			 }
		   }
		 }
		 flg = 0;
		 for(kk=hist[0];kk>hist[0]-100;kk--){
			 if(hist[kk] < 0.00000001) flg++;
			 else flg = 0;
			 if(flg > 20) break;
		 }
		 if(flg < 20) {
		    kkC = hist[0];
		    if(hist[0] > 300) kkC = hist[0]-300;
		    for(kk=kkC;kk<hist[0]-16;kk++){
			 if(hist[kk] > 0.00999) {
				 flg = 0;
				 for(j=0;j<15;j++){
					 if(hist[kk+j] < 0.00999) {flg= 1;break;}
				 }

//printf(" kk= %d, flg= %d, kkC= %d, kkC3= %d\n",kk, flg, kkC, kkC3);
				 if(flg == 0) {
					 kCut = kk; if(hist[0]-kCut < 300 ) kCut -= 22;
					 cleanBlackSide(imgOut, kCut, hist[0]);break;
				 }
				 kk += j;
			 }
		    }
		 }

}

 setDPI(img, imgOut);
//setNewImageName(imgName, img->szIMGName, 0);
//saveBMPbuffer (imgOut->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
}


int computeLuminosity(int r, int g, int b){
	return (int)((0.299 * (float)r) + (0.587 * (float)g) + (0.114 * (float)b)+0.5);
}


int getMeanLumFromGrid_24(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biWidthPad, int biHeight, int grid, int gridW, int gridH, int byteSteps){
	int mean = 0;
	int xna, yna;
	int iW, iH, xn, yn;
	long nY;
	long nX;
	int sh = 3;
	int r, g, b;
	LPBYTE ptr;
	int cc;
	//  char buff[500];

	if(byteSteps <= 0) byteSteps = 1;
	cc = 0;
	yna = idxH*grid;
	xna = idxW*grid;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		nY=(biHeight-1-yn)*biWidthPad;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn*sh;
			ptr = imgPtr+nY+nX;
			//	        mean += ((*ptr+*(ptr+1)+*(ptr+2))/3);
			b =*ptr;
			g =*(ptr+1);
			r =*(ptr+2);

			mean += computeLuminosity(r, g, b);
			cc++;
		}
	}
	//	mean /= (gridH*gridW);
	mean /= cc;
	return mean;
}
int getMeanLumFromGridCenter_24(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biWidthPad, int biHeight, int grid, int gridW, int gridH){
	int mean = 0;
	int xna, yna;
	int iW, iH, xn, yn;
	long nY;
	long nX;
	int sh = 3;
	int r, g, b;
	LPBYTE ptr;
	//  char buff[500];

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=-gridH/2;iH<gridH/2;iH++){
		yn =yna+iH;
		if(yn < 0 || yn >= biHeight) continue;
		nY=(biHeight-1-yn)*biWidthPad;
		for(iW=-gridW/2;iW<gridW/2;iW++){
			xn = xna+iW;
			if(xn < 0 || xn >= biWidth) continue;
			nX = xn*sh;
			ptr = imgPtr+nY+nX;
			//	        mean += ((*ptr+*(ptr+1)+*(ptr+2))/3);
			b =*ptr;
			g =*(ptr+1);
			r =*(ptr+2);

			mean += computeLuminosity(r, g, b);
		}
	}
	mean /= (gridH*gridW);
	return mean;
}

int getSigmaLumFromGrid_24(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biWidthPad, int biHeight, int grid, int gridW, int gridH, int mean, int byteSteps){
	int sigma = 0;
	int xna, yna;
	int iW, iH, xn, yn, a;
	long nY;
	long nX;
	int sh = 3;
	LPBYTE ptr;
	int r, g, b;
	int cc;

	if(byteSteps <= 0) byteSteps = 1;
	cc = 0;
	yna = idxH*grid;
	xna = idxW*grid;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		nY=(biHeight-1-yn)*biWidthPad;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn*sh;
			ptr = imgPtr+nY+nX;
			b =*ptr;
			g =*(ptr+1);
			r =*(ptr+2);
			a = computeLuminosity(r, g, b)-mean;
			sigma += (a*a);
			cc++;
		}
	}
	//	sigma /= (gridH*gridW);
	sigma /= cc;
	return sqrt(sigma);
}

void getMapValues_24(LPBYTE imgPtr, int biWidth, int biWidthPad, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma){
	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int m;
	//	char buff[500];

	sizeW = biWidth/grid;
	lastW = biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;
	sizeH = biHeight/grid;
	lastH = biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;



	//sprintf(buff," %d\n",biWidthPad*biHeight);
	//saveLog("max img point= ", buff);
	//sprintf(buff," sizeW= %d, lastW= %d, sizeH= %d lastH= %d\n",sizeW, lastW, sizeH, lastH);
	//saveLog("-----------------------------------", buff);
	//sprintf(buff," biWidth= %d, biWidthPad= %d, biHeight= %d\n",biWidth, biWidthPad, biHeight);
	//	saveLog("-----------------------------------", buff);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = getMeanLumFromGrid_24(imgPtr, idxW, idxH, biWidth, biWidthPad, biHeight, grid, gridW, gridH, byteSteps);

			*(lpImgMap_m+idxH*sizeW+ idxW) = m;
			if(skipSigma != 0)
				*(lpImgMap_s+idxH*sizeW+ idxW) = 0;
			else
			   *(lpImgMap_s+idxH*sizeW+ idxW) = getSigmaLumFromGrid_24(imgPtr, idxW, idxH, biWidth, biWidthPad, biHeight, grid, gridW, gridH, m, byteSteps);

		}
	}
	/*
	for(idxH=0;idxH<sizeH;idxH++){
		sprintf(buff,"\n");
    	saveLog("", buff);
	    for(idxW=0;idxW<sizeW;idxW++){
	    	sprintf(buff,"%d (%d), ", *(lpImgMap_m+idxH*sizeW+ idxW), *(lpImgMap_s+idxH*sizeW+ idxW));
	    	saveLog("", buff);

//	    	printf(" iW= %d, iH= %d, m= %d, s= %d \n", idxW, idxH, *(lpImgMap_m+idxH*sizeW+ idxW), *(lpImgMap_s+idxH*sizeW+ idxW));
		}
	}
	 */
}
int getMeanLumFromGrid_RGBA(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biHeight, int grid, int gridW, int gridH, int byteSteps){
	int mean = 0;
	int xna, yna;
	int iW, iH, xn, yn;
	long nY;
	long nX;
	int sh = 4;
	int r, g, b;
	int cc;
	LPBYTE ptr;
	//  char buff[500];

	yna = idxH*grid;
	xna = idxW*grid;
	cc = 0;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		//		nY=(biHeight-1-yn)*biWidthPad;
		nY=yn*biWidth*sh;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn*sh;
			ptr = imgPtr+nY+nX;
//			b =*ptr;
			g =*(ptr+1);
//			r =*(ptr+2);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
			r =*ptr;
			b =*(ptr+2);
#else
			b =*ptr;
			r =*(ptr+2);
#endif

			//	    	r = *ptr;
			//	    	g = *(ptr+1);
			//	    	b = *(ptr+2);

			mean += computeLuminosity(r, g, b);
			cc++;
		}
	}
	//	mean /= (gridH*gridW);
	mean /= cc;
	return mean;
}
int getSigmaLumFromGrid_RGBA(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biHeight, int grid, int gridW, int gridH, int mean, int byteSteps){
	int sigma = 0;
	int xna, yna;
	int iW, iH, xn, yn, a;
	long nY;
	long nX;
	int sh = 4;
	LPBYTE ptr;
	int r, g, b;
	int cc;

	yna = idxH*grid;
	xna = idxW*grid;
	cc = 0;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		//		nY=(biHeight-1-yn)*biWidthPad;
		nY=yn*biWidth*sh;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn*sh;
			ptr = imgPtr+nY+nX;
//			b =*ptr;
			g =*(ptr+1);
//			r =*(ptr+2);

			//	    	r = *ptr;
			//	    	g = *(ptr+1);
			//	    	b = *(ptr+2);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
			r =*ptr;
			b =*(ptr+2);
#else
			b =*ptr;
			r =*(ptr+2);
#endif

			a = computeLuminosity(r, g, b)-mean;
			sigma += (a*a);
			cc++;
		}
	}
	//	sigma /= (gridH*gridW);
	sigma /= cc;
	return sqrt(sigma);
}

void getMapValues_RGBA(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma){
	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
	int gridW, gridH;
	int m;
	//	char buff[500];

	sizeW = biWidth/grid;
	lastW = biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = biHeight/grid;
	lastH = biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;


	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		idxHW = idxH*sizeW;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = getMeanLumFromGrid_RGBA(imgPtr, idxW, idxH, biWidth, biHeight, grid, gridW, gridH, byteSteps);
			*(lpImgMap_m+idxHW+ idxW) = m;
			if(skipSigma != 0)
				*(lpImgMap_s+idxHW+ idxW) = 0;
			else
			    *(lpImgMap_s+idxHW+ idxW) = getSigmaLumFromGrid_RGBA(imgPtr, idxW, idxH, biWidth, biHeight, grid, gridW, gridH, m, byteSteps);
		}
	}
}
int getMeanLumFromGrid_RGB565(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biHeight, int grid, int gridW, int gridH, int byteSteps){
	int mean = 0;
	int xna, yna;
	int iW, iH, xn, yn;
	long nY;
	long nX;
	//   int sh = 2;
	int r, g, b;
	int cc;
	//   LPBYTE ptr;
	WORD *ptr;
	WORD wr;
	//  char buff[500];

	yna = idxH*grid;
	xna = idxW*grid;
	cc = 0;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		nY=yn*biWidth;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn;
			ptr = (WORD*)imgPtr+nY+nX;

			wr =  *ptr;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );

			//	        b =*ptr;
			//	        g =*(ptr+1);
			//	        r =*(ptr+2);

			//	    	r = *ptr;
			//	    	g = *(ptr+1);
			//	    	b = *(ptr+2);


			mean += computeLuminosity(r, g, b);
			cc++;
		}
	}
	//	mean /= (gridH*gridW);
	mean /= cc;
	return mean;
}
int getSigmaLumFromGrid_RGB565(LPBYTE imgPtr, int idxW, int idxH, int biWidth, int biHeight, int grid, int gridW, int gridH, int mean, int byteSteps){
	int sigma = 0;
	int xna, yna;
	int iW, iH, xn, yn, a;
	long nY;
	long nX;
	//   int sh = 2;
	//   LPBYTE ptr;
	int r, g, b;
	int cc;
	WORD *ptr;
	WORD wr;

	yna = idxH*grid;
	xna = idxW*grid;
	cc = 0;
	for(iH=0;iH<gridH;iH += byteSteps){
		yn =yna+iH;
		nY=yn*biWidth;
		for(iW=0;iW<gridW;iW += byteSteps){
			xn = xna+iW;
			nX = xn;
			ptr = (WORD*)imgPtr+nY+nX;

			wr =  *ptr;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );

			//	        b =*ptr;
			//	        g =*(ptr+1);
			//	        r =*(ptr+2);

			//	    	r = *ptr;
			//	    	g = *(ptr+1);
			//	    	b = *(ptr+2);
			a = computeLuminosity(r, g, b)-mean;
			sigma += (a*a);
			cc++;
		}
	}
	//	sigma /= (gridH*gridW);
	sigma /= cc;
	return sqrt(sigma);
}

void getMapValues_RGB565(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma){
	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
	int gridW, gridH;
	int m;
	//	char buff[500];

	sizeW = biWidth/grid;
	lastW = biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = biHeight/grid;
	lastH = biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		idxHW = idxH*sizeW;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = getMeanLumFromGrid_RGB565(imgPtr, idxW, idxH, biWidth, biHeight, grid, gridW, gridH, byteSteps);
			*(lpImgMap_m+idxHW+ idxW) = m;
			if(skipSigma != 0)
				*(lpImgMap_s+idxHW+ idxW) = 0;
			else
			    *(lpImgMap_s+idxHW+ idxW) = getSigmaLumFromGrid_RGB565(imgPtr, idxW, idxH, biWidth, biHeight, grid, gridW, gridH, m, byteSteps);

		}
	}
}
void convertBMPTo_mono_BMP_initSmall(LPIMGDATA img, LPIMGDATA imgOutp, int biWidthOut, int biHeightOut){
	int hSize = 54;
	int bufforSize;
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage, biWidthPad;

	biWidthPad = ((biWidthOut+31)/32)*4;


	biSizeImage = biWidthPad*biHeightOut;
	bufforSize= biSizeImage+hSize+8;

	ptr = yl_calloc("PP3", bufforSize, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertBMPToMonoBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}
	bf = (LPBITMAPFILEHEADER)(ptr);
	bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));

	bf->bfType[0]= 0x42;
	bf->bfType[1]= 0x4d;
	set4Bytes(bf->bfSize, bufforSize);

	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	bf->bfReserved2[0] = 0;
	bf->bfReserved2[1] = 0;
	set4Bytes(bf->bfOffBits, hSize+8);

	imgOutp->ptr = ptr;
	imgOutp->bf.bfType = getLeftBytes((int*)bf->bfType, 2);
	imgOutp->bf.bfSize = getLeftBytes((int*)bf->bfSize, 4);
	imgOutp->bf.bfReserved1 =  getLeftBytes((int*)bf->bfReserved1, 2);
	imgOutp->bf.bfReserved2 =  getLeftBytes((int*)bf->bfReserved2, 2);
	imgOutp->bf.bfOffBits =   getLeftBytes((int*)bf->bfOffBits, 4);
	imgOutp->pixPtr = ptr +imgOutp->bf.bfOffBits;



	set4Bytes(bi->biSize, hSize-14);
	set4Bytes(bi->biWidth, biWidthOut);
	set4Bytes(bi->biHeight, biHeightOut);
	*bi->biPlanes = (short)1;
	*bi->biBitCount= (short)1;


	set4Bytes(bi->biCompression, 0);
	set4Bytes(bi->biSizeImage, biSizeImage);
	set4Bytes(bi->biXPelsPerMeter, 512);
	set4Bytes(bi->biYPelsPerMeter, 512);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);


	imgOutp->bi.biSize = getLeftBytes((int*)bi->biSize, 4);
	imgOutp->bi.biWidth = getLeftBytes((int*)bi->biWidth, 4);
	imgOutp->bi.biHeight = getLeftBytes((int*)bi->biHeight, 4);
	imgOutp->bi.biPlanes = getLeftBytes((int*)bi->biPlanes, 2);
	imgOutp->bi.biBitCount = getLeftBytes((int*)bi->biBitCount, 2);
	imgOutp->bi.biCompression = getLeftBytes((int*)bi->biCompression, 4);
	imgOutp->bi.biSizeImage = getLeftBytes((int*)bi->biSizeImage, 4);
	imgOutp->bi.biXPelsPerMeter = getLeftBytes((int*)bi->biXPelsPerMeter, 4);
	imgOutp->bi.biYPelsPerMeter = getLeftBytes((int*)bi->biYPelsPerMeter, 4);
	imgOutp->bi.biClrUsed = getLeftBytes((int*)bi->biClrUsed, 4);
	imgOutp->bi.biClrImportant = getLeftBytes((int*)bi->biClrImportant, 4);


	imgOutp->bi.biWidthPad = ((imgOutp->bi.biWidth*imgOutp->bi.biBitCount+31)/32)*4;
	imgOutp->colorPtr = ptr + 14 + imgOutp->bi.biSize;
	*(ptr+hSize) = 0;
	*(ptr+hSize+1) = 0;
	*(ptr+hSize+2) = 0;
	*(ptr+hSize+3) = 0;
	*(ptr+hSize+4) = 255;
	*(ptr+hSize+5) = 255;
	*(ptr+hSize+6) = 255;
	*(ptr+hSize+7) = 0;

	//      setPixelArrayFromBMPTomonoWB_tH3(imgOutp, img, tH1, tH2, tH3);
}

void convertBMPTo_mono_BMP_init(LPIMGDATA img, LPIMGDATA imgOutp){
	int hSize = 54;
	int bufforSize;
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage, biWidthPad;

	biWidthPad = ((img->bi.biWidth+31)/32)*4;


	biSizeImage = biWidthPad*img->bi.biHeight;
	bufforSize= biSizeImage+hSize+8;

	ptr = yl_calloc("PP3", bufforSize, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertBMPToMonoBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}
	bf = (LPBITMAPFILEHEADER)(ptr);
	bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));

	bf->bfType[0]= 0x42;
	bf->bfType[1]= 0x4d;
	set4Bytes(bf->bfSize, bufforSize);

	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	bf->bfReserved2[0] = 0;
	bf->bfReserved2[1] = 0;
	set4Bytes(bf->bfOffBits, hSize+8);

	imgOutp->ptr = ptr;
	imgOutp->bf.bfType = getLeftBytes((int*)bf->bfType, 2);
	imgOutp->bf.bfSize = getLeftBytes((int*)bf->bfSize, 4);
	imgOutp->bf.bfReserved1 =  getLeftBytes((int*)bf->bfReserved1, 2);
	imgOutp->bf.bfReserved2 =  getLeftBytes((int*)bf->bfReserved2, 2);
	imgOutp->bf.bfOffBits =   getLeftBytes((int*)bf->bfOffBits, 4);
	imgOutp->pixPtr = ptr +imgOutp->bf.bfOffBits;



	set4Bytes(bi->biSize, hSize-14);
	set4Bytes(bi->biWidth, img->width);
	set4Bytes(bi->biHeight, img->height);
	*bi->biPlanes = (short)1;
	*bi->biBitCount= (short)1;


	set4Bytes(bi->biCompression, 0);
	set4Bytes(bi->biSizeImage, biSizeImage);
	set4Bytes(bi->biXPelsPerMeter, 512);
	set4Bytes(bi->biYPelsPerMeter, 512);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);


	imgOutp->bi.biSize = getLeftBytes((int*)bi->biSize, 4);
	imgOutp->bi.biWidth = getLeftBytes((int*)bi->biWidth, 4);
	imgOutp->bi.biHeight = getLeftBytes((int*)bi->biHeight, 4);
	imgOutp->bi.biPlanes = getLeftBytes((int*)bi->biPlanes, 2);
	imgOutp->bi.biBitCount = getLeftBytes((int*)bi->biBitCount, 2);
	imgOutp->bi.biCompression = getLeftBytes((int*)bi->biCompression, 4);
	imgOutp->bi.biSizeImage = getLeftBytes((int*)bi->biSizeImage, 4);
	imgOutp->bi.biXPelsPerMeter = getLeftBytes((int*)bi->biXPelsPerMeter, 4);
	imgOutp->bi.biYPelsPerMeter = getLeftBytes((int*)bi->biYPelsPerMeter, 4);
	imgOutp->bi.biClrUsed = getLeftBytes((int*)bi->biClrUsed, 4);
	imgOutp->bi.biClrImportant = getLeftBytes((int*)bi->biClrImportant, 4);


	imgOutp->bi.biWidthPad = ((imgOutp->bi.biWidth*imgOutp->bi.biBitCount+31)/32)*4;
	imgOutp->colorPtr = ptr + 14 + imgOutp->bi.biSize;
	*(ptr+hSize) = 0;
	*(ptr+hSize+1) = 0;
	*(ptr+hSize+2) = 0;
	*(ptr+hSize+3) = 0;
	*(ptr+hSize+4) = 255;
	*(ptr+hSize+5) = 255;
	*(ptr+hSize+6) = 255;
	*(ptr+hSize+7) = 0;


	//      setPixelArrayFromBMPTomonoWB_tH3(imgOutp, img, tH1, tH2, tH3);
}

void convertBMPTo_mono_BMPLum_Next(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int idxW, int idxH, int grid, int gridW, int gridH, int tH){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	long nYOut, nXOut;

	int sh = 3;
	LPBYTE ptrI, ptrO;
	int v;
	BYTE mask;
	//	   int  max;
	int r, g, b;


	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		nYOut=(biHeightOut-1-yn)*biWidthPadOut;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*sh;
			nXOut = xn/8;
			ptrI = ptrInp+nYInp+nXInp;
			//	            max = max(*ptrI, *(ptrI+1));
			//	            max = max(max, *(ptrI+2));
			b =*ptrI;
			g =*(ptrI+1);
			r =*(ptrI+2);
			v = computeLuminosity(r, g, b);
	        if(v < tH) v= 0; else v = 255;

			ptrO = ptrOut+nYOut+nXOut;
			mask=LEFT_BIT;
			mask >>=(xn%8);
			if(v!=0){
				*ptrO = *ptrO | mask;
			}
			else {
				*ptrO = *ptrO & (~mask);
			}

		}
	}

}
void convert24BMPTo_mono_BMPLum_OnePixel(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int xInp, int yInp, int tH, int xOut, int yOut){

	long nYInp, nXInp;
	long nYOut, nXOut;

	int sh = 3;
	LPBYTE ptrI, ptrO;
	int v;
	BYTE mask;
	int r, g, b;

	nYInp=(biHeightInp-1-yInp)*biWidthPadInp;
	nYOut=(biHeightOut-1-yOut)*biWidthPadOut;
	nXInp = xInp*sh;
	nXOut = xOut/8;
	ptrI = ptrInp+nYInp+nXInp;
	b =*ptrI;
	g =*(ptrI+1);
	r =*(ptrI+2);
	v = computeLuminosity(r, g, b);
    if(v < tH) v= 0; else v = 255;

	ptrO = ptrOut+nYOut+nXOut;
	mask=LEFT_BIT;
	mask >>=(xOut%8);
	if(v!=0){
		*ptrO = *ptrO | mask;
	}
	else {
		*ptrO = *ptrO & (~mask);
	}
}

void convertRGBATo_mono_BMPLum_OnePixel(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int xInp, int yInp, int tH, int xOut, int yOut){

	long nYInp, nXInp;
	long nYOut, nXOut;

	int shInp = 4;
	LPBYTE ptrI, ptrO;
	int v;
	BYTE mask;
	int r, g, b;

	nYInp=yInp*biWidthInp*shInp;
	nYOut=(biHeightOut-1-yOut)*biWidthPadOut;
	nXInp = xInp*shInp;
	nXOut = xOut/8;
	ptrI = ptrInp+nYInp+nXInp;

	g =*(ptrI+1);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif
			v = computeLuminosity(r, g, b);
			if(v < tH) v= 0; else v = 255;

			ptrO = ptrOut+nYOut+nXOut;
			mask=LEFT_BIT;
			mask >>=(xOut%8);
			if(v!=0){
				*ptrO = *ptrO | mask;
			}
			else {
				*ptrO = *ptrO & (~mask);
			}

}

void convertRGB565To_mono_BMPLum_OnePixel(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int xInp, int yInp, int tH, int xOut, int yOut){

	long nYInp, nXInp;
	long nYOut, nXOut;

	LPBYTE  ptrO;
	int v;
	BYTE mask;
	int r, g, b;

	WORD *ptrI;
	WORD wr;


	nYInp=yInp*biWidthInp;
	nYOut=(biHeightOut-1-yOut)*biWidthPadOut;
	nXInp = xInp;
	nXOut = xOut/8;
	ptrI = (WORD*)ptrInp+nYInp+nXInp;
	wr =  *ptrI;
	b = ((wr & eRGB565_R ) >> 11);
	g = ((wr & eRGB565_G) ) >> 5;
	r = (wr & eRGB565_B );
	v = computeLuminosity(r, g, b);
	if(v < tH) v= 0; else v = 255;

	ptrO = ptrOut+nYOut+nXOut;
	mask=LEFT_BIT;
	mask >>=(xOut%8);
	if(v!=0){
		*ptrO = *ptrO | mask;
	}
	else {
		*ptrO = *ptrO & (~mask);
	}

}

void convertRGBATo_mono_BMPLum_Next(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int idxW, int idxH, int grid, int gridW, int gridH, int tH){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	long nYOut, nXOut;

	int shInp = 4;
	LPBYTE ptrI, ptrO;
	int v;
	BYTE mask;
	//	   int  max;
	int r, g, b;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		//			nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		nYInp=yn*biWidthInp*shInp;
		nYOut=(biHeightOut-1-yn)*biWidthPadOut;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*shInp;
			nXOut = xn/8;
			ptrI = ptrInp+nYInp+nXInp;
			//	            max = max(*ptrI, *(ptrI+1));
			//	            max = max(max, *(ptrI+2));
//			b =*ptrI;
			g =*(ptrI+1);
//			r =*(ptrI+2);

			//		        r =*ptrI;
			//		        g =*(ptrI+1);
			//		        b =*(ptrI+2);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif

			v = computeLuminosity(r, g, b);
			if(v < tH) v= 0; else v = 255;

			ptrO = ptrOut+nYOut+nXOut;
			mask=LEFT_BIT;
			mask >>=(xn%8);
			if(v!=0){
				*ptrO = *ptrO | mask;
			}
			else {
				*ptrO = *ptrO & (~mask);
			}

		}
	}

}

int isItPAred(int b, int g, int r, int th){
	int ret = 0;
//	if(g < 20 && r > 40) ret = 1;
//	if(r > 60 && g-r > 40) ret = 1;
//	v = computeLuminosity(r, g, b);
	if( r > th) ret = 1;


//	if(g < 20 && r > 85) ret = 1;
//	if(b> 20 && b < 60 && g < 25 && r > 80 && r < 150) ret = 1;
//	if(b < 60 && g < 25 && r > 100) ret = 1;
	return ret;
}
void setWhitePTR(LPBYTE ptrI){
	*ptrI = 255; *(ptrI+1) = 255;*(ptrI+2) = 255;
}
void setBlackPTR(LPBYTE ptrI){
	*ptrI = 0; *(ptrI+1) = 0;*(ptrI+2) = 0;
}

void convertBMPRedToWhite(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	int sh = 3;
	LPBYTE ptrI;
	int r, g, b;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*sh;
			ptrI = ptrInp+nYInp+nXInp;
			b =*ptrI;
			g =*(ptrI+1);
			r =*(ptrI+2);

//		if(b<255 && g < 255 && r < 255){
//				 char buff[500];
//				sprintf(buff," idxW= %d, idxH= %d, b= %d, g= %d, r= %d\n",idxW, idxH, b, g, r);
//				saveLog("---------------", buff);
//			}

            if(isItPAred(b, g, r, th) == 1)
            	setWhitePTR(ptrI);

		}
	}
}
int isItPAredNY(int b, int g, int r, int th){
	int ret = 0;
//	if(g < 20 && r > 40) ret = 1;
//	if(r > 60 && g-r > 40) ret = 1;
//	v = computeLuminosity(r, g, b);
//	if( r > th) ret = 1;
//	if( r > th && b <15) ret = 1;


//	if(g < 20 && r > 85) ret = 1;
//	if(b> 20 && b < 60 && g < 25 && r > 80 && r < 150) ret = 1;
//	if(b < 60 && g < 25 && r > 100) ret = 1;
	return ret;
}

void convertBMPRedToWhiteNY(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	int sh = 3;
	LPBYTE ptrI;
	int r, g, b;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*sh;
			ptrI = ptrInp+nYInp+nXInp;
			b =*ptrI;
			g =*(ptrI+1);
			r =*(ptrI+2);

//		if(b<255 && g < 255 && r < 255){
//				 char buff[500];
//				sprintf(buff," idxW= %d, idxH= %d, b= %d, g= %d, r= %d\n",idxW, idxH, b, g, r);
//				saveLog("---------------", buff);
//			}

            if(isItPAredNY(b, g, r, th) == 1)
            	setWhitePTR(ptrI);

		}
	}
}

float getColorValueBMP(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int th){
//		                   int *rOut, int *gOut, int *bOut){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	int sh = 3;
	LPBYTE ptrI;
	int r, g, b;
    float value = 0;

	yna = idxH*grid;
	xna = idxW*grid;

//	*rOut = 0;
//	*gOut = 0;
//	*bOut = 0;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*sh;
			ptrI = ptrInp+nYInp+nXInp;
			b =*ptrI;
			g =*(ptrI+1);
			r =*(ptrI+2);

//		if(b<255 && g < 255 && r < 255){
//				 char buff[500];
//				sprintf(buff," idxW= %d, idxH= %d, b= %d, g= %d, r= %d\n",idxW, idxH, b, g, r);
//				saveLog("---------------", buff);
//			}
//			*rOut += r;
//			*gOut += g;
//			*bOut += b;
			value += (r+ g + b);

		}
	}
	value /= (float)(gridH*gridW);
//	(*rOut) /= (gridH*gridW);;
//	(*gOut) /= (gridH*gridW);;
//	(*bOut) /= (gridH*gridW);;

	return value;
}

void getTestRGB(LPIMGDATA img, int x, int y, int *r, int *g, int *b){
  LPBYTE ptrI;
//  int biWidthInp, biWidthPadInp, biHeightInp;
  int  biWidthPadInp, biHeightInp;
  int nY, nX;
  int sh = 3;

//  biWidthInp=img->bi.biWidth;
  biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
  biHeightInp = img->bi.biHeight;

  nY=(biHeightInp-1-y)*biWidthPadInp;
  nX = x*sh;
  ptrI = img->pixPtr+nY+nX;
  *b =*ptrI;
  *g =*(ptrI+1);
  *r =*(ptrI+2);
//  *r =*ptrI;
//  *g =*(ptrI+1);
//  *b =*(ptrI+2);
}

void setTestRGB(LPIMGDATA img, int x, int y, int r, int g, int b){
  LPBYTE ptrI;
//  int biWidthInp, biWidthPadInp, biHeightInp;
  int  biWidthPadInp, biHeightInp;
  int nY, nX;
  int sh = 3;

//  biWidthInp=img->bi.biWidth;
  biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
  biHeightInp = img->bi.biHeight;

  nY=(biHeightInp-1-y)*biWidthPadInp;
  nX = x*sh;
  ptrI = img->pixPtr+nY+nX;
   *ptrI = b;
  *(ptrI+1) = g;
  *(ptrI+2) = r;

}

void testFilterCARed24BMP(LPIMGDATA img){
int x, x1, x2;
int y, y1, y2;
int r, g, b, v;
//int dd1, dd2, dd, perc;
int dd;
char buff[500];

//x1 = 1800;
//x2 = 2200;
//y1 = 1004;
//y2 = 1009;
//x1 = 800;
//x2 = 1000;
//y1 = 1604;
//y2 = 1609;
x1 = 400;
x2 = 406;
y1 = 800;
y2 = 1000;

	for(x=x1;x<x2;x++){
		saveLog("----------------------------------------------------", "\n");
	  for(y=y1;y<y2;y++){
		  getTestRGB(img, x, y, &r, &g, &b);
		  setTestRGB(img, x, y, 255, 255, 255);
		  v = computeLuminosity(r, g, b);
//		  dd1 = r-g;if(dd1<0) dd1= -dd1;
//		  dd2 = b-g;if(dd2<0) dd2= -dd2;
//		  dd = (dd1+dd2)/2;
//		  perc = 0;
//		  if(v>0) perc = (int)(((float)dd/(float)v)*100.0);
		  dd = r- b;

sprintf(buff," x= %d, y= %d, r= %d, g= %d, b= %d, v= %d, dd= %d\n",x, y, r, g, b, v, dd);
saveLog("---", buff);

	  }
	}

saveBMPbuffer (img->ptr, "A_testRGB.bmp", img->szOutPath);
}

void getNextAvBMP(LPBYTE ptrInp, int biWidthInp, int biWidthPadInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int *av){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	int sh = 3;
	LPBYTE ptrI;
	int r, g, b, v;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*sh;
			ptrI = ptrInp+nYInp+nXInp;
			b =*ptrI;
			g =*(ptrI+1);
			r =*(ptrI+2);
			v = computeLuminosity(r, g, b);
            if(v>70 && v<=110) av[0]++;
            if(v>110 && v<=150) av[1]++;
            if(v>150 && v<=190) av[2]++;
            if(v>190 && v<=230) av[3]++;

		}
	}
}
void getNextAvRGBA(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int *av){
	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	int shInp = 4;
	LPBYTE ptrI;

	int r, g, b, v;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=yn*biWidthInp*shInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*shInp;
			ptrI = ptrInp+nYInp+nXInp;
			g =*(ptrI+1);

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif
			v = computeLuminosity(r, g, b);
            if(v>70 && v<=110) av[0]++;
            if(v>110 && v<=150) av[1]++;
            if(v>150 && v<=190) av[2]++;
            if(v>190 && v<=230) av[3]++;

		}
	}
}
void getNextAvRGB565(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int *av){
	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	WORD wr;

	WORD *ptrI;

	int r, g, b, v;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=yn*biWidthInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn;
			ptrI = (WORD*)ptrInp+nYInp+nXInp;

			wr =  *ptrI;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );
			v = computeLuminosity(r, g, b);

            if(v>8 && v<=15) av[0]++;
            if(v>15 && v<=25) av[1]++;
            if(v>25 && v<=32) av[2]++;
            if(v>32 && v<=40) av[3]++;

		}
	}
}



void filterCARed24BMP(LPIMGDATA img,  int grid){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int thClr;
	int minH, maxH;
	int cc, i;
	int av[4];

	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

//	convertBMPTo_mono_BMP_init(img, imgOut);
//	biWidthOut=imgOut->bi.biWidth;
//	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
//	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	cc = 0;
   for(i=0;i<4;i++)
	   av[i] = 0;

	minH = 0;
	maxH = sizeH/3;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvBMP(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;
		}
	}

	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 50;
    else thClr = 70;
	}

//	maxH  = mrg; if(maxH > sizeH) maxH = sizeH;
//	thClr = 50;
 //printf("---thClr= %d\n", thClr);

	minH = 0;
	maxH = sizeH/3;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr);
		}
	}


	minH = maxH;
	maxH = (4*sizeH)/5;
	cc = 0;
   for(i=0;i<4;i++)
	   av[i] = 0;

	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvBMP(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;

		}
	}
	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 50;
    else thClr = 70;
	}
//printf("---thClr= %d\n", thClr);
//	thClr = 90;
	minH = maxH;
	maxH = (4*sizeH)/5;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr);
		}
	}


	minH = maxH;
	maxH = sizeH;
    cc = 0;
    for(i=0;i<4;i++)
	   av[i] = 0;

	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvBMP(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;

		}
	}

	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 50;
    else thClr = 60;
	}
//printf("---thClr= %d\n", thClr);
//	thClr = 60;
	minH = maxH;
	maxH = sizeH;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr);
		}
	}

}

void filterNYRed24BMP(LPIMGDATA img,  int grid, int side){
//	side = 0 - all
//	side = 1 - only left
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int maxH;
	int mrg = 8;
//	int biWidthOut, biWidthPadOut, biHeightOut;


	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

//	convertBMPTo_mono_BMP_init(img, imgOut);
//	biWidthOut=imgOut->bi.biWidth;
//	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
//	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

		maxH  = sizeH - mrg; if(maxH < 0) maxH = 0;
		for(idxH=0;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertBMPRedToWhiteNY(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
			}
		}

}

void filterPARed24BMP(LPIMGDATA img,  int grid, int side){
//	side = 0 - all
//	side = 1 - only left
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int maxH, maxW;
	int mrg = 8;
//	int biWidthOut, biWidthPadOut, biHeightOut;


	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

//	convertBMPTo_mono_BMP_init(img, imgOut);
//	biWidthOut=imgOut->bi.biWidth;
//	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
//	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;
if(side == 0) {
	maxH  = mrg; if(maxH > sizeH) maxH = sizeH;
	for(idxH=0;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side == 0 || side == 1) {
	maxH  = sizeH - mrg; if(maxH < 0) maxH = 0;
	for(idxH=maxH;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side == 0 || side == 1) {
	maxW  = mrg; if(maxW > sizeW) maxW = sizeW;
	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<maxW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side == 0) {
	maxW  = sizeW - mrg; if(maxW < 0) maxW = 0;
	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=maxW;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
 }
}

void convertRGBARedToWhite(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	int shInp = 4;
	LPBYTE ptrI;

	int r, g, b;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		//			nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		nYInp=yn*biWidthInp*shInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*shInp;
			ptrI = ptrInp+nYInp+nXInp;
//			b =*ptrI;
			g =*(ptrI+1);
//			r =*(ptrI+2);

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__	|| LINUX || linux || UNIX || __unix__)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif


            if(isItPAred(b, g, r, th) == 1)
            	setWhitePTR(ptrI);



		}
	}
}
float getColorValueRGBA(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	int shInp = 4;
	LPBYTE ptrI;
	int r, g, b;
    float value = 0;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		//			nYInp=(biHeightInp-1-yn)*biWidthPadInp;
		nYInp=yn*biWidthInp*shInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			nXInp = xn*shInp;
			ptrI = ptrInp+nYInp+nXInp;
//			b =*ptrI;
			g =*(ptrI+1);
//			r =*(ptrI+2);

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif

            if(isItPAred(b, g, r, th) == 1)
            	setWhitePTR(ptrI);

			value += (r+ g + b);
		}
	}
	value /= (float)(gridH*gridW);
	return value;

}
void filterOrangeBMP(LPIMGDATA img, int grid){

		int sizeW, sizeH, lastW, lastH;
		int idxW, idxH;
		int gridW, gridH;
		int biWidthInp, biWidthPadInp, biHeightInp;
		int thClr;

		biWidthInp=img->bi.biWidth;
		biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
		biHeightInp = img->bi.biHeight;

		sizeW = img->bi.biWidth/grid;
		lastW = img->bi.biWidth - sizeW*grid;
		if(lastW > 0)
			sizeW++;
		else
			lastW = grid;

		sizeH = img->bi.biHeight/grid;
		lastH = img->bi.biHeight - sizeH*grid;
		if(lastH > 0)
			sizeH++;
		else
			lastH = grid;

		thClr = 150;
		for(idxH=0;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertBMPRedToWhite(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr);
			}
		}
}
void filterOrangeRGB565(LPIMGDATA img, int grid){

		int sizeW, sizeH, lastW, lastH;
		int idxW, idxH;
		int gridW, gridH;
		int biWidthInp, biHeightInp;
		int thClr;

		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;

		sizeW = img->bi.biWidth/grid;
		lastW = img->bi.biWidth - sizeW*grid;
		if(lastW > 0)
			sizeW++;
		else
			lastW = grid;

		sizeH = img->bi.biHeight/grid;
		lastH = img->bi.biHeight - sizeH*grid;
		if(lastH > 0)
			sizeH++;
		else
			lastH = grid;

		thClr = 18;

		for(idxH=0;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
			}
		}
}
void filterCARedRGBA(LPIMGDATA img, int grid){
//		LPBITMAPINFOHEADER bi;

		int sizeW, sizeH, lastW, lastH;
		int idxW, idxH;
		int gridW, gridH;
		int biWidthInp, biHeightInp;
		int thClr;
		int minH, maxH;

		int cc, i;
		int av[4];



		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;

		sizeW = img->bi.biWidth/grid;
		lastW = img->bi.biWidth - sizeW*grid;
		if(lastW > 0)
			sizeW++;
		else
			lastW = grid;

		sizeH = img->bi.biHeight/grid;
		lastH = img->bi.biHeight - sizeH*grid;
		if(lastH > 0)
			sizeH++;
		else
			lastH = grid;

		cc = 0;
	   for(i=0;i<4;i++)
		   av[i] = 0;
		minH = 0;
		maxH = sizeH/3;
		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=8;idxW<sizeW-8;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				getNextAvRGBA(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, av);
				cc++;
			}
		}

		if(cc > 0){
		   for(i=0;i<4;i++)
			   av[i] /= cc;
		}
		{
	     int max = 0, iMax = 0;
	       for(i=0;i<4;i++){
	//    printf("---i= %d, av= %d\n", i, av[i]);
	        if(max < av[i]) {max = av[i];iMax = i;}
	        }
	    if(iMax == 0) thClr = 50;
	    else thClr = 70;
		}


//		thClr = 50;
		minH = 0;
		maxH = sizeH/3;
		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
			}
		}



		minH = maxH;
		maxH = (4*sizeH)/5;
		cc = 0;
	   for(i=0;i<4;i++)
		   av[i] = 0;

		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=8;idxW<sizeW-8;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				getNextAvRGBA(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
				cc++;

			}
		}
		if(cc > 0){
		   for(i=0;i<4;i++)
			   av[i] /= cc;
		}
		{
	     int max = 0, iMax = 0;
	       for(i=0;i<4;i++){
	//    printf("---i= %d, av= %d\n", i, av[i]);
	        if(max < av[i]) {max = av[i];iMax = i;}
	        }
	    if(iMax == 0) thClr = 50;
	    else thClr = 70;
		}

//		thClr = 90;
		minH = maxH;
		maxH = (4*sizeH)/5;
		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
			}
		}


		minH = maxH;
		maxH = sizeH;
	    cc = 0;
	    for(i=0;i<4;i++)
		   av[i] = 0;

		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=8;idxW<sizeW-8;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				getNextAvRGBA(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
				cc++;

			}
		}

		if(cc > 0){
		   for(i=0;i<4;i++)
			   av[i] /= cc;
		}
		{
	     int max = 0, iMax = 0;
	       for(i=0;i<4;i++){
	//    printf("---i= %d, av= %d\n", i, av[i]);
	        if(max < av[i]) {max = av[i];iMax = i;}
	        }
	    if(iMax == 0) thClr = 50;
	    else thClr = 60;
		}


//		thClr = 60;
		minH = maxH;
		maxH = sizeH;
		for(idxH=minH;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
			}
		}
}

int testRedRGB565(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH){

int xn, yn;
long nYInp, nXInp;

WORD *ptrI;

int r, g, b;
WORD wr;
int drb, drg;
int ret = 0;

//	char buff[500];

	yn =idxH;
	nYInp=yn*biWidthInp;
	xn = idxW;
	nXInp = xn;

	ptrI = (WORD*)ptrInp+nYInp+nXInp;

	wr =  *ptrI;
	b = ((wr & eRGB565_R ) >> 11);
	g = ((wr & eRGB565_G) ) >> 5;
	r = (wr & eRGB565_B );

//			wr =  whiteRGB565;
//			*ptrI = wr;

	drb = r-b;
	drg = r-g;
	if((drb > 5 && drg > 1 && drb+drg > 7 && r > 14 && g< 17 && b < 15) ||
	   (drb > 5 && drg > 5 && drb+drg > 12 && r > 11 && g< 10 && b < 10) ||
	   (drb > 4 && drg > 4 && drb+drg > 10 && r > 7 && g< 6 && b < 6)) {
		wr =  whiteRGB565;
		*ptrI = wr;
		ret = 1;
//		     sprintf(buff,"!!!!!!!!!!!!!!!!!!---------y== %d, x= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",yn, xn, b, g, r, drb, drg);
//			     saveLog("", buff);
		}
//					else{
//			sprintf(buff,"y== %d, x= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",yn, xn, b, g, r, drb, drg);
//		     saveLog("", buff);
//				}


//        if(isItPAred(b, g, r, th) == 1){
//			wr =  whiteRGB565;
//			*ptrI = wr;
//        }
	return ret;
}

int testRedRGBA(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                   int idxW, int idxH){
	int xn, yn;
	long nYInp, nXInp;

	int shInp = 4;
	LPBYTE ptrI;

	int r, g, b;
	int drb, drg;
	int ret = 0;

//	char buff[500];


		yn =idxH;
		nYInp=yn*biWidthInp*shInp;
		xn = idxW;
		nXInp = xn*shInp;
		ptrI = ptrInp+nYInp+nXInp;
			g =*(ptrI+1);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
			r =*ptrI;
			b =*(ptrI+2);
#else
			b =*ptrI;
			r =*(ptrI+2);
#endif
			drb = r-b;
			drg = r-g;
			if(drb > 40 && drg>40 && r > 80 && g< 115 && b < 115) {
				setWhitePTR(ptrI);
				ret = 1;
//	     sprintf(buff,"!!!!!!!!!!!!!!!!!!---------y== %d, x= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",yn, xn, b, g, r, drb, drg);
//	     saveLog("", buff);
			}
//			else{
//	sprintf(buff,"y== %d, x= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",yn, xn, b, g, r, drb, drg);
//     saveLog("", buff);
//			}
         return ret;
}
int testRed24BMP(LPBYTE ptrInp, int biWidthPadInp, int biHeightInp,
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
	if(drb > 40 && drg>40 && r > 80 && g< 115 && b < 115) {
		setWhitePTR(ptrI);
		ret = 1;
	}
    return ret;
}



int testRedInfo(int* tab, int len, int v){
	int i;
	for(i=0;i<len;i++){
		if(tab[i] == v) return i;
	}
	return -1;
}

void setRedInfoV(int x, int y, int th){
    int idx;
	if(y<th){
		if(Red1Idx >= maxElemRed) return;
        idx = testRedInfo(x1Red, Red1Idx, x);
        if(idx<0)
        	{x1Red[Red1Idx] = x; y1Red[Red1Idx] = y;freq1Red[Red1Idx] = 1;Red1Idx++;}
        else
        	{freq1Red[idx]++;}

   	}else{
		if(Red2Idx >= maxElemRed) return;
		idx = testRedInfo(x2Red, Red2Idx, x);
        if(idx<0)
        	{x2Red[Red2Idx] = x; y2Red[Red2Idx] = y;freq2Red[Red2Idx] = 1;Red2Idx++;}
        else
        	{freq2Red[idx]++;}
	}
}
void setRedInfoH(int x, int y, int th){
    int idx;
	if(x<th){
		if(Red1Idx >= maxElemRed) return;
        idx = testRedInfo(y1Red, Red1Idx, y);
        if(idx<0)
        	{y1Red[Red1Idx] = y; x1Red[Red1Idx] = x;freq1Red[Red1Idx] = 1;Red1Idx++;}
        else
        	{freq1Red[idx]++;}
   	}else{
		if(Red2Idx >= maxElemRed) return;
		idx = testRedInfo(y2Red, Red2Idx, y);
        if(idx<0)
        	{y2Red[Red2Idx] = y; x2Red[Red2Idx] = x;freq2Red[Red2Idx] = 1;Red2Idx++;}
        else
        	{freq2Red[idx]++;}
	}
}
void cleanRedBarH_24BMP(LPBYTE ptrInp, int biWidthPadInp, int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	yMin1 = 10000;xMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMin1 > y1Red[i] && freq1Red[i] > 100) {yMin1 = y1Red[i];xMin1 = x1Red[i];}
	}
	yMax1 = 0;xMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMax1 < y1Red[i] && freq1Red[i] > 100) {yMax1 = y1Red[i];xMax1 = x1Red[i];}
	}
//printf("yMin1= %d, xMin1= %d,yMax1= %d, xMax1= %d\n",yMin1,xMin1,yMax1,xMax1);
//fflush(stdout);

yMin2 = 10000;xMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMin2 > y2Red[i] && freq2Red[i] > 100) {yMin2 = y2Red[i];xMin2 = x2Red[i];}
}
yMax2 = 0;xMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMax2 < y2Red[i] && freq2Red[i] > 100) {yMax2 = y2Red[i];xMax2 = x2Red[i];}
}
//printf("yMin2= %d, xMin2= %d,yMax2= %d, xMax2= %d\n",yMin2,xMin2,yMax2,xMax2);
//fflush(stdout);

if(xMin1 < 0 ) return;
if(xMax1 < 0 ) xMax1 = xMin1;
if(xMin2 < 0 ) {xMin2 = xMin1;yMin2 = yMin1;}

{int x1 =min(xMin1, xMax1);
// int x2 =max(xMin2, xMax2);
 double dy;
 int y, j;
 double tangA;
 long nYInp, nXInp;
 int sh = 3;
 LPBYTE ptrI;

 // char buff[500];
//  printf("xMin1= %d, xMax1= %d\n",xMin1, xMax1);

 tangA = 0;
 if(xMin2-xMin1 != 0){
	tangA = (double)(yMin2-yMin1)/(double)(xMin2-xMin1);
 }

// printf("tangA= %e\n",tangA);
     for(i=x1;i<biWidthInp;i++){
	  dy = (double)(i-x1)*tangA;
	  y = (int)((double)yMin1+dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthPadInp;
	     nXInp = i*sh;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
//	   printf("x= %d, dy= %e, y= %d\n",i,dy,y);
// sprintf(buff,"x= %d, dy= %e, y= %d\n",i, dy, y);
// saveLog("", buff);
    }
     for(i=x1-1;i>0;i--){
	  dy = (double)(x1-i)*tangA;
	  y = (int)((double)yMin1-dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthPadInp;
	     nXInp = i*sh;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
 }
 i = xMax2; // to avoid compiler warning
}
void cleanRedBarV_24BMP(LPBYTE ptrInp, int biWidthPadInp,  int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	xMin1 = 10000;yMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMin1 > x1Red[i] && freq1Red[i] > 100) {xMin1 = x1Red[i];yMin1 = y1Red[i];}
	}
	xMax1 = 0;yMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMax1 < x1Red[i] && freq1Red[i] > 100) {xMax1 = x1Red[i];yMax1 = y1Red[i];}
	}
//printf("yMin1= %d, xMin1= %d,yMax1= %d, xMax1= %d\n",yMin1,xMin1,yMax1,xMax1);
//fflush(stdout);

xMin2 = 10000;yMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMin2 > x2Red[i] && freq2Red[i] > 100) {xMin2 = x2Red[i];yMin2 = y2Red[i];}
}
xMax2 = 0;yMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMax2 < x2Red[i] && freq2Red[i] > 100) {xMax2 = x2Red[i];yMax2 = y2Red[i];}
}
//printf("yMin2= %d, xMin2= %d,yMax2= %d, xMax2= %d\n",yMin2,xMin2,yMax2,xMax2);
//fflush(stdout);

if(yMax1 < 0 ) return;
if(yMin1 < 0 ) yMin1 = yMax1;
if(yMax2 < 0 ) {yMax2 = yMax1;xMax2 = xMax1;}

{int y1 =min(yMin1, yMax1);
// int y2 =max(yMin2, yMax2);
 double dx;
 int x, j;
 double tangA;
 long nYInp, nXInp;
 int sh = 3;
 LPBYTE ptrI;

 tangA = 0;
 if(yMax2-yMax1 != 0){
	tangA = (double)(xMax2-xMax1)/(double)(yMax2-yMax1);
 }

     for(i=y1;i<biHeightInp;i++){
	  dx = (double)(i-y1)*tangA;
	  x = (int)((double)xMax1+dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthPadInp;
	     nXInp = j*sh;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
     for(i=y1-1;i>0;i--){
	  dx = (double)(y1-i)*tangA;
	  x = (int)((double)xMax1-dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthPadInp;
	     nXInp = j*sh;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
 }
i = yMin2; // to avoid compiler warning
}

void cleanRedBarH_RGBA(LPBYTE ptrInp, int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	yMin1 = 10000;xMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMin1 > y1Red[i] && freq1Red[i] > 100) {yMin1 = y1Red[i];xMin1 = x1Red[i];}
	}
	yMax1 = 0;xMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMax1 < y1Red[i] && freq1Red[i] > 100) {yMax1 = y1Red[i];xMax1 = x1Red[i];}
	}
//printf("yMin1= %d, xMin1= %d,yMax1= %d, xMax1= %d\n",yMin1,xMin1,yMax1,xMax1);
//fflush(stdout);

yMin2 = 10000;xMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMin2 > y2Red[i] && freq2Red[i] > 100) {yMin2 = y2Red[i];xMin2 = x2Red[i];}
}
yMax2 = 0;xMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMax2 < y2Red[i] && freq2Red[i] > 100) {yMax2 = y2Red[i];xMax2 = x2Red[i];}
}
//printf("yMin2= %d, xMin2= %d,yMax2= %d, xMax2= %d\n",yMin2,xMin2,yMax2,xMax2);
//fflush(stdout);

if(xMin1 < 0 ) return;
if(xMax1 < 0 ) xMax1 = xMin1;
if(xMin2 < 0 ) {xMin2 = xMin1;yMin2 = yMin1;}

{int x1 =min(xMin1, xMax1);
// int x2 =max(xMin2, xMax2);
 double dy;
 int y, j;
 double tangA;
 long nYInp, nXInp;
 int shInp = 4;
 LPBYTE ptrI;

 // char buff[500];
//  printf("xMin1= %d, xMax1= %d\n",xMin1, xMax1);

 tangA = 0;
 if(xMin2-xMin1 != 0){
	tangA = (double)(yMin2-yMin1)/(double)(xMin2-xMin1);
 }

// printf("tangA= %e\n",tangA);
     for(i=x1;i<biWidthInp;i++){
	  dy = (double)(i-x1)*tangA;
	  y = (int)((double)yMin1+dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthInp*shInp;
	     nXInp = i*shInp;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
//	   printf("x= %d, dy= %e, y= %d\n",i,dy,y);
// sprintf(buff,"x= %d, dy= %e, y= %d\n",i, dy, y);
// saveLog("", buff);
    }
     for(i=x1-1;i>0;i--){
	  dy = (double)(x1-i)*tangA;
	  y = (int)((double)yMin1-dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthInp*shInp;
	     nXInp = i*shInp;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
/*
    for(i=0;i<x1;i++){
      for(j=0;j<biHeightInp;j++){
	     nYInp=j*biWidthInp*shInp;
	     nXInp = i*shInp;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
*/
 }
i = xMax2; // to avoid compiler warning
}
void cleanRedBarV_RGBA(LPBYTE ptrInp, int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	xMin1 = 10000;yMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMin1 > x1Red[i] && freq1Red[i] > 100) {xMin1 = x1Red[i];yMin1 = y1Red[i];}
	}
	xMax1 = 0;yMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMax1 < x1Red[i] && freq1Red[i] > 100) {xMax1 = x1Red[i];yMax1 = y1Red[i];}
	}
//printf("yMin1= %d, xMin1= %d,yMax1= %d, xMax1= %d\n",yMin1,xMin1,yMax1,xMax1);
//fflush(stdout);

xMin2 = 10000;yMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMin2 > x2Red[i] && freq2Red[i] > 100) {xMin2 = x2Red[i];yMin2 = y2Red[i];}
}
xMax2 = 0;yMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMax2 < x2Red[i] && freq2Red[i] > 100) {xMax2 = x2Red[i];yMax2 = y2Red[i];}
}
//printf("yMin2= %d, xMin2= %d,yMax2= %d, xMax2= %d\n",yMin2,xMin2,yMax2,xMax2);
//fflush(stdout);

if(yMax1 < 0 ) return;
if(yMin1 < 0 ) yMin1 = yMax1;
if(yMax2 < 0 ) {yMax2 = yMax1;xMax2 = xMax1;}

{int y1 =min(yMin1, yMax1);
// int y2 =max(yMin2, yMax2);
 double dx;
 int x, j;
 double tangA;
 long nYInp, nXInp;
 int shInp = 4;
 LPBYTE ptrI;

 // char buff[500];
// printf("y1 = %d, yMin1= %d, yMax1= %d\n",y1, yMin1, yMax1);

 tangA = 0;
 if(yMax2-yMax1 != 0){
	tangA = (double)(xMax2-xMax1)/(double)(yMax2-yMax1);
 }

     for(i=y1;i<biHeightInp;i++){
	  dx = (double)(i-y1)*tangA;
	  x = (int)((double)xMax1+dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthInp*shInp;
	     nXInp = j*shInp;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
     for(i=y1-1;i>0;i--){
	  dx = (double)(y1-i)*tangA;
	  x = (int)((double)xMax1-dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthInp*shInp;
	     nXInp = j*shInp;
	     ptrI = ptrInp+nYInp+nXInp;
	     setWhitePTR(ptrI);
      }
    }
 }
 i = yMin2; // to avoid compiler warning
}

void cleanRedBarH_RGB565(LPBYTE ptrInp, int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	yMin1 = 10000;xMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMin1 > y1Red[i] && freq1Red[i] > 100) {yMin1 = y1Red[i];xMin1 = x1Red[i];}
	}
	yMax1 = 0;xMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(yMax1 < y1Red[i] && freq1Red[i] > 100) {yMax1 = y1Red[i];xMax1 = x1Red[i];}
	}
yMin2 = 10000;xMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMin2 > y2Red[i] && freq2Red[i] > 100) {yMin2 = y2Red[i];xMin2 = x2Red[i];}
}
yMax2 = 0;xMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(yMax2 < y2Red[i] && freq2Red[i] > 100) {yMax2 = y2Red[i];xMax2 = x2Red[i];}
}
if(xMin1 < 0 ) return;
if(xMax1 < 0 ) xMax1 = xMin1;
if(xMin2 < 0 ) {xMin2 = xMin1;yMin2 = yMin1;}

{int x1 =min(xMin1, xMax1);
// int x2 =max(xMin2, xMax2);
 double dy;
 int y, j;
 double tangA;
 long nYInp, nXInp;
 WORD wr;
 WORD *ptrI;

 // char buff[500];
//  printf("xMin1= %d, xMax1= %d\n",xMin1, xMax1);

 tangA = 0;
 if(xMin2-xMin1 != 0){
	tangA = (double)(yMin2-yMin1)/(double)(xMin2-xMin1);
 }
     wr =  whiteRGB565;
     for(i=x1;i<biWidthInp;i++){
	  dy = (double)(i-x1)*tangA;
	  y = (int)((double)yMin1+dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthInp;
	     nXInp = i;
	     ptrI = (WORD*)ptrInp+nYInp+nXInp;
	     *ptrI = wr;
      }
    }
     for(i=x1-1;i>0;i--){
	  dy = (double)(x1-i)*tangA;
	  y = (int)((double)yMin1-dy+0.5);
      for(j=y;j<biHeightInp;j++){
	     nYInp=j*biWidthInp;
	     nXInp = i;
	     ptrI = (WORD*)ptrInp+nYInp+nXInp;
	     *ptrI = wr;
      }
    }
 }
i = xMax2; // to avoid compiler warning
}
void cleanRedBarV_RGB565(LPBYTE ptrInp, int biWidthInp, int biHeightInp){
	int yMin1, yMax1, xMin1, xMax1;
	int yMin2, yMax2, xMin2, xMax2;
	int i;

	xMin1 = 10000;yMin1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMin1 > x1Red[i] && freq1Red[i] > 100) {xMin1 = x1Red[i];yMin1 = y1Red[i];}
	}
	xMax1 = 0;yMax1 = -1;
	for(i=0;i<Red1Idx;i++){
		if(xMax1 < x1Red[i] && freq1Red[i] > 100) {xMax1 = x1Red[i];yMax1 = y1Red[i];}
	}
xMin2 = 10000;yMin2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMin2 > x2Red[i] && freq2Red[i] > 100) {xMin2 = x2Red[i];yMin2 = y2Red[i];}
}
xMax2 = 0;yMax2 = -1;
for(i=0;i<Red2Idx;i++){
	if(xMax2 < x2Red[i] && freq2Red[i] > 100) {xMax2 = x2Red[i];yMax2 = y2Red[i];}
}

if(yMax1 < 0 ) return;
if(yMin1 < 0 ) yMin1 = yMax1;
if(yMax2 < 0 ) {yMax2 = yMax1;xMax2 = xMax1;}

{int y1 =min(yMin1, yMax1);
// int y2 =max(yMin2, yMax2);
 double dx;
 int x, j;
 double tangA;
 long nYInp, nXInp;
 WORD wr;
 WORD *ptrI;

 // char buff[500];
// printf("y1 = %d, yMin1= %d, yMax1= %d\n",y1, yMin1, yMax1);

 tangA = 0;
 if(yMax2-yMax1 != 0){
	tangA = (double)(xMax2-xMax1)/(double)(yMax2-yMax1);
 }
     wr =  whiteRGB565;
     for(i=y1;i<biHeightInp;i++){
	  dx = (double)(i-y1)*tangA;
	  x = (int)((double)xMax1+dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthInp;
	     nXInp = j;
	     ptrI = (WORD*)ptrInp+nYInp+nXInp;
	     *ptrI = wr;
      }
    }
     for(i=y1-1;i>0;i--){
	  dx = (double)(y1-i)*tangA;
	  x = (int)((double)xMax1-dx+0.5);
      for(j=0;j<x;j++){
	     nYInp=i*biWidthInp;
	     nXInp = j;
	     ptrI = (WORD*)ptrInp+nYInp+nXInp;
	     *ptrI = wr;

      }
    }
 }
 i = yMin2; // to avoid compiler warning
}

void filterRedLeftRGB565(LPIMGDATA img){

		int idxW, idxH;
		int biWidthInp, biHeightInp;
		int flgHor;
		int dd, ret;


		Red1Idx = 0; Red2Idx = 0;

		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;

		flgHor = 0;
	    if(biWidthInp > biHeightInp)  flgHor = 1;



if(flgHor > 0) {
	    dd = biHeightInp/2;
//	    xx = 620;
		for(idxH=dd;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<biWidthInp;idxW++){
//		for(idxH=dd;idxH<biHeightInp;idxH++){
//			for(idxW=xx-5;idxW<xx+5;idxW++){
				ret = testRedRGB565(img->pixPtr, biWidthInp,  biHeightInp, idxW, idxH);
				if(ret>0)
				  setRedInfoH(idxW, idxH, biWidthInp/2);

			}
		}
       cleanRedBarH_RGB565(img->pixPtr, biWidthInp,  biHeightInp);
}

else{
	   dd = biWidthInp/2;
		for(idxH=0;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<dd;idxW++){
				ret = testRedRGB565(img->pixPtr, biWidthInp,  biHeightInp, idxW, idxH);
				if(ret>0)
					setRedInfoV(idxW, idxH, biHeightInp/2);

			}
		}
	    cleanRedBarV_RGB565(img->pixPtr, biWidthInp,  biHeightInp);
 }

}

void filterRedLeftRGBA(LPIMGDATA img){

		int idxW, idxH;
		int biWidthInp, biHeightInp;
		int flgHor;
		int dd, ret;

//char buff[500];
		Red1Idx = 0; Red2Idx = 0;

		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;

		flgHor = 0;
	    if(biWidthInp > biHeightInp)  flgHor = 1;


if(flgHor > 0) {
//	    dd = biHeightInp/2+biHeightInp/4;
	    dd = biHeightInp/2;
		for(idxH=dd;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<biWidthInp;idxW++){
				ret = testRedRGBA(img->pixPtr, biWidthInp,  biHeightInp, idxW, idxH);
				if(ret>0)
				  setRedInfoH(idxW, idxH, biWidthInp/2);
			}
		}
       cleanRedBarH_RGBA(img->pixPtr, biWidthInp,  biHeightInp);
}
else{
//	   dd = biWidthInp/2+biWidthInp/4;
	   dd = biWidthInp/2;
		for(idxH=0;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<dd;idxW++){
				ret = testRedRGBA(img->pixPtr, biWidthInp,  biHeightInp, idxW, idxH);
				if(ret>0)
					setRedInfoV(idxW, idxH, biHeightInp/2);
			}
		}
  cleanRedBarV_RGBA(img->pixPtr, biWidthInp,  biHeightInp);

}
}
void filterRedLeft_24BMP(LPIMGDATA img){

		int idxW, idxH;
		int biWidthInp, biWidthPadInp, biHeightInp;
		int flgHor;
		int dd, ret;

//char buff[500];
		Red1Idx = 0; Red2Idx = 0;

		biWidthInp=img->bi.biWidth;
		biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
		biHeightInp = img->bi.biHeight;


		flgHor = 0;
	    if(biWidthInp > biHeightInp)  flgHor = 1;


if(flgHor > 0) {
//	    dd = biHeightInp/2+biHeightInp/4;
	    dd = biHeightInp/2;
		for(idxH=dd;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<biWidthInp;idxW++){
				ret = testRed24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
				if(ret>0)
				  setRedInfoH(idxW, idxH, biWidthInp/2);
			}
		}
    cleanRedBarH_24BMP(img->pixPtr, biWidthPadInp, biWidthInp,  biHeightInp);
}
else{
//	   dd = biWidthInp/2+biWidthInp/4;
	   dd = biWidthInp/2;
		for(idxH=0;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<dd;idxW++){
				ret = testRed24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
				if(ret>0)
					setRedInfoV(idxW, idxH, biHeightInp/2);
			}
		}
  cleanRedBarV_24BMP(img->pixPtr, biWidthPadInp, biWidthInp,  biHeightInp);

}

}

void filterPARedRGBA(LPIMGDATA img, int grid, int side){
//		LPBITMAPINFOHEADER bi;

		int sizeW, sizeH, lastW, lastH;
		int idxW, idxH;
		int gridW, gridH;
		int biWidthInp, biHeightInp;
		int maxH, maxW;
		int mrg = 8;


		biWidthInp=img->bi.biWidth;
		biHeightInp = img->bi.biHeight;

		sizeW = img->bi.biWidth/grid;
		lastW = img->bi.biWidth - sizeW*grid;
		if(lastW > 0)
			sizeW++;
		else
			lastW = grid;

		sizeH = img->bi.biHeight/grid;
		lastH = img->bi.biHeight - sizeH*grid;
		if(lastH > 0)
			sizeH++;
		else
			lastH = grid;

if(side ==0) {
		maxH  = mrg; if(maxH > sizeH) maxH = sizeH;
		for(idxH=0;idxH<maxH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
			}
		}
}

if(side ==0 || side == 1) {
		maxH  = sizeH - mrg; if(maxH < 0) maxH = 0;
		for(idxH=maxH;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
			}
		}
}
if(side ==0 || side == 1) {
		maxW  = mrg; if(maxW > sizeW) maxW = sizeW;
		for(idxH=0;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=0;idxW<maxW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
			}
		}
}
if(side ==0) {
		maxW  = sizeW - mrg; if(maxW < 0) maxW = 0;
		for(idxH=0;idxH<sizeH;idxH++){
			gridH = grid;
			if(idxH == sizeH-1) gridH = lastH;
			for(idxW=maxW;idxW<sizeW;idxW++){
				gridW = grid;
				if(idxW == sizeW-1) gridW = lastW;
				convertRGBARedToWhite(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
			}
		}
 }
}
void convertRGB565RedToWhite(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                     int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	WORD *ptrI;

	int r, g, b;
	WORD wr;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=yn*biWidthInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			//				nXInp = xn*shInp;
			nXInp = xn;
			ptrI = (WORD*)ptrInp+nYInp+nXInp;

			wr =  *ptrI;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );

            if(isItPAred(b, g, r, th) == 1){
    			wr =  whiteRGB565;
    			*ptrI = wr;
            }
		}
	}
}

float getColorValueRGB565(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		                     int idxW, int idxH, int grid, int gridW, int gridH, int th){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;

	WORD *ptrI;

	int r, g, b;
	WORD wr;
	float value = 0;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		nYInp=yn*biWidthInp;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			//				nXInp = xn*shInp;
			nXInp = xn;
			ptrI = (WORD*)ptrInp+nYInp+nXInp;

			wr =  *ptrI;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );

            if(isItPAred(b, g, r, th) == 1){
    			wr =  whiteRGB565;
    			*ptrI = wr;
            }

			value += (r+ g + b);
		}
	}
	value /= (float)(gridH*gridW);
	return value;

}


void filterCARedRGB565(LPIMGDATA img, int grid){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int thClr;
	int minH, maxH;

	int cc, i;
	int av[4];


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;


	cc = 0;
   for(i=0;i<4;i++)
	   av[i] = 0;
	minH = 0;
	maxH = sizeH/3;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvRGB565(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;
		}
	}

	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 6;
    else thClr = 7;
	}
//  printf("---thClr= %d\n", thClr);
//	thClr = 50;
	minH = 0;
	maxH = sizeH/3;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
		}
	}


	minH = maxH;
	maxH = (4*sizeH)/5;
	cc = 0;
   for(i=0;i<4;i++)
	   av[i] = 0;

	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvRGB565(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;

		}
	}
	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 7;
    else thClr = 8;
	}


//	thClr = 90;
	minH = maxH;
	maxH = (4*sizeH)/5;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
		}
	}


	minH = maxH;
	maxH = sizeH;
    cc = 0;
    for(i=0;i<4;i++)
	   av[i] = 0;

	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=8;idxW<sizeW-8;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			getNextAvRGB565(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, av);
			cc++;

		}
	}
	if(cc > 0){
	   for(i=0;i<4;i++)
		   av[i] /= cc;
	}
	{
     int max = 0, iMax = 0;
       for(i=0;i<4;i++){
//    printf("---i= %d, av= %d\n", i, av[i]);
        if(max < av[i]) {max = av[i];iMax = i;}
        }
    if(iMax == 0) thClr = 6;
    else thClr = 7;
	}

//	thClr = 60;
	minH = maxH;
	maxH = sizeH;
	for(idxH=minH;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, thClr );
		}
	}

}

void filterPARedRGB565(LPIMGDATA img, int grid, int side){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int maxH, maxW;
	int mrg = 8;


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

if(side == 0) {
	maxH  = mrg; if(maxH > sizeH) maxH = sizeH;
	for(idxH=0;idxH<maxH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side==0 || side == 1) {
	maxH  = sizeH - mrg; if(maxH < 0) maxH = 0;
	for(idxH=maxH;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side==0 || side == 1) {
	maxW  = mrg; if(maxW > sizeW) maxW = sizeW;
	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<maxW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp, biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
}
if(side==0) {
	maxW  = sizeW - mrg; if(maxW < 0) maxW = 0;
	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=maxW;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			convertRGB565RedToWhite(img->pixPtr, biWidthInp,  biHeightInp,idxW, idxH, grid, gridW, gridH, 70);
		}
	}
 }
}
float setkFactor(float *kFactor, int idxW, int sizeW, int idxH, int sizeH){
	int w2 = sizeW/2;
	int h2 = sizeH/2;
	if(idxW <= w2 && idxH <= h2) return kFactor[0];
	if(idxW <= w2 && idxH > h2) return kFactor[1];
	if(idxW > w2 && idxH <= h2) return kFactor[2];
	if(idxW > w2 && idxH > h2) return kFactor[3];
	return kFactor[0];

}


void convert24BMPtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor){
	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	float k = kFactor;

	int   R = 128;

	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	convertBMPTo_mono_BMP_init(img, imgOut);
	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			tH = m*(1 + k*(s/R-1));
			convertBMPTo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}


	//horizontal histogram to check if it is too dark
	{
		int x1, x2, y1, y2;
		int xC, yC;
		int i, zeroCount, lowCount;
		int maxZero, zeroFlag;
		int *ptrH;


		//	  	      sprintf(buff,"---------imgOut->bi.biWidth= %d, imgOut->bi.biHeight= %d\n",imgOut->bi.biWidth, imgOut->bi.biHeight);
		//	   	      saveLog("", buff);

		//	 		 if(imgOut->bi.biWidth > imgOut->bi.biHeight)
		//	 		    rotate(imgOut, 90);
		//	 		   biWidthOut=imgOut->bi.biWidth;
		biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
		biHeightOut = imgOut->bi.biHeight;


		//	  	      sprintf(buff,"2---------imgOut->bi.biWidth= %d, imgOut->bi.biHeight= %d\n",imgOut->bi.biWidth, imgOut->bi.biHeight);
		//	   	      saveLog("", buff);

		xC = (imgOut->bi.biWidth*imgOut->bi.biBitCount)/2;
		yC = imgOut->bi.biHeight/2;

		x1 = xC-xC/2;
		x2 = xC+xC/2;
		y1 = yC-(2*yC)/3;
		y2 = yC;
		histHTest_X1_X1(imgOut->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1, y2, hHistPreProc);
		histVPtrPix(imgOut->pixPtr, biWidthPadOut, biHeightOut, H_SIZE_PREPROC, x1, x2, y1, y2, vHistPreProc);
		ptrH = hHistPreProc;
		if(imgOut->bi.biWidth > imgOut->bi.biHeight)
			ptrH = vHistPreProc;

		zeroCount = 0;
		lowCount = 0;
		zeroFlag = 0;
		maxZero = 0;
		for(i=1;i<=ptrH[0];i++) {
			//   	  	      sprintf(buff,"h---i= %d, ptrH[i]= %d\n",i, ptrH[i]);
			//   	   	      saveLog("", buff);
			if(ptrH[i] == 0){
				zeroCount++;
				zeroFlag = 1;
			}
			else{
				if(zeroFlag == 1) {
					zeroFlag = 0;
					if(zeroCount > maxZero)
						maxZero = zeroCount;
					zeroCount = 0;
				}
				if(ptrH[i] < 10)
					lowCount++;
			}
		}
		//    	  	      sprintf(buff,"---------maxZero= %d, lowCount= %d\n",maxZero, lowCount);
		//    	   	      saveLog("", buff);

		//	   	  	      sprintf(buff,"biWidthPadOut= %d, imgOut->bi.biHeight= %d, x2-x1-1 = %d\n",biWidthPadOut, imgOut->bi.biHeight, x2-x1-1);
		//	   	   	      saveLog("", buff);

		//	   	  	      sprintf(buff,"x1= %d, x2= %d, y1 = %d, y2= %d\n",x1, x2, y1, y2);
		//	   	   	      saveLog("", buff);



		if(lowCount > 50 && maxZero < 80){
			// do again conversion changing threshold
			k = 0.2;
			for(idxH=0;idxH<sizeH;idxH++){
				gridH = grid;
				if(idxH == sizeH-1) gridH = lastH;
				for(idxW=0;idxW<sizeW;idxW++){
					gridW = grid;
					if(idxW == sizeW-1) gridW = lastW;
					m = *(lpImgMap_m+idxH*sizeW+ idxW);
					s = *(lpImgMap_s+idxH*sizeW+ idxW);
					tH = m*(1 + k*(s/R-1));
					convertBMPTo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,
							imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
							idxW, idxH, grid, gridW, gridH, tH);
				}
			}


		}
	}

	if( 1 || imgOut->bi.biWidth > 1400)
		convertDPI_in_BMP(imgOut, 300, 300);
	else{
		imgOut->bi.biXPelsPerMeter=300;
		imgOut->bi.biYPelsPerMeter=300;
		bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
	}
}
void convert24BMPtoMonochromeGridSmall(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
//	int gridW, gridH;
	int x, y, xn, yn;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	float k = kFactor;
	int   R = 128;



	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	for(y=0;y<biHeightOut;y++){
		yn = y_1+y;
		for(x=0;x<biWidthOut;x++){
			xn = x_1+x;
			idxW = xn/grid;
			idxH = yn/grid;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			tH = m*(1 + k*(s/R-1));

			convert24BMPTo_mono_BMPLum_OnePixel(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					xn, yn, tH, x, y);
		}
	}
}

void convertRGBAtoMonochromeGridSmall_faster(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
//	int gridW, gridH;
	int x, y, xn, yn;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, tH; //, s;
//	int   R = 128;
	float k = kFactor;
//	float kR = k/(float)R;



	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

//	convertBMPTo_mono_BMP_init(img, imgOut);
	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	for(y=0;y<biHeightOut;y++){
		yn = y_1+y;
		idxH = yn/grid;
		idxHW = idxH*sizeW;
		for(x=0;x<biWidthOut;x++){
			xn = x_1+x;
			idxW = xn/grid;
			m = *(lpImgMap_m+idxHW+ idxW);
//			s = *(lpImgMap_s+idxHW+ idxW);
//			sR = *(lpImgMap_s_r+idxHW+ idxW);
//			tH = m*(1 + k*(s/R-1));
//			tH = (int)((float)m*(1.0 + kR*(float)sR));
//			tH = (int)((float)m*(1.0 + k*((float)s/(float)R-1.0)));
			tH = m*(1 - k);
//			printf("-- tH= %d, m = %d, s= %d, R= %d, k= %e\n", tH,m,s,R,k);

			convertRGBATo_mono_BMPLum_OnePixel(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					xn, yn, tH, x, y);
		}
	}

}

void convert24BMPtoMonochromeGrid2_CA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biWidthPadInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	float k;
	int   R = 128;
    int   cX1, cX2, cY1, cY2;
    int   ccX_2;


	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;
	if(biWidthInp >biHeightInp){
	   cX1 = sizeW/2-4;
	   cX2 = sizeW/2 + 8;
	   cY1 = sizeH/2 - 5;
	   cY2 = sizeH/2 + 7;
	   ccX_2 = sizeH/2;
	}else{
		   cX1 = sizeW/2 - 7;
		   cX2 = sizeW/2 + 5;
		   cY1 = sizeH/2 - 4;
		   cY2 = sizeH/2 + 8;
		   ccX_2 = sizeW/2;
	}

//printf("---------cX1= %d, cX2= %d, sizeW= %d, cY1= %d, cY2= %d, sizeH= %d\n",cX1, cX2, sizeW, cY1, cY2, sizeH);
//	  printf("---------kFactor= %e\n",kFactor);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			k = kFactorL;
			if(biWidthInp >biHeightInp){
				if(idxH > ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}else{
				if(idxW < ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}

			if(idxW > cX1 && idxW < cX2 && idxH > cY1 && idxH < cY2){
				if(k > 0.37) k -= 0.1;
				else  k -= 0.06;
			}

			tH = m*(1 + k*(s/R-1));
//if(loop < 100)
//    printf("---------idxH= %d, idxW= %d, k= %e, R= %d, tH= %d, m= %d, s= %d\n",idxH, idxW, k, R, tH, m, s);
//loop++;

			convertBMPTo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biWidthPadInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}
}
void convertRGBAtoMonochromeGrid2_CA_faster(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
//	int m, s, tH;
	int m, tH;
	float k;
//	int   R = 128;
    int   cX1, cX2, cY1, cY2;
    int   ccX_2;


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;
	if(biWidthInp >biHeightInp){
	   cX1 = sizeW/2-4;
	   cX2 = sizeW/2 + 8;
	   cY1 = sizeH/2 - 5;
	   cY2 = sizeH/2 + 7;
	   ccX_2 = sizeH/2;
	}else{
		   cX1 = sizeW/2 - 7;
		   cX2 = sizeW/2 + 5;
		   cY1 = sizeH/2 - 4;
		   cY2 = sizeH/2 + 8;
		   ccX_2 = sizeW/2;
	}


//printf("---------cX1= %d, cX2= %d, sizeW= %d, cY1= %d, cY2= %d, sizeH= %d\n",cX1, cX2, sizeW, cY1, cY2, sizeH);
//	  printf("---------kFactor= %e\n",kFactor);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		idxHW= idxH*sizeW;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxHW+ idxW);
//			s = *(lpImgMap_s+idxHW+ idxW);
			k = kFactorL;
			if(biWidthInp >biHeightInp){
				if(idxH > ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}else{
				if(idxW < ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}

			if(idxW > cX1 && idxW < cX2 && idxH > cY1 && idxH < cY2){
				if(k > 0.37) k -= 0.1;
				else  k -= 0.06;
			}

//			tH = (int)((float)m*(1.0 + k*((float)s/(float)R-1.0)));
			tH = m*(1 - k);

//if(loop < 100)
//    printf("---------idxH= %d, idxW= %d, k= %e, R= %d, tH= %d, m= %d, s= %d\n",idxH, idxW, k, R, tH, m, s);
//loop++;

			convertRGBATo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}
}

void convertRGB565toMonochromeGrid2_CA(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactorL, float kFactorR){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
//	int m, s, tH;
	int m, tH;
	float k;
//	int   R = 32;
    int   cX1, cX2, cY1, cY2;
    int   ccX_2;


	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;
	if(biWidthInp >biHeightInp){
	   cX1 = sizeW/2-4;
	   cX2 = sizeW/2 + 8;
	   cY1 = sizeH/2 - 5;
	   cY2 = sizeH/2 + 7;
	   ccX_2 = sizeH/2;
	}else{
		   cX1 = sizeW/2 - 7;
		   cX2 = sizeW/2 + 5;
		   cY1 = sizeH/2 - 4;
		   cY2 = sizeH/2 + 8;
		   ccX_2 = sizeW/2;
	}

//printf("---------cX1= %d, cX2= %d, sizeW= %d, cY1= %d, cY2= %d, sizeH= %d\n",cX1, cX2, sizeW, cY1, cY2, sizeH);
//	  printf("---------kFactor= %e\n",kFactor);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		idxHW= idxH*sizeW;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxHW+ idxW);
//			s = *(lpImgMap_s+idxHW+ idxW);
			k = kFactorL;
			if(biWidthInp >biHeightInp){
				if(idxH > ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}else{
				if(idxW < ccX_2)
					k = kFactorL;
				else
					k = kFactorR;
			}
//			k = kFactor;

			if(idxW > cX1 && idxW < cX2 && idxH > cY1 && idxH < cY2){
				if(k > 0.37) k -= 0.1;
				else  k -= 0.06;
			}

//			tH = m*(1 + k*(s/R-1));
			tH = m*(1 - k);
//if(loop < 100)
//    printf("---------idxH= %d, idxW= %d, k= %e, R= %d, tH= %d, m= %d, s= %d\n",idxH, idxW, k, R, tH, m, s);
//loop++;

			convertRGB565To_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}
}

void convertRGB565toMonochromeGridSmall(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor,
		int x_1, int y_1){
//	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH, idxHW;
	int idxW, idxH;
//	int gridW, gridH;
	int x, y, xn, yn;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
//	int m, s, tH;
	int m,  tH;
	float k = kFactor;
//	int   R = 32;



	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;

	for(y=0;y<biHeightOut;y++){
		yn = y_1+y;
		idxH = yn/grid;
		idxHW = idxH*sizeW;
		for(x=0;x<biWidthOut;x++){
			xn = x_1+x;
			idxW = xn/grid;
			m = *(lpImgMap_m+idxHW+ idxW);
//			s = *(lpImgMap_s+idxHW+ idxW);
//			tH = m*(1 + k*(s/R-1));
			tH = m*(1 - k);

			convertRGB565To_mono_BMPLum_OnePixel(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					xn, yn, tH, x, y);
		}
	}
}



int blackImage_K(int xInp, int yInp, int biWidthOut, int biHeightOut, float *kTab, int gridK){
  int pixXb = biWidthOut/gridK, pixYb = biHeightOut/gridK;
  int ix, iy;
  float tH = 0.005;

//  	char buff[500];

    ix = xInp/pixXb;
    iy = yInp/pixYb;

//   sprintf(buff," xInp= %d, yInp= %d, ix= %d, iy= %d, iy*gridK+ix= %d, kTab[iy*gridK+ix]= %e\n",xInp, yInp, ix, iy, iy*gridK+ix, kTab[iy*gridK+ix]);
//   saveLog("", buff);

    if(kTab[iy*gridK+ix] > tH) return 1;
    return 0;
}


void convertRGBAtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor){
	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	//	    int sh = 3;
	//	    float k = 0.07;
	//	    float k = 0.1;
//	float k = 0.15;
	float k = kFactor;
	int   R = 128;
	//	    int   R = 500;
	//   	    char buff[500];
//	int loop = 0;



	biWidthInp=img->bi.biWidth;
	//		biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	convertBMPTo_mono_BMP_init(img, imgOut);
	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;


	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			tH = m*(1 + k*(s/R-1));
//if(loop < 100)
//    printf("---------idxH= %d, idxW= %d, k= %e, R= %d, tH= %d, m= %d, s= %d\n",idxH, idxW, k, R, tH, m, s);
//loop++;

			convertRGBATo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}

	//horizontal histogram to check if it is too dark
	{
		int x1, x2, y1, y2;
		int xC, yC;
		int i, zeroCount, lowCount;
		int maxZero, zeroFlag;
		int *ptrH;


		//	  	      sprintf(buff,"---------imgOut->bi.biWidth= %d, imgOut->bi.biHeight= %d\n",imgOut->bi.biWidth, imgOut->bi.biHeight);
		//	   	      saveLog("", buff);

		//	 		 if(imgOut->bi.biWidth > imgOut->bi.biHeight)
		//	 		    rotate(imgOut, 90);
		//	 		   biWidthOut=imgOut->bi.biWidth;
		biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
		biHeightOut = imgOut->bi.biHeight;


		//	  	      sprintf(buff,"2---------imgOut->bi.biWidth= %d, imgOut->bi.biHeight= %d\n",imgOut->bi.biWidth, imgOut->bi.biHeight);
		//	   	      saveLog("", buff);

		xC = (imgOut->bi.biWidth*imgOut->bi.biBitCount)/2;
		yC = imgOut->bi.biHeight/2;

		x1 = xC-xC/2;
		x2 = xC+xC/2;
		y1 = yC-(2*yC)/3;
		y2 = yC;
		histHTest_X1_X1(imgOut->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1, y2, hHistPreProc);
		histVPtrPix(imgOut->pixPtr, biWidthPadOut, biHeightOut, H_SIZE_PREPROC, x1, x2, y1, y2, vHistPreProc);
		ptrH = hHistPreProc;
		if(imgOut->bi.biWidth > imgOut->bi.biHeight)
			ptrH = vHistPreProc;

		zeroCount = 0;
		lowCount = 0;
		zeroFlag = 0;
		maxZero = 0;
		for(i=1;i<=ptrH[0];i++) {
			//   	  	      sprintf(buff,"h---i= %d, ptrH[i]= %d\n",i, ptrH[i]);
			//   	   	      saveLog("", buff);
			if(ptrH[i] == 0){
				zeroCount++;
				zeroFlag = 1;
			}
			else{
				if(zeroFlag == 1) {
					zeroFlag = 0;
					if(zeroCount > maxZero)
						maxZero = zeroCount;
					zeroCount = 0;
				}
				if(ptrH[i] < 10)
					lowCount++;
			}
		}
		//    	  	      sprintf(buff,"---------maxZero= %d, lowCount= %d\n",maxZero, lowCount);
		//    	   	      saveLog("", buff);

		//	   	  	      sprintf(buff,"biWidthPadOut= %d, imgOut->bi.biHeight= %d, x2-x1-1 = %d\n",biWidthPadOut, imgOut->bi.biHeight, x2-x1-1);
		//	   	   	      saveLog("", buff);

		//	   	  	      sprintf(buff,"x1= %d, x2= %d, y1 = %d, y2= %d\n",x1, x2, y1, y2);
		//	   	   	      saveLog("", buff);


		if(lowCount > 50 && maxZero < 80){
			// do again conversion changing threshold
			k = 0.2;
			for(idxH=0;idxH<sizeH;idxH++){
				gridH = grid;
				if(idxH == sizeH-1) gridH = lastH;
				for(idxW=0;idxW<sizeW;idxW++){
					gridW = grid;
					if(idxW == sizeW-1) gridW = lastW;
					m = *(lpImgMap_m+idxH*sizeW+ idxW);
					s = *(lpImgMap_s+idxH*sizeW+ idxW);
					tH = m*(1 + k*(s/R-1));
					convertRGBATo_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
							imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
							idxW, idxH, grid, gridW, gridH, tH);
				}
			}


		}
	}

	if( 1 || imgOut->bi.biWidth > 1400)
		convertDPI_in_BMP(imgOut, 300, 300);
	else{
		imgOut->bi.biXPelsPerMeter=300;
		imgOut->bi.biYPelsPerMeter=300;
		bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
	}

}
void convertRGB565To_mono_BMPLum_Next(LPBYTE ptrInp, int biWidthInp, int biHeightInp,
		LPBYTE ptrOut, int biWidthOut, int biWidthPadOut, int biHeightOut,
		int idxW, int idxH, int grid, int gridW, int gridH, int tH){

	int iW, iH;
	int xna, yna;
	int xn, yn;
	long nYInp, nXInp;
	long nYOut, nXOut;

	//	   int shInp = 2;
	LPBYTE  ptrO;
	WORD *ptrI;
	int v;
	BYTE mask;
	//	   int  max;
	int r, g, b;
	WORD wr;

	yna = idxH*grid;
	xna = idxW*grid;

	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		//			nYInp=yn*biWidthInp*shInp;
		nYInp=yn*biWidthInp;
		nYOut=(biHeightOut-1-yn)*biWidthPadOut;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			//				nXInp = xn*shInp;
			nXInp = xn;
			nXOut = xn/8;
			ptrI = (WORD*)ptrInp+nYInp+nXInp;

			wr =  *ptrI;
			b = ((wr & eRGB565_R ) >> 11);
			g = ((wr & eRGB565_G) ) >> 5;
			r = (wr & eRGB565_B );

			//		        b =*ptrI;
			//		        g =*(ptrI+1);
			//		        r =*(ptrI+2);

			//		        r =*ptrI;
			//		        g =*(ptrI+1);
			//		        b =*(ptrI+2);
			v = computeLuminosity(r, g, b);
			if(v < tH) v= 0; else v = 255;

			ptrO = ptrOut+nYOut+nXOut;
			mask=LEFT_BIT;
			mask >>=(xn%8);
			if(v!=0){
				*ptrO = *ptrO | mask;
			}
			else {
				*ptrO = *ptrO & (~mask);
			}

		}
	}

}
extern     int       gr_ar[];
int byteMass(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int y1, int y2)
{
	int  yy, yD, ind;
	int lByte, w_bt;
	long nY;
	BYTE *hptr;
	int mass = 0;

	w_bt= widthImg;
	lByte=x1/8;
	for(yy=y1;yy<=y2;yy++) {
		yD=yy;
		yD= hightImg-1-yD;
		nY=(long)yD*(long)w_bt;
		hptr=ptrPix+nY+(long)lByte;
        ind=(int)(*hptr);
        if(ind<0) {ind+=256;}
        ind=255-ind;
		mass += gr_ar[ind];
	}
	return  mass;
}/* of byteMass()*/
int byteMassY(LPBYTE ptrPix, int widthImg, int hightImg, int y1, int x1, int x2)
{
	int  xx, yD, ind;
	int w_bt;
	long nY;
	BYTE *hptr;
	int mass = 0;

	w_bt= widthImg;
	yD=y1;
	yD= hightImg-1-yD;
	nY=(long)yD*(long)w_bt;
	for(xx=x1/8;xx<=(x2/8);xx+=8) {
		hptr=ptrPix+nY+(long)xx;
        ind=(int)(*hptr);
        if(ind<0) {ind+=256;}
        ind=255-ind;
		mass += gr_ar[ind];
	}
	return  mass;
}/* of byteMassY()*/
int byteMassXY(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2)
{
	int xx;
	int  yy, yD, ind;
	int lByte1, lByte2, w_bt;
	long nY;
	BYTE *hptr;
	int mass = 0;

	w_bt= widthImg;
	lByte1=x1/8;
	lByte2=x2/8;
	for(yy=y1;yy<=y2;yy++) {
		yD=yy;
		yD= hightImg-1-yD;
		nY=(long)yD*(long)w_bt;
		for(xx=lByte1;xx<=lByte2;xx++) {
		   hptr=ptrPix+nY+(long)xx;
           ind=(int)(*hptr);
           if(ind<0) {ind+=256;}
           ind=255-ind;
		   mass += gr_ar[ind];
		}
	}
	return  mass;
}/* of byteMass()*/

void convertRGB565toMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor){
	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	int iMonoLoop;
	float k = kFactor, dk = 0;
//	int   R = 128;
	int   R = 32;
	int tHT1 = 3, tHT2 = 10;

//	char buff[500];

	biWidthInp=img->bi.biWidth;
	//		biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	convertBMPTo_mono_BMP_init(img, imgOut);
	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;




  for(iMonoLoop=0;iMonoLoop<6;iMonoLoop++){


//	  sprintf(buff," kLT= %e, kLB= %e kRT= %e, kRB= %e\n ",kFactor[0]+dk, kFactor[1]+dk, kFactor[2]+dk, kFactor[3]+dk);
//	  saveLog("", buff);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
//			k = setkFactor(kFactor, idxW, sizeW, idxH, sizeH);
			k = kFactor;
			k += dk;
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			tH = m*(1 + k*(s/R-1));
			convertRGB565To_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}
// test background
	{
		int x1, y1, y2;
		int xC, yC;
		int i, iMass;
		int massTab[10];
		int cc, ccT, flg;

//		  char buff[500];

		  xC = (imgOut->bi.biWidth*imgOut->bi.biBitCount)/2;
		  yC = imgOut->bi.biHeight/2;
		  y1 = yC-(2*yC)/3;
		  y2 = yC;
		  iMass = 0;
		  for(x1=40;x1<xC;x1+=40){
			if(biWidthOut > biHeightOut)
			    massTab[iMass] = byteMassY(imgOut->pixPtr, biWidthPadOut, biHeightOut, biHeightOut-x1, xC-(2*xC)/3,  xC);
			else
				massTab[iMass] = byteMass(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, y1, y2);
			 iMass++;
			 if(iMass> 6) break;
		  }

//		  sprintf(buff,"---------------------------------mass, k= %e\n", k);
//		  saveLog("", buff);
//		  for(i=0;i<iMass;i++){
//			  printf("---------mass[ %d ]=  %d\n",i, massTab[i]);
//			sprintf(buff,"---------mass[ %d ]=  %d\n",i, massTab[i]);
//			saveLog("", buff);
//		  }
// first test
		  cc = 0;ccT = 0;flg = 1;
		  for(i=0;i<iMass;i++){
			  if(massTab[i] < tHT2) {
				  ccT++;
				  if(flg>0)
				    cc++;
			  }
			  else
				  flg = 0;
		  }
		  if(iMonoLoop == 0 && iMass > 6 && (cc > tHT1 || ccT > tHT1)){
			  if(massTab[4]>40 && massTab[5]>40 && massTab[6]>40)
				  {cc = 0;ccT = 0;}
		  }

// printf("----------------------------iMass= %d, cc= %d, ccT= %d\n", iMass, cc, ccT);
//fflush(stdout);
		  if(cc > tHT1 || ccT > tHT1) break;
		    dk += 0.05;
		    if(iMonoLoop == 2) tHT1 = 2;
	      tHT2++;

	}

  }// iMonoLoop


	if(1 || imgOut->bi.biWidth > 1400)
		convertDPI_in_BMP(imgOut, 300, 300);
	else{
		imgOut->bi.biXPelsPerMeter=300;
		imgOut->bi.biYPelsPerMeter=300;
		bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
	}
}
void convertRGB565toMonochromeGrid_sOff(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor){
	LPBITMAPINFOHEADER bi;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int biWidthInp, biHeightInp;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int m, s, tH;
	float k = kFactor;
//	int   R = 128;
	int   R = 32;



	biWidthInp=img->bi.biWidth;
	biHeightInp = img->bi.biHeight;

	convertBMPTo_mono_BMP_init(img, imgOut);
	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


	sizeW = img->bi.biWidth/grid;
	lastW = img->bi.biWidth - sizeW*grid;
	if(lastW > 0)
		sizeW++;
	else
		lastW = grid;

	sizeH = img->bi.biHeight/grid;
	lastH = img->bi.biHeight - sizeH*grid;
	if(lastH > 0)
		sizeH++;
	else
		lastH = grid;



	for(idxH=0;idxH<sizeH;idxH++){
		gridH = grid;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
//			k = setkFactor(kFactor, idxW, sizeW, idxH, sizeH);
			k = kFactor;
			gridW = grid;
			if(idxW == sizeW-1) gridW = lastW;
			m = *(lpImgMap_m+idxH*sizeW+ idxW);
			s = *(lpImgMap_s+idxH*sizeW+ idxW);
			tH = m*(1 + k*(s/R-1));
			convertRGB565To_mono_BMPLum_Next(img->pixPtr, biWidthInp, biHeightInp,
					imgOut->pixPtr, biWidthOut, biWidthPadOut, biHeightOut,
					idxW, idxH, grid, gridW, gridH, tH);
		}
	}

	if(1 || imgOut->bi.biWidth > 1400)
		convertDPI_in_BMP(imgOut, 300, 300);
	else{
		imgOut->bi.biXPelsPerMeter=300;
		imgOut->bi.biYPelsPerMeter=300;
		bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
	}
}

void setWhitePixels(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2){
//	int sww, i ,j, dy, sum, w_bt;
	int sww, i ,j, dy, w_bt;
	int xStart, xStop;
	LONG nY;
	BYTE  *hptr;
	//	  BYTE mask;

	w_bt=w;
	dy=y2-y1+1;
	i=0;
	xStart = x1/8;
	xStop =  x2/8;
	while(i<dy)  {
//		sum=0;
		sww=y1+i;
		sww=h-1-sww;
		nY=(LONG)sww*(LONG)w_bt;
		for(j = xStart;j<=xStop;j++) {
			hptr=ptr+nY+j;
			*hptr = 255;
		}
		i++;
	}

}
void setWhitePixelsX(LPBYTE ptr, int w, int h, int x1, int x2, int x12, int x22, int y1, int y2){
	int sww, i ,j, dy, w_bt;
//	int sww, i ,j, dy, sum, w_bt;
	int xStart, xStop;
	LONG nY;
	BYTE  *hptr;
	//   	  char buff[500];

	w_bt=w;
	dy=y2-y1+1;
	i=0;
	//   	              xStart = x1/8;
	//   	              xStop =  x2/8;
	while(i<dy)  {
//		sum=0;
		sww=y1+i;
		xStart = (i*(x12-x1))/(y2-y1) + x1;
		xStop =  (i*(x22-x2))/(y2-y1) + x2;
		xStart /= 8;
		xStop /= 8;
		//  	      sprintf(buff,"xStart = %d, xStop= %d\n",xStart, xStop);
		//  	      saveLog("", buff);

		sww=h-1-sww;
		nY=(LONG)sww*(LONG)w_bt;
		for(j = xStart;j<=xStop;j++) {
			hptr=ptr+nY+j;
			*hptr = 255;
		}
		i++;
	}

}
int testCleanLeftSide(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 30;
	int stop = hist[0]-3;
	k  = 0;
	for(i=1;i<stop;i++){
		if(hist[i] > av) k++;
		else break;
	}
	if(k < 10) return c;
	cc = 0;
	c = 0;
	for(i=k;i<stop;i++){
		if(hist[i] < av) {if(cc == 0) c = i; cc++;}
		else cc = 0;
		if(cc > tH) break;
	}
	if(cc > tH && c > k) return c;
	return 0;
}
int testCleanLeftSide2(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 10;
	int stop = hist[0]-3;
	int av1;
	//    	 char buff[500];

	av1 = 3*av;
	k  = 0;
	for(i=1;i<stop;i++){
		if(hist[i] < av1) k++;
		else break;
	}

	// 	     sprintf(buff,"k = %d, av1 = %d\n",k, av1);
	// 	     saveLog("", buff);

	if(k < 10) return c;
	cc = 0;
	c = 0;
	for(i=k;i<stop;i++){
		if(hist[i] > av1) {if(cc == 0) c = i; cc++;}
		else cc = 0;
		if(cc > tH) break;
	}
	// 	     sprintf(buff,"c = %d, cc = %d\n",c, cc);
	// 	     saveLog("", buff);

	if(cc > tH && c > k) return c;
	return 0;
}
int testRestOfHist_1(int *hist, int start, int av){
	int i, j, cc = 0;
	int cc1, cc2;
	int av1 = 2*av;
	int step = 6;
	for(i=start;i>step;i-=step/2){
		cc1 = 0, cc2 = 0;
//     sprintf(buffLog,"------------i = %d,  cc = %d \n",i,  cc);
//	     saveLog("", buffLog);

		for(j=0;j<step;j++){
  		   if(hist[i-j] >= av1) cc1++;
  		   if(hist[i-j] < av) cc2++;
// 	     sprintf(buffLog,"i-j = %d,  hist[i-j]= %d, cc1 = %d, cc2 = %d \n",i-j,  hist[i-j], cc1, cc2);
// 	     saveLog("", buffLog);

		}
		if(cc1>0 && cc2>0) cc++;
	}
//     sprintf(buffLog,"cc = %d,  av= %d, av1= %d\n",cc,  av, av1);
//	      saveLog("", buffLog);
	return cc;
}

int testCleanLeftSide_fromCenter(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 1;
	int av1;
	int start = hist[0]-50;
	int nMaxCut =20;
	int minCut[nMaxCut], lowCutTh, ccL;
	int iMincut;
	k  = start;
	av1 = 5*av;
	if(av1 < 300) av1 = 300;
//	printf(" av1= %d\n",av1);
	lowCutTh = av1/3;
	cc = 0;ccL = 0;
	iMincut = 0;

//printf(" start= %d\n",start);
	for(i=start;i>hist[0]/2;i--){
		if(hist[i] < lowCutTh) ccL++;
		else {
			if(ccL>0 && iMincut < nMaxCut) {minCut[iMincut] = i+ccL/2;iMincut++;}
			ccL = 0;
		}
		if(iMincut > nMaxCut) break;
	}
//printf(" iMincut= %d\n",iMincut);
/*
	for(i=0;i<iMincut;i++){
		printf(" minCut[ %d ]= %d\n",i, minCut[i]);
	}
	for(i=0;i<iMaxcut;i++){
		printf(" maxCut[ %d ]= %d\n",i, maxCut[i]);
	}
*/
	if(iMincut > 1)	return 0;


	cc = 0;
	for(i=start;i>0;i--){
		if(hist[i] > av1) {if(cc == 0) k = i; cc++;}
		else cc = 0;
		if(cc >= tH) {
		   if((hist[0]-k)>hist[0]/3)
	             break;
//     sprintf(buffLog," k = %d\n", k);
//     saveLog("", buffLog);

		   if(testRestOfHist_1(hist, k, av)<6)
		               break;
           i -= 4;
		}

	}
//	 	     sprintf(buffLog,"cc = %d, k = %d, av1 = %d\n",cc, k, av1);
//	 	     saveLog("", buffLog);

	if(cc < tH || k >= start) return 0;
	c = k;
	for(i=k;i<k+20;i++){
		if(hist[i] < av) {c = i; break;}
	}
//     sprintf(buffLog,"c = %d, av = %d\n",c, av);
//     saveLog("", buffLog);
	i = minCut[0]; // to avoid compiler warning
	return c;
}

int adjustCleanEdge(int av, int *hist, int x){
	int i, k, min;
	int start = x-15, stop = x+15;
	k = x;
	min = hist[x];
	if(start < 1) start = 1;
	if(stop > hist[0]) stop = hist[0];
	for(i=start;i<stop;i++){
		if(hist[i] < min) {min = hist[i]; k = i;}
	}
	return k;
}
int adjustCleanEdge_1(int av, int *hist, int x){
	int i, k, min;
	int start = x-15, stop = x;
	k = x;
	min = hist[x];
	if(start < 1) start = 1;
	if(stop > hist[0]) stop = hist[0];
	for(i=start;i<stop;i++){
		if(hist[i] < min) {min = hist[i]; k = i;}
	}
	return k;
}
int adjustCleanEdge_2(int av, int *hist, int x){
	int i, k, min;
	int start = x, stop = x+15;
	k = x;
	min = hist[x];
	if(start < 1) start = 1;
	if(stop > hist[0]) stop = hist[0];
	for(i=start;i<stop;i++){
		if(hist[i] < min) {min = hist[i]; k = i;}
	}
	return k;
}

int testRestOfHist(int *hist, int start, int av){
	int i, j, cc = 0;
	int cc1, cc2;
	int av1 = 2*av;
	int step = 6;
	int step1 = 1;
	for(i=start;i<hist[0]-step;i+=step1){
		cc1 = 0, cc2 = 0;
		for(j=0;j<step;j++){
  		   if(hist[i+j] >= av1) cc1++;
  		 if(hist[i+j] < av) cc2++;
		}
		if(cc1>0 && cc2>0) {cc++;step1 = step/2;}
		else step1 = 1;
	}
//     sprintf(buffLog,"cc = %d,  av= %d, av1= %d\n",cc,  av, av1);
//	      saveLog("", buffLog);
	return cc;
}
int testCleanRightSide_fromCenter(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 1;
	int av1;
	int start = 50, stop;
	int nMaxCut =20;
	int minCut[nMaxCut], lowCutTh, ccL;
	int  iMincut;


	k  = start;
	av1 = 5*av;
	if(av1 < 300) av1 = 300;

	lowCutTh = av1/3;
//	printf("av1= %d, lowCutTh= %d, hist[0]/2= %d\n",av1, lowCutTh, hist[0]/2);
	cc = 0;ccL = 0;
	iMincut = 0;

	for(i=start;i<hist[0]/2;i++){
//		if(hist[i] > av1) cc++;
//		else {
//			if(cc>0 && iMaxcut < nMaxCut) {maxCut[iMaxcut] = i-cc/2;iMaxcut++; }
//			cc = 0;
//		}
		if(hist[i] < lowCutTh) ccL++;
		else {
			if(ccL>5 && iMincut < nMaxCut) {minCut[iMincut] = i-ccL/2;iMincut++;}
			ccL = 0;
		}
		if(iMincut > nMaxCut) break;
	}
	if(ccL>5 && iMincut < nMaxCut) {minCut[iMincut] = i-ccL/2;iMincut++;}

//printf("R iMincut= %d\n",iMincut);
	if(iMincut > 1)	return 0;


	cc = 0;
	for(i=start;i<hist[0]-3;i++){
		if(hist[i] > av1) {if(cc == 0) k = i; cc++;}
		else cc = 0;
		if(cc >= tH) {
			if(k>hist[0]/3)
		          break;
			if(testRestOfHist(hist, k, av)<6)
			               break;
           i += 4;
		}
	}

//	 	      sprintf(buffLog,"cc = %d, k= %d, av1= %d\n",cc, k, av1);
//	   	      saveLog("", buffLog);

	if(cc < tH || k <= start) return 0;
	c = k;
	stop = k-20;
	if(stop < 1) stop = 1;
	for(i=k;i>stop;i--){
		if(hist[i] < av) {c = i; break;}
	}
//     sprintf(buffLog,"c = %d, stop= %d, av= %d\n",c, stop, av);
//	      saveLog("", buffLog);
	i = minCut[0]; // to avoid compiler warning
	return c;
}
int testCleanTop(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 3;
	int av1;
	int start = 1, stop, min;

	k  = start;
	av1 = 6*av;
	if(av1 < 200) av1 = 200;
	cc = 0;
	stop = hist[0] - 10;
	for(i=start;i<stop;i++){
		if(hist[i] > av1) {if(cc == 0) k = i; cc++;}
		else cc = 0;
		if(cc >= tH) break;
	}

//	 	      sprintf(buffLog,"cc = %d, k= %d, av1= %d\n",cc, k, av1);
//	   	      saveLog("", buffLog);

	if(cc < tH || k <= start) return 0;
	stop = k+20;
	if(stop > hist[0]-3) stop = hist[0]-3;
	min = hist[k];
	c = k;
	for(i=k;i<stop;i++){
		if(hist[i] < min) {min=hist[i]; c = i;}
	}

	return c;
}

int testCleanBottom(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 1;
	int av1;
	int start = hist[0], stop, min;
	//    	 char buff[500];

	k  = start;
	av1 = 5*av;
	cc = 0;
	stop = hist[0]/2;
	for(i=start;i>stop;i--){
		if(hist[i] > av1) {if(cc == 0) k = i; cc++;}
		else cc = 0;
		if(cc >= tH) break;
	}

	// 	      sprintf(buff,"cc = %d, k= %d, av1= %d\n",cc, k, av1);
	//   	      saveLog("", buff);

	if(cc < tH || k >= start) return 0;
	stop = k-20;
	if(stop < 1) stop = 1;
	min = hist[k];
	c = k;
	for(i=k;i>stop;i--){
		if(hist[i] < min) {min=hist[i]; c = i;}
	}

	return c;
}

int testCleanRightSide(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 30;
	int start = hist[0]-3;

	k  = 0;
	for(i=start;i>0;i--){
		if(hist[i] > av) k++;
		else break;
	}
	if(k < 10) return c;
	cc = 0;
	c = 0;
	k = hist[0] - k;
	for(i=k;i>0;i--){
		if(hist[i] < av) {if(cc == 0) c = i; cc++;}
		else cc = 0;
		if(cc > tH) break;
	}
	if(cc > tH && c < k) return c;
	return 0;
}
int testCleanRightSide2(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 15;
//	int av1;
	int start = hist[0]-3;

	k  = 0;
//	av1 = av+av/2;
	for(i=start;i>0;i--){
		if(hist[i] < av) k++;
		else break;
	}
	if(k < 10) return c;
	cc = 0;
	c = 0;
	k = hist[0] - k;
//	av1 = 2*av;
	for(i=k;i>0;i--){
		if(hist[i] > av) {if(cc == 0) c = i; cc++;}
		else cc = 0;
		if(cc > tH) break;
	}
	if(cc > tH && c < k) return c;
	return 0;
}
void lineTestAfterCleaning(int *hHist, int *c1, int *c2){
	int i, k1, k2;
	int av, cc;
	*c1 = 0;*c2 = 0;
    k1 = 0;
    for(i=1;i<=hHist[0];i++) {
    	if(hHist[i] > 2) {k1 = i;break;}
    }
    if(k1 == 0) return;
    k2 = 0;
    for(i=k1;i<=hHist[0];i++) {
    	if(hHist[i] < 3) {k2 = i;break;}
    }
    if(k2 == 0) return;
    av = 0; cc = 0;
    for(i=k2;i<=hHist[0];i++) {
    	if(hHist[i] > 2) {av += hHist[i];cc++;}
    }
    if(cc > 0) av /= cc;
    for(i=k1;i<k2;i++) {
    	if(hHist[i] > av*8){
            *c1 = k1; *c2 = k2;
    		break;
    	}
    }
}
int TestForTxtLines(int *hHist, int US_state){
	int ret = 0;
	int i, cc = 0;
	int th1, th2;

	th1 = 3;
	th2 = 3;
	if( US_state == 0){
		th1 = 1;
		th2 = 5;
	}
    for(i=1;i<hHist[0];i++) {
    	if(hHist[i] < th1){
            cc++;
            if(cc > th2) {break;}
    	}else
    		cc = 0;
    }
	if(cc > th2) ret = 1;
	return ret;
}


int biSection(LPIMGDATA img, int biWidthPad, int maxX, int maxXLeft, int xC, int y1, int y2, int US_state){
	int x1, x2, ok, x_cand = 0;
   if(maxX > maxXLeft+16){
	  int xL, xR;
	  xL = maxXLeft; xR= maxX;
	  x2 = xC;
	  while(xR-xL > 4){
		  x1 = (xL+xR)/2;
	      histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	      ok = TestForTxtLines(hHistPreProc, US_state);
	      if(ok == 1) {xR = x1;}
	      else{xL=x1;}
	  }

     x_cand = (xL+xR)/2;
  }
   return x_cand;
}
int biSection_r(LPIMGDATA img, int biWidthPad, int maxX, int maxXRight, int xC, int y1, int y2, int US_state){
	int x1, x2, ok, x_cand = 0;
   if(maxX < maxXRight-16){
	  int xL, xR;
	  xL = maxX; xR= maxXRight;
	  x1 = xC;
//	  x2 = xC;
	  while(xR-xL > 4){
		  x2 = (xL+xR)/2;
	      histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	      ok = TestForTxtLines(hHistPreProc, US_state);
	      if(ok == 1) {xL = x2;}
	      else{xR=x2;}
	  }

     x_cand = (xL+xR)/2;
  }
   return x_cand;
}
void cleanTicket(LPIMGDATA img, int US_state){

	int i;
	int biWidthPad;
	int x1, x2, y1, y2;
	//    	 char buff[500];
	long av = 0, av1;
	int hAv;
	int clean1;// , clean2;
//	int rest;
	int xC, yC;
	int  yC_cut;
	int  yC8;


	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
//	rest = biWidthPad*8-img->bi.biWidth*img->bi.biBitCount;
	xC = (img->bi.biWidth*img->bi.biBitCount)/2;
	yC = img->bi.biHeight/2;
	yC8 = yC/4;
	yC_cut = yC8*2;

// new left side
//-----------------------------------------------------------------------------------------
// 1
	{
	int k;
    int ok = 0;
    int cleanTab[3], cleanTabLeft[3];
    int yTTab[3], yBTab[3];
    int maxX, maxXLeft, kBest;
    int x_cand1, x_cand2, x_cand3;
    int thLogo = 0, cc, cctH;

      y1 = yC_cut-yC8/3;
      y2 = yC_cut+yC8/3;

//printf(" yC_cut= %d, yC8= %d, y1= %d, y2= %d, y2-y1= %d\n", yC_cut, yC8, y1, y2, y2-y1);
//fflush(stdout);

      k = 0;
      while(k<3){
	   i = 0;
	   x1 = 0;
	   x2 = xC;

//     printf(" k=%d, start\n", k);
//     fflush(stdout);

	   cleanTab[k] = 0;
	   cleanTabLeft[k] = 0;
	   yTTab[k] = y1;
	   yBTab[k] = y2;
	   while(i< 4 && x1<x2-20) {
// printf(" x1= %d, x2= %d, y1= %d, y2= %d, y2-y1= %d\n", x1, x2, y1, y2, y2-y1);
//  printf(" k=%d, i= %d, x1= %d, x2= %d, xC= %d\n", k, i, x1, x2, xC);
//    fflush(stdout);
	      histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);

//	      printf(" k=%d, i= %d, hHistPreProc[0]= %d\n", k, i, hHistPreProc[0]);
//	        fflush(stdout);
//		   sprintf(buffLog,"---------------------- x1= %d, x2= %d, y1= %d, y2= %d, hHistPreProc[0]= %d\n", x1, x2, y1, y2, hHistPreProc[0]);
//		   saveLog("", buffLog);
//		   saveLog("", "\n");
//	          {int n;
//		      	     for(n=1;n<=hHistPreProc[0];n++) {
//		   		   	  	      sprintf(buffLog,"hist[%d]= %d\n",n, hHistPreProc[n]);
//		   	   	   	      saveLog("", buffLog);
//		   	   	       }
//	          }
	      ok = TestForTxtLines(hHistPreProc, US_state);
          if(ok == 1){cleanTab[k] = x1;break;}
          cleanTabLeft[k] = x1;
	      x1 += (xC/4);
	      i++;
	   }
       y1 += yC8;
       y2 += yC8;
       k++;
      }
      x_cand1 = 0; x_cand2 = 0; x_cand3 = 0;
      thLogo = (xC/4)*3 - 20;
      cctH = 0;
      if(cleanTab[0] >= thLogo) cctH++;
      if(cleanTab[1] >= thLogo) cctH++;
      if(cleanTab[2] >= thLogo) cctH++;
      if(cleanTab[0] > 0 && cleanTab[0] < thLogo){
    	  kBest = 0;
          maxX = cleanTab[kBest];maxXLeft = cleanTabLeft[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
          x_cand1 = biSection(img, biWidthPad, maxX, maxXLeft, xC, y1, y2, US_state);
      }
      if(cleanTab[1] > 0 && (cleanTab[1] < thLogo || cctH == 3)){
    	  kBest = 1;
          maxX = cleanTab[kBest];maxXLeft = cleanTabLeft[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
          x_cand2 = biSection(img, biWidthPad, maxX, maxXLeft, xC, y1, y2, US_state);
      }
      if(cleanTab[2] > 0 && (cleanTab[2] < thLogo || cctH == 3)){
    	  kBest = 2;
          maxX = cleanTab[kBest];maxXLeft = cleanTabLeft[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
          x_cand3 = biSection(img, biWidthPad, maxX, maxXLeft, xC, y1, y2, US_state);
      }

//    printf(" cleanTab[0]= %d, cleanTab[1]= %d, cleanTab[2]= %d, thLogo= %d, cctH= %d\n", cleanTab[0], cleanTab[1], cleanTab[2], thLogo, cctH);
//    fflush(stdout);


//  printf(" x_cand1= %d, x_cand2= %d, x_cand3= %d\n", x_cand1, x_cand2, x_cand3);
     cc = 0;
     if(x_cand1 == 0) cc++;
     if(x_cand2 == 0) cc++;
     if(x_cand3 == 0) cc++;
     kBest = 0;
     if(x_cand2 > x_cand1) {x_cand1 = x_cand2;kBest = 1;}
     if(US_state != 2)
        if(x_cand3 > x_cand1) {x_cand1 = x_cand3;kBest = 2;}

//     printf(" x_cand1= %d, x_cand2= %d, x_cand3= %d, cc= %d\n", x_cand1, x_cand2, x_cand3, cc);
     if(cc > 1 && cleanTab[kBest] > thLogo)  x_cand1 = 0;
//     printf(" x_cand1= %d, x_cand2= %d, x_cand3= %d, cc= %d\n", x_cand1, x_cand2, x_cand3, cc);
//     printf(" x_cand1= %d\n", x_cand1);
//     x_cand1 = 302;
     if(x_cand1 > 0){
    	 int r;
    	 r = x_cand1 - (x_cand1/8)*8;
//   	    printf(" r= %d\n", r);
   	    if(r > 4) x_cand1 += 8;
        setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, x_cand1, 0, x_cand1, 0, img->bi.biHeight-1);
     }



	}

	// new right side
	//-----------------------------------------------------------------------------------------
		{
		int k;
	    int ok = 0;
	    int cleanTab[3], cleanTabRight[3];
	    int yTTab[3], yBTab[3];
	    int maxX, maxXRight, kBest;
	    int x_cand1, x_cand2, x_cand3;
	    int thLogo = 0;

	      y1 = yC_cut-yC8/3;
	      y2 = yC_cut+yC8/3;

	//printf(" yC_cut= %d, yC8= %d, y1= %d, y2= %d, y2-y1= %d\n", yC_cut, yC8, y1, y2, y2-y1);
	//fflush(stdout);

	      k = 0;
	      while(k<3){
		   i = 0;
		   x1 = xC;
		   x2 = img->bi.biWidth-1;

	//     printf(" k=%d, start\n", k);
	//     fflush(stdout);

		   cleanTab[k] = img->bi.biWidth-1;
		   cleanTabRight[k] = img->bi.biWidth-1;
		   yTTab[k] = y1;
		   yBTab[k] = y2;
		   while(i< 4 && x1<x2-20) {

//  printf(" k=%d, i= %d, x1= %d, x2= %d, xC= %d, img->bi.biWidth-1= %d\n", k, i, x1, x2, xC, img->bi.biWidth-1);
		      histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);

/*
		      		   sprintf(buffLog,"---------------------- x1= %d, x2= %d, y1= %d, y2= %d, hHistPreProc[0]= %d\n", x1, x2, y1, y2, hHistPreProc[0]);
		      		   saveLog("", buffLog);
		      		   saveLog("", "\n");
		      	          {int n;
		      		      	     for(n=1;n<=hHistPreProc[0];n++) {
		      		   		   	  	      sprintf(buffLog,"hist[%d]= %d\n",n, hHistPreProc[n]);
		      		   	   	   	      saveLog("", buffLog);
		      		   	   	       }
		      	          }
*/

	          ok = TestForTxtLines(hHistPreProc, US_state);
	          if(ok == 1){cleanTab[k] = x2;break;}
	          cleanTabRight[k] = x2;
		      x2 -= (xC/4);
		      i++;
		   }
	       y1 += yC8;
	       y2 += yC8;
	       k++;
	      }

	      x_cand1 = img->bi.biWidth-1; x_cand2 = img->bi.biWidth-1; x_cand3 = img->bi.biWidth-1;
	      thLogo = img->bi.biWidth-1-(xC/4)*3 + 20;
	      if(cleanTab[0] < img->bi.biWidth-1 && cleanTab[0] > thLogo){
	    	  kBest = 0;
	          maxX = cleanTab[kBest];maxXRight = cleanTabRight[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
	          x_cand1 = biSection_r(img, biWidthPad, maxX, maxXRight, xC, y1, y2, US_state);
	      }
	      if(cleanTab[1] < img->bi.biWidth-1){
	    	  kBest = 1;
	          maxX = cleanTab[kBest];maxXRight = cleanTabRight[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
	          x_cand2 = biSection_r(img, biWidthPad, maxX, maxXRight, xC, y1, y2, US_state);
	      }
	      if(cleanTab[2] < img->bi.biWidth-1){
	    	  kBest = 2;
	          maxX = cleanTab[kBest];maxXRight = cleanTabRight[kBest];y1 = yTTab[kBest];y2 = yBTab[kBest];
	          x_cand3 = biSection_r(img, biWidthPad, maxX, maxXRight, xC, y1, y2, US_state);
	      }

//	    printf(" cleanTab[0]= %d, cleanTab[1]= %d, cleanTab[2]= %d, thLogo= %d\n", cleanTab[0], cleanTab[1], cleanTab[2], thLogo);
//	    fflush(stdout);


//	  printf(" x_cand1= %d, x_cand2= %d, x_cand3= %d\n", x_cand1, x_cand2, x_cand3);
//	  fflush(stdout);
	     if(x_cand2 < x_cand1) x_cand1 = x_cand2;
         if(US_state != 2)
	        if(x_cand3 < x_cand1) x_cand1 = x_cand3;
//	     printf(" x_cand1= %d\n", x_cand1);
	//     x_cand1 = 302;
	     if(x_cand1 < img->bi.biWidth-1){
	    	 int r;
	    	 r = x_cand1 - (x_cand1/8)*8;
//	   	    printf(" r= %d\n", r);
	   	    if(r < 4) x_cand1 -= 8;
	        setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, x_cand1, img->bi.biWidth-1, x_cand1, img->bi.biWidth-1, 0, img->bi.biHeight-1);
	     }



		}

//-----------------------------------------------------------------------------------------

/*
	// left side
	x1 = 8;
	x2 = xC;
	y1 = yC-yC/2;
	y2 = yC+yC/2;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];

	clean1 = testCleanLeftSide_fromCenter(av, hHistPreProc);

//  printf(" clean1= %d\n", clean1);

//	   	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	   	      saveLog("", buffLog);
//	   	       }
	x1 = 8;
	x2 = xC;
	y2 = yC-yC/2;
	y1 = y2-yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	   	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	     	   	      saveLog("", buffLog);
//	   	     	       }
	clean2 = clean1;
	if(clean2 != 0)
		clean1 = adjustCleanEdge_1(av, hHistPreProc, clean1);
//	   printf(" clean1= %d, adjusted clean1=%d \n", clean2, clean1);

	y1 = yC+yC/2;
	y2 = y1+yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//	 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	 	     	   	      saveLog("", buffLog);
//	 	     	       }
	if(clean2 != 0)
		clean2 = adjustCleanEdge_2(av, hHistPreProc, clean2);
//	 printf(" adjclean2= %d \n", clean2);

	if(clean1 != 0 ||clean2 != 0) {
		setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, clean1, 0, clean2, 0, img->bi.biHeight-1);
//    printf("1----- clean1= %d, clean2=%d\n", clean1, clean2);
    x1 = clean1-20;if(x1 < 0) x1 = 0;
    x2 = clean2+100;if(x2 > xC) x2 = xC;
    y1 = yC-yC/2;
    y2 = yC+yC/2;
    histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
    lineTestAfterCleaning(hHistPreProc, &clean1, &clean2);
//  printf(" clean1= %d, clean2=%d\n", clean1, clean2);
  //printf("1.5----- clean1= %d, clean2=%d\n", clean1, clean2);
    setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, x1+clean1, 0, x1+clean2, 0, img->bi.biHeight-1);
   }
*/
/*
	// right side
	x1 = xC;
	x2 = (img->bi.biWidth*img->bi.biBitCount)-rest;
	y1 = yC-yC/2;
	y2 = yC+yC/2;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);

	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];

	clean1 = testCleanRightSide_fromCenter(av, hHistPreProc);

//	 printf("r;  clean1= %d\n", clean1);
	y2 = yC-yC/2;
	y1 = y2-yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
	clean2 = clean1;
	if(clean2 != 0)
		clean1 = adjustCleanEdge(av, hHistPreProc, clean1);

//	  	   printf(" clean1= %d, adjusted clean1=%d \n", clean2, clean1);

	y1 = yC+yC/2;
	y2 = y1+yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
	if(clean2 != 0)
		clean2 = adjustCleanEdge(av, hHistPreProc, clean2);
// 	   printf(" adjclean2= %d \n", clean2);

	if(clean1 != 0 ||clean2 != 0) {
//	printf("2----- clean1= %d, clean2=%d\n", clean1, clean2);
		clean1 += x1;
		clean2 += x1;
		setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, clean1, img->bi.biWidth-1, clean2, img->bi.biWidth-1, 0, img->bi.biHeight-1);
	}

*/
	// top
	x1 = 0;
	x2 = xC;
	y1 = 1;
	y2 = yC/2;
	histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;av1 = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
		if(i == 100) av1 = av;
	}
	av /= hHistPreProc[0];
	av1 /= 100;
	clean1 = 0;
    if(av1 > 5)
	   clean1 = testCleanTop(av, hHistPreProc);

//    sprintf(buffLog,"av= %ld, av1= %ld, clean1= %d\n",av, av1,clean1);
//    saveLog("", buffLog);

//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		 	     	   	      saveLog("", buffLog);
//	 	     	       }
//   	    sprintf(buffLog,"---------------------------------------- \n");
//     	    saveLog("", buffLog);
//       	    sprintf(buffLog,"---------------------------------------- \n");
//         	    saveLog("", buffLog);

	if(clean1 != 0)
		setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, 0, clean1+20);


	/*
// bottom
 	   	 	     		 x1 = xC-xC/2;
 	   	 	     		 x2 = xC+xC/2;
 	   	 	     		 y1 = yC;
 	   	 	     		 y2 = img->bi.biHeight-1;
 	   	 	      	     histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
 	   	 	  	     for(i=3;i<=hHistPreProc[0];i++) {
 	   	 	  	    	 hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
 	   	 	  	    	 hHistPreProc[i-2] = hAv;
 	   	 	  	     }
 	   	 	  	     hHistPreProc[0] -= 2;
 	   	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	 	                av += hHistPreProc[i];
 	   	 	     	     }
 	   	 	     	     av /= hHistPreProc[0];

 	   	 	           clean1 = testCleanBottom(av, hHistPreProc);

 	   	// 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	// 	     	  	      sprintf(buff,"%ld,%d\n",av, hHistPreProc[i]);
 	   	// 	     	   	      saveLog("", buff);
 	   	// 	     	       }
 	   	 	   	   if(clean1 != 0) {
 	   	 	   		    clean1 += x1;
 	   	 	   	        setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, clean1, img->bi.biHeight-1);
 	   	 	   	   }
	 */
	// 	   	          setWhitePixelsY(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth, 0, clean1, 0, clean2);


}

void cleanTicketOLD(LPIMGDATA img){

	int i;
	int biWidthPad;
	int x1, x2, y1, y2;
	//    	 char buff[500];
	long av = 0, av1;
	int hAv;
	int clean1, clean2;
	int rest;
	int xC, yC;

	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	rest = biWidthPad*8-img->bi.biWidth*img->bi.biBitCount;
	xC = (img->bi.biWidth*img->bi.biBitCount)/2;
	yC = img->bi.biHeight/2;

	// left side
	x1 = 8;
	x2 = xC;
	y1 = yC-yC/2;
	y2 = yC+yC/2;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];

	clean1 = testCleanLeftSide_fromCenter(av, hHistPreProc);

//  printf(" clean1= %d\n", clean1);

//	   	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	   	      saveLog("", buffLog);
//	   	       }
	x1 = 8;
	x2 = xC;
	y2 = yC-yC/2;
	y1 = y2-yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	   	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	     	   	      saveLog("", buffLog);
//	   	     	       }
	clean2 = clean1;
	if(clean2 != 0)
		clean1 = adjustCleanEdge_1(av, hHistPreProc, clean1);
//	   printf(" clean1= %d, adjusted clean1=%d \n", clean2, clean1);

	y1 = yC+yC/2;
	y2 = y1+yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//	 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	 	     	   	      saveLog("", buffLog);
//	 	     	       }
	if(clean2 != 0)
		clean2 = adjustCleanEdge_2(av, hHistPreProc, clean2);
//	 printf(" adjclean2= %d \n", clean2);

	if(clean1 != 0 ||clean2 != 0) {
		setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, clean1, 0, clean2, 0, img->bi.biHeight-1);
//    printf("1----- clean1= %d, clean2=%d\n", clean1, clean2);
    x1 = clean1-20;if(x1 < 0) x1 = 0;
    x2 = clean2+100;if(x2 > xC) x2 = xC;
    y1 = yC-yC/2;
    y2 = yC+yC/2;
    histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
    lineTestAfterCleaning(hHistPreProc, &clean1, &clean2);
//  printf(" clean1= %d, clean2=%d\n", clean1, clean2);
  //printf("1.5----- clean1= %d, clean2=%d\n", clean1, clean2);
    setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, x1+clean1, 0, x1+clean2, 0, img->bi.biHeight-1);
   }

	// right side
	x1 = xC;
	x2 = (img->bi.biWidth*img->bi.biBitCount)-rest;
	y1 = yC-yC/2;
	y2 = yC+yC/2;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);

	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];

	clean1 = testCleanRightSide_fromCenter(av, hHistPreProc);

//	 printf("r;  clean1= %d\n", clean1);
/*
		   	     for(i=1;i<=hHistPreProc[0];i++) {
			   	  	      sprintf(buffLog,"hist[%d] = %d\n",i, hHistPreProc[i]);
		   	   	      saveLog("", buffLog);
		   	       }
*/
	y2 = yC-yC/2;
	y1 = y2-yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
	clean2 = clean1;
	if(clean2 != 0)
		clean1 = adjustCleanEdge(av, hHistPreProc, clean1);

//	  	   printf(" clean1= %d, adjusted clean1=%d \n", clean2, clean1);

	y1 = yC+yC/2;
	y2 = y1+yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
	if(clean2 != 0)
		clean2 = adjustCleanEdge(av, hHistPreProc, clean2);
// 	   printf(" adjclean2= %d \n", clean2);

	if(clean1 != 0 ||clean2 != 0) {
//	printf("2----- clean1= %d, clean2=%d\n", clean1, clean2);
		clean1 += x1;
		clean2 += x1;
		setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, clean1, img->bi.biWidth-1, clean2, img->bi.biWidth-1, 0, img->bi.biHeight-1);
	}


	// top
	x1 = 0;
	x2 = xC;
	y1 = 1;
	y2 = yC/2;
	histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;av1 = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
		if(i == 100) av1 = av;
	}
	av /= hHistPreProc[0];
	av1 /= 100;
	clean1 = 0;
    if(av1 > 5)
	   clean1 = testCleanTop(av, hHistPreProc);

//    sprintf(buffLog,"av= %ld, av1= %ld, clean1= %d\n",av, av1,clean1);
//    saveLog("", buffLog);

//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		 	     	   	      saveLog("", buffLog);
//	 	     	       }
//   	    sprintf(buffLog,"---------------------------------------- \n");
//     	    saveLog("", buffLog);
//       	    sprintf(buffLog,"---------------------------------------- \n");
//         	    saveLog("", buffLog);

	if(clean1 != 0)
		setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, 0, clean1+20);


	/*
// bottom
 	   	 	     		 x1 = xC-xC/2;
 	   	 	     		 x2 = xC+xC/2;
 	   	 	     		 y1 = yC;
 	   	 	     		 y2 = img->bi.biHeight-1;
 	   	 	      	     histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
 	   	 	  	     for(i=3;i<=hHistPreProc[0];i++) {
 	   	 	  	    	 hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
 	   	 	  	    	 hHistPreProc[i-2] = hAv;
 	   	 	  	     }
 	   	 	  	     hHistPreProc[0] -= 2;
 	   	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	 	                av += hHistPreProc[i];
 	   	 	     	     }
 	   	 	     	     av /= hHistPreProc[0];

 	   	 	           clean1 = testCleanBottom(av, hHistPreProc);

 	   	// 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	// 	     	  	      sprintf(buff,"%ld,%d\n",av, hHistPreProc[i]);
 	   	// 	     	   	      saveLog("", buff);
 	   	// 	     	       }
 	   	 	   	   if(clean1 != 0) {
 	   	 	   		    clean1 += x1;
 	   	 	   	        setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, clean1, img->bi.biHeight-1);
 	   	 	   	   }
	 */
	// 	   	          setWhitePixelsY(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth, 0, clean1, 0, clean2);


}
void cleanTicketNY(LPIMGDATA img){

	int i;
	int biWidthPad;
	int x1, x2, y1, y2;
	//    	 char buff[500];
	long av = 0, av1;
	int hAv;
	int clean1;
//	int rest;
	int xC, yC;

	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
//	rest = biWidthPad*8-img->bi.biWidth*img->bi.biBitCount;
	xC = (img->bi.biWidth*img->bi.biBitCount)/2;
	yC = img->bi.biHeight/2;


	// left side

	x1 = 0;
	x2 = img->bi.biWidth-1;
	y1 = 1;
	y2 = yC;
	histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);

//		   	     for(i=1;i<=hHistPreProc[0];i++) {
//			   	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		   	   	      saveLog("", buffLog);
//		   	       }


	x1 = 8;
	x2 = xC;
	y1 = yC-yC/2;
	y2 = yC+yC/2;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);

    setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 300, 364, 323, 364, 0, img->bi.biHeight-1);
//		   	     for(i=1;i<=hHistPreProc[0];i++) {
//			   	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		   	   	      saveLog("", buffLog);
//		   	       }

/*
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];

	clean1 = testCleanLeftSide_fromCenter(av, hHistPreProc);

//  printf(" clean1= %d\n", clean1);

//	   	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	   	      saveLog("", buffLog);
//	   	       }
	x1 = 8;
	x2 = xC;
	y2 = yC-yC/2;
	y1 = y2-yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	   	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		   	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	   	     	   	      saveLog("", buffLog);
//	   	     	       }
	clean2 = clean1;
	if(clean2 != 0)
		clean1 = adjustCleanEdge_1(av, hHistPreProc, clean1);
//	   printf(" clean1= %d, adjusted clean1=%d \n", clean2, clean1);

	y1 = yC+yC/2;
	y2 = y1+yC/4;
	histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
	}
	av /= hHistPreProc[0];
//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//	 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//	 	     	   	      saveLog("", buffLog);
//	 	     	       }
	if(clean2 != 0)
		clean2 = adjustCleanEdge_2(av, hHistPreProc, clean2);
//	 printf(" adjclean2= %d \n", clean2);

	if(clean1 != 0 ||clean2 != 0) {
		setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, clean1, 0, clean2, 0, img->bi.biHeight-1);
//    printf("1----- clean1= %d, clean2=%d\n", clean1, clean2);
    x1 = clean1-20;if(x1 < 0) x1 = 0;
    x2 = clean2+100;if(x2 > xC) x2 = xC;
    y1 = yC-yC/2;
    y2 = yC+yC/2;
    histVPtrPix(img->pixPtr, biWidthPad, img->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
    lineTestAfterCleaning(hHistPreProc, &clean1, &clean2);
//  printf(" clean1= %d, clean2=%d\n", clean1, clean2);
//  printf("1.5----- clean1= %d, clean2=%d\n", clean1, clean2);
    setWhitePixelsX(img->pixPtr, biWidthPad, img->bi.biHeight, 0, x1+clean1, 0, x1+clean2, 0, img->bi.biHeight-1);
   }
*/
	// top
	x1 = 0;
	x2 = xC;
	y1 = 1;
	y2 = yC/2;
	histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;av1 = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
		if(i == 100) av1 = av;
	}
	av /= hHistPreProc[0];
	av1 /= 100;
	clean1 = 0;
    if(av1 > 5)
	   clean1 = testCleanTop(av, hHistPreProc);

//    sprintf(buffLog,"av= %ld, av1= %ld, clean1= %d\n",av, av1,clean1);
//    saveLog("", buffLog);

//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		 	     	   	      saveLog("", buffLog);
//	 	     	       }
//   	    sprintf(buffLog,"---------------------------------------- \n");
//     	    saveLog("", buffLog);
//       	    sprintf(buffLog,"---------------------------------------- \n");
//         	    saveLog("", buffLog);

	if(clean1 != 0)
		setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, 0, clean1+20);


	/*
// bottom
 	   	 	     		 x1 = xC-xC/2;
 	   	 	     		 x2 = xC+xC/2;
 	   	 	     		 y1 = yC;
 	   	 	     		 y2 = img->bi.biHeight-1;
 	   	 	      	     histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
 	   	 	  	     for(i=3;i<=hHistPreProc[0];i++) {
 	   	 	  	    	 hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
 	   	 	  	    	 hHistPreProc[i-2] = hAv;
 	   	 	  	     }
 	   	 	  	     hHistPreProc[0] -= 2;
 	   	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	 	                av += hHistPreProc[i];
 	   	 	     	     }
 	   	 	     	     av /= hHistPreProc[0];

 	   	 	           clean1 = testCleanBottom(av, hHistPreProc);

 	   	// 	     	     for(i=1;i<=hHistPreProc[0];i++) {
 	   	// 	     	  	      sprintf(buff,"%ld,%d\n",av, hHistPreProc[i]);
 	   	// 	     	   	      saveLog("", buff);
 	   	// 	     	       }
 	   	 	   	   if(clean1 != 0) {
 	   	 	   		    clean1 += x1;
 	   	 	   	        setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, clean1, img->bi.biHeight-1);
 	   	 	   	   }
	 */
	// 	   	          setWhitePixelsY(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth, 0, clean1, 0, clean2);


}

int testCleanTopCA(int av, int *hist){
	int c = 0;
	int i, k, cc;
	int tH = 3;
	int av1;
	int start = 1, stop, min;

	k  = start;
	av1 = 6*av;
	if(av1 < 250) av1 = 250;
	cc = 0;
	stop = hist[0] - 10;
	if(stop>350) stop = 350;
	for(i=start;i<stop;i++){
		if(hist[i] > av1) {if(cc == 0) k = i; cc++;}
		else cc = 0;
		if(cc >= tH) break;
	}

//	 	      printf("cc = %d, k= %d, av1= %d\n",cc, k, av1);

	if(cc < tH || k <= start) return 0;
	stop = k+25;
	if(stop > hist[0]-3) stop = hist[0]-3;
	min = hist[k];
	c = k;
	for(i=k;i<stop;i++){
		if(hist[i] < min) {min=hist[i]; c = i;}
	}

	return c;
}

void cleanTicketCA(LPIMGDATA img){

	int i;
	int biWidthPad;
	int x1, x2, y1, y2;
	//    	 char buff[500];
	long av = 0, av1;
	int hAv;
	int clean1;
//	int rest;
	int xC, yC;

	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
//	rest = biWidthPad*8-img->bi.biWidth*img->bi.biBitCount;
	xC = (img->bi.biWidth*img->bi.biBitCount)/2;
	yC = img->bi.biHeight/2;


	// top
	x1 = 0;
	x2 = xC;
	y1 = 1;
	y2 = yC/2;
	histHTest_X1_X1(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	for(i=3;i<=hHistPreProc[0];i++) {
		hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
		hHistPreProc[i-2] = hAv;
	}
	hHistPreProc[0] -= 2;
	av = 0;av1 = 0;
	for(i=1;i<=hHistPreProc[0];i++) {
		av += hHistPreProc[i];
		if(i == 100) av1 = av;
	}
	av /= hHistPreProc[0];
	av1 /= 100;
	clean1 = 0;
    if(av1 > 5)
	   clean1 = testCleanTopCA(av, hHistPreProc);

//    printf("av= %ld, av1= %ld, clean1= %d\n",av, av1,clean1);

//	 	     	     for(i=1;i<=hHistPreProc[0];i++) {
//		 	     	  	      sprintf(buffLog,"hist[%d]= %d\n",i, hHistPreProc[i]);
//		 	     	   	      saveLog("", buffLog);
//	 	     	       }

	if(clean1 != 0)
		setWhitePixels(img->pixPtr, biWidthPad, img->bi.biHeight, 0, img->bi.biWidth-1, 0, clean1+10);


}

int getPixelSmearing(LPBYTE ptr, int xn, int yn, int wPad, int height){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;
	BYTE b;

	yn=height-1-yn;
	nY=yn*wPad;
	nX = (long)(xn/8);
	ptrP = ptr+nY+nX;

	mask=LEFT_BIT;
	mask >>=(xn%8);

	b=(~(*ptrP))&mask;
	if(b!=0) {
		return(0);
	}
	else {
		return(1);
	}
}


int getValueForSmearing(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH){
	int  sum = 0;
	int xna, yna;
	int iW, iH, xn, yn;
	//        LPBYTE ptr;
	//        int max;
	//  char buff[500];

	yna = idxH*gridy;
	xna = idxW*gridx;
	for(iH=0;iH<gridH;iH++){
		yn =yna+iH;
		for(iW=0;iW<gridW;iW++){
			xn = xna+iW;
			sum +=   getPixelSmearing(imgPtr, xn, yn, biWidthPad, biHeight);
		}
	}
	if(sum > (5*(gridH*gridW))/6)
		return 255;
	return 0;
}

/*
    void setPixelsOut(LPBYTE ptr, int w, int h, int xinp, int yinp, int v){
      	  int  w_bt;
      	  int x, y;
      	  int nY;
      	  BYTE  *hptr;
  	      BYTE mask;

            w_bt=w;
            x = xinp/8;
            y=h-1-yinp;
            nY=y*w_bt;
            hptr=ptr+nY+x;

	        mask=LEFT_BIT;
	        mask >>=(x%8);
            if(v!=0){
 *hptr = *hptr | mask;
	         }
	         else {
 *hptr = *hptr & (~mask);
	         }
    }
 */
void setPixelsOut(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2, int v){
	int sww, i ,j, dy,  w_bt;
//	int sww, i ,j, dy, sum, w_bt;
	int xStart, xStop;
	LONG nY;
	BYTE  *hptr;

	w_bt=w;
	dy=y2-y1+1;
	i=0;
	xStart = x1/8;
	xStop =  x2/8;
	while(i<dy)  {
//		sum=0;
		sww=y1+i;
		sww=h-1-sww;
		nY=(LONG)sww*(LONG)w_bt;
		for(j = xStart;j<=xStop;j++) {
			hptr=ptr+nY+j;
			*hptr = v;
		}
		i++;
	}

}
int AverageValueOfHistogram(int *hHistPreProc, int start, int dd){
	int i, av, cc, stop;
	stop = start-dd;
	if(stop< 1) stop = 1;
	av = 0;
	cc = 0;
	for(i=start;i>stop;i--){
		av += hHistPreProc[i];
		cc++;
	}
	if(cc > 0)
		av /= cc;
	return av;
}


int testSmearingRightOld(int *hHistPreProc, int av){
	int clean = -1;
	int i, a, k, n, cc;
	//    	char buff[500];
	int  th = (5*av)/6;

	//    sprintf(buff,"th = %d\n",th);
	//      saveLog("", buff);

	k = -1;
	for(i = hHistPreProc[0]-12;i>3; i--){
		a = (hHistPreProc[i]+hHistPreProc[i-1]+hHistPreProc[i-2])/3;
		if(a < th) {k = i;break;}
	}

	// 	    sprintf(buff,"k = %d\n",k);
	//      saveLog("", buff);

	if (k > 100){
		cc = 0;
		n = -1;
		for(i = k;i>k-30; i--){
			a = (hHistPreProc[i]+hHistPreProc[i-1]+hHistPreProc[i-2])/3;
			if(a < 10) cc++;
			else cc = 0;
			if(cc > 10) {n = i;break;}
		}
		//    	    sprintf(buff,"n = %d\n",n);
		//   	      saveLog("", buff);

		if(n > 100) clean = n+10;
	}

	return clean;
}
int testSmearingLeftOld(int *hHistPreProc, int av){
	int clean = -1;
	int i, a, k, n, cc;
	//    	char buff[500];

	k = -1;
	for(i = 10;i<hHistPreProc[0]-100; i++){
		a = (hHistPreProc[i]+hHistPreProc[i+1]+hHistPreProc[i+2])/3;
		if(a < av) {k = i;break;}
	}

	// 	    sprintf(buff,"k = %d\n",k);
	//      saveLog("", buff);

	if (k > 10){
		cc = 0;
		n = -1;
		for(i = k;i<k+30; i++){
			a = (hHistPreProc[i]+hHistPreProc[i+1]+hHistPreProc[i+2])/3;
			if(a < 10) cc++;
			else cc = 0;
			if(cc > 10) {n = i;break;}
		}
		//    	    sprintf(buff,"n = %d\n",n);
		//   	      saveLog("", buff);

		if(n > 10) clean = n+10;
	}

	return clean;
}
int testSmearingRight(int *hHistPreProc, int av){
	int clean = -1;
	int dd = 8;
	int a1, a2, max, maxn;
	int i, k, n, i1, i2;
	int thRatio = 10;
	int ratio[500];
//	int stop = hHistPreProc[0]/2;
	int stop = hHistPreProc[0]/3;
	int count = 0;
	//    	char buff[500];

	//    	sprintf(buff," av= %d\n", av);
	//    	saveLog("", buff);
	for(i = hHistPreProc[0]- dd;i>hHistPreProc[0]- 3*dd; i--){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		if(a1<av/5) count++;
	}
	if(count > 10)
		return clean;

	k = 0;
	for(i = hHistPreProc[0]- dd;i>stop; i--){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		a2 = AverageValueOfHistogram(hHistPreProc, i-dd, dd);
        if(k>=500) break;
		ratio[k] = 0;
		if(a2 != 0)
			ratio[k] = a1/a2;
		//        sprintf(buff," i= %d ,a1= %d, a2= %d, ratio = %d\n",i, a1, a2, ratio[k]);
		//         saveLog("", buff);
		k++;
	}

	//        for(i=0;i<k;i++){
	// 	        sprintf(buff," i= %d, n= %d, ratio = %d\n",i, hHistPreProc[0]- dd-i-dd, ratio[i]);
	//            saveLog("", buff);
	//        }
	for(i=0;i<k;i++){
		if(ratio[i] > thRatio){
			i1 = i-dd/2; if(i1 < 0) i1 = 0;
			i2 = i+dd; if(i2 >k) i2 = k;
			max = 0;maxn = 0;
			for(n = i1;n<i2;n++){
				if(ratio[n] > max) {max = ratio[n];maxn = n;}
			}
			//     	        sprintf(buff,"  n= %d, max = %d\n", maxn, max);
			//                saveLog("", buff);

			if(max > thRatio) {
				clean = hHistPreProc[0]- dd-maxn-dd;
				break;
			}
		}
	}

	return clean;
}

int testSmearingLeft(int *hHistPreProc, int av){
	int clean = -1;
	int dd = 8;
	int a1, a2, max, maxn;
	int i, k, n, i1, i2;
	int thRatio = 6;
//	int thRatio = 100;
	int ratio[500];
	int stop = hHistPreProc[0]/2;
	int count = 0;
	//    	char buff[500];


	//		 sprintf(buff," av/5= %d\n",av/5);
	//  		 saveLog("", buff);

	for(i = dd+5;i< 3*dd+5; i++){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		if(a1<av/5) count++;
	}
	if(count > 10)
		return clean;

	k = 0;
	for(i = dd+5;i<stop; i++){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		a2 = AverageValueOfHistogram(hHistPreProc, i+dd, dd);
        if(k>=500) break;
		ratio[k] = 0;
		if(a2 != 0)
			ratio[k] = a1/a2;

		//    		 sprintf(buff," i= %d ,a1= %d, a2= %d, ratio = %d\n",i, a1, a2, ratio[k]);
		//    		 saveLog("", buff);

		k++;
	}
	//    	     for(i=0;i<k;i++){
	//    	       sprintf(buff," i= %d, n= %d, ratio = %d\n",i,dd+5+i, ratio[i]);
	//    	        saveLog("", buff);
	//    	     }

	for(i=0;i<k;i++){

//printf("  ratio[ %d ] = %d\n", i, ratio[i]);

		if(ratio[i] > thRatio){
			i1 = i-dd/2; if(i1 < 0) i1 = 0;
			i2 = i+dd; if(i2 >k) i2 = k;
			max = 0;maxn = 0;
			for(n = i1;n<i2;n++){
				if(ratio[n] > max) {max = ratio[n];maxn = n;}
			}
			//     	        sprintf(buff,"  n= %d, max = %d\n", maxn, max);
			//                saveLog("", buff);

			if(max > thRatio) {
				clean = dd+5+maxn;
				break;
			}
		}
	}


	return clean;
}

int testSmearingBottom(int *hHistPreProc, int av){
	int clean = -1;
	int dd = 8;
	int a1, a2, max, maxn;
	int i, k, n, i1, i2;
	int thRatio = 10;
	int ratio[500];
	int stop = hHistPreProc[0]/2;
	int count = 0;
	//    	char buff[500];

	//    	sprintf(buff," av= %d\n", av);
	//    	saveLog("", buff);


	for(i = hHistPreProc[0]- dd;i>hHistPreProc[0]- 3*dd; i--){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		if(a1<av/5) count++;
	}
	if(count > 10)
		return clean;

	k = 0;
	for(i = hHistPreProc[0]- dd;i>stop; i--){
		a1 = AverageValueOfHistogram(hHistPreProc, i, dd);
		a2 = AverageValueOfHistogram(hHistPreProc, i-dd, dd);
        if(k>=500) break;
		ratio[k] = 0;
		if(a2 != 0)
			ratio[k] = a1/a2;
		//        sprintf(buff," i= %d ,a1= %d, a2= %d, ratio = %d \n",i, a1, a2, ratio[k] );
		//         saveLog("", buff);
		k++;
	}


	//        for(i=0;i<k;i++){
	// 	        sprintf(buff," i= %d, n= %d, ratio = %d\n",i, hHistPreProc[0]- dd-i-dd, ratio[i]);
	//            saveLog("", buff);
	//        }

	for(i=0;i<k;i++){
		if(ratio[i] > thRatio){
			i1 = i-dd/2; if(i1 < 0) i1 = 0;
			i2 = i+dd; if(i2 >k) i2 = k;
			max = 0;maxn = 0;
			for(n = i1;n<i2;n++){
				if(ratio[n] > max) {max = ratio[n];maxn = n;}
			}
			//     	        sprintf(buff,"  n= %d, max = %d\n", maxn, max);
			//                saveLog("", buff);

			if(max > thRatio) {
				clean = hHistPreProc[0]- dd-maxn-dd;
				break;
			}
		}
	}

	return clean;
}

int testBarCode(int *vHist, int th1, int th2, int th3){
	int i, cc, k, ccSp;
	k = 1;ccSp = 0;
	while(k<vHist[0]){
	   cc = 0;
	   for(i=k;i<vHist[0];i++){
		   if(vHist[i]>th1) cc++;
		   else cc = 0;
		   if(cc > th2) break;
	   }
	   k = i-cc+1;
	   cc = 0;
	   for(i=k;i<vHist[0];i++){
		   if(vHist[i]<th3) cc++;
		   else cc = 0;
		   if(cc > th2) {ccSp++;break;}
	   }
	   k = i-cc+1;
	   if(ccSp>6) break;
	}
	return ccSp;

}
void smearing(LPIMGDATA imgInp, LPIMGDATA imgOut, int US_state){
	int ddSmearingx = 16;
	int ddSmearingy = 16;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int m;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	//    int           sizeOut;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;



	biWidthPad = ((imgInp->bi.biWidth*imgInp->bi.biBitCount+31)/32)*4;

	sizeW = imgInp->bi.biWidth/ddSmearingx;
	lastW = imgInp->bi.biWidth - sizeW*ddSmearingx;
	if(lastW > 0)
		sizeW++;
	else
		lastW = ddSmearingx;

	sizeH = imgInp->bi.biHeight/ddSmearingy;
	lastH = imgInp->bi.biHeight - sizeH*ddSmearingy;
	if(lastH > 0)
		sizeH++;
	else
		lastH = ddSmearingy;


	//	    sizeOut = offBits + sizeWbyte * sizeH;

	ptrImgOut = yl_calloc("PP4", imgInp->bf.bfSize, sizeof(BYTE));
	if(ptrImgOut == NULL) {
		printf("yl_calloc failed, ptrImgOut: smearing\n");
		return;
	}
	bf = (LPBITMAPFILEHEADER) ptrImgOut;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, imgInp->bf.bfSize);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, imgInp->bi.biWidth);
	set4Bytes(bi->biHeight, imgInp->bi.biHeight);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 1;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, 0);
	set4Bytes(bi->biXPelsPerMeter, imgInp->bi.biXPelsPerMeter);
	set4Bytes(bi->biYPelsPerMeter, imgInp->bi.biYPelsPerMeter);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;
	ptrBuffOut = ptrImgOut+ offBits;


	for(idxH=0;idxH<sizeH;idxH++){
		gridH = ddSmearingy;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = ddSmearingx;
			if(idxW == sizeW-1) gridW = lastW;

			m = getValueForSmearing(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);

			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
		}

	}

	// 	    saveBMPbuffer (ptrImgOut, "smearing.bmp", imgInp->szOutPath);
//	saveBMPbuffer (ptrImgOut, imgInp->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

	{
		int x1, x2, y1, y2;
//			    	 char buff[500];
		long av = 0;
		int hAv;
		int clean1;
		int rest;
		int xC, yC;
		int i;


		rest = biWidthPad*8-imgInp->bi.biWidth*imgInp->bi.biBitCount;
		xC = (imgInp->bi.biWidth*imgInp->bi.biBitCount)/2;
		yC = imgInp->bi.biHeight/2;

		// left side
		x1 = 8;
		x2 = xC;
		if(US_state == 2)
			x2 = (imgInp->bi.biWidth*imgInp->bi.biBitCount)/3;
		y1 = yC-(2*yC)/3;
		y2 = yC+(2*yC)/3;
		histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);
		for(i=3;i<=hHistPreProc[0];i++) {
			hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
			hHistPreProc[i-2] = hAv;
		}
		hHistPreProc[0] -= 2;
		for(i=1;i<=hHistPreProc[0];i++) {
			av += hHistPreProc[i];
		}
		av /= hHistPreProc[0];

		//	  	   	     for(i=1;i<=hHistPreProc[0];i++) {
			//	  	   	  	      sprintf(buff,"%d, %ld, %d\n",i, av, hHistPreProc[i]);
			//	   	   	   	      saveLog("", buff);
		//	  	   	       }


		clean1 = testSmearingLeft(hHistPreProc, av);
		//	 	  	   	    sprintf(buff,"clean1 = %d\n",clean1);
		//	 	  	   	  	      saveLog("", buff);

// printf("--------------------------------------------clean1 = %d\n",clean1);

		if(clean1 > 0) {
			setWhitePixels(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, 0, clean1, 0, imgInp->bi.biHeight-1);
		}

		// right side

		x1 = xC;
		x2 = (imgInp->bi.biWidth*imgInp->bi.biBitCount)-rest;
		y1 = yC-(2*yC)/3;
		y2 = yC+(2*yC)/3;
		histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1-1, x1, x2, y1, y2, hHistPreProc);

		for(i=3;i<=hHistPreProc[0];i++) {
			hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
			hHistPreProc[i-2] = hAv;
		}
		hHistPreProc[0] -= 2;
		for(i=1;i<=hHistPreProc[0];i++) {
			av += hHistPreProc[i];
		}
		av /= hHistPreProc[0];


		// 	   	     for(i=1;i<=hHistPreProc[0];i++) {
			// 	   	  	      sprintf(buff,"%d, %ld, %d\n",i, av, hHistPreProc[i]);
		//  	   	   	      saveLog("", buff);
		// 	   	       }


		clean1 = testSmearingRight(hHistPreProc, av);
		//	  	   	    sprintf(buff,"clean1 = %d\n",clean1);
		//	  	   	  	      saveLog("", buff);

		if(clean1 > 0) {
			clean1 += x1;
			//	  	            setWhitePixelsX(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, clean1, imgInp->bi.biWidth-1, clean2, img->bi.biWidth-1, 0, imgInp->bi.biHeight-1);
			setWhitePixels(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, clean1, imgInp->bi.biWidth-1, 0, imgInp->bi.biHeight-1);
		}


		//	  	   	     for(i=1;i<=hHistPreProc[0];i++) {
		//	  	   	  	      sprintf(buff,"%ld,%d\n",av, hHistPreProc[i]);
		//	  	   	   	      saveLog("", buff);
		//	  	   	       }

         if(US_state == 3){
        	 int cc, k1, k2, k3, yk1, yk2, kk;
//top

	  	   	     x1 = xC-(2*xC)/3;
	  	   	     x2 = xC+(2*xC)/3;
	  	   		 y1 = 0;
	  	   	     y2 = yC;
	  	   	     histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	  	   	     cc = 0;k1 = -1;
 	   	         for(i=1;i<hHistPreProc[0];i++) {
 	   	        	 if(hHistPreProc[i] > 50) cc++;
 	   	        	 else cc = 0;
	 	   	         if(cc > 4) {k1 = i;break;}
 	   	         }
// 	printf("------------k1= %d\n",k1);
 	   	         if(k1 > 0 && k1 < 10){
 	   	        	cc = 0;k2 = -1;
                    for(i=k1;i<hHistPreProc[0];i++) {
  	 	   	        	if(hHistPreProc[i] < 5) cc++;
  	 	   	        	else cc = 0;
  	 	   	            if(cc > 4) {k2 = i;break;}
	  	 	   	    }
//  	printf("------------k2= %d\n",k2);
    	   	         if(k2 > k1){
    	 	   	        	cc = 0;k3 = -1;
    	                    for(i=k2;i<hHistPreProc[0];i++) {
   	    	   	        	   if(hHistPreProc[i] > 50) cc++;
    	    	   	           else cc = 0;
    	   	 	   	           if(cc > 4) {k3 = i;break;}
    		  	 	   	    }
//  	printf("------------k3= %d, k3-k2= %d\n",k3, k3-k2);
    	                    if(k3-k2 > 50){
                    			setWhitePixels(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, 0, imgInp->bi.biWidth-1, 0, k2);
    	                    }
    	   	         }
 	   	         }



// bottom
	  		  	   	     x1 = xC-(2*xC)/3;
	  		  	   	     x2 = xC+(2*xC)/3;
//	  		  	   		 x2 = (imgInp->bi.biWidth*imgInp->bi.biBitCount)-rest;
	  		  	   		 y1 = yC;
	  		  	   		 y2 = imgInp->bi.biHeight-1;
	  		  	   	     histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2, hHistPreProc);
	  		  	   	     cc = 0;k1 = 0;
	  	 	   	         for(i=hHistPreProc[0];i>0;i--) {
	  	 	   	        	if(hHistPreProc[i] > 15) cc++;
	  	 	   	        	else cc = 0;
	  	 	   	            if(cc > 4) {k1 = i;break;}
 	  	 	   	         }
	  	 	   	         cc = 0;k2 = 0;
	  	 	   	         if(hHistPreProc[0] - k1 < 100){
		  	 	   	         for(i=k1;i>0;i--) {
		  	 	   	        	if(hHistPreProc[i] < 5) cc++;
		  	 	   	        	else cc = 0;
		  	 	   	            if(cc > 4) {k2 = i;break;}
	 	  	 	   	        }
                      		yk1 = y1+k1;
                      		yk2 = y1+k2;
//          	printf("------------k2= %d, hHistPreProc[0] - k2= %d, k1= %d, k1-k2= %d, y1= %d, y2= %d\n",k2,hHistPreProc[0] - k2, k1, k1-k2,yk1, yk2);
                            if(yk1 < imgInp->bi.biHeight && hHistPreProc[0] - k2 < 500 && k1-k2 > 50){
//             	printf("------------x1= %d, x2= %d, y1= %d, y2= %d\n",x1, x2, yk1, yk2);
                      		   histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1-1, x1, x2, yk2, yk1, hHistPreProc);

//  		  	   	                                    for(i=1;i<=hHistPreProc[0];i++) {
//                      		 	  		  	   	  	      sprintf(buff,"hist[ %d ]= %d\n",i, hHistPreProc[i]);
//                      		 	  		  	   	   	      saveLog("", buff);
//                      		 	  		  	   	       }

                             kk = testBarCode(hHistPreProc, 50, 3, 5);
//          	printf("------------kk= %d, y1= %d, y2= %d\n",kk, yk1, yk2);
                             if(kk < 4){
                     			setWhitePixels(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, 0, imgInp->bi.biWidth-1, yk2, imgInp->bi.biHeight-1);
                             }

                            }
	  	 	   	         }


/*
	  		  		     for(i=3;i<=hHistPreProc[0];i++) {
	  		  		    	 hAv = ((hHistPreProc[i] +hHistPreProc[i-1]+hHistPreProc[i-2])/3);
	  		  		    	 hHistPreProc[i-2] = hAv;
	  		  		     }
	  		  		     hHistPreProc[0] -= 2;
	  		  	   	     for(i=1;i<=hHistPreProc[0];i++) {
	  		  	              av += hHistPreProc[i];
	  		  	   	     }
	  		  	   	     av /= hHistPreProc[0];


 	 	   	        for(i=1;i<=hHistPreProc[0];i++) {
	  	 	   	  	      sprintf(buff,"%d, %ld, %d\n",i, av, hHistPreProc[i]);
	  	  	   	   	      saveLog("", buff);
	  	 	   	       }


	  		  	   	     clean1 = testSmearingBottom(hHistPreProc, av);
	  		  	   	    sprintf(buff,"clean1 = %d\n",clean1);
	  		  	   	  	      saveLog("", buff);

	  		  	 	   if(clean1 > 0) {
	  		  	 		    clean1 += y1;
	  		  	          setWhitePixels(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, 0, imgInp->bi.biWidth-1, clean1, imgInp->bi.biHeight-1);
	  		  	 	   }
*/

//	  		  	   	     for(i=1;i<=hHistPreProc[0];i++) {
//	  		  	   	  	      sprintf(buff,"hist[ %d ]= %d\n",i, hHistPreProc[i]);
//	  		  	   	   	      saveLog("", buff);
//	  		  	   	       }
         }

	}



	yl_free(ptrImgOut);

}



//-------------------------------------------------------------------------------------------------------------------------------
// photoshop sharpening image ---------------------------------------------------------------------------------------------------

void setColor24(LPIMGDATA img, int xn, int yn, BYTE R, BYTE G, BYTE B){
	int  w;
	long nY;
	long nX;
	int sh = 3;
	LPBYTE ptr;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=img->pixPtr+nY+nX;
	*ptr = B;
	*(ptr+1) = G;
	*(ptr+2) = R;
}
void getColorFromImge24(LPIMGDATA img, int offset, LPBYTE R, LPBYTE G, LPBYTE B){
	int  w;
	long nY;
	long nX;
	int sh = 3;
	LPBYTE ptr;
	int xn,  yn;
	//      char buff[500];

	xn = offset%img->bi.biWidth;
	yn = offset/img->bi.biWidth;

	//   	sprintf(buff," xn= %d, yn= %d\n", xn, yn);
	// 	saveLog("-------", buff);


	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=img->pixPtr+nY+nX;
	*B=*ptr;
	*G=*(ptr+1);
	*R=*(ptr+2);
}
void setColorFromImge24(LPIMGDATA img, int offset, BYTE R, BYTE G, BYTE B){
	int  w;
	long nY;
	long nX;
	int sh = 3;
	LPBYTE ptr;
	int xn,  yn;
	xn = offset%img->bi.biWidth;
	yn = offset/img->bi.biWidth;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=img->pixPtr+nY+nX;
	*ptr= B;
	*(ptr+1) = G;
	*(ptr+2) = R;
}


void calculateGradient(LPBYTE ptr, LPIMGDATA imgInp, int ddX, int ddY){
	int x, y;
	int width, height, wPad;
	//          BYTE v0, vX1, vX_1, vY1, vY_1, vOut;
	BYTE v0, vX1, vY1, vOut;

	width  = imgInp->bi.biWidth;
	height = imgInp->bi.biHeight;
	wPad = ((width+31)/32)*4;

	// I will skip later this part of the image, so not necessary to calculate
	//         for(y=1;y<height-1;y++){
	for(y=ddY;y<height-ddY;y++){
		//             for(x=1;x<width-1;x++){
		for(x=ddX;x<width-ddX;x++){
			v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
			vX1 = getPixelCompress(imgInp->pixPtr, x+1, y, wPad, height);
			//            	 vX_1 = getPixelCompress(imgInp->pixPtr, x-1, y, wPad, height);
			vY1 = getPixelCompress(imgInp->pixPtr, x, y+1, wPad, height);
			//            	 vY_1 = getPixelCompress(imgInp->pixPtr, x, y-1, wPad, height);
			//            	 vOut = (vX1 -v0)*(vX1 -v0) + (vY1 -v0)*(vY1 -v0) +(vX_1 -v0)*(vX_1 -v0) + (vY_1 -v0)*(vY_1 -v0);
			vOut = (vX1 -v0)*(vX1 -v0) + (vY1 -v0)*(vY1 -v0);
			setPixelDecompress(ptr,  wPad, height, x, y, vOut);
		}
	}
	// I will skip later this part of the image, so not necessary to calculate
	/*
         y=0;
         for(x=0;x<width;x++){
           	 v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
           	 setPixelDecompress(ptr,  wPad, height, x, y, v0);
           }
         y=height-1;
         for(x=0;x<width;x++){
           	 v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
           	 setPixelDecompress(ptr,  wPad, height, x, y, v0);
           }
         for(y=0;y<height;y++){
        	 x = 0;
           	 v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
           	 setPixelDecompress(ptr,  wPad, height, x, y, v0);
        	 x = width-1;
           	 v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
           	 setPixelDecompress(ptr,  wPad, height, x, y, v0);
         }
	 */

}

void createEdgeMonochrom(LPIMGDATA imgInp, LPIMGDATA img, int ddX, int ddY){
	LPBYTE ptrOut = NULL;
	LPBYTE ptrT = NULL;
	int bufforSize;
	int imgWidth, imgHeight, biWidthPad;

	LPRGBQUAD          qd;

	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER)
			+ sizeof (RGBQUAD) * 2;

	imgWidth = imgInp->bi.biWidth;
	imgHeight = imgInp->bi.biHeight;
	biWidthPad = ((imgWidth+31)/32)*4;
	bufforSize = biWidthPad*imgHeight+offBits;

	ptrOut = yl_calloc("PreP2", bufforSize, sizeof(BYTE));
	if(ptrOut == NULL) {
		printf(img->error,"deCompressImage ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}
	bf = (LPBITMAPFILEHEADER) ptrOut;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, bufforSize);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, imgWidth);
	set4Bytes(bi->biHeight, imgHeight);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 1;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, bufforSize - offBits);
	set4Bytes(bi->biXPelsPerMeter, imgInp->bi.biXPelsPerMeter);
	set4Bytes(bi->biYPelsPerMeter, imgInp->bi.biYPelsPerMeter);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;

	img->ptr = ptrOut;
	img->bf.bfType = 0x4d42;
	img->bf.bfSize = bufforSize;
	img->bf.bfReserved1 =  0;
	img->bf.bfReserved2 =  0;
	img->bf.bfOffBits =   offBits;
	img->pixPtr = ptrOut +img->bf.bfOffBits;

	img->bi.biSize = offBits - 14;
	img->width = imgWidth;
	img->bi.biWidth = img->width;
	img->height = imgHeight;
	img->bi.biHeight = img->height;
	img->bi.biPlanes = 1;
	img->bi.biBitCount = 1;
	img->bi.biCompression = 0;
	img->bi.biSizeImage = bufforSize - offBits;
	img->bi.biXPelsPerMeter = imgInp->bi.biXPelsPerMeter;
	img->bi.biYPelsPerMeter = imgInp->bi.biYPelsPerMeter;
	img->bi.biClrUsed = 0;
	img->bi.biClrImportant = 0;
	img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

	ptrT = ptrOut+offBits;
	calculateGradient(ptrT, imgInp, ddX, ddY);
}

void addEdgeMonochrom(LPIMGDATA img, LPIMGDATA imgEdge){

	int x, y;
	int width, height, wPad;
	BYTE v0, v1;

	width  = img->bi.biWidth;
	height = img->bi.biHeight;
	wPad = ((width+31)/32)*4;

	for(y=50;y<height-50;y++){
		for(x=20;x<width-20;x++){
			v0 = getPixelCompress(img->pixPtr, x, y, wPad, height);
			v1 = getPixelCompress(imgEdge->pixPtr, x, y, wPad, height);
			if(v1 > 0) v0 = 0;
			setPixelDecompress(img->pixPtr,  wPad, height, x, y, v0);
		}
	}
}
void addEdgeMonochromCenter(LPIMGDATA img, LPIMGDATA imgEdge){

	int x, y;
	int width, height, wPad;
	BYTE v0, v1;

	width  = img->bi.biWidth;
	height = img->bi.biHeight;
	wPad = ((width+31)/32)*4;

	for(y=250;y<height-250;y++){
		for(x=250;x<width-250;x++){
			v0 = getPixelCompress(img->pixPtr, x, y, wPad, height);
			v1 = getPixelCompress(imgEdge->pixPtr, x, y, wPad, height);
			if(v1 > 0) v0 = 0;
			setPixelDecompress(img->pixPtr,  wPad, height, x, y, v0);
		}
	}


}
/*
int regognizeCnf(LPBYTE pixPtr, int biWidth, int biHeight, int y1, int y2, LPTICKETDATA ticketData,
		STRINGS * pResults, WORD* pwConf, QUADRANGLE* pPattern, char *iName, LPIMGDATA img, int rotateId){
	  int cnf = 0;
	  IMAGEL imageL;
	  IMAGEL* lpImageL;
	  SEGMENT segment;
	  SEGMENT * lpSegment;
	  LINEREM lineRem;
	  LINEREM * lpLineRem;
	  TEXTLINE textLine;
	  TEXTLINE * lpTextLine;
	  SPECK speck;
	  SPECK * lpSpeck;
	  IGNSUBSIGNS ignSubSigns;
	  IGNSUBSIGNS * lpIgnSubSigns;
	  CONTEXT context;
	  CONTEXT * lpContext;
	  int iFieldType;
	  int iAlphaIndx;
	  WORD wConfThreshold;
	  WORD size = 15;
	  WORD wWidthSegment = 192;
	  WORD wHeightSegment = 192;
	  WORD nNormVersion;

	  EXTRACTCHAR extractChar;
	  EXTRACTCHAR * lpExtractChar;
	  RECCHAR recChar;
	  RECCHAR * lpRecChar;
	  int iRecMode;
	  int iRecContinue;
	  STRINGS * lpStrings;
	  COORDINATES coordinates;
	  COORDINATES * lpCoordinates;
	  WORD alphaMap[8];
	  LPBYTE lpAlphaMap = (LPBYTE)&alphaMap[0];
	  int iWidthFrame;
	  int iHeightFrame;
	  int err;
	  BYTE normPict[300];
	  LPBYTE lpNormPict;
	  LPBYTE lpWorkBuffer;
	  LPBYTE lpExtractCh;

	  WORD width;
	  WORD height;
	  WORD wDpiX;
	  WORD wDpiY;
	  BYTE* lpFieldBuffer;
	  WORD* pxOrigin;
	  WORD* pyOrigin;
	  WORD* pxCorner;
	  WORD * pyCorner;

	  LPDWORD      lpImage;
	  DWORD        offBits;
  	  DWORD bitsOfset;
	  LPBYTE biPtr;
	  FIELDDATA field;

	  LPBYTE ptr;
		int angleFlag = 0;
	    double angle;

		HLPBYTE hpFieldH;
		ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
		if (ptr == NULL) {
			return -1;     // out of memory
		}
		hpFieldH.ptr = ptr;
		hpFieldH.size = 8;
		offBits = sizeof (BITMAPFILEHEADER) +
				sizeof (BITMAPINFOHEADER) +
				sizeof (RGBQUAD) * 2;
		lpImage = (LPDWORD) ptr;
		*lpImage = 0;
		*(lpImage + 1) = offBits;

	    angle = 0;

// extract field !!!!!!!!!!!!!!!
		err =
				extractField(pixPtr, biWidth, biHeight,
						200, y1,
						biWidth-200, y1,
						8, y2,
						&hpFieldH,
						&width,
						&height,
						8,
						"name",
						angleFlag,
						&angle
				);
		if(err < 0) return -2;

		{

			LPBITMAPFILEHEADER bf;
			LPBITMAPINFOHEADER bi;
			LPRGBQUAD          qd;
		    int  biSizeImage, biWidthPad;

		bf = (LPBITMAPFILEHEADER) hpFieldH.ptr;
		bf->bfType[0] = 0x42;
		bf->bfType[1] = 0x4d;
		set4Bytes(bf->bfSize, offBits + (((width + 31) /32) * 4) * height);
		bf->bfReserved1[0] = 0;
		bf->bfReserved1[1] = 0;
		set4Bytes(bf->bfOffBits, offBits);


		// fill in BITMAPINFOHEADER
		biWidthPad = ((width+31)/32)*4;

		//printf("biWidthPad= %d, img->bi.biWidth= %d \n", biWidthPad, img->bi.biWidth);

		biSizeImage = biWidthPad*height;


		bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
		set4Bytes(bi->biWidth, width);
		set4Bytes(bi->biHeight, height);
		bi->biPlanes[0] = 1;
		bi->biPlanes[1] = 0;
		bi->biBitCount[0]= 1;
		bi->biBitCount[1]= 0;
		set4Bytes(bi->biCompression, BI_RGB);
		set4Bytes(bi->biSizeImage, biSizeImage);
		set4Bytes(bi->biXPelsPerMeter, img->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, img->bi.biYPelsPerMeter);
		set4Bytes(bi->biClrUsed, 0);
		set4Bytes(bi->biClrImportant, 0);

		// generate pair of RGBQUADs (black and white)
		qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
		*(LPDWORD)qd       = 0x00000000;
		*(LPDWORD)(qd + 1) = 0x00ffffff;
//		{


//	 	    saveBMPbuffer (hpFieldH.ptr, iName, img->szOutPath);
//			{ char buffSave[200];
//			sprintf(buffSave,"rotate_%d.bmp", rotateId);
//    saveBMPbuffer (hpFieldH.ptr, buffSave, img->szOutPath);
//			}


//		}


	}


//		printf(" after saveBMPbuffer, name = %s\n",iName);
//		fflush(stdout);

//		printf(" width= %d, height= %d\n",width, height);
//		printf(" hpFieldH.ptr= %lu\n",(unsigned long)hpFieldH.ptr);
//		fflush(stdout);
//       saveAllLinesPtr(hpFieldH.ptr, width, height);

		bitsOfset = *(LPDWORD)(hpFieldH.ptr + BFOOFBITSOFF);
		biPtr = hpFieldH.ptr + BFOOFBITSOFF+4;

//		printf(" bitsOfset= %d\n",bitsOfset);
//		fflush(stdout);

	  lpFieldBuffer  = hpFieldH.ptr+bitsOfset;
	  width          = *(LPDWORD)(biPtr + BIWIDTHOFF);
	  height         = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	  wDpiX          = *(LPDWORD)(biPtr + BIXPELSPERMETEROFF);
	  wDpiY          = *(LPDWORD)(biPtr + BIYPELSPERMETEROFF);

//		printf(" width= %d, height= %d\n",width, height);
//		fflush(stdout);


	  lpImageL      = &imageL;
	  lpSegment     = &segment;
	  lpLineRem     = &lineRem;
	  lpTextLine    = &textLine;
	  lpSpeck       = &speck;
	  lpIgnSubSigns = &ignSubSigns;
	  lpContext     = &context;
	  lpExtractChar  = &extractChar;
	  lpRecChar     = &recChar;
	  lpCoordinates = &coordinates;

//	  wFieldType= 6, wFieldAlphabet= 11
	  lpImageL -> lpPointer  = lpFieldBuffer;
	  lpImageL -> iLineWidth = (int)width;
	  lpImageL -> iImgHeight = (int)height;
	  lpImageL -> iXBegin    = 0;
	  lpImageL -> iYBegin    = 0;
	  lpImageL -> iWidth     = (int)width;
	  lpImageL -> iHeight    = (int)height;
	  lpImageL -> iDpiX      = (int)wDpiX;
	  lpImageL -> iDpiY      = (int)wDpiY;

	  lpSegment -> iVer       = 1;
	  lpSegment -> iCellInfo  = 0;
	  lpSegment -> iCellWidth = 0;

	  lpLineRem -> iHorLineRem = 1;
	  lpLineRem -> iVerLineRem = 0;

	  lpTextLine -> iVer        = 1;
	  lpTextLine -> iLineHeight = 0;

	  lpSpeck -> iHSize         = 0;
	  lpSpeck -> iVSize         = 0;

	  lpIgnSubSigns -> iIgnSubSigns    = 0;
	  lpIgnSubSigns -> iIgnSubSignsPar = 0;

	  lpContext -> iContextOn  = 0;
	  lpContext -> iDictIndx   = 0;
	  lpContext -> iCorrectionLevel = 1;
	  nNormVersion = 1;

	  iFieldType = 6;

	  iAlphaIndx = 14; // ?
//	  iAlphaIndx = 2; // ?
	  wConfThreshold  = 20;

	  iWidthFrame  = width  - 2*EXTRACTMARGIN;
	  iHeightFrame = height - 2*EXTRACTMARGIN;

	  lpWorkBuffer = &workBuffer[0];

	  lpExtractCh = &extractCh[0];
	  lpExtractChar -> lpExtractCh    = lpExtractCh;
	  lpExtractChar -> iWidthSegment  = wWidthSegment;
	  lpExtractChar -> iHeightSegment = wHeightSegment;

	  lpNormPict = &normPict[0];
	  lpRecChar -> lpNormPict = lpNormPict;
	  lpRecChar -> iSize      = (int)size;
	  lpRecChar -> iNormVersion = (int)nNormVersion;

	  iRecMode = 0;

	  iRecContinue = 0;

	  lpStrings = pResults;

	  pxOrigin = &xOriginR[0];
	  pyOrigin = &yOriginR[0];
	  pxCorner = &xCornerR[0];
	  pyCorner = &yCornerR[0];

	  lpCoordinates -> xOrigin = (int*)pxOrigin;
	  lpCoordinates -> yOrigin = (int*)pyOrigin;
	  lpCoordinates -> xCorner = (int*)pxCorner;
	  lpCoordinates -> yCorner = (int*)pyCorner;
	  {int i;
	  for(i=0;i<8;i++) alphaMap[i] = 0;
	  }

//		printf(" before recField\n");
//		fflush(stdout);
	  ticketData->fieldAddress = &field;
	  strcpy(ticketData->fieldAddress->name, iName);
	  ticketData->fieldBMPPtr = hpFieldH.ptr;

	  err=recField(ticketData, lpImageL, lpSegment, lpLineRem, lpTextLine,
	           lpSpeck, lpIgnSubSigns, lpContext, iFieldType, iAlphaIndx,
	           lpAlphaMap, wConfThreshold, iWidthFrame, iHeightFrame, lpWorkBuffer,
	           lpExtractChar, lpRecChar, iRecMode, iRecContinue,
	           lpStrings, pwConf, lpCoordinates);

//	  {char bufLog[500];
//	     sprintf(bufLog,"result= %s\n",lpStrings->lpResult);
//	     printf(bufLog);
//	     saveLog(bufLog, "");
//	  }
	  {int i, cc = 0;
	     cnf = 0;
	     for(i=0;i<strlen(lpStrings->lpResult);i++){
	    	 if(lpStrings->lpResult[i]>32){
	    		 cnf += pwConf[i];
	    		 cc++;
	    	 }
	     }
	     if(cc > 0) cnf /= cc;
	  }

//printf(" l_free(hpFieldH.ptr)\n");
//fflush(stdout);
	 yl_free(hpFieldH.ptr);
	 ticketData->fieldBMPPtr = NULL;
// printf(" after l_free(hpFieldH.ptr)\n");
// fflush(stdout);
	  return cnf;
}
int getCNfforThisRotation(LPIMGDATA imgOut, LPTICKETDATA ticketData, int rotateId){
   int  cnf;
   BYTE * pDataBlock;

		if(sizeof(DWORD)*(MAXCHARS * 22 + 4) > ticketData->workArea.size  ) {
//			sprintf(buff," ticketData->workArea.size= %ld\n",ticketData->workArea.size);
//			saveLog("-------", buff);

			return -1;
		}

		pDataBlock = ticketData->workArea.ptr;

		ticketData->strings.lpCand1  = (char*)pDataBlock;
		ticketData->strings.lpCand2  = (char*)(pDataBlock + MAXCHARS + 1);
		ticketData->strings.lpResult = (char*)(pDataBlock + MAXCHARS * 2 + 2);
		ticketData->strings.lpReject = (char*)(pDataBlock + MAXCHARS * 3 + 3);
		ticketData->flConf = (LPWORD) (pDataBlock  + MAXCHARS * 4 + 4);
		ticketData->flQuad = (LPQUADRANGLE)(pDataBlock  + MAXCHARS * 6 + 4);


	  strcpy(ticketData->currentImgName, imgOut->szIMGName);

	ticketData->sizeWImg = imgOut->bi.biWidth;
	ticketData->sizeHImg = imgOut->bi.biHeight;

	cnf = regognizeCnf(imgOut->pixPtr, imgOut->bi.biWidth, imgOut->bi.biHeight, imgOut->bi.biHeight/4, imgOut->bi.biHeight/2, ticketData,
			&ticketData->strings, ticketData->flConf, ticketData->flQuad, "ang180.bmp", imgOut, rotateId);


	return cnf;
}
int getRotationAngle(LPIMGDATA imgOut, LPTICKETDATA ticketData, int rotateId){
  int  cnf[4]; // 0, 90, 180, 270;
  int ang[4] = {0,90,180,270};
  int i, iMax, max;

      for(i=0;i<4;i++){
	    cnf[0] = getCNfforThisRotation(imgOut, ticketData, rotateId);
	    rotate(imgOut, 90);
      }
      max = cnf[0];iMax = 0;
      for(i=1;i<4;i++){
    	  if(cnf[i]>max) {max = cnf[i];iMax = i;}
      }

   printf(" cnf0= %d, cnf90= %d, cnf180= %d, cnf270= %d\n",cnf[0], cnf[1], cnf[2], cnf[3]);
   printf(" best rotation= %d\n",ang[iMax]);
   fflush(stdout);

   return ang[iMax];

}
*/
void saveAllLinesPtr(LPBYTE hpFieldHPtr, int width, int height){

    char buffLog[200];
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
    int  y, yy;
    long lY;
    int wImg  =((width+31)/32)*4;
    LPBYTE hptr, ptrStart = hpFieldHPtr+offBits;
    y = 0;

    while(y<height)  {
      yy=y;
      yy=height-1-y;
      lY=(LONG)yy*(LONG)wImg;
      hptr=ptrStart+lY;

    sprintf(buffLog, "StartPointer= %lu, y= %d, hptr= %lu, value= %d\n", (unsigned long)ptrStart, y, (unsigned long)hptr, *hptr);
    saveLog(buffLog, "");
    y++;
    }

}
void cleanBlackSideScratchOff(LPIMGDATA imgOut, int k1, int k2, int mode){
	int biWidthPadOut, biHeightOut;

	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	if(mode==0){
		setWhitePixelsX(imgOut->pixPtr, biWidthPadOut, biHeightOut, 0, imgOut->bi.biWidth-1, 0, imgOut->bi.biWidth-1, k1, k2);
	}else{
		setWhitePixelsX(imgOut->pixPtr, biWidthPadOut, biHeightOut, k1, k2, k1, k2, 0, biHeightOut-1);
	}
}

void getHistogramForScratchOff(LPIMGDATA imgOut, float *hist, int mode) {
	int biWidthPadOut, biHeightOut;
	int x1, x2, y1, y2;
	int xC, yC;
	int loop;

	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	xC = (imgOut->bi.biWidth*imgOut->bi.biBitCount)/2;
	yC = imgOut->bi.biHeight/2;

	if(mode==0){
	      x1 = xC-xC/2;
		  x2 = xC+xC/2;
		  y1 = 0;
		  y2 = imgOut->bi.biHeight-1;
//		  histHTest_X1_X1(imgOut->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1, y2, hist);
		  for(loop= y1;loop<y2;loop++)
              hist[loop+1] = getNextDataForMonoLocalTH2(imgOut->pixPtr, imgOut->bi.biWidth, biHeightOut, x1, x2-x1, loop, 1, 1);
		  hist[0] = loop;
	}else{
		  x1 = 0;
		  x2 = imgOut->bi.biWidth-1;
		  y1 = yC-yC/2;
		  y2 = yC+yC/2;
//		 histVPtrPix(imgOut->pixPtr, biWidthPadOut, biHeightOut, H_SIZE_PREPROC, x1, x2, y1, y2, hist);
		  for(loop= x1;loop<x2;loop++)
              hist[loop+1] = getNextDataForMonoLocalTH2(imgOut->pixPtr, imgOut->bi.biWidth, biHeightOut, loop, 1, y1, y2-y1, 0);
		  hist[0] = loop;

	}
	loop = biWidthPadOut; // to avoid compiler warning
}

void cleanSidesForScratchOff(LPIMGDATA imgOut){

	 float hist[5000];
	 float av;
	 int kCut;
//		 char buff[500];
	 int kk, kkC, flg, j;

	 getHistogramForScratchOff(imgOut, hist, 1);
	 for(kk=1;kk<hist[0]-4;kk++){
		 av = (hist[kk]+hist[kk+1]+hist[kk+2]+hist[kk+3])/4;
		 hist[kk] = av;
//			 sprintf(buff," hist[ %d ]= %e\n",kk, hist[kk]);
//			 saveLog("-----------------------------------", buff);
	 }
//		 kkC = hist[0]/2;
//		 kkC3 = kkC/3;
	 flg = 0;
	 for(kk=0;kk<100;kk++){
		 if(hist[kk] < 0.00000001) flg++;
		 else flg = 0;
		 if(flg > 20) break;
	 }
	 if(flg < 20) {
	    kkC = 0;
	    if(hist[0] > 300) kkC = 300;
	    for(kk=kkC;kk>16;kk--){
		 if(hist[kk] > 0.00999) {
			 flg = 0;
			 for(j=0;j<15;j++){
				 if(hist[kk-j] < 0.00999) {flg= 1;break;}
			 }
			 if(flg == 0) {
				 kCut = kk; if(kCut < 300 ) kCut += 22;
				 cleanBlackSideScratchOff(imgOut, 0, kCut, 1);break;
			 }
			 kk -= j;
		 }
	   }
	 }
	 flg = 0;
	 for(kk=hist[0];kk>hist[0]-100;kk--){
		 if(hist[kk] < 0.00000001) flg++;
		 else flg = 0;
		 if(flg > 20) break;
	 }
	 if(flg < 20) {
	    kkC = hist[0];
	    if(hist[0] > 300) kkC = hist[0]-300;
	    for(kk=kkC;kk<hist[0]-16;kk++){
		 if(hist[kk] > 0.00999) {
			 flg = 0;
			 for(j=0;j<15;j++){
				 if(hist[kk+j] < 0.00999) {flg= 1;break;}
			 }

//printf(" kk= %d, flg= %d, kkC= %d, kkC3= %d\n",kk, flg, kkC, kkC3);
			 if(flg == 0) {
				 kCut = kk; if(hist[0]-kCut < 300 ) kCut -= 22;
				 cleanBlackSideScratchOff(imgOut, kCut, hist[0], 1);break;
			 }
			 kk += j;
		 }
	    }
	 }

	 getHistogramForScratchOff(imgOut, hist, 0);
	 for(kk=1;kk<hist[0]-4;kk++){
		 av = (hist[kk]+hist[kk+1]+hist[kk+2]+hist[kk+3])/4;
		 hist[kk] = av;
//			 sprintf(buff," hist[ %d ]= %e\n",kk, hist[kk]);
//			 saveLog("-----------------------------------", buff);
	 }
//		 kkC = hist[0]/2;
//		 kkC3 = kkC/3;
	 flg = 0;
	 for(kk=0;kk<100;kk++){
		 if(hist[kk] < 0.00000001) flg++;
		 else flg = 0;
		 if(flg > 20) break;
	 }
	 if(flg < 20) {
	    kkC = 0;
	    if(hist[0] > 300) kkC = 300;
	    for(kk=kkC;kk>16;kk--){
		 if(hist[kk] > 0.00999) {
			 flg = 0;
			 for(j=0;j<15;j++){
				 if(hist[kk-j] < 0.00999) {flg= 1;break;}
			 }
			 if(flg == 0) {
				 kCut = kk; if(kCut < 300 ) kCut += 22;
				 cleanBlackSideScratchOff(imgOut, 0, kCut, 0);break;
			 }
			 kk -= j;
		 }
	   }
	 }
	 flg = 0;
	 for(kk=hist[0];kk>hist[0]-100;kk--){
		 if(hist[kk] < 0.00000001) flg++;
		 else flg = 0;
		 if(flg > 20) break;
	 }
	 if(flg < 20) {
	    kkC = hist[0];
	    if(hist[0] > 300) kkC = hist[0]-300;
	    for(kk=kkC;kk<hist[0]-16;kk++){
		 if(hist[kk] > 0.00999) {
			 flg = 0;
			 for(j=0;j<15;j++){
				 if(hist[kk+j] < 0.00999) {flg= 1;break;}
			 }

//printf(" kk= %d, flg= %d, kkC= %d, kkC3= %d\n",kk, flg, kkC, kkC3);
			 if(flg == 0) {
				 kCut = kk; if(hist[0]-kCut < 300 ) kCut -= 22;
				 cleanBlackSideScratchOff(imgOut, kCut, hist[0], 0);break;
			 }
			 kk += j;
		 }
	    }
	 }
}
void cutTopImage(LPIMGDATA img, int top){
	int biWidthPad = ((img->bi.biWidth+31)/32)*4;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	int bfSize, newIimgHeight;

    newIimgHeight = img->bi.biHeight - top;
    bfSize = biWidthPad*newIimgHeight+offBits;

//	printf(" newIimgHeight= %d, bfSize= %d\n", newIimgHeight, bfSize);
//	fflush(stdout);

	bf = (LPBITMAPFILEHEADER) img->ptr;
	set4Bytes(bf->bfSize, bfSize);
	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));

	set4Bytes(bi->biHeight, newIimgHeight);
	set4Bytes(bi->biSizeImage, bfSize - offBits);

	img->bf.bfSize = bfSize;
	img->height = newIimgHeight;

	img->bi.biHeight = newIimgHeight;
	img->bi.biSizeImage = bfSize - offBits;
}
void testForSmallImg(LPIMGDATA img){
	int biWidthPadOut, biHeightOut;
	int x1, x2, y1, y2;
	int xC, yC, i, k, cc;

//	char buff[500];

	biWidthPadOut = ((img->bi.biWidth+31)/32)*4;
	biHeightOut = img->bi.biHeight;

	xC = (img->bi.biWidth*img->bi.biBitCount)/2;
	yC = img->bi.biHeight/2;
    x1 = xC-xC/2;
    x2 = xC+xC/2;
    y1 = 0;
    y2 = yC+yC/2;
    histHTest_X1_X1(img->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1, y2, hHistPreProc);

    cc = 0;k = 1;
	for(i=1;i<=hHistPreProc[0];i++) {
		k = i;
		if(hHistPreProc[i] > 5) cc++;
		else cc = 0;
		if(cc > 3) break;
//		   sprintf(buff," hHistPreProc[ %d ]= %d\n",i, hHistPreProc[i]);
//   	        saveLog("", buff);
	}
	if(k > 600) {
		cutTopImage(img, k/2);
	}

}



