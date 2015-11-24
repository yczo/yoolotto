/*
 * searchForLogoNY.c
 *
 *  Created on: Nov 5, 2013
 *      Author: Wicek
 */


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


int getValueForSmearing(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getValueForSmearing_White(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
void setPixelsOut(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2, int v);
//void createDesityTabCA(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len);
void getBarcodeXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
int createImageLogoBarcodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2);
int getValueForSmearing_White(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getBarcodeYFromDensityTable_many(int *dTab, int *y1, int *y2, int length, int size);
void getBarcodeYFromDensityTable(int *dTab, int *y1, int *y2, int length);
int testTypeFromBarCodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int *confOut);
//void getLogoYFromDensityTable(int *dTab, int *y1, int *y2, int length);
//void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2, YlogoClassificationResult *resultLogo);
//void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
//void getLogoXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);

//extern int vHistPreProc[H_SIZE_PREPROC];
int  typeOfLotteryID_NY[LOTTO_TYPES_NUM_NY] = {
		8,9,10,11,12,14,15,27
};
//int  typeOfLotteryLogoID_CA[LOTTO_TYPES_NUM_NY] = {
//		50,49,48,51,52,53,54
//};


void setNYlotteryTypes(void){
    typeOfLotteryNYT[0] = "LOTTO NY"; //8
    typeOfLotteryNYT[1] = "NUMBERS NY"; //9
    typeOfLotteryNYT[2] = "TAKE 5 NY"; //10;
    typeOfLotteryNYT[3] = "SWEET MILLION NY"; //11
    typeOfLotteryNYT[4] = "MEGA MILLIONS NY"; //12
    typeOfLotteryNYT[5] = "WIN 4 NY"; //14
    typeOfLotteryNYT[6] = "POWER BALL NY"; //15;
    typeOfLotteryNYT[7] = "PICK 10 NY"; //27

}

void getNYLottoType(char *strResult, int *typeInt, char* typeChar){
   int len, k, num, tOut= -1;
   int k1, k2;

	 *typeInt = tOut;

   printf("--------------strResult: %s\n", strResult);
   fflush(stdout);
//   sprintf(bufLog,"--------------strResult: %s\n", strResult);
//  saveLog(bufLog, "");
      len = strlen(strResult);
      if(len < 10){
	      strcpy(typeChar,"???");
	      return;
      }
     if(len > 10){
      k1 = len-6; k2 = len-5;
      if(strResult[k1] < '0' || strResult[k1] > '9' ||
   	     strResult[k2] < '0' || strResult[k2] > '9'){
	      strcpy(typeChar,"???");
	      return;
      }

      num = 0;
      if(strResult[k1] != '0')
 	     num =  (strResult[k1]-'0')*10;
      num +=  strResult[k2]-'0';

 printf("--------------num= %d\n", num);

// sprintf(bufLog,"--------------num= %d\n", num);
 //saveLog(bufLog, "");

	  for(k=0;k<LOTTO_TYPES_NUM_NY;k++){
		  if(typeOfLotteryID_NY[k] == num) {tOut =k;break;}
	  }
     }

// sprintf(bufLog,"--------------tOut= %d\n", tOut);
// saveLog(bufLog, "");
	  if(tOut >= 0){
         strcpy(typeChar,typeOfLotteryNYT[tOut]);
	  }

	  else {
		  strcpy(typeChar,"???");
	  }
// printf("--------------typeChar= %s\n", typeChar);
//	  fflush(stdout);
	  *typeInt = tOut;
}
void getLogoYFromDensityTableNY(int *dTab, int *y1, int *y2, int length){
//	char bufLog[200];

     int i, k, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int  thWTr = 60, thWB = 50, thH = 10,  thHB = 50;
     int ddTh = 40;

     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
     for(i=1;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
    	 if(dTab[k]>4){
            if(startY == 0){
        	   startY = k;
            }
            ccY++;
            av += dTab[k];
    	 }else{
             if(startY != 0 && ccY > 6 && iCand < 10){
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
     {int ddd;
	    for(i=0;i<iCand;i++){
	    	if(i>0)
	    	  ddd = candidatesSt[i] - candidatesSt[i-1]+ candidatesH[i-1];
	    	else
    		  ddd = candidatesSt[i] - dTab[0];
//	    	  sprintf(bufLog,"  candidatesSt= %d, candidatesH= %d, candidatesW= %d, ddd= %d\n",candidatesSt[i], candidatesH[i], candidatesW[i], ddd);
//	    	  saveLog( "", bufLog );

	    }
     }
     {
      int ddd, iStart;
      int thDD;
        iStart = -1;
	    for(i=0;i<iCand;i++){
	    	if(candidatesH[i] > thH) {
	    		if(candidatesH[i] < 50)
		            thDD = 80;
	    		else
	    			thDD = 150;
				if(candidatesH[i] < thHB && candidatesW[i] > thWTr) continue;
				ddd = 0;
				if(i+1 < iCand)
				   ddd =  candidatesSt[i]- candidatesH[i]- candidatesSt[i+1];
// 	  sprintf(bufLog,"1 -----------------  i= %d, candidatesH= %d, candidatesW= %d, ddd= %d, iStart= %d, thH= %d\n",i, candidatesH[i], candidatesW[i], ddd, iStart, thH);
//  	  saveLog( "", bufLog );

				if(iStart < 0 && ddd < thDD) {
				   if(candidatesH[i] < thHB && candidatesW[i] < thWB) {
					  iStart = i;
//	  sprintf(bufLog,"2 -----------------  i= %d, candidatesH= %d, candidatesW= %d, ddd= %d, iStart= %d, thH= %d\n",i, candidatesH[i], candidatesW[i], ddd, iStart, thH);
// 	  saveLog( "", bufLog );

					  if(ddd > ddTh){
			             *y1 = candidatesSt[i];
				         *y2 = candidatesSt[i] - candidatesH[i];
				        return;
					  }
				  }else{
					   if(candidatesH[i] > thHB+20 && candidatesW[i] < thWB) {
						  iStart = i;
//		  sprintf(bufLog,"3 -----------------  i= %d, candidatesH= %d, candidatesW= %d, ddd= %d, iStart= %d, thH= %d\n",i, candidatesH[i], candidatesW[i], ddd, iStart, thH);
//	 	  saveLog( "", bufLog );
						  if(ddd > ddTh){
				             *y1 = candidatesSt[i];
					         *y2 = candidatesSt[i] - candidatesH[i];
					        return;
						  }
					  }
				  }
				}else{
//			  sprintf(bufLog,"4 -----------------  i= %d, candidatesH= %d, candidatesW= %d, ddd= %d, iStart= %d, thH= %d\n",i, candidatesH[i], candidatesW[i], ddd, iStart, thH);
//		 	  saveLog( "", bufLog );

					  if(iStart >= 0 && ddd > ddTh && ddd < thDD){
			             *y1 = candidatesSt[iStart];
				         *y2 = candidatesSt[i] - candidatesH[i];
// sprintf(bufLog,"4 -----------------  i= %d, *y1= %d, *y2= %d \n",i, *y1, *y2);
// saveLog( "", bufLog );

				        return;
					  }
				}

	    	}
	    }

//  sprintf(bufLog,"5 -----------------  i= %d, candidatesH= %d, candidatesW= %d, ddd= %d, iStart= %d, thH= %d\n",i, candidatesH[i], candidatesW[i], ddd, iStart, thH);
//  saveLog( "", bufLog );

	    for(i=0;i<iCand;i++){
	    	if(candidatesH[i] > thH) {
				if(candidatesH[i] < thHB && candidatesW[i] > thWTr) continue;
		             *y1 = candidatesSt[i];
			         *y2 = candidatesSt[i] - candidatesH[i];
			        return;
	    	}
	    }

     }
}
void createDesityTabNY(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, i, j;
	int xCcBlack, xCcWhite;
	int thX, wPad_2;
	int size, len2;
	wPad_2= wPad/2;
	thX = pixX/8 + 1;

	if(len > height-1)	len = height-1;
	len2 = len/2;
	densityTab[0] = len;
//	size = 50;
//	if(size > wPad/2-2) size = wPad/2-2;
	size = wPad/2-2;
	for(i=0;i<len;i++){
//		yn = height-1 -i;
		yn = i;
		xCcBlack = 0;
		xCcWhite = 0;
//		for(j=0;j<wPad_2-2;j++){
		if(i>len2) thX = 32;
		for(j=0;j<size;j++){
//			yn=height-1-yn;
			nY=yn*wPad;
			nX = wPad_2 - j;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) {xCcBlack++;xCcWhite = 0;}
			else xCcWhite++;
			if(xCcWhite > thX) break;
		}
		xCcWhite = 0;
//		for(j=0;j<wPad_2-2;j++){
		for(j=0;j<size;j++){
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
void getLogoXFromInpImgNY(int * vHist, int *x1, int *x2){

	int i, len, len2;
	int xCcBlack, xCcWhite;
	int thX = 120;

	len = vHist[0];len2 = len/2;
	xCcWhite = 0;
	xCcBlack = len2;
	for(i=len2;i>0;i--){
		if(vHist[i] > 2) {xCcBlack = i;xCcWhite = 0;}
		else xCcWhite++;
		if(xCcWhite > thX) break;
	}
//	printf("x1************** i= %d, xCcBlack= %d\n", i, xCcBlack);
	xCcWhite = 0;
   	*x1 = xCcBlack;
	xCcBlack = len2;
	for(i=len2;i<len;i++){
		if(vHist[i] > 2) {xCcBlack = i;xCcWhite = 0;}
		else xCcWhite++;
		if(xCcWhite > thX) break;
	}
//	printf("x2************** i= %d, xCcBlack= %d\n", i, xCcBlack);
   	*x2 = xCcBlack;
}


//int getTypeFromLogoNY( int candidate) {
//	int k;
//    for(k=0;k<LOTTO_TYPES_NUM_NY;k++){
//	  if(typeOfLotteryLogoID_NY[k] == candidate) return k;
//   }
//    return -1;
//}

int smearingForTypeNY(LPIMGDATA imgInp, LPIMGDATA imgOut, char *mainPath, char *imgName, int pixX, int pixY ){
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
	int typeInt = -1, typeIntOld = -1;
//	int typeIntLogo = -1;
	IMGDATA imgOut_Sm;
//	int skipLogo = 0, confOut;
	int confOut;


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

	 createDesityTabNY(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);


// barcode test
	 {
		 int size = 5;
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
//	 printf("1-----------------typeInt= %d, confOut= %d\n",typeInt, confOut);
//	 sprintf(bufLog,"1-----------------typeInt= %d\n",typeInt);
//	 saveLog( "", bufLog );
	 if(typeInt > 0 && confOut < -1){
	   typeIntOld = typeInt;
	   typeInt = -1;
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

//  if(typeInt<0){
//	  sprintf(bufLog,"-----------------unknown, typeInt= %d\n",typeInt);
//	  saveLog( "", bufLog );
//  }
// printf("2-----------------typeInt= %d\n",typeInt);
// sprintf(bufLog,"2-----------------typeInt= %d\n",typeInt);
// saveLog( "", bufLog );



//		   if(typeInt>=0) return typeInt;  // !!!!!!!!!!!!!!!!!!!

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// test
           yl_free(ptrImgOut);  // !!!!!!!!!!!!!!!!!!!
		   return typeInt;
/*
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// LOGO
		{
			int yL1, yL2, xL1, xL2;
//			char bufLog[500];
			 getLogoYFromDensityTableNY(densityTab, &yL1, &yL2, DENSITY_L);
//			 getLogoXFromDensityTableNY(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX*2, yL1, yL2, &xL1, &xL2);


			  histVPtrPix(imgInp->pixPtr, biWidthPad, imgInp->bi.biHeight, H_SIZE_PREPROC, 10, imgInp->bi.biWidth-10, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, vHistPreProc);
			  getLogoXFromInpImgNY(vHistPreProc, &xL1, &xL2);
			  xL1 += 9;
			  xL2 += 9;
//			  sprintf(bufLog,"--             ---------   xL1= %d, xL2= %d\n",xL1, xL2);
//			  saveLog( "", bufLog );



//				sprintf(bufLog,"-----imgInp->bi.biHeight-1-yL1= %d, imgInp->bi.biHeight-1-yL2= %d\n",imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2);
//				saveLog( "", bufLog );
//			 yL1 = imgInp->bi.biHeight-1 - 2;
//			 yL2 = imgInp->bi.biHeight-1 - 287;
//			 		createImageLogo(imgInp, xL1*8, xL2*8, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, &resultLogo);
			 		createImageLogo(imgInp, xL1, xL2, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, &resultLogo);
//			 strcpy(imgOut_Sm.szIMGName, imgInp->szIMGName);
//			 strcpy(imgOut_Sm.szOutPath, imgInp->szOutPath);
//			 		createImageLogo(&imgOut_Sm, xL1*8, xL2*8, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, &resultLogo);

					  {int nnn;
						  for(nnn=0;nnn<resultLogo.resultsNum;nnn++){
							  printf("------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
//							  sprintf(bufLog,"------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
//							  saveLog( "", bufLog );
						  }

			//			  if(resultLogo.resultsNum > 1 && resultLogo.confidende[1] > 0) {
			//				  sprintf(bufLog,"------------warning!!!!!!!!!!!!!!!!!!!! result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
			//			      saveLog( "", bufLog );
			//			  }
					  }


		}

//	    saveBMPbuffer (ptrImgOut, "smearingLogoBarcode.bmp", imgInp->szOutPath);
//	    saveBMPbuffer (ptrImgOut, imgInp->szIMGName, imgInp->szOutPath);


	yl_free(ptrImgOut);
	return typeInt;
*/
}
