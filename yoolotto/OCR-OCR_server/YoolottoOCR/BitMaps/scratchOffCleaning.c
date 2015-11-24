/*
 * scratchOffCleaning.c
 *
 *  Created on: Jan 28, 2014
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include  "../headers/Functions.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"

#include "../headers/BarCode.h"

extern int hHistPreProc[H_SIZE_PREPROC];
extern int vHistPreProc[H_SIZE_PREPROC];

extern char strGResultI25[64];
extern int  cnfGResultI25[64];
extern QUADRANGLE qGResultI25[64];
extern char strGResultUPC[64];
extern int  cnfGResultUPC[64];
extern QUADRANGLE qGResultUPC[64];



void getMapValues_24(LPBYTE imgPtr, int biWidth, int biWidthPad, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGBA(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void getMapValues_RGB565(LPBYTE imgPtr, int biWidth, int biHeight, int *lpImgMap_m, int *lpImgMap_s, int grid, int byteSteps, int skipSigma);
void convert24BMPtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertRGBAtoMonochromeGrid(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);
void convertRGB565toMonochromeGrid_sOff(LPIMGDATA img, LPIMGDATA imgOut, int *lpImgMap_m, int *lpImgMap_s, int grid, float kFactor);

//void JustSmearing(LPIMGDATA imgInp, LPIMGDATA imgOut, int pix);
void JustSmearing2(LPIMGDATA imgInp, LPIMGDATA imgOut, int pix, LPBYTE *net, int *sW, int *sH);
//int getValueForSmearing(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getPixelSmearing(LPBYTE ptr, int xn, int yn, int wPad, int height);
void filterH(LPBYTE net, int sW, int sH, int thL, int thM, int thR);
void copyPixelsFromNetToImage(LPBYTE net, int sW, int sH, LPIMGDATA img);
void setPixelOut(LPBYTE ptr, int w, int h, int xinp, int yinp, int v);
int  findBarcodes_SRC(LPIMGDATA imgOut, int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int overLap);
//void extractField_BC(LPIMGDATA imgOut, int x, int dx, int y, int dy, int pix, int idx,
//		   char *strResultOut, QUADRANGLE *q, int *confROut, float *angleOut, int *UPCflg);
void extractField_BC(LPIMGDATA imgOut, int x, int dx, int y, int dy, int pix, int idx,
		   char *strResultOut, QUADRANGLE *q, int *confROut, float *angleOut, int *UPCflg,
		   int *xOutG1, int *xOutG2, int *yOutG1, int *yOutG2);
int extractField_BC_OCR(LPIMGDATA imgOut, int x, int dx, int y, int dy, int pix, int idx,
		                 int xOut1, int xOut2, int yOut1, int yOut2, double angle, LPTICKETDATA ticketData);

int filter_BC(int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int numBarcodes);
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
int barDeCodeI25 (int   *hist, int lenH, LPMASK maskAlph, int lenStr, int nOfCharInBar, int invDec,
                  LPSTR strResult, int *confResult, LPSTR cand2);
int barDeCodeUPC (
    int   *hist,               // vertical histogram of barcode
    int      lenH,                // histogram length
    int      lenStr,              // number of characters for recognition
    LPSTR    strResult,           // pointer to output string
    int      *confResult         // results confidences
  );
int barDeCodeUPC2 (
    int   *hist,               // vertical histogram of barcode
    int      lenH,                // histogram length
    int      lenStr,              // number of characters for recognition
    LPSTR    strResult,           // pointer to output string
    int      *confResult         // results confidences
  );
int getWrongChars(char *strR);
int getWrongChars_2(char *strR);
void copyQ(QUADRANGLE *qOut, QUADRANGLE *qInp);
void copyBarcodedResults(LPTICKETRESULTS ticketResults, char *strResult, QUADRANGLE *q, int *conf, char *name, float angle, int idx);
void barCPostProcessing_TX(LPTICKETRESULTS ticketResults);
int testConfidence_BC(char *strResult, int *conf, int th);
void adjustConfidenceUPC(char *strResult, int *conf);
void adjustConfidenceI25(char *strResult, int *conf);
int getOCRforBC(LPIMGDATA imgOut, LPTICKETDATA ticketData, int x1, int x2, int y1, int y2, double angle, int idx);
int test_I25_OCR(LPTICKETDATA ticketData);
void adjustResultsI25(LPTICKETDATA ticketData, char* strResultI25, int *confI25);
void trimSpOut(LPTICKETDATA ticketData);
void trimtResultsI25(char* strResultI25, int *confI25);
int test_G_I25();
void initQ(LPQUADRANGLE q);

#define MAX_BARCODES_NUM 50


void searchBarcodes_SRC (LPIMGDATA img, LPTICKETRESULTS lpTicketResults, int US_state, LPTICKETDATA ticketData){
//	LPBYTE imgPtr = img->pixPtr;
	int    biWidthPad, biSizeImage;
	IMGDATA imgOut2;
	int pix = 16;
    LPBYTE net_values = NULL;
    int sizeNetW, sizeNetH;
    int barCodeX[MAX_BARCODES_NUM], barCodedX[MAX_BARCODES_NUM];
    int barCodeY[MAX_BARCODES_NUM], barCodedY[MAX_BARCODES_NUM];
    int numBarcodes = 0, i, k;
	char strResultI25[64];
	char strResultUPC[64];
	char strResult[64];
	int  confUPC[64];
	int  confI25[64];
	int  confR[64];
	QUADRANGLE qUPC, qI25, q;
	int UPCflg, numWrong, numWrongUPC, numWrongI25;
	float angle, angleUPC, angleI25;
	int xOut1, xOut2, yOut1, yOut2;
	int flgI25_OCR, testGI25;

//      char imgName[100];

     lpTicketResults->type = ticketTypeInt;
//  	setNewImageName(imgName, img->szIMGName, 0);
//  	saveBMPbuffer (img->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


	biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biSizeImage = biWidthPad*img->bi.biHeight;

//		cleanTopBottom_SRC(img );
	sizeNetW = 0; sizeNetH = 0;
	JustSmearing2(img, &imgOut2, pix, &net_values, &sizeNetW, &sizeNetH);
	if(net_values == NULL) {
		return;
	}
//	 printf(" JustSmearing2;  sizeNetW= %d, sizeNetH= %d\n",sizeNetW, sizeNetH);
//	 fflush(stdout);


//		    	setNewImageName(imgName, img->szIMGName, 1);
//		    	saveBMPbuffer (imgOut2.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


	filterH(net_values, sizeNetW, sizeNetH, 2, 3, 2);

//	 printf(" filterH;  sizeNetW= %d, sizeNetH= %d\n",sizeNetW, sizeNetH);
//	 fflush(stdout);

	copyPixelsFromNetToImage(net_values, sizeNetW, sizeNetH, &imgOut2);

//	setNewImageName(imgName, img->szIMGName, 1);
//	saveBMPbuffer (imgOut2.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


//	 printf(" copyPixelsFromNetToImage;  sizeNetW= %d, sizeNetH= %d\n",sizeNetW, sizeNetH);
//	 fflush(stdout);

//	saveBMPbuffer (imgOut2.ptr, img->szIMGName, img->szOutPath);
	numBarcodes = findBarcodes_SRC(&imgOut2, barCodeX, barCodedX, barCodeY, barCodedY, 2);
//printf(" numBarcodes= %d\n",numBarcodes);
//fflush(stdout);
	if(numBarcodes > 5) {
		numBarcodes = findBarcodes_SRC(&imgOut2, barCodeX, barCodedX, barCodeY, barCodedY, 90);
	}
//printf(" numBarcodes= %d\n",numBarcodes);
//fflush(stdout);
// filter overlapping
	numBarcodes= filter_BC(barCodeX, barCodedX, barCodeY, barCodedY, numBarcodes);
//printf(" numBarcodes= %d\n",numBarcodes);
//fflush(stdout);
	testGI25 = 0;
	testGI25 = test_G_I25();
//printf(" testGI25= %d\n",testGI25);
//fflush(stdout);
	initQ(&qUPC);
	initQ(&qI25);
	numWrongUPC = 100;
	numWrongI25 = 100;
	strResultUPC[0] = 0;strResultI25[0] = 0;
	angleUPC = 0; angleI25 = 0;flgI25_OCR = 0;
	for(i=0;i<numBarcodes;i++){
           extractField_BC(img, barCodeX[i], barCodedX[i], barCodeY[i], barCodedY[i], pix, i,
        		   strResult, &q, confR,  &angle, &UPCflg,
        		   &xOut1, &xOut2, &yOut1, &yOut2);
           numWrong = getWrongChars(strResult);
//     printf("out--------------i= %d, strResult: %s, UPCflg= %d, numWrong= %d, numWrongUPC= %d, numWrongI25= %d\n", i, strResult, UPCflg, numWrong, numWrongUPC, numWrongI25);
           switch(UPCflg){
              case 1: if(numWrongUPC > numWrong) {
            	         numWrongUPC = numWrong;
            	         strcpy(strResultUPC, strResult);
                         for(k=0;k<strlen(strResult);k++)
        	    	         confUPC[k] = confR[k];
                         copyQ(&qUPC, &q);
                         angleUPC = angle;
                     }
        	         break;
              default:
            	     if(numWrongI25 > numWrong) {
            	    	 numWrongI25 = numWrong;
                        strcpy(strResultI25, strResult);
                        for(k=0;k<strlen(strResult);k++)
        	    	        confI25[k] = confR[k];
                        copyQ(&qI25, &q);
                        angleI25 = angle;
           	          }
        	          break;
           }

//           printf(" ---------- strResultI25= %s\n",strResultI25);

           if(extractField_BC_OCR(img, barCodeX[i], barCodedX[i], barCodeY[i], barCodedY[i], pix, i,
        		   xOut1, xOut2, yOut1, yOut2, angle, ticketData) >=0){
             if(test_I25_OCR(ticketData)>0 && flgI25_OCR == 0){
        		  adjustResultsI25(ticketData, strResultI25, confI25);
        		  flgI25_OCR = 1;
             }
           }

//       	printf("i= %d, strResultUPC= %s\n", i, strResultUPC);
//       	printf("i= %d, strResultI25= %s\n", i, strResultI25);
	}
	if(flgI25_OCR == 0){
		trimtResultsI25(strResultI25, confI25);
	}
//	printf(" flgI25_OCR= %d\n",flgI25_OCR);
//	fflush(stdout);

	adjustConfidenceUPC(strResultUPC, confUPC);
	adjustConfidenceI25(strResultI25, confI25);

	lpTicketResults->status = 0;
	if(numWrongUPC<2 ||numWrongI25<2)
		lpTicketResults->status = 1;
//	  printf(" ---------- qUPC->p1X= %d, qUPC->p2X= %d, qUPC->p3X= %d, qUPC->p4X= %d\n", qUPC.p1X, qUPC.p2X, qUPC.p3X, qUPC.p4X);
//	  printf(" ---------- qUPC->p1Y= %d, qUPC->p2Y= %d, qUPC->p3Y= %d, qUPC->p4Y= %d\n", qUPC.p1Y, qUPC.p2Y, qUPC.p3Y, qUPC.p4Y);
	copyBarcodedResults(lpTicketResults, strResultUPC, &qUPC, confUPC, "UPC", angleUPC, 0);
	copyBarcodedResults(lpTicketResults, strResultI25, &qI25, confI25, "I25", angleI25, 1);

//	printf("------------------- copyBarcodedResults\n");
//	fflush(stdout);

	if(imgOut2.ptr != NULL) {
		yl_free(imgOut2.ptr);
		imgOut2.ptr = NULL;
	}
	if(net_values != NULL){
		yl_free(net_values);
		net_values = NULL;
	}
//	barCPostProcessing_TX(lpTicketResults);
}

int getFirstLeftWhiteNet(LPBYTE net, int sW, int sH,  int thL, int idxL, int idxH) {
	int idxW, ccL;

	 ccL = 0;
     for(idxW=idxL;idxW<sW;idxW++){
	    if(net[idxH*sW+idxW] == 255) ccL++;
	    else {
	      if(ccL > thL)
		     return idxW;
	      ccL = 0;
	   }
	}
    return -1;
}
int getMidBlackNet(LPBYTE net, int sW, int sH,  int thM, int idxM, int idxH) {
	int idxW, ccM;

	 ccM = 0;
     for(idxW=idxM;idxW<sW;idxW++){
	    if(net[idxH*sW+idxW] == 0) ccM++;
	    else {
	      if(ccM < thM)
		     return idxW;
	      else
    	    return -idxW;
	   }
	}
     if(ccM > thM)
	     return sW;
     else
	    return -sW;
}
int getRightWhiteNet(LPBYTE net, int sW, int sH,  int thR, int idxR, int idxH) {
	int idxW, ccR;

	 ccR = 0;
     for(idxW=idxR;idxW<sW;idxW++){
	    if(net[idxH*sW+idxW] == 255) ccR++;
	    else {
	      if(ccR > thR)
		     return idxW;
	      else
    	    return -idxW;
	   }
	}
     if(ccR > thR)
	     return sW;
     else
	    return -sW;

}

void filterH(LPBYTE net, int sW, int sH, int thL, int thM, int thR){
	int idxH, idxL, idxM, idxR, idxR2, idx;
	int ok = 1;

	for(idxH=0;idxH<sH;idxH++){
		idxL = 0;idxM = 0;idxR = 0;
		while(ok>0){
			idxM = getFirstLeftWhiteNet(net, sW, sH,  thL, idxL, idxH);
//if(idxH < 10)
//  printf(" idxM= %d, idxL= %d, idxH= %d\n",idxM, idxL, idxH);
			if(idxM < 0) break;
			idxR = getMidBlackNet(net, sW, sH,  thM, idxM, idxH);

//if(idxH < 10)
//  printf(" idxM= %d, idxR= %d, idxH= %d, sW= %d\n",idxM, idxR, idxH, sW);


			if(idxR >= 0 && idxR < sW) {
				idxR2 = getRightWhiteNet(net, sW, sH,  thR, idxR, idxH);

//if(idxH < 10)
//  printf(" idxR= %d, idxR2= %d, idxH= %d, sW= %d\n",idxR, idxR2, idxH, sW);

			    if(idxR2 >= 0) {
				   for(idx=idxM;idx<idxR;idx++)
					  net[idxH*sW+idx] = 255;
			   }
			}

			if(idxR < 0)
				idxL = -idxR;
			else
				idxL = idxR;

//	if(idxH < 10)
//		  printf("next; idxL= %d, idxR= %d, idxH= %d, sW= %d\n",idxL, idxR, idxH, sW);

			if(idxL >= sW) break;
		}
	}
}


void copyPixelsFromNetToImage(LPBYTE net, int sW, int sH, LPIMGDATA img){
	int idxW, idxH;
	int  sizeH, sizeWbyte, nY;

	sizeH = img->bi.biHeight;
	sizeWbyte = ((img->bi.biWidth +31)/32)*4;


//	 printf(" copyPixelsFromNetToImage;  sizeWbyte= %d, img->bi.biWidth= %d, sizeH= %d\n",sizeWbyte, img->bi.biWidth, sizeH);
//	 fflush(stdout);


	for(idxH=0;idxH<sH;idxH++){
		for(idxW=0;idxW<sW;idxW++){
			nY = sH-idxH-1;
			setPixelOut(img->pixPtr, sizeWbyte, sizeH, idxW, idxH, net[nY*sW+idxW]);
		}
	}
}
void setPixelOut(LPBYTE ptr, int w, int h, int xinp, int yinp, int v){
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
        mask >>=(xinp%8);
        if(v!=0){
           *hptr = *hptr | mask;
         }
         else {
           *hptr = *hptr & (~mask);
        }
}

int getValueForSmearingTH(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH, int TH){
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
	if(sum > TH)
		return 255;
	return 0;
}


void JustSmearing2(LPIMGDATA imgInp, LPIMGDATA imgOut, int pix, LPBYTE *net, int *sW, int *sH){
	int ddSmearingx = pix;
	int ddSmearingy = pix;

	int sizeW, sizeH, lastW, lastH;
	int idxW, idxH;
	int gridW, gridH;
	int m;
	int biWidthPad;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int           sizeOut, sizeWbyte;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	int TH, sizeNet;
	LPBYTE lpNet;
	int nY;

//	 printf(" imgInp->bi.biWidth= %d, ddSmearingx= %d\n", imgInp->bi.biWidth, ddSmearingx);
//	 fflush(stdout);


	*sW = 0; *sH = 0;*net = NULL;

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

	sizeWbyte = ((sizeW+31)/32)*4;
	sizeOut = offBits + sizeWbyte * sizeH;

	ptrImgOut = yl_calloc("PP4", sizeOut, sizeof(BYTE));
	if(ptrImgOut == NULL) {
		printf("yl_calloc failed, ptrImgOut");
		return;
	}

	*sW = sizeW; *sH = sizeH;
	sizeNet = sizeW*sizeH;
	lpNet = yl_calloc("PP4", sizeNet, sizeof(BYTE));
	*net = lpNet;
	if(*net == NULL) {
		printf("yl_calloc failed, *net");
		return;
	}

//	 printf(" *net= %lu, lpNet= %lu, *sW= %d, *sH= %d, sizeW*sizeH= %d\n", (long unsigned)*net, (long unsigned)lpNet, *sW, *sH, sizeNet);
//	 fflush(stdout);


	bf = (LPBITMAPFILEHEADER) ptrImgOut;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, sizeOut);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, sizeW);
	set4Bytes(bi->biHeight, sizeH);
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

			TH = (5*(gridH*gridW))/7;
//			TH = (4*(gridH*gridW))/6;
			m = getValueForSmearingTH(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH, TH);

//			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
//					idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
			setPixelOut(ptrBuffOut, sizeWbyte, sizeH, idxW, idxH, m);
			nY = sizeH - idxH-1;
			lpNet[nY*sizeW+idxW] = m;

//	 printf(" idxH= %d, idxW= %d, sizeH= %d, m= %d, idxH*sizeH+idxW= %d\n", idxH, idxW, sizeW, m, idxH*sizeW+idxW);
//	 fflush(stdout);

		}
	}
	imgOut->ptr = ptrImgOut;
	imgOut->pixPtr = ptrBuffOut;
	imgOut->bi.biHeight = sizeH;
	imgOut->bi.biWidth = sizeW;

	// 	    saveBMPbuffer (ptrImgOut, "smearing.bmp", imgInp->szOutPath);
}
int testBlackEnviroment2(LPIMGDATA imgOut, int x, int y){
	int bEnv = 0;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int xx, yy, x1, x2, y1, y2;
	int  mass;
	int lenX = 5, lenY= 3;

//	char buff[500];


	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	x1=x-lenX;x2=x+lenX;
	y1=y-lenY;y2=y+lenY;
	if(x1<0) x1 = 0;if(y1<0) y1 = 0;
	if(x2<0) x2 = 0;if(y2<0) y2 = 0;
	if(x1>biWidthOut-1) x1=biWidthOut-1;
	if(x2>biWidthOut-1) x2=biWidthOut-1;
	if(y1>biHeightOut-1) y1=biHeightOut-1;
	if(y2>biHeightOut-1) y2=biHeightOut-1;
	mass=0;
	for(yy=y1;yy<y2;yy++){
		for(xx=x1;xx<x2;xx++){
			mass +=  (1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut));
	   }
	}

//	sprintf(buff," x= %d, y= %d, mass= %d\n",x, y, mass);
//	 saveLog("-----------------------------------", buff);
    if(mass > 40) bEnv = 1;
	return bEnv;
}
void getXforBarCode(LPIMGDATA imgOut, int x, int y, int *xOut1, int *xOut2){
//	int xOut = x;
	int xx, yy, x1, x2, y1, y2;
	int pix, gap, gapY, flg;
	int max, min, xLine;
	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	x1=x-35;x2=x+35;
	if(x1<0) x1 = 0;if(x2<0) x2 = 0;
	if(x1>biWidthOut-1) x1=biWidthOut-1;
	if(x2>biWidthOut-1) x2=biWidthOut-1;
	y1=y-6;y2=y+6;
	if(y1<0) y1 = 0;if(y2<0) y2 = 0;
	if(y1>biHeightOut-1) y1=biHeightOut-1;
	if(y2>biHeightOut-1) y2=biHeightOut-1;
	min = x;max = x;

//	printf(" x= %d, y= %d\n",x, y);

	gapY = 0;
	for(yy=y;yy>y1;yy--){
		gap = 0;xLine = x;flg = 0;
		for(xx=x;xx>x1;xx--){
			xLine=xx;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(min > xLine) min = xLine;
//	printf("1 xLine= %d, yy= %d, min= %d\n",xLine, yy, min);
		gap = 0;xLine = x;
		for(xx=x+1;xx<x2;xx++){
			xLine=xx;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(max < xLine) max = xLine;
		if(flg == 0) gapY++;
		else gapY = 0;
//	printf("2 xLine= %d, yy= %d, max= %d, gapY= %d\n",xLine, yy, max, gapY);
		if(gapY > 1) break;
	}
	gapY = 0;
	for(yy=y+1;yy<y2;yy++){
		gap = 0;xLine = x;flg = 0;
		for(xx=x;xx>x1;xx--){
			xLine=xx;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(min > xLine) min = xLine;
//	printf("3 xLine= %d, yy= %d, min= %d\n",xLine, yy, min);
		gap = 0;xLine = x;
		for(xx=x+1;xx<x2;xx++){
			xLine=xx;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(max < xLine) max = xLine;
		if(flg == 0) gapY++;
		else gapY = 0;
//	printf("4 xLine= %d, yy= %d, max= %d, gapY= %d\n",xLine, yy, max, gapY);
		if(gapY > 1) break;
	}
	*xOut1 = min; *xOut2 = max;
}
void getYforBarCode(LPIMGDATA imgOut, int x, int y, int *yOut1, int *yOut2){
	int xx, yy, x1, x2, y1, y2;
	int pix, gap, gapX, flg;
	int max, min, yLine;
	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	x1=x-5;x2=x+5;
	if(x1<0) x1 = 0;if(x2<0) x2 = 0;
	if(x1>biWidthOut-1) x1=biWidthOut-1;
	if(x2>biWidthOut-1) x2=biWidthOut-1;
	y1=y-20;y2=y+20;
	if(y1<0) y1 = 0;if(y2<0) y2 = 0;
	if(y1>biHeightOut-1) y1=biHeightOut-1;
	if(y2>biHeightOut-1) y2=biHeightOut-1;
	min = y;max = y;

	gapX = 0;
	for(xx=x;xx>x1;xx--){
		gap = 0;yLine = y;flg = 0;
		for(yy=y;yy>y1;yy--){
			yLine=yy;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(min > yLine) min = yLine;
		gap = 0;yLine = y;
		for(yy=y+1;yy<y2;yy++){
			yLine=yy;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(max < yLine) max = yLine;
		if(flg == 0) gapX++;
		else gapX = 0;
		if(gapX > 1) break;
	}
	gapX = 0;
	for(xx=x+1;xx<x2;xx++){
		gap = 0;yLine = y;flg = 0;
		for(yy=y;yy>x1;yy--){
			yLine=yy;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(min > yLine) min = yLine;
		gap = 0;yLine = y;
		for(yy=y+1;yy<y2;yy++){
			yLine=yy;
			pix = 1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut);
			if(pix == 0) gap++;
			else {gap = 0;flg++;}
			if(gap > 1) break;
		}
		if(max < yLine) max = yLine;
		if(flg == 0) gapX++;
		else gapX = 0;
		if(gapX > 1) break;
	}
	*yOut1 = min; *yOut2 = max;

}

int getXforBarCodeOld(LPIMGDATA imgOut, int x, int y, int dd){
	int xOut = x;
	int xx, yy, y1, y2;
	int mass, gap;
	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	y1=y-1;y2=y+1;
	if(y1<0) y1 = 0;if(y2<0) y2 = 0;
	if(y1>biHeightOut-1) y1=biHeightOut-1;
	if(y2>biHeightOut-1) y2=biHeightOut-1;
	xx = x;
	gap = 0;
	while(xx> 0 && xx < biWidthOut-1){
		mass = 0;
		for(yy=y1;yy<y2;yy++){
			mass +=  (1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut));
		}
		if(mass > 0) {xOut = xx;gap = 0;}
		else gap++;
		if(gap > 2) break;
		xx += dd;
	}
	return xOut;
}
int getYforBarCodeOld(LPIMGDATA imgOut, int x, int y, int dd){
	int yOut = y;
	int xx, yy, x1, x2;
	int mass, gap;
	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	x1=x-1;x2=x+1;
	if(x1<0) x1 = 0;if(x2<0) x2 = 0;
	if(x1>biWidthOut-1) x1=biWidthOut-1;
	if(x2>biWidthOut-1) x2=biWidthOut-1;
	yy = y;
	gap = 0;
	while(yy> 0 && yy < biHeightOut-1){
		mass = 0;
		for(xx=x1;xx<x2;xx++){
			mass +=  (1-getPixelSmearing(imgOut->pixPtr, xx, yy, biWidthPadOut, biHeightOut));
		}
		if(mass > 0) {yOut = yy;gap = 0;}
		else gap++;
		if(gap > 2) break;
		yy += dd;
	}
	return yOut;
}

int testBlackEnviroment1(LPIMGDATA imgOut, int x, int y){
	int bEnv = 0;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int i, x1, y1;
	int pix, mass, gap, maxGap;
	int len = 5;

//	char buff[500];

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	y1=y;gap=0;mass=0;
	for(i=0;i<len;i++){
		x1=x-i;if(x1<0) break;
		pix =  getPixelSmearing(imgOut->pixPtr, x1, y1, biWidthPadOut, biHeightOut);
		if(pix >0) gap++;
		mass += (1-pix);
	}
	y1=y;gap=0;
	maxGap = gap;
	for(i=0;i<len;i++){
		x1=x+i;if(x1>biWidthOut-1) break;
		pix =  getPixelSmearing(imgOut->pixPtr, x1, y1, biWidthPadOut, biHeightOut);
		if(pix >0) gap++;
		mass += (1-pix);
	}
	if(maxGap < gap) maxGap = gap;
	x1=x;gap=0;
	for(i=0;i<len;i++){
		y1=y+i;if(y1>biHeightOut-1) break;
		pix =  getPixelSmearing(imgOut->pixPtr, x1, y1, biWidthPadOut, biHeightOut);
		if(pix >0) gap++;
		mass += (1-pix);
	}
	if(maxGap < gap) maxGap = gap;
	x1=x;gap=0;
	for(i=0;i<len;i++){
		y1=y-i;if(y1<0) break;
		pix =  getPixelSmearing(imgOut->pixPtr, x1, y1, biWidthPadOut, biHeightOut);
		if(pix >0) gap++;
		mass += (1-pix);
	}
	if(maxGap < gap) maxGap = gap;


//printf(" x= %d, y= %d, mass= %d, maxGap= %d\n",x, y, mass, maxGap);
//		 sprintf(buff," x= %d, y= %d, mass= %d, maxGap= %d\n",x, y, mass, maxGap);
//		 saveLog("-----------------------------------", buff);
	if(mass > 10 && maxGap < 5) {
		bEnv = 1;
//	printf(" x= %d, y= %d, mass= %d, maxGap= %d\n",x, y, mass, maxGap);
	}
//	if(mass > 12 && maxGap < 4) {
//		bEnv = 1;
//	printf(" x= %d, y= %d, mass= %d, maxGap= %d\n",x, y, mass, maxGap);
//	}

	return bEnv;
}
int outsideTheBarCode(int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int numBc, int x, int y){
	int i, xR, yB;
	if(numBc > 0){
		for(i=0;i<numBc;i++){
			xR = barCodeX[i]+ barCodedX[i];yB = barCodeY[i]+barCodedY[i];
			if(x>=barCodeX[i] && x<=xR && y>=barCodeY[i] && y<=yB) return 0;
		}
	}
	return 1;
}
int notOverlap(int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int numBc, int xL, int xR, int yT, int yB, int overLap){
	int flgOvr = 1;
	int i;
	int xp1, xp2, yp1, yp2;
	int x1, x2, y1, y2;
	int ar, ar1, ar2, ovAr;
	int xx, yy;
    for(i=0;i<numBc;i++){
   	   xp1 = barCodeX[i];xp2 = xp1+barCodedX[i];
   	   yp1 = barCodeY[i];yp2 = yp1+barCodedY[i];
   	   ar1 =(xp2-xp1)*(yp2-yp1);
   	   ar2 =(xR-xL)*(yB-yT);
   	   ar = min(ar1, ar2);ar = ar/overLap;
   	x1 = max(xp1,xL);x2 = min(xp2,xR);
   	y1 = max(yp1,yT);y2 = min(yp2,yB);
   	xx = x2-x1; yy = y2-y1;
   	ovAr = xx*yy;
   	if(xx < 0 || yy < 0) ovAr = -100;
//printf("x1= %d, x2= %d, y1= %d, y2= %d\n",x1,x2, y1, y2);
//printf("x2-x1= %d, y2-y1= %d\n",x2-x1, y2-y1);
//printf("i= %d, ovAr= %d, ar= %d\n",i,ovAr, ar);
   	if(ovAr > ar) {flgOvr = 0;break;}
//   	if(x1 < x2 && y1 < y2){
//    		flgOvr = 0;break;
//    	}
    }
//	printf("flgOvr= %d\n",flgOvr);
//  	fflush(stdout);

	return flgOvr;
}
int  findBarcodes_SRC(LPIMGDATA imgOut, int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int overLap){
	int numBc = 0;
	int biWidthOut, biWidthPadOut, biHeightOut;
	int x, y;
	int xL, xR, yT, yB;
    int ddx, ddy;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	for(y=2;y<biHeightOut-2;y++){
		for(x=2;x<biWidthOut-2;x++){

             if(outsideTheBarCode(barCodeX, barCodedX, barCodeY, barCodedY, numBc, x, y)>0 && testBlackEnviroment1(imgOut, x, y)>0){
     			if(testBlackEnviroment2(imgOut, x, y)> 0){
            	   getXforBarCode(imgOut, x, y, &xL, &xR);
              	    getYforBarCode(imgOut, x, y, &yT, &yB);
//            	 xL = getXforBarCode(imgOut, x, y, -1);
//            	 xR = getXforBarCode(imgOut, x, y, 1);
//            	 yT = getYforBarCode(imgOut, x, y,-1);
//            	 yB = getYforBarCode(imgOut, x, y, 1);
           		    ddx = xR-xL;ddy=yB-yT;
//   		printf("1 x= %d, y= %d, xL= %d, xR= %d, yT= %d, yB= %d, ddx= %d, ddy= %d\n",x, y, xL, xR, yT, yB, ddx, ddy);
//      	fflush(stdout);

                if(ddx > 5 && ddy > 3 && (notOverlap(barCodeX, barCodedX, barCodeY, barCodedY, numBc, xL, xR, yT, yB, overLap)>0)) {
//   		printf("2 x= %d, y= %d, xL= %d, xR= %d, yT= %d, yB= %d, ddx= %d, ddy= %d\n",x, y, xL, xR, yT, yB, ddx, ddy);
//       	fflush(stdout);
//               	printf(" numBc= %d\n",numBc);
//printf(" x= %d, y= %d, xL= %d, xR= %d, yT= %d, yB= %d, ddx= %d, ddy= %d\n",x, y, xL, xR, yT, yB, ddx, ddy);

                	barCodeX[numBc] = xL;
                	barCodeY[numBc] = yT;
                	barCodedX[numBc] = ddx;
                	barCodedY[numBc] = ddy;
               	    numBc++;
               	    if(numBc >= MAX_BARCODES_NUM) break;
//               	    x = xR+2;
                }
                x = xR+2;
     		   }
             }
		}
		if(numBc >= MAX_BARCODES_NUM) break;
	}
//	printf("last;  numBc= %d\n",numBc);
//	fflush(stdout);
	return numBc;
}

int filter_BC(int *barCodeX, int *barCodedX, int *barCodeY, int *barCodedY, int numBarcodes){
	int newNum = 0;
	int i, j, flgOvr;
	int x1, x2, y1, y2;
	int xp1, xp2, yp1, yp2;
	int xn1, xn2, yn1, yn2;
    int barCodeXn[MAX_BARCODES_NUM], barCodedXn[MAX_BARCODES_NUM];
    int barCodeYn[MAX_BARCODES_NUM], barCodedYn[MAX_BARCODES_NUM];

    if(numBarcodes<=0) return numBarcodes;
    for(i=0;i<numBarcodes-1;i++){
     flgOvr = 0;
     if(barCodeX[i] < 0) continue;
     for(j=i+1;j<numBarcodes;j++){
    	if(barCodeX[j] < 0) continue;
    	xp1 = barCodeX[i];xp2 = xp1+barCodedX[i];
    	yp1 = barCodeY[i];yp2 = yp1+barCodedY[i];
    	xn1 = barCodeX[j];xn2 = xn1+barCodedX[j];
    	yn1 = barCodeY[j];yn2 = yn1+barCodedY[j];
    	x1 = max(xp1,xn1);x2 = min(xp2,xn2);
    	y1 = max(yp1,yn1);y2 = min(yp2,yn2);

    	if(x1 < x2 && abs(yp1-yn1) < 5 && abs(yp2-yn2) < 5 ){
//    	     printf(" merge: i=%d, xp1= %d, xp2= %d, yp1= %d, yp2= %d, xn1= %d, xn2= %d, yn1= %d, yn2= %d\n",i, xp1, xp2, yp1, yp2, xn1, xn2, yn1, yn2);
    	     barCodeX[i] = min(xp1,xn1); barCodedX[i] = max(xp2,xn2) - barCodeX[i];
    	     barCodeY[i] = min(yp1,yn1); barCodedY[i] = max(yp2,yn2) - barCodeY[i];
    	     barCodeX[j] = -1;
    	} else {
    	  if(x1 < x2 && y1 < y2){
     		  flgOvr = 1;break;
     	  }
    	}
     }
//     printf(" i=%d, xp1= %d, xp2= %d, yp1= %d, yp2= %d, xn1= %d, xn2= %d, yn1= %d, yn2= %d\n",i, xp1, xp2, yp1, yp2, xn1, xn2, yn1, yn2);
//     printf("  i= %d, j= %d, numBarcodes= %d, flgOvr= %d\n",i, j, numBarcodes, flgOvr);
//     printf("  x1= %d, x2= %d, y1= %d, y2= %d\n",x1, x2, y1, y2);
//     fflush(stdout);

 	if(flgOvr == 0){
 		barCodeXn[newNum] = barCodeX[i];
 		barCodedXn[newNum] = barCodedX[i];
 		barCodeYn[newNum] = barCodeY[i];
 		barCodedYn[newNum] = barCodedY[i];
 		newNum++;
 	}

    }
    i = numBarcodes-1;
	barCodeXn[newNum] = barCodeX[i];
	barCodedXn[newNum] = barCodedX[i];
	barCodeYn[newNum] = barCodeY[i];
	barCodedYn[newNum] = barCodedY[i];
	newNum++;
    for(i=0;i<newNum;i++){
    	barCodeX[i] = barCodeXn[i];
    	barCodedX[i] = barCodedXn[i];
    	barCodeY[i] = barCodeYn[i];
    	barCodedY[i] = barCodedYn[i];
    }
	return newNum;
}
float getBarcodeAngle(LPBYTE pixPtr, int biWidthPad, int biHeight, int x1, int x2, int y1, int y2){
	float angle = 0;
	int *ptrH;
	int xx1, xx2, yy1, yy2;
	int i;
    int gap;
    int k, w;
    int xL, yL, xR, yR;
    float tg;

//	char buff[500];

   w = biWidthPad*8;

// left side
	xx1 = x1-10; xx2 = x1+40; yy1 = y2-40; yy2 = y2+10;
	if(xx1<0) xx1 = 0;
	if(yy1<0) yy1 = 0;
	if(xx2 >= w) xx2 = w-10;
	if(yy2 >= biHeight) yy2 = biHeight-1;
	histHTest_X1_X1(pixPtr, biWidthPad, biHeight,  xx1, xx2, yy1, yy2, hHistPreProc);
	ptrH = hHistPreProc;

//	    sprintf(buff,"-----------------------\n\n");
//	 	saveLog("", buff);
//	 	       	for(i=1;i<=ptrH[0];i++) {
//	 	       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
//	 	       	 	saveLog("", buff);
//	 	       	}

	k=ptrH[0];gap = 0;
    for(i=1;i<=ptrH[0];i++) {
    	k = i;
    	if(ptrH[i] < 1) gap++;
    	else gap= 0;
    	if(gap > 1) break;
    }
  	yL = k-gap;

	xx1 = x1-10; xx2 = x1+40; yy1 = y2-40; yy2 = y2;
	if(xx1<0) xx1 = 0;
	if(yy1<0) yy1 = 0;
	if(xx2 >= w) xx2 = w-10;
	if(yy2 >= biHeight) yy2 = biHeight-1;
    histVPtrPix(pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, xx1, xx2, yy1, yy2, vHistPreProc);
    ptrH = vHistPreProc;
	k=1;gap = 0;
    for(i=1;i<=ptrH[0];i++) {
    	k = i;
    	if(ptrH[i] > 2) gap++;
    	else gap= 0;
    	if(gap > 3) break;
    }
  	xL = k-gap;

//   printf("   xL= %d, yL= %d\n",xL, yL);

// right side
	xx1 = x2-40; xx2 = x2+10; yy1 = y2-40; yy2 = y2+10;
	if(xx1<0) xx1 = 0;
	if(yy1<0) yy1 = 0;
	if(xx2 >= w) xx2 = w-10;
	if(yy2 >= biHeight) yy2 = biHeight-1;
	histHTest_X1_X1(pixPtr, biWidthPad, biHeight,  xx1, xx2, yy1, yy2, hHistPreProc);
	ptrH = hHistPreProc;

//	    sprintf(buff,"-----------------------\n\n");
//	 	saveLog("", buff);
//	 	       	for(i=1;i<=ptrH[0];i++) {
//	 	       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
//	 	       	 	saveLog("", buff);
//	 	       	}

	k=ptrH[0];gap = 0;
    for(i=1;i<=ptrH[0];i++) {
   	   k = i;
   	   if(ptrH[i] < 1) gap++;
   	   else gap= 0;
   	   if(gap > 1) break;
    }
 	yR = k-gap;
// 	printf("   yR= %d, k= %d, gap= %d\n",yR, k, gap);

	xx1 = x2-40; xx2 = x2+10; yy1 = y2-40; yy2 = y2;
	if(xx1<0) xx1 = 0;
	if(yy1<0) yy1 = 0;
	if(xx2 >= w) xx2 = w-10;
	if(yy2 >= biHeight) yy2 = biHeight-1;

    histVPtrPix(pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, xx1, xx2, yy1, yy2, vHistPreProc);
    ptrH = vHistPreProc;
	k=ptrH[0];gap = 0;
    for(i=ptrH[0];i>=0;i--) {
   	   k = i;
   	   if(ptrH[i] > 2) gap++;
   	   else gap= 0;
   	   if(gap > 3) break;
    }
    xR = x2-40-(x1-10)+1+k+gap;
//    printf("   xR= %d, yR= %d\n",xR, yR);
    tg = 0;
    if(xR-xL > 0)
        tg = (float)(yL-yR)/(float)(xR-xL);

    angle = (float)atan((double)tg);
    angle = (angle*180)/PI;

//    printf("   tg= %e, angle= %e\n",tg, angle);

	return angle;
}
void adjustYcoordinates_2(LPBYTE pixPtr, int biWidthPad, int biHeight, int x1, int x2, int *y1, int *y2){
	int *ptrH;
	int i;
    int xx1, xx2, yy1, yy2;
    int xC, xC1, xC2;
    int gap1, max, iMax;
    int k1, k2, cc;
    int linesTab1[10], linesTab2[10], numLines;
//	char buff[500];

//    sprintf(buff,"-----------------------\n\n");
// 	saveLog("", buff);
//    printf("-----------------------\n\n");

	xx1 = x1; xx2 = x2; yy1 = *y1; yy2 = *y2;
//  printf("   yy1= %d, yy2= %d\n",yy1, yy2);

	xC= (xx2+xx1)/2;
	xC1= xC - xC/4;
	xC2= xC + xC/4;

//printf("   xx1= %d, xx2= %d, xC1= %d, xC2= %d\n",xx1, xx2, xC1, xC2);

    histHTest_X1_X1(pixPtr, biWidthPad, biHeight,  xC1, xC2, yy1, yy2, hHistPreProc);
    ptrH = hHistPreProc;
    for(i=1;i<=ptrH[0]-2;i++) {
    	ptrH[i] = (ptrH[i] +ptrH[i+1]+ptrH[i+2])/3;
    }
    ptrH[0] = ptrH[0]-2;

    k1 = 1;numLines= 0;
    while(k1<=ptrH[0]) {
    	if(numLines<10)
    	  linesTab1[numLines] = k1;
    	k2=k1;gap1 = 0;
        for(i=k1;i<=ptrH[0];i++) {
        	if(ptrH[i] > 1) gap1++;
        	else {gap1= 0;k2=i;}
        	if(gap1 > 3) break;
        }
    	if(numLines<10)
           linesTab2[numLines] = k2;
        numLines++;
        k1=k2+1;
        if(k1 > ptrH[0]) break;
    	if(numLines<10)
    	   linesTab1[numLines] = k1;
    	k2=k1;gap1 = 0;
        for(i=k1;i<=ptrH[0];i++) {
        	if(ptrH[i] < 2) gap1++;
        	else {gap1= 0;k2=i;}
        	if(gap1 > 2) break;
        }
    	if(numLines<10)
           linesTab2[numLines] = k2;
        numLines++;
        k1=k2+1;
//      printf("   k1= %d\n",k1);
    }
//printf("   numLines= %d\n",numLines);
    if(numLines<2 || numLines>=10) return;
    max = 0; iMax = -1;
    for(i=1;i<numLines;i += 2){
    	cc = linesTab2[i]-linesTab1[i];
    	if(max < cc) {max = cc;iMax = i;}
    }
//  printf("   iMax= %d, max= %d\n",iMax, max);
    if(iMax < 0 || max < 50) return;

    *y1 = yy1+linesTab1[iMax];
    *y2= yy1+linesTab2[iMax];
//  printf("exit   *y1= %d, *y2= %d\n",*y1, *y2);
}
int adjustYcoordinates(LPBYTE pixPtr, int biWidthPad, int biHeight, int x1, int x2, int *y1, int *y2){
	int *ptrH;
	int i;
    int xx1, xx2, yy1, yy2;
    int xC, xC1, xC2;
    int gap1, max, iMax;
    int k1, k2, cc;
    int start, stop;
    float ratio;
    int linesTab1[10], linesTab2[10], numLines;
//	char buff[500];

//    sprintf(buff,"-----------------------\n\n");
// 	saveLog("", buff);
//    printf("-----------------------\n\n");

	xx1 = x1; xx2 = x2; yy1 = *y1; yy2 = *y2;
//  printf("   yy1= %d, yy2= %d\n",yy1, yy2);

	xC= (xx2+xx1)/2;
	xC1= xC - xC/4;
	xC2= xC + xC/4;

//printf("   xx1= %d, xx2= %d, xC1= %d, xC2= %d\n",xx1, xx2, xC1, xC2);

    histHTest_X1_X1(pixPtr, biWidthPad, biHeight,  xC1, xC2, yy1, yy2, hHistPreProc);
    ptrH = hHistPreProc;
    for(i=1;i<=ptrH[0]-2;i++) {
    	ptrH[i] = (ptrH[i] +ptrH[i+1]+ptrH[i+2])/3;
    }
    ptrH[0] = ptrH[0]-2;
//       	for(i=1;i<=ptrH[0];i++) {
//       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
//       	 	saveLog("", buff);
//       	}
//   printf("   ptrH[0] %d\n",ptrH[0]);

    k1 = 1;numLines= 0;
    while(k1<=ptrH[0]) {
    	if(numLines<10)
    	  linesTab1[numLines] = k1;
    	k2=k1;gap1 = 0;
        for(i=k1;i<=ptrH[0];i++) {
        	if(ptrH[i] > 1) gap1++;
        	else {gap1= 0;k2=i;}
        	if(gap1 > 3) break;
        }
    	if(numLines<10)
           linesTab2[numLines] = k2;
        numLines++;
        k1=k2+1;
        if(k1 > ptrH[0]) break;
    	if(numLines<10)
    	   linesTab1[numLines] = k1;
    	k2=k1;gap1 = 0;
        for(i=k1;i<=ptrH[0];i++) {
        	if(ptrH[i] < 2) gap1++;
        	else {gap1= 0;k2=i;}
        	if(gap1 > 2) break;
        }
    	if(numLines<10)
           linesTab2[numLines] = k2;
        numLines++;
        k1=k2+1;
//      printf("   k1= %d\n",k1);
    }
//printf("   numLines= %d\n",numLines);
    if(numLines<2 || numLines>=10) return -1;

//  printf("   numLines= %d\n",numLines);
//  for(i=0;i<numLines;i++){
//	  printf("   linesTab1[ %d ]= %d, linesTab2[ %d ]= %d\n",i, linesTab1[i], i, linesTab2[i]);
//  }

    max = 0; iMax = -1;
    for(i=1;i<numLines;i += 2){
    	cc = linesTab2[i]-linesTab1[i];
    	if(max < cc) {max = cc;iMax = i;}
    }
//  printf("   iMax= %d, max= %d\n",iMax, max);

    if(iMax < 0 || max < 50) return -1;

//    *y1 = yy1+k1;
    cc = 0;
    start = linesTab1[iMax]+4;stop = linesTab2[iMax]-4;
    for(i=start;i<stop;i++) {
    	ratio = 0;
    	if(ptrH[i+1] >0){
    		ratio = (float)ptrH[i]/(float)ptrH[i+1];
    	}
    	if(ratio < 0.55 || ratio > 2.5) cc++;
    }
//printf("   cc= %d\n",cc);
    if(cc > 10) return -1;
    *y1 = yy1+linesTab1[iMax];
    *y2= yy1+linesTab2[iMax];
//        printf("   *y1= %d, *y2= %d, cc = %d\n",*y1, *y2, cc);
    return 1;
}

int adjustXcoordinates(LPBYTE pixPtr, int biWidthPad, int biHeight, int *x1, int *x2, int y1, int y2){
	int *ptrH;
	int i;
    int xx1, xx2, yy1, yy2;
    int gap1;
    int k, k1, k2, kk, dd, ddY, shY;
    int start, stop, cc;
    float ratio;

//	char buff[500];

//    sprintf(buff,"-----------------------\n\n");
// 	saveLog("", buff);


	xx1 = *x1; xx2 = *x2; yy1 = y1; yy2 = y2;
	ddY = yy2-yy1;shY = 10;
	if(ddY > 80) shY = 20;
//  printf("   xx1= %d, xx2= %d\n",xx1, xx2);

// inside
	if(ddY > 25){
	    histVPtrPix(pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, xx1, xx2, yy1+shY, yy2-shY, vHistPreProc);
	    ptrH = vHistPreProc;
	    for(i=1;i<=ptrH[0]-2;i++) {
	    	ptrH[i] = (ptrH[i] +ptrH[i+1]+ptrH[i+2])/3;
	    }
	    ptrH[0] = ptrH[0]-2;

	//    	for(i=1;i<=ptrH[0];i++) {
	//    	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
	//   	 	saveLog("", buff);
	//    	}

	    k1 = 1;
	    gap1 = 0;k = 1;
	    for(i=1;i<=ptrH[0];i++) {
	    	k = i;
	    	if(ptrH[i] > 1) gap1++;
	    	else gap1= 0;
	    	if(gap1 > 3) break;
	    }
	    k1 = k - gap1;
	//  printf("   k1= %d\n",k1);
	    k2 = 1;
	    gap1 = 0;k = ptrH[0];
	    for(i=ptrH[0];i>0;i--) {
	    	k = i;
	    	if(ptrH[i] > 1) gap1++;
	    	else gap1= 0;
	    	if(gap1 > 3) break;
	    }
	    kk = k+gap1;
	    if(kk >= ptrH[0])
	    	k2 = 0;
	    else
	       k2 = ptrH[0]-kk;

	//   printf("   k2= %d\n",k2);
	    *x1= xx1+k1;
	    *x2= xx2-k2;
	    if(*x2-*x1 < 20) return -1;
//  printf("inside   *x1= %d, *x2= %d\n",*x1, *x2);

	  cc = 0;
	  start = k1;stop = ptrH[0]-k2;
	  for(i=start;i<stop;i++) {
	  	ratio = 0;
	  	if(ptrH[i+1] >0){
	  		ratio = (float)ptrH[i]/(float)ptrH[i+1];
	  	}
	//    printf("   ratio= %e\n",ratio);
	  	if(ratio < 0.55 || ratio > 2.5) cc++;
	  }
//	  printf("   cc= %d\n",cc);
	  if(cc < 15) return -1;
	}
	xx1 = *x1; xx2 = *x2;
// left side
		    dd = xx1+10;
		    if(dd > 300) dd = 300;
//			printf("L   ddY= %d\n",ddY);
			if(dd > 50 && ddY > 25) {
			    histVPtrPix(pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, xx1-dd, xx1, yy1+shY, yy2-shY, vHistPreProc);
			    ptrH = vHistPreProc;
			    for(i=1;i<=ptrH[0]-2;i++) {
			    	ptrH[i] = (ptrH[i] +ptrH[i+1]+ptrH[i+2])/3;
			    }
			    ptrH[0] = ptrH[0]-2;

//			      	for(i=1;i<=ptrH[0];i++) {
//			       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
//			       	 	saveLog("", buff);
//			        	}
			    gap1 = 0;k = 1;
			    for(i=ptrH[0];i>0;i--) {
			    	k = i;
			    	if(ptrH[i] < 2) gap1++;
			    	else gap1= 0;
			    	if(gap1 > 15) break;
			    }
//			    printf("L   k= %d, ptrH[0]= %d, gap1= %d\n",k, ptrH[0], gap1);
			    k1 = ptrH[0]-(k+gap1-1);
			    *x1= xx1-k1;
			}
//printf("L   *x1= %d, *x2= %d\n",*x1, *x2);
// right side
	    dd = biWidthPad*8-xx2-10;
	    if(dd > 300) dd = 300;
	//  printf("R   dd= %d\n",dd);
		if(dd > 50 && ddY > 25) {
		    histVPtrPix(pixPtr, biWidthPad, biHeight, H_SIZE_PREPROC, xx2, xx2+dd, yy1+shY, yy2-shY, vHistPreProc);
		    ptrH = vHistPreProc;
		    for(i=1;i<=ptrH[0]-2;i++) {
		    	ptrH[i] = (ptrH[i] +ptrH[i+1]+ptrH[i+2])/3;
		    }
		    ptrH[0] = ptrH[0]-2;

//		    		      	for(i=1;i<=ptrH[0];i++) {
//		    		       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
//		    		       	 	saveLog("", buff);
//		    		        	}

		    k2 = 1;
		    gap1 = 0;k = 1;
		    for(i=1;i<=ptrH[0];i++) {
		    	k = i;
		    	if(ptrH[i] < 2) gap1++;
		    	else gap1= 0;
		    	if(gap1 > 15) break;
		    }
		    k2 = k - gap1+1;
		    if(k2 > 1) {
		    	*x2= xx2+k2;
	//	    	  printf("2   *x1= %d, *x2= %d\n",*x1, *x2);
		    }
		}
//printf("R   *x1= %d, *x2= %d\n",*x1, *x2);


    return 1;
}

int testUPCresults(char *strResult){
	int i, len = strlen(strResult);
	for(i=0;i<len;i++){
		if(strResult[i] == '?') return -1;
	}
   return 1;
}
void extractField_BC(LPIMGDATA imgOut, int x, int dx, int y, int dy, int pix, int idx,
		   char *strResultOut, QUADRANGLE *q, int *confROut, float *angleOut, int *UPCflg,
		   int *xOutG1, int *xOutG2, int *yOutG1, int *yOutG2){
	int err, ret;
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
//    int angleTest;
    int biWidthOut, biWidthPadOut, biHeightOut;
    int  biSizeImage, biWidthPad;
    int x1, x2, y1, y2;
    int ddx, ddy;
	int xOut1, xOut2, yOut1, yOut2;

//    char imgName[100];

    *angleOut = 0;
    strResultOut[0] = 0;
    *UPCflg = 1;
    q->p1X = 0;q->p1Y = 0;q->p2X = 0;q->p2Y = 0;
    q->p3X = 0;q->p3Y = 0;q->p4X = 0;q->p4Y = 0;

	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;

	ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
	if (ptr == NULL) {
		return;     // out of memory
	}
	hpFieldH.ptr = ptr;
	hpFieldH.size = 8;
	offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	lpImage = (LPDWORD) ptr;
	*lpImage = 0;
	*(lpImage + 1) = offBits;


    x1 = x;x2 = x1+dx;y1 = y;y2 = y1+dy;
    x2++; y2++;
    x1 *=pix;x2 *=pix;y1 *=pix;y2 *=pix;
    x1 -= 10;x2 += 10;
    y1 -= 8;y2 += 8;

    angle = 0;

    if(x1>=biWidthOut-1) x1 = biWidthOut-1;if(x2>=biWidthOut-1) x2 = biWidthOut-1;
    if(y1>=biHeightOut-1) y1 = biHeightOut-1;if(y2>=biHeightOut-1) y2 = biHeightOut-1;

    ret = adjustYcoordinates(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, &y1, &y2);
    if(ret < 0) return;
    ret = adjustXcoordinates(imgOut->pixPtr, biWidthPadOut, biHeightOut, &x1, &x2, y1, y2);
    if(ret < 0) return;

    x1 -=4; x2 += 4;
    y1 -=2; y2 += 2;

    if(x1>=biWidthOut-1) x1 = biWidthOut-1;if(x2>=biWidthOut-1) x2 = biWidthOut-1;
    if(y1>=biHeightOut-1) y1 = biHeightOut-1;if(y2>=biHeightOut-1) y2 = biHeightOut-1;

    ddx = x2-x1; ddy = y2-y1;
//    printf("   ddx= %d, ddy= %d\n", ddx, ddy);
    if(ddy > 0 && (float)ddx/(float)ddy > 3){
    	angle = getBarcodeAngle(imgOut->pixPtr, biWidthPadOut, biHeightOut, x1, x2, y1, y2);
//	printf("   angle= %e\n", angle);
    }
err =
		extractField(imgOut->pixPtr, imgOut->bi.biWidth, imgOut->bi.biHeight,
				x1, y1,
				x2, y1,
				x1, y2,
				&hpFieldH,
				&width,
				&height,
				8,
				"bc",
				angleFlag,
				&angle
		);
//printf("------------ err= %d\n", err);
//fflush(stdout);

if(err < 0) return;
    *angleOut = angle;
     xOut1 = x1; xOut2 = x2;
     yOut1 = y1; yOut2 = y2;
     *xOutG1 = x1; *xOutG2 = x2;
     *yOutG1 = y1; *yOutG2 = y2;

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
   set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
   set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
   set4Bytes(bi->biClrUsed, 0);
   set4Bytes(bi->biClrImportant, 0);

// generate pair of RGBQUADs (black and white)
   qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
   *(LPDWORD)qd       = 0x00000000;
   *(LPDWORD)(qd + 1) = 0x00ffffff;

//   	setNewImageName(imgName, imgOut->szIMGName, idx+10);
//   saveBMPbuffer (hpFieldH.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
   {
	   int byteWidthHist=    ((width + 31) /32) * 4;
		MASKDEF barMask[20];
		char strResult[64];
		char strResult2[64];
		int  confR[64];
		int  confR2[64];
		char cand2[64];
		int i, ddY, ddYY;

//	printf("----byteWidthHist= %d, width= %d, height= %d, offBits= %d\n", byteWidthHist, width, height, offBits);
		y1 = 0; y2 = height-1;
		strResult[0] = 0;
		ret = -1;
        ddYY = 15;
        ddY = y2-y1;
		if(height > 40) {
           adjustYcoordinates_2(hpFieldH.ptr+offBits, byteWidthHist, height, 10, width-10, &y1, &y2);
           yOut1 += y1; yOut2 -= (height-1 - y2);
           histVPtrPix(hpFieldH.ptr+offBits, byteWidthHist, height, width-1, 2, width-2, y1+15, y2-15, vHistPreProc);

/*
   {int nnH;
     char bufLog[500];
    sprintf(bufLog,"--------------------------------------------\n\n");
    saveLog("", bufLog);
      for(nnH=1;nnH<=vHistPreProc[0];nnH++){
   	   sprintf(bufLog,"vHist[ %d ]= %d\n",nnH, vHistPreProc[nnH]);
   	   saveLog("", bufLog);
      }
   }
*/
         for(i=0;i<20;i++)
           barMask[i].info = 128;

         ddYY = 15;
         ddY = y2-y1;
//           printf("--------------ddY=  %d\n", ddY);
         ret = barDeCodeI25 (&vHistPreProc[1], vHistPreProc[0], (LPMASK)&barMask, 20, 20, 2, strResult, confR, cand2);
	  }
      if(strlen(strResult)<12) ret = -1;
      if(ret == 0 || ret == 1){
         yOut1 -= 80;
         yOut2 -= 60;
         if(yOut1<0) yOut1 = 0;
         if(yOut2<2) yOut2 = 2;
         q->p1X = xOut1;q->p1Y = yOut1;q->p2X = xOut2;q->p2Y = yOut1;
         q->p3X = xOut2;q->p3Y = yOut2;q->p4X = xOut1;q->p4Y = yOut2;
         strcpy(strResultOut, strResult);
         for(i=0;i<strlen(strResult);i++) {
//        	    printf("I25--------------confR[ %d ]=  %d\n", i, confR[i]);
	    	 confROut[i] = confR[i];
	     }
         *UPCflg = 0;
//         printf("--------------idx= %d, strResult: %s, UPCflg= %d\n", idx, strResult, *UPCflg);
   }else{
	   if(ddY > 70)
		   ddYY = 30;
	   if(ddY > 170)
		   ddYY = 50;

//        printf("--------------ddYY=  %d\n", ddYY);
//        ddYY = 60;
	   histVPtrPix(hpFieldH.ptr+offBits, byteWidthHist, height, width-1, 2, width-2, y1+ddYY, y2-ddYY, vHistPreProc);
/*
	   {int nnH;
	     char bufLog[500];
	    sprintf(bufLog,"--------------------------------------------\n\n");
	    saveLog("", bufLog);
	      for(nnH=1;nnH<=vHistPreProc[0];nnH++){
	   	   sprintf(bufLog,"vHist[ %d ]= %d\n",nnH-1, vHistPreProc[nnH]);
	   	   saveLog("", bufLog);
	      }
	   }
*/
	   barDeCodeUPC (&vHistPreProc[1], vHistPreProc[0], 64, strResult, confR);
 //printf("strResult= %s\n", strResult);
	   if(testUPCresults(strResult) < 0) {
		   int yCC; // = (y1+y2)/2;
		     for(i=0;i<strlen(strResult);i++)
		    	 confR2[i] = confR[i];
		     strcpy(strResult2,strResult);

		     for(i=0;i<15;i++) {
		    	 strResult[i] = '?';
		    	 confR[i] = 0;
		     }
		     strResult[1] = ' ';
		     strResult[13] = ' ';
		     strResult[7] = ' ';
		     strResult[15] = 0;

             ddYY = 30;
//     printf("--------------y1= %d, y2= %d, ddYY=  %d\n", y1, y2, ddYY);
             yCC = y1 + ddYY;
		     while(yCC < y2-ddYY){
			    histVPtrPix(hpFieldH.ptr+offBits, byteWidthHist, height, width-1, 2, width-2, yCC, yCC+2, vHistPreProc);
				barDeCodeUPC2 (&vHistPreProc[1], vHistPreProc[0], 64, strResult, confR);

//printf(" yCC= %d, strResult= %s\n", yCC, strResult);
			    if(testUPCresults(strResult) >= 0) break;
			    yCC += 5;
		     }

		     for(i=0;i<strlen(strResult);i++) {
		    	 if(strResult[i] == '?' && strResult2[i] != '?') {strResult[i] = strResult2[i];confR[i] = confR2[i];}
		    	 else { confR[i] = 100 - confR[i]*50;if(confR[i] < 0) confR[i] = 0;}
		     }
//   printf("slice  strResult= %s\n", strResult);
	   }
	      yOut1 += 60;
	      yOut2 += 60;
	      if(yOut1>=imgOut->bi.biHeight-2) yOut1 = imgOut->bi.biHeight-2;
	      if(yOut2>=imgOut->bi.biHeight-1) yOut2 = imgOut->bi.biHeight-1;
	      q->p1X = xOut1;q->p1Y = yOut1;q->p2X = xOut2;q->p2Y = yOut1;
	      q->p3X = xOut2;q->p3Y = yOut2;q->p4X = xOut1;q->p4Y = yOut2;
	      strcpy(strResultOut, strResult);
	      for(i=0;i<strlen(strResult);i++) {
		    	 confROut[i] = confR[i];
//		    	 printf("UPC--------------confR[ %d ]=  %d\n", i, confR[i]);
		  }
	      *UPCflg = 1;
//	      printf("--------------idx= %d, strResult: %s, UPCflg= %d\n", idx, strResult, *UPCflg);
     }
 }
   if(hpFieldH.ptr != NULL) yl_free(hpFieldH.ptr);
}

int extractField_BC_OCR(LPIMGDATA imgOut, int x, int dx, int y, int dy, int pix, int idx,
		                 int xOut1, int xOut2, int yOut1, int yOut2, double angle, LPTICKETDATA ticketData){
	int ret;
    int  biWidthPadOut, biHeightOut;
    int x1, x2, y1, y2;
//    int *ptrH;
//    long angleH;
//    float tG;
//#define    SCL 10000
//    int ddx, ddy;

//    char imgName[100];

//	biWidthOut=imgOut->bi.biWidth;
	biWidthPadOut = ((imgOut->bi.biWidth+31)/32)*4;
	biHeightOut = imgOut->bi.biHeight;


    x1 = xOut1-5; x2 = xOut2;
    y1 = yOut1; y2 = yOut2;

   if(x1 < 0 || x1 >= imgOut->bi.biWidth) return -1;
   if(x2 < 0 || x2 >= imgOut->bi.biWidth) return -1;
   if(x1 >= x2) return -1;
   if(y1 < 0 || y1 >= biHeightOut) return -1;
   if(y2 < 0 || y2 >= biHeightOut) return -1;
   if(y1 >= y2) return -1;

/*
    angleH=(long)(angle*SCL);
    tG = (float)tan((angle*PI)/180.0);
//    printf("   x1= %d, x2= %d, y1= %d, y2= %d, angle= %e\n", x1, x2, y1, y2, angle);
//printf("----before");fflush(stdout);
    histH_alpha2(imgOut->pixPtr, biWidthPadOut, biHeightOut,
  		         x1, y1-80, x2, y1+40,
                 angleH,tG, &hHistPreProc[1], &hHistPreProc[0]);

//   printf("----after");fflush(stdout);
//	histHTest_X1_X1(imgOut->pixPtr, biWidthPadOut, biHeightOut,  x1, x2, y1-60, y1+10, hHistPreProc);
	ptrH = hHistPreProc;

	{int i;char buff[500];
	    sprintf(buff,"-----------------------\n\n");
	 	saveLog("", buff);
	 	       	for(i=1;i<=ptrH[0];i++) {
	 	       	    sprintf(buff,"---ptrH[ %d ]= %d\n",i, ptrH[i]);
	 	       	 	saveLog("", buff);
	 	       	}

	}
*/
//    y2 = y1-6;y1 -= 60;
	y2 = y1; y1 -= 60;
    if(y1<0) y1 = 0;if(y2<0) y2 = 0;
    if(x1<0) x1 = 0;
//   printf("   x1= %d, x2= %d, y1= %d, y2= %d, angle= %e\n", x1, x2, y1, y2, angle);

    ret =  getOCRforBC(imgOut, ticketData, x1, x2, y1, y2, angle, idx);
    return ret;
}

int getWrongChars(char *strR){
	int cc = 0, i, len;
	len = strlen(strR);
	for(i=0;i<len;i++){
		if(strR[i]=='?') cc++;
	}
	cc += (15-len);
	return cc;
}
int getWrongChars_2(char *strR){
	int cc = 0, i, len;
	len = strlen(strR);
	for(i=0;i<len;i++){
		if(strR[i]=='?') cc++;
	}
//	cc += (15-len);
	if(len<15) cc++;
	return cc;
}

void copyQ(QUADRANGLE *qOut, QUADRANGLE *qInp) {
      qOut->p1X = qInp->p1X;qOut->p1Y = qInp->p1Y;qOut->p2X = qInp->p2X;qOut->p2Y = qInp->p2Y;
      qOut->p3X = qInp->p3X;qOut->p3Y = qInp->p3Y;qOut->p4X = qInp->p4X;qOut->p4Y = qInp->p4Y;
}


void copyBarcodedResults(LPTICKETRESULTS ticketResults, char *strResult, QUADRANGLE *q, int *conf, char *name, float angle, int idx){
	int i, len;
	LPFIELDSRESULTS lpFresults;

	if(idx < NUMFIELDS_RESULTS) {
		len = strlen(strResult);
		strcpy(ticketResults->fieldsResults[idx].name, name);
		strcpy(ticketResults->fieldsResults[idx].result, strResult);
		for(i=0;i<len;i++){
			ticketResults->fieldsResults[idx].flConf[i] = conf[i];
		}
		copyQuad(ticketResults->fieldsResults[idx].flQuad, q, 0, len-1);
//		copyLineQuads(&ticketResults->fieldsResults[idx], ticketDataPtr);
		lpFresults = &ticketResults->fieldsResults[idx];
		lpFresults->angle = angle;
		lpFresults->lineQuadStatus[0] = 1;
		lpFresults->lineQuad[0].p1X = q->p1X;
		lpFresults->lineQuad[0].p2X = q->p2X;
		lpFresults->lineQuad[0].p3X = q->p3X;
		lpFresults->lineQuad[0].p4X = q->p4X;
		lpFresults->lineQuad[0].p1Y = q->p1Y;
		lpFresults->lineQuad[0].p2Y = q->p2Y;
		lpFresults->lineQuad[0].p3Y = q->p3Y;
		lpFresults->lineQuad[0].p4Y = q->p4Y;
		for(i=1;i<NUM_LINESTATUS;i++){
			lpFresults->lineQuadStatus[i] = 0;
			lpFresults->lineQuad[i].p1X = 0;
			lpFresults->lineQuad[i].p2X = 0;
			lpFresults->lineQuad[i].p3X = 0;
			lpFresults->lineQuad[i].p4X = 0;
			lpFresults->lineQuad[i].p1Y = 0;
			lpFresults->lineQuad[i].p2Y = 0;
			lpFresults->lineQuad[i].p3Y = 0;
			lpFresults->lineQuad[i].p4Y = 0;
		}

	}
}
void adjustConfidenceUPC(char *strResult, int *conf){
	int i, len = strlen(strResult);
	for(i=0;i<len;i++){
		if(strResult[i] == '?') conf[i] = 0;
		if(conf[i] >= 20 && conf[i] < 100) conf[i] = 15;
	}
}
void adjustConfidenceI25(char *strResult, int *conf){
	int i, len = strlen(strResult);
	for(i=0;i<len;i++){
		if(strResult[i] == '?') conf[i] = 0;
		if(conf[i] > 10) conf[i] += 40;
		if(conf[i] > 100) conf[i] = 100;
	}
}

int testConfidence_BC(char *strResult, int *conf, int th){
	int i, len = strlen(strResult);
	for(i=0;i<len;i++){
		if(conf[i] < th) return -1;
	}
	return 1;
}

void barCPostProcessing_TX(LPTICKETRESULTS ticketResults){
	int i;

//	  printf ("Results before post-processing----------------------------------------\n");
//	  saveLog("Results before post-processing----------------------------------------", "\n");
//		printResults_1(ticketResults);
//	 printf ("-----------------------------------------------------------------------\n");
//	 saveLog("-----------------------------------------------------------------------", "\n");
//	fflush(stdout);

    ticketResults->type = ticketTypeInt;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		printf ("-----------------------------------------------------------------------\n");
    		printf (" name= %s\n",ticketResults->fieldsResults[i].name);
    		printf (" result= %s\n",ticketResults->fieldsResults[i].result);

    	}
    }
}
int testResultsBC(){
	int i, len;
	len = strlen(strGResultI25);
	if(len<14) return -1;
	for(i=0;i<len;i++)
	    if(strGResultI25[i] == '?') return -1;

	len = strlen(strGResultUPC);
	if(len<12) return -1;
	for(i=0;i<len;i++)
	    if(strGResultUPC[i] == '?') return -1;

	return 0;
}
void copyAllTicketResultsGUPC(LPTICKETRESULTS ticketResults, int idx){
	int i, len;;
	LPFIELDSRESULTS lpFresults = &ticketResults->fieldsResults[idx];
	int x1, x2, y1, y2;

	strcpy(lpFresults->result, strGResultUPC);
	len = strlen(strGResultUPC);
	for(i=0;i<len;i++){
		lpFresults->flConf[i] = cnfGResultUPC[i];
		lpFresults->flQuad[i].p1X = qGResultUPC[i].p1X;
		lpFresults->flQuad[i].p1Y = qGResultUPC[i].p1Y;
		lpFresults->flQuad[i].p2X = qGResultUPC[i].p2X;
		lpFresults->flQuad[i].p2Y = qGResultUPC[i].p2Y;
		lpFresults->flQuad[i].p3X = qGResultUPC[i].p3X;
		lpFresults->flQuad[i].p3Y = qGResultUPC[i].p3Y;
		lpFresults->flQuad[i].p4X = qGResultUPC[i].p4X;
		lpFresults->flQuad[i].p4Y = qGResultUPC[i].p4Y;
	}
	lpFresults->angle = 0;
	lpFresults->lineQuadStatus[0] = 1;
	x1 = qGResultUPC[0].p1X;
	x2 = qGResultUPC[len-1].p2X;
	y1 = qGResultUPC[0].p1Y;
	y2 = qGResultUPC[len-1].p3Y;
	lpFresults->lineQuad[0].p1X = x1;
	lpFresults->lineQuad[0].p2X = x2;
	lpFresults->lineQuad[0].p3X = x2;
	lpFresults->lineQuad[0].p4X = x1;
	lpFresults->lineQuad[0].p1Y = y1;
	lpFresults->lineQuad[0].p2Y = y1;
	lpFresults->lineQuad[0].p3Y = y2;
	lpFresults->lineQuad[0].p4Y = y2;
	for(i=1;i<NUM_LINESTATUS;i++){
		lpFresults->lineQuadStatus[i] = 0;
		lpFresults->lineQuad[i].p1X = 0;
		lpFresults->lineQuad[i].p2X = 0;
		lpFresults->lineQuad[i].p3X = 0;
		lpFresults->lineQuad[i].p4X = 0;
		lpFresults->lineQuad[i].p1Y = 0;
		lpFresults->lineQuad[i].p2Y = 0;
		lpFresults->lineQuad[i].p3Y = 0;
		lpFresults->lineQuad[i].p4Y = 0;
	}

}

void copyAllTicketResultsGI25(LPTICKETRESULTS ticketResults, int idx){
	int i, len;
	LPFIELDSRESULTS lpFresults = &ticketResults->fieldsResults[idx];
	int x1, x2, y1, y2;

	strcpy(lpFresults->result, strGResultI25);
	len = strlen(strGResultI25);
	for(i=0;i<len;i++){
		lpFresults->flConf[i] = cnfGResultI25[i];
		lpFresults->flQuad[i].p1X = qGResultI25[i].p1X;
		lpFresults->flQuad[i].p1Y = qGResultI25[i].p1Y;
		lpFresults->flQuad[i].p2X = qGResultI25[i].p2X;
		lpFresults->flQuad[i].p2Y = qGResultI25[i].p2Y;
		lpFresults->flQuad[i].p3X = qGResultI25[i].p3X;
		lpFresults->flQuad[i].p3Y = qGResultI25[i].p3Y;
		lpFresults->flQuad[i].p4X = qGResultI25[i].p4X;
		lpFresults->flQuad[i].p4Y = qGResultI25[i].p4Y;
	}
	lpFresults->angle = 0;
	lpFresults->lineQuadStatus[0] = 1;
	x1 = qGResultI25[0].p1X;
	x2 = qGResultI25[len-1].p2X;
	y1 = qGResultI25[0].p1Y;
	y2 = qGResultI25[len-1].p3Y;
	lpFresults->lineQuad[0].p1X = x1;
	lpFresults->lineQuad[0].p2X = x2;
	lpFresults->lineQuad[0].p3X = x2;
	lpFresults->lineQuad[0].p4X = x1;
	lpFresults->lineQuad[0].p1Y = y1;
	lpFresults->lineQuad[0].p2Y = y1;
	lpFresults->lineQuad[0].p3Y = y2;
	lpFresults->lineQuad[0].p4Y = y2;
	for(i=1;i<NUM_LINESTATUS;i++){
		lpFresults->lineQuadStatus[i] = 0;
		lpFresults->lineQuad[i].p1X = 0;
		lpFresults->lineQuad[i].p2X = 0;
		lpFresults->lineQuad[i].p3X = 0;
		lpFresults->lineQuad[i].p4X = 0;
		lpFresults->lineQuad[i].p1Y = 0;
		lpFresults->lineQuad[i].p2Y = 0;
		lpFresults->lineQuad[i].p3Y = 0;
		lpFresults->lineQuad[i].p4Y = 0;
	}
}
void copyBackGResultsBC(LPTICKETRESULTS ticketResults){
	int i;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(strcmp(ticketResults->fieldsResults[i].name, "UPC") == 0){
    			copyAllTicketResultsGUPC(ticketResults, i);
//    			strcpy(ticketResults->fieldsResults[i].result,strGResultUPC);
    		}
    		if(strcmp(ticketResults->fieldsResults[i].name, "I25") == 0){
    			copyAllTicketResultsGI25(ticketResults, i);
//    			strcpy(ticketResults->fieldsResults[i].result,strGResultI25);
    	}
    }
  }
}
void initGBC(){
	int i;
	strcpy(strGResultUPC,"? ????? ????? ?");
    for(i=0;i<strlen(strGResultUPC);i++){
    	cnfGResultUPC[i] = 0;
    	qGResultUPC[i].p1X = 0;
    	qGResultUPC[i].p1Y = 0;
    	qGResultUPC[i].p2X = 0;
    	qGResultUPC[i].p2Y = 0;
    	qGResultUPC[i].p3X = 0;
    	qGResultUPC[i].p3Y = 0;
    	qGResultUPC[i].p4X = 0;
    	qGResultUPC[i].p4Y = 0;
	   }

	strcpy(strGResultI25,"???\?-??????\?-??\?-??????");
    for(i=0;i<strlen(strGResultI25);i++){
    	cnfGResultI25[i] = 0;
    	qGResultI25[i].p1X = 0;
    	qGResultI25[i].p1Y = 0;
    	qGResultI25[i].p2X = 0;
    	qGResultI25[i].p2Y = 0;
    	qGResultI25[i].p3X = 0;
    	qGResultI25[i].p3Y = 0;
    	qGResultI25[i].p4X = 0;
    	qGResultI25[i].p4Y = 0;
	   }
}
void setGresultUPC(int k, char ch, int cnf, LPQUADRANGLE lpQuad){
    strGResultUPC[k] = ch;
	cnfGResultUPC[k] = cnf;
	qGResultUPC[k].p1X = lpQuad->p1X;
	qGResultUPC[k].p1Y = lpQuad->p1Y;
	qGResultUPC[k].p2X = lpQuad->p2X;
	qGResultUPC[k].p2Y = lpQuad->p2Y;
	qGResultUPC[k].p3X = lpQuad->p3X;
	qGResultUPC[k].p3Y = lpQuad->p3Y;
	qGResultUPC[k].p4X = lpQuad->p4X;
	qGResultUPC[k].p4Y = lpQuad->p4Y;
}
void setGresultI25(int k, char ch, int cnf, LPQUADRANGLE lpQuad){
    strGResultI25[k] = ch;
	cnfGResultI25[k] = cnf;
	qGResultI25[k].p1X = lpQuad->p1X;
	qGResultI25[k].p1Y = lpQuad->p1Y;
	qGResultI25[k].p2X = lpQuad->p2X;
	qGResultI25[k].p2Y = lpQuad->p2Y;
	qGResultI25[k].p3X = lpQuad->p3X;
	qGResultI25[k].p3Y = lpQuad->p3Y;
	qGResultI25[k].p4X = lpQuad->p4X;
	qGResultI25[k].p4Y = lpQuad->p4Y;
}
void saveCurResultsBC(LPTICKETRESULTS ticketResults){
	int i, k;
	char * res;
//printf("-------saveCurResultsBC \n" );

    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(strcmp(ticketResults->fieldsResults[i].name, "UPC") == 0){
    		   res = ticketResults->fieldsResults[i].result;
//   printf("-------res UPC= %s strlen(res)= %d\n", res, strlen(res));
   			   for(k=0;k<strlen(res);k++){
    			   if(strGResultUPC[k] == '?' && res[k] >= '0' && res[k] <= '9')
    				   setGresultUPC(k, res[k], ticketResults->fieldsResults[i].flConf[k], &ticketResults->fieldsResults[i].flQuad[k]);
//    				   strGResultUPC[k] = res[k];
    		   }
//      	  	   strGResultUPC[k] = 0;
    		}
    		if(strcmp(ticketResults->fieldsResults[i].name, "I25") == 0){
    			res = ticketResults->fieldsResults[i].result;
//printf("-------res I25= %s, strlen(res)= %d \n", res, strlen(res));
   			    for(k=0;k<strlen(res);k++){
    			   if(strGResultI25[k] == '?' && res[k] >= '0' && res[k] <= '9')
    				   setGresultI25(k, res[k], ticketResults->fieldsResults[i].flConf[k], &ticketResults->fieldsResults[i].flQuad[k]);
//    			      strGResultI25[k] = res[k];
   			   }
//   			   strGResultI25[k] = 0;
    	}
    }
  }
}
int test_G_I25(){
	int i, cc, len;

	len = strlen(strGResultI25);
	if(len<14) return 0;
	cc = 0;
	for(i=0;i<len;i++) {
	    if(strGResultI25[i] == '?') return 0;
	    if(cnfGResultI25[i] < 10) cc++;
	}

printf("-------strGResultI25= %s, cc= %d\n", strGResultI25, cc);

	if(cc> 3) return 0;
	return 1;
}
void pre_ProcessColorImageSCR(LPIMGDATA img, LPIMGDATA imgOut, int byteSteps, int US_state, int iter){
	LPBYTE imgPtr = img->pixPtr;
	int    biWidthPad, biSizeImage;
	int *  lpImgMap_m;
	int *  lpImgMap_s;
	int    grid = 50;
	int    mapSize;
	float kFactor = 0.17;

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
		   default: kFactor = 0.15;break;
		}
		switch(iter){
		   case 1:kFactor += 0.1;break;
		   case 2:kFactor -= 0.1;break;
		}
		switch(US_state){
		   case 0:
				getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			    convert24BMPtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
			   break;
		   default:
				getMapValues_24(imgPtr, img->bi.biWidth, biWidthPad, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			    convert24BMPtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
                break;
		}
//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

		break;
	case 32:
		switch(US_state)
		{
		   case 0:kFactor = 0.15;break;
		   default: kFactor = 0.15;break;
		}

//		kFactor += (0.1*iter);
		switch(iter){
		   case 1:kFactor += 0.1;break;
		   case 2:kFactor -= 0.1;break;
		}
		switch(US_state){
		   case 0:
			   getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   convertRGBAtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
		       break;
		   default:
			   getMapValues_RGBA(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   convertRGBAtoMonochromeGrid(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
                break;
		}

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

		break;
	case 565:
		switch(US_state)
		{
		   case 0:kFactor = 0.08;break;
		   default: kFactor = 0.08;break;
		}

		switch(iter){
		   case 1:kFactor += 0.05;break;
		   case 2:kFactor -= 0.05;break;
		}
		switch(US_state){
		   case 2:
			   break;
		   default:
			   getMapValues_RGB565(imgPtr, img->bi.biWidth, img->bi.biHeight, lpImgMap_m, lpImgMap_s, grid, byteSteps, 0);
			   convertRGB565toMonochromeGrid_sOff(img, imgOut, lpImgMap_m, lpImgMap_s, grid, kFactor);
                break;
		}

//		saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");
		break;

	default:
		sprintf(img->error,"image type not supported yet \n");

		break;
	}

	yl_free(lpImgMap_m);
	yl_free(lpImgMap_s);
	if(img->error[0] != 0)
		return;

	if(imgOut->bi.biWidth > imgOut->bi.biHeight) {
		rotate(imgOut, 90);
	}
/*
else{

//	cleanSidesForScratchOff(imgOut);
//	 saveBMPbuffer (imgOut->ptr, img->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");

  }
*/
}

int regognizeOCR_BC(int x1, int x2, int y1, int y2, LPTICKETDATA ticketData,
		STRINGS * pResults, WORD* pwConf, QUADRANGLE* pPattern, LPIMGDATA img, double angle, int iAlphaIndx, int idx, char *fieldName){
//	  int cnf = 0;
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
//	  int iAlphaIndx;
	  WORD wConfThreshold;
	  WORD size = 15;
	  WORD wWidthSegment = 192;
	  WORD wHeightSegment = 192;
	  WORD nNormVersion;
	  char* lpResultBuffer;

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
		HLPBYTE hpFieldH;
		LPBYTE pixPtr;
		int biWidth, biHeight;

//	    char imgName[100];

		pixPtr = img->pixPtr; biWidth= img->bi.biWidth, biHeight= img->bi.biHeight;



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

//	    angle = 0;

// extract field !!!!!!!!!!!!!!!
		err = extractField(pixPtr, biWidth, biHeight,
				x1, y1,
				x2, y1,
				x1, y2,
				&hpFieldH,
				&width,
				&height,
				8,
				"bc",
				angleFlag,
				&angle
		);
		if(err < 0){
			 yl_free(hpFieldH.ptr);
			 ticketData->fieldBMPPtr = NULL;
			return -2;
		}

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
	}

//   setNewImageName(imgName, img->szIMGName, idx+70);
//   saveBMPbuffer (hpFieldH.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");


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

	  lpResultBuffer = pResults -> lpResult;

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

//	  iAlphaIndx = 22; // ?
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
//	  strcpy(ticketData->fieldAddress->name, img->szIMGName);
	  strcpy(ticketData->fieldAddress->name, fieldName);
	  ticketData->fieldBMPPtr = hpFieldH.ptr;

	  err=recField(ticketData, lpImageL, lpSegment, lpLineRem, lpTextLine,
	           lpSpeck, lpIgnSubSigns, lpContext, iFieldType, iAlphaIndx,
	           lpAlphaMap, wConfThreshold, iWidthFrame, iHeightFrame, lpWorkBuffer,
	           lpExtractChar, lpRecChar, iRecMode, iRecContinue,
	           lpStrings, pwConf, lpCoordinates);

//	    printf("result= %s\n",lpStrings->lpResult);
//	  {char bufLog[500];
//	     printf(bufLog,"result= %s\n",lpStrings->lpResult);
//	     printf(bufLog);
//	     saveLog(bufLog, "");
//	  }
/*
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
*/
	  {int i;
	  WORD xOrig;
	  WORD yOrig;
	  WORD xCorn;
	  WORD yCorn;

	  for(i=0; i<1000; i++){
	   if(*(lpResultBuffer+(long)i) == 0) break;
	    xOrig = *(pxOrigin+i); yOrig = *(pyOrigin+i);
	    xCorn = *(pxCorner+i); yCorn = *(pyCorner+i);
	    (pPattern+(DWORD)i) -> p1X = xOrig;
	    (pPattern+(DWORD)i) -> p1Y = yOrig;
	    (pPattern+(DWORD)i) -> p2X = xCorn;
	    (pPattern+(DWORD)i) -> p2Y = yOrig;
	    (pPattern+(DWORD)i) -> p3X = xCorn;
	    (pPattern+(DWORD)i) -> p3Y = yCorn;
	    (pPattern+(DWORD)i) -> p4X = xOrig;
	    (pPattern+(DWORD)i) -> p4Y = yCorn;
	   }
	  }

//printf(" l_free(hpFieldH.ptr)\n");
//fflush(stdout);
	 yl_free(hpFieldH.ptr);
	 ticketData->fieldBMPPtr = NULL;
// printf(" after l_free(hpFieldH.ptr)\n");
// fflush(stdout);
	  return 0;
}

int getOCRforBC(LPIMGDATA imgOut, LPTICKETDATA ticketData, int x1, int x2, int y1, int y2, double angle, int idx){
   int  cnf;
   BYTE * pDataBlock;
   int iAlphaIndx = 22;

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
			&ticketData->strings, ticketData->flConf, ticketData->flQuad, imgOut, angle, iAlphaIndx, idx, imgOut->szIMGName);

	trimSpOut(ticketData);

	return cnf;
}
int test_I25_OCR(LPTICKETDATA ticketData){
	int ret = 0;
	char *result = ticketData->strings.lpResult;
	int i, k, cc, len, pen;


	pen = 0;
	len = strlen(result);
//printf(" ----------- len= %d, result= %s\n",len, result);
	if(len > 15){
	   cc = 0;k = 0;
       for(i=k;i<4;i++){
    	   if(result[i] < '0' || result[i] > '9') cc++;
       }
       pen += cc;k = 4;
       if(result[k] < '0' || result[k] > '9') k++;
       cc = 0;
       for(i=0;i<7;i++){
    	   if(result[k+i] < '0' || result[k+i] > '9') cc++;
       }
       pen += cc; k += i;
       if(result[k] < '0' || result[k] > '9') k++;
       cc = 0;
       for(i=0;i<3;i++){
    	   if(result[k+i] < '0' || result[k+i] > '9') cc++;
       }
       pen += cc;
       cc = 0;
       for(i=0;i<len;i++){
    	   if(ticketData->flConf[i]< 10) cc++;
       }
       if(cc > 4) pen += cc;
//       printf(" ----------- cc= %d, pen= %d\n",cc, pen);
//    printf(" ----------- cc= %d, ret= %d, result[0]= %c, result[4]= %c, result[12]= %c\n",cc, ret, result[0], result[4], result[12]);
//       if(cc >= 14 && len-cc < 6){
//    	   ret = 1;
//    	   if((result[4] == '-' || result[4] == ' ') && (result[12] == '-' || result[12] == ' ')) ret = 1;
//       }
	}
	else pen += 100;

    if(pen < 1) ret = 1;
//   printf(" ----------- ret= %d, pen= %d\n",ret, pen);
	return ret;
}
void insertCharOCRresults(LPTICKETDATA ticketData, int idx, char ch){
    int i, lenOCR;
    char *result = ticketData->strings.lpResult;
    lenOCR = strlen(result);
    if(idx>=lenOCR) return;
    for(i=lenOCR;i>=idx;i--){
    	result[i+1] = result[i];
    }
    result[idx] = ch;

}
void correctOCRResults_I25(LPTICKETDATA ticketData){
    int lenOCR, i, cc;
    char *result = ticketData->strings.lpResult;

    lenOCR = strlen(result);
    if(lenOCR<18) return;
    if(result[4] == ' ') result[4] = '-';
    if(result[4] >= '0' && result[4] <= '9') insertCharOCRresults(ticketData, 4, '-');
    lenOCR = strlen(result);
    if(result[12] == ' ') result[12] = '-';
    cc = 0;
    for(i=12;i<lenOCR;i++){
 	   if(result[i] < '0' || result[i] > '9') break;
 	   cc++;
    }
    if(result[12] >= '0' && result[12] <= '9') {
    	if(cc <=3)
    	  insertCharOCRresults(ticketData, 12, '-');
    	else
    		result[12] = '-';
    }

}
void trimtResultsI25(char* strResultI25, int *confI25){
    int i, len, stop;
    char strTmp[64];

    len = strlen(strResultI25);if(len >=60) return;
    if(len>10) {
      strcpy(strTmp, strResultI25);
      strResultI25[4] = '-';
      for(i=4;i<=10;i++)
  	    strResultI25[i+1] = strTmp[i];
      strResultI25[12] = '-';
      stop = 14;if(stop>len) stop = len;
      for(i=11;i<stop;i++)
  	    strResultI25[i+2] = strTmp[i];
      if(len > stop) {
         strResultI25[stop+2] = '-';strResultI25[stop+3] = 0;
         for(i=stop;i<len;i++)
  	       strResultI25[i+3] = strTmp[i];
         strResultI25[len+3] = 0;
      }else
    	  strResultI25[stop+2] = 0;
    }
}
void adjustResultsI25(LPTICKETDATA ticketData, char* strResultI25, int *confI25){
	char *result = ticketData->strings.lpResult;
    char strTmp[64];
    int i, len, lenOCR, stop, cc, ccCnf;

//   printf(" 1----------- strResultI25= %s\n",strResultI25);
      correctOCRResults_I25(ticketData);
      lenOCR = strlen(result);
      len = strlen(strResultI25);if(len >=60) return;
      if(len>10) {
        strcpy(strTmp, strResultI25);
        strResultI25[4] = '-';
        for(i=4;i<=10;i++)
    	    strResultI25[i+1] = strTmp[i];
        strResultI25[12] = '-';
        stop = 14;if(stop>len) stop = len;
        for(i=11;i<stop;i++)
    	    strResultI25[i+2] = strTmp[i];
        strResultI25[stop+2] = '-';strResultI25[stop+3] = 0;
        for(i=stop;i<len;i++)
    	    strResultI25[i+3] = strTmp[i];
        strResultI25[len+3] = 0;
        stop = 15;
        if(stop>=lenOCR) return;

//        printf(" 1----------- strResultI25= %s\n",strResultI25);
        cc = 0;ccCnf = 0;
        for(i=0;i<stop;i++){
//       printf(" result[i]= %c, ccCnf= %d\n",result[i], ticketData->flConf[i]);
         	   if(ticketData->flConf[i]< 60) ccCnf++;
        	if(result[i] >= '0' && result[i] <= '9'){
        	   if(result[i] != strResultI25[i]) cc++;
        	}
        }
//        printf(" cc= %d, ccCnf= %d\n",cc, ccCnf);
        if(cc > 4){
            if(ccCnf < 3){
          	  strcpy(strResultI25, result);
                for(i=0;i<lenOCR;i++){
              	  confI25[i] = ticketData->flConf[i];
                }
            }
        }else {
           for(i=0;i<stop;i++){
        	   if(result[i] == strResultI25[i]) confI25[i] = 100;
        	   else {
        		   if(ticketData->flConf[i]> 70)
        		      strResultI25[i] = result[i];
        		   confI25[i] = ticketData->flConf[i];
        	   }
           }
        }
      }
      else{
    	  strcpy(strResultI25, result);
          for(i=0;i<lenOCR;i++){
        	  confI25[i] = ticketData->flConf[i];
          }
      }
//      printf(" 2----------- strResultI25= %s\n",strResultI25);
}

void trimSpOut(LPTICKETDATA ticketData){
   int i, k, k1, k2, len;
   char *str = ticketData->strings.lpResult;

   len = strlen(str);
   if(str[0] > 32 && str[1]== 32) k1=1;
   i= k1;
   while(i<len && str[i] <= ' ') i++;
   k1 = i;
   i= len-1;
   while(i>0 && str[i] <= ' ') i--;
   k2 = i;
   if(k1 == 0 && k2 == len-1) return;
   if(k1<k2){
	  k = 0;
      for(i=k1;i<=k2;i++){
    	  str[k] = str[i];
  		  ticketData->strings.lpCand1[k]  = ticketData->strings.lpCand1[i];
  		  ticketData->strings.lpCand2[k]  = ticketData->strings.lpCand2[i];
  		  ticketData->flConf[k] = ticketData->flConf[i];
  		  ticketData->flQuad[k].p1X = ticketData->flQuad[i].p1X;
  		  ticketData->flQuad[k].p1Y = ticketData->flQuad[i].p1Y;
  		  ticketData->flQuad[k].p2X = ticketData->flQuad[i].p2X;
  		  ticketData->flQuad[k].p2Y = ticketData->flQuad[i].p2Y;
  		  ticketData->flQuad[k].p3X = ticketData->flQuad[i].p3X;
  		  ticketData->flQuad[k].p3Y = ticketData->flQuad[i].p3Y;
  		  ticketData->flQuad[k].p4X = ticketData->flQuad[i].p4X;
  		  ticketData->flQuad[k].p4Y = ticketData->flQuad[i].p4Y;
    	  k++;
      }
      str[k] = 0;
	  ticketData->strings.lpCand1[k]  = 0;
	  ticketData->strings.lpCand2[k]  = 0;
   }
}

void initQ(LPQUADRANGLE q){
	q->p1X = 0;q->p2X = 0;q->p3X = 0;q->p4X = 0;
	q->p1Y = 0;q->p2Y = 0;q->p3Y = 0;q->p4Y = 0;
}
