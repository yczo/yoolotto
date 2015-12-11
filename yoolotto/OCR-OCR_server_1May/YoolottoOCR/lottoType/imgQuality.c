/*
 * imgQuality.c
 *
 *  Created on: Jun 13, 2014
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
#include  "../headers/ocr.h"

#include "../headers/BarCode.h"

extern int Red1Idx, Red2Idx;
extern char outputDMCode[DM_CODE_LEN];



int getValueForSmearing(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getValueForSmearing_White(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getValueForSmearingTH(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH, int TH);
void setPixelsOut(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2, int v);
void saveBMPbuffer (LPBYTE buff, char* name, char* path);
void createDesityTab(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int* densityTab, int len);
int getBarcodeYFromDensityTable_many(int *dTab, int *y1, int *y2, int length, int size);
void getBarcodeXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
int getYtopForAng(int *hHistPreProc);
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
//int barDeCodeI25_verY (int *hist, char *strResult);
//int barDeCodeI25_verYInv (int *hist, char *strResult);
int barDeCodeI25 (int   *hist, int lenH, LPMASK maskAlph, int lenStr, int nOfCharInBar, int invDec,
        LPSTR strResult, int *confResult, LPSTR cand2);
void getBarcodeYFromDensityTable(int *dTab, int *y1, int *y2, int length);
int getYbotForAng(int *hHistPreProc);

int getValuesFromBarCode_1(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int imgIdx);
int getValuesFromBarCode_2(LPIMGDATA img, int x1, int x2, int y1, int y2);
int barcodeTest2D(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx );
int testBCresults(int ret, char * strResult);
void setNewImageName(char *imgName, char *imgNInp, int loop);
//void cleanRedBarV_24BMP(LPBYTE ptrInp, int biWidthPadInp,  int biWidthInp, int biHeightInp);
//void cleanRedBarH_24BMP(LPBYTE ptrInp, int biWidthPadInp, int biWidthInp, int biHeightInp);
void setRedInfoV(int x, int y, int th);
void setRedInfoH(int x, int y, int th);
void setWhitePTR(LPBYTE ptrI);
int shiftHistToNextSpace(int *vHist);
int  bcHistogramTest(int *vHist);
int barcodeTest3D(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx);
void createDesityTabPA1(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len);
void GetFirstY_PA(int *densityTab, int *y1 );
int  getYShiftPA(int *Hist);
int getY2PA(int *Hist);
int getX1PA(int *Hist);
int getX2PA(int *Hist);
int getCnfLevelbarcode_2D(int *Hist);
int getX2PA_b(int *Hist);
int getY1PA_b(int *Hist);
int getY2PA_b(int *Hist);
//void barcode_DataMatrix(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2);
int getValuesFromBarCode_3D(LPIMGDATA img, int x1, int x2, int y1, int y2, int imgIdx);
//void getHistogramForBlack(LPIMGDATA imgOut, float *hist);
//void createEdgeMonochrom(LPIMGDATA imgInp, LPIMGDATA img, int ddX, int ddY);
//void calculateGradient(LPBYTE ptr, LPIMGDATA imgInp, int ddX, int ddY);
void setPixelDecompress(LPBYTE ptr,  int wPad, int height, int x, int y, int color);
BYTE getPixelCompress(LPBYTE ptr, int xn, int yn, int wPad, int height);
void createEdgeMonochromRev(LPIMGDATA imgInp, LPIMGDATA img, int ddX, int ddY);
int histBlurTest(int *hist);
int histBlurTest2(int *hist);
//void smearingRev(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx );
int whatIsThisPreP(BYTE  *lpPointer, int  wImg, int wImgHeight, int xx, int yy);
int cleanSpecksImg( SPECKCOUNT  *lpData  );
void pre_ProcessColorImage_getImagesForOCR_Old(LPIMGDATA img, LPIMGDATA *imgOut, int byteSteps, int US_state, EDGEINFO **lpEdgeInfo);
//int recognizeAllLines(LPIMGDATA img, int x1, int x2, int y1, int y2, LPTICKETDATA ticketData);
int recognizeAllLines(LPIMGDATA img, int x1, int x2, int y1, int y2, LPTICKETDATA ticketData, int idx, int sumcc_V_H);
int regognizeOCR_BC(int x1, int x2, int y1, int y2, LPTICKETDATA ticketData,
		STRINGS * pResults, WORD* pwConf, QUADRANGLE* pPattern, LPIMGDATA img, double angle, int iAlphaIndx, int idx, char *fieldName);
//void trimSpOut(LPTICKETDATA ticketData);
int TrimField (LPTICKETDATA ticketDataPtr);
int getLine(char* results, int n, int* iStart, int* iStop);
int computeLuminosity(int r, int g, int b);
void barcode_DataMatrix(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2);
//void copyLocalTime(struct tm *mytmOut, struct tm *mytmInp);
//void getTimeDifference(struct tm *mytm1, struct tm *mytm2, int *hOut, int *mOut, int *sOut);
void getMapValues_24(LPBYTE imgPtr, int biWidth, int biWidthPad, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGBA(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGB565(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void convert24BMPtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertRGBAtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertRGB565toMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
//void convertRGBAToBMP(LPIMGDATA img, LPIMGDATA imgOutp);


void setParams_StrokeWidthTransform(int directionInp, float sigmaInp);
void processImageSWT(LPIMGDATA  image);

void ImageEdgeTest(LPIMGDATA imgInp,  int imgIdx, EDGEINFO* edgeInfo ){

  IMGDATA imgOut2;
  int biWidthPadOut2, biHeightOut2;
  int x1, x2, y1, y2;
  int xC, yC;
  int hist[5000];
//	   char buff[500];
//	   int kk;

//    char imgName[100];

   imgOut2.ptr = NULL;
   strcpy(imgOut2.szIMGName,imgInp->szIMGName);
   createEdgeMonochromRev(imgInp, &imgOut2, 20, 50);


   biWidthPadOut2 = ((imgOut2.bi.biWidth*imgOut2.bi.biBitCount+31)/32)*4;
   biHeightOut2 = imgOut2.bi.biHeight;
   xC= imgOut2.bi.biWidth/2;yC= imgOut2.bi.biHeight/2;
   x1 = xC-xC/2;x2 = xC+xC/2;
   y1 = yC-yC/2;y2 = yC+yC/2;
	histVPtrPix(imgOut2.pixPtr, biWidthPadOut2, biHeightOut2, H_SIZE_PREPROC, x1, x2, y1, y2, hist);

	edgeInfo->ccV = histBlurTest(hist);

/*
	sprintf(buff," ------------------------------histV[0]= %d, th= %d\n",hist[0], hist[0]/10+10);
	 saveLog(buff, "\n");
	 for(kk=1;kk<hist[0];kk++){
		 sprintf(buff," histV[ %d ]= %d",kk, hist[kk]);
//		 sprintf(buff,"%d",hist[kk]);
		 saveLog(buff, "\n");
	 }
	 saveLog("------------------------------------------------------------------------", "\n");
*/

   histHTest_X1_X1(imgOut2.pixPtr, biWidthPadOut2, biHeightOut2,  x1, x2, y1, y2, hist);

   edgeInfo->ccH = histBlurTest(hist);

/*
	 sprintf(buff," ------------------------------histH[0]= %d, th= %d\n",hist[0], hist[0]/10+10);
	 saveLog(buff, "\n");
	 for(kk=1;kk<hist[0];kk++){
		 sprintf(buff," histH[ %d ]= %d",kk, hist[kk]);
//		 sprintf(buff,"%d",hist[kk]);
		 saveLog(buff, "\n");
	 }
	 saveLog("------------------------------------------------------------------------", "\n");
*/

	if(imgOut2.ptr != NULL) {
		yl_free(imgOut2.ptr); // memory leak
		imgOut2.ptr = NULL;
	}
}
void speckFiltration(LPIMGDATA imgInp){

	   SPECKCOUNT spData;
	   int xC, yC;

	   xC= imgInp->bi.biWidth/2;yC= imgInp->bi.biHeight/2;


	   spData.lpPointer = imgInp->pixPtr;
	   spData.iLineWidth = imgInp->bi.biWidth;
	   spData.iImgHeight = imgInp->bi.biHeight;

	   spData.iXBegin =  50;
	   spData.iYBegin = yC-3*yC/4;
	   spData.iWidth = imgInp->bi.biWidth-100;
	   spData.iHeight = 3*yC/2;
//	   spData.iSpeckSizeV = 10;
//	   spData.iSpeckSizeH = 10;
	   spData.iSpeckSizeV = 3;
	   spData.iSpeckSizeH = 3;

	   cleanSpecksImg( &spData );
//	   spData.iSpeckSizeV = 40;
//	   spData.iSpeckSizeH = 10;
//	   cleanSpecksImg( &spData );

//	   spData.iSpeckSizeV = 15;
//	   spData.iSpeckSizeH = 35;
//	   cleanSpecksImg( &spData );

//	   setNewImageName(imgName, imgInp->szIMGName, imgIdx);
	   //   smearingRev(&imgOut2,  40, 40, imgIdx );
	   //   saveBMPbuffer (imgOut2.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

}

void copyPixelsForTests(LPBYTE ptrInp, LPBYTE ptrOut, int width, int height){
	int i;
	int biWidthPad = ((width+31)/32)*4;
	for(i=0;i<height*biWidthPad;i++){
		*(ptrOut+i) = *(ptrInp+i);
	}
}

void copyImageForTests(LPIMGDATA imgInp, LPIMGDATA imgOut) {
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
		printf(imgInp->error,"deCompressImage ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",imgInp->szIMGName, bufforSize, strerror(errno));
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

	imgOut->ptr = ptrOut;
	imgOut->bf.bfType = 0x4d42;
	imgOut->bf.bfSize = bufforSize;
	imgOut->bf.bfReserved1 =  0;
	imgOut->bf.bfReserved2 =  0;
	imgOut->bf.bfOffBits =   offBits;
	imgOut->pixPtr = ptrOut +imgOut->bf.bfOffBits;

	imgOut->bi.biSize = offBits - 14;
	imgOut->width = imgWidth;
	imgOut->bi.biWidth = imgOut->width;
	imgOut->height = imgHeight;
	imgOut->bi.biHeight = imgOut->height;
	imgOut->bi.biPlanes = 1;
	imgOut->bi.biBitCount = 1;
	imgOut->bi.biCompression = 0;
	imgOut->bi.biSizeImage = bufforSize - offBits;
	imgOut->bi.biXPelsPerMeter = imgInp->bi.biXPelsPerMeter;
	imgOut->bi.biYPelsPerMeter = imgInp->bi.biYPelsPerMeter;
	imgOut->bi.biClrUsed = 0;
	imgOut->bi.biClrImportant = 0;
	imgOut->bi.biWidthPad = ((imgOut->bi.biWidth*imgOut->bi.biBitCount+31)/32)*4;

	ptrT = ptrOut+offBits;

	copyPixelsForTests(imgInp->pixPtr, ptrT, imgWidth, imgHeight);

//	calculateGradientRev(ptrT, imgInp, ddX, ddY);
}

int calculateHlines(int *hist, int *lines, int max_H_lines){
	int numLines = 0;
	int i, k, cc, n, line;
	  k= 1;
	  line = 0;
	  while(k<hist[0]) {
		 lines[line] = -1;
//printf("left; k= %d, hist[0] = %d\n",k, hist[0]);
		 cc = 0; n = k;
	     for(i=k;i<hist[0];i++){
		     if(hist[i] > 2 ) cc++;
		     else cc = 0;
		     if(cc>2) {n = i; break;}
	     }
         if(i >= hist[0]) break;

         lines[line] = n-cc;
	     line++;
// printf(" line= %d, n = %d, cc= %d\n",line, n, cc);
	     k = n+1;
		 cc = 0; n = k;
//printf("right; k= %d, hist[0] = %d\n",k, hist[0]);
	     for(i=k;i<hist[0];i++){
		     if(hist[i] < 2 ) cc++;
		     else cc = 0;
		     if(cc>2) {n = i; break;}
	     }
 //printf(" line= %d, n = %d, cc= %d, n-k= %d\n",line, n, cc, n-k);
	     if(n>k) {
	        lines[line] = n-cc;
	        line++;
	        k = n+1;
	     }else{
	    	 lines[line] = hist[0];
	    	 line++; k = hist[0];
	     }
	  }
	  numLines = line/2;
	return numLines;
}
int recalculateLineY_t(int *hist, int yInp){
	int yOut = yInp;
	int i, cc;
	cc = 0;
//	printf("start; yOut= %d; \n",yOut);
    for(i=yInp+1;i<hist[0];i++){
    	if(hist[i] < 10) {yOut++;cc = 0;}
    	else cc++;
//    	printf("i= %d, hist[i]= %d, yOut= %d; \n",i, hist[i], yOut);
    	if(cc > 2) break;
    }
//	printf("stop; yOut= %d;\n",yOut);
	return yOut;
}
int recalculateLineY_b(int *hist, int yInp){
	int yOut = yInp;
	int i, cc;
	cc = 0;
    for(i=yInp-1;i>0;i--){
    	if(hist[i] < 10) {yOut--;cc = 0;}
    	else cc++;
    	if(cc > 2) break;
    }
	return yOut;
}
int getMassFromhist(int *hist){
	int i, mass;
	mass = 0;
	for(i=1;i<hist[0];i++)
		mass += hist[i];
	return mass;
}

void testTrashInsideLine(int *hist, int *numBlk, int *avWidthBlk){
	int countBlk= 0;
	int avWblk = 0 ;
	int i, k, k1, k2, cc, n, line;
	  k= 1;
	  line = 0;
	  while(k<hist[0]) {
//printf("left; k= %d, hist[0] = %d\n",k, hist[0]);
		 cc = 0; n = k;
	     for(i=k;i<hist[0];i++){
		     if(hist[i] > 2 ) cc++;
		     else cc = 0;
		     if(cc>2) {n = i; break;}
	     }
         if(i >= hist[0]) break;
         k1 = n-cc;
         countBlk++;
	     k = n+1;
		 cc = 0; n = k;
	     for(i=k;i<hist[0];i++){
		     if(hist[i] < 2 ) cc++;
		     else cc = 0;
		     if(cc>2) {n = i; break;}
	     }
	     if(n>k) {
	    	 k2 = n-cc;
	        k = n+1;
	     }else{
	    	 k2 = hist[0];
	    	 k = hist[0];
	     }
	     avWblk +=k2-k1;
	  }
	  *numBlk = countBlk;
	  if(countBlk>0) avWblk /= countBlk;
      *avWidthBlk = avWblk;
}

int ImageTest_Quality_Old(LPIMGDATA img, LPIMGDATA *imgOut, int byteSteps, int US_state, LPTICKETDATA ticketData){

   int retValue = -1;
   IMGDATA *imgOut_tmp;
   EDGEINFO edgeInfo[3];
   EDGEINFO *lpEdgeInfo;
   int kk;
   IMGDATA imgOutTabtmp[3];

//     time_t mytime;
//     struct tm *mytm;
//     struct tm  mytm1, mytm2;
//     int hDiff, mDiff, sDiff;


   char imgName[100];


     lpEdgeInfo = &edgeInfo[0];
     pre_ProcessColorImage_getImagesForOCR_Old(img, imgOut, byteSteps, US_state, &lpEdgeInfo);
     retValue = -1;
     for(kk=0;kk<3;kk++){
        imgOutTabtmp[kk].ptr = NULL;
     }


     imgOut_tmp = imgOut[0];
     copyImageForTests(imgOut_tmp, &imgOutTabtmp[0]);
     imgOut_tmp = &imgOut_tmp[1];
     copyImageForTests(imgOut_tmp, &imgOutTabtmp[1]);
     imgOut_tmp = &imgOut_tmp[1];
     copyImageForTests(imgOut_tmp, &imgOutTabtmp[2]);


// very white/black
     lpEdgeInfo = &edgeInfo[0];
     if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH <= 10) retValue = -77;
     if(retValue> -10) {
        if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH < 24){
            lpEdgeInfo = &edgeInfo[1];
            if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH <= 10) retValue = -77;
        }
     }
     if(retValue> -10) {
    	 lpEdgeInfo = &edgeInfo[2];
         if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH > 1000)   retValue = -77;
     }


/*
     if(retValue> -10 && retValue < 0){
         int pixX, pixY;
         pixX = 24; pixY = 24;
            for(kk=0;kk<3;kk++){
//printf("kk= %d\n",kk);
//fflush(stdout);

    	       retValue = barcodeTest3D(&imgOutTabtmp[kk], pixX, pixY, kk);
 //printf("kk= %d, retValue= %d\n",kk, retValue);
 //fflush(stdout);
    	       if(retValue >= 0) break;
            }
     }
*/

     if(retValue> -10 && retValue < 0){
    	  int biWidthPad, biHeight;
    	  int x1, x2, y1, y2;
    	  int xC, yC;
    	  int digitCount = 0; //, mass1, mass2;
    	  int too_W_B, sumcc_V_H;
   	      int pixX, pixY;

   	      pixX = 24; pixY = 24;
//    	  char buff[500];

//       	mytime = time(NULL);
//       	mytm = localtime(&mytime);
//       	copyLocalTime(&mytm1, mytm);


    	     for(kk=0;kk<3;kk++){
    		     retValue = barcodeTest2D(&imgOutTabtmp[kk], pixX, pixY, kk);
    		     if(retValue >= 0) break;

    	    	 too_W_B = 0;
//    	    	 ccBlack = 0;
//    	    	digitCount = 0;
    	        speckFiltration(&imgOutTabtmp[kk]);

    	        biWidthPad = ((imgOutTabtmp[kk].bi.biWidth*imgOutTabtmp[kk].bi.biBitCount+31)/32)*4;
    	        biHeight = imgOutTabtmp[kk].bi.biHeight;
    	        xC= imgOutTabtmp[kk].bi.biWidth/2;yC= imgOutTabtmp[kk].bi.biHeight/2;
    	        x1 = xC-3*xC/4;x2 = xC+5*xC/8;
    	        y1 = yC-5*yC/8;y2 = yC+yC/2;

//   	        printf("------kk= %d\n",kk);
   	     strcpy(imgOutTabtmp[kk].szIMGName, img->szIMGName);

   	  lpEdgeInfo = &edgeInfo[kk];
   	  sumcc_V_H = lpEdgeInfo->ccV+ lpEdgeInfo->ccH;
// 	  printf("cV= %d, cH= %d, sumcc_V_H= %d\n",lpEdgeInfo->ccV, lpEdgeInfo->ccH, sumcc_V_H);
         if(sumcc_V_H <= 10) {too_W_B = 1;break;}
   	            if(sumcc_V_H < 1000) {
	                digitCount = recognizeAllLines(&imgOutTabtmp[kk], x1, x2, y1, y2, ticketData, kk, sumcc_V_H);
//	                printf(" digitCount= %d\n",digitCount);
 	                if(digitCount > 0) {retValue = 0;break;}
   	            }
//   	            else
//   	            	too_W_B = 1;

	  printf(" retValue= %d, too_W_B= %d\n",retValue, too_W_B);
	  fflush(stdout);
       }

//   			mytime = time(NULL);
//   			mytm = localtime(&mytime);
//   			copyLocalTime(&mytm2, mytm);
//   			getTimeDifference(&mytm1, &mytm2, &hDiff, &mDiff, &sDiff);
//   		    printf("OCR     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);

     }


   imgOut_tmp = imgOut[0];
   setNewImageName(imgName, img->szIMGName, 0);
   saveBMPbuffer (imgOut_tmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

   imgOut_tmp = &imgOut_tmp[1];
   setNewImageName(imgName, img->szIMGName, 1);
   saveBMPbuffer (imgOut_tmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

   imgOut_tmp = &imgOut_tmp[1];
   setNewImageName(imgName, img->szIMGName, 2);
   saveBMPbuffer (imgOut_tmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");



     for(kk=0;kk<3;kk++){
    	 if(imgOutTabtmp[kk].ptr != NULL){
    			yl_free(imgOutTabtmp[kk].ptr);
    			imgOutTabtmp[kk].ptr = NULL;
    	 }
     }

   return retValue;
}

int barcodeTest_Quality(LPIMGDATA *imgInp){
   int pixX, pixY;
   int retValue = 0;
   IMGDATA *imgOut;

   pixX = 24; pixY = 24;
//   pixX = 64; pixY = 64;
   switch(US_State) {
   case 0: // TX
   case 2: // CA
   case 3: // NY
	   imgOut = imgInp[0];
	   retValue = barcodeTest2D(imgOut, pixX, pixY, 0);
	   if(retValue >= 0) break;

	   imgOut = &imgOut[1];
//	   printf(" imgInp[2]->szIMGName= %s\n", imgOut->szIMGName);
//	   fflush(stdout);

	   retValue = barcodeTest2D(imgOut, pixX, pixY, 1);
	   if(retValue >= 0) break;

	   imgOut = &imgOut[1];
	   retValue = barcodeTest2D(imgOut, pixX, pixY, 2);break;

   case 1: // PA
//	   retValue = barcodeTest3D(imgInp,  mainPath, imgName, pixX, pixY);break;
	   imgOut = imgInp[0];
	   retValue = barcodeTest3D(imgOut, pixX, pixY, 0);
	   if(retValue >= 0) break;

	   imgOut = &imgOut[1];
	   retValue = barcodeTest3D(imgOut, pixX, pixY, 1);
	   if(retValue >= 0) break;

	   imgOut = &imgOut[1];
	   retValue = barcodeTest3D(imgOut, pixX, pixY, 2);break;

break;
   default:
	   imgOut = imgInp[0];
	   retValue = barcodeTest2D(imgOut, pixX, pixY, 0);
	   if(retValue >= 0) break;
	   imgOut = &imgOut[1];
	   retValue = barcodeTest2D(imgOut, pixX, pixY, 1);
	   if(retValue >= 0) break;
	   imgOut = &imgOut[1];
	   retValue = barcodeTest2D(imgOut, pixX, pixY, 2);break;
   }
   return retValue;
}
void calculateGradientRev(LPBYTE ptr, LPIMGDATA imgInp, int ddX, int ddY){
	int x, y;
	int width, height, wPad;
	BYTE v0, vX1, vY1, vOut;

	width  = imgInp->bi.biWidth;
	height = imgInp->bi.biHeight;
	wPad = ((width+31)/32)*4;

	for(y=ddY;y<height-ddY;y++){

		for(x=ddX;x<width-ddX;x++){
			v0 = getPixelCompress(imgInp->pixPtr, x, y, wPad, height);
			vX1 = getPixelCompress(imgInp->pixPtr, x+1, y, wPad, height);
			vY1 = getPixelCompress(imgInp->pixPtr, x, y+1, wPad, height);
			vOut = (vX1 -v0)*(vX1 -v0) + (vY1 -v0)*(vY1 -v0);
			if(vOut != 0) vOut = 0;
			else vOut = 1;
			setPixelDecompress(ptr,  wPad, height, x, y, vOut);
		}
	}

}

void createEdgeMonochromRev(LPIMGDATA imgInp, LPIMGDATA img, int ddX, int ddY){
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
	calculateGradientRev(ptrT, imgInp, ddX, ddY);
}

void smearingRev(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx ){

	int ddSmearingx = pixX;
	int ddSmearingy = pixY;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
//	int m, stopSm;
	int m;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	IMGDATA imgOut_Sm;
	int TH;

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
		printf("yl_calloc failed, ptrImgOut");
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
	copyHeadersBMP(ptrImgOut, &imgOut_Sm);

//	stopSm = sizeH/2;
//	DENSITY_L = stopSm*ddSmearingy;

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = ddSmearingy;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = ddSmearingx;
			if(idxW == sizeW-1) gridW = lastW;
			TH = (8*(gridH*gridW))/9;

			   m = getValueForSmearingTH(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH, TH);
//			int getValueForSmearingTH(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH, int TH){
//			if(idxH > stopSm)
//			   m = getValueForSmearing(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);
//			else
//			   m = getValueForSmearing_White(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);

			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
		}
	}

	/*
// second smearing
	{
	int ddSmearingx2 = ddSmearingx*2;
	int ddSmearingy2 = ddSmearingy;
	int sizeW2, sizeH2, lastW2, lastH2, stopSm2;


	sizeW2 = imgInp->bi.biWidth/ddSmearingx2;
	lastW2 = imgInp->bi.biWidth - sizeW2*ddSmearingx2;
	if(lastW2 > 0)
		sizeW2++;
	sizeH2 = imgInp->bi.biHeight/ddSmearingy2;
	lastH2 = imgInp->bi.biHeight - sizeH2*ddSmearingy2;
	if(lastH2 > 0)
		sizeH2++;
	stopSm2 = sizeH2/2;
	{
    int mW[sizeW2];
	  for(idxH=0;idxH<sizeH2;idxH++){
		gridH = ddSmearingy2;
		if(idxH == sizeH2-1) gridH = lastH2;
		if(idxH > stopSm2) break;
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;
			TH = (3*(gridH*gridW))/6;
//		    m = getValueForSmearing(ptrBuffOut, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx2, ddSmearingy2, gridW, gridH);
		   m = getValueForSmearingTH(ptrBuffOut, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx2, ddSmearingy2, gridW, gridH, TH);
		    mW[idxW] = m;
		}
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;
			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx2, idxW*ddSmearingx2 + gridW,
					idxH*ddSmearingy2, idxH*ddSmearingy2+gridH-1, mW[idxW]);

		}
	  }
	}

	}

*/
	{
 		char imgName[100];
    setNewImageName(imgName, imgInp->szIMGName, imgIdx);
    saveBMPbuffer (ptrImgOut, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	}

	yl_free(ptrImgOut);

}

int histBlurTest(int *hist){
	int cc = 0;
	int i, th;
	th = hist[0]/10+10;
	for(i= 50;i<hist[0]-50;i++){
		if(hist[i] > th) cc++;
	}

	return cc;
}
int histBlurTest2(int *hist){
	int cc = 0;
	int i, k, th, stop;
	int v1, v2L, v2R, vDif;
	th = 4;
	vDif = 0;cc = 0;
	for(i= 6;i<hist[0]-2;i++){
		if(hist[i]+th < hist[i+1] && hist[i+1] > hist[i+2]+th){
			v1 = hist[i+1];

			v2L = v1;
			stop = i+2-5;if(stop < 1) stop = 1;
			for(k= i+2;k>=stop;k--){
				if(v2L > hist[k]) v2L = hist[k];
			}
			stop = i+2+5;if(stop >= hist[0]-2) stop = hist[0]-2;
			v2R = v1;
			for(k= i+2;k<=stop;k++){
				if(v2R > hist[k]) v2R = hist[k];
			}
			if(v1-v2L > 35 && v1-v2R > 35 ){
				if(v2L > v2R) v2L = v2R;
			    vDif += (v1-v2L);cc++;
			}
		}
	}
//	if(cc > 0)
//		vDif /= cc;
//	return vDif;
	return cc;
}

int barcodeTest2D(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx ){
	int DENSITY_L;
//	YlogoClassificationResult resultLogo;

	int ddSmearingx = pixX;
	int ddSmearingy = pixY;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int m, stopSm;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	int densityTab[imgInp->bi.biHeight];
	int typeInt = -1;//, typeIntOld = -1;
	IMGDATA imgOut_Sm;

//    char buffLog[500];


//	 printf("--------------------------------barcodeTest_Quality------------- imgIdx= %d \n", imgIdx);
//	 fflush(stdout);



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
		printf("yl_calloc failed, ptrImgOut");
		return typeInt;
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

//	imgOut_Sm.ptr = ptrImgOut;
//	imgOut_Sm.pixPtr = ptrBuffOut;
	copyHeadersBMP(ptrImgOut, &imgOut_Sm);


// LOGO/BARCODE
	stopSm = sizeH/2;
	DENSITY_L = stopSm*ddSmearingy;
//	if(stopSm > sizeH) stopSm = sizeH;

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = ddSmearingy;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = ddSmearingx;
			if(idxW == sizeW-1) gridW = lastW;
			if(idxH > stopSm)
			   m = getValueForSmearing(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);
			else
			   m = getValueForSmearing_White(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);

			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
		}
	}

// second smearing
	{
	int ddSmearingx2 = ddSmearingx*2;
	int ddSmearingy2 = ddSmearingy;
	int sizeW2, sizeH2, lastW2, lastH2, stopSm2;


	sizeW2 = imgInp->bi.biWidth/ddSmearingx2;
	lastW2 = imgInp->bi.biWidth - sizeW2*ddSmearingx2;
	if(lastW2 > 0)
		sizeW2++;
	sizeH2 = imgInp->bi.biHeight/ddSmearingy2;
	lastH2 = imgInp->bi.biHeight - sizeH2*ddSmearingy2;
	if(lastH2 > 0)
		sizeH2++;
	stopSm2 = sizeH2/2;
	{
    int mW[sizeW2];
	  for(idxH=0;idxH<sizeH2;idxH++){
		gridH = ddSmearingy2;
		if(idxH == sizeH2-1) gridH = lastH2;
		if(idxH > stopSm2) break;
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;

		    m = getValueForSmearing(ptrBuffOut, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx2, ddSmearingy2, gridW, gridH);
		    mW[idxW] = m;
		}
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;
			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx2, idxW*ddSmearingx2 + gridW,
					idxH*ddSmearingy2, idxH*ddSmearingy2+gridH-1, mW[idxW]);

		}
	  }
	}

	}


//	{
// 		char imgName[100];
//    setNewImageName(imgName, imgInp->szIMGName, imgIdx);
//    saveBMPbuffer (ptrImgOut, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//	}


	 createDesityTab(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);


// barcode test
	 {
		 int size = 5;
		 int y1[size], y2[size], x1, x2, sizeBc, i;
		 typeInt = -1;

		 sizeBc = getBarcodeYFromDensityTable_many(densityTab, y1, y2, DENSITY_L, size);
// printf("  sizeBc= %d\n", sizeBc);
		 for(i=0;i<sizeBc;i++) {
		   getBarcodeXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, y1[i], y2[i], &x1, &x2);

		   typeInt = getValuesFromBarCode_1(imgInp, x1*8, x2*8, imgInp->bi.biHeight-1-y1[i], imgInp->bi.biHeight-1-y2[i], i, imgIdx);
		   if(typeInt>=0) break;

		 }
	 }
 // second barcode test

		if(typeInt < 0)
		{
			int y1, y2, x1, x2;

//			printf("------------ second barcode test\n");

			getBarcodeYFromDensityTable(densityTab, &y1, &y2, DENSITY_L);
			getBarcodeXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, y1, y2, &x1, &x2);


			typeInt = getValuesFromBarCode_2(imgInp, x1*8, x2*8, imgInp->bi.biHeight-1-y1, imgInp->bi.biHeight-1-y2);

		}


	yl_free(ptrImgOut);
	return typeInt;
}

extern int hHistPreProc[H_SIZE_PREPROC];
int getValuesFromBarCode_1(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int imgIdx){


//	int err;
//	LPBYTE ptr;
//	LPDWORD      lpImage;
//	DWORD        offBits;
//	HLPBYTE hpFieldH;
//	WORD width;
//	WORD height;
//	LPBITMAPFILEHEADER bf;
//	LPBITMAPINFOHEADER bi;
//	LPRGBQUAD          qd;
//	int angleFlag = 0;
    double angle;
//    int  biSizeImage, biWidthPad;
	int typeInt = -1;


//	ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
//	if (ptr == NULL) {
//		return typeInt;     // out of memory
//	}
//	hpFieldH.ptr = ptr;
//	hpFieldH.size = 8;
//	offBits = sizeof (BITMAPFILEHEADER) +
//			sizeof (BITMAPINFOHEADER) +
//			sizeof (RGBQUAD) * 2;
//	lpImage = (LPDWORD) ptr;
//	*lpImage = 0;
//	*(lpImage + 1) = offBits;

    angle = 0;
    { //decode barcode
	int vHist[img->bi.biWidth];
	int i, ret;// , rsd;
	MASKDEF barMask[20];
	char strResult[64];
	int  confR[64];
	char cand2[64];
//	char typeChar[64];
	int histTestFlg= 0;

//	strcpy(typeChar, "???");

	WORD byteWidthHist = ((img->bi.biWidth + 31)/32)*4;
    for(i=0;i<20;i++)
            barMask[i].info = 128;
    fieldAngleFromBarCode = 0;

    {
    	int centx, startx, stopx, dx;
    	int yL_T, yC_T, yR_T;
    	int yL_B, yC_B, yR_B;
    	float angL_T, angL_B;
    	float angR_T, angR_B;
    	 centx = (x2+x1)/2;
    	 dx = (x2-x1)/6;
    	 startx = x1;
    	 stopx = x1+dx;
	     histHTest_X1_X1(img->pixPtr, byteWidthHist, img->bi.biHeight,  startx, stopx, y1, y2, hHistPreProc);
	     yL_T = getYtopForAng(hHistPreProc);
	     yL_B = getYbotForAng(hHistPreProc);
			startx = centx-dx;
	    	stopx = centx+dx;
	        histHTest_X1_X1(img->pixPtr, byteWidthHist, img->bi.biHeight,  startx, stopx, y1, y2, hHistPreProc);
	        yC_T = getYtopForAng(hHistPreProc);
		    yC_B = getYbotForAng(hHistPreProc);

				startx = x2-dx;
		    	stopx = x2;
			    histHTest_X1_X1(img->pixPtr, byteWidthHist, img->bi.biHeight,  startx, stopx, y1, y2, hHistPreProc);
		        yR_T = getYtopForAng(hHistPreProc);
			    yR_B = getYbotForAng(hHistPreProc);


		    stopx = x2-centx;
		    angL_T = 0;angL_B = 0;
		    if(stopx > 0){
		    	angL_T = (float)(yL_T-yC_T)/(float)stopx;
		    	angL_B = (float)(yC_B-yL_B)/(float)stopx;
		    }
		    angL_T = (float)atan((double)angL_T);
		    angL_B = (float)atan((double)angL_B);
		    angL_T = (angL_T*180)/PI;
		    angL_B = (angL_B*180)/PI;

		    angR_T = 0;angR_B = 0;
		    if(stopx > 0){
		    	angR_T = (float)(yC_T-yR_T)/(float)stopx;
		    	angR_B = (float)(yR_B-yC_B)/(float)stopx;
		    }
		    angR_T = (float)atan((double)angR_T);
		    angR_B = (float)atan((double)angR_B);
		    angR_T = (angR_T*180)/PI;
		    angR_B = (angR_B*180)/PI;

//		    printf("-----angL_T= %e, angL_B= %e\n",angL_T, angL_B);
//		    printf("-----angR_T= %e, angR_B= %e\n",angR_T, angR_B);
		    angL_T = max(angL_T, angL_B);
		    angR_T = max(angR_T, angR_B);
		    fieldAngleFromBarCode = max(angL_T, angR_T);
    }


        histVPtrPix(img->pixPtr, byteWidthHist, img->bi.biHeight, img->bi.biWidth-1, x1, x2, y1, y2, vHist);
        histTestFlg = bcHistogramTest(vHist);


         ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);
//  printf("--- histTestFlg= %d, ret= %d; strResult= %s\n",histTestFlg, ret, strResult);
         ret = testBCresults(ret, strResult);

         if(ret == 0)  typeInt = 5;
//printf("--- histTestFlg= %d\n",histTestFlg);
//       	 if(histTestFlg > 0)  typeInt = 5;

/*
    // ----------------------------- debugging

 	err =
 			extractField(img->pixPtr, img->bi.biWidth, img->bi.biHeight,
 					x1, y1,
 					x2, y1,
 					x1, y2,
 					&hpFieldH,
 					&width,
 					&height,
 					8,
 					"name",
 					angleFlag,
 					&angle
 			);
 	if(err < 0) return typeInt;



 	bf = (LPBITMAPFILEHEADER) hpFieldH.ptr;
 	bf->bfType[0] = 0x42;
 	bf->bfType[1] = 0x4d;
 	set4Bytes(bf->bfSize, offBits + (((width + 31) /32) * 4) * height);
 	bf->bfReserved1[0] = 0;
 	bf->bfReserved1[1] = 0;
 	set4Bytes(bf->bfOffBits, offBits);


 	// fill in BITMAPINFOHEADER
 	biWidthPad = ((width+31)/32)*4;

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

// 	{

 //		printBMPDataTest (img->ptr);
 //		printBMPDataTest (hpFieldH.ptr);


// 		{char name[100], numStr[10];

// 		strcpy(name, img->szIMGName);
// 		sprintf(numStr, "%d",iter);
// 		strcpy(&name[strlen(name)-4], "_");
// 		strcat(name, numStr);
// 		strcat(name, ".bmp");
//  	    saveBMPbuffer (hpFieldH.ptr, name, img->szOutPath);
// 		}

// 	}
 	{
 		char imgName[100];
 		int iter_ImgIdx = 0;
 		switch(imgIdx) {
 		case 0: iter_ImgIdx = iter;break;
 		case 1: iter_ImgIdx = 10+iter;break;
 		case 2: iter_ImgIdx = 20+iter;break;
 		}

 	   setNewImageName(imgName, img->szIMGName, iter_ImgIdx);
//  printf("--- imgName= %s \n",imgName);
//  fflush(stdout);
//	   saveBMPbuffer (hpFieldH.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
 	}
 	// ----------------------------- debugging
*/


    }

//	yl_free(hpFieldH.ptr);
	return typeInt;
}

int getValuesFromBarCode_2(LPIMGDATA img, int x1, int x2, int y1, int y2){

//	char bufLog[200];

//	int err;
//	LPBYTE ptr;
//	LPDWORD      lpImage;
//	DWORD        offBits;
//	HLPBYTE hpFieldH;
//	WORD width;
//	WORD height;

//	LPBITMAPFILEHEADER bf;
//	LPBITMAPINFOHEADER bi;
//	LPRGBQUAD          qd;

//	int angleFlag = 0;
//    double angle;

//    int  biSizeImage, biWidthPad;

	int typeInt = -1;
	int histTestFlg= 0;

//    char buffLog[500];



//	ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
//	if (ptr == NULL) {
//		return typeInt;     // out of memory
//	}
//	hpFieldH.ptr = ptr;
//	hpFieldH.size = 8;
//	offBits = sizeof (BITMAPFILEHEADER) +
//			sizeof (BITMAPINFOHEADER) +
//			sizeof (RGBQUAD) * 2;
//	lpImage = (LPDWORD) ptr;
//	*lpImage = 0;
//	*(lpImage + 1) = offBits;

//    angle = 0;
    { //decode barcode
	int vHist[img->bi.biWidth];

	WORD byteWidthHist = ((img->bi.biWidth + 31)/32)*4;

		x1 -=40;
		x2 +=40;
		if(x1 < 0) x1 = 0;
		if(x2>= img->bi.biWidth) x2 = img->bi.biWidth-1;
        histVPtrPix(img->pixPtr, byteWidthHist, img->bi.biHeight, img->bi.biWidth-1, x1, x2, y1, y2, vHist);
        histTestFlg = bcHistogramTest(vHist);
//        printf("2_1---  histTestFlg= %d\n",histTestFlg);
        if(histTestFlg > 0) typeInt = 6;

//        yl_free(hpFieldH.ptr);
        return typeInt;

/*
        {int nnH;

           for(nnH=1;nnH<=vHist[0];nnH++){
        	   sprintf(bufLog,"vHist[ %d ]= %d\n",nnH, vHist[nnH]);
        	   saveLog("", bufLog);
           }
        }
*/
/*
           ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);
  printf("2_1---  ret= %d; strResult= %s\n",ret, strResult);
        ret = testBCresults(ret, strResult);
        if(ret == 0){
        	again = 0;
      	     typeInt = 6;

        }
*/
/*
        if(typeInt < 0){
            histVPtrPix(img->pixPtr, byteWidthHist, img->bi.biHeight, img->bi.biWidth-1, x1, x2, y1, y2, vHist);
            ret = barDeCodeI25_verY (vHist, strResult);
      printf("_verY--------------ret= %d, strResult: %s\n", ret, strResult);
      ret = testBCresults(ret, strResult);
            if(ret == 0)
       	       typeInt = 7;

            if(typeInt >= 0) again = 0;
            else{
            	  ret = barDeCodeI25_verYInv (vHist, strResult);
                  printf("_verYInv--------------ret= %d, strResult: %s\n", ret, strResult);
                  ret = testBCresults(ret, strResult);
                  if(ret == 0)
                	   typeInt = 8;
            	}
                if(typeInt >= 0) again = 0;
            }
*/


//        }



/*
    // ----------------------------- debugging
 	err =
 			extractField(img->pixPtr, img->bi.biWidth, img->bi.biHeight,
 					x1, y1,
 					x2, y1,
 					x1, y2,
 					&hpFieldH,
 					&width,
 					&height,
 					8,
 					"name",
 					angleFlag,
 					&angle
 			);
 	if(err < 0) return typeInt;



 	bf = (LPBITMAPFILEHEADER) hpFieldH.ptr;
 	bf->bfType[0] = 0x42;
 	bf->bfType[1] = 0x4d;
 	set4Bytes(bf->bfSize, offBits + (((width + 31) /32) * 4) * height);
 	bf->bfReserved1[0] = 0;
 	bf->bfReserved1[1] = 0;
 	set4Bytes(bf->bfOffBits, offBits);


 	// fill in BITMAPINFOHEADER
 	biWidthPad = ((width+31)/32)*4;

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
 	{

 //		printBMPDataTest (img->ptr);
 //		printBMPDataTest (hpFieldH.ptr);


 	{
 		char imgName[100];
 	   setNewImageName(imgName, img->szIMGName, 51);
// 	  printf("--- imgName= %s \n",imgName);
// 	  fflush(stdout);
//	   saveBMPbuffer (hpFieldH.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

 	}

//  	    saveBMPbuffer (hpFieldH.ptr, img->szIMGName, img->szOutPath);

 	}
//printf("              again= %d\n", again);
 	// ----------------------------- debugging
*/

 }

// yl_free(hpFieldH.ptr);
// return typeInt;
}


int testBCresults(int ret, char * strResult){
int i, count= 0, len = strlen(strResult);

  if(len < 12) return -1;

    for(i=0;i<len;i++){
    	   if(strResult[i] < '0' || strResult[i] > '9') count++;
       }
 if(ret != 0 && ret != 5 && ret != 10 && ret != 15) return -1;
 if(count > 1) return -1;
 return 0;
}

int testRedBC_24BMP(LPBYTE ptrInp, int biWidthPadInp, int biHeightInp,
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

/*

		if(xn>100 && xn<200 && yn > 2000){
				 char buff[500];
//					printf(" x= %d, y= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",xn, yn, b, g, r, drb, drg);
					sprintf(buff," x= %d, y= %d, b= %d, g= %d, r= %d, drb= %d, drg= %d\n",xn, yn, b, g, r, drb, drg);
					saveLog("---------------", buff);
				}
*/
	if(g< 80 && b < 80) {
	    if((drb > 40 && drg > 30) || (drb > 30 && drg > 40)) {
		   setWhitePTR(ptrI);
		   ret = 1;
	    }
	}
    return ret;
}

void filterRedLeftBC_24BMP(LPIMGDATA img){

		int idxW, idxH;
		int biWidthInp, biWidthPadInp, biHeightInp;
		int flgHor;
		int dd, ret;

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
				ret = testRedBC_24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
				if(ret>0)
				  setRedInfoH(idxW, idxH, biWidthInp/2);
			}
		}
//    cleanRedBarH_24BMP(img->pixPtr, biWidthPadInp, biWidthInp,  biHeightInp);
}
else{
//	   dd = biWidthInp/2+biWidthInp/4;
	   dd = biWidthInp/2;
		for(idxH=0;idxH<biHeightInp;idxH++){
			for(idxW=0;idxW<dd;idxW++){
				ret = testRedBC_24BMP(img->pixPtr, biWidthPadInp,  biHeightInp, idxW, idxH);
				if(ret>0)
					setRedInfoV(idxW, idxH, biHeightInp/2);
			}
		}
//  cleanRedBarV_24BMP(img->pixPtr, biWidthPadInp, biWidthInp,  biHeightInp);

}
}

int shiftHistToNextSpace(int *vHist){
	int i,  k1, k2, k, len;
	len = vHist[0];

	k1 = 1;
	for(i=1;i<len-10;i++){
		if(vHist[i] > 5 && vHist[i+1] > 5 && vHist[i+2] < 5) {k1 = i;break;}
	}
	k2 = k1;
	for(i=k1;i<len-10;i++){
		if(vHist[i] < 2 && vHist[i+1] < 2 && vHist[i+2] < 2) {k2 =i+2;break;}
	}
//printf("--- k1= %d \n",k1);
	if(k2 <= k1) return -1;
//	printf("--- k2= %d \n",k1);

	k = 0;
	for(i=k2;i<=len;i++){
		k = i-k2+1;
		vHist[k] = vHist[i];
	}
	vHist[0] = k;
	printf("--- k2= %d, len= %d\n",k2,vHist[0]);
	if(vHist[0] < 700) return -1;
	return k2;
}

int  bcHistogramTest(int *vHist){

	int i,  k1, k2, len;
	int maxB, maxW, cc;
	int thB = 50;
	int thW = 2;

//    char buffLog[500];


	len = vHist[0];
//	printf("--- len= %d\n",len);
//	fflush(stdout);

// sprintf(buffLog," ----------vHist'; len= %d\n",  len);
// saveLog("", buffLog);

	if(len < 700) return -1;
	k1 = 200;
	for(i=200;i<len-10;i++){
		if(vHist[i] > thB && vHist[i+1] > thB && vHist[i+2] > thB) {k1 = i;break;}
	}

	cc = 0;
	maxB = 0;
	maxW = 0;
	k2 = 0;
	while(k1 < len-100 && k2 < len-100) {
	   k2 = k1;
	   for(i=k1;i<len-10;i++){
		   if(vHist[i] < thW && vHist[i+1] < thW && vHist[i+2] < thW) {k2 =i;break;}
	   }
	   if(k2==k1) break;
	   if(k2-k1 > maxB) maxB = k2-k1;

//  printf("--- maxB= %d, k1= %d; k2= %d\n",maxB, k1, k2);

	   k1 = k2+1;
	   for(i=k1;i<len-10;i++){
		   if(vHist[i] > thB && vHist[i+1] > thB && vHist[i+2] > thB) {k1 = i;break;}
	   }
	   if(k2==k1) break;
	   if(k1-k2 > maxW) maxW = k1-k2;
	   cc++;
	}
//	printf("--- maxB= %d, maxW= %d; cc= %d\n",maxB, maxW, cc);
//	fflush(stdout);
//	 sprintf(buffLog,"--- maxB= %d, maxW= %d; cc= %d\n",maxB, maxW, cc);
//	 saveLog("", buffLog);

	if(maxB < 50 && maxW < 50 && cc > 20) return 1;
	return -1;

}


//int smearingForTypePA(LPIMGDATA imgInp, LPIMGDATA imgOut, char *mainPath, char *imgName, int pixX, int pixY ){
int barcodeTest3D(LPIMGDATA imgInp,  int pixX, int pixY, int imgIdx ){
	int DENSITY_L;

	int ddSmearingx = pixX;
	int ddSmearingy = pixY;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int m, stopSm;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	int densityTab[imgInp->bi.biHeight];
	int typeInt = -1;
	IMGDATA imgOut_Sm;


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
		printf("yl_calloc failed, ptrImgOut");
		return typeInt;
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

	copyHeadersBMP(ptrImgOut, &imgOut_Sm);


// BARCODE
	stopSm = sizeH/2;
	DENSITY_L = stopSm*ddSmearingy;
	for(idxH=0;idxH<sizeH;idxH++){
		gridH = ddSmearingy;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = ddSmearingx;
			if(idxW == sizeW-1) gridW = lastW;
			if(idxH > stopSm)
			   m = getValueForSmearing(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);
			else
			   m = getValueForSmearing_White(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);

			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
		}
	}

//	 printf("-------smearing\n");
//	fflush(stdout);

//  second smearing
	{
	int ddSmearingx2 = ddSmearingx*2;
	int ddSmearingy2 = ddSmearingy;
	int sizeW2, sizeH2, lastW2, lastH2, stopSm2;


	sizeW2 = imgInp->bi.biWidth/ddSmearingx2;
	lastW2 = imgInp->bi.biWidth - sizeW2*ddSmearingx2;
	if(lastW2 > 0)
		sizeW2++;
	sizeH2 = imgInp->bi.biHeight/ddSmearingy2;
	lastH2 = imgInp->bi.biHeight - sizeH2*ddSmearingy2;
	if(lastH2 > 0)
		sizeH2++;
	stopSm2 = sizeH2/2;
	{
    int mW[sizeW2];
	  for(idxH=0;idxH<sizeH2;idxH++){
		gridH = ddSmearingy2;
		if(idxH == sizeH2-1) gridH = lastH2;
		if(idxH > stopSm2) break;
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;

		    m = getValueForSmearing(ptrBuffOut, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx2, ddSmearingy2, gridW, gridH);
		    mW[idxW] = m;
		}
		for(idxW=0;idxW<sizeW2;idxW++){
			gridW = ddSmearingx2;
			if(idxW == sizeW2-1) gridW = lastW2;
			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx2, idxW*ddSmearingx2 + gridW,
					idxH*ddSmearingy2, idxH*ddSmearingy2+gridH-1, mW[idxW]);

		}
	  }
	}

	}
//	 printf("2-------smearing\n");
//	fflush(stdout);

	 createDesityTabPA1(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);

//	 printf("-------createDesityTabPA1\n");
//	fflush(stdout);

//	 {int ii;
//	 				for(ii=1;ii<=densityTab[0];ii++) {
//	 					sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//	 					saveLog( "", bufLog );
//	 				}
//	 }
//	 saveBMPbuffer (ptrImgOut, imgInp->szIMGName, imgInp->szOutPath);
	 {int x1,x2,y1,y2;
	  int Hist[1000];
	  int  yShift, xShift;
	 GetFirstY_PA(densityTab, &y1);
	 x1 = 10;
	 x2 = (biWidthPad*8)/2+10;
     y2 = y1+300;
	 if(y1 > 0) {
		 y1 -= 10;
			//		  int i;
// top
	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x2/2-10, x2, y1, y2, Hist);
	yShift = getYShiftPA(Hist);
    y1 += yShift;
//printf("------yShift= %d\n",yShift);
//fflush(stdout);
// second test
	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x2/2-10, x2, y1, y2, Hist);
//	printf("1------Hist[0]= %d\n",Hist[0]);
//	fflush(stdout);

	yShift = getYShiftPA(Hist);

//	printf("2------yShift= %d\n",yShift);
//	fflush(stdout);
    y1 += yShift;

// bottom
    y2 = y1+300;
    if(y1 >= imgInp->bi.biHeight) y1 = imgInp->bi.biHeight-10;
    if(y2 >= imgInp->bi.biHeight) y2 = imgInp->bi.biHeight-1;
	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x2/2-10, x2, y1, y2, Hist);
//	printf("1------Hist[0]= %d\n",Hist[0]);
//	fflush(stdout);

	yShift = getY2PA(Hist);
    y2 = y1+yShift+10;

//	printf("------y2= %d\n",y2);
//	fflush(stdout);

//printf("------y1= %d, yShift= %d, y2= %d\n",y1, yShift, y2);
//fflush(stdout);
//      		 for(nn=1;nn<Hist[0];nn++){
//     			    printf("   Hist[ %d ]= %d \n", nn, Hist[nn]);
//    		  }


    histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x2, y1+20, y2, Hist);
	xShift = getX1PA(Hist);
	x1 += xShift;
	if(x1> 50) x1 -= 15;
	xShift = getX2PA(Hist);
	x2 -= xShift;

//printf("------xShift= %d\n",xShift);
//fflush(stdout);

//  		 for(nn=1;nn<Hist[0];nn++){
// 			    printf("   Hist[ %d ]= %d \n", nn, Hist[nn]);
//		  }


//		createImageLogo(imgInp, x1, x2, y1, y2);
	 }
	 {int nn;int cnfLevel[10];
	 int max, iMax;
	 y2 = imgInp->bi.biHeight-1-100;
     x1 = imgInp->bi.biWidth/8;
     x2 = imgInp->bi.biWidth-x1;
	 for(nn=0;nn<8;nn++){
		   y1 = y2-100;
		    histVPtrPix(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x2, y1, y2, Hist);
		    cnfLevel[nn] = getCnfLevelbarcode_2D(Hist);

//	 	 {int ii;
//	 	 if(nn == 4 || nn == 5) {
//			sprintf(bufLog,"---------------nn= %d, y1= %d\n",nn, y1);
//			saveLog( "", bufLog );

//	 	 				for(ii=1;ii<=Hist[0];ii++) {
//	 	 					sprintf(bufLog,"Hist[ %d ]= %d\n",ii, Hist[ii]);
//	 	 					saveLog( "", bufLog );
//	 	 				}
//	 	 }
//	 	 }
	 	 y2 -= 50;
	 }
	 max = 0;iMax = 0;
	 for(nn=0;nn<8;nn++){
		 if(cnfLevel[nn] > max) {max = cnfLevel[nn]; iMax = nn;}
	 }
	 if(iMax < 3){
		 for(nn=iMax+1;nn<8;nn++){
			 if(cnfLevel[nn] == max) {iMax = nn;break;}
		 }
	 }
	 y2 = (imgInp->bi.biHeight-1-100)-50*iMax+50;
	 y1 = y2 - 300;
// top
 	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1+100, x2, y1, y2, Hist);
 	yShift = getYShiftPA(Hist);
// 	printf("1   y1= %d, yShift= %d \n", y1, yShift);
    y1 += yShift;
//    printf("1   y1= %d \n", y1);
//    fflush(stdout);

// second test
   	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1+150, x2/2, y1, y2, Hist);

//   	    	 	 {int ii;
//   	    	 	 				for(ii=1;ii<=Hist[0];ii++) {
//   	    	 	 					sprintf(bufLog,"Hist[ %d ]= %d\n",ii, Hist[ii]);
//   	    	 	 					saveLog( "", bufLog );
//   	    	 	 				}
//   	   	 	 }

   	yShift = getYShiftPA(Hist);
//    printf("2   y1= %d, yShift= %d \n", y1, yShift);
//    fflush(stdout);

    y1 += yShift;
//    printf("2   y1= %d \n", y1);

//    y1 -= 20;

//    printf("bottom   y1= %d, y2= %d, imgInp->bi.biHeight= %d \n", y1, y2, imgInp->bi.biHeight);
//    printf("bottom   x1+100= %d, x2= %d, imgInp->bi.biWidth= %d \n", x1+100, x2, imgInp->bi.biWidth);
//    fflush(stdout);

// bottom
   histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1+100, x2, y1, y2, Hist);

//   printf("   Hist[0]= %d \n", Hist[0]);
//   fflush(stdout);

   yShift = getY2PA(Hist);
   y2 = y1+yShift+10;

//   printf("first barcode   yShift= %d, y2= %d \n", yShift, y2);
//   fflush(stdout);

// first barcode
   histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x1+300, y1+40, y2-40, Hist);
	xShift = getX1PA(Hist);
	x1 += xShift;
	x2 = x1+300;

//    printf("   x1= %d, x2= %d \n", x1, x2);
//    fflush(stdout);

    histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x2, y1+40, y2-40, Hist);
    xShift = getX2PA_b(Hist);
	x2 = x1+xShift+10;

//    printf("---   xShift= %d, x2= %d \n", xShift, x2);
//    fflush(stdout);

 	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2, Hist);
 	yShift = getY1PA_b(Hist);
    y1 += yShift;

    //      printf("   y1= %d, yShift= %d, y2= %d \n", y1, yShift, y2);
          histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2, Hist);
          yShift = getY2PA_b(Hist);
//          printf("   y2= %d, yShift= %d \n", y2, yShift);
          y2 -= yShift;
//          printf("   y2= %d\n", y2);

//   printf("   y1= %d \n", y1);

//    	 	 {int ii;
//    	 	 				for(ii=1;ii<=Hist[0];ii++) {
//    	 	 					sprintf(bufLog,"Hist[ %d ]= %d\n",ii, Hist[ii]);
//    	 	 					saveLog( "", bufLog );
//    	 	 				}
//   	 	 }
//	typeInt = getValuesFromBarCode_3D(imgInp, x1*8, x2*8, imgInp->bi.biHeight-1-y1, imgInp->bi.biHeight-1-y2);
//             printf("   x1= %d, x2= %d, y1= %d, y2= %d \n", x1, x2,y1, y2);
//             fflush(stdout);
//    typeInt = getValuesFromBarCode_3D(imgInp, x1, x2, y1, y2, imgIdx);
	barcode_DataMatrix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2);

	printf("   imgIdx= %d, barcode= %s \n", imgIdx, outputDMCode);
    fflush(stdout);
	typeInt = -1;




//	typeInt =  getTypeFromBarcode_PA(outputDMCode, numDMcodeChars);
//	printf("  typeInt= %d \n", typeInt);




//	 	 	 {int ii;
//	 	 		for(ii=0;ii<10;ii++) {
//	 		  	  sprintf(bufLog,"---------------ii= %d, cnfLevel[ii]= %d,\n",ii, cnfLevel[ii] );
//	 			  saveLog( "", bufLog );
//	 	 		}
//	 	 	 }

	 }

 }
	yl_free(ptrImgOut);
	return typeInt;
}


int getValuesFromBarCode_3D(LPIMGDATA img, int x1, int x2, int y1, int y2, int imgIdx){

	int err;
	LPBYTE ptr;
	LPDWORD      lpImage;
	DWORD        offBits;
	HLPBYTE hpFieldH;
	WORD width;
	WORD height;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int angleFlag = 0;
    double angle;
    int  biSizeImage, biWidthPad;
	int typeInt = -1;


	ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
	if (ptr == NULL) {
		return typeInt;     // out of memory
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
    { //decode barcode
//	int vHist[img->bi.biWidth];
//	int i, ret;// , rsd;
//	MASKDEF barMask[20];
//	char strResult[64];
//	int  confR[64];
//	char cand2[64];
//	char typeChar[64];
//	int histTestFlg= 0;

//	strcpy(typeChar, "???");

//	WORD byteWidthHist = ((img->bi.biWidth + 31)/32)*4;
    fieldAngleFromBarCode = 0;

	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

//         barcode_DataMatrix(img->pixPtr, biWidthPad, img->bi.biHeight,  x1, x2, y1, y2);
//         printf("   imgIdx= %d, barcode= %s \n", imgIdx, outputDMCode);
         typeInt = -1;
    // ----------------------------- debugging

 	err =
 			extractField(img->pixPtr, img->bi.biWidth, img->bi.biHeight,
 					x1, y1,
 					x2, y1,
 					x1, y2,
 					&hpFieldH,
 					&width,
 					&height,
 					8,
 					"name",
 					angleFlag,
 					&angle
 			);
 	if(err < 0) return typeInt;



 	bf = (LPBITMAPFILEHEADER) hpFieldH.ptr;
 	bf->bfType[0] = 0x42;
 	bf->bfType[1] = 0x4d;
 	set4Bytes(bf->bfSize, offBits + (((width + 31) /32) * 4) * height);
 	bf->bfReserved1[0] = 0;
 	bf->bfReserved1[1] = 0;
 	set4Bytes(bf->bfOffBits, offBits);


 	// fill in BITMAPINFOHEADER
 	biWidthPad = ((width+31)/32)*4;

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

 	{
 		char imgName[100];
 	   setNewImageName(imgName, img->szIMGName, imgIdx);
//  printf("--- imgName= %s \n",imgName);
//  fflush(stdout);
	   saveBMPbuffer (hpFieldH.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
 	}
 	// ----------------------------- debugging



    }

	yl_free(hpFieldH.ptr);
	return typeInt;
}

//------------------------------------countSpecks
#define  MAXARR_IMG         300
int arrTrace_IMG[MAXARR_IMG+2][2];
int indxTr_IMG;
BYTE arrExc_IMG[256];
//int invTet[200];

void arrExcF_IMG(int nB)
{
   int i;
     for(i=1;i<255;i++) {
        arrExc_IMG[i]=1;
        }
     arrExc_IMG[0]  =0;
     arrExc_IMG[255]=0;
     switch(nB) {
        case 1:arrExc_IMG[3]  =0;
               arrExc_IMG[6]  =0;
               arrExc_IMG[7]  =0;
               arrExc_IMG[12] =0;
               arrExc_IMG[14] =0;
               arrExc_IMG[15] =0;
               arrExc_IMG[24] =0;
               arrExc_IMG[27] =0;
               arrExc_IMG[28] =0;
               arrExc_IMG[30] =0;
               arrExc_IMG[31] =0;
               arrExc_IMG[48] =0;
               arrExc_IMG[51] =0;
               arrExc_IMG[54] =0;
               arrExc_IMG[55] =0;
               arrExc_IMG[56] =0;
               arrExc_IMG[59] =0;
               arrExc_IMG[60] =0;
               arrExc_IMG[62] =0;
               arrExc_IMG[63] =0;
               arrExc_IMG[96] =0;
               arrExc_IMG[99] =0;
               arrExc_IMG[102]=0;
               arrExc_IMG[103]=0;
               arrExc_IMG[108]=0;
               arrExc_IMG[110]=0;
               arrExc_IMG[111]=0;
               arrExc_IMG[112]=0;
               arrExc_IMG[115]=0;
               arrExc_IMG[118]=0;
               arrExc_IMG[119]=0;
               arrExc_IMG[120]=0;
               arrExc_IMG[124]=0;
               arrExc_IMG[126]=0;
               arrExc_IMG[127]=0;
               arrExc_IMG[192]=0;
               arrExc_IMG[195]=0;
               arrExc_IMG[198]=0;
               arrExc_IMG[199]=0;
               arrExc_IMG[204]=0;
               arrExc_IMG[206]=0;
               arrExc_IMG[207]=0;
               arrExc_IMG[216]=0;
               arrExc_IMG[219]=0;
               arrExc_IMG[220]=0;
               arrExc_IMG[222]=0;
               arrExc_IMG[223]=0;
               arrExc_IMG[224]=0;
               arrExc_IMG[227]=0;
               arrExc_IMG[230]=0;
               arrExc_IMG[231]=0;
               arrExc_IMG[236]=0;
               arrExc_IMG[238]=0;
               arrExc_IMG[239]=0;
               arrExc_IMG[240]=0;
               arrExc_IMG[243]=0;
               arrExc_IMG[246]=0;
               arrExc_IMG[247]=0;
               arrExc_IMG[248]=0;
               arrExc_IMG[251]=0;
               arrExc_IMG[252]=0;
               arrExc_IMG[254]=0;
               break;
        case 2:
               arrExc_IMG[7]  =0;
               arrExc_IMG[14] =0;
               arrExc_IMG[15] =0;
               arrExc_IMG[28] =0;
               arrExc_IMG[30] =0;
               arrExc_IMG[31] =0;
               arrExc_IMG[56] =0;
               arrExc_IMG[60] =0;
               arrExc_IMG[62] =0;
               arrExc_IMG[63] =0;
               arrExc_IMG[112]=0;
               arrExc_IMG[119]=0;
               arrExc_IMG[120]=0;
               arrExc_IMG[124]=0;
               arrExc_IMG[126]=0;
               arrExc_IMG[127]=0;
               arrExc_IMG[224]=0;
               arrExc_IMG[231]=0;
               arrExc_IMG[238]=0;
               arrExc_IMG[239]=0;
               arrExc_IMG[240]=0;
               arrExc_IMG[247]=0;
               arrExc_IMG[248]=0;
               arrExc_IMG[252]=0;
               arrExc_IMG[254]=0;
               break;
        case 3:
               arrExc_IMG[15] =0;
               arrExc_IMG[30] =0;
               arrExc_IMG[31] =0;
               arrExc_IMG[60] =0;
               arrExc_IMG[62] =0;
               arrExc_IMG[63] =0;
               arrExc_IMG[120]=0;
               arrExc_IMG[124]=0;
               arrExc_IMG[126]=0;
               arrExc_IMG[127]=0;
               arrExc_IMG[240]=0;
               arrExc_IMG[248]=0;
               arrExc_IMG[252]=0;
               arrExc_IMG[254]=0;
               break;
        case 4:
               arrExc_IMG[31] =0;
               arrExc_IMG[62] =0;
               arrExc_IMG[63] =0;
               arrExc_IMG[124]=0;
               arrExc_IMG[126]=0;
               arrExc_IMG[127]=0;
               arrExc_IMG[248]=0;
               arrExc_IMG[252]=0;
               arrExc_IMG[254]=0;
               break;
        case 5:
               arrExc_IMG[63] =0;
               arrExc_IMG[126]=0;
               arrExc_IMG[127]=0;
               arrExc_IMG[252]=0;
               arrExc_IMG[254]=0;
               break;
        case 6:
               arrExc_IMG[127]=0;
               arrExc_IMG[254]=0;
               break;
        }
}/* arrExc_IMGF() */



void cleanSpeckImg(
   SPECKCOUNT  *lpData,
   int        wXBegin,
   int        wYBegin
)
{
  int x, y, wImg, mask, i;
  LONG lY;
  BYTE  *hptr;
    wImg  =(((lpData->iLineWidth)+31)/32)*4;
                for(i=0;i<indxTr_IMG;i++) {
                     x=arrTrace_IMG[i][0];
                     y=arrTrace_IMG[i][1];
                     y=(lpData->iImgHeight)-1-y;
                     lY=(LONG)y*(LONG)wImg;
                     hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                     mask=LEFT_BIT;
                     mask >>=(x%8);
                     hptr[0]=hptr[0]|mask;
                     }

}

int sizeSpotImg(SPECKCOUNT *lpData, int wXBegin, int wYBegin){
  int x, y, xx, yy, wImg, flgB, yUpA = 0, yUpP = 0, yBtA = 0, yBtP = 0, y1 = 0, y2 = 0;
  int sizeX, cc, xmin, xmax, i;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
             y=wYBegin;
             x=wXBegin;
             yy=y-1;
             if(yy>=0) {
                if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),x,yy)) return(0);
                xx=x-1;
                if(xx>=0) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) return(0);
                        }
                xx=x+1;
                if(xx<(lpData->iLineWidth)) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) return(0);
                        }
                }
             indxTr_IMG=0;
             sizeX=0;
             xx=x;
             yUpP=yy;
             yBtP=yy;
             while((xx>=0)) {
               yy=y;
               cc=0;
               flgB=0;
               while((yy<(lpData->iImgHeight))) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        if(flgB==0) {
                            if((yy<yUpP-1)||(yy>yBtP+1)) break;
                            flgB=1;
                            yUpA=yy;
                            }
                        cc++;
                        arrTrace_IMG[indxTr_IMG][0]=xx;
                        arrTrace_IMG[indxTr_IMG][1]=yy;
                        indxTr_IMG++;
                        if(indxTr_IMG>=MAXARR_IMG) return(0);
                        }
                  else {
                        if(flgB)  {
                            yBtA=yy-1;
                            break;
                            }
                        }
                  if(cc>(lpData->iSpeckSizeV)) return(0);
                  yy++;
                  }
                if(cc>0) {
                        yUpP=yUpA;
                        yBtP=yBtA;
                        if(xx==x) {
                                y1=yUpA;
                                y2=yBtA;
                                }
                        sizeX++;
                        }
                else {
                        break;
                        }
                if(sizeX>(lpData->iSpeckSizeH)) return(0);
                xx--;
                if(y-1>=0) {
                  if(xx-1>=0) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx-1,y-1)) return(0);
                        }
                  }

                }
             xx=x+1;
             yUpP=y1;
             yBtP=y2;
             while(xx<(lpData->iLineWidth)) {
               yy=y;
               cc=0;
               flgB=0;
               while(yy<(lpData->iImgHeight)) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        if(flgB==0) {
                            if((yy<yUpP-1)||(yy>yBtP+1)) break;
                            flgB=1;
                            yUpA=yy;
                            }
                        cc++;
                        arrTrace_IMG[indxTr_IMG][0]=xx;
                        arrTrace_IMG[indxTr_IMG][1]=yy;
                        indxTr_IMG++;
                        if(indxTr_IMG>=MAXARR_IMG) return(0);
                        }
                  else {
                        if(flgB)  {
                            yBtA=yy-1;
                            break;
                            }
                        }
                  if(cc>(lpData->iSpeckSizeV)) return(0);
                  yy++;
                  }
                if(cc>0) {
                        yUpP=yUpA;
                        yBtP=yBtA;
                        sizeX++;
                        }
                else {
                        break;
                        }
                if(sizeX>(lpData->iSpeckSizeH)) return(0);
                xx++;
                if(y-1>=0) {
                  if(xx+1<(lpData->iLineWidth)) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx+1,y-1)) return(0);
                        }
                  }

                }
        xmax=x;
        xmin=x;
        for(i=0;i<indxTr_IMG;i++) {
                if(arrTrace_IMG[i][0]<xmin) xmin=arrTrace_IMG[i][0];
                if(arrTrace_IMG[i][0]>xmax) xmax=arrTrace_IMG[i][0];
                }


         yy=y;
         while((yy<=y+(lpData->iSpeckSizeV)) && (yy<(lpData->iImgHeight))) {
               xx=x;
               cc=0;
               flgB=0;
               y1=0;
               while(xx>0) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        flgB=1;
                        cc++;
                        if(cc>(lpData->iSpeckSizeH)) return(0);
                        }
                  else {
                        if(xx<xmin) break;
                        if(flgB==1) break;
                        }
                  xx--;
                  }
               if(cc==0) y1=1;
               xx=x;
               cc=0;
               flgB=0;
               while(xx<(lpData->iLineWidth)) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        flgB=1;
                        cc++;
                        if(cc>(lpData->iSpeckSizeH)) return(0);
                        }
                  else {
                        if(xx>xmax) break;
                        if(flgB==1) break;
                        }
                  xx++;
                  }
               if((y1==1)&&(cc==0)) break;
                yy++;
                }
           return(1);
} /* sizeSpot() */
int cleanSpecksImg( SPECKCOUNT  *lpData  ){

  int x, y, x1, wImg, i, ch, ind, mask, ret, yy;
  LONG lY;
  BYTE  *hptr;
  int count = 0;


    if( (lpData->iXBegin)+(lpData->iWidth)>=(lpData->iLineWidth) ) (lpData->iWidth)=(lpData->iLineWidth)-(lpData->iXBegin)-1;
//    if( ((lpData->iImgHeight)<MINSIZE) || ((lpData->iLineWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( (lpData->iYBegin)+(lpData->iHeight)>=(lpData->iImgHeight) ) (lpData->iHeight)=(lpData->iImgHeight)-(lpData->iYBegin)-1;
//    if( ((lpData->iHeight)<MINSIZE) || ((lpData->iWidth)<MINSIZE) ) return(SIZE_ERR1);
//    if( ((lpData->iHeight)>MAXSIZE) || ((lpData->iWidth)>MAXSIZE) ) return(SIZE_ERR2);
    if( ((lpData->iXBegin)>(lpData->iLineWidth)) || ((lpData->iYBegin)>(lpData->iImgHeight)) ) return(-1);

    if((lpData->iSpeckSizeH)<0) (lpData->iSpeckSizeH)=0;
    if((lpData->iSpeckSizeV)<0) (lpData->iSpeckSizeV)=0;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
              arrExcF_IMG((lpData->iSpeckSizeH));
              x1=(lpData->iXBegin)/8;
              x =x1*8;
              y =(lpData->iYBegin);

              while(y<(lpData->iYBegin)+(lpData->iHeight))  {
                x =x1*8;
                yy=y;

                yy=(lpData->iImgHeight)-1-y;
                lY=(LONG)yy*(LONG)wImg;
                hptr=(lpData->lpPointer)+lY+(LONG)x1;

                while(x<(lpData->iXBegin)+(lpData->iWidth)) {
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        if(arrExc_IMG[ind]) {
                            for(i=0;i<8;i++) {
                              mask=LEFT_BIT;
                              mask >>=i;
                              ch=(~hptr[0])&mask;
                              if(ch!=0) {
                                if((ret=sizeSpotImg(lpData,x+i,y))) {
                                        cleanSpeckImg(lpData,x+i,y);
                                	    count++;
                                        }
                                }
                              }

                            }
                        hptr++;
                        x+=8;
                        }
                y++;
                }

        return(count);

} /* of cleanSpecks() */


int getOCRforLine(LPIMGDATA imgOut, LPTICKETDATA ticketData, int x1, int x2, int y1, int y2, double angle, int idx, char *fieldName){
   int  cnf;
   BYTE * pDataBlock;
   int iAlphaIndx = 23;


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

	cnf = regognizeOCR_BC(x1, x2, y1, y2, ticketData,
			&ticketData->strings, ticketData->flConf, ticketData->flQuad, imgOut, angle, iAlphaIndx, idx, fieldName);

//	trimSpOut(ticketData);
	TrimField(ticketData);

	return cnf;
}
void printLineOut(LPTICKETDATA ticketData, int iStart, int iStop){
	char *result = ticketData->strings.lpResult;
	int i;

	for(i=iStart;i<iStop;i++){
		printf("result[ %d ]= %c,  conf= %d\n", i, result[i], ticketData->flConf[i]);
	}


}
void testOneLineDigitRec(LPTICKETDATA ticketData, int iStart, int iStop, int *lConf, int *hConf){
	char *result = ticketData->strings.lpResult;
	int i;
	int thCnf = 34;

	*lConf = 0; *hConf = 0;
	for(i=iStart;i<iStop;i++){
//		printf("result[ %d ]= %c,  conf= %d \n", i, result[i],  ticketData->flConf[i]);
//		if((result[i] > '1' && result[i] <= '9') || result[i] == '0') {
		if(result[i] > 32 && result[i] != '1' && result[i] != 'I' && result[i] != '-') {
			if(ticketData->flConf[i] < thCnf) (*lConf)++;
			else (*hConf)++;
		} else{
			if(result[i] == '-') (*lConf)++;
		}
	}
}
int getLineYSize(LPTICKETDATA ticketData, int iStart, int iStop){
	char *result = ticketData->strings.lpResult;
	int i;
    int maxY, minY, yy;
    maxY = 0; minY = 10000;
	for(i=iStart;i<iStop;i++){
		if(result[i] > 32) {
		   if(minY > ticketData->flQuad[i].p1Y) minY = ticketData->flQuad[i].p1Y;
		   if(maxY < ticketData->flQuad[i].p4Y) maxY = ticketData->flQuad[i].p4Y;
		}
	}
	yy = maxY-minY;
    return yy;
}
int TestForTrash(LPTICKETDATA ticketData, int iStart, int iStop){
	char *result = ticketData->strings.lpResult;
	int i;
    int ccTr, cc;

    ccTr = 0; cc = 0;
	for(i=iStart;i<iStop;i++){
		if(result[i] > 32) {
			cc++;
		   if(result[i] == 'I' || result[i] == '-' || result[i] == '1') ccTr++;
		}
	}
	if(cc > 0){
		if( ((float)ccTr/(float)cc)*100.0 > 90.0 ) return 1;
	}
    return 0;

}

int test_DigitsRecogR(LPTICKETDATA ticketData, int sumcc_V_H){
	int ret = 0;
	char *result = ticketData->strings.lpResult;
	int i;
	int hConf, lConf;
//	int thCnf = 20;
	 int iStart, iStop;
	 int numLines = 0, count, realLines;
	 int  yy, ccLinesHCnf, trashLine;
	 float ratioHcnfL;

	hConf = 0; lConf = 0;
// get number of lines
		 i = 0;
		 while(1){
			if(getLine(result, i, &iStart, &iStop) < 0) break;
			numLines++;
			i++;
		 }
		 i = 0;count = 0;realLines = 0;ccLinesHCnf = 0;
		 while(i<numLines){
			 if(getLine(result, i, &iStart, &iStop) < 0) break;
//  printf("line: %d iStop-iStart= %d\n", i, iStop -iStart);
			 if(iStop -iStart > 5) {
			    yy = getLineYSize(ticketData, iStart, iStop);
			    trashLine = TestForTrash(ticketData, iStart, iStop);
// printf("line: %d  yy= %d, trashLine= %d\n", i,  yy, trashLine);
			    if(yy > 10 && yy < 150 && trashLine == 0) {
				    realLines++;
			        testOneLineDigitRec(ticketData, iStart, iStop, &lConf, &hConf);
//   printf("line: %d lConf= %d, hConf= %d\n\n", i, lConf, hConf);
				     if(hConf> 5) ccLinesHCnf++;
				    if(hConf+lConf > 4 && hConf> lConf) {
					    count++;
//					 printLineOut(ticketData, iStart, iStop);
				    }
			    }
			 }
			 i++;
		 }
		 ratioHcnfL = 0;
		 if(realLines > 0) {
		    ratioHcnfL = (float)ccLinesHCnf/(float)realLines;
		    if(realLines < 12 && count > 0) ret = 1;
		    if(realLines >= 12 && count > 1) ret = 1;
		    if(ratioHcnfL > 0.35) ret = 1;
		    if(sumcc_V_H > 400){
		    	if(ratioHcnfL > 0.2) ret = 1;
		    }
		 }

//	 printf("realLines= %d, count= %d, ccLinesHCnf= %d, ratioHcnfL= %e, ret= %d\n", realLines, count, ccLinesHCnf, ratioHcnfL, ret);
	return ret;
}
int recognizeAllLines(LPIMGDATA img, int x1, int x2, int y1, int y2, LPTICKETDATA ticketData, int idx, int sumcc_V_H){
  int cnfLineCount = 0;
  int ret;
  double angle = 0;

        trapezoidVer = 0;

	   ret = getOCRforLine(img, ticketData, x1, x2, y1, y2, angle, idx, "segmMode0");
	   cnfLineCount =test_DigitsRecogR(ticketData, sumcc_V_H);
//		printf("\n--------------------segmMode0; cnfLineCount= %d \n\n", cnfLineCount);
//		fflush(stdout);

	   if(cnfLineCount == 0){

	    ret = getOCRforLine(img, ticketData, x1, x2, y1, y2, angle, idx+20, "segmMode1");
	    cnfLineCount =test_DigitsRecogR(ticketData, sumcc_V_H);

//		printf("\n--------------------segmMode1; cnfLineCount= %d \n\n", cnfLineCount);
//		fflush(stdout);
	   }






  return cnfLineCount;
}


int getValue_RGBA(LPBYTE imgPtr, int biWidth, int biHeight, int xn, int yn){

	long nY;
	long nX;
	int sh = 4;
	int r, g, b, v;
	LPBYTE ptr;

		nY=yn*biWidth*sh;
		nX = xn*sh;
		ptr = imgPtr+nY+nX;
			g =*(ptr+1);
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
			r =*ptr;
			b =*(ptr+2);
#else
			b =*ptr;
			r =*(ptr+2);
#endif
			v = computeLuminosity(r, g, b);
	return v;
}

void getMeanDiffValue(LPBYTE imgPtr, int biWidth, int biHeight, int *hMean, int *vMean, int margin){
	int xn, yn;
	int h, v, cc;
    h = 0; v = 0;cc = 0;
	for(yn=margin;yn<biHeight-margin;yn++){
		for(xn=margin;yn<biWidth-margin;yn++){
			h += abs(getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn+1) - getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn-1));
			v += abs(getValue_RGBA(imgPtr, biWidth, biHeight, xn+1, yn) - getValue_RGBA(imgPtr, biWidth, biHeight, xn-1, yn));
			cc++;
		}
	}
	if(cc>0){
		h /= cc;
		v /= cc;
	}
	*hMean = h;
	*vMean = v;
}
void getRdgeAndBlur(LPBYTE imgPtr, int biWidth, int biHeight, int hMean, int vMean,  int *EdgeH, int *EdgeV, int *Blur, int margin){
	int xn, yn;
	int vx_L, v, vx_R, vx_L2, vx_R2;
	int vy_T,  vy_B, vy_T2,  vy_B2;
	int Eh, Ev, B;
	int Dh, Dv;
	int Ch, ChT, ChB;
	int Cv, CvL, CvR;
	int Ah, Av;
	float Bh, Bv, Th = 0.02;


	Eh = 0;Ev = 0;B = 0;
	for(yn=margin;yn<biHeight-margin;yn++){
		for(xn=margin;yn<biWidth-margin;yn++){
			vx_L2 = getValue_RGBA(imgPtr, biWidth, biHeight, xn-2, yn);
			vx_L = getValue_RGBA(imgPtr, biWidth, biHeight, xn-1, yn);
			vx_R = getValue_RGBA(imgPtr, biWidth, biHeight, xn+1, yn);
			vx_R2 = getValue_RGBA(imgPtr, biWidth, biHeight, xn+2, yn);
			vy_T2 = getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn-2);
			vy_T = getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn-1);
			vy_B = getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn+1);
			vy_B2 = getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn+2);
			v = getValue_RGBA(imgPtr, biWidth, biHeight, xn, yn);

// Eh
			Dh = abs(vy_T-vy_B);Ch = 0;
			if(Dh > hMean) Ch = Dh;
			Dh = abs(vy_T2-v);ChT=0;
			if(Dh > hMean) ChT = Dh;
			Dh = abs(v- vy_B2);ChB=0;
			if(Dh > hMean) ChB = Dh;
			if(Ch > ChT && Ch > ChB) Eh++;

// Ev
			Dv = abs(vx_L-vx_R);Cv = 0;
			if(Dv > vMean) Cv = Dv;
			Dv = abs(vx_L2-v);CvL=0;
			if(Dv > vMean) CvL = Dv;
			Dv = abs(v- vx_R2);CvR=0;
			if(Dv > vMean) CvR = Dv;
			if(Cv > CvL && Cv > CvR) Ev++;

// Bh
	      Ah = (vy_T+vy_B)/2;Bh = 0;
	      if(Ah != 0) Bh = (float)abs(v-Ah)/(float)Ah;
	      Av = (vx_L+vx_R)/2;Bv = 0;
	      if(Av != 0) Bv = (float)abs(v-Av)/(float)Av;

//    printf("---------------------Bh= %e; Bv= %e\n", Bh, Bv);
	      if(Bh < Bv) Bh = Bv;
	      if(Bh>Th) B++;
		}
	}

	*EdgeH = Eh;*EdgeV = Ev; *Blur = B;
}
void calculateImageBlurness(LPBYTE imgPtr, int biWidth, int biHeight){

	int hMean, vMean;
	int EdgeH, EdgeV, Blur, sum;
	float ratio;
	getMeanDiffValue(imgPtr, biWidth, biHeight, &hMean, &vMean, 64);

	getRdgeAndBlur(imgPtr, biWidth, biHeight, hMean, vMean, &EdgeH, &EdgeV, &Blur, 64);
//	sum = EdgeH+ EdgeV;ratio = 0;
	sum =  EdgeV;ratio = 0;
	if(sum > 0)
	   ratio = (float)Blur/(float)sum;
	printf("---------------------EdgeH= %d; EdgeV= %d, Blur= %d, ratio= %e\n", EdgeH, EdgeV, Blur, ratio);
}

// ------------------------------------compression
// ------------------------------------
/*

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
*/
void setColorArray(LPBYTE ptrInp, int x, int color){
	    BYTE mask;
	    LPBYTE ptr;

	         ptr = ptrInp+x/8;
	         mask=LEFT_BIT;
	         mask >>=(x%8);
	         if(color!=0){
	        	 *ptr = *ptr | mask;
	         }
	         else {
	        	 *ptr = *ptr & (~mask);
	         }
}

int compressPixels(LPBYTE ptrO, LPBYTE ptrI, int imgWidth, int imgHeight,
		   LPBYTE ptrOutColors, int *colorSize){
	   int biWidthPad = ((imgWidth+31)/32)*4;
	   int x, y, num, size, sizeC;
	   BYTE color, colorC;
//       char buff[500];

	    sizeC = 0;
	    color = getPixelCompress(ptrI, 0, 0, biWidthPad, imgHeight);

//	    setColorArray(ptrOutColors, sizeC, color);sizeC++;

	    size = 0;
	    num = 0;
	    colorC = color;
	    for(y=0;y<imgHeight;y++){
         for(x=0;x<imgWidth;x++){
           	color = getPixelCompress(ptrI, x, y, biWidthPad, imgHeight);
         	if(color != colorC || num >= 255){

//     sprintf(buff,"-------color= %d, colorC= %d, num= %d\n", color, colorC, num);
//     saveLog("", buff);

         		ptrO[size] = num;size++;
         		setColorArray(ptrOutColors, sizeC, colorC);sizeC++;
         	    num = 1;
         	    colorC = color;
         	}else{
         		num++;
         	}
         }
     }
	    if(num > 0) {
 		ptrO[size] = num-1;size++;
 	    num = 1;
	    }
	    *colorSize = sizeC;
     return size;
}
void copyColorsToPixels(LPBYTE ptrT, LPBYTE ptrOutColors, int colorSize){
	   int i, x;
	   x = colorSize/8+1;
	   for(i=0;i<x;i++){
		   ptrT[i] = ptrOutColors[i];

	   }
}

LPBYTE compressImage(LPIMGDATA img){
   LPBYTE ptrOut = NULL;
   LPBYTE ptrOutColors = NULL;
   LPBYTE ptrT = NULL;
   int bufforSize, bufforSizeC, size;
   int offBits = 28;
   int  colorSize;

//      char buff[500];

   bufforSize = img->bi.biWidth*img->bi.biHeight+offBits;
   bufforSizeC = (img->bi.biWidth/8)*img->bi.biHeight+offBits;

   ptrOut = calloc(bufforSize, sizeof(BYTE));
   if(ptrOut == NULL) {
   	    sprintf(img->error,"compressImage ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
   	    return ptrOut;
   }
   ptrOutColors = calloc(bufforSizeC, sizeof(BYTE));
   if(ptrOutColors == NULL) {
   	    sprintf(img->error,"compressImage ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
   	    return ptrOut;
   }

// pixels
   ptrT = ptrOut+offBits;
   size = compressPixels(ptrT, img->pixPtr, img->bi.biWidth, img->bi.biHeight, ptrOutColors, &colorSize);
// header
   ptrT = ptrOut;
   set4Bytes(ptrT, size+offBits);ptrT += 4;
   set4Bytes(ptrT, colorSize);ptrT += 4;
   *ptrT = offBits;ptrT++;
   set4Bytes(ptrT, img->bi.biWidth);ptrT += 4;
   set4Bytes(ptrT, img->bi.biHeight);ptrT += 4;
   set4Bytes(ptrT, img->bi.biXPelsPerMeter);ptrT += 4;
   set4Bytes(ptrT, img->bi.biYPelsPerMeter);ptrT += 4;
   ptrT = ptrOut+offBits+size;


   copyColorsToPixels(ptrT, ptrOutColors, colorSize);
   if(ptrOutColors != NULL) free(ptrOutColors);

//      *ptrT = firstColor;

/*
   printf("compress-------sizeInp= %d, offBitsInp= %d\n", size+offBits, offBits);
   printf("-------imgWidth= %d, imgHeight= %d\n", img->bi.biWidth, img->bi.biHeight);
   printf("-------xDpi= %d, yDpi= %d\n", img->bi.biXPelsPerMeter, img->bi.biYPelsPerMeter);
   printf("-------firstColor= %d\n", colorSize);

   sprintf(buff,"compress-------sizeInp= %d, offBitsInp= %d\n", size+offBits, offBits);
   saveLog("", buff);
   sprintf(buff,"-------imgWidth= %d, imgHeight= %d\n", img->bi.biWidth, img->bi.biHeight);
   saveLog("", buff);
   sprintf(buff,"-------xDpi= %d, yDpi= %d\n", img->bi.biXPelsPerMeter, img->bi.biYPelsPerMeter);
   saveLog("", buff);
   sprintf(buff,"-------colorSize= %d\n", colorSize);
   saveLog("", buff);

printf("-------size= %d\n", size);
sprintf(buff,"-------size= %d\n", size);
saveLog("", buff);
*/
   return ptrOut;
}
void getNextXY(int imgWidth, int imgHeight, int *x, int *y){
	  int xx = *x;
	  int yy = *y;

	   xx++;
	   if(xx >= imgWidth) {xx = 0;yy++;}
	   *x = xx;*y = yy;
}
void setOutPixels(LPBYTE ptrOut, int imgWidth, int imgHeight, int num, int color, int *x, int *y){
		 int i;
		 int wPad;
		 wPad = ((imgWidth+31)/32)*4;
		 for(i=0;i<num;i++){
			 getNextXY(imgWidth, imgHeight, x, y);
			 setPixelDecompress(ptrOut, wPad, imgHeight, *x, *y, color);
		 }

	 }
int getColorC(LPBYTE ptrColor, int idxC){
	    BYTE mask;
	    LPBYTE ptr;
	    BYTE b;

            ptr = ptrColor+idxC/8;
	         mask=LEFT_BIT;
	         mask >>=(idxC%8);

	         b=(~(*ptr))&mask;
	         if(b!=0) {
	               return(0);
	               }
	         else {
	               return(1);
	               }
}
int deCompressPixels(LPBYTE ptrOut, LPBYTE ptrInp, int imgWidth, int imgHeight, int sizeInp, LPBYTE ptrColor, int colorSize){
      int size = 0;
      int x, y, idxC;
      int i, num, color;
//         char buff[500];

      x = 0, y = 0;
      idxC = 0;
      for(i=0;i<sizeInp;i++){
     	 color = getColorC(ptrColor, idxC);
     	 num = ptrInp[i];

//      sprintf(buff,"i= %d, num = %d, x= %d, y=%d, color= %d\n", i, num , x, y, color);
//       saveLog("", buff);

     	 setOutPixels(ptrOut, imgWidth, imgHeight, num, color, &x, &y);
     	 idxC++;
      }

	     return size;
}


int deCompressImage(LPBYTE ptrC, LPIMGDATA img){
   LPBYTE ptrOut = NULL;
   LPBYTE ptrT = NULL;
   LPBYTE ptrColor = NULL;
   int bufforSize, sizeInp, sizeOut;
   int imgWidth, imgHeight, biWidthPad;
   int xDpi, yDpi;
   int offBitsInp, colorSize;
//      BYTE firstColor;
   LPRGBQUAD          qd;

   LPBITMAPFILEHEADER bf;
   LPBITMAPINFOHEADER bi;
   int offBits = sizeof (BITMAPFILEHEADER) +
             sizeof (BITMAPINFOHEADER)
            + sizeof (RGBQUAD) * 2;

//      char buff[500];


   ptrT = ptrC;
   sizeInp = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   colorSize = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   offBitsInp = *ptrT; ptrT++;
   imgWidth = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   imgHeight = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   xDpi = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   yDpi = getLeftBytes((int*)ptrT, 4);ptrT += 4;
   ptrColor = ptrC+sizeInp;


/*
  printf("decompress-------sizeInp= %d, offBitsInp= %d\n", sizeInp, offBitsInp);
  printf("-------imgWidth= %d, imgHeight= %d\n", imgWidth, imgHeight);
  printf("-------xDpi= %d, yDpi= %d\n", xDpi, yDpi);
  printf("-------colorSize= %d\n", colorSize);

  sprintf(buff,"decompress-------sizeInp= %d, offBitsInp= %d\n", sizeInp, offBitsInp);
  saveLog("", buff);
  sprintf(buff,"-------imgWidth= %d, imgHeight= %d\n", imgWidth, imgHeight);
  saveLog("", buff);
  sprintf(buff,"-------xDpi= %d, yDpi= %d\n", xDpi, yDpi);
  saveLog("", buff);
  sprintf(buff,"-------colorSize= %d\n", colorSize);
  saveLog("", buff);
*/


   biWidthPad = ((imgWidth+31)/32)*4;
   bufforSize = biWidthPad*imgHeight+offBits;

   ptrOut = calloc(bufforSize, sizeof(BYTE));
   if(ptrOut == NULL) {
    	    sprintf(img->error,"deCompressImage ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
    	    return -1;
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
	  set4Bytes(bi->biXPelsPerMeter, xDpi);
	  set4Bytes(bi->biYPelsPerMeter, yDpi);
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
	  img->bi.biXPelsPerMeter = xDpi;
	  img->bi.biYPelsPerMeter = yDpi;
   img->bi.biClrUsed = 0;
	  img->bi.biClrImportant = 0;
	  img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

   ptrT = ptrOut+offBits;
   sizeOut = deCompressPixels(ptrT, ptrC+offBitsInp, img->bi.biWidth, img->bi.biHeight, sizeInp-offBitsInp, ptrColor, colorSize);


//     printf("-------sizeOut= %d\n", sizeOut);
   return 0;
}

int getCompressedImageSizeOld(LPBYTE ptrInp){
    int sizeInp, colorSize;
    LPBYTE ptrT = NULL;

       ptrT = ptrInp;
	   sizeInp = getLeftBytes((int*)ptrT, 4);ptrT += 4;
	   colorSize = getLeftBytes((int*)ptrT, 4);

	return (sizeInp+colorSize);
}
void getCompressedImageSize(LPBYTE ptrInp, int *cmpsize, int *decmpsize){
    LPBYTE ptrT = NULL;

       ptrT = ptrInp;
       *cmpsize = getLeftBytes((int*)ptrT, 4);ptrT += 4;
       *decmpsize = getLeftBytes((int*)ptrT, 4);
}

void copyBytesCmp(LPBYTE ptrOut, LPBYTE ptrInp, int size, int deCmpSize){
	int i;
	LPBYTE ptrT;

//printf("copyBytesCmp size= %d, deCmpSize= %d\n", size, deCmpSize);
//fflush(stdout);
    ptrT = ptrOut;
	set4Bytes(ptrT, size);ptrT += 4;
	set4Bytes(ptrT, deCmpSize);ptrT += 4;
	for(i=0;i<size;i++){
		ptrT[i] = ptrInp[i];
	}
}
int getWholeBMPsize(LPIMGDATA img){
	int size = 0;
    int offBits = sizeof (BITMAPFILEHEADER) +
	             sizeof (BITMAPINFOHEADER)
	            + sizeof (RGBQUAD) * 2;
    int biWidthPad;

      biWidthPad = ((img->bi.biWidth+31)/32)*4;
      size = biWidthPad*img->bi.biHeight+offBits;

	return size;
}
int compressMonoImages(LPIMGDATA *img, COMPRESSED_IMGS *outImgBuffer){
	IMGDATA *imgOutTmp[3];
//	IMGDATA *imgOutTmp;
	LPBYTE  imgsTmp[3];
	LPBYTE  ptrImgs, ptrTmp;
//	UINT  *ptrWork;
	int i, size[3], buffSize;

//	char imgName[100];

	imgOutTmp[0] = img[0];
	imgOutTmp[1] = &imgOutTmp[0][1];imgOutTmp[2] = &imgOutTmp[0][2];


//	for(i=0;i<3;i++) {
//		setNewImageName(imgName, imgOutTmp[i]->szIMGName, i);
//		saveBMPbuffer (imgOutTmp[i]->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//	}

// calculate input size
	for(i=0;i<3;i++) {
		size[i] = getWholeBMPsize(imgOutTmp[i]);
		imgsTmp[i] = calloc(size[i]+500, sizeof(BYTE));
	    if(imgsTmp[i] == NULL){
	       sprintf(img[0]->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img[0]->szIMGName, size[i]+500, strerror(errno));
	       return - 1;
	    }

 //printf("uncompressed size i= %d, size= %d\n", i, size[i]);
	}


// compressed image
//    ptrTmp = ptrImgs;
	buffSize = 0;
	for(i=0;i<3;i++) {
		//		imgsTmp[i]= compressImage(imgOutTmp[i]);
//		imgsTmp[i] = NULL;
		outImgBuffer->singleSize[i] = Huffman_Compress( imgOutTmp[i]->ptr, imgsTmp[i], size[i]);
		buffSize += outImgBuffer->singleSize[i];
//printf("Huffman_Compress size i= %d, size= %d\n", i, outImgBuffer->singleSize[i]);
//fflush(stdout);
	}

//  copy images to the buffer
	ptrImgs = calloc(buffSize+24, sizeof(BYTE));
	if(ptrImgs == NULL){
	   sprintf(img[0]->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img[0]->szIMGName, buffSize, strerror(errno));
	   return - 1;
	}

    ptrTmp = ptrImgs;
	for(i=0;i<3;i++) {
			copyBytesCmp(ptrTmp, imgsTmp[i], outImgBuffer->singleSize[i], size[i]);ptrTmp += outImgBuffer->singleSize[i]+8;
	}
	outImgBuffer->ptrBuffer = ptrImgs;
	outImgBuffer->totalSize = buffSize;

	for(i=0;i<3;i++) {
		if(imgsTmp[i] != NULL){
			free(imgsTmp[i]);
			imgsTmp[i] = NULL;
		}
	}

//		ptrTmp += size[i];

//		imgsTmp[i]= compressImage(imgOutTmp[i]);
//		size[i] = getCompressedImageSize(imgsTmp[i]);
// printf("compressed size i= %d, size= %d\n", i, size[i]);
//        buffSize += size[i];

// printf("buffSize size= %d\n", buffSize);

// ptrImgs = calloc(buffSize, sizeof(BYTE));
// if(ptrImgs == NULL){
//    sprintf(img[0]->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img[0]->szIMGName, buffSize, strerror(errno));
//    return - 1;
// }
//  copy images to the buffer
//    ptrTmp = ptrImgs;
//	for(i=0;i<3;i++) {
//		copyBytesCmp(ptrTmp, imgsTmp[i], size[i]);ptrTmp += size[i];
//	}

	return 0;
}
int compressMixedImages(LPIMGDATA img, COMPRESSED_IMGS *outImgBuffer){
	IMGDATA *imgOutTmp;
	LPBYTE  imgsTmp;
	LPBYTE  ptrImgs, ptrTmp;
//	UINT  *ptrWork;
	int  size, buffSize;

//	char imgName[100];

	imgOutTmp = img;
// calculate input size
	size = getWholeBMPsize(imgOutTmp);
	imgsTmp = calloc(size+500, sizeof(BYTE));
    if(imgsTmp == NULL){
       sprintf(img->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, size+500, strerror(errno));
       return - 1;
    }

 //printf("uncompressed size= %d\n",  size);

// compressed image
	buffSize = 0;

	 outImgBuffer->singleSize[1] = Huffman_Compress( imgOutTmp->ptr, imgsTmp, size);
	 buffSize += outImgBuffer->singleSize[1];
//printf("Huffman_Compress size i= %d, size= %d\n", i, outImgBuffer->singleSize[i]);
//fflush(stdout);

//  copy images to the buffer
	ptrImgs = calloc(buffSize+24, sizeof(BYTE));
	if(ptrImgs == NULL){
	   sprintf(img->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, buffSize, strerror(errno));
	   return - 1;
	}

    ptrTmp = ptrImgs;
	copyBytesCmp(ptrTmp, imgsTmp, outImgBuffer->singleSize[1], size);

	outImgBuffer->ptrBuffer = ptrImgs;
	outImgBuffer->totalSize = buffSize;

	if(imgsTmp != NULL){
		free(imgsTmp);
		imgsTmp = NULL;
	}


//		ptrTmp += size[i];

//		imgsTmp[i]= compressImage(imgOutTmp[i]);
//		size[i] = getCompressedImageSize(imgsTmp[i]);
// printf("compressed size i= %d, size= %d\n", i, size[i]);
//        buffSize += size[i];

// printf("buffSize size= %d\n", buffSize);

// ptrImgs = calloc(buffSize, sizeof(BYTE));
// if(ptrImgs == NULL){
//    sprintf(img[0]->error,"compressMonoImages ( \"%s\" ) calloc ( %d ) failed: %s\n",img[0]->szIMGName, buffSize, strerror(errno));
//    return - 1;
// }
//  copy images to the buffer
//    ptrTmp = ptrImgs;
//	for(i=0;i<3;i++) {
//		copyBytesCmp(ptrTmp, imgsTmp[i], size[i]);ptrTmp += size[i];
//	}

	return 0;
}

int decompressMonoImagesOld(COMPRESSED_IMG *inpImgBuffer, LPIMGDATA *img){
	IMGDATA *imgOutTmp[3];
	LPBYTE   ptrTmp;
	int i, size;
	int ret = 0;

	imgOutTmp[0] = img[0];imgOutTmp[1] = &imgOutTmp[0][1];imgOutTmp[2] = &imgOutTmp[0][2];
	ptrTmp = inpImgBuffer->ptrBuffer;
	for(i=0;i<3;i++) {
		size = getCompressedImageSizeOld(ptrTmp);
printf("image i=%d, size= %d\n", i, size);
	    ret = deCompressImage(ptrTmp, imgOutTmp[i]);
	    if(ret <0) break;
	    ptrTmp += size;
	}
    return ret;
}

int deCompressImageNew(LPBYTE ptrC, LPIMGDATA img, int cmpsize, int decmpsize){
   LPBYTE ptrOut = NULL;


   LPBITMAPFILEHEADER bf;
   LPBITMAPINFOHEADER bi;
//   int offBits = sizeof (BITMAPFILEHEADER) +
//             sizeof (BITMAPINFOHEADER)
//            + sizeof (RGBQUAD) * 2;

//      char buff[500];

   ptrOut = calloc(decmpsize, sizeof(BYTE));
   if(ptrOut == NULL) {
    	    sprintf(img->error,"deCompressImage ( \"%s\" ) calloc ( %d ) failed: %s\n",img->szIMGName, decmpsize, strerror(errno));
    	    return -1;
     }
   Huffman_Uncompress( ptrC, ptrOut, cmpsize, decmpsize);

   bf = (LPBITMAPFILEHEADER) ptrOut;
   bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));

    img->ptr = ptrOut;
	img->bf.bfType = getLeftBytes((int*)bf->bfType, 2);
	img->bf.bfSize = getLeftBytes((int*)bf->bfSize, 4);
	img->bf.bfReserved1 =  getLeftBytes((int*)bf->bfReserved1, 2);
	img->bf.bfReserved2 =  getLeftBytes((int*)bf->bfReserved2, 2);
	img->bf.bfOffBits =   getLeftBytes((int*)bf->bfOffBits, 4);
    img->pixPtr = ptrOut +img->bf.bfOffBits;

	img->bi.biSize = getLeftBytes((int*)bi->biSize, 4);
	img->bi.biWidth = getLeftBytes((int*)bi->biWidth, 4);
	img->bi.biHeight = getLeftBytes((int*)bi->biHeight, 4);
	img->bi.biPlanes = getLeftBytes((int*)bi->biPlanes, 2);
	img->bi.biBitCount = getLeftBytes((int*)bi->biBitCount, 2);
	img->bi.biCompression = getLeftBytes((int*)bi->biCompression, 4);
	img->bi.biSizeImage = getLeftBytes((int*)bi->biSizeImage, 4);
	img->bi.biXPelsPerMeter = getLeftBytes((int*)bi->biXPelsPerMeter, 4);
	img->bi.biYPelsPerMeter = getLeftBytes((int*)bi->biYPelsPerMeter, 4);
	img->bi.biClrUsed = getLeftBytes((int*)bi->biClrUsed, 4);
	img->bi.biClrImportant = getLeftBytes((int*)bi->biClrImportant, 4);
	img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

	img->colorPtr = ptrOut + 14 + img->bi.biSize;
	img->width = img->bi.biWidth;
	img->height = img->bi.biHeight;

   return 0;
}

int decompressMonoImages(LPBYTE inpImg, LPIMGDATA *img){
	IMGDATA *imgOutTmp[3];
	LPBYTE   ptrTmp;
	int i, cmpsize, decmpsize;
	int ret = 0;

	imgOutTmp[0] = img[0];imgOutTmp[1] = &imgOutTmp[0][1];imgOutTmp[2] = &imgOutTmp[0][2];
	ptrTmp = inpImg;
	for(i=0;i<3;i++) {
		getCompressedImageSize(ptrTmp, &cmpsize, &decmpsize);
		ptrTmp += 8;

printf("image i=%d, cmpsize= %d, decmpsize= %d\n", i, cmpsize, decmpsize);
fflush(stdout);

        ret = deCompressImageNew(ptrTmp, imgOutTmp[i], cmpsize, decmpsize);

	    if(ret <0) break;
	    ptrTmp += cmpsize;
	}
    return ret;
}

int saveBytesCMPToDisk(COMPRESSED_IMGS *outImgBuffer, char *path, char *iName){
	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;

	char imgName[100];
	strcpy(imgName, iName);
	imgName[strlen(imgName)-4] = 0;
    strcat(imgName, ".cmp");

	strcpy(fileName, path);
	strcat(fileName, imgName);
	len = outImgBuffer->totalSize;

	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMPbuffer (\"%s\") failed: %s\n",fileName, strerror(errno));
		return -1;
	}

	fSize = fwrite(outImgBuffer->ptrBuffer, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f)) {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: %s\n",fileName, strerror(errno));
		}
		else {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		}
		return -1;
	}
	close_file(f);
	return 0;
}
void testCompressImages(COMPRESSED_IMGS *outImgBuffer, char *iName){
	IMGDATA imgOut[3];
	LPIMGDATA lpImgOut;
	int i;

	char imgName[100];

//	saveBytesCMPToDisk(outImgBuffer, "c:\\Users\\Wicek\\OCR\\output\\", iName);
//	printf("----------saveBytesToDisk\n");
//	fflush(stdout);


	for(i=0;i<3;i++) {
        imgOut[i].ptr = NULL;
	}

	lpImgOut = &imgOut[0];
	for(i=0;i<3;i++) {
		strcpy(imgOut[i].szIMGName, iName);
	}

	if(decompressMonoImages(outImgBuffer->ptrBuffer, &lpImgOut)<0){
		printf("----------decompressMonoImages ERROR\n");
		return;
	}
	for(i=0;i<3;i++) {
		setNewImageName(imgName, imgOut[i].szIMGName, 10+i);
		printf("----------saveBMPbuffer; imgName= %s\n", imgName);
		fflush(stdout);
		saveBMPbuffer (imgOut[i].ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
		printf("----------saveBMPbuffer; imgName= %s, done\n", imgName);
		fflush(stdout);

	}

}
void freeMemoryCMPImages(COMPRESSED_IMGS *outImgBuffer){

	if(outImgBuffer->ptrBuffer != NULL) {
		free(outImgBuffer->ptrBuffer);
		outImgBuffer->ptrBuffer = NULL;
	}
}

void freeMemoryCMPImageOld(COMPRESSED_IMG *outImgBuffer){

	if(outImgBuffer->ptrBuffer != NULL) {
		free(outImgBuffer->ptrBuffer);
		outImgBuffer->ptrBuffer = NULL;
	}
}


void copyPixel_BMP24(LPBYTE ptrOut, LPBYTE ptrInp){
	*ptrOut = *ptrInp;
	*(ptrOut+1) = *(ptrInp+1);
	*(ptrOut+2) = *(ptrInp+2);
}
void copyPixel_RGBA(LPBYTE ptrOut, LPBYTE ptrI){
	int r, g, b;
	g =*(ptrI+1);
	#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || __APPLE__ || __linux__ || LINUX || linux || UNIX)
	r =*ptrI;
	b =*(ptrI+2);
	#else
	b =*ptrI;
	r =*(ptrI+2);
	#endif

//	 printf(" r= %d, b= %d, g %d\n", r, g, b);
//	 fflush(stdout);

	*ptrOut = b;  //B
	*(ptrOut+1) = g; //G
	*(ptrOut+2) = r; //R
}
void scaleToBMP(BYTE *R, BYTE *G, BYTE *B);
#define eRGB565_R  0xf800 ///< Mask for blue	(1111100000000000)
#define eRGB565_G  0x07e0 ///< Mask for green	(0000011111100000)
#define eRGB565_B  0x001f ///< Mask for red	(0000000000011111)

void copyPixel_RGB565(LPBYTE ptrOut, WORD *ptrI){
	BYTE r, g, b;
	WORD wr;
	   wr =  *ptrI;
	   b = ((wr & eRGB565_R ) >> 11);
	   g = ((wr & eRGB565_G) ) >> 5;
	   r = (wr & eRGB565_B );
	   scaleToBMP(&r, &g, &b);
		*ptrOut = b;  //B
		*(ptrOut+1) = g; //G
		*(ptrOut+2) = r; //R

}

void reloadBufferBytes_BMP(LPBYTE ptrOut, int widthOut, int heightOut, LPBYTE ptrInp, int widthInp, int heightInp, int pixelStepsX, int pixelStepsY){
int xInp, yInp, iInp, jInp;
int xOut, yOut, iOut, jOut;
int sh= 3;
int biWidthPadInp = ((widthInp*24+31)/32)*4;
int biWidthPadOut = ((widthOut*24+31)/32)*4;
  jInp = 0;
  for(jOut=0;jOut<heightOut;jOut++){
	  iInp = 0;
	  for(iOut=0;iOut<widthOut;iOut++){
		  yInp=jInp*biWidthPadInp; xInp = iInp*sh;
		  yOut=jOut*biWidthPadOut; xOut = iOut*sh;
		  copyPixel_BMP24(ptrOut+yOut+xOut, ptrInp+yInp+xInp);
		  iInp  += pixelStepsX;
	  }
	  jInp += pixelStepsY;
  }
}
void reloadBufferBytes_RGBA(LPBYTE ptrOut, int widthOut, int heightOut, LPBYTE ptrInp, int widthInp, int heightInp, int pixelStepsX, int pixelStepsY){
int xInp, yInp, iInp, jInp;
int xOut, yOut, iOut, jOut;
int shInp= 4;
int shOut= 3;
int biWidthPadInp = widthInp*shInp;
int biWidthPadOut = ((widthOut*24+31)/32)*4;
//int print = 0;

  jInp = 0;
  for(jOut=0;jOut<heightOut;jOut++){
	  iInp = 0;
	  for(iOut=0;iOut<widthOut;iOut++){
		  yInp=jInp*biWidthPadInp; xInp = iInp*shInp;
		  yOut=(heightOut -1 -jOut)*biWidthPadOut; xOut = iOut*shOut;
//		  yOut=jOut*biWidthPadOut; xOut = iOut*shOut;
		  copyPixel_RGBA(ptrOut+yOut+xOut, ptrInp+yInp+xInp);
//if(jOut > 1222) {
// printf("2 widthOut= %d, heightOut= %d, widthInp= %d, heightInp= %d\n", widthOut, heightOut, widthInp, heightInp);
// printf("2 iOut= %d, jOut= %d, iInp= %d, jInp= %d\n", iOut, jOut, iInp, jInp);
// fflush(stdout);
// print++;
//}
iInp  += pixelStepsX;
	  }
	  jInp += pixelStepsY;
  }

}
void reloadBufferBytes_RGB565(LPBYTE ptrOut, int widthOut, int heightOut, LPBYTE ptrInp, int widthInp, int heightInp, int pixelStepsX, int pixelStepsY){
int xInp, yInp, iInp, jInp;
int xOut, yOut, iOut, jOut;
int shOut= 3;
int biWidthPadInp = widthInp;
int biWidthPadOut = ((widthOut*24+31)/32)*4;
//int print = 0;
WORD *ptrInpW;


  ptrInpW = (WORD *)ptrInp;
  jInp = 0;
  for(jOut=0;jOut<heightOut;jOut++){
	  iInp = 0;
	  for(iOut=0;iOut<widthOut;iOut++){
		  yInp=jInp*biWidthPadInp; xInp = iInp;
		  yOut=(heightOut -1 -jOut)*biWidthPadOut; xOut = iOut*shOut;
		  copyPixel_RGB565(ptrOut+yOut+xOut, ptrInpW+yInp+xInp);
          iInp  += pixelStepsX;
	  }
	  jInp += pixelStepsY;
  }

}

void copyBytesOffBits(LPBYTE ptrOut, LPBYTE ptrInp, int offBits){
	int i;
	for(i=0;i<offBits;i++)
		ptrOut[i] = ptrInp[i];
}

void createSmallImageBMP(LPIMGDATA img, LPIMGDATA imgOut, int pixelStepsX, int pixelStepsY){
	int bufforSize;
//	int offBits = sizeof (BITMAPFILEHEADER) +
//			sizeof (BITMAPINFOHEADER);
	int offBits;
	LPBYTE ptrH;
	int dpi = 500;
//	int nBytes = 0;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;



//	int biWidthPad = ((img->width*24+31)/32)*4;
	int widthOut, heightOut;
	int biWidthPadOut;

	widthOut = img->width/pixelStepsX;
	heightOut = img->height/pixelStepsY;
	biWidthPadOut = ((widthOut*24+31)/32)*4;

	offBits = img->bf.bfOffBits;

	bufforSize = biWidthPadOut*heightOut+offBits;

    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
    if(ptrH == NULL) {
//    	sprintf(error,"BMP_OCR_Call();   yl_calloc ( %d ) failed: %s\n",bufforSize, strerror(errno));
    	return ;
    }
//    printf("\n   offBits= %d \n", offBits);
//    fflush(stdout);

    copyBytesOffBits(ptrH, img->ptr, offBits);

//    printf("after   offBits= %d \n", offBits);
//    fflush(stdout);


	if(widthOut < heightOut)
	  dpi = (int)((double)widthOut/3.1875);
	else
	  dpi = (int)((double)heightOut/3.1875);

	bf = (LPBITMAPFILEHEADER) ptrH;
//	bf->bfType[0] = 0x42;
//	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, bufforSize);
//	bf->bfReserved1[0] = 0;
//	bf->bfReserved1[1] = 0;
//	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
//	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, widthOut);
	set4Bytes(bi->biHeight, heightOut);
//	bi->biPlanes[0] = 1;
//	bi->biPlanes[1] = 0;
//	bi->biBitCount[0]= 1;
//	bi->biBitCount[1]= 0;
//	set4Bytes(bi->biCompression, BI_RGB);
//	set4Bytes(bi->biSizeImage, bufforSize - offBits);
	set4Bytes(bi->biXPelsPerMeter, dpi);
	set4Bytes(bi->biYPelsPerMeter, dpi);
//	set4Bytes(bi->biClrUsed, 0);
//	set4Bytes(bi->biClrImportant, 0);




	imgOut->ptr = ptrH;
	imgOut->pixPtr = ptrH + offBits;

	imgOut->bf.bfType = 0x4d42;
	imgOut->bf.bfSize = bufforSize;
	imgOut->bf.bfReserved1 =  0;
	imgOut->bf.bfReserved2 =  0;
	imgOut->bf.bfOffBits =   offBits;


	imgOut->bi.biSize = img->bi.biSize;
	imgOut->width = widthOut;
	imgOut->bi.biWidth = imgOut->width;
	imgOut->height = heightOut;
	imgOut->bi.biHeight = imgOut->height;
	imgOut->bi.biPlanes = img->bi.biPlanes;
	imgOut->bi.biBitCount = img->bi.biBitCount;

	imgOut->bi.biCompression = img->bi.biCompression;
	imgOut->bi.biSizeImage = bufforSize - offBits;

	imgOut->bi.biXPelsPerMeter = dpi;
	imgOut->bi.biYPelsPerMeter = dpi;
	imgOut->bi.biClrUsed = 0;
	imgOut->bi.biClrImportant = 0;
	imgOut->bi.biWidthPad = ((imgOut->bi.biWidth*imgOut->bi.biBitCount+31)/32)*4;

	reloadBufferBytes_BMP(imgOut->pixPtr, widthOut, heightOut, img->pixPtr, img->width, img->height, pixelStepsX, pixelStepsY);

}
void createSmallImageRGBA_RGB565(LPIMGDATA img, LPIMGDATA imgOut, int pixelStepsX, int pixelStepsY, int mode){
	int bufforSize;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER);
//	int offBits;
	LPBYTE ptrH;
	int dpi = 500;
//	int nBytes = 0;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;




	int widthOut, heightOut;
	int biWidthPadOut;

	widthOut = img->width/pixelStepsX;
	heightOut = img->height/pixelStepsY;
	biWidthPadOut = ((widthOut*24+31)/32)*4;


	bufforSize = biWidthPadOut*heightOut+offBits;

    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
    if(ptrH == NULL) {
//    	sprintf(error,"BMP_OCR_Call();   yl_calloc ( %d ) failed: %s\n",bufforSize, strerror(errno));
    	return ;
    }

	if(widthOut < heightOut)
	  dpi = (int)((double)widthOut/3.1875);
	else
	  dpi = (int)((double)heightOut/3.1875);

	bf = (LPBITMAPFILEHEADER) ptrH;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, bufforSize);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, widthOut);
	set4Bytes(bi->biHeight, heightOut);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 24;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, bufforSize - offBits);
	set4Bytes(bi->biXPelsPerMeter, dpi);
	set4Bytes(bi->biYPelsPerMeter, dpi);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);



	imgOut->ptr = ptrH;
	imgOut->pixPtr = ptrH + offBits;

	imgOut->bf.bfType = 0x4d42;
	imgOut->bf.bfSize = bufforSize;
	imgOut->bf.bfReserved1 =  0;
	imgOut->bf.bfReserved2 =  0;
	imgOut->bf.bfOffBits =   offBits;


	imgOut->bi.biSize = sizeof (BITMAPINFOHEADER);
	imgOut->width = widthOut;
	imgOut->bi.biWidth = imgOut->width;
	imgOut->height = heightOut;
	imgOut->bi.biHeight = imgOut->height;
	imgOut->bi.biPlanes = getLeftBytes((int*)bi->biPlanes, 2);
	imgOut->bi.biBitCount = getLeftBytes((int*)bi->biBitCount, 2);

	imgOut->bi.biCompression = getLeftBytes((int*)bi->biCompression, 4);
	imgOut->bi.biSizeImage = bufforSize - offBits;

	imgOut->bi.biXPelsPerMeter = dpi;
	imgOut->bi.biYPelsPerMeter = dpi;
	imgOut->bi.biClrUsed = getLeftBytes((int*)bi->biClrUsed, 4);
	imgOut->bi.biClrImportant = getLeftBytes((int*)bi->biClrImportant, 4);
	imgOut->bi.biWidthPad = ((imgOut->bi.biWidth*imgOut->bi.biBitCount+31)/32)*4;


	if(mode == 0)
	  reloadBufferBytes_RGBA(imgOut->pixPtr, widthOut, heightOut, img->pixPtr, img->width, img->height, pixelStepsX, pixelStepsY);
	if(mode == 1)
	  reloadBufferBytes_RGB565(imgOut->pixPtr, widthOut, heightOut, img->pixPtr, img->width, img->height, pixelStepsX, pixelStepsY);


}


int createSmallColorImage(LPIMGDATA img, LPIMGDATA imgOut, int pixelStepsX, int pixelStepsY){
//	printf("\n   img->bi.biBitCount= %d \n", img->bi.biBitCount);

	switch(img->bi.biBitCount) {
	case 24:
		createSmallImageBMP(img, imgOut, pixelStepsX, pixelStepsY);
		break;
	case 32:
		createSmallImageRGBA_RGB565(img, imgOut, pixelStepsX, pixelStepsY, 0);
		break;
	case 565:
		createSmallImageRGBA_RGB565(img, imgOut, pixelStepsX, pixelStepsY, 1);
		break;

	}


return 0;
}

void convertDPI_in_BMP_2(LPIMGDATA img, int dpiX_Otp, int dpiY_Otp){
	int wOtp, wPadOtp;
	int hOtp;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;


	// calculate wOtp
	wOtp = img->bi.biWidth;
	hOtp = img->bi.biHeight;
	wPadOtp = ((wOtp+31)/32)*4;

	img->bf.bfSize = wPadOtp*hOtp + img->bf.bfOffBits;

	img->bi.biWidth = wOtp;
	img->width = wOtp;
	img->bi.biHeight= hOtp;
	img->height= hOtp;
	img->bi.biXPelsPerMeter=dpiX_Otp;
	img->bi.biYPelsPerMeter=dpiY_Otp;
	img->bi.biWidthPad = wPadOtp;
	img->bi.biSizeImage = wPadOtp*hOtp;

	bf = (LPBITMAPFILEHEADER)(img->ptr);
	bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));



	set4Bytes(bf->bfSize, img->bf.bfSize);

	set4Bytes(bi->biWidth, img->bi.biWidth);
	set4Bytes(bi->biHeight, img->bi.biHeight);

	set4Bytes(bi->biSizeImage, img->bi.biSizeImage);
	set4Bytes(bi->biXPelsPerMeter, img->bi.biXPelsPerMeter);
	set4Bytes(bi->biYPelsPerMeter, img->bi.biYPelsPerMeter);

}

void pre_ProcessColorImage_getImagesForOCR_Old(LPIMGDATA img, LPIMGDATA *imgOut, int byteSteps, int US_state, EDGEINFO **lpEdgeInfo){
	LPBYTE imgPtr = img->pixPtr;
	int    biWidthPad, biSizeImage;
	int *  lpImgMap_m;
	int *  lpImgMap_s;
	int    grid = 50;
	int    mapSize;
	float kFactor;
    IMGDATA *imgOutTmp;
    float kFhh;
    int  tooDark, iter;
    int cVth, cHth;
    EDGEINFO *edgeInfoTmp;


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
	switch(img->bi.biBitCount) {
	case 24:
		getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);

		cVth = 300; cHth = 140;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 140; cHth = 300;
		}
	    iter = 0; kFhh = 0.1;kFactor = 0.0;
	    imgOutTmp = imgOut[0];edgeInfoTmp = lpEdgeInfo[0];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
	      convert24BMPtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
		  tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//	       printf("0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	       if(tooDark == 0) break;
	       iter++;
		}
		cVth = 250; cHth = 100;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 100; cHth = 250;
		}
	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convert24BMPtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  1, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//	      printf("1;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	      fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;kFhh = 0.05;
		}
		cVth = 200; cHth = 80;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 80; cHth = 200;
		}

	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 20) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convert24BMPtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  2, edgeInfoTmp);
		  if((edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) || (edgeInfoTmp->ccV+edgeInfoTmp->ccH>cVth+cHth)) tooDark++;
//	      printf("2;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;
		}

		break;
	case 32:

	    getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
		cVth = 300; cHth = 140;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 140; cHth = 300;
		}

	    iter = 0; kFhh = 0.1;kFactor = 0.0;
	    imgOutTmp = imgOut[0];edgeInfoTmp = lpEdgeInfo[0];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGBAtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

		  tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//	       printf("0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	       if(tooDark == 0) break;
	       iter++;
		}

		cVth = 250; cHth = 100;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 100; cHth = 250;
		}

	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGBAtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  1, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//	      printf("1;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	      fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;kFhh = 0.05;
		}

		cVth = 200; cHth = 80;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 80; cHth = 200;
		}

	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 20) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGBAtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  2, edgeInfoTmp);
		  if((edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) || (edgeInfoTmp->ccV+edgeInfoTmp->ccH>cVth+cHth)) tooDark++;
//	      printf("2;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;
		}

        break;
	case 565:
        getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);

		cVth = 300; cHth = 140;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 140; cHth = 300;
		}

	    iter = 0; kFhh = 0.05;kFactor = 0.0;
	    imgOutTmp = imgOut[0];edgeInfoTmp = lpEdgeInfo[0];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

		  tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//       printf("0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//       fflush(stdout);
	       if(tooDark == 0) break;
	       iter++;
	   }

		cVth = 250; cHth = 100;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 100; cHth = 250;
		}

	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 3) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  1, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
//     printf("1;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//      fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;kFhh = 0.05;
		}

		cVth = 200; cHth = 80;
		if(img->bi.biWidth > img->bi.biHeight) {
			cVth = 80; cHth = 200;
		}

	    iter = 0;
        imgOutTmp = &imgOutTmp[1];edgeInfoTmp = &edgeInfoTmp[1];
		while(iter < 20) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }

		  convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
	      tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  2, edgeInfoTmp);
		  if((edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) || (edgeInfoTmp->ccV+edgeInfoTmp->ccH>cVth+cHth)) tooDark++;
// printf("2;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//fflush(stdout);
	      if(tooDark == 0) break;
	      iter++;
		}


  /*
		kFactor = 0.08+kFhh;
		imgOutTmp = imgOut[0];
        convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);


        getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
		kFactor = 0.25+kFhh;
		imgOutTmp = &imgOutTmp[1];
        convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

        getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
        kFactor = 0.45+kFhh;
		imgOutTmp = &imgOutTmp[1];
        convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);
*/
		break;

	default:
		sprintf(img->error,"image type not supported yet \n");

		break;
	}

	yl_free(lpImgMap_m);
	yl_free(lpImgMap_s);
	if(img->error[0] != 0)
		return;

	imgOutTmp = imgOut[0];
	{int i;
	if(imgOutTmp->bi.biWidth > imgOutTmp->bi.biHeight) {
		for(i=0;i<3;i++){
		   rotate(imgOutTmp, 90);
		   imgOutTmp = &imgOutTmp[1];
		}
	}

/*
	{
	char imgName[100];
	imgOutTmp = imgOut[0];
	for(i=0;i<3;i++) {
	   setNewImageName(imgName, img->szIMGName, i+70);
	   saveBMPbuffer (imgOutTmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	   imgOutTmp = &imgOutTmp[1];
	}
	}
*/

	}
}


void pre_ProcessColorImage_getImagesForOCR(LPIMGDATA img, LPIMGDATA imgOut, int byteSteps, int US_state, EDGEINFO *lpEdgeInfo){
	LPBYTE imgPtr = img->pixPtr;
	int    biWidthPad, biSizeImage;
	int *  lpImgMap_m;
	int *  lpImgMap_s;
	int    grid = 50;
	int    mapSize;
	float kFactor;
    IMGDATA *imgOutTmp = NULL;
    float kFhh;
    int  tooDark, iter;
    int cVth, cHth, veryDark_th;
//    int cVth2, cHth2;
    EDGEINFO *edgeInfoTmp;
//    int ccVPrev, ccHPrev;

    char imgName[100];
//    char buff[2000];

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

	    iter = 0; kFhh = 0.1;kFactor = 0.0;
	    imgOutTmp = imgOut;edgeInfoTmp = lpEdgeInfo;
		while(iter < 6) {
		  kFactor += kFhh;
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
	       setNewImageName(imgName, img->szIMGName, iter);
	       saveBMPbuffer (imgOutTmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//     sprintf(buff,"24; 0;---iter= %d; kFactor= %e\n",iter, kFactor);
// 	 saveLogFile("", buff, "log_SWT.txt");


		  if(tooDark <= 0) break;
	       iter++;
		}

		break;
	case 32:
	    getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
//		cVth = 300; cHth = 140;
//		if(img->bi.biWidth > img->bi.biHeight) {
//			cVth = 140; cHth = 300;
//		}
	    cVth = 200; cHth = 40;
	    if(img->bi.biWidth > img->bi.biHeight) {
	    	cVth = 40; cHth = 200;
	    }
		veryDark_th = 1300;

	    iter = 0; kFhh = 0.1;kFactor = 0.0;
	    imgOutTmp = imgOut;edgeInfoTmp = lpEdgeInfo;
//	    ccVPrev = 0; ccHPrev = 0;
		while(iter < 6) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGBAtoMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

		  tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
		  if(edgeInfoTmp->ccV + edgeInfoTmp->ccH > veryDark_th) tooDark = -7;
//	       printf("0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
	       if(tooDark <= 0) break;
	       iter++;
		}

        break;
	case 565:
        getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);

	    cVth = 200; cHth = 40;
	    if(img->bi.biWidth > img->bi.biHeight) {
	    	cVth = 40; cHth = 200;
	    }

//		cVth = 300; cHth = 140;
//		if(img->bi.biWidth > img->bi.biHeight) {
//			cVth = 140; cHth = 300;
//		}
		veryDark_th = 1300;

	    iter = 0; kFhh = 0.1;kFactor = 0.0;
	    imgOutTmp = imgOut;edgeInfoTmp = lpEdgeInfo;
		while(iter < 6) {
		  kFactor += kFhh;
		  if(imgOutTmp->ptr != NULL){
	 	        yl_free(imgOutTmp->ptr);
	 	       imgOutTmp->ptr = NULL;
		  }
		  convertRGB565toMonochromeGrid(img, imgOutTmp, lpImgMap_m, lpImgMap_s, grid, kFactor);

		  tooDark = 0;
		  ImageEdgeTest(imgOutTmp,  0, edgeInfoTmp);
		  if(edgeInfoTmp->ccV > cVth && edgeInfoTmp->ccH > cHth) tooDark++;
		  if(edgeInfoTmp->ccV + edgeInfoTmp->ccH > veryDark_th) tooDark = -7;

//       printf("0;---iter= %d; kFactor= %e, cV= %d, cH= %d, tooDark= %d\n",iter, kFactor, edgeInfoTmp->ccV, edgeInfoTmp->ccH, tooDark);
//       fflush(stdout);
//       setNewImageName(imgName, img->szIMGName, iter);
//       saveBMPbuffer (imgOutTmp->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

	       if(tooDark == 0) break;
	       iter++;
	   }

		break;

	default:
		sprintf(img->error,"image type not supported yet \n");

		break;
	}

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


//	if(imgOut->ptr != NULL) {
//	   yl_free(imgOut->ptr);
//	   imgOut->ptr = NULL;
//	}

}


/*
float getMassInside(LPIMGDATA ptrImg){
	  int biWidthPadOut2, biHeightOut2;
	  int x1, x2, y1, y2;
	  int xC, yC;
	  int hist[5000];
	  int i, d, sum;
	  float m;


	   biWidthPadOut2 = ((ptrImg->bi.biWidth*ptrImg->bi.biBitCount+31)/32)*4;
	   biHeightOut2 = ptrImg->bi.biHeight;

	   xC= ptrImg->bi.biWidth/2;yC= ptrImg->bi.biHeight/2;
	   x1 = xC-xC/2;x2 = xC+xC/2;
	   y1 = yC-yC/2;y2 = yC+yC/2;
	   histVPtrPix(ptrImg->pixPtr, biWidthPadOut2, biHeightOut2, H_SIZE_PREPROC, x1, x2, y1, y2, hist);
	   sum = 0;
       for(i=1;i<hist[0];i++){
    	   sum += hist[i];
       }
       m = 0;
       d= (x2-x1)*(y2-y1);
       if(d>0)
    	   m = (float)sum/(float)d;

       return m;
}
*/
int testLineStruct(LPIMGDATA ptrImg){
	int ok = 0;
	  int biWidthPad, biHeight;
	  int x1, x2, y1, y2;
	  int xC, yC;
	  int hist[5000];

	     char buff[500];
	     int kk;


	   biWidthPad = ((ptrImg->bi.biWidth*ptrImg->bi.biBitCount+31)/32)*4;
	   biHeight = ptrImg->bi.biHeight;

	   xC= ptrImg->bi.biWidth/2;yC= ptrImg->bi.biHeight/2;
	   x1 = xC-xC/3;x2 = xC+xC/3;
	   y1 = yC;y2 = yC+yC/3;
	   histVPtrPix(ptrImg->pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, x1, x2, y1, y2, hist);

		sprintf(buff," top------------------------------histV[0]= %d, \n",hist[0] );
		 saveLog(buff, "\n");
		 for(kk=1;kk<hist[0];kk++){
			 sprintf(buff," histV[ %d ]= %d",kk, hist[kk]);
			 saveLog(buff, "\n");
		 }

		   histHTest_X1_X1(ptrImg->pixPtr, biWidthPad, biHeight,  x1, x2, y1, y2, hist);

			 saveLog("", "\n\n");
			sprintf(buff," top------------------------------histH[0]= %d, \n",hist[0] );
			 saveLog(buff, "\n");
			 for(kk=1;kk<hist[0];kk++){
				 sprintf(buff," histH[ %d ]= %d",kk, hist[kk]);
				 saveLog(buff, "\n");
			 }



	   x1 = xC-xC/3;x2 = xC+xC/3;
	   y1 = yC-yC/3;y2 = yC;
	   histVPtrPix(ptrImg->pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, x1, x2, y1, y2, hist);

			sprintf(buff," bottom------------------------------histV[0]= %d, \n",hist[0] );
			 saveLog(buff, "\n");
			 for(kk=1;kk<hist[0];kk++){
				 sprintf(buff," histV[ %d ]= %d",kk, hist[kk]);
				 saveLog(buff, "\n");
			 }


	   histHTest_X1_X1(ptrImg->pixPtr, biWidthPad, biHeight,  x1, x2, y1, y2, hist);

		 saveLog("", "\n\n");
		sprintf(buff," bottom------------------------------histH[0]= %d, \n",hist[0] );
		 saveLog(buff, "\n");
		 for(kk=1;kk<hist[0];kk++){
			 sprintf(buff," histH[ %d ]= %d",kk, hist[kk]);
			 saveLog(buff, "\n");
		 }


	return ok;
}
int linearStructureTest(LPIMGDATA imgInp, char* imgN){
	int linStruct = 1;

	int ddSmearingx = 40;
	int ddSmearingy = 40;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
//	int m, stopSm;
	int m;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	IMGDATA imgOut_Sm;
	int TH;
	int ok;
	//float mass;

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
		printf("yl_calloc failed, ptrImgOut");
		return -1;
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
	copyHeadersBMP(ptrImgOut, &imgOut_Sm);

	for(idxH=0;idxH<sizeH;idxH++){
		gridH = ddSmearingy;
		if(idxH == sizeH-1) gridH = lastH;
		for(idxW=0;idxW<sizeW;idxW++){
			gridW = ddSmearingx;
			if(idxW == sizeW-1) gridW = lastW;
			TH = (5*(gridH*gridW))/6;

			   m = getValueForSmearingTH(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH, TH);
//			   m = getValueForSmearing_White(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);
			  setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
		}
	}

	{
 		char imgName[100];
    setNewImageName(imgName, imgN, 2);

    saveBMPbuffer (ptrImgOut, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
	}
//    mass = getMassInside(&imgOut_Sm);
//    printf("--- mass= %e\n", mass);
	ok = testLineStruct(&imgOut_Sm);

    yl_free(ptrImgOut);

	return linStruct;
}

int ImageTest_QualityOld(LPIMGDATA img,  int byteSteps, int US_state, LPTICKETDATA ticketData){

   int retValue = -1;
   IMGDATA imgOut;
   IMGDATA *imgOut_tmp;
   EDGEINFO edgeInfo;
   EDGEINFO *lpEdgeInfo;
//   int kk;
   IMGDATA imgOutTabtmp;
//   int ok;

//   char imgName[100];


   imgOut.ptr = NULL;
   strcpy(imgOut.szIMGName,img->szIMGName);


     lpEdgeInfo = &edgeInfo;
     pre_ProcessColorImage_getImagesForOCR(img, &imgOut, byteSteps, US_state, lpEdgeInfo);
     retValue = -1;
     imgOutTabtmp.ptr = NULL;


     imgOut_tmp = &imgOut;
     copyImageForTests(imgOut_tmp, &imgOutTabtmp);


// very white/black
     lpEdgeInfo = &edgeInfo;

//     printf("--- cV= %d, cH= %d, lpEdgeInfo->ccV+ lpEdgeInfo->ccH= %d\n", lpEdgeInfo->ccV, lpEdgeInfo->ccH, lpEdgeInfo->ccV+ lpEdgeInfo->ccH);
//     fflush(stdout);

     if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH < 6) retValue = -77;
     if(lpEdgeInfo->ccV+ lpEdgeInfo->ccH > 1000)   retValue = -77;

// tests------------------------------------
//     if(retValue> -10 && retValue < 0){
//            linearStructureTest(&imgOutTabtmp, img->szIMGName);
//    		ok = testLineStruct(&imgOutTabtmp);

//     }
// tests------------------------------------

     if(retValue> -10 && retValue < 0){
    	  int biWidthPad, biHeight;
    	  int x1, x2, y1, y2;
    	  int xC, yC;
    	  int digitCount = 0; //, mass1, mass2;
    	  int too_W_B, sumcc_V_H;
   	      int pixX, pixY;

   	      pixX = 24; pixY = 24;
//    	  char buff[500];

         retValue = barcodeTest2D(&imgOutTabtmp, pixX, pixY, 0);
    	 if(retValue < 0){
    	    	 too_W_B = 0;
    	        speckFiltration(&imgOutTabtmp);

    	        biWidthPad = ((imgOutTabtmp.bi.biWidth*imgOutTabtmp.bi.biBitCount+31)/32)*4;
    	        biHeight = imgOutTabtmp.bi.biHeight;
    	        xC= imgOutTabtmp.bi.biWidth/2;yC= imgOutTabtmp.bi.biHeight/2;
    	        x1 = xC-3*xC/4;x2 = xC+5*xC/8;
    	        y1 = yC-5*yC/8;y2 = yC+yC/2;

//   	        printf("------kk= %d\n",kk);
   	     strcpy(imgOutTabtmp.szIMGName, img->szIMGName);

   	  lpEdgeInfo = &edgeInfo;
   	  sumcc_V_H = lpEdgeInfo->ccV+ lpEdgeInfo->ccH;
//   	            if(sumcc_V_H < 1000) {
	                digitCount = recognizeAllLines(&imgOutTabtmp, x1, x2, y1, y2, ticketData, 0, sumcc_V_H);
//	                printf(" digitCount= %d\n",digitCount);
 	                if(digitCount > 0) {retValue = 0;}


//	  printf(" retValue= %d, \n",retValue);
//	  fflush(stdout);
    	 }
     }


//   setNewImageName(imgName, img->szIMGName, 7);
//   saveBMPbuffer (imgOutTabtmp.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");

   	 if(imgOutTabtmp.ptr != NULL){
	     yl_free(imgOutTabtmp.ptr);
		 imgOutTabtmp.ptr = NULL;
   	 }
 	if(imgOut.ptr != NULL) {
 	   yl_free(imgOut.ptr);
 	   imgOut.ptr = NULL;
 	}

   return retValue;
}
