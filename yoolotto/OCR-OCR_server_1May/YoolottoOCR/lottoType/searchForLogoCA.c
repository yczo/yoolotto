/*
 * searchForLogoCA.c
 *
 *  Created on: Aug 30, 2013
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
void setPixelsOut(LPBYTE ptr, int w, int h, int x1, int x2, int y1, int y2, int v);
//void saveBMPbuffer (LPBYTE buff, char* name, char* path);
//void createDesityTab(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int* densityTab, int len);
//void createDesityTab2(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len);
void getBarcodeYFromDensityTable(int *dTab, int *y1, int *y2, int length);
void getBarcodeXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
//void getLogoXFromDensityTable(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int y1, int y2, int *x1, int *x2);
int createImageLogoBarcodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2);
int getValueForSmearing_White(LPBYTE imgPtr, int idxW, int idxH, int biWidthPad, int biHeight, int gridx, int gridy, int gridW, int gridH);
int getBarcodeYFromDensityTable_many(int *dTab, int *y1, int *y2, int length, int size);
int testTypeFromBarCodeTX(LPIMGDATA img, int x1, int x2, int y1, int y2, int iter, int *confOut);
void setCAlotteryTypes(void);
void getCALottoType(char *strResult, int *typeInt, char* typeChar);

//void copyPixelsTest(LPBYTE ptrOut, LPBYTE ptrInp, int width, int height);
//int barDeCodeI25 (int   *hist, int lenH, LPMASK maskAlph, int lenStr, int nOfCharInBar, int invDec,
//                  LPSTR strResult, int *confResult, LPSTR cand2);
//void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
//void getLogoYFromDensityTable(int *dTab, int *y1, int *y2, int length);
//void getLogoYFromDensityTable_2(int *dTab, int *y1, int *y2, int length);
//void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2);
//void createImageLogo(LPIMGDATA img, int x1, int x2, int y1, int y2, YlogoClassificationResult *resultLogo);
//int getPixelSmearing(LPBYTE ptr, int xn, int yn, int wPad, int height);

/*
‘0’    ( code 48 )    - megaMillions
‘1’    ( code 49 )    - powerBall
‘2’    ( code 50 )    - superLotto

‘3’    ( code 51 )    - daily3
‘4’    ( code 52 )    - daily4
‘5’    ( code 53 )    - fantasy5
‘6’    ( code 54 )    - hotSpot

8   - superLotto
9   - daily3
10  - fantasy5
12  - powerBall
14  - daily4
15  - megaMillions
22  - hotSpot

11 - Daily Derby
*/
char bufLog[200];
int  typeOfLotteryID_CA[LOTTO_TYPES_NUM_CA] = {
		8,12,15,9,14,10,22,11
};
int  typeOfLotteryLogoID_CA[LOTTO_TYPES_NUM_CA] = {
		50,49,48,51,52,53,54,55
};


void setCAlotteryTypes(void){
    typeOfLotteryCAT[0] = "SUPER LOTTO CA"; //8
    typeOfLotteryCAT[1] = "POWER BALL CA"; //12
    typeOfLotteryCAT[2] = "MEGA MILLIONS CA"; //15

    typeOfLotteryCAT[3] = "DAILY 3 CA"; //9
    typeOfLotteryCAT[4] = "DAILY 4 CA"; //14
    typeOfLotteryCAT[5] = "FANTASY 5 CA"; //10
    typeOfLotteryCAT[6] = "HOT SPOT CA"; //22
    typeOfLotteryCAT[7] = "DAILY DERBY CA"; //11

}
int getTypeFromLogoCA( int candidate) {
	int k;
    for(k=0;k<LOTTO_TYPES_NUM_CA;k++){
	  if(typeOfLotteryLogoID_CA[k] == candidate) return k;
   }
    return -1;
}


void getCALottoType(char *strResult, int *typeInt, char* typeChar){
   int len, k, num, tOut= -1;
   int k1, k2;

	 *typeInt = tOut;

//   printf("--------------strResult: %s\n", strResult);
//   sprintf(bufLog,"--------------strResult: %s\n", strResult);
//  saveLog(bufLog, "");
      len = strlen(strResult);
      if(len < 10){
	      strcpy(typeChar,"???");
	      return;
      }
     if(len > 13){
      k1 = 12; k2 = 13;
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

	  for(k=0;k<LOTTO_TYPES_NUM_CA;k++){
		  if(typeOfLotteryID_CA[k] == num) {tOut =k;break;}
	  }
     }
// printf("--------------tOut= %d\n", tOut);
// sprintf(bufLog,"--------------tOut= %d\n", tOut);
// saveLog(bufLog, "");
	  if(tOut >= 0)
         strcpy(typeChar,typeOfLotteryCAT[tOut]);
	  else {
	      k1 = len-4; k2 = k1+1;

	      if(strResult[k1] < '0' || strResult[k1] > '9' ||
	   	     strResult[k2] < '0' || strResult[k2] > '9'){
		      strcpy(typeChar,"???");
		      return;
	      }

	      num = 0;
	      if(strResult[k1] != '0')
	 	     num =  (strResult[k1]-'0')*10;
	      num +=  strResult[k2]-'0';
		  for(k=0;k<LOTTO_TYPES_NUM_CA;k++){
			  if(typeOfLotteryID_CA[k] == num) {tOut =k;break;}
		  }
//  printf("2--------------tOut= %d\n", tOut);
//  sprintf(bufLog,"2--------------tOut= %d\n", tOut);
//  saveLog(bufLog, "");

		  if(tOut >= 0)
	         strcpy(typeChar,typeOfLotteryCAT[tOut]);
		  else
		     strcpy(typeChar,"???");
	  }

	  *typeInt = tOut;
}
void testDensityTabCA_Y1(int *dTab, int *y1, int yStart, int yStop, int length){
//	char bufLog[200];

     int i, ii, k = 0;
//     *y1 = yStart;;


     ii = 0;
     for(i=yStart;i<yStop;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
    	 if(dTab[k]<4){
    		 ii = i;
    		 break;
            }
    	 }
     }
// printf(" !!!!!!!!!!!!!!!--------ii= %d, yStart= %d, yStop = %d\n",ii, yStart, yStop);

// printf(" !!!!!!!!!!!!!!!-------- ii-yStart= %d, yStop - ii= %d\n",ii-yStart, yStop - ii);
     if(ii > yStart &&  ii - yStart < 100 &&  yStop - ii >  100 ) {
    	 yStart = ii;
         ii = 0;
         for(i=yStart;i<yStop;i++){
       	   k = dTab[0]-i;
       	   if(k>0) {
        	 if(dTab[k]> 5){
        		 ii = i;
        		 break;
                }
        	 }
         }
// printf(" !!!!!!!!!!!!!!!-------- ii= %d\n",ii);
// printf(" !!!!!!!!!!!!!!!-------- yStop= %d, yStop - ii= %d\n",yStop, yStop - ii);
// printf(" !!!!!!!!!!!!!!!-------- yStart= %d, ii-yStart= %d\n",yStart, ii-yStart);
         if( (yStop - ii >  100) || (ii-yStart > 100 && yStop - ii > 35) ) {
        	 *y1 = k;
         }
     }
}
int testDensityTabCA_Y2(int *dTab, int *y2, int yStart, int yStop, int length){

     int i, ii, k = 0;


     ii = 0;
     for(i=yStop;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
    	 if(dTab[k]<10){
    		 ii = i;
    		 break;
            }
    	 }
     }
//printf(" !!!!!!!!!!!!!!!--------ii= %d, yStop = %d\n",ii, yStop);
     if(ii>0 && ii-yStop>4){
    	 *y2 = k;return 1;
     }
     return 0;
// printf(" !!!!!!!!!!!!!!!--------ii= %d, yStart= %d, yStop = %d\n",ii, yStart, yStop);

// printf(" !!!!!!!!!!!!!!!-------- ii-yStart= %d, yStop - ii= %d\n",ii-yStart, yStop - ii);
}

void getLogoYFromDensityTableCA(int *dTab, int *y1, int *y2, int length, int idxStart){
//	char bufLog[200];

     int i, k, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int thW = 10;
     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
//printf(" !!!!!!!!!!!!!!!--------idxStart= %d\n",idxStart);
     for(i=idxStart;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
//printf(" !!!!!!!!!!!!!!!--------dTab[%d]= %d\n",k, dTab[k]);
    	 if(dTab[k]>6){
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
void getLogoYFromDensityTableCA_2(int *dTab, int *y1, int *y2, int length, int idxStart){
//	char bufLog[200];

     int i, k, startY, ccY, av;
     int candidatesSt[10], candidatesH[10], candidatesW[10], iCand;
     int thW = 10;
     *y1 = 0; *y2 = 0;
     startY=0;
     ccY = 0; iCand = 0;
     av = 0;
//printf(" !!!!!!!!!!!!!!!--------idxStart= %d\n",idxStart);
     for(i=idxStart;i<length;i++){
   	   k = dTab[0]-i;
   	   if(k>0) {
//printf(" !!!!!!!!!!!!!!!--------dTab[%d]= %d\n",k, dTab[k]);
    	 if(dTab[k]>6){
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

void createDesityTabCA(LPBYTE ptrBuffOut, int wPad, int height, int pixX, int *densityTab, int len){
	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, i, j;
	int xCcBlack, xCcWhite;
	int thX, wPad_2;
	int size;
	wPad_2= wPad/2;
	thX = pixX/8 + 1;

	if(len > height-1)	len = height-1;

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

int testLogoCandidates(LPBYTE ptrBuffOut, int wPad, int height, int y1, int y2, int x1, int x2){
//	char bufLog[200];

	long nY;
	long nX;
	LPBYTE ptrP;
	int yn, y, j;
	int xCcBlack = 0;
	int  wPad_2;
	int x8_1=x1/8 - 10;
	int x8_2=x2/8 + 10;

	wPad_2= wPad/2;

	xCcBlack = 0;
	for(y=y2;y<y1;y++){
		yn=y;
		for(j=0;j<wPad_2-2;j++){
			nY=yn*wPad;
			nX = x8_1 - j;
//printf("nX= %d, \n",nX);
			if(nX < 5) break;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) xCcBlack++;
		}

		for(j=0;j<wPad_2-2;j++){
			nY=yn*wPad;
			nX = x8_2 + j;
//printf("nX= %d, wPad-5= %d \n",nX, wPad-5);
			if(nX > wPad-5) break;
			ptrP = ptrBuffOut+nY+nX;
			if(*ptrP == 0) xCcBlack++;
//printf("nX= %d, yn= %d, *ptrP= %d, xCcBlack= %d, ",nX, yn, *ptrP,xCcBlack);

//sprintf(bufLog,"X= %d, y= %d, *ptrP= %d, xCcBlack= %d",nX*8, yn, *ptrP, xCcBlack);
//saveLog( "", bufLog );

		}
//printf("\n");
//sprintf(bufLog,"\n");
//saveLog( "", bufLog );
    }
    return xCcBlack;
}

int smearingForTypeCA(LPIMGDATA imgInp, LPIMGDATA imgOut, char *mainPath, char *imgName, int pixX, int pixY ){
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
	int confOut;
//	int skipLogo = 0, confOut;


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

	 createDesityTabCA(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX, densityTab, imgInp->bi.biHeight-1);


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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//		   if(typeInt>=0) {yl_free(ptrImgOut);return typeInt;}  // !!!!!!!!!!!!!!!!!!!

		   yl_free(ptrImgOut);
		   return typeInt;
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
// LOGO
	{
//		int densityTab[imgInp->bi.biHeight];
		int ii;
//		int yLT1[2], yLT2[2];
		int yL1, yL2, xL1, xL2;
		int yL2_1, yL2_2;
		int yLc1, yLc2, d, d1, d2;
		int mass1, mass2; // nCand;

//						for(ii=1;ii<=densityTab[0];ii++) {
//							sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//							saveLog( "", bufLog );
//						}



		 getLogoYFromDensityTable(densityTab, &yL1, &yL2, DENSITY_L);
		 yLc1 = imgInp->bi.biHeight-1-yL1;
		 yLc2 = imgInp->bi.biHeight-1-yL2;
		 d = yLc2 - yLc1;
// printf("1 !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);
        if(yLc1 < 200 && d < 30){
        	mass1 = 0; mass2 = 0;
			for(ii=yL2;ii<=yL1;ii++) {
				mass1 += densityTab[ii];
				mass2++;
			}
			if(mass2>0)
				mass1 /=mass2;
	        if(mass1 > 90){
	  		    getLogoYFromDensityTableCA(densityTab, &yL2_1, &yL2_2, DENSITY_L, imgInp->bi.biHeight-1-yL2+2);
	  			 yLc1 = imgInp->bi.biHeight-1-yL2_1;
	  			 yLc2 = imgInp->bi.biHeight-1-yL2_2;
	  			 d = yLc2 - yLc1;
//	  			 printf("2 !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);
	  		     if(imgInp->bi.biHeight-1-yL2_1 < imgInp->bi.biHeight/2 && d > 45){
	  			   yL1 = yL2_1;
	  			   yL2 = yL2_2;
	  			   d = 100;
	  		       }

	        }
        }
       if(yLc1 < 25 && d < 75){
//second candidate
		    getLogoXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX*2, yL1, yL2, &xL1, &xL2);
		    yLc1 = yL1 + 50;
		    if(yLc1 >= imgInp->bi.biHeight) yLc1 = imgInp->bi.biHeight-1;
		    yLc2 = yL2 - 100;
		    if(yLc2 <= 0) yLc2 = 1;
//  printf("1 !!!!!!!!!!!!!!!--------xL1*8= %d, xL2*8= %d, yLc1= %d, yLc2= %d\n",xL1*8, xL2*8, yLc1, yLc2);
//  printf("1 !!!!!!!!!!!!!!!--------imgInp->bi.biWidth= %d, imgInp->bi.biHeight= %d\n",imgInp->bi.biWidth, imgInp->bi.biHeight);

		    mass1 = testLogoCandidates(ptrBuffOut, biWidthPad, imgInp->bi.biHeight,  yLc1, yLc2, xL1*8, xL2*8);

//   printf("1 !!!!!!!!!!!!!!!--------mass1= %d\n",mass1);
            yLc2 = imgInp->bi.biHeight-1-yL2+4;
            if(yLc2 <= 0) yLc2 = 1;
            if(yLc2 >= imgInp->bi.biHeight) yLc2 = imgInp->bi.biHeight-1;

  		    getLogoYFromDensityTableCA(densityTab, &yL2_1, &yL2_2, DENSITY_L, imgInp->bi.biHeight-1-yL2+2);

//  			 yLc1 = imgInp->bi.biHeight-1-yL2_1;
//  			 yLc2 = imgInp->bi.biHeight-1-yL2_2;
//  			 d = yLc2 - yLc1;

//  printf("2 !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);

  getLogoXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX*2, yL2_1, yL2_2, &xL1, &xL2);
  yLc1 = yL2_1 + 50;
  if(yLc1 >= imgInp->bi.biHeight) yLc1 = imgInp->bi.biHeight-1;
  yLc2 = yL2_2 - 100;
  if(yLc2 <= 0) yLc2 = 1;
  mass2 = testLogoCandidates(ptrBuffOut, biWidthPad, imgInp->bi.biHeight,  yLc1, yLc2, xL1*8, xL2*8);
   if(mass1 > mass2+2 && imgInp->bi.biHeight-1-yL2_1 < imgInp->bi.biHeight/2){
	   yL1 = yL2_1;
	   yL2 = yL2_2;
       }

//	 yLc1 = imgInp->bi.biHeight-1-yL1;
//	 yLc2 = imgInp->bi.biHeight-1-yL2;
//	 d = yLc2 - yLc1;

//  printf("3 !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);
   } else{
      if(imgInp->bi.biHeight- yLc1 < 100 && d < 50){
// printf("skipLogo !!!!!!!!!!!!!!!--------imgInp->bi.biHeight- yLc1= %d, d= %d\n",imgInp->bi.biHeight- yLc1, d);
//    	  				for(ii=1;ii<=densityTab[0];ii++) {
//    	  					sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//    	  					saveLog( "", bufLog );
//    	  				}
    	    if(imgInp->bi.biHeight- yLc1 < 2 && d < 2){
      		    getLogoYFromDensityTableCA_2(densityTab, &yL1, &yL2, DENSITY_L, 1);
      		  	 yLc1 = imgInp->bi.biHeight-1-yL1;
      		  	 yLc2 = imgInp->bi.biHeight-1-yL2;
      		  	 d = yLc2 - yLc1;
      		  	 if(d<40 || yLc1 > imgInp->bi.biHeight/2)
      		  		skipLogo = 1;
//      		  	  printf("3 !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);
    	    }else
    	     skipLogo = 1;
       }
      if(d > 350){
//    	  printf(" !!!!!!!!!!!!!!!--------yL1= %d, yLc1= %d, yLc2= %d\n",yL1, yLc1, yLc2);
    	            testDensityTabCA_Y1(densityTab, &yL1, yLc1, yLc2, DENSITY_L);
//     	  printf(" !!!!!!!!!!!!!!!--------yL1= %d\n",yL1);
    	   		 yLc1 = imgInp->bi.biHeight-1-yL1;
//    	   		 yLc2 = imgInp->bi.biHeight-1-yL2;
    	   		 d = yLc2 - yLc1;

//  printf(" !!!!!!!!!!!!!!!--------yLc1= %d, yLc2= %d, d= %d\n",yLc1, yLc2, d);
//    	  				for(ii=1;ii<=densityTab[0];ii++) {
//    	  					sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
//    	  					saveLog( "", bufLog );
//    	  				}
//    	  skipLogo = 1;
       }

   }

  if(skipLogo == 0) {

  	 getLogoXFromDensityTable(ptrBuffOut, biWidthPad, imgInp->bi.biHeight, pixX*2, yL1, yL2, &xL1, &xL2);
  		 yLc1 = imgInp->bi.biHeight-1-yL1;
  		 yLc2 = imgInp->bi.biHeight-1-yL2;
 	  if(yLc2 - yLc1 < 75) {
 		 if(testDensityTabCA_Y2(densityTab, &yL2, yLc1, yLc2, DENSITY_L) == 0) {
 		 yL1 += 15;
 		 if(yL1 >= imgInp->bi.biHeight) yL1 = imgInp->bi.biHeight-1;
 		 yL2 -= 15;
 		if(yL2 <= 0) yL2 = 1;
// 								for(ii=1;ii<=densityTab[0];ii++) {
// 									sprintf(bufLog,"densityTab[ %d ]= %d\n",ii, densityTab[ii]);
// 									saveLog( "", bufLog );
// 								}
 		 }
 	  }
  	  d2 = (xL2 - xL1)*8;
// 	  printf(" !!!!!!!!!!!!!!!--------imgInp->bi.biWidth= %d, d2= %d\n",imgInp->bi.biWidth, d2);
// 	  printf(" !!!!!!!!!!!!!!!--------yLc2 - yLc1= %d\n",yLc2 - yLc1);
  	  if(yLc2 - yLc1 < 75 && imgInp->bi.biWidth > 2*d2) {
//  	  	  printf(" !!!!!!!!!!!!!!!--------left= %d, right= %d\n",xL1*8, imgInp->bi.biWidth - 1- xL2*8);
  	  	  d1 = xL1*8;
  		  d2 = imgInp->bi.biWidth - 1- xL2*8;
  		  d= d1-d2; if(d<0) d =-d;
//printf(" !!!!!!!!!!!!!!!--------d= %d\n",d);

  			d2 = xL2 - xL1;
  			d = imgInp->bi.biWidth/3 - d2;
  			if(d > 0){
//	printf("2 !!!!!!!!!!!!!!!--------d= %d\n",d);
  				xL1 -= d/16;
  				if(xL1 <= 0) xL1 = 1;
  				xL2 += d/16;
 				if(xL2 >= imgInp->bi.biWidth) xL2 = imgInp->bi.biWidth-1;
  			}
  	  }

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
// printf("3 !!!!!!!!!!!!!!!--------xL1*8= %d, xL2*8= %d \n",xL1*8, xL2*8);
// printf("3 !!!!!!!!!!!!!!!--------imgInp->bi.biHeight-1-yL1= %d, imgInp->bi.biHeight-1-yL2= %d \n",imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2);
// printf("           typeInt= %d, retLogoClass= %d \n",typeInt, retLogoClass);
// fflush(stdout);
		createImageLogo(imgInp, xL1*8, xL2*8, imgInp->bi.biHeight-1-yL1, imgInp->bi.biHeight-1-yL2, &resultLogo);

//		 printf("           after createImageLogo \n");
//		 fflush(stdout);
//		  printf("------------ result num= %d\n", resultLogo.resultsNum);
//		  sprintf(bufLog,"------------ result num= %d\n", resultLogo.resultsNum);
//		  saveLog( "", bufLog );

		  {int nnn;
			  for(nnn=0;nnn<resultLogo.resultsNum;nnn++){
				  printf("------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
				  sprintf(bufLog,"------------ result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
				  saveLog( "", bufLog );

			  }

//			  if(resultLogo.resultsNum > 1 && resultLogo.confidende[1] > 0) {
//				  sprintf(bufLog,"------------warning!!!!!!!!!!!!!!!!!!!! result.candidates[ %d ]= %d, resultLogo.confidence[ %d ]= %e\n", nnn, resultLogo.candidates[nnn],nnn,resultLogo.confidende[nnn]);
//			      saveLog( "", bufLog );
//			  }
		  }


       if(resultLogo.resultsNum > 0 && resultLogo.confidende[0] >= 0.0005){
             if((resultLogo.resultsNum == 1) || (resultLogo.resultsNum > 1 && resultLogo.confidende[1] < 0.00001)){

                   typeIntLogo = getTypeFromLogoCA(resultLogo.candidates[0]);
//                   if(typeIntLogo != typeInt) {
                	   printf("           typeInt= %d != typeIntLogo= %d \n",typeInt, typeIntLogo);
//                       sprintf(bufLog,"           typeInt= %d != typeIntLogo= %d \n",typeInt, typeIntLogo);
//                       saveLog( "", bufLog );

//                   }
           }
       }

      }
	}


	if(typeInt < 0 && typeIntLogo >= 0) {
        printf("------------ typeIntLogo= %d\n", typeIntLogo);
        sprintf(bufLog,"------------ typeIntLogo= %d\n", typeIntLogo);
        saveLog( "", bufLog );
		typeInt = typeIntLogo;
	}
	if(typeInt < 0 && typeIntOld >= 0) {
        printf("------------ typeIntOld= %d\n", typeIntOld);
        sprintf(bufLog,"------------ typeIntOld= %d\n", typeIntOld);
        saveLog( "", bufLog );
		typeInt = typeIntOld;
	}

//	    saveBMPbuffer (ptrImgOut, "smearingLogoBarcode.bmp", imgInp->szOutPath);
//	    saveBMPbuffer (ptrImgOut, imgInp->szIMGName, imgInp->szOutPath);


	yl_free(ptrImgOut);
	return typeInt;
*/
}

