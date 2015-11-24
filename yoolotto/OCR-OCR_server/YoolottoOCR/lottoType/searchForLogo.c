#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//#include "../YLogoClassifier/ylogo_classifier.h"
#include  "../headers/Image.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include  "../headers/Functions.h"

#include "../headers/BarCode.h"
#include "../headers/ocr.h"

int barDeCodeI25_verY (int *hist, char *strResult);
int barDeCodeI25_verYInv (int *hist, char *strResult);

//int  typeOfLotteryID_TX[LOTTO_TYPES_NUM_TX] = {
//		8,9,10,12,13,14,15,16
//};
int  typeOfLotteryID_TX[LOTTO_TYPES_NUM_TX] = {
		0,14,15,8,12,9,13,10,16
};

//int  typeOfLotteryLogo_TX[LOTTO_TYPES_NUM_TX] = {
//		7,0,1,2,4,5,6,3
//};




int getValueForSmearing(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
void setPixelsOut(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2, int v);
//void saveBMPbuffer (LPBYTE buff, char* name, char* path);
void createDesityTab(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int* densityTab, int len);
void createDesityTab2(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len);
void getBarcodeYFromDensityTable(int *dTab, int *y1, int *y2, int length);
void getBarcodeXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
void getLogoXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
int createImageLogoBarcodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2);
//void copyPixelsTest(LPBYTE ptrOut, LPBYTE ptrInp, int width, int height);
int barDeCodeI25 (int   *hist, int lenH, LPMASK maskAlph, int lenStr, int nOfCharInBar, int invDec,
                  LPSTR strResult, int *confResult, LPSTR cand2);
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
void getLogoYFromDensityTable(int *dTab, int *y1, int *y2, int length);
void getLogoYFromDensityTable_2(int *dTab, int *y1, int *y2, int length);
//void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2);
//void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2, YlogoClassificationResult *resultLogo);
int getPixelSmearing(LPBYTE ptr, int xn, int yn, int wPad, int height);


//int getLogoRectangle(LPIMGDATA imgOut, char *mainPath, char *imgName, int* xL1, int* xL2, int* yL1, int* yL2);
//int extractLogoField (LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPRADON radonStruct, LPTICKETRESULTS ticketResults);
//void initTicketResults(LPTICKETRESULTS lpTicketResults);
//void setGlobalDataPrototype(LPTICKETDATA ticketDataPtr, char *mainPath);
//void set_I_O_Path(LPIMGDATA img, char *mainPath, char *test);
int getBarcodeYFromDensityTable_many(int *dTab, int *y1, int *y2, int length, int size);
int testTypeFromBarCodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int *confOut);
//int histVSkewed(  LPBYTE ptrPix, int widthImg, int hightImg, float begAlpha, float maxAlpha, float stepAlpha, int x1, int x2, int y1, int y2,
//                  int *histV, int  *lenH, long   *angleHOtp, int *scAngleH, long  *shX );
//void setCAlotteryTypes(void);
//void setNYlotteryTypes(void);
void getCALottoType(char *strResult, int *typeInt, char* typeChar);
void getNYLottoType(char *strResult, int *typeInt, char* typeChar);

//int ylogo_classifier_initB(const char *initBuffer, int buffLen);

void strcpyPA_type(char *typeStr, int n);
//void saveLog(char *str1, char *str2);

//void printBMPDataTest (LPBYTE imgPtr);

char bufLog[200];
/*
int initLogoClassifier(LPTICKETDATA ticketDataPtr, LPBYTE *yLogoBinBufferPtr, int *lenBinBuffer){
    int retLogo = 0;
	FILE   *f;
    char   path[500];
    char   fName[500];
    int    len;
    LPBYTE  ptrBuf;
	size_t fSize = 0;

    *yLogoBinBufferPtr = NULL;
    *lenBinBuffer = 0;
    strcpy (path, ticketDataPtr->prototypePath);

    if (strlen (path) == 0) {
    	sprintf(ticketDataPtr->error,"initLogoClassifier:  directory not defined\n");
        saveLog("LC Error: ", ticketDataPtr->error);
    	return -1;
    }
	strcpy(fName, path);
    switch(US_State) {
	   case 0: // TX
			strcat (fName, "ppProjData_TX.bin");break;
	   case 1: // PA
			strcat (fName, "ppProjData_PA.bin");break;
	   case 2: // CA
			strcat (fName, "ppProjData_CA.bin");break;
	   case 3: // NY
			strcat (fName, "ppProjData_NY.bin");break;
	   default:
			strcat (fName, "ppProjData_TX.bin");break;
	}

	f= fopen(fName, "r");
	if(f == NULL) {
		sprintf(ticketDataPtr->error,"initLogoClassifier (\"%s\") failed: %s\n",fName, strerror(errno));
		return -1;
	}
	fseek(f, 0L,SEEK_END);


	if(ferror(f)){
		sprintf(ticketDataPtr->error,"initLogoClassifier (\"%s\") fseek failed: %s\n",fName, strerror(errno));
		return -2;
	}
	len = ftell(f);
	fseek(f, 0L,SEEK_SET);

//printf("file %s length= %d\n",fName, len);
//fflush(stdout);

    ptrBuf = yl_calloc( "LC_A", len, sizeof(BYTE));
	if(ptrBuf == NULL) {
		printf("yl_calloc failed");
		return -7;
	}
	fSize = fread((char *)ptrBuf, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			sprintf(ticketDataPtr->error,"initLogoClassifier (\"%s\") read failed: %s\n",fName, strerror(errno));
		else
			sprintf(ticketDataPtr->error,"initLogoClassifier (\"%s\") read failed: (size= %d) != (len= %d\n)",fName, fSize*len, len);
		yl_free(ptrBuf);
		close_file(f);
		return -3;
	}
	*yLogoBinBufferPtr = ptrBuf;
	*lenBinBuffer = len;

    printf("---------------ylogo_classifier_initB\n");
    fflush(stdout);

    printf("file %s length= %d\n",fName, len);
    fflush(stdout);

    retLogo= ylogo_classifier_initB((const char *)ptrBuf, len);
    close_file(f);

    printf("---------------retLogo= %d\n", retLogo);
    fflush(stdout);

    return 0;
}
*/

void setUSstates(void){
    USstate[0] = "TEXAS"; //0
    USstate[1] = "PENNSYLVANIA"; //1
    USstate[2] = "CALIFORNIA"; //2
    USstate[3] = "NEW YORK"; //3
}


int getTypeOfTicket(LPIMGDATA imgInp, char *mainPath, char *imgName){
   int pixX = 16, pixY = 16;
   int type = 0;
   IMGDATA imgOut2;
   switch(US_State) {
   case 0: // TX
   case 1: // TX
     type = smearingForTypeTX(imgInp, &imgOut2, mainPath, imgName, pixX, pixY);break;
/*
   case 1: // PA
	   pixX = 24; pixY = 24;
	     setPAlotteryTypes();type = smearingForTypePA(imgInp, &imgOut2, mainPath, imgName, pixX, pixY);break;
   case 2: // CA
	   pixX = 24; pixY = 24;
	     setCAlotteryTypes();type = smearingForTypeCA(imgInp, &imgOut2, mainPath, imgName, pixX, pixY);break;
   case 3: // NY
	   pixX = 24; pixY = 24;
	     setNYlotteryTypes();type = smearingForTypeNY(imgInp, &imgOut2, mainPath, imgName, pixX, pixY);break;
*/
   default:
	   type = smearingForTypeTX(imgInp, &imgOut2, mainPath, imgName, pixX, pixY);break;
   }
     return type;
}


int getValueForSmearing_White(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH){
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
	if(sum > (3*(gridH*gridW))/6)
		return 255;
	return 0;
}

int smearingForTypeTX(LPIMGDATA imgInp, LPIMGDATA imgOut, char *mainPath, char *imgName, int pixX, int pixY ){
//#define DENSITY_L 1000
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
	//    int           sizeOut;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	int densityTab[imgInp->bi.biHeight];
//	int densityTab2[imgInp->bi.biHeight];
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

// LOGO second smearing
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
//			setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx2, idxW*ddSmearingx2 + gridW,
//					idxH*ddSmearingy2, idxH*ddSmearingy2+gridH-1, m);
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
//	int yL1, yL2, xL1, xL2;

//             getLogoRectangle(&imgOut_Sm, mainPath, imgName, &xL1, &xL2, &yL1, &yL2);

//}

	 createDesityTab(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);
//	 createDesityTab2(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, pixX, densityTab2, imgInp->bi.biHeight-1);


// barcode test
	 {
		 int size = 5, confOut;
		 int y1[size], y2[size], x1, x2, sizeBc, i;
		 typeInt = -1;

		 sizeBc = getBarcodeYFromDensityTable_many(densityTab, y1, y2, DENSITY_L, size);
// printf("  sizeBc= %d\n", sizeBc);
		 for(i=0;i<sizeBc;i++) {
		   getBarcodeXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, y1[i], y2[i], &x1, &x2);
//   printf("  try; y1 = %d, y2= %d, x1= %d, x2= %d\n", y1[i], y2[i], x1, x2);
		   typeInt = testTypeFromBarCodeTX(imgInp, x1*8, x2*8, imgInp->bi.biHeight-1-y1[i], imgInp->bi.biHeight-1-y2[i], i, &confOut);
//		   typeInt = 0;
//   printf("!!!!!!!!!!!!!!!!-----------------typeInt= %d\n",typeInt);

		   if(typeInt>=0) break;

		 }
	 }



	 // second barcode test
			if(typeInt < 0)
			{
				int y1, y2, x1, x2;

				printf("------------ second barcode test\n");

				getBarcodeYFromDensityTable(densityTab, &y1, &y2, DENSITY_L);
				getBarcodeXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, y1, y2, &x1, &x2);


				typeInt = createImageLogoBarcodeTX(imgInp, x1*8, x2*8, imgInp->bi.biHeight-1-y1, imgInp->bi.biHeight-1-y2);

			}

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//				   if(typeInt>=0) return typeInt;  // !!!!!!!!!!!!!!!!!!!
			       yl_free(ptrImgOut);  // !!!!!!!!!!!!!!!!!!!
				   return typeInt;

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*

// LOGO
	{
//		int densityTab[imgInp->bi.biHeight];
//		int ii;
		int yL1, yL2, xL1, xL2;

//				for(ii=1;ii<=densityTab[0];ii++) {
//					sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//					saveLog( "", bufLog );
//				}
//						for(ii=1;ii<=densityTab2[0];ii++) {
//							sprintf(bufLog,"densityTab2[ %d ]= %d\n",ii, densityTab2[ii]);
//							saveLog( "", bufLog );
//						}


		 getLogoYFromDensityTable(densityTab, &yL1, &yL2, DENSITY_L);

		 if(yL1 - yL2 < 40){
				for(idxH=0;idxH<sizeH;idxH++){
					gridH = ddSmearingy;
					if(idxH == sizeH-1) gridH = lastH;
					for(idxW=0;idxW<sizeW;idxW++){
						gridW = ddSmearingx;
						if(idxW == sizeW-1) gridW = lastW;
						if(idxH > stopSm) break;
					    m = getValueForSmearing(imgInp->pixPtr, idxW, idxH, biWidthPad, imgInp->bi.biHeight, ddSmearingx, ddSmearingy, gridW, gridH);
						setPixelsOut(ptrBuffOut,  biWidthPad, imgInp->bi.biHeight, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
								idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
					}
				}
			   createDesityTab(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);

//				for(ii=1;ii<=densityTab[0];ii++) {
//					sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//					saveLog( "", bufLog );
//				}

			   getLogoYFromDensityTable_2(densityTab, &yL1, &yL2, DENSITY_L);
		 }
		 getLogoXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX*2, yL1, yL2, &xL1, &xL2);

//		 		 sprintf(bufLog,"yL1= %d, yL2= %d, yL1 - yL2= %d\n",yL1, yL2, yL1 - yL2);
//		 		 saveLog( "", bufLog );
//		 		 sprintf(bufLog,"xL1= %d, xL2= %d, xL1 - xL2= %d\n",8*xL1, 8*xL2, 8*(xL1 - xL2));
//		 		 saveLog( "", bufLog );

//		   printf("------------ ptrImgOut = %lu\n", (unsigned long)ptrImgOut);
//		   printf("------------ afterBarCode\n");

//     add margins
		 {int dd1 = 4, dd2 = 4, dd = 0;
// y
		    if(yL1 - yL2 < 150){
		    	dd1 += 10;
		    }
		    yL1 += dd1;
		    yL2 -= dd2;
		    if(yL1 >= imgInp->bi.biHeight) yL1 = imgInp->bi.biHeight-1;
		    if(yL2 < 0) yL2 = 0;
// x
		    dd1 = 1, dd2 = 1;
		    dd =500 -8*(xL2 - xL1);
		    if(dd > 16){
		    	dd1 += (dd/16);
		    	dd2 += (dd/16);
		    }
		    xL1 -= dd1;
		    xL2 += dd2;
  	        if(xL1 < 0) xL1 = 0;
		    if(8*xL2 >= imgInp->bi.biWidth) xL2 = (imgInp->bi.biWidth-1)/8;

		 }
//		 sprintf(bufLog,"yL1= %d, yL2= %d, yL1 - yL2= %d\n",yL1, yL2, yL1 - yL2);
//		 saveLog( "", bufLog );
//		 sprintf(bufLog,"xL1= %d, xL2= %d, xL1 - xL2= %d\n",8*xL1, 8*xL2, 8*(xL1 - xL2));
//		 saveLog( "", bufLog );

//        x1 -= 2;x2 += 2;
//        if(x1 < 0) x1 = 0;
//        if(x2 > imgInp->bi.biWidth-1) x2 = imgInp->bi.biWidth-1;


//		for(ii=1;ii<=densityTab[0];ii++) {
//			sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//			saveLog( "", bufLog );
//		}

		createImageLogo(imgInp, xL1*8, xL2*8, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, &resultLogo);
	}


//	    saveBMPbuffer (ptrImgOut, "smearingLogoBarcode.bmp", imgInp->szOutPath);
//	    saveBMPbuffer (ptrImgOut, imgInp->szIMGName, imgInp->szOutPath);

//	   printf("------------ ptrImgOut = %lu\n", (unsigned long)ptrImgOut);
//	   printf("------------ afterBarCode\n");
//	   fflush(stdout);


	yl_free(ptrImgOut);
	return typeInt;
*/
}
void createDesityTab(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, i, j;
	int xCcBlack, xCcWhite;
	int thX, wPad_2;
	wPad_2= wPad/2;
	thX = pixX/8 + 1;

	if(len > height-1)	len = height-1;

	densityTab[0] = len;
	for(i=0;i<len;i++){
//		yn = height-1 -i;
		yn = i;
		xCcBlack = 0;
		xCcWhite = 0;
		for(j=0;j<wPad_2-2;j++){
//			yn=height-1-yn;
			nY=yn*wPad;
			nX = wPad_2 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		xCcWhite = 0;
		for(j=0;j<wPad_2-2;j++){
//			yn=height-1-yn;
			nY=yn*wPad;
			nX = wPad_2 + j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		densityTab[i+1] = xCcBlack;

	}
}

	void createDesityTab2(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
		long nY;
		long nX;
		LPBYTE ptrP;
		int yn, i, j;
		int xCcBlack, xCcWhite;
		int thX, wPad_2;
		wPad_2= wPad/2;
		thX = pixX/8 + 4;

		if(len > height-1)	len = height-1;

		densityTab[0] = len;
		for(i=0;i<len;i++){
			yn = i;
			xCcBlack = 0;
			xCcWhite = 0;
			for(j=0;j<wPad_2-2;j++){
				nY=yn*wPad;
				nX = wPad_2 - j;
				ptrP = ptrBuffOut+nY+nX;
				if(*ptrP == 0) {xCcBlack++;xCcBlack += xCcWhite;xCcWhite = 0;}
				else xCcWhite++;
				if(xCcWhite > thX) break;
			}
			xCcWhite = 0;
			for(j=0;j<wPad_2-2;j++){
				nY=yn*wPad;
				nX = wPad_2 + j;
				ptrP = ptrBuffOut+nY+nX;
				if(*ptrP == 0) {xCcBlack++;xCcBlack += xCcWhite; xCcWhite = 0;}
				else xCcWhite++;
				if(xCcWhite > thX) break;
			}
			densityTab[i+1] = xCcBlack;

		}

}

void getLogoYFromDensityTable(int *dTab, int *y1, int *y2, int length){
//	char bufLog[200];

     int i, k, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int thW = 10;
     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
     for(i=1;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
    	 if(dTab[k]>20){
            if(startY == 0){
        	   startY = k;
            }
            ccY++;
            av += dTab[k];
    	 }else{
             if(startY != 0 && ccY > 10 && iCand < 10){
            	 candidatesSt[iCand] = startY;
            	 candidatesH[iCand] = ccY;
            	 candidatesW[iCand] = av/ccY;
//	sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",iCand, candidatesSt[iCand], candidatesH[iCand], candidatesW[iCand]);
//	saveLog( "", bufLog );

            	 iCand++;
             }
    		 startY = 0;
    		 ccY=0;
    		 av = 0;
    	 }
   	   }else break;

     }
//     sprintf(bufLog,"iCand= %d------------------\n",iCand);
//     saveLog( "", bufLog );

     {int ii, kk, kkH;
//        kk = 0;
//	    for(i=0;i<iCand;i++){
//	    	if(dTab[0] - candidatesSt[i] >400) break;
//	    	if(candidatesH[i] > 100) {kk =i;break;}
//	    }

	    for(i=0;i<iCand;i++){
			if(candidatesW[i] > thW) {
//				   sprintf(bufLog," i= %d, candidatesW[i] = %d, dTab[0] - candidatesSt[i] = %d\n",i, candidatesW[i], dTab[0] - candidatesSt[i]);
//		  		   saveLog( "", bufLog );
//				   sprintf(bufLog," i= %d, candidatesSt[i] - (candidatesSt[i+1] - candidatesH[i+1]) = %d\n",i, candidatesSt[i] - (candidatesSt[i+1] - candidatesH[i+1]));
//		  		   saveLog( "", bufLog );
                kk = 300;kkH = 80;
                if(dTab[0] - candidatesSt[i] < 80) kk = 260;
                if(dTab[0] - candidatesSt[i] < 150 && candidatesW[i] > 85) {kk = 180;kkH = 10;}

//				   sprintf(bufLog," kk= %d, kkH= %d\n",kk, kkH);
//		  		   saveLog( "", bufLog );

                if(dTab[0] - candidatesSt[i] < 200 && i<iCand-1) {
                	if((candidatesH[i] < 40 && candidatesSt[i] - (candidatesSt[i+1] - candidatesH[i+1]) > kk &&
                			candidatesH[i+1] > kkH)) {
                		i++;
                	}
// 				   sprintf(bufLog,"  i= %d\n",i);
// 		  		   saveLog( "", bufLog );

                }
                ii = i;
                while(ii<iCand-1) {
	            	if((candidatesSt[ii] - candidatesH[ii] - candidatesSt[ii+1] < 50 && candidatesW[ii+1] > thW &&
	            			candidatesSt[i] - (candidatesSt[ii+1] - candidatesH[ii+1]) < 240) ||
	            			candidatesH[i] < 60){
	            		candidatesH[i] = candidatesSt[i] - (candidatesSt[ii+1] - candidatesH[ii+1]);
	            	}
	            	ii++;
                }

		   }
           *y1 = candidatesSt[i];
	       *y2 = candidatesSt[i] - candidatesH[i];
//		   sprintf(bufLog," *y1= %d, *y2 = %d\n",*y1, *y2);
//  		   saveLog( "", bufLog );
	      return;

	    }
     }

}
void getLogoYFromDensityTable_2(int *dTab, int *y1, int *y2, int length){
//	char bufLog[200];

     int i, k, j, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
     for(i=1;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
    	 if(dTab[k]>30){
            if(startY == 0){
        	   startY = k;
            }
            ccY++;
            av += dTab[k];
    	 }else{
             if(startY != 0 && ccY > 10 && iCand < 10){
            	 candidatesSt[iCand] = startY;
            	 candidatesH[iCand] = ccY;
            	 candidatesW[iCand] = av/ccY;
//         		sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",iCand, candidatesSt[iCand], candidatesH[iCand], candidatesW[iCand]);
//         		saveLog( "", bufLog );

            	 iCand++;
             }
    		 startY = 0;
    		 ccY=0;
    		 av = 0;
    	 }
   	   }else break;

     }
//     sprintf(bufLog,"iCand= %d------------------\n",iCand);
//     saveLog( "", bufLog );


     {int ii, i_1, kk;
	 for(i=0;i<iCand;i++){
		 if(candidatesW[i] > 30 && candidatesH[i] > 10) {
		    kk = i;
            if(kk==0 && iCand > 1){
                ii = kk;
                while(ii<iCand-1) {
	            	if(candidatesSt[ii] - candidatesH[ii] - candidatesSt[ii+1] < 100 && candidatesW[ii+1] > 30 &&
	            			candidatesSt[kk] - (candidatesSt[ii+1] - candidatesH[ii+1]) < 240){
	            		candidatesH[kk] = candidatesSt[kk] - (candidatesSt[ii+1] - candidatesH[ii+1]);
	            	}
	            	ii++;
                }
            }

            if(kk>0){
//         	sprintf(bufLog," kk= %d\n",kk);
//	  		saveLog( "", bufLog );
                ii = kk;
                while(ii>0) {
               	   i_1 = ii-1;
                   if(candidatesW[i_1] > 30 && candidatesSt[i_1] - candidatesH[i_1] - candidatesSt[ii] > 60){
                	  kk=i_1;ii = kk;
                   }else break;
                }

//             	sprintf(bufLog," kk= %d, ii= %d\n",kk, ii);
//    	  		saveLog( "", bufLog );

                while(ii>0) {
               	   i_1 = ii-1;
//                	sprintf(bufLog," candidatesW[i_1]= %d, candidatesSt[i_1] - candidatesH[i_1] - candidatesSt[ii]= %d\n",candidatesW[i_1], candidatesSt[i_1] - candidatesH[i_1] - candidatesSt[ii]);
//       	  		saveLog( "", bufLog );

            	   if(candidatesW[i_1] > 30 && candidatesSt[i_1] - candidatesH[i_1] - candidatesSt[ii] < 40){
            		  candidatesH[kk] = candidatesSt[i_1] - (candidatesSt[kk] - candidatesH[kk]);
            		  candidatesSt[kk] = candidatesSt[i_1];
            	   }
            	   ii--;
                }
            }
			 if(candidatesH[kk] < 120) {
				 int dd = 120-candidatesH[kk];
				 if(dd < 20) dd = 20;
			     for(j=1;j<dd;j++){
		     	   if(k<dTab[0]) {
			    	 k = candidatesSt[kk]+j;
			    	 if(dTab[k] == 0)
			    		 break;
			       }
			     }
			     if(candidatesSt[kk] + j<dTab[0]) {
			        candidatesSt[kk] += j;
			        candidatesH[kk] += j;
			     }
			 }

              *y1 = candidatesSt[kk];
		      *y2 = candidatesSt[kk] - candidatesH[kk];
//			 sprintf(bufLog," *y1= %d, *y2 = %d, j= %d\n",*y1, *y2, j);
//	  		saveLog( "", bufLog );

		      return;
		 }
	 }
    }


}

void getBarcodeYFromDensityTable(int *dTab, int *y1, int *y2, int length){
//	char bufLog[200];

     int i, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int th1 = 50, th2= 50, th3 = 50;
	 int iter = 0;

  while(iter < 3) {
     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
     for(i=1;i<length;i++){
    	 if(dTab[i]>th1){
            if(startY == 0){
        	   startY = i;
            }
            ccY++;
            av += dTab[i];
    	 }else{
             if(startY != 0 && ccY > 10 && iCand < 10){
            	 candidatesSt[iCand] = startY;
            	 candidatesH[iCand] = ccY;
            	 candidatesW[iCand] = av/ccY;
//         		sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",iCand, candidatesSt[iCand], candidatesH[iCand], candidatesW[iCand]);
//         		saveLog( "", bufLog );

            	 iCand++;
             }
    		 startY = 0;
    		 ccY=0;
    		 av = 0;
    	 }

     }

//     sprintf(bufLog,"iCand= %d------------------\n",iCand);
//     saveLog( "", bufLog );

	 for(i=0;i<iCand;i++){
//		 sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",i, candidatesSt[i], candidatesH[i], candidatesW[i]);
//  		saveLog( "", bufLog );
		 if(candidatesW[i] > th2 && candidatesH[i] > th3) {
			 int kk = i, k, cc;
			 while(candidatesH[kk] < 50 && kk < iCand-1 && candidatesSt[kk+1] < 450){

			     cc= 0;
			     for(k=candidatesSt[kk] + candidatesH[kk];k<candidatesSt[kk+1];k++){
		    	    if(dTab[k]<2){
		    		  cc++;
		    	    }
			    }
			     if(cc > 4) kk++;
			     else break;
			 }
			 k = kk;

//	    sprintf(bufLog,"k= %d\n",k);
//  		saveLog( "", bufLog );

             while(k<iCand-1) {
//            	    sprintf(bufLog,"k=%d, kk= %d, candidatesSt[k+1] + candidatesH[k+1] - candidatesSt[kk]= %d\n", k, kk, candidatesSt[k+1] + candidatesH[k+1] - candidatesSt[kk]);
//            		saveLog( "", bufLog );

	            	if(candidatesSt[k+1] + candidatesH[k+1] - candidatesSt[kk]  < 120){
	            		candidatesH[kk] = candidatesSt[k+1] + candidatesH[k+1] - candidatesSt[kk];
	            	} else
	            	   break;
	            	k++;
             }


              *y1 = candidatesSt[kk] + candidatesH[kk];
		      *y2 = candidatesSt[kk];

//		 sprintf(bufLog,"*y1= %d, *y2= %d \n",*y1, *y2);
//  		 saveLog( "", bufLog );

		      break;
		 }
	 }
	 if(*y1 -*y2 > 10){
		 break;
	 }
	 th1 -= 15;th2 -= 15;th3 -= 15;
	 iter++;
   }
}
int getBarcodeYFromDensityTable_many(int *dTab, int *y1, int *y2, int length, int size){
//	char bufLog[200];

     int i, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int th1 = 25, thW= 25, thH = 15;
     int idx = 0;

     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;

//printf("length= %d\n",length);


     for(i=1;i<length;i++){
    	 if(dTab[i]>th1){
            if(startY == 0){
        	   startY = i;
            }
            ccY++;
            av += dTab[i];
    	 }else{
             if(startY != 0 && ccY > 10 && iCand < 10){
            	 candidatesSt[iCand] = startY;
            	 candidatesH[iCand] = ccY;
            	 candidatesW[iCand] = av/ccY;
//         		sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",iCand, candidatesSt[iCand], candidatesH[iCand], candidatesW[iCand]);
//         		saveLog( "", bufLog );

            	 iCand++;
             }
    		 startY = 0;
    		 ccY=0;
    		 av = 0;
    	 }

     }

//     sprintf(bufLog,"iCand= %d------------------\n",iCand);
//     saveLog( "", bufLog );


	 for(i=0;i<iCand;i++){
//		 sprintf(bufLog,"iCand= %d, candidatesSt= %d, candidatesH = %d, candidatesW= %d\n",i, candidatesSt[i], candidatesH[i], candidatesW[i]);
//  		saveLog( "", bufLog );
		 if(candidatesW[i] > thW && candidatesH[i] > thH && idx < size) {
              y1[idx] = candidatesSt[i] + candidatesH[i];
		      y2[idx] = candidatesSt[i];
		      idx++;

//		 sprintf(bufLog,"*y1= %d, *y2= %d \n",*y1, *y2);
//  		 saveLog( "", bufLog );
		 }
	 }

   return idx;
}

void getBarcodeXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2){
//	char bufLog[200];

	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, y, j;
	int xCcBlack, xCcWhite;
	int thX, wPad_2;
	int maxX1, maxX2;
	wPad_2= wPad/2;
	thX = pixX/8 + 1;
	maxX1 = 0;maxX2 = 0;
	for(y=y2;y<y1;y++){
//		yn=height-1-y;
		yn=y;
		xCcBlack = 0;
		xCcWhite = 0;
		for(j=0;j<wPad_2-2;j++){
			nY=yn*wPad;
			nX = wPad_2 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		if(xCcBlack > maxX1) maxX1 = xCcBlack;
//		sprintf(bufLog,"y= %d, xCcBlack= %d, maxX1= %d\n",y, xCcBlack, maxX1);
//		saveLog( "", bufLog );


		xCcWhite = 0;
		xCcBlack = 0;
		for(j=0;j<wPad_2-2;j++){
//			yn=height-1-yn;
			nY=yn*wPad;
			nX = wPad_2 + j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		if(xCcBlack > maxX2) maxX2 = xCcBlack;


//		sprintf(bufLog,"y= %d, xCcBlack= %d, maxX2= %d\n",y, xCcBlack, maxX2);
//		saveLog( "", bufLog );


    }
    *x1 = wPad_2-maxX1;*x2 = wPad_2+maxX2;
}
void getLogoXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2){
//	char bufLog[200];

	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, y, j;
	int xCcBlack, xCcWhite;
	int thX, wPad_2;
	int maxX1, maxX2;
	wPad_2= wPad/2;
	thX = pixX/8 + 1;
	maxX1 = 0;maxX2 = 0;
	for(y=y2;y<y1;y++){
//		yn=height-1-y;
		yn=y;
		xCcBlack = 0;
		xCcWhite = 0;
		for(j=0;j<wPad_2-2;j++){
			nY=yn*wPad;
			nX = wPad_2 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		if(xCcBlack > maxX1) maxX1 = xCcBlack;
//		sprintf(bufLog,"y= %d, xCcBlack= %d, maxX1= %d\n",y, xCcBlack, maxX1);
//		saveLog( "", bufLog );


		xCcWhite = 0;
		xCcBlack = 0;
		for(j=0;j<wPad_2-2;j++){
//			yn=height-1-yn;
			nY=yn*wPad;
			nX = wPad_2 + j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		if(xCcBlack > maxX2) maxX2 = xCcBlack;


//		sprintf(bufLog,"y= %d, xCcBlack= %d, maxX2= %d\n",y, xCcBlack, maxX2);
//		saveLog( "", bufLog );


    }
    *x1 = wPad_2-maxX1;*x2 = wPad_2+maxX2;
}

/*
void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2, YlogoClassificationResult *resultLogo){

//	char bufLog[200];

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


    resultLogo->resultsNum = -7;
    resultLogo->candidates[0] = -7;
    resultLogo->candidates[1] = -7;
    resultLogo->candidates[2] = -7;
    resultLogo->confidende[0] = 0;
    resultLogo->confidende[1] = 0;
    resultLogo->confidende[2] = 0;

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

//	printf("-------------sizeof(*lpImage)= %d\n", sizeof(*lpImage));

//	sprintf(bufLog,"x1= %d, x2 = %d, y1= %d, y2 = %d\n", x1, x2, y1, y2);
//	saveLog( "", bufLog );

//    x1 = 0;
//    x2 = img->bi.biWidth-1;
//    printf("img->bi.biWidth= %d\n",img->bi.biWidth);
//    y1=0;
//    y2 = img->bi.biHeight-1;
    angle = 0;
//	    saveBMPbuffer (img->ptr, "barcodeImage.bmp", img->szOutPath);
//	printf("img->pixPtr= %u, img->ptr+offBits= %u\n", (unsigned)img->pixPtr, (unsigned)(img->ptr+offBits));
	//  	    saveLog("", buffLog);

//    angleFlag = 1;
//    printf("-------------y1= %d, x1= %d, x2= %d, y2= %d\n", y1, x1, x2, y2);
//  y1 = 780;
//  x1 -= 150;
//  x1 += 350;
//  x2 -= 90;
//  x2  = img->bi.biWidth-550;
//  y2 = y1+160;
//  y2 +=65;
//  	printf("-------------y1= %d, x1= %d, x2= %d, y2= %d\n", y1, x1, x2, y2);


      x1 -= 40;if(x1 < 0) x1 = 0;
      x2 += 60;if(x2 >= img->bi.biWidth) x2 = img->bi.biWidth-1;
      y1 -= 40;if(y1 < 0) y1 = 0;
      y2 += 40;if(y2 >= img->bi.biHeight) y2 = img->bi.biHeight-1;

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
//printf("------------ err= %d\n", err);
	if(err < 0) return;

//	 printf("------------ angle= %f\n", angle);

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
	{
//		YlogoClassificationResult result;

//		printBMPDataTest (img->ptr);
//		printBMPDataTest (hpFieldH.ptr);

//		 	    saveBMPbuffer (hpFieldH.ptr, img->szIMGName, img->szOutPath);

//printf("------------ width= %d, height= %d\n", width, height);
//fflush(stdout);

		if(retLogoClass >= 0){
			printf("------------ retLogoClass= %d\n", retLogoClass);
//			fflush(stdout);

		   *resultLogo = ylogo_classifier_classify(hpFieldH.ptr);
		}
//printf("after ------------ retLogoClass= %d\n", retLogoClass);
//fflush(stdout);

//		  {int nnn;
//			  for(nnn=0;nnn<resultLogo->resultsNum;nnn++){
//				  printf("------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo->candidates[nnn],nnn,resultLogo->confidende[nnn]);
//				  sprintf(bufLog,"------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo->candidates[nnn],nnn,resultLogo->confidende[nnn]);
//				  saveLog( "", bufLog );

//			  }
//		  }


//		printf("after ------------ img->szIMGName= %s, img->szOutPath= %s\n", img->szIMGName, img->szOutPath);
// 	    saveBMPbuffer (hpFieldH.ptr, img->szIMGName, img->szOutPath);

	}
	yl_free(hpFieldH.ptr);

}
*/
int getYtopForAng(int *hHistPreProc){
	int y_T = 0, i;
    for(i=1;i<=hHistPreProc[0]-1;i++) {
	    if(hHistPreProc[i] > 5 && hHistPreProc[i+1] > 5){y_T = i-1;break;}
    }
   return y_T;
}
int getYbotForAng(int *hHistPreProc){
	int y_B = 0, i;
    for(i=hHistPreProc[0];i>1;i--) {
	    if(hHistPreProc[i] > 5 && hHistPreProc[i-1] > 5){y_B = hHistPreProc[0]-i;break;}
    }
   return y_B;
}

extern int hHistPreProc[H_SIZE_PREPROC];
int testTypeFromBarCodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int *confOut){


//	int err;
	LPBYTE ptr;
	LPDWORD      lpImage;
	DWORD        offBits;
	HLPBYTE hpFieldH;
//	WORD width;
//	WORD height;
//	LPBITMAPFILEHEADER bf;
//	LPBITMAPINFOHEADER bi;
//	LPRGBQUAD          qd;
//	int angleFlag = 0;
//    double angle;
//    int  biSizeImage, biWidthPad;
	int typeInt = -1;


	*confOut = 0;
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

//    angle = 0;
    { //decode barcode
	int vHist[img->bi.biWidth];
	int i, ret;// , rsd;
	MASKDEF barMask[20];
	char strResult[64];
	int  confR[64];
	char cand2[64];
	char typeChar[64];

	strcpy(typeChar, "???");

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
//			printf("---------------left----------------\n");
//			for(ii=1;ii<=hHistPreProc[0];ii++) {
//				printf("hHistPreProc[ %d ]= %d\n",ii, hHistPreProc[ii]);
///			}
//			printf("---------------center----------------\n");
			startx = centx-dx;
	    	stopx = centx+dx;
	        histHTest_X1_X1(img->pixPtr, byteWidthHist, img->bi.biHeight,  startx, stopx, y1, y2, hHistPreProc);
	        yC_T = getYtopForAng(hHistPreProc);
		    yC_B = getYbotForAng(hHistPreProc);

//				printf("---------------left----------------\n");
//				for(ii=1;ii<=hHistPreProc[0];ii++) {
//					printf("hHistPreProc[ %d ]= %d\n",ii, hHistPreProc[ii]);
//				}
//				printf("---------------right----------------\n");
				startx = x2-dx;
		    	stopx = x2;
			    histHTest_X1_X1(img->pixPtr, byteWidthHist, img->bi.biHeight,  startx, stopx, y1, y2, hHistPreProc);
		        yR_T = getYtopForAng(hHistPreProc);
			    yR_B = getYbotForAng(hHistPreProc);

//					printf("---------------left----------------\n");
//					for(ii=1;ii<=hHistPreProc[0];ii++) {
//						printf("hHistPreProc[ %d ]= %d\n",ii, hHistPreProc[ii]);
//					}

//		    printf("-----yL_T= %d, yL_B= %d\n",yL_T, yL_B);
//		    printf("-----yC_T= %d, yC_B= %d\n",yC_T, yC_B);
//		    printf("-----yR_T= %d, yR_B= %d\n",yR_T, yR_B);
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
        ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);

//  printf("--- ret= %d; strResult= %s\n",ret, strResult);
        if((ret == 5 || ret == 10) && US_State == 2) ret = 0;
       for(i=0;i<strlen(strResult);i++){
    	   if(strResult[i] < '0' || strResult[i] > '9') (*confOut)--;
       }


        if(ret == 0){
        	   switch(US_State) {
        	      case 0:
        	      case 1:
        	    	  getTXLottoType(strResult, &typeInt, typeChar);break;
        	      case 2: getCALottoType(strResult, &typeInt, typeChar);break;
        	   }

//        	getTXLottoType(strResult, &typeInt, typeChar);
//        	printf("--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//        	sprintf(bufLog,"--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//            saveLog(bufLog, "");
        }
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
	    saveBMPbuffer (hpFieldH.ptr, img->szIMGName, img->szOutPath);
 	// ----------------------------- debugging
*/


    }


	yl_free(hpFieldH.ptr);
	return typeInt;
}

int createImageLogoBarcodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2){

//	char bufLog[200];

	int err;
	LPBYTE ptr;
	LPDWORD      lpImage;
	DWORD        offBits;
	HLPBYTE hpFieldH;
	WORD width;
	WORD height;

//	LPBITMAPFILEHEADER bf;
//	LPBITMAPINFOHEADER bi;
//	LPRGBQUAD          qd;

	int angleFlag = 0;
    double angle;

//    int  biSizeImage, biWidthPad;

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

//	sprintf(bufLog,"x1= %d, x2 = %d, y1= %d, y2 = %d\n", x1, x2, y1, y2);
//	saveLog( "", bufLog );

//    x1 = 0;
//    x2 = img->bi.biWidth-1;
//    printf("img->bi.biWidth= %d\n",img->bi.biWidth);
//    y1=0;
//    y2 = img->bi.biHeight-1;
    angle = 0;
//	    saveBMPbuffer (img->ptr, "barcodeImage.bmp", img->szOutPath);
//	printf("img->pixPtr= %u, img->ptr+offBits= %u\n", (unsigned)img->pixPtr, (unsigned)(img->ptr+offBits));
	//  	    saveLog("", buffLog);

    { //decode barcode
	int vHist[img->bi.biWidth];
	int i, ret;// , rsd;
	MASKDEF barMask[20];
	char strResult[64];
	int  confR[64];
	char cand2[64];
	char typeChar[64];
	int again = 0;
//	float ang;

//	float shfx1, shfx2, shfy1, shfy2;
//	float siN, coS, tG;
//	int angleTest;
//	float maxAlpha = 0.9;
//	float stepAlpha = 0.1;
//	int ysT, xsTa, xsTo;

	strcpy(typeChar, "???");

	WORD byteWidthHist = ((img->bi.biWidth + 31)/32)*4;
    for(i=0;i<20;i++)
            barMask[i].info = 128;

		again = 1;
		x1 -=40;
		x2 +=40;
		if(x1 < 0) x1 = 0;
		if(x2>= img->bi.biWidth) x2 = img->bi.biWidth-1;
        histVPtrPix(img->pixPtr, byteWidthHist, img->bi.biHeight, img->bi.biWidth-1, x1, x2, y1, y2, vHist);

/*
        {int nnH;

           for(nnH=1;nnH<=vHist[0];nnH++){
        	   sprintf(bufLog,"vHist[ %d ]= %d\n",nnH, vHist[nnH]);
        	   saveLog("", bufLog);
           }
        }
*/

        ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);
        if(ret == 0){
//printf("--------------strResult: %s\n", strResult);
        	again = 0;
       	   switch(US_State) {
       	      case 0:
       	      case 1:
       	    	  getTXLottoType(strResult, &typeInt, typeChar);break;
//       	      case 2: getCALottoType(strResult, &typeInt, typeChar);break;
//       	      case 3: getNYLottoType(strResult, &typeInt, typeChar);break;
       	   }
//        	printf("--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//        	sprintf(bufLog,"--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//            saveLog(bufLog, "");
        }
/*
        else{
//        	again = 1;
        	rsd = 0;
        	if(ret < 0)	{printf("cannot decode barcode \n");saveLog("cannot decode barcode", "\n");}
            if(ret >= 15) { printf("Start and Stop Problem \n");saveLog("Start and Stop Problem", "\n");rsd = ret%15;ret = 0;}
            if(ret >= 10) { printf("Start Problem \n");saveLog("Start Problem", "\n");rsd = ret%10;ret = 0;}
            if(ret >= 5)  { printf("Stop Problem \n");saveLog("Stop Problem", "\n");rsd = ret%5;ret = 0;}
            if(ret > 0) { rsd = 1;}
            if(rsd == 1){
                	printf("Opposite direction !!! \n");saveLog("Opposite direction !!!", "\n");}
        }
*/
//           	printf("--------------typeInt= %d \n", typeInt);

        if(typeInt < 0){
            histVPtrPix(img->pixPtr, byteWidthHist, img->bi.biHeight, img->bi.biWidth-1, x1, x2, y1, y2, vHist);
            ret = barDeCodeI25_verY (vHist, strResult);
//printf("--------------typeInt= %d \n", typeInt);
//           printf("2--------------strResult: %s\n", strResult);
            if(ret == 0){
           	   switch(US_State) {
           	      case 0:
           	      case 1:
           	    	  getTXLottoType(strResult, &typeInt, typeChar);break;
           	      case 2: getCALottoType(strResult, &typeInt, typeChar);break;
           	      case 3: getNYLottoType(strResult, &typeInt, typeChar);break;
           	   }
            }
            if(typeInt >= 0) again = 0;
            else{
            	switch(US_State) {
            	  case 3: ret = barDeCodeI25_verYInv (vHist, strResult);
            	          getNYLottoType(strResult, &typeInt, typeChar);break;
            	}
                if(typeInt >= 0) again = 0;
            }
        }

     angleFlag = 0;
     angle = 0;

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



  	    saveBMPbuffer (hpFieldH.ptr, img->szIMGName, img->szOutPath);

 	}
printf("              again= %d\n", again);
 	// ----------------------------- debugging
*/

//     if(again == 1)
//        angle = 5.0;



	if( again == 1) {

		yl_free(hpFieldH.ptr);
		ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
		if (ptr == NULL) {
			return typeInt;     // out of memory
		}
		hpFieldH.ptr = ptr;
		hpFieldH.size = 8;
		lpImage = (LPDWORD) ptr;
		*lpImage = 0;
		*(lpImage + 1) = offBits;

        angle = 3.0;
	 	err =
	 			extractField(img->pixPtr, img->bi.biWidth, img->bi.biHeight,
	 					x1, y1-16,
	 					x2, y1-16,
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

//		 printf("------------ second try angle= %f\n", angle);
//		 fflush(stdout);
//		 saveLog("------------ second try", "\n");

/*
 // ----------------------------- debugging
		 {
//			 char nameTmp[100];
//		     strcpy(nameTmp, img->szIMGName);
//		     strcat(&nameTmp[strlen(nameTmp)-4], "_2.bmp");

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
//	  	    saveBMPbuffer (hpFieldH.ptr, nameTmp, img->szOutPath);


		 }
 // ----------------------------- debugging
*/

	 	x1 = 0; x2= width-1;
	 	if(x2 >img->bi.biWidth-1) x2 = img->bi.biWidth-1;
     histVPtrPix(hpFieldH.ptr+offBits, ((width+31)/32)*4, height, img->bi.biWidth-1, x1, x2, 0, height-1, vHist);

     ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);
     if(ret == 0){
    	 again = 0;
     	   switch(US_State) {
     	      case 0:
       	      case 1:
     	    	  getTXLottoType(strResult, &typeInt, typeChar);break;
     	      case 2: getCALottoType(strResult, &typeInt, typeChar);break;
       	      case 3: getNYLottoType(strResult, &typeInt, typeChar);break;
     	   }

//     	getTXLottoType(strResult, &typeInt, typeChar);
//     	printf("--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//     	sprintf(bufLog,"--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//         saveLog(bufLog, "");
//         fflush(stdout);
     }

//     else{
//    	 again = 1;
//     	rsd = 0;
//     	if(ret < 0)	{printf("cannot decode barcode \n");saveLog("cannot decode barcode", "\n");}
//         if(ret >= 15) { printf("Start and Stop Problem \n");saveLog("Start and Stop Problem", "\n");rsd = ret%15;ret = 0;}
//         if(ret >= 10) { printf("Start Problem \n");saveLog("Start Problem", "\n");rsd = ret%10;ret = 0;}
//         if(ret >= 5)  { printf("Stop Problem \n");saveLog("Stop Problem", "\n");rsd = ret%5;ret = 0;}
//         if(ret > 0) { rsd = 1;}
//         if(rsd == 1){
//             	printf("Opposite direction !!! \n");saveLog("Opposite direction !!!", "\n");}
//     }

    if( again ==1 ) {
//    	        x1 =0;x2 = img->bi.biWidth-1;
//    	        if(x1 < 0) x1 = 0;
//    	        if(x2 >= img->bi.biWidth) x2 = img->bi.biWidth-1;

		yl_free(hpFieldH.ptr);
		ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
		if (ptr == NULL) {
			return typeInt;     // out of memory
		}
		hpFieldH.ptr = ptr;
		hpFieldH.size = 8;
		lpImage = (LPDWORD) ptr;
		*lpImage = 0;
		*(lpImage + 1) = offBits;

        angle = -3.0;
	err =
			extractField(img->pixPtr, img->bi.biWidth, img->bi.biHeight,
					x1, y1-16,
					x2, y1-16,
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

/*
// ----------------------------- debugging
	 {
//		char nameTmp[100];
//	     strcpy(nameTmp, img->szIMGName);
//	     strcat(&nameTmp[strlen(nameTmp)-4], "_3.bmp");

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
//	  	    saveBMPbuffer (hpFieldH.ptr, nameTmp, img->szOutPath);



	 }
*/
// ----------------------------- debugging

//	 printf("------------ third try angle= %f\n", angle);
//	 fflush(stdout);
//	 saveLog("------------ third try", "\n");
 	  x1 = 1; x2= width-1;
 	  if(x2 >img->bi.biWidth-2) x2 = img->bi.biWidth-2;
      histVPtrPix(hpFieldH.ptr+offBits, ((width+31)/32)*4, height, img->bi.biWidth-1, x1, x2, 1, height-1, vHist);

    ret = barDeCodeI25 (&vHist[1], vHist[0], (LPMASK)&barMask, 20, 20, 0, strResult, confR, cand2);
    if(ret == 0){
 	   switch(US_State) {
 	      case 0:
   	      case 1:
 	    	  getTXLottoType(strResult, &typeInt, typeChar);break;
 	      case 2: getCALottoType(strResult, &typeInt, typeChar);break;
   	      case 3: getNYLottoType(strResult, &typeInt, typeChar);break;
 	   }

//    	getTXLottoType(strResult, &typeInt, typeChar);
//    	printf("--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//    	sprintf(bufLog,"--------------ticket type: %s, typeIdx= %d \n", typeChar, typeInt);
//        saveLog(bufLog, "");
    }

/*
    else{
    	rsd = 0;
    	if(ret < 0)	{printf("cannot decode barcode \n");saveLog("cannot decode barcode", "\n");}
        if(ret >= 15) { printf("Start and Stop Problem \n");saveLog("Start and Stop Problem", "\n");rsd = ret%15;ret = 0;}
        if(ret >= 10) { printf("Start Problem \n");saveLog("Start Problem", "\n");rsd = ret%10;ret = 0;}
        if(ret >= 5)  { printf("Stop Problem \n");saveLog("Stop Problem", "\n");rsd = ret%5;ret = 0;}
        if(ret > 0) { rsd = 1;}
        if(rsd == 1){
            	printf("Opposite direction !!! \n");saveLog("Opposite direction !!!", "\n");}
    }
*/
   }

}

}


	yl_free(hpFieldH.ptr);
	return typeInt;
}
void printBMPDataTest (LPBYTE imgPtr) {
	saveLog( "", "printBMPData");

	char buf[1024];
	LPBITMAPFILEHEADER bf;
	 LPBITMAPINFOHEADER bi;

	    bf = (LPBITMAPFILEHEADER) imgPtr;
	    bi = (LPBITMAPINFOHEADER)(imgPtr + sizeof (BITMAPFILEHEADER));

	sprintf(buf, "bf.bfType= %x \n",getLeftBytes((int*)bf->bfType, 2));
	saveLog( "", buf );

	sprintf(buf, "bf->bfReserved1= %d \n",getLeftBytes((int*)bf->bfReserved1, 2));
	saveLog( "", buf );

	sprintf(buf,"bf.bfSize= %d, bf.bfOffBits= %d \n",getLeftBytes((int*)bf->bfSize, 4), getLeftBytes((int*)bf->bfOffBits, 4));
	saveLog( "", buf );

	sprintf(buf,"bi.biSize= %d, bi.biWidth= %d, bi.biHeight= %d  \n",getLeftBytes((int*)bi->biSize, 4), getLeftBytes((int*)bi->biWidth, 4), getLeftBytes((int*)bi->biHeight, 4));
	saveLog( "", buf );

	sprintf(buf,"bi.biPlanes= %d, bi.biBitCount= %d, bi.biCompression= %d  \n",getLeftBytes((int*)bi->biPlanes, 2), getLeftBytes((int*)bi->biBitCount, 2), getLeftBytes((int*)bi->biCompression, 4));
	saveLog( "", buf );

	sprintf(buf,"bi.biSizeImage= %d, bi.biXPelsPerMeter= %d, bi.biYPelsPerMeter= %d  \n",getLeftBytes((int*)bi->biSizeImage, 4), getLeftBytes((int*)bi->biXPelsPerMeter, 4), getLeftBytes((int*)bi->biYPelsPerMeter, 4));
	saveLog( "", buf );

	sprintf(buf,"bi.biClrUsed= %d bi.biClrImportant= %d \n",getLeftBytes((int*)bi->biClrUsed, 4), getLeftBytes((int*)bi->biClrImportant, 4));
	saveLog( "", buf );





    {int i;
    BYTE test;
       for(i=0;i<20;i++) {
         test = *(imgPtr + 54+i);
//         printf("i= %d, test= %c testx= %x\n",i, test, test);
         sprintf(buf, "i= %d, test= %d, testx= %x\n",i, test, test);
     	saveLog( "", buf );
       }
    }
/*
    for(i=0;i<8;i++) {
      test = *(img->ptr + 62+i);
      printf("i= %d, test= %d \n",i, test);
    }

    }
	 */
}

void copyPixelsTest(LPBYTE ptrOut, LPBYTE ptrInp, int width, int height){
	int i, k, idxInp, idxOut;
	int biWidthPad = ((width+31)/32)*4;
	for(i=0;i<height;i++){
		for(k=0;k<biWidthPad;k++){
			idxInp = (height-1-i)*biWidthPad+k;
			idxOut = (height-1-i)*biWidthPad+k;
			*(ptrOut+idxOut) = *(ptrInp+idxInp);
		}
	}

}
/*
void setTXlotteryTypes(void){
    typeOfLotteryTXT[0] = "LOTTO TEXAS"; //8
    typeOfLotteryTXT[1] = "PICK 3 TX";  //9
    typeOfLotteryTXT[2] = "CASH FIVE TX"; //10
    typeOfLotteryTXT[3] = "TWO STEP TX"; //12
    typeOfLotteryTXT[4] = "DAILY 4 TX"; //13
    typeOfLotteryTXT[5] = "POWER BALL TX"; //14
    typeOfLotteryTXT[6] = "MEGA MILLIONS TX"; //15
    typeOfLotteryTXT[7] = "ALL OR NOTHING TX"; //16
}
*/
void getTXLottoType(char *strResult, int *typeInt, char* typeChar){
   int len, k, num, tOut= -1;
   int k1, k2;


	 *typeInt = tOut;

//   printf("--------------strResult: %s\n", strResult);
//   sprintf(bufLog,"--------------strResult: %s\n", strResult);
//  saveLog(bufLog, "");
      len = strlen(strResult);
      if(len < 15){
	      strcpy(typeChar,"???");
	      return;
      }
      k1 = 13; k2 = 14;
      if((strResult[13] != strResult[len-6]) && strResult[len-1] == '0') {
	      k1 = len-7; k2 = k1+1;
      }

      if(strResult[k1] < '0' || strResult[k1] > '9' ||
   	     strResult[k2] < '0' || strResult[k2] > '9'){
	      strcpy(typeChar,"???");
	      return;
      }

      num = 0;
      if(strResult[k1] != '0')
 	     num =  (strResult[k1]-'0')*10;
      num +=  strResult[k2]-'0';

// printf("--------------num= %d\n", num);
// sprintf(bufLog,"--------------num= %d\n", num);
// saveLog(bufLog, "");

	  for(k=0;k<LOTTO_TYPES_NUM_TX;k++){
		  if(typeOfLotteryID_TX[k] == num) {tOut =k;break;}
	  }
// printf("--------------tOut= %d\n", tOut);
// sprintf(bufLog,"--------------tOut= %d\n", tOut);
// saveLog(bufLog, "");
	  if(tOut >= 0)
         strcpy(typeChar,typeOfLotteryTXT[tOut]);
	  else {
		  int n = 6, len = strlen(strResult);
		  if(strResult[len-1] == '0')
			   n = 7;
		  k1 = len-n;k2 = len-n+1;
	      if(strResult[k1] < '0' || strResult[k1] > '9' ||
	   	     strResult[k2] < '0' || strResult[k2] > '9'){
		      strcpy(typeChar,"???");
		      return;
	      }

	      num = 0;
	      if(strResult[len-n] != '0')
	 	     num =  (strResult[len-n]-'0')*10;
	      num +=  strResult[len-n+1]-'0';

//      printf("2--------------num= %d\n", num);
//      sprintf(bufLog,"2--------------num= %d\n", num);
//      saveLog(bufLog, "");
	  for(k=0;k<LOTTO_TYPES_NUM_TX;k++){
		  if(typeOfLotteryID_TX[k] == num) {tOut =k;break;}
	  }
// printf("2--------------tOut= %d\n", tOut);
// sprintf(bufLog,"2--------------tOut= %d\n", tOut);
// saveLog(bufLog, "");
	  if(tOut >= 0)
         strcpy(typeChar,typeOfLotteryTXT[tOut]);
	  else
		  strcpy(typeChar,"???");
	  }

	  *typeInt = tOut;
}
/*
typeOfLotteryTXT[0] = "LOTTO TEXAS"; //8
typeOfLotteryTXT[1] = "PICK 3 TX";  //9
typeOfLotteryTXT[2] = "CASH FIVE TX"; //10
typeOfLotteryTXT[3] = "TWO STEP TX"; //12
typeOfLotteryTXT[4] = "DAILY 4 TX"; //13
typeOfLotteryTXT[5] = "POWER BALL TX"; //14
typeOfLotteryTXT[6] = "MEGA MILLIONS TX"; //15
typeOfLotteryTXT[7] = "ALL OR NOTHING TX"; //16
*/
int changeThePRTNameTX(int n){
	if(n == 1) n = 4;
	if(n == 5) n = 6;
	return n;
}
void getTypeStrForPRT(char *typeStr, int n){
	   switch(US_State) {
	      case 0: // TX
   	      case 1:
	    	    n = changeThePRTNameTX(n);
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_TX)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryTXT[n]);
	    		break;
//	      case 1: // PA
//	    			strcpyPA_type(typeStr, n);
//	    		break;
	      case 2: // CA
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_CA)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryCAT[n]);
	    		break;
	      case 3: // NY
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_NY)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryNYT[n]);
	    		break;
	      default:
 			 strcpy(typeStr, "???");
	    		break;

	   }

}
void getTypeStr(char *typeStr, int n){
	   switch(US_State) {
	      case 0: // TX
   	      case 1:
//   	    	  printf("-------------------------------- n= %d\n", n);
	    	    if(n == 77){
	    	    	strcpy(typeStr, "SCRATCHOFF");break;
	    	    }
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_TX)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryTXT[n]);
	    		break;
//	      case 1: // PA
//	    			strcpyPA_type(typeStr, n);
//	    		break;
	      case 2: // CA
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_CA)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryCAT[n]);
	    		break;
	      case 3: // NY
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_NY)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryNYT[n]);
	    		break;
	      default:
    			 strcpy(typeStr, "???");
	    		break;

	   }
}
void getPrototypeName(char *typeStr, char *prototypeName){
	int len = strlen(typeStr);
	int i;
	strcpy(prototypeName, typeStr);
	for(i=0;i<len;i++){
		if(prototypeName[i] == ' ') prototypeName[i] = '_';

	}
}
/*
int getLogoRectangle(LPIMGDATA imgOut, char *mainPath, char *imgName, int* xL1, int* xL2, int* yL1, int* yL2){

#define WORKAREA_MAXSIZE_TMP 4000000

	int returnValue = 0;
	TICKETRESULTS ticketResults;
	LPTICKETDATA ticketData;
	HLPBYTE workAreaPtr;
	LPBYTE iDStructPtr;
	int  angle;
	RADON radonStruct;
	char error[200];
	RECTANGLE area;
	int ver;
	int ok;



	ticketTypeInt = -1;
	while(1){
		initTicketResults(&ticketResults);

//		img.ptr = NULL;
//		imgOut->ptr = NULL;
		imgOut->ptr = NULL;
		ticketData = (LPTICKETDATA)yl_calloc( "OCR_D", sizeof(TICKETDATA), sizeof(BYTE));
		if(ticketData == NULL) {
			sprintf(error, "yl_calloc(ticketData) error= %s\n", strerror(errno));
			returnValue = -100;
			break;
		}

		setGlobalDataPrototype(ticketData, mainPath);
		ticketData->fieldBMPPtr = NULL;
//		set_I_O_Path(&img, mainPath, imgName);
		set_I_O_Path(imgOut, mainPath, imgName);


		workAreaPtr.ptr = yl_calloc("workAreaPtr", WORKAREA_MAXSIZE_TMP, sizeof(BYTE));
		if(workAreaPtr.ptr == NULL) {
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "workAreaPtr; yl_calloc failed: %s\n", strerror(errno));
			returnValue = -130;
			break;

		}
		workAreaPtr.size = WORKAREA_MAXSIZE_TMP;
		ticketData->workArea = workAreaPtr;
//		initLib (ticketData);
		ticketData->error[0] = 0;
		//------------------------------------------
//		returnValue = LoadPrototypes (ticketData);
		//------------------------------------------

		//------------------------------------------

	    returnValue = LoadPrototypesForType (ticketData, "LOGO_TX");

		if(returnValue < 0){
			if(workAreaPtr.ptr != NULL) {
				yl_free(workAreaPtr.ptr);
				workAreaPtr.size = 0;
			}

			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "LoadPrototypes(); error= %s\n", ticketData->error);
			returnValue = -120;
			break;
		}
		area.wOriginX = 0;area.wOriginY = 0;
		area.wWidth = imgOut->bi.biWidth;area.wHeight = imgOut->bi.biHeight;
		ver = 0;

		iDStructPtr = NULL;
		ok = ticketRecognitionInit(imgOut, &radonStruct, &area, ver, &workAreaPtr,  &iDStructPtr,
				&angle, &ticketData->phi);
		if(ok < 0){
			ReleasePrototypes (ticketData);
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "ticketRecognitionInit(); error= %s\n", imgOut->error);
			returnValue = -130;
			break;
		}
//LOGD("0. size of workAreaPtr:  %ld\n", workAreaPtr.size);

		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		ticketData->iAngle = angle;
		ok = IdentifyTicketCall (&radonStruct, &workAreaPtr, ticketData, iDStructPtr);
		if(ok < 0){
			if(ticketData->ptrOut != NULL)
				yl_free(ticketData->ptrOut);
			ticketData->ptrOut = NULL;
			ReleasePrototypes (ticketData);
			if(iDStructPtr)
				yl_free(iDStructPtr);
			if(workAreaPtr.ptr != NULL) {
				yl_free(workAreaPtr.ptr);
				workAreaPtr.size = 0;
			}
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "IdentifyTicketCall(); error= %s\n", " unknown error");
			returnValue = -140;
			break;
		}
		ticketData->error[0] = 0;
		ok = LoadPrototype (&ticketData, 0);
		if(ok < 0){
			if(ticketData->ptrOut != NULL)
				yl_free(ticketData->ptrOut);
			ticketData->ptrOut = NULL;
			ReleasePrototypes (ticketData);
			if(iDStructPtr)
				yl_free(iDStructPtr);
			if(workAreaPtr.ptr != NULL) {
				yl_free(workAreaPtr.ptr);
				workAreaPtr.size = 0;
			}

			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "LoadPrototype(); error= %s\n", ticketData->error);
			returnValue = -150;
			break;
		}
		ok = ticketRecognitionStruct(imgOut, &radonStruct, ticketData->prototypePtr, &workAreaPtr, &ticketData->ptrTicketStruct,
				angle, ticketData->tgp);
		if(ok < 0 || workAreaPtr.size == 0){
			sprintf(error, "ticketRecognitionStruct(); error= %s\n", "unknown error");
			returnValue = -160;
			break;
		}
		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		ticketData->workArea = workAreaPtr;

		ok = extractLogoField ( imgOut, ticketData, &radonStruct, &ticketResults);

		if(ticketData->returnCode < 0){
			sprintf(error, "ProcessField(); error= %s\n", "unknown error");
			returnValue = -170;
			break;
		}


	}

    return returnValue;

}

int extractLogoField (LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPRADON radonStruct,  LPTICKETRESULTS ticketResults){

	DWORD  offset=0;         // offset to current field info block
	WORD   currentZone;    // number of current zone
	DWORD  protlth = 0;         // length of prototype file
	LPZONEDATA zone;
	double angleOut = 0;

	BYTE* lpProt = ticketDataPtr->prototypePtr;
	int  i, fieldNumber;
	fieldNumber = 1;

//	pMemNorm = NULL;
//	pMemMomentsFD = NULL;



//	while(fieldNumber<=ticketDataPtr->numFields) {


		offset      = ticketDataPtr->fieldChain;
		currentZone = 0xffff;
		protlth = ticketDataPtr->allocLength;



//		while  (1) {
			char        name[60];       // name of the field
			LPFIELDDATA field      = (LPFIELDDATA)((LPBYTE)ticketDataPtr + offset);
			BYTE*  descr      = lpProt + field->descr;
			DWORD attrOffset = field->descr + OffFlBuffer +
					*(WORD *)(descr + OffFlSize);
			BYTE*  attr       = lpProt + attrOffset;
			int        firstField;
//			FIELD      f;

			WORD logZoneNum  = 0xfff0;
			WORD validation  = 2;
			int failed      = 0;
			int extractOnly = 0;
			if (protlth >= attrOffset + OffFlSubset) {
				logZoneNum  = *(WORD*)(attr + OffFlLogZoneNo);
				validation  = *(WORD*)(attr + OffFlValidation);
				extractOnly = *(WORD*)(attr + OffFlExtractOnly);
			}
			offset = field->next;   // set pointer to the next field

			// reset skip if we are in the next logical zone
			if (currentZone != logZoneNum) {
				currentZone  = logZoneNum;
				firstField   = 1;
			} else
				firstField   = 0;

			for (i = 0; i < 32; i++)
				name[i] = attr[i];
			name[32] = 0;

			strcpy(field->name, name);

		{// totally skip field

			zone = &ticketDataPtr->zone[field->zone];  // get zone data block
			if(extractTicketsFieldcall(img, ticketDataPtr, field, radonStruct, ticketResults, name, &angleOut)  < 0)   {

				//                printf ("extractFormsField returns %i\n",
				//                		ticketDataPtr->returnCode);
				failed = 1;                  // set failure flag
				// set field quadrangle from prototype
				for (i = 0; i < sizeof (QUADRANGLE); i++) {
					field->quad.p1X = field->quad.p4X =
							*(WORD*)(descr);
					field->quad.p1Y = field->quad.p2Y =
							*(WORD*)(descr + 2);
					field->quad.p2X = field->quad.p3X =
							field->quad.p1X + *(WORD*)(descr + 4);
					field->quad.p3Y = field->quad.p4Y =
							field->quad.p1Y + *(WORD*)(descr + 6);
				}
			}


		}// totally skip field



//		}

//	}
//	if(pMemNorm != NULL) {yl_free(pMemNorm); pMemNorm = NULL;}
//	if(pMemMomentsFD != NULL) {yl_free(pMemMomentsFD); pMemMomentsFD = NULL;}
	return 0;

}
*/
//--------------------------------------------------------------
//----------------------------------------PA

int  typeOfLotteryID_PA[LOTTO_TYPES_NUM_PA][LOTTO_TYPES_MULTI_NUM_PA];
void setPAlotteryIDs(void){
	int i, j;
	for(i=0;i<LOTTO_TYPES_NUM_PA;i++)
		for(j=0;j<LOTTO_TYPES_MULTI_NUM_PA;j++)
			typeOfLotteryID_PA[i][j] = 0;

	 typeOfLotteryID_PA[0][0] = 31;
	 typeOfLotteryID_PA[1][0] = 29;
	 typeOfLotteryID_PA[1][1] = 32;
	 typeOfLotteryID_PA[2][0] = 30;
	 typeOfLotteryID_PA[3][0] = 31;
	 typeOfLotteryID_PA[4][0] = 77; // unknown yet
	 typeOfLotteryID_PA[5][0] = 28;
	 typeOfLotteryID_PA[6][0] = 27;
	 typeOfLotteryID_PA[7][0] = 77; // unknown yet
	 typeOfLotteryID_PA[8][0] = 77; // unknown yet
	 typeOfLotteryID_PA[9][0] = 77; // unknown yet

}
void setPAlotteryTypes(void){
    typeOfLotteryPAT[0] = "BIG 4 PA"; //31
    typeOfLotteryPAT[1] = "CASH 5 PA"; // 29, 32
    typeOfLotteryPAT[2] = "DAILY NUMBER PA"; // 30
    typeOfLotteryPAT[3] = "MATCH 6 PA"; //31
    typeOfLotteryPAT[4] = "INSTANT GAMES PA";
    typeOfLotteryPAT[5] = "POWER BALL PA"; // 28
    typeOfLotteryPAT[6] = "MEGA MILLIONS PA"; //27
    typeOfLotteryPAT[7] = "MILIONAIRE RAFFLE PA";
    typeOfLotteryPAT[8] = "QUINTO PA";
    typeOfLotteryPAT[9] = "TREASURE HUNT PA";

    setPAlotteryIDs();
}
void strcpyPA_type(char *typeStr, int n){
	int i, j, idx;
	int iT[LOTTO_TYPES_NUM_PA];

	typeStr[0] = 0;
    idx = 0;
	for(i=0;i<LOTTO_TYPES_NUM_PA;i++){
		for(j=0;j<LOTTO_TYPES_MULTI_NUM_PA;j++){
			if(typeOfLotteryID_PA[i][j] == 0) break;
			if(typeOfLotteryID_PA[i][j] == n) {iT[idx] = i;idx++;break;}
		}
	}
	if(idx == 0){
		strcpy(typeStr, "???");return;
	}
    for(i=0;i<idx;i++){
    	if(i > 0) strcat(typeStr, " or ");
		strcat(typeStr, typeOfLotteryPAT[iT[i]]);
    }

}

void createDesityTabPA1(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, i, j;
	int xCcBlack;
	int wPad_2, wPad_3;
	wPad_2= wPad/2;
	wPad_3= wPad/3;

	if(len > height-1)	len = height-1;

	densityTab[0] = len;
	for(i=0;i<len;i++){
//		yn = height-1 -i;
		yn = i;
		xCcBlack = 0;
		for(j=0;j<wPad_2-wPad_3;j++){
			nY=yn*wPad;
			nX = wPad_2 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) xCcBlack++;
		}
		for(j=0;j<wPad_2-wPad_3;j++){
			nY=yn*wPad;
			nX = wPad_2 + j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) xCcBlack++;
		}
		densityTab[i+1] = xCcBlack;

	}
}
void createDesityTabPA2(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, i, j;
	int xCcBlack;
	int wPad_5, wPad_3;
	wPad_5= wPad/5;
	wPad_3= wPad/3;

	if(len > height-1)	len = height-1;

	densityTab[0] = len;
	for(i=0;i<len;i++){
//		yn = height-1 -i;
		yn = i;
		xCcBlack = 0;
		for(j=0;j<wPad_5;j++){
			nY=yn*wPad;
			nX = wPad_3 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) xCcBlack++;
		}
		densityTab[i+1] = xCcBlack;

	}
}

void GetFirstY_PA(int *densityTab, int *y1 ){
	int i, len = densityTab[0];
	int ccWh = 0, idxWh = 0, idxStart;
	int tabCCWh[100];
	int tabCCWhidx[100];

	*y1 = 0;

	idxStart = 0;
	for(i=len;i>1;i--){
		if(densityTab[i] < 3) {
			if(ccWh == 0) idxStart = len-i;
			ccWh++;
		}
		else{
			if(ccWh > 0) {
			   if(idxWh < 100){
			      tabCCWh[idxWh] = ccWh;
			      tabCCWhidx[idxWh] = idxStart;
			   }
			   idxWh++;
			   ccWh = 0;
			}
		}
	}
//		 {int ii;
//		 				for(ii=0;ii<idxWh;ii++) {
//		 					sprintf(bufLog,"tabCCWh[ %d ]= %d, tabCCWhidx[ %d ] = %d\n",ii, tabCCWh[ii], ii, tabCCWhidx[ii]);
//		 					saveLog( "", bufLog );
//		 				}
//		 }
		 {int ii, idxM, cut1, dd, dd1, dd2, ok;
		  int maxTab[100];
		        idxM = 0;
		        ok = 0;
		 		for(ii=1;ii<idxWh;ii++) {
		 			if(tabCCWh[ii] > 100) maxTab[idxM++] = ii;
		 		}
		 		for(ii=0;ii<idxM;ii++) {
		 			cut1 = tabCCWhidx[maxTab[ii]]+tabCCWh[maxTab[ii]];
		 			dd1 = cut1 - tabCCWh[0];
		 			dd2 = tabCCWhidx[maxTab[ii]] - tabCCWh[0];
//printf("------ii= %d, dd1= %d, dd2= %d, tabCCWhidx[maxTab[ii]]= %d\n",ii, dd1, dd2, tabCCWhidx[maxTab[ii]]);
		 			if(dd2 > 200 && dd1 > 300){
		 				if(ii+1<idxM) {
		 					if(tabCCWh[maxTab[ii+1]] > tabCCWh[maxTab[ii]]){
		 						dd = tabCCWhidx[maxTab[ii+1]] - tabCCWh[0];
		 						if(dd < 1000) {
		 							ii++;
		 							cut1 = tabCCWhidx[maxTab[ii]]+tabCCWh[maxTab[ii]];
		 						}
		 					}
		 				}
		 				*y1 = cut1;
		 				ok = 1;
//printf("------ii= %d, cut1= %d\n",ii, cut1);
		 				break;
		 			}
		 		}
		 		if(ok == 0) {
		 			dd = 0;
		 			if(idxM > 0) {
					   dd = tabCCWhidx[maxTab[0]] - tabCCWh[0];
		 			}
					if(dd < 100){
				        idxM = 0;
				 		for(ii=1;ii<idxWh;ii++) {
				 			if(tabCCWh[ii] > 60) maxTab[idxM++] = ii;
				 		}

					}
			 		for(ii=0;ii<idxM;ii++) {
			 			cut1 = tabCCWhidx[maxTab[ii]]+tabCCWh[maxTab[ii]];
			 			dd1 = cut1 - tabCCWh[0];
			 			dd2 = tabCCWhidx[maxTab[ii]] - tabCCWh[0];
			 			if(dd2 > 200 && dd1 > 300){
			 				if(ii+1<idxM) {
			 					if(tabCCWhidx[maxTab[ii+1]] > tabCCWhidx[maxTab[ii]]){
			 						dd = tabCCWhidx[maxTab[ii+1]] - tabCCWh[0];
			 						if(dd < 1000) {
			 							ii++;
			 							cut1 = tabCCWhidx[maxTab[ii]]+tabCCWh[maxTab[ii]];
			 						}
			 					}
			 				}
			 				*y1 = cut1;
			 				ok = 1;
			 				break;
			 			}
			 		}

			 	   if(ok == 0 && idxM > 0) {
	 			      cut1 = tabCCWhidx[maxTab[0]]+tabCCWh[maxTab[0]];
 				      *y1 = cut1;
			 	   }
		 		}
	 }

}
int  getYShiftPA(int *Hist){
	int yShift = 0;
	int i, len, th = 4;
	int cc, ccW, idx;
	len = Hist[0];

	cc = 0;
	ccW = 0;
	for(i=1;i<len;i++){
		if(Hist[i] > th) cc++;
		else ccW++;
		if(ccW>2) break;
	}
//printf("1------cc= %d, \n",cc);
	if(cc > 50) return yShift;
	yShift += cc;
//printf("1------yShift= %d, \n",yShift);
	idx = cc;
	for(i=cc+1;i<len;i++){
		if(Hist[i] <= th) idx = i;
		else break;
	}
	yShift += (idx-cc);
//printf("1------cc= %d, idx= %d, yShift = %d \n",cc, idx, yShift);
	cc = 0;
	for(i=idx+1;i<len;i++){
		if(Hist[i] > th) cc++;
		else break;
	}
//printf("2------cc= %d, \n",cc);
	if(cc > 50) return yShift;
//	if(cc > 30) return yShift;
	yShift += cc;
	return yShift;
}
int getY2PA(int *Hist){
	int yShift = 0;
	int i, len, th = 5;
	int cc, idx, nn, ccS;
	len = Hist[0];

	yShift = 1;
	nn = 0;
	while(yShift < len && yShift < 80) {

		idx = yShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] <= th) cc++;
			else break;
		}
		ccS = cc;
//printf("1------cc= %d, nn= %d, len= %d\n",cc, nn, len);
//fflush(stdout);
        if(nn > 0 && yShift +cc > 80 && cc > 20) break;
		yShift += cc;
//	printf("1------yShift= %d\n",yShift);
//fflush(stdout);
		idx = yShift;
		cc = 0;
		for(i=idx+1;i<=len;i++){
			if(Hist[i] > th) cc++;
			else break;
		}
		ccS += cc;
		yShift += cc;
        if(ccS == 0) break;
//printf("2------yShift= %d\n",yShift);
//fflush(stdout);
        nn++;
	}
	return yShift;
}
int getY2PA_b(int *Hist){
	int yShift = 0;
	int i, len, th = 5;

	len = Hist[0];

	yShift = 0;
	for(i=len-1;i>100;i--){
	   if(Hist[i] <= th) yShift++;
		else break;
	}
//printf("1------cc= %d, nn= %d\n",cc, nn);
	yShift -= 5;
    if(yShift < 0) yShift = 0;
	return yShift;
}


int getX1PA(int *Hist){
	int xShift = 0;
	int i, len, th = 5;
	int cc, idx, ccS;
	int n, ccZ;
	len = Hist[0];

	xShift = 1;
	while(xShift < len-50) {
		idx = xShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] <= th) cc++;
			else break;
		}
		xShift += cc;
		ccS = cc;
		idx = xShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] > th) cc++;
			else {
				ccZ = 0;
				for(n=i+1;n<len;n++){
					if(Hist[n] <= th) ccZ++;
					else break;
				}
				if(ccZ > 4)
				    break;
			}
		}
		if(cc > 50) break;
		xShift += cc;
		ccS += cc;
        if(ccS == 0) break;
	}
	return xShift;
}

int getX2PA(int *Hist){
	int xShift = 0;
	int i, len, th = 5;
	int cc, idx, ccS;
	int n, ccZ;
	len = Hist[0];

	xShift = 1;
	while(xShift < len-50) {
		idx = xShift;
		cc = 0;
		for(i=len-idx-1;i>1;i--){
			if(Hist[i] <= th) cc++;
			else break;
		}
		xShift += cc;
		ccS = cc;
		idx = xShift;
		cc = 0;
		for(i=len-idx-1;i>1;i--){
			if(Hist[i] > th) cc++;
			else {
				ccZ = 0;
				for(n=i-1;n>1;n--){
					if(Hist[n] <= th) ccZ++;
					else break;
				}
				if(ccZ > 4)
				    break;
			}
		}
		if(cc > 50) break;
		xShift += cc;
		ccS += cc;
        if(ccS == 0) break;
	}
	return xShift;
}
int getX2PA_b(int *Hist){
	int yShift = 0;
	int i, len, th = 5;
	int cc, idx, nn, ccS;
	len = Hist[0];

	yShift = 1;
	nn = 0;
	while(yShift < len && yShift < 200) {

		idx = yShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] <= th) cc++;
			else break;
		}
        if(nn > 0 && yShift +cc > 200 && cc > 20) break;
		yShift += cc;
		ccS = cc;
//	printf("1------yShift= %d\n",yShift);
		idx = yShift;
		cc = 0;
		for(i=idx+1;i<=len;i++){
			if(Hist[i] > th) cc++;
			else break;
		}
		yShift += cc;
		ccS += cc;
        if(ccS == 0) break;
//printf("2------yShift= %d\n",yShift);
        nn++;
	}
	return yShift;
}
int getY1PA_b(int *Hist){
	int xShift = 0;
	int i, len, th = 5;
	int cc, idx, ccS;
	int n, ccZ;
	len = Hist[0];

	xShift = 1;
	while(xShift < len-50) {
		idx = xShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] <= th) cc++;
			else break;
		}
		xShift += cc;
		ccS = cc;
		idx = xShift;
		cc = 0;
		for(i=idx+1;i<len;i++){
			if(Hist[i] > th) cc++;
			else {
				ccZ = 0;
				for(n=i+1;n<len;n++){
					if(Hist[n] <= th) ccZ++;
					else break;
				}
				if(ccZ > 4)
				    break;
			}
		}
		if(cc > 70) break;
		xShift += cc;
		ccS += cc;
        if(ccS == 0) break;
	}
	return xShift;
}

int getBarcode_N(int *Hist, int idx, int *cnf1, int thCC, int thMax){
	int i, len, cc, th = 5;
	int idx1, max;
	idx1 = idx;
	len = Hist[0];
	*cnf1 = 0;
	while(idx1<len){
	   cc = 0;
	   for(i=idx1;i<len;i++){
		   if(Hist[i] <= th) cc++;
		   else break;
	   }
	   idx1 += cc+1;
	   cc = 0;max = 0;
	   for(i=idx1;i<len;i++){
		   if(Hist[i] > th) {cc++;if(max < Hist[i]) max = Hist[i];}
		   else break;
	   }
	   idx1 += cc+1;
	   if(cc > thCC && max > thMax) {*cnf1 = 30;break;}
	}
	return idx1;
}
int getCnfLevelbarcode_2D(int *Hist){
	int cnf = 0, cnf1;
	int idx;

// first barcode
	idx = 1;
	idx = getBarcode_N(Hist, idx, &cnf1, 60, 60);
	if(idx < Hist[0]/2)
	   cnf += cnf1;
//printf("1------idx= %d, Hist[0]= %d, cnf1= %d\n",idx, Hist[0], cnf1);

// second barcode
	idx = getBarcode_N(Hist, idx, &cnf1, 30, 30);
	if(idx < Hist[0]/2 +Hist[0]/3)
	   cnf += cnf1;

//printf("2------idx= %d, Hist[0]= %d, cnf1= %d\n",idx, Hist[0], cnf1);

// third barcode
	idx = getBarcode_N(Hist, idx, &cnf1, 60, 60);
	if(idx > Hist[0]/2)
	cnf += cnf1;

//printf("3------idx= %d, Hist[0]= %d, cnf1= %d\n",idx, Hist[0], cnf1);
	return cnf;
}

void printCode(int *codeLine, int codeIdx){
	 	 	 {int ii;
		 	 		for(ii=0;ii<codeIdx;ii++) {
		 		  	  sprintf(bufLog,"%d",codeLine[ii]);
		 			  saveLog( "", bufLog );
		 	 		}
		 	 		sprintf(bufLog,"\n");
		 	 		saveLog( "", bufLog );
		 	 	 }
}
int getPixelV(LPBYTE ptr, int xn, int yn, int wPad, int height){
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
int getCellNum(LPBYTE codeLine, int len){
	int i, cc, ccW, ccB;
	int idx;
	idx = 0;
	ccW = 0;ccB = 0;
//	for(i=0;i<len;i++){
	for(i=0;i<20;i++){
		if(codeLine[i] == 1) {ccB++;ccW=0;}
		else ccW++;
//printf("------codeLine[%d]= %d, ccB =%d, ccW= %d\n",i, codeLine[i], ccB, ccW);
//fflush(stdout);

		idx = i;
		if(ccW > 2) ccB = 0;
		if(ccB > 4) break;
	}
//printf("------idx= %d, ccB =%d, idx-ccB= %d\n",idx, ccB, idx-ccB);
//fflush(stdout);

	if(idx-ccB> 10) return 0;
//	idx = 0;
	ccW = 0;
	for(i=idx;i<len;i++){
		if(codeLine[i] == 0) ccW++;
		else ccW = 0;
		idx = i;
		if(ccW > 2) break;
	}
//printf("------idx= %d, len =%d\n",idx, len);
//fflush(stdout);

	cc = 0;
	while(idx<len) {
	   ccB = 0;
	   ccW = 0;
	   idx++;
	   for(i=idx;i<len;i++){
		 if(codeLine[i] == 1) ccB++;
		 else ccB=0;
		 idx = i;
		 ccW++;
		 if(ccB > 2) break;
	   }
	   if(ccW > 6) cc++;
	   ccB = 0;
	   ccW = 0;
	   for(i=idx;i<len;i++){
		 if(codeLine[i] == 0) ccW++;
		 else ccW=0;
		 idx = i;
		 ccB++;
		 if(ccW > 2) break;
	   }
	   if(ccB > 6) cc++;
	}

	return cc;
}
int getCellSize(LPBYTE codeLine, int len, int *x1){
	int i, idx, ccW, ccB;
	int idx2;
	ccW = 0;
	ccB = 0;
	for(i=0;i<20;i++){
		if(codeLine[i] == 1) {ccB++;ccW=0;}
		else ccW++;
		idx = i;
		if(ccW > 2) ccB = 0;
		if(ccB > 4) break;
	}
//printf("------idx= %d, ccB =%d, idx-ccB= %d\n",idx, ccB, idx-ccB);
    idx2 = idx-ccB;
	if(idx2> 10) return 10;
	ccW = 0;
	for(i=idx;i<len;i++){
		if(codeLine[i] == 0) ccW++;
		else ccW = 0;
		idx = i;
		if(ccW > 2) break;
	}
	*x1 = idx - ccW;
//    printf("1------idx= %d\n",idx);
     idx = idx-idx2-ccW;
	return idx;
}

int getStartBlack(LPBYTE codeLine, int len, int idxStart){
	int i, idx, ccB;
	   idx = 0;
	   ccB = 0;
	   for(i=idxStart;i<len;i++){
//   	printf("------codeLine[%d]= %d\n",i, codeLine[i]);
		 if(codeLine[i] == 1) ccB++;
		 else ccB=0;
		 idx = i;
		 if(ccB > 2) break;
	   }
//   printf("------idx= %d, ccB= %d\n",idx, ccB);
	return idx-ccB;
}
int getStartWhite(LPBYTE codeLine, int len, int idxStart){
	int i, idx, ccW;
	   idx = 0;
	   ccW = 0;
	   for(i=idxStart;i<len;i++){
		 if(codeLine[i] == 0) ccW++;
		 else ccW=0;
		 idx = i;
		 if(ccW > 2) break;
	   }
	return idx-ccW;
}
#define DM_LPATTERN_LEN 32
#define DM_EXCEPTION_LEN 8

#define MAX_CODEDM_LINE_LEN 400
int DataMatrix_16LPattern[DM_LPATTERN_LEN];
int DataMatrix_16LPatternX[DM_LPATTERN_LEN];
int DataMatrix_16LPatternY[DM_LPATTERN_LEN];
//int DataMatrix_16Exception[DM_EXCEPTION_LEN];
int DataMatrix_16CodeX[DM_EXCEPTION_LEN*8];
int DataMatrix_16CodeY[DM_EXCEPTION_LEN*8];

char outputDMCode[DM_CODE_LEN];
int numDMcodeChars;


void setDataMatrixCode(){

	DataMatrix_16LPattern[1] = -1;
	DataMatrix_16LPatternX[1] = 0;
	DataMatrix_16LPatternY[1] = 0;

	DataMatrix_16LPattern[2] = 1;
	DataMatrix_16LPatternX[2] = 0;
	DataMatrix_16LPatternY[2] = 0;

	DataMatrix_16LPattern[3] = -1;
	DataMatrix_16LPatternX[3] = 8;
	DataMatrix_16LPatternY[3] = 8;

	DataMatrix_16LPattern[4] = -1;
	DataMatrix_16LPatternX[4] = 16;
	DataMatrix_16LPatternY[4] = 16;

	DataMatrix_16LPattern[5] = 1;
	DataMatrix_16LPatternX[5] = 1;
	DataMatrix_16LPatternY[5] = 3;

	DataMatrix_16LPattern[6] = 1;
	DataMatrix_16LPatternX[6] = 3;
	DataMatrix_16LPatternY[6] = 1;

	DataMatrix_16LPattern[7] = -1;
	DataMatrix_16LPatternX[7] = 24;
	DataMatrix_16LPatternY[7] = 24;

	DataMatrix_16LPattern[8] = -1;
	DataMatrix_16LPatternX[8] = 32;
	DataMatrix_16LPatternY[8] = 32;

	DataMatrix_16LPattern[9] = 1;
	DataMatrix_16LPatternX[9] = 8;
	DataMatrix_16LPatternY[9] = 0;

	DataMatrix_16LPattern[10] = 1;
	DataMatrix_16LPatternX[10] = 6;
	DataMatrix_16LPatternY[10] = 2;

	DataMatrix_16LPattern[11] = 1;
	DataMatrix_16LPatternX[11] = 4;
	DataMatrix_16LPatternY[11] = 4;

	DataMatrix_16LPattern[12] = 1;
	DataMatrix_16LPatternX[12] = 2;
	DataMatrix_16LPatternY[12] = 6;

	DataMatrix_16LPattern[13] = 1;
	DataMatrix_16LPatternX[13] = 0;
	DataMatrix_16LPatternY[13] = 8;

	DataMatrix_16LPattern[14] = -1;
	DataMatrix_16LPatternX[14] = 40;
	DataMatrix_16LPatternY[14] = 40;

	DataMatrix_16LPattern[15] = -1;
	DataMatrix_16LPatternX[15] = 48;
	DataMatrix_16LPatternY[15] = 48;

	DataMatrix_16LPattern[16] = 1;
	DataMatrix_16LPatternX[16] = 1;
	DataMatrix_16LPatternY[16] = 11;

	DataMatrix_16LPattern[17] = 1;
	DataMatrix_16LPatternX[17] = 3;
	DataMatrix_16LPatternY[17] = 9;

	DataMatrix_16LPattern[18] = 1;
	DataMatrix_16LPatternX[18] = 5;
	DataMatrix_16LPatternY[18] = 7;

	DataMatrix_16LPattern[19] = 1;
	DataMatrix_16LPatternX[19] = 7;
	DataMatrix_16LPatternY[19] = 5;

	DataMatrix_16LPattern[20] = 1;
	DataMatrix_16LPatternX[20] = 9;
	DataMatrix_16LPatternY[20] = 3;

	DataMatrix_16LPattern[21] = 1;
	DataMatrix_16LPatternX[21] = 11;
	DataMatrix_16LPatternY[21] = 1;

	DataMatrix_16LPattern[22] = -1;
	DataMatrix_16LPatternX[22] = 56;
	DataMatrix_16LPatternY[22] = 56;

	DataMatrix_16LPattern[23] = 1;
	DataMatrix_16LPatternX[23] = 12;
	DataMatrix_16LPatternY[23] = 4;

	DataMatrix_16LPattern[24] = 1;
	DataMatrix_16LPatternX[24] = 10;
	DataMatrix_16LPatternY[24] = 6;

	DataMatrix_16LPattern[25] = 1;
	DataMatrix_16LPatternX[25] = 8;
	DataMatrix_16LPatternY[25] = 8;

	DataMatrix_16LPattern[26] = 1;
	DataMatrix_16LPatternX[26] = 6;
	DataMatrix_16LPatternY[26] = 10;

	DataMatrix_16LPattern[27] = 1;
	DataMatrix_16LPatternX[27] = 4;
	DataMatrix_16LPatternY[27] = 12;

	DataMatrix_16LPattern[28] = 1;
	DataMatrix_16LPatternX[28] = 7;
	DataMatrix_16LPatternY[28] = 13;

	DataMatrix_16LPattern[29] = 1;
	DataMatrix_16LPatternX[29] = 9;
	DataMatrix_16LPatternY[29] = 11;

	DataMatrix_16LPattern[30] = 1;
	DataMatrix_16LPatternX[30] = 11;
	DataMatrix_16LPatternY[30] = 9;

	DataMatrix_16LPattern[31] = 1;
	DataMatrix_16LPatternX[31] = 13;
	DataMatrix_16LPatternY[31] = 7;

//1
     DataMatrix_16CodeX[0] = 2;DataMatrix_16CodeX[1] = 2;DataMatrix_16CodeX[2] = 3;DataMatrix_16CodeX[3] = 3;
     DataMatrix_16CodeX[4] = 3;DataMatrix_16CodeX[5] = 4;DataMatrix_16CodeX[6] = 4;DataMatrix_16CodeX[7] = 4;
     DataMatrix_16CodeY[0] = 14;DataMatrix_16CodeY[1] = 15;DataMatrix_16CodeY[2] = 14;DataMatrix_16CodeY[3] = 15;
     DataMatrix_16CodeY[4] = 0;DataMatrix_16CodeY[5] = 14;DataMatrix_16CodeY[6] = 15;DataMatrix_16CodeY[7] = 0;
//3
     DataMatrix_16CodeX[8] = 14;DataMatrix_16CodeX[9] = 14;DataMatrix_16CodeX[10] = 15;DataMatrix_16CodeX[11] = 15;
     DataMatrix_16CodeX[12] = 15;DataMatrix_16CodeX[13] = 0;DataMatrix_16CodeX[14] = 0;DataMatrix_16CodeX[15] = 0;
     DataMatrix_16CodeY[8] = 2;DataMatrix_16CodeY[9] = 3;DataMatrix_16CodeY[10] = 2;DataMatrix_16CodeY[11] = 3;
     DataMatrix_16CodeY[12] = 4;DataMatrix_16CodeY[13] = 2;DataMatrix_16CodeY[14] = 3;DataMatrix_16CodeY[15] = 4;
//4
     DataMatrix_16CodeX[16] = 15;DataMatrix_16CodeX[17] = 15;DataMatrix_16CodeX[18] = 0;DataMatrix_16CodeX[19] = 0;
     DataMatrix_16CodeX[20] = 0;DataMatrix_16CodeX[21] = 1;DataMatrix_16CodeX[22] = 1;DataMatrix_16CodeX[23] = 1;
     DataMatrix_16CodeY[16] = 5;DataMatrix_16CodeY[17] = 6;DataMatrix_16CodeY[18] = 5;DataMatrix_16CodeY[19] = 6;
     DataMatrix_16CodeY[20] = 7;DataMatrix_16CodeY[21] = 5;DataMatrix_16CodeY[22] = 6;DataMatrix_16CodeY[23] = 7;
//7
     DataMatrix_16CodeX[24] = 5;DataMatrix_16CodeX[25] = 5;DataMatrix_16CodeX[26] = 6;DataMatrix_16CodeX[27] = 6;
     DataMatrix_16CodeX[28] = 6;DataMatrix_16CodeX[29] = 7;DataMatrix_16CodeX[30] = 7;DataMatrix_16CodeX[31] = 7;
     DataMatrix_16CodeY[24] = 15;DataMatrix_16CodeY[25] = 0;DataMatrix_16CodeY[26] = 15;DataMatrix_16CodeY[27] = 0;
     DataMatrix_16CodeY[28] = 1;DataMatrix_16CodeY[29] = 15;DataMatrix_16CodeY[30] = 0;DataMatrix_16CodeY[31] = 1;
//8
     DataMatrix_16CodeX[32] = 10;DataMatrix_16CodeX[33] = 10;DataMatrix_16CodeX[34] = 11;DataMatrix_16CodeX[35] = 11;
     DataMatrix_16CodeX[36] = 11;DataMatrix_16CodeX[37] = 12;DataMatrix_16CodeX[38] = 12;DataMatrix_16CodeX[39] = 12;
     DataMatrix_16CodeY[32] = 14;DataMatrix_16CodeY[33] = 15;DataMatrix_16CodeY[34] = 14;DataMatrix_16CodeY[35] = 15;
     DataMatrix_16CodeY[36] = 0;DataMatrix_16CodeY[37] = 14;DataMatrix_16CodeY[38] = 15;DataMatrix_16CodeY[39] = 0;
//14
     DataMatrix_16CodeX[40] = 14;DataMatrix_16CodeX[41] = 14;DataMatrix_16CodeX[42] = 15;DataMatrix_16CodeX[43] = 15;
     DataMatrix_16CodeX[44] = 15;DataMatrix_16CodeX[45] = 0;DataMatrix_16CodeX[46] = 0;DataMatrix_16CodeX[47] = 0;
     DataMatrix_16CodeY[40] = 10;DataMatrix_16CodeY[41] = 11;DataMatrix_16CodeY[42] = 10;DataMatrix_16CodeY[43] = 11;
     DataMatrix_16CodeY[44] = 12;DataMatrix_16CodeY[45] = 10;DataMatrix_16CodeY[46] = 11;DataMatrix_16CodeY[47] = 12;
//15
     DataMatrix_16CodeX[48] = 15;DataMatrix_16CodeX[49] = 15;DataMatrix_16CodeX[50] = 0;DataMatrix_16CodeX[51] = 0;
     DataMatrix_16CodeX[52] = 0;DataMatrix_16CodeX[53] = 1;DataMatrix_16CodeX[54] = 1;DataMatrix_16CodeX[55] = 1;
     DataMatrix_16CodeY[48] = 13;DataMatrix_16CodeY[49] = 14;DataMatrix_16CodeY[50] = 13;DataMatrix_16CodeY[51] = 14;
     DataMatrix_16CodeY[52] = 15;DataMatrix_16CodeY[53] = 13;DataMatrix_16CodeY[54] = 14;DataMatrix_16CodeY[55] = 15;
//22
     DataMatrix_16CodeX[56] = 13;DataMatrix_16CodeX[57] = 13;DataMatrix_16CodeX[58] = 14;DataMatrix_16CodeX[59] = 14;
     DataMatrix_16CodeX[60] = 14;DataMatrix_16CodeX[61] = 15;DataMatrix_16CodeX[62] = 15;DataMatrix_16CodeX[63] = 15;
     DataMatrix_16CodeY[56] = 15;DataMatrix_16CodeY[57] = 0;DataMatrix_16CodeY[58] = 15;DataMatrix_16CodeY[59] = 0;
     DataMatrix_16CodeY[60] = 1;DataMatrix_16CodeY[61] = 15;DataMatrix_16CodeY[62] = 0;DataMatrix_16CodeY[63] = 1;


}
int getLpatternCode(LPBYTE codeDM, int len, int lx, int ly, char* chOut){
    int code, m;
    int x, y;

    m = 1;code = 0;
    for(x=2;x>0;x--){
        for(y=2;y>=0;y--){
            code += codeDM[(ly+y)*len+lx+x]*m;
            m *= 2;
        }
    }
    x = 0;
    for(y=1;y>=0;y--){
        code += codeDM[(ly+y)*len+lx+x]*m;
        m *= 2;
    }
//  printf("code = %d\n", code);
    if(code >= 130){
        code -= 130;
//  printf("2, code = %d\n", code);
        chOut[0] = '0'+ (char)(code/10);
        chOut[1] = '0'+ (char)(code - (code/10)*10);
// printf("chOut[0]= %d, chOut[1]= %d\n", chOut[0], chOut[1]);
        return 2;
    }
    code--;
    chOut[0] = (char)code;
    return 1;
}

 int getnotLpatternCode(LPBYTE codeDM, int len, int lx, int ly, char* chOut){
	    int code, m;
	    int i, x, y;
	    m = 1;code = 0;
//printf("-------##################\n");
//printf("------lx= %d, ly= %d\n", lx, ly);
	    for(i=1;i<=8;i++){
//printf("------lx+8-i= %d\n",lx+8-i);
	    	x = DataMatrix_16CodeX[lx+8-i];
	    	y = DataMatrix_16CodeY[ly+8-i];
// printf("------i= %d, x= %d, y= %d, codeDM[y*len+x]= %d, m= %d\n",i, x, y, codeDM[y*len+x], m);
            code += codeDM[y*len+x]*m;

            m *= 2;
	    }
// printf("------code= %d\n",code);
	    if(code >= 130){
	        code -= 130;
// 	 printf("------code= %d\n",code);
	        chOut[0] = '0'+ (char)(code/10);
	        chOut[1] = '0'+ (char)(code - (code/10)*10);
	        return 2;
	    }
	    code--;
	    chOut[0] = (char)code;
	    return 1;

 }
void decodeDM16(LPBYTE codeDM, int len){
    int i, cc, nn;
    int lx,ly;
    char chOut[2];
    setDataMatrixCode();
    for(i=0;i<DM_CODE_LEN;i++)
	   outputDMCode[i] = 0;
    nn = 0;
    for(i=1;i<DM_LPATTERN_LEN;i++) {
  	     lx = DataMatrix_16LPatternX[i];
  	     ly = DataMatrix_16LPatternY[i];
//printf("---!!!!!!!!---i = %d, lx= %d, ly= %d\n",i, lx, ly);

  	     if(lx <0 || ly < 0)
  	    	outputDMCode[nn++] = '?';
  	     else{
//printf("------DataMatrix_16LPattern[i] = %d, nn= %d\n",DataMatrix_16LPattern[i], nn);

    	   if(DataMatrix_16LPattern[i] > 0) {
    	      cc = getLpatternCode(codeDM, len, lx, ly, chOut);
    	      outputDMCode[nn++] = chOut[0];
    	      if(cc > 1)
    		     outputDMCode[nn++] = chOut[1];
// printf("1------chOut[0] = %c,cc= %d, nn=  %d\n",chOut[0],cc, nn);
    	   }
    	   else{
     	      cc = getnotLpatternCode(codeDM, len, lx, ly, chOut);
     	      outputDMCode[nn++] = chOut[0];
//  printf("2------chOut[0] = %c\n",chOut[0]);
     	      if(cc > 1)
     		     outputDMCode[nn++] = chOut[1];
    	  }
  	     }
  	     if(nn > 27) break;
    }
    numDMcodeChars = nn;
}
void getfirstXY_LT(LPBYTE codeLine, int *colNum, int kY, int *X, int *Y){
int i, j,startX, startY, cc, ccCount;
int n, n1=0, startX1, startY1;
int jx,jy;

  startX1 = 0; startY1 = 0;
  startX = 0; startY = 0;
  ccCount = 0;
  for(i=0;i<50;i++){
      cc = 0;n = 0;
//printf("----------------------------i= %d\n",i);
	  for(j=0;j<50;j++){
		  jx = startX-j;
		  jy = startY+j;
		  if(jx < 0 || jy >= colNum[jx]) break;
//printf("------i= %d, jx= %d, jy= %d, codeLine[]= %d\n",i, jx, jy, codeLine[jx*MAX_CODEDM_LINE_LEN+jy]);
//printf("------i= %d, startX= %d, startY= %d, j= %d\n",i, startX, startY, j);
		  if(codeLine[jx*MAX_CODEDM_LINE_LEN+jy] == 1) cc++;
		  n = j;
		  if(cc>1) break;
	  }
	  if(cc>1) ccCount++;
	  else{ccCount = 0;}
	  if(ccCount==1) {
//		  printf("------i= %d, startX= %d, startY= %d, n= %d\n",i, startX, startY, n);
		  startX1 = startX+1; startY1 = startY-1;n1 = n;}
	  if(ccCount>2) {break;}
	  startX++;
  }
  *X = startX1-n1;*Y = startY1+n1;
}

void getfirstXY_RB(LPBYTE codeLine, int *colNum, int kY, int *X, int *Y){
int i, j,startX, startY, cc, ccCount;
int n, n1=0, startX1, startY1;
int jx,jy;

  startX1 = kY-1; startY1 = colNum[kY-1];
  startX = kY-1; startY = colNum[kY-1];
  ccCount = 0;
  for(i=0;i<50;i++){
      cc = 0;n = 0;
	  for(j=0;j<50;j++){
		  jx = startX-j;
		  jy = startY+j;
		  if(jx < 0 || jy >= colNum[jx]) break;
		  if(codeLine[jx*MAX_CODEDM_LINE_LEN+jy] == 1) cc++;
		  n = j;
		  if(cc>1) break;
	  }
	  if(cc>1) ccCount++;
	  else{ccCount = 0;}
	  if(ccCount==1) {startX1 = startX+1; startY1 = startY-1;n1 = n;}
	  if(ccCount>2) break;
	  startY--;
  }
  *X = startX1-n1+1;*Y = startY1+n1+1;
}

void getfirstXY_LB(LPBYTE codeLine, int *colNum, int kY, int *X, int *Y){
int i, j,startX, startY, cc, ccCount;
int n, n1=0, startX1, startY1;
int jx,jy;

  startX1 = kY-1; startY1 = 0;
  startX = kY-1; startY = 0;
  ccCount = 0;
  for(i=0;i<50;i++){
      cc = 0;n = 0;
	  for(j=0;j<50;j++){
		  jx = startX+j;
		  jy = startY+j;
		  if(jx >= kY || jy >= colNum[jx]) break;
		  if(codeLine[jx*MAX_CODEDM_LINE_LEN+jy] == 1) cc++;
		  n = j;
		  if(cc>1) break;
	  }
	  if(cc>1) ccCount++;
	  else{ccCount = 0;}
	  if(ccCount==1) {startX1 = startX-1; startY1 = startY-1;n1 = n;}
	  if(ccCount>2) break;
	  startX--;
  }
  *X = startX1+n1+1;*Y = startY1+n1+1;
}

void barcode_DataMatrix(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2){
#define MAX_CODEDM_LEN 16
	int   yy, xx;
	BYTE codeLine[MAX_CODEDM_LINE_LEN][MAX_CODEDM_LINE_LEN];
	BYTE codeDM[MAX_CODEDM_LEN][MAX_CODEDM_LEN];
	int colNum[400];
	int cellSizeTabX[MAX_CODEDM_LEN];
	int cellSizeTabY[MAX_CODEDM_LEN];
	int codeIdx, cc, yStart, xStart;
	int kY;
//	int xTop=0, xBot=0, yTop=0, yBot=0;

//	printf("------x1= %d, x2= %d, y2= %d, y2= %d\n",x1, x2, y1, y2);
//	fflush(stdout);

	{
	  BYTE codeLineTmp[400][400];

	cc = 0;
	yStart = y1;
	for(yy=y1;yy<=y2;yy++) {
		for(xx=x1;xx<x2;xx++) {
			if(getPixelV(ptrPix, xx, yy, widthImg, hightImg) == 0) cc++;
		}
		yStart = yy;
		if(cc > 15) break;
	}
	kY = 0;
	for(yy=yStart;yy<=y2;yy++) {
//		xStart = x1;
//		cc = 0;
//		for(xx=x1;xx<x2;xx++) {
//			if(getPixelV(ptrPix, xx, yy, widthImg, hightImg) == 0) cc++;
//			xStart = xx;
//			if(cc > 2) break;
//		}
//		xStart -= cc;
//		if(yy-yStart == 10) {xTop = xStart;yTop = yy;}
//		if(y2-yy == 40 && x2-xStart > 100) {xBot = xStart;yBot = yy;}
//		if(y2-yy == 20 && x2-xStart > 100) {xBot = xStart;yBot = yy;}
//		if(y2-yy == 10 && x2-xStart > 100) xBot = xStart;
//if(y2-yy >50 ) xBot = xStart;
//printf("------yy= %d, yy-yStart= %d, y2-yy= %d, xStart-x1= %d, xStart= %d\n",yy, yy-yStart, y2-yy, xStart-x1, xStart);
//fflush(stdout);
		codeIdx  = 0;
//		for(xx=xStart;xx<x2;xx++) {
			for(xx=x1;xx<x2;xx++) {
			if(kY<400 && codeIdx<400)
			   codeLineTmp[kY][codeIdx++] = 1-getPixelV(ptrPix, xx, yy, widthImg, hightImg);

		}
		if(codeIdx < 10 && kY > 10) break;

//	printf("------row= %d, colNum[ %d ]= %d\n",yy-yStart, kY, codeIdx);
//	fflush(stdout);

	    if(kY<400)
	       colNum[kY] = codeIdx;
		kY++;
	}

/*
	 {int i1, i2;
	 		for(i1=0;i1<kY;i1++) {
		 		for(i2=0;i2<colNum[i1];i2++) {
		  	       sprintf(bufLog,"%d",codeLineTmp[i1][i2]);
			       saveLog( "", bufLog );
		 		}
		 		sprintf(bufLog," line= %d\n",i1);
		 		saveLog( "", bufLog );

	 		}
	 }
*/
	{int XL, YL, XR, YR, XT, YT;
	 int X, Y;
	 double Tg = 0;
	 int corr = 0, corrV = 1;
	 int ySh;

	   getfirstXY_LB(&codeLineTmp[0][0], colNum, kY, &XL, &YL);
//printf("------_LB X= %d, Y= %d\n",XL, YL);
//fflush(stdout);
	   getfirstXY_RB(&codeLineTmp[0][0], colNum, kY, &XR, &YR);
//printf("------_RB X= %d, Y= %d\n",XR, YR);
//fflush(stdout);
	   getfirstXY_LT(&codeLineTmp[0][0], colNum, kY, &XT, &YT);
//printf("------_LT X= %d, Y= %d\n",XT, YT);
//fflush(stdout);

       Y = YR-YL;
       X = XR-XL;
       if(Y > 0)
         Tg = (double)(X)/(double)(Y);
       if(Tg != 0)
           corr = (int)(1.0/Tg +0.5);
	   if(corr < 0) {corrV = -1;corr = -corr;}
	  xStart = YT;
	  for(yy=0;yy<kY;yy++) {
		  ySh = yy;
		 if(yy>0 && corr>0 && yy%corr == 0) {xStart += corrV;if(xStart<0) xStart = 0;}

	 		for(xx=xStart;xx<colNum[yy];xx++) {
	 			if(xx>0 && corr>0 && xx%corr == 0) {ySh += corrV;}
	 			   if(ySh < 0 || ySh >= kY)
	 				  codeLine[yy][xx] = 0;
	 			   else
		 			codeLine[yy][xx] = codeLineTmp[ySh][xx];
	 		}
	 		colNum[yy] -= xStart;
		}
	}


   }

/*
	 {int i1, i2;
	 		for(i1=0;i1<kY;i1++) {
		 		for(i2=0;i2<colNum[i1];i2++) {
		  	       sprintf(bufLog,"%d",codeLine[i1][i2]);
			       saveLog( "", bufLog );
		 		}
		 		sprintf(bufLog," line= %d\n",i1);
		 		saveLog( "", bufLog );

	 		}
	 }

*/
    {int i1, i2, st = 30;
     int nn;
     int  startCellsY = 0;
     int startCellsX1 = 0, startCellsY1 = 0;
     int startCellsX2 = 0, startCellsY2 = kY;
     int cellSize = 0, startCellsX1av = 0;
     BYTE cellNum[400];

        if(st > kY) st = kY;
	    for(i1=0;i1<st;i1++) {
//	 		for(i2=0;i2<colNum[i1];i2++) {
	 			cellNum[i1] = getCellNum(codeLine[i1], colNum[i1]);
//	 		}
	    }


//   for(i1=0;i1<st;i1++) {
//        printf("------cellNum[%d]= %d\n",i1,cellNum[i1]);
//        fflush(stdout);
//   }

	    for(i1=0;i1<st-2;i1++) {
	    	if(cellNum[i1] > 15 && cellNum[i1+1] > 15 && cellNum[i1+2] > 15){
	    		startCellsY1 = i1;break;
	    	}
	    }

//    printf("------_startCellsY1= %d\n",startCellsY1);
//    fflush(stdout);

	    startCellsY = startCellsY1;
	    for(i1=startCellsY1;i1<st-2;i1++) {
	    	if(cellNum[i1] < 15 && cellNum[i1+1] < 15 && cellNum[i1+2] < 15){
	    		startCellsY1 = i1;break;
	    	}
	    }


	    cc = 0;
	    startCellsX1 = 0;
 		for(i1=startCellsY;i1<startCellsY+4;i1++) {
	 			cc += getCellSize(codeLine[i1], colNum[i1], &startCellsX1av);
	 			startCellsX1 += startCellsX1av;
 		}

 		cellSize = cc/4;
 		startCellsX1 /= 4;

 		nn = 0;

	    for(i1=kY-1;i1>kY-st;i1--) {

//	 		for(i2=0;i2<colNum[i1];i2++) {
	 			cellNum[nn] = getCellNum(codeLine[i1], colNum[i1]);
//	 		}
           nn++;
	    }



        for(i1=0;i1<st-2;i1++) {
	       if(cellNum[i1] > 5 && cellNum[i1+1] > 5 && cellNum[i1+2] > 5){
	    		startCellsY2 = kY-i1-1;break;
	    	}
	    }

//	printf("------cellSize= %d\n",cellSize);
//    fflush(stdout);
//     printf("------startCellsX1= %d, startCellsY= %d, startCellsY1= %d, startCellsY2= %d \n",startCellsX1,startCellsY, startCellsY1, startCellsY2);


        {int ix, iy, nx, ny;
         int ix1, iy1;
         int ix2, iy2;
         int ccB, ccW;
// get code
         for(nn = 0;nn<MAX_CODEDM_LEN;nn++){
      	   cellSizeTabX[nn] = 0;
      	   cellSizeTabY[nn] = 0;
         }

// X borders
//   	   	printf("------startCellsY= %d \n",startCellsY);
           ix = startCellsX1;
           ny = 0;
           for(nn = 0;nn<MAX_CODEDM_LEN/2;nn++){
        	  ix += 2;
        	  cellSize = 0;
      	      for(i1=startCellsY;i1<startCellsY+4;i1++) {
      	    	cellSize += getStartBlack(codeLine[i1], colNum[i1], ix);
   		      }
      	      cellSize /= 4;
      	      ix = cellSize;
//  	   	printf("------ny= %d, ix= %d\n",ny, ix);
      	      cellSizeTabX[ny++] = ix;


        	  ix += 2;
        	  cellSize = 0;
      	      for(i1=startCellsY;i1<startCellsY+4;i1++) {
      	    	cellSize += getStartWhite(codeLine[i1], colNum[i1], ix);
   		      }
      	      cellSize /= 4;
      	      ix = cellSize;
      	      cellSizeTabX[ny++] = ix;
           }

//   	printf("------startCellsX1= %d \n",startCellsX1);
//           for(nn = 0;nn<MAX_CODEDM_LEN;nn++){
//    	   	printf("------cellSizeTabX[%d]= %d\n",nn, cellSizeTabX[nn]);
//           }
//    	   fflush(stdout);

// Y borders
//           for(nn = 0;nn<kY;nn++){
//           	printf("------colNum[%d]= %d \n",nn,colNum[nn]);
//           }

    	   startCellsX2 = cellSizeTabX[MAX_CODEDM_LEN-1];
//   	printf("------startCellsX2= %d, startCellsY1= %d \n",startCellsX2,startCellsY1);

   	       nx = startCellsX2+2;
           iy = startCellsY1;
           ny = 0;
           for(nn = 0;nn<MAX_CODEDM_LEN/2;nn++){
        	  iy += 2;
      	      for(i1=iy;i1<kY;i1++) {
      	    	 cellSize = 0;
      	    	    ccW = 0;
      	    	    cc = 0;
      	            for(i2=nx;i2<colNum[i1];i2++) {
      	            	if(i2 > nx+5 && codeLine[i1][i2] == 0) ccW++;
      	            	else ccW = 0;
      	    	        cellSize += codeLine[i1][i2];
      	    	        cc++;
      	    	        if(ccW > 3) break;

   		            }
      	            if(cellSize < (cc-ccW)/2){
      	        	   cellSizeTabY[ny++] = i1;break;
      	            }
      	      }
      	      iy = cellSizeTabY[ny-1];
//  	   	printf("------ny= %d, ix= %d\n",ny, ix);
        	  iy += 2;
      	      for(i1=iy;i1<kY;i1++) {
      	    	  cellSize = 0;
   	    	      ccW = 0;
   	    	      cc = 0;
   	              for(i2=nx;i2<colNum[i1];i2++) {
    	            	if(i2 > nx+5 && codeLine[i1][i2] == 0) ccW++;
    	            	else ccW = 0;
      	    	        cellSize += codeLine[i1][i2];
      	    	        cc++;
      	    	        if(ccW > 3) break;
   		            }
       	            if(cellSize > (cc-ccW)/2){
      	        	   cellSizeTabY[ny++] = i1;break;
      	            }

      	      }
      	      iy = cellSizeTabY[ny-1];
           }

//   	printf("------startCellsY1= %d \n",startCellsY1);
//           for(nn = 0;nn<MAX_CODEDM_LEN;nn++){
//    	   	printf("------cellSizeTabY[%d]= %d\n",nn, cellSizeTabY[nn]);
//           }
//       	printf("------startCellsY2= %d \n",startCellsY2);
//    	   fflush(stdout);

      	   startCellsY2 = cellSizeTabY[MAX_CODEDM_LEN-1];

      	 iy1 =   startCellsY1;
      	 for(iy = 0;iy<MAX_CODEDM_LEN;iy++){
      		iy2 = cellSizeTabY[iy];
         	ix1 =   startCellsX1;
        	for(ix = 0;ix<MAX_CODEDM_LEN;ix++){
        		ix2 = cellSizeTabX[ix];

//if(iy < 2 && ix < 2){
//	printf("iy=%d, ix=%d\n",iy, ix);
//}
          	    ccB = 0; ccW = 0;
 	            for(ny=iy1+2;ny<iy2-2;ny++) {
//if(iy < 2 && ix < 2){
//	printf(", ny=%d",ny);
//}
 	   	            for(nx=ix1+2;nx<ix2-2;nx++) {
   	                    if(codeLine[ny][nx] == 1) ccB++;
 	   	                else ccW++;
//if(iy < 2 && ix < 2){
//   	printf(",nx=%d,ccB=%d,ccW=%d",nx,ccB,ccW);
//}
 	   	              }
//if(iy < 2 && ix < 2){
 //printf("\n");
//}

 	              }
//if(iy < 2 && ix < 2){
   //printf("\n ccB=%d,ccW=%d\n",ccB,ccW);
   //printf("-------------------\n");
 //}

               if(ccB > ccW)
	              codeDM[ix][iy] = 1;
	           else
	              codeDM[ix][iy] = 0;
               ix1 = ix2;
         	}
        	iy1 = iy2;
      	 }

/*
      	 for(iy = 0;iy<MAX_CODEDM_LEN;iy++){
         	for(ix = 0;ix<MAX_CODEDM_LEN;ix++){
		       sprintf(bufLog,"%d",codeDM[ix][iy]);
		        saveLog( "", bufLog );
         	}
	 		sprintf(bufLog,"  %d\n",iy);
	 		saveLog( "", bufLog );
      	 }
*/

      	 decodeDM16(&codeDM[0][0], MAX_CODEDM_LEN);


// 		sprintf(bufLog,"\n");
// 		saveLog( "", bufLog );
//	       sprintf(bufLog,"%s",outputDMCode);
//	       saveLog( "", bufLog );
//  		sprintf(bufLog,"\n");
//  		saveLog( "", bufLog );

//      printf("\n------------\n");
//  	  printf("%s",outputDMCode);
//      printf("\n------------\n");

//            printf("%c",outputDMCode[i1]);
//         printf("\n------------\n");


        }
// 		BYTE codeDM[MAX_CODEDM_LEN][MAX_CODEDM_LEN];

//	printf("------startCellsY1= %d, startCellsX1= %d, startCellsY2= %d,startCellsX2= %d\n",startCellsY1, startCellsX1, startCellsY2, startCellsX2);
//	fflush(stdout);

        nn = startCellsY2; // to avoid compiler warning
	}
//	printCode(codeLine, colNum);
/*
 	 {int i1, i2;
	 		for(i1=0;i1<kY;i1++) {
		 		for(i2=0;i2<colNum[i1];i2++) {
		  	       sprintf(bufLog,"%d",codeLine[i1][i2]);
			       saveLog( "", bufLog );
		 		}
		 		sprintf(bufLog,"\n");
		 		saveLog( "", bufLog );

	 		}
 	 }
*/

}

int  getTypeFromBarcode_PA(char *code, int len){
	int type = 0, i, k;
	if(len < 7) return type;
	k = 0;
	for(i=0;i<7;i++){
		if(code[i] == '0' && code[i+1] == ';') {k = i+1;break;}
		if(code[i] == ';' && code[i+1] == 'S') {k = i;break;}
		if(code[i] == 'S' && code[i+1] == '5') {k = i-1;break;}
	}
//	printf("  k= %d, code[k] = %c \n", k, code[k]);
	if(k == 0)  return type;
	k += 12;
	type = (code[k]-'0')*10 + (code[k+1]-'0');
	return type;
}

int smearingForTypePA(LPIMGDATA imgInp, LPIMGDATA imgOut, char *mainPath, char *imgName, int pixX, int pixY ){
//	int DENSITY_L;

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
	//    int           sizeOut;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	LPBYTE ptrImgOut, ptrBuffOut;
	int densityTab[imgInp->bi.biHeight];
//	int densityTab2[imgInp->bi.biHeight];
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

//	imgOut_Sm.ptr = ptrImgOut;
//	imgOut_Sm.pixPtr = ptrBuffOut;
	copyHeadersBMP(ptrImgOut, &imgOut_Sm);


// LOGO/BARCODE
	stopSm = sizeH/2;
//	DENSITY_L = stopSm*ddSmearingy;
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

// LOGO second smearing
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
	 createDesityTabPA1(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);
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
// second test
	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x2/2-10, x2, y1, y2, Hist);
	yShift = getYShiftPA(Hist);
    y1 += yShift;

// bottom
    y2 = y1+300;
    if(y1 >= imgInp->bi.biHeight) y1 = imgInp->bi.biHeight-10;
    if(y2 >= imgInp->bi.biHeight) y2 = imgInp->bi.biHeight-1;
	histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x2/2-10, x2, y1, y2, Hist);
	yShift = getY2PA(Hist);
    y2 = y1+yShift+10;


//printf("------y1= %d, yShift= %d, y2= %d\n",y1, yShift, y2);
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

//  		 for(nn=1;nn<Hist[0];nn++){
// 			    printf("   Hist[ %d ]= %d \n", nn, Hist[nn]);
//		  }


//		createImageLogo(imgInp, x1, x2, y1, y2);
	 }
//	 createDesityTabPA2(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);
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
    y1 += yShift;
//    printf("2   y1= %d \n", y1);

//    y1 -= 20;
// bottom
//   if(y1 >= imgInp->bi.biHeight) y1 = imgInp->bi.biHeight-10;
//   if(y2 >= imgInp->bi.biHeight) y2 = imgInp->bi.biHeight-1;
   histHTest_X1_X1(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1+100, x2, y1, y2, Hist);
   yShift = getY2PA(Hist);
   y2 = y1+yShift+10;



// first barcode
   histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x1+300, y1+40, y2-40, Hist);
	xShift = getX1PA(Hist);
	x1 += xShift;
	x2 = x1+300;

    histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, x2-x1, x1, x2, y1+40, y2-40, Hist);
    xShift = getX2PA_b(Hist);
	x2 = x1+xShift+10;

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

    {
//    YlogoClassificationResult resultLogo;
//	createImageLogo(imgInp, x1, x2, y1, y2, &resultLogo);
	barcode_DataMatrix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight,  x1, x2, y1, y2);

	typeInt =  getTypeFromBarcode_PA(outputDMCode, numDMcodeChars);
	printf("  typeInt= %d \n", typeInt);

    }


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

void repairField(LPIMGDATA img, LPTICKETDATA ticketData){

//int DENSITY_L;

int ddSmearingx = 24;
int ddSmearingy = 24;

int sizeW, sizeH, lastW, lastH;
int idxW, idxH;
int gridW, gridH;
int m, stopSm;
int biWidthPad;
int width, height, buffSize, bitCount, pixX, pixY;
LPBYTE fPtr;
LPBITMAPFILEHEADER  bfOut;
LPBITMAPINFOHEADER  biOut;
//LPBITMAPFILEHEADER bf, bfOut;
//LPBITMAPINFOHEADER bi, biOut;

LPRGBQUAD          qd;
//    int           sizeOut;
int offBits = sizeof (BITMAPFILEHEADER) +
		sizeof (BITMAPINFOHEADER) +
		sizeof (RGBQUAD) * 2;
LPBYTE ptrImgOut, ptrBuffOut;
//int typeInt = -1;
IMGDATA imgOut_Sm;
fPtr = ticketData->fieldBMPPtr;

//    bf = (LPBITMAPFILEHEADER) fPtr;
//    bi = (LPBITMAPINFOHEADER)(fPtr + sizeof (BITMAPFILEHEADER));
//    len = bf->bfSize;
//    len = getLeftBytes((int*)bf->bfSize, 4);
    buffSize = *(LPDWORD)(fPtr + BFSIZEOFF);
    {
  	LPBYTE biPtr = fPtr + BFOOFBITSOFF+4;
//	printf("bi.biSize= %d, bi.biWidth= %d, bi.biHeight= %d \n",*(LPDWORD)(biPtr + BISIZEOFF), *(LPDWORD)(biPtr + BIWIDTHOFF),*(LPDWORD)(biPtr + BIHEIGHTOFF));
//	printf("bi.biPlanes= %d, bi.biBitCount= %d, bi.biCompression= %d  \n",*(LPWORD)(biPtr + BIPLANESOFF), *(LPWORD)(biPtr + BIBITCOUNTOFF), *(LPDWORD)(biPtr + BICOMPRESSIONOFF));
//	printf("bi.biSizeImage= %d, bi.biXPelsPerMeter= %d, bi.biYPelsPerMeter= %d  \n",*(LPDWORD)(biPtr + BISIZEIMAGEOFF), *(LPDWORD)(biPtr + BIXPELSPERMETEROFF), *(LPDWORD)(biPtr + BIYPELSPERMETEROFF));

	width = *(LPDWORD)(biPtr + BIWIDTHOFF);
	height = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	bitCount = *(LPWORD)(biPtr + BIBITCOUNTOFF);
//  biWidthPad = ((imgInp->bi.biWidth*imgInp->bi.biBitCount+31)/32)*4;
    biWidthPad = ((width*bitCount+31)/32)*4;
    pixX = *(LPDWORD)(biPtr + BIXPELSPERMETEROFF);
    pixY = *(LPDWORD)(biPtr + BIYPELSPERMETEROFF);

    sizeW = width/ddSmearingx;
    lastW = width - sizeW*ddSmearingx;
    if(lastW > 0)
	   sizeW++;
	else
	   lastW = ddSmearingx;

    sizeH = height/ddSmearingy;
    lastH = height - sizeH*ddSmearingy;
    if(lastH > 0)
	    sizeH++;
	else
	   lastH = ddSmearingy;


   ptrImgOut = yl_calloc("PP4", buffSize, sizeof(BYTE));
   if(ptrImgOut == NULL) {
	  printf("yl_calloc failed, ptrImgOut");
	  return;
   }

   bfOut = (LPBITMAPFILEHEADER) ptrImgOut;
   bfOut->bfType[0] = 0x42;
   bfOut->bfType[1] = 0x4d;
   set4Bytes(bfOut->bfSize, buffSize);
   bfOut->bfReserved1[0] = 0;
   bfOut->bfReserved1[1] = 0;
   set4Bytes(bfOut->bfOffBits, offBits);

   biOut = (LPBITMAPINFOHEADER)((LPBYTE)bfOut + sizeof (BITMAPFILEHEADER));
   set4Bytes(biOut->biSize, sizeof (BITMAPINFOHEADER));
   set4Bytes(biOut->biWidth, width);
   set4Bytes(biOut->biHeight, height);
   biOut->biPlanes[0] = 1;
   biOut->biPlanes[1] = 0;
   biOut->biBitCount[0]= 1;
   biOut->biBitCount[1]= 0;
   set4Bytes(biOut->biCompression, BI_RGB);
   set4Bytes(biOut->biSizeImage, 0);
   set4Bytes(biOut->biXPelsPerMeter, pixX);
   set4Bytes(biOut->biYPelsPerMeter, pixY);
   set4Bytes(biOut->biClrUsed, 0);
   set4Bytes(biOut->biClrImportant, 0);

   qd = (LPRGBQUAD)((LPBYTE)biOut + sizeof (BITMAPINFOHEADER));
   *(LPDWORD)qd       = 0x00000000;
   *(LPDWORD)(qd + 1) = 0x00ffffff;
   ptrBuffOut = ptrImgOut+ offBits;

   copyHeadersBMP(ptrImgOut, &imgOut_Sm);


// LOGO/BARCODE
stopSm = sizeH/2;
//DENSITY_L = stopSm*ddSmearingy;

for(idxH=0;idxH<sizeH;idxH++){
	gridH = ddSmearingy;
	if(idxH == sizeH-1) gridH = lastH;
	for(idxW=0;idxW<sizeW;idxW++){
		gridW = ddSmearingx;
		if(idxW == sizeW-1) gridW = lastW;
		if(idxH > stopSm)
		   m = getValueForSmearing(fPtr+offBits, idxW, idxH, biWidthPad, height, ddSmearingx, ddSmearingy, gridW, gridH);
		else
		   m = getValueForSmearing_White(fPtr+offBits, idxW, idxH, biWidthPad, height, ddSmearingx, ddSmearingy, gridW, gridH);

		setPixelsOut(ptrBuffOut,  biWidthPad, height, idxW*ddSmearingx, idxW*ddSmearingx + gridW,
				idxH*ddSmearingy, idxH*ddSmearingy+gridH-1, m);
	}
  }
//    saveBMPbuffer (ptrImgOut, img->szIMGName, img->szOutPath);

  }
}

int setCode25ToTab(int *idx, int *barTab, int av, int idxCodeB, int *codeB, int maxBars){
	int i, stop = *idx+5;
	   for(i=*idx;i<stop;i++){
		   if(barTab[i] < av)
			   codeB[idxCodeB] = 0;
		   else
			   codeB[idxCodeB] = 1;
		   idxCodeB++;
		   if(idxCodeB>=maxBars) break;
	   }
   *idx = stop;
	return idxCodeB;
}
int setCode25ToTabInv(int *idx, int *barTab, int av, int idxCodeB, int *codeB, int maxBars){
	int i, stop = *idx-5;
	int k = idxCodeB+4;
	   for(i=*idx;i>stop;i--){
		   if(barTab[i] < av)
			   codeB[k] = 0;
		   else
			   codeB[k] = 1;
		   idxCodeB++;
		   k--;
		   if(idxCodeB>=maxBars) break;
	   }
   *idx = stop;
	return idxCodeB;
}


#define CODEI25_LEN_verY     10
int  codeBarI25_verY[CODEI25_LEN_verY][5];

int  codeBarI25Vector_verY[CODEI25_LEN_verY*5] = {
       0,0,1,1,0, // 0
       1,0,0,0,1, // 1
       0,1,0,0,1, // 2
       1,1,0,0,0, // 3
       0,0,1,0,1, // 4
       1,0,1,0,0, // 5
       0,1,1,0,0, // 6
       0,0,0,1,1, // 7
       1,0,0,1,0, // 8
       0,1,0,1,0, // 9
       };
char tabTransI25_verY[CODEI25_LEN_verY] = {
       '0','1','2','3','4','5','6','7','8','9',
    };

void setCodeBarI25_verY(void){
	int i, j;

	for(i=0;i<CODEI25_LEN_verY;i++)
		for(j=0;j<5;j++)
			codeBarI25_verY[i][j] = codeBarI25Vector_verY[i*5+j];

	}
char deCodeOneBarI25_verY(int  *codeB, int *idx)
{
  int i, k, ifOk, stop = *idx+5;
  char ch = '?';

    for(i=0;i<CODEI25_LEN_verY;i++) {
          ifOk = 1;
//  printf("--------------------i= %d\n", i);
          for(k=*idx;k<stop;k++) {
//  printf("------codeB[ %d ]= %d, codeBarI25_verY[ %d ][ %d ]= %d\n", k, codeB[k], i, k, codeBarI25_verY[i][k]);
             if(codeB[k] != codeBarI25_verY[i][k-*idx]) {
                           ifOk = 0;
                           break;
             }

         }
//   printf("------ifOk= %d\n", ifOk);
          if(ifOk){
        	  ch = tabTransI25_verY[i];
// printf("------i= %d, ch= %c\n", i, ch);
               break;
          }
    }
    *idx = stop;
    return(ch);

}

int barDeCodeI25_verY (int *hist, char *strResult){
#define maxBars 200
  int ret = -1;
  int bB[maxBars];
  int wB[maxBars];
  int i, len, iStart, iStop;
  int th = 10;
  int bL, wL, bIdx, wIdx, bAv, wAv;
  int bI, wI;
  int codeB[maxBars], idxCodeB;
  int idxCode;

  setCodeBarI25_verY();

   strResult[0] = 0;
   len = hist[0]; iStart=1;iStop = len;
   for(i=1;i<=len;i++){
	   iStart = i;
	   if(hist[i] > th) break;
   }
   for(i=len;i>0;i--){
	   iStop = i;
	   if(hist[i] > th) break;
   }
   bL = 0; wL = 0; bIdx = 0; wIdx = 0;
   for(i=iStart;i<=iStop;i++){
	   if(hist[i] > th) {
		   if(i > iStart+4 && wIdx < maxBars &&  bL == 0) {
			   wB[wIdx] = wL;
			   wIdx++;
		   }
		   bL++;
		   wL = 0;
	   }else{
		   if(bIdx < maxBars && wL == 0) {
		      bB[bIdx] = bL;
		      bIdx++;
		   }
		   bL = 0;
		   wL++;
	   }
   }
   bAv = 0;wAv = 0;
   for(i=0;i<bIdx;i++){
//  	   printf("------bB[ %d ]= %d\n", i, bB[i]);
  	 bAv += bB[i];
   }
   if(bIdx > 0)
	   bAv /= bIdx;
   for(i=0;i<wIdx;i++){
		 wAv += wB[i];
//  	   printf("------wB[ %d ]= %d\n", i, wB[i]);
   }
   if(wIdx > 0)
	   wAv /= wIdx;

//   printf("------bAv= %d, wAv= %d\n", bAv, wAv);
   bAv += 2;
   wAv += 2;
// code
   idxCodeB = 0;
   bI=0; wI=0;
// start
   if(bB[0]<bAv && bB[1]<bAv && wB[0]<wAv && wB[1]<wAv){
	   bI=2; wI=2;
   }
//  printf("------bIdx= %d, wIdx= %d\n", bIdx, wIdx);
   while(bI+4<bIdx && wI+4<wIdx && idxCodeB<maxBars) {
//   while(bI+5<bIdx && wI+5<wIdx && idxCodeB<maxBars) {
	   idxCodeB = setCode25ToTab(&bI, bB, bAv, idxCodeB, codeB, maxBars);
	   idxCodeB = setCode25ToTab(&wI, wB, wAv, idxCodeB, codeB, maxBars);

//	   printf("------idxCodeB= %d, bI= %d, wI= %d\n", idxCodeB, bI, wI);
   }
//   for(i=0;i<idxCodeB;i++) {
//      printf("------codeB[ %d ]= %d\n", i, codeB[i]);
//   }

   i = 0;
   idxCode = 0;
   while(i+4<idxCodeB) {
	   strResult[idxCode] = deCodeOneBarI25_verY(codeB, &i);

// printf("------strResult[ %d ]= %d, i+5= %d\n", idxCode, strResult[idxCode], i+5);
	   idxCode++;
   }
   strResult[idxCode] = 0;
   {int cc = 0;
      len = strlen(strResult);
      for(i=0;i<len;i++){
	     if(strResult[i] == '?') cc++;
      }
      if(len > 10 && cc < 4) ret = 0;
   }

//   printf("------strResult_verY= %s\n", strResult);

 return ret;
}
void inverseCode( char *str){
	int i, k, len = strlen(str);
	char strTmp[len+1];
	strcpy(strTmp, str);
	k = 0;
	for(i=len-1;i>=0;i--){
		str[k] = strTmp[i];k++;
	}
}
int barDeCodeI25_verYInv (int *hist, char *strResult){
#define maxBars 200
  int ret = -1;
  int bB[maxBars];
  int wB[maxBars];
  int i, len, iStart, iStop;
  int th = 10;
  int bL, wL, bIdx, wIdx, bAv, wAv;
  int bI, wI;
  int codeB[maxBars], idxCodeB;
  int idxCode;

  setCodeBarI25_verY();

   strResult[0] = 0;
   len = hist[0]; iStart=1;iStop = len;
   for(i=1;i<=len;i++){
	   iStart = i;
	   if(hist[i] > th) break;
   }
   for(i=len;i>0;i--){
	   iStop = i;
	   if(hist[i] > th) break;
   }
   bL = 0; wL = 0; bIdx = 0; wIdx = 0;
   for(i=iStart;i<=iStop;i++){
	   if(hist[i] > th) {
		   if(i > iStart+4 && wIdx < maxBars &&  bL == 0) {
			   wB[wIdx] = wL;
			   wIdx++;
		   }
		   bL++;
		   wL = 0;
	   }else{
		   if(bIdx < maxBars && wL == 0) {
		      bB[bIdx] = bL;
		      bIdx++;
		   }
		   bL = 0;
		   wL++;
	   }
   }
   if(wL > 0 && bL == 0){
	   wB[wIdx] = wL;
	   wIdx++;
   }
   if(wL == 0 && bL > 0){
	      bB[bIdx] = bL;
	      bIdx++;
   }
   bAv = 0;wAv = 0;
   for(i=0;i<bIdx;i++){
//  	   printf("------bB[ %d ]= %d\n", i, bB[i]);
  	 bAv += bB[i];
   }
   if(bIdx > 0)
	   bAv /= bIdx;
   for(i=0;i<wIdx;i++){
		 wAv += wB[i];
//  	   printf("------wB[ %d ]= %d\n", i, wB[i]);
   }
   if(wIdx > 0)
	   wAv /= wIdx;

//   printf("------bAv= %d, wAv= %d\n", bAv, wAv);
   bAv += 2;
   wAv += 2;
// code
   idxCodeB = 0;
   bI=bIdx-1; wI=wIdx-1;

// stop
   if(bB[bI]<bAv && bB[bI-1]>bAv && wB[wI]<wAv ){
	   bI -= 2; wI--;
   }
//  printf("------bIdx= %d, bI= %d, wIdx= %d, wI= %d\n", bIdx, bI, wIdx, wI);

   while(bI+4>=0 && wI+4>=0 && idxCodeB<maxBars) {
	   idxCodeB = setCode25ToTabInv(&wI, wB, wAv, idxCodeB, codeB, maxBars);
	   idxCodeB = setCode25ToTabInv(&bI, bB, bAv, idxCodeB, codeB, maxBars);


//	   printf("------idxCodeB= %d, bI= %d, wI= %d\n", idxCodeB, bI, wI);
   }
//   for(i=0;i<idxCodeB;i++) {
//      printf("------codeB[ %d ]= %d\n", i, codeB[i]);
//   }

   i = 0;
   idxCode = 0;
   while(i+4<idxCodeB) {
	   strResult[idxCode] = deCodeOneBarI25_verY(codeB, &i);

// printf("------strResult[ %d ]= %d, i+5= %d\n", idxCode, strResult[idxCode], i+5);
	   idxCode++;
   }
   strResult[idxCode] = 0;
   inverseCode(strResult);
   {int cc = 0;
      len = strlen(strResult);
      for(i=0;i<len;i++){
	     if(strResult[i] == '?') cc++;
      }
      if(len > 10 && cc < 4) ret = 0;
   }

//   printf("------strResult_verY= %s\n", strResult);


 return ret;
}

/*
int  codeBarI25Vector[CODEI25_LEN*5] = {
       0,0,1,1,0, // 0
       1,0,0,0,1, // 1
       0,1,0,0,1, // 2
       1,1,0,0,0, // 3
       0,0,1,0,1, // 4
       1,0,1,0,0, // 5
       0,1,1,0,0, // 6
       0,0,0,1,1, // 7
       1,0,0,1,0, // 8
       0,1,0,1,0, // 9
       };
*/

