/*
 * UtilRecognition5.c
 *
 *  Created on: Sep 6, 2012
 *      Author: Wicek
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include  "../headers/UtilRecognition.h"
#include  "../headers/CommonRec.h"
#include "../headers/Functions.h"

int pictWidthHeightSizeDeltaFactor (WORD widthPar, WORD heightPar, WORD sizePar, WORD deltaPar, WORD factorPar);
int setMapVers ( WORD mapVersionPar);
int getMassFromTheSegment(int *hist, int hLen, int cL, int cR);

int charIdxTmp = 0;

int extractChar(IMAGEL *lpImage, long angleH, long shiftX, int leftInd, int rightInd, LPBYTE lpExtractCh, int *lpWidth, int  *lpHeight)
{
 float  cosAlph;
 POINT  pP1, pP2;
 int    dX = 0, lenT, yT, yB, xL, xR, kk;

 int   x_start, x_end, i, j, i_y, width_bt, wImg;
 int   min, max, length, width, height, left_b, right_b;
 int   top_offset, left_offset, i1, ww_bt;
 BYTE maskB;
 short mask, ch;
 LONG nY;
 BYTE *hptr;
       width  = *lpWidth;
      height = *lpHeight;

    wImg  = (((lpImage->iLineWidth)+31)/32)*4;

      if((angleH != 0L) || (shiftX != 0)) {
         cosAlph = (float)cos((float)angleH*PI/(180.0*(float)SCALE2));
         if(cosAlph < (float)0)
                   cosAlph = -cosAlph;
         if(cosAlph < (float)1E-10)
                   cosAlph = (float)1.0;

         leftInd  = (int)((float)leftInd/cosAlph  - (float)shiftX/(float)SCALE2 + (float)0.5);
         rightInd = (int)((float)rightInd/cosAlph  - (float)shiftX/(float)SCALE2 + (float)0.5);
         }

      yT = lpImage->iYBegin;
      yB = lpImage->iYBegin + lpImage->iHeight - 1;

      xL = lpImage->iXBegin + leftInd;
      xR = lpImage->iXBegin + rightInd;

// traces

      traceL[0] = yT;
      traceC[0] = yT;

      if((angleH != 0L) || (shiftX != 0)) {
           cosAlph = (float)angleH*(float)PI/((float)180.0*(float)SCALE2);
           if(cosAlph < 0) {cosAlph = -cosAlph;}
           if( cosAlph < (float)0.785) {
              dX =(int)((float)(yB -yT)*tan(cosAlph)+(float)0.5);
              }
           pP1.y = yT;
           pP1.x = xL + dX;
           min   = pP1.x;
           if(pP1.x > lpImage->iXBegin + lpImage->iWidth - 1) {
                pP1.x = lpImage->iXBegin + lpImage->iWidth - 1;
                }
           min   = pP1.x;
           pP2.y = yB;
           pP2.x = xL;
           if(xL < min)
                   min = xL;
           lengthTraceL = skewTraces((int *)&traceL[1],pP1,pP2);
           pP1.x = xR + dX;
           if(pP1.x > lpImage->iXBegin + lpImage->iWidth - 1) {
                pP1.x = lpImage->iXBegin + lpImage->iWidth - 1;
                }
           max = pP1.x;
           pP2.x = xR;
           if(xR > max)
                   max = xR;
           lengthTraceC = skewTraces((int *)&traceC[1],pP1,pP2);
           min++;
           lenT = min(lengthTraceL,lengthTraceC);
           }
         else {
           lenT = min(yB-yT+1,MAXLTRACE-1);
           lengthTraceL = lenT;
           lengthTraceC = lenT;

           for(i=1;i<=lenT;i++) {
                traceL[i] = xL;
                traceC[i] = xR;
                }
           min = xL;
           max = xR;
           }

// sent to buffer;

        kk=0;
        length = max-min+1;
        if( length > width) {
                     width_bt  =((width + 31)/32)*4;
                     *lpWidth  = width;
                     }
        else  {
                     width_bt  =((length + 31)/32)*4;
                     *lpWidth  = length;
                     }

        left_offset = 0;
        if(width < length){
           left_offset = (length-width)/2;
           if((length-width)%2)
                           max--;
           }
        left_b = min + left_offset;
        right_b = max - left_offset;

        top_offset = 0;
        if(height < lengthTraceL){
             top_offset=(lengthTraceL- height)/2;
             if((lengthTraceL- height)%2)
                                     kk=1;
             *lpHeight = height;
             }
         else
             *lpHeight = lengthTraceL;

         ww_bt  =((width + 31)/32)*4;
         width  = *lpWidth;
         height = *lpHeight;
        memset(lpExtractCh,0xFF,ww_bt*height);

        for(i=top_offset;i<lengthTraceL-top_offset-kk;i++) {
             i1 = i+1;
             i_y = i + traceL[0];
             if(traceL[i1] < left_b){
                  x_start = left_b;
                  }
             else{
                  x_start = traceL[i1];
                  }

             if(traceC[i1] > right_b){
                  x_end = right_b;
                  }
             else{
                  x_end = traceC[i1];
                  }

             i_y=(lpImage->iImgHeight)-1-i_y;
             nY=(LONG)i_y*(LONG)wImg;
             for(j=x_start;j<x_end+1;j++) {

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                   hptr=(lpImage->lpPointer)+nY+(LONG)(j/8);

                   mask=LEFT_BIT;
                   mask >>= (j%8);
                   ch=(~hptr[0])&mask;
                   if(ch != 0) {

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                   maskB=LEFT_BIT;
                   maskB >>= ((j-left_b)%8);
                   lpExtractCh[(height-(i-top_offset)-1)* width_bt+(j-left_b)/8] &= ~(maskB);
                   }
               } /* for(j) */
             }
    return(0);

} /* of extractChar() */
int  recChar(LPBYTE pBuffer, int width, int height, IMAGEL * pImage, RECCHARSTRUCT * pRecCharStruct, char * pChar, int * pConf){
 int size;
 int normVersion;
 int mapVersion;
 int confThreshold;
 int iAlphaIndx;

 int iDpiX;
 int iDpiY;

// int delta = 1;
// int factor = 10;

 LPBYTE pNormPicture;
 short result[12];
 short * pResult;

 unsigned short t;
 int err, retValue;
 int conf;

 LPBYTE pSubsetMap = lpAlphaMap;
 int flag;
 int nChar;
 int nCand;
 int i;
 int index;


 size          = pRecCharStruct -> size;
 normVersion   = pRecCharStruct -> normVersion;
 mapVersion    = pRecCharStruct -> mapVersion;
 confThreshold = pRecCharStruct -> confThreshold;
 iAlphaIndx    = pRecCharStruct -> iAlphaIndx;

 iDpiX = pImage -> iDpiX;
 iDpiY = pImage -> iDpiY;

 pNormPicture = &normPicture[0];
 pResult = &result[0];


// err = setRes((WORD)iDpiX, (WORD)iDpiY);
//              if(err < 0) {*pChar = 32; *pConf = 0; return(-301);}
 err = setMapVers((WORD)mapVersion);
//              if(err < 0) {*pChar = 32; *pConf = 0; return(-302);}
    mapVersionR = mapVersion;

 err = pictWidthHeightSizeDeltaFactor((WORD)width, (WORD)height,
	                                  (WORD)size, (WORD)1, (WORD)10);
               if(err < 0) {*pChar = 32; *pConf = 0; return(-303);}

                widthR  = width;
                heightR = height;
                sizeR   = size;
                deltaR  = 1;
                factorR = 10;

// err = setNormVers((WORD)normVersion);
//              if(err < 0) {*pChar = 32; *pConf = 0; return(-304);}
              normVersionR = normVersion;
 retValue =
       pictFromNormal(pBuffer, pNormPicture);
              if(retValue < 0) {*pChar = 32; *pConf = 0; return(-305);}


 if((iAlphaIndx < 32)||(iAlphaIndx > 54)) index = iAlphaIndx;
 else {index = iAlphaIndx - 32;
       t = 100;
//       err = -77;
       if(lpLibrary)
//          err = setT(lpLibrary, t);
//       if(err < 0) return(-300);
    	   thR = t;
      }
  err = -77;
  if(lpLibrary)
   err = recCharacter(pNormPicture, lpLibrary, libSize, pSubsetMap, pResult);
              if(err < 0) {*pChar = 32; *pConf = 0; return(-306);}

 flag = pResult[0];
 nChar = pResult[1];
 nCand = (nChar < 5 ? nChar : 5);

 if(flag==0)
    {for(i=0; i<nCand; i++)
         {conf = (int)((2*256L - pResult[2+2*i])*100L/(2*256L));
          if(conf < 0) conf = 0;
          pResult[2+2*i] = conf;
         }
    }
 else if(flag==1)
    {for(i=0; i<nCand; i++)
         {conf = (int)((1000L - pResult[2+2*i])*100L/1000L);
          if(conf < 0) conf = 0;
          pResult[2+2*i] = conf;
         }
    }
 else return(-1);

 *pChar = (char)pResult[3];
 *pConf = pResult[2];

 return(retValue);
}
int  filtrMark(STRUCTWB  *lpWorkBuffer){
 int j, i, conT, h, lm, rm, irm, irm2;

   h=(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> hightImg)-1;
   for(j=0;j<((lpWorkBuffer->markNumber)-1);j++) {
        if( (lpWorkBuffer->mark[j]).g_m<0 ) (lpWorkBuffer->mark[j]).g_m=0;
        if( (lpWorkBuffer->mark[j]).d_m>h ) (lpWorkBuffer->mark[j]).d_m=h;
        if( ((lpWorkBuffer->mark[j]).d_m==2*MAXSIZE)||((lpWorkBuffer->mark[j]).g_m==2*MAXSIZE)) {
            i=j+1;
            conT=1;
            while( (i<(lpWorkBuffer->markNumber))&&conT) {
                if( ((lpWorkBuffer->mark[i]).d_m!=2*MAXSIZE)&&((lpWorkBuffer->mark[i]).g_m!=2*MAXSIZE)) {
                        conT=0;
                        continue;
                        }
                i++;
                }
            if(conT==0) {
              (lpWorkBuffer->mark[j]).d_m=(lpWorkBuffer->mark[i]).d_m;
              (lpWorkBuffer->mark[j]).g_m=(lpWorkBuffer->mark[i]).g_m;
              }
            else {
              (lpWorkBuffer->mark[j]).d_m=h;
              (lpWorkBuffer->mark[j]).g_m=(lpWorkBuffer -> begRect.y);
                }
           }
        }
   j=(lpWorkBuffer->markNumber)-1;
   if( (lpWorkBuffer->mark[j]).g_m<0 ) (lpWorkBuffer->mark[j]).g_m=0;
   if( (lpWorkBuffer->mark[j]).d_m>h ) (lpWorkBuffer->mark[j]).d_m=h;
   if( (((lpWorkBuffer->mark[j]).d_m==2*MAXSIZE)||(lpWorkBuffer->mark[j]).g_m==2*MAXSIZE)&&(j>0)) {
        (lpWorkBuffer->mark[j]).d_m=(lpWorkBuffer->mark[j-1]).d_m;
        (lpWorkBuffer->mark[j]).g_m=(lpWorkBuffer->mark[j-1]).g_m;
        }
   rm=(lpWorkBuffer->mark[(lpWorkBuffer->markNumber)-1]).r_m;
   if( rm<0 )  (lpWorkBuffer->mark[(lpWorkBuffer->markNumber)-1]).r_m=-rm;

   if(((lpWorkBuffer->lettWidth)>0)&&((lpWorkBuffer->Method)==2)) {
        j=0;
        while(j<(lpWorkBuffer->markNumber)-1) {
             if( (lpWorkBuffer->mark[j]).l_m>0 ) {
                lm=(lpWorkBuffer->mark[j]).l_m;
                irm=-1;
                for(i=j;i<(lpWorkBuffer->markNumber);i++) {
                      if( (lpWorkBuffer->mark[i]).r_m>0 ) {
                         irm=i;
                         break;
                         }
                      } /* for i */
                if(irm<0) break;
                rm=0;
                for(i=irm+1;i<(lpWorkBuffer->markNumber);i++) {
                      if( (lpWorkBuffer->mark[i]).r_m>0 ) {
                         rm=(lpWorkBuffer->mark[i]).r_m;
                         irm2=i;
                         break;
                         }
                      } /* for i */
                if(rm==0) break;
                if((rm-lm)<=(lpWorkBuffer->lettWidth)) {
                        if(irm2-irm>1) {
                           (lpWorkBuffer->mark[irm]).r_m=-(lpWorkBuffer->mark[irm]).r_m;
                           (lpWorkBuffer->mark[irm+1]).l_m=(lpWorkBuffer->mark[irm]).r_m;
                           (lpWorkBuffer->mark[irm]).g_m=min((lpWorkBuffer->mark[irm]).g_m,(lpWorkBuffer->mark[irm2]).g_m);
                           (lpWorkBuffer->mark[irm]).d_m=max((lpWorkBuffer->mark[irm]).d_m,(lpWorkBuffer->mark[irm2]).d_m);
                           j--;
                           }
                        else {
                           (lpWorkBuffer->mark[irm]).r_m=rm;
                           (lpWorkBuffer->mark[irm]).g_m=min((lpWorkBuffer->mark[irm]).g_m,(lpWorkBuffer->mark[irm2]).g_m);
                           (lpWorkBuffer->mark[irm]).d_m=max((lpWorkBuffer->mark[irm]).d_m,(lpWorkBuffer->mark[irm2]).d_m);
                           for(i=irm2+1;i<(lpWorkBuffer->markNumber);i++) {
                              (lpWorkBuffer->mark[i-1]).l_m=(lpWorkBuffer->mark[i]).l_m;
                              (lpWorkBuffer->mark[i-1]).r_m=(lpWorkBuffer->mark[i]).r_m;
                              (lpWorkBuffer->mark[i-1]).g_m=(lpWorkBuffer->mark[i]).g_m;
                              (lpWorkBuffer->mark[i-1]).d_m=(lpWorkBuffer->mark[i]).d_m;
                              }
                           (lpWorkBuffer->markNumber)--;
                           j--;
                           }
                        }
                }
              j++;

              } /* for j */
        }

   return(1);
} /* of filtrMark() */
void shiftSegm(int* tab, int k, int* len){
  int i;
    (*len)--;
    for(i=k;i<(*len);i++)
       tab[i] = tab[i+1];
} /* of shiftSegm() */
int testCutLeft(int  *hist, int lenH, int cutL, int tH){
int i, h = tH;
    if(hist[cutL] == 0) return cutL;
    i= cutL;
    while(i>0 && h > 0){
    	if(hist[i] == 0) return i;
    	i--;h--;
    }
    return cutL;
}
int testCutRight(int  *hist, int lenH, int cutR, int tH){
int i, h = tH;
    if(hist[cutR] == 0) return cutR;
    i= cutR;
    while(i<lenH && h > 0){
    	if(hist[i] == 0) return i;
    	i++;h--;
    }
    return cutR;
}
int getCutDirection(int *hist, int lenH, int cut){
	int  dL = 0, dR = 0;
	int i, h = 4;
//	char buff[500];

		    if(cut < lenH-1) {
	       dR =  0;
	       i= cut;
	       h = 6;
	       while(i<lenH-1 && h > 0){
	    	   dR += abs(hist[i+1] - hist[i]);
    	      i++;h--;
	       }
	    }
//	    if(cut == 328) {
//	        sprintf(buff,"\n------------ dR= %d\n", dR);
//	         saveLog("", buff);
//	    }

	    if(cut > 1) {
	       dL = 0;
	       i= cut;
	       h = 6;
	       while(i>1 && h > 0){
	    	   dL += abs(hist[i] - hist[i-1]);
    	      i--;h--;
	       }
	    }
//	    if(cut == 328) {
//	        sprintf(buff,"\n------------ dL= %d\n", dL);
//	         saveLog("", buff);
//	    }

	    if(dL < dR)
	    	return -1;
	    if(dL > dR)
	    	return 1;

   return 0;
}
int testShiftLeftOrRight(int *hist, int lenH, int cut){
	int newCut = cut;
//	int direction;
//	char buff[500];

    if(hist[cut] == 0) return cut;
    if(cut > 1){
    	newCut = testCutLeft(hist, lenH, cut, 8);
       	 if(newCut != cut)
       		 return newCut;
    }
    if(cut < lenH){
    	newCut = testCutRight(hist, lenH, cut, 8);
	    if(newCut != cut)
	    	 return newCut;
    }
/*
    direction = getCutDirection(hist, lenH, cut);
//    if(cut == 328) {
//        sprintf(buff,"\n------------ direction= %d\n", direction);
//         saveLog("", buff);
//    }

    if(direction < 0) {
    	newCut = testCutLeft(hist, lenH, cut, 7);
       	 if(newCut != cut)
       		 return newCut;
    }
    if(direction > 0) {
    	newCut = testCutRight(hist, lenH, cut, 7);
       	 if(newCut != cut)
       		 return newCut;
    }
*/
    return cut;
}

int testLeftRight(int *hist, int lenH, int cutL, int cutR, int sizeWImg){
	int i, cc;
	 int thW = sizeWImg/48;
//    char buff[500];

	if(cutR - cutL > thW) return -1;
	cc = 0;
	for(i=cutL;i<=cutR;i++){
		cc += hist[i];
	}
//printf("------------ cutL= %d, cutR= %d, cutL - cutR= %d, cc= %d \n", cutL, cutR, cutL - cutR, cc);
	if(cc < 4 || cutR - cutL < 5) {
//  sprintf(buff," cutR - cutL= %d\n",cutR - cutL);
//     saveLog("", buff);

		return (cutL+cutR)/2;
	}
	return -1;
}

void testSegmConf(IMAGEL *pImage, RECCHARSTRUCT *pRecCharStruct, int *hist, int lenH, int cutL, int cutR){
	 LPBYTE pBuffer;
	 int width0 = 96;
	 int height0 = 96;
	 int width;
	 int height;
	 int * pWidth;
	 int * pHeight;
	 char char1;
	 char * pChar;
	 int conf;
	 int * pConf;
     int err;
     long angle = 0, shift = 0;
//     char buff[500];

         pBuffer = &bufferSeg[0];
         pWidth = &width;
         pHeight = &height;
         pChar = &char1;
         pConf = &conf;
         width = width0; height = height0;
         err = extractChar(pImage, angle, shift,
        		           cutL, cutR,
                           pBuffer, pWidth, pHeight);
//     	  sprintf(buff,"extract; err= %d\n",err);
//          saveLog("", buff);

         if(err < 0) conf = 0;
         else
            {err = recChar(pBuffer, width, height, pImage,
                           pRecCharStruct,
                           pChar, pConf);

//       	  sprintf(buff,"recChar; err= %d\n",err);
//          saveLog("", buff);

             if(err < 0) conf = 0;
            }
//      	  sprintf(buff,"recChar; cutL= %d, cutR= %d, conf= %d, char1= %c\n",cutL, cutR, conf, char1);
//           saveLog("", buff);
}

void addNewCut(int iCut, int cut){
	int n;
    n = indxMarkC2-1;
    while(n>=iCut) {
       markConst[n+1] = markConst[n];
       n--;
    }
   markConst[iCut] = cut;
   indxMarkC2++;
}
void deleteCut(int iCut){
	int n;
//    char buff[500];

//    sprintf(buff,"deleteCut; indxMarkC2= %d\n",indxMarkC2);
//    saveLog("", buff);
    n = iCut;
    while(n<indxMarkC2-1) {
       markConst[n] = markConst[n+1];
// sprintf(buff,"n= %d, markConst[n] = %d \n",n, markConst[n]);
//        saveLog("", buff);

       n++;
    }
    indxMarkC2--;
//    sprintf(buff,"deleteCut2; indxMarkC2= %d\n",indxMarkC2);
//    saveLog("", buff);
}

void lastTestSegm(IMAGEL *pImage, RECCHARSTRUCT *pRecCharStruct, int *hist, int lenH, int i1, int i2){
	 LPBYTE pBuffer;
	 int width0 = 96;
	 int height0 = 96;
	 int width;
	 int height;
	 int * pWidth;
	 int * pHeight;
	 char char1;
	 char * pChar;
	 int conf, cnf1=0, cnf2;
	 int * pConf;
     int err;
     long angle = 0, shift = 0;
     int cutL, cutR1, cutR2;
     int k;

//     char buff[500];
//     int i;

     cutL  = markConst[i1];
     cutR1 = markConst[i2];
     if(i2-1>1)
        cutR2 = markConst[i2-1];
     else
        cutR2 = markConst[1];

// sprintf(buff,"lastTestSegm; cutL= %d cutR1= %d, cutR2= %d\n",cutL, cutR1, cutR2);
// saveLog("", buff);

if(hist[cutR1] != 0) {
// sprintf(buff,"lastTestSegm; hist[cutR1]= %d cutR1 - cutL= %d, cutR2-cutL= %d\n",hist[cutR1], cutR1 - cutL, cutR2-cutL);
// saveLog("", buff);
      if(hist[cutL] == 0) {
// add new cut
    	  k = cutL;
    	  while(k < lenH && hist[k] == 0) k++;
    	  if(k - cutL > 3 && cutR1 - k > 10) {
    		  addNewCut(i1, k);
    	      cutL  = markConst[i1];

/*
  sprintf(buff,"add new cut; i1= %d, cutL= %d\n",i1, cutL);
  saveLog("", buff);

  //---------------------------------------------------
       for(i=0;i<indxMarkC2;i++) {
     	  sprintf(buff,"%d, ",markConst[i]);
           saveLog("", buff);
       }
       saveLog("", "\n");
  //---------------------------------------------------

*/
    	  }
      }
}

//      sprintf(buff,"lastTestSegm; hist[cutR1]= %d cutR1 - cutL= %d, cutR2-cutL= %d\n",hist[cutR1], cutR1 - cutL, cutR2-cutL);
//      saveLog("", buff);

       if((hist[cutR1] == 0) || (cutR1 - cutL > 30) || (cutR2-cutL) > width0) return;
         pBuffer = &bufferSeg[0];
         pWidth = &width;
         pHeight = &height;
         pChar = &char1;
         pConf = &conf;
         width = width0; height = height0;
         err = extractChar(pImage, angle, shift,
        		           cutL, cutR1,
                           pBuffer, pWidth, pHeight);
//     	  sprintf(buff,"extract; err= %d\n",err);
//          saveLog("", buff);

         if(err < 0) conf = 0;
         else
            {
//saveCharacter(pBuffer, width, height, 0, charIdxTmp, "test.bmp"); charIdxTmp++;
        	 err = recChar(pBuffer, width, height, pImage,
                           pRecCharStruct,
                           pChar, pConf);

//       	  sprintf(buff,"recChar; err= %d\n",err);
//          saveLog("", buff);

             if(err < 0) conf = 0;
             cnf1 = conf;
            }
//      	  sprintf(buff,"recChar; cutL= %d, cutR= %d, conf= %d, char1= %c\n",cutL, cutR1, cnf1, char1);
//           saveLog("", buff);
           width = width0; height = height0;
           k = cutR1;
           while(k < lenH && hist[k] != 0) k++;
           if(cutR2- k < 5) k = cutR2;
           else{
     		  addNewCut(i2, k);
     		  cutR2 = markConst[i2];
     		  i2++;
/*
   		  sprintf(buff,"add new cut; i2= %d, cutR2= %d\n",i2, cutR2);
   		  saveLog("", buff);

     		  //---------------------------------------------------
     		       for(i=0;i<indxMarkC2;i++) {
     		     	  sprintf(buff,"%d, ",markConst[i]);
     		           saveLog("", buff);
     		       }
     		       saveLog("", "\n");
     		  //---------------------------------------------------
*/
           }


           err = extractChar(pImage, angle, shift,
          		           cutL, cutR2,
                             pBuffer, pWidth, pHeight);

//      	   sprintf(buff,"extract; err= %d\n",err);
//           saveLog("", buff);

           if(err < 0) conf = 0;
           else
              {
//   saveCharacter(pBuffer, width, height, 0, charIdxTmp, "test.bmp"); charIdxTmp++;

        	   err = recChar(pBuffer, width, height, pImage,
                             pRecCharStruct,
                             pChar, pConf);
//           	  sprintf(buff,"recChar; err= %d\n",err);
//              saveLog("", buff);

               if(err < 0) conf = 0;
               cnf2 = conf;
//          	  sprintf(buff,"recChar; cutL= %d, cutR= %d, conf= %d, char1= %c\n",cutL, cutR2, cnf2, char1);
//               saveLog("", buff);

               if(cnf2 > cnf1){
            	   deleteCut(i2);
/*
	      	  sprintf(buff,"delete i2= %d, markConst[i2]= %d\n",i2, markConst[i2]);
	            saveLog("", buff);

            	   //---------------------------------------------------
            	        for(i=0;i<indxMarkC2;i++) {
            	      	  sprintf(buff,"%d, ",markConst[i]);
            	            saveLog("", buff);
            	        }
            	        saveLog("", "\n");
            	   //---------------------------------------------------
*/
               }
              }
}
int getLeftFromHist(int *hist, int lenH, int th) {
	int  i, mass;
	for(i=3;i<lenH;i++){
	   mass = (hist[i-2]+hist[i-1]+hist[i])/3;
	   if(mass > th)
		   return i;
    }
	return hist[0];
}
int getRightFromHist(int *hist, int lenH, int th) {
	int  i, mass;
	for(i=lenH-4;i>0;i--){
	   mass = (hist[i+2]+hist[i+1]+hist[i])/3;
	   if(mass > th)
		   return i;
    }
	return 0;
}

void getGapsFromHist(int *hist, int lenH, int left, int right, int* sGap, int* bGap) {
#define MAX_SP 100
	int  i, k, iMax1, min, max, max2;
	int sp1, sp2;
	int avMin, avMax, ccMin, ccMax;
	int spTab[MAX_SP];
	sp1 = 0;sp2 = 0;
	k = 0;
	for(i=left;i<right;i++){
	   if(hist[i] < 2){
         if(sp1 == 0) sp1 = i;
	   }else{
		 if(sp1 != 0 && sp2 == 0) {
			 sp2 = i;
			 if(k < MAX_SP) {
			    spTab[k] = sp2-sp1;
			    k++;
			 }
			 sp1 = 0;sp2 = 0;
		 }
	  }
    }

//	for(i=0;i<k;i++){
//	 printf(" ---------------------i= %d, spTab[i]= %d \n", i, spTab[i]);
//	}

	min = 2000;
	max = 0;
	iMax1 = 0;
	for(i=0;i<k;i++){
		if(spTab[i] < min) min = spTab[i];
		if(spTab[i] > max) {max = spTab[i];iMax1 = i;}
	}
	max2 = 0;
	for(i=0;i<k;i++){
		if(spTab[i] > max2 && spTab[i] < max) max2 = spTab[i];
	}
	if(max2 > 20 && max2 < max) max = max2;
	else iMax1 = -1;

//printf(" ---------------------min= %d, max= %d \n", min, max);
	avMin = 0; avMax = 0;
	ccMin= 0; ccMax = 0;
	for(i=0;i<k;i++){
		if(abs(spTab[i]-min) < 4){
			avMin += spTab[i];ccMin++;
//			 printf(" min-------------------i= %d, spTab[i]= %d \n", i, spTab[i]);
		}
		if(abs(spTab[i]-max) < 10 && i != iMax1){
			avMax += spTab[i];ccMax++;
//			 printf(" max-------------------i= %d, spTab[i]= %d \n", i, spTab[i]);
		}
	}
	if(ccMin > 0) avMin /= ccMin;
	if(ccMax > 0) avMax /= ccMax;

	*sGap = avMin;
	*bGap = avMax;
}

void addSpaces(int  *hist, int lenH, int sizeWImg){
	 int i,  i1; //, mass;
	 int cut1, cut2;
	 int cutNew1, cutNew2;
	 int ok;
//	 char buff[500];
	 int thW = sizeWImg/45;
	 int left, right;
	 int smallGap, bigGap;
	 int thW2;

// printf(" ----------------------------------\n");
	 left=getLeftFromHist(hist, lenH, 4);
	 right=getRightFromHist(hist, lenH, 4);
	 getGapsFromHist(hist, lenH, left, right, &smallGap, &bigGap);
	 thW2 = (3*bigGap+smallGap)/4;
	 if(thW > thW2) thW = thW2;
//	 printf(" ---------------------left= %d, right= %d, right - left= %d \n", left, right, right - left);
//	 printf(" ---------------------smallGap= %d, bigGap= %d, thW= %d \n", smallGap, bigGap, thW);

/*
	 //---------------------------------------------------
	      for(i=0;i<indxMarkC2;i++) {
	    	  sprintf(buff,"m= %d,",markConst[i]);
	          saveLog("", buff);
	      }
	 //---------------------------------------------------
	      saveLog("", "\n");

     for(i=1;i<lenH;i++) {
   	  sprintf(buff,"i= %d, hist= %d\n",i, hist[i]);
         saveLog("", buff);
     }
*/
	i = 1;
	indxMarkC2 = indxMarkC;
    while(i<indxMarkC2-1) {
        i1=indxMarkC2-i-1;
//        i2=indxMarkC2-i-2;
        cut1 = markConst[i1];
        cut2 = markConst[i1+1];
        cutNew1 = cut1;
        cutNew2 = cut1;
        ok = 1;
        while(cutNew1>cut2){
        	if(hist[cutNew1] != 0) break;
        	cutNew1--;
        }
        if(cutNew1-cut2 < 8)
        	ok = 0;
        else
        	cutNew1++;
//    sprintf(buff," cut1 = %d, cut2 = %d, cutNew1 = %d, ok= %d \n",cut1, cut2, cutNew1, ok);
//    saveLog("", buff);

        if(ok == 1) {
           cut2 = markConst[i1-1];
//           cutNew2 = cut1;
           while(cutNew2<cut2){
        	   if(hist[cutNew2] != 0) break;
        	   cutNew2++;
           }
           if(cut2 - cutNew2 < 8)
           	  ok = 0;
           else
        	   cutNew2--;
//      sprintf(buff," cut1 = %d, cut2 = %d, cutNew2 = %d, ok= %d \n",cut1, cut2, cutNew2, ok);
//       saveLog("", buff);

        }
//        if(ok == 1) {
//       	   sprintf(buff," cutNew2-cutNew1 = %d\n",cutNew2-cutNew1);
//       	   saveLog("", buff);
//        }

        if(ok == 1 && cutNew2-cutNew1> thW) {
//  	   sprintf(buff," cutNew2-cutNew1 = %d\n",cutNew2-cutNew1);
//   	   saveLog("", buff);

        	markConst[i1] = cutNew1;
        	addNewCut(i1, cutNew2);
        	i++;
//  	        mass = getMassFromTheSegment(hist, lenH, cutNew1, cutNew2);
//  	        sprintf(buff," new cuts cutNew1= %d, cutNew2= %d \n",cutNew1, cutNew2);
//  	        saveLog("", buff);


        }

//  sprintf(buff," cut2-cut1 = %d, mass= %d \n",cut2-cut1, mass);
//   saveLog("", buff);
/*
        if(cut2-cut1 < 25){
        	mass = getMassFromTheSegment(hist, lenH, cut1, cut2);
        	if(mass < 8) {

//  	      	  sprintf(buff,"deleteCut; i1 = %d, cut1= %d, i2= %d, cut2= %d\n",i1, cut1, i2, cut2);
//  	            saveLog("", buff);

        	   markConst[i1] = (cut1+cut2)/2;
        	   deleteCut(i2);
        	}
        }
*/
        i++;
    }
    indxMarkC = indxMarkC2;
//    saveLog("-----------------------", "\n\n");
}

void cleanSpaces(int  *hist, int lenH){
	 int i, i1, i2, mass;
	 int cut1, cut2;
//	 char buff[500];
/*
	 //---------------------------------------------------
	      for(i=0;i<indxMarkC2;i++) {
	    	  sprintf(buff,"m= %d,",markConst[i]);
	          saveLog("", buff);
	      }
	 //---------------------------------------------------
	      saveLog("", "\n");
*/
	i = 0;
	indxMarkC2 = indxMarkC;
    while(i<indxMarkC2-1) {
        i1=indxMarkC2-i-1;
        i2=indxMarkC2-i-2;
        cut1 = markConst[i1];
        cut2 = markConst[i2];

  	mass = getMassFromTheSegment(hist, lenH, cut1, cut2);

//  sprintf(buff," cut2-cut1 = %d, mass= %d \n",cut2-cut1, mass);
//   saveLog("", buff);

        if(cut2-cut1 < 25){
        	mass = getMassFromTheSegment(hist, lenH, cut1, cut2);
        	if(mass < 8) {

//  	      	  sprintf(buff,"deleteCut; i1 = %d, cut1= %d, i2= %d, cut2= %d\n",i1, cut1, i2, cut2);
//  	            saveLog("", buff);

        	   markConst[i1] = (cut1+cut2)/2;
        	   deleteCut(i2);
/*
        	   //---------------------------------------------------
        	        for(i=0;i<indxMarkC2;i++) {
        	      	  sprintf(buff,"m= %d,",markConst[i]);
        	            saveLog("", buff);
        	        }
        		      saveLog("", "\n");
        	   //---------------------------------------------------
*/
        	}
        }
        i++;
    }
    indxMarkC = indxMarkC2;
//    saveLog("-----------------------", "\n\n");
}

void cleanCuts(IMAGEL *pImage, RECCHARSTRUCT *pRecCharStruct, int  *hist, int lenH, int sizeW){
	 int i, i1, i2, k;
	 int cut1, cut2, cut;
//	 char buff[500];
//	 int ii;

/*
//saveLog("-----------------------------------", "\n");
     for(i=1;i<lenH;i++) {
   	  sprintf(buff,"i= %d, hist[i]= %d\n",i,hist[i]);
         saveLog("", buff);
     }
  saveLog("-----------------------------------", "\n");

//---------------------------------------------------
     for(i=0;i<indxMarkC;i++) {
   	  sprintf(buff,"m= %d,",markConst[i]);
         saveLog("", buff);
     }
//---------------------------------------------------
*/
//        saveLog("--------------------------------------------------------------------------------", "\n");
     //---------------------------------------------------
//          for(i=0;i<indxMarkC;i++) {
//        	  sprintf(buff,"%d, ",markConst[i]);
//              saveLog("", buff);
//          }
     //---------------------------------------------------
//          saveLog("", "\n\n");

      i=0;
      indxMarkC2 = indxMarkC;
      while(i<indxMarkC2-1) {
         i1=indxMarkC2-i-1;
         i2=indxMarkC2-i-2;
         cut1 = markConst[i1];
         cut2 = markConst[i2];

//   sprintf(buff,"\n------------ cut1= %d, cut2= %d, i= %d, i1= %d, i2= %d\n", cut1, cut2, i, i1, i2);
//   saveLog("", buff);

    cut = testShiftLeftOrRight(hist, lenH, cut1);
	 if(cut != cut1)
		 markConst[i1] = cut;


// sprintf(buff,"------------ cut1= %d,  i= %d, i1= %d\n", markConst[i1], i, i1);
// saveLog("", buff);

 cut = testShiftLeftOrRight(hist, lenH, cut2);
	 if(cut != cut2)
		 markConst[i2] = cut;

// sprintf(buff,"------------ cut2= %d,  i= %d, i2= %d\n", markConst[i2], i, i2);
// saveLog("", buff);



 cut = testLeftRight(hist, lenH, markConst[i1], markConst[i2], sizeW);
    	 if(cut > 0){
      		 markConst[i2] = cut;
    		  k = i1;
    	      while(k<indxMarkC2-1) {
//    	          i3=indxMarkC2-k-1;
    	          markConst[k] = markConst[k+1];
    	          k++;
    	      }
    		 indxMarkC2--;
//    		 i++;

//		 sprintf(buff,"------------ testLeftRight; cut= %d,  i= %d, i1= %d, i2= %d\n", cut, i, i1, i2);
//   		saveLog("", buff);

    	 }else{
//    		 sprintf(buff,"------------lastTestSegm\n");
//    		 saveLog("", buff);

             if(i2 > 1){
	    	    lastTestSegm(pImage, pRecCharStruct, hist, lenH, i1, i2);
             }

    		 i++;
    	 }

//  testSegmConf(pImage, pRecCharStruct, hist, lenH, markConst[i1], markConst[i2]);
     }

      //---------------------------------------------------
//           for(i=0;i<indxMarkC2;i++) {
//         	  sprintf(buff,"%d, ",markConst[i]);
//               saveLog("", buff);
//           }
      //---------------------------------------------------
//           saveLog("1", "\n\n");

//k = indxMarkC - indxMarkC2;
//for(i=0;i<indxMarkC2;i++) {
//	markConst[i] = markConst[i+k];
//      }
indxMarkC = indxMarkC2;

cleanSpaces(hist, lenH);

//---------------------------------------------------
//     for(i=0;i<indxMarkC;i++) {
//   	  sprintf(buff,"%d, ",markConst[i]);
//         saveLog("", buff);
//     }
//---------------------------------------------------
//     saveLog("2", "\n\n");
  addSpaces(hist, lenH, sizeW);
//---------------------------------------------------
//     for(i=0;i<indxMarkC;i++) {
//   	  sprintf(buff,"%d, ",markConst[i]);
//         saveLog("", buff);
//     }
//---------------------------------------------------
//     saveLog("3", "\n\n");
/*
saveLog("\n\n", "\n");
//---------------------------------------------------
     for(i=0;i<indxMarkC;i++) {
   	  sprintf(buff,"m= %d,",markConst[i]);
         saveLog("", buff);
     }
//---------------------------------------------------

  saveLog("\n\n", "\n");
*/
}
int segmUNP(int  begHOtp, STRUCTWB * lpWorkBuffer){
 int i, i1, i2;
 int prevMax;
 int prevMin;
 int tHm, rm, j, x, y, yB;


       prevMin=0;
       prevMax=0;
       for(i=0;i<indxMarkC;i++) {
        markConst[i]+=((lpWorkBuffer->curLine[0]).x1+begHOtp-1);
        }

       for(i=0;i<indxMarkC-1;i++) {
             i1=indxMarkC-i-1;
             i2=indxMarkC-i-2;
             (lpWorkBuffer -> arrCellP[i]).lT.x=markConst[i1];
             (lpWorkBuffer -> arrCellP[i]).lT.y=minYmark(markConst[i1],(int *)&prevMin,lpWorkBuffer);

             (lpWorkBuffer -> arrCellP[i]).lB.x=markConst[i1];
             (lpWorkBuffer -> arrCellP[i]).lB.y=maxYmark(markConst[i1],(int *)&prevMax,lpWorkBuffer);


             (lpWorkBuffer -> arrCellP[i]).rT.x=markConst[i2];
             (lpWorkBuffer -> arrCellP[i]).rT.y=minYmark(markConst[i2],(int *)&prevMin,lpWorkBuffer);

             (lpWorkBuffer -> arrCellP[i]).rB.x=markConst[i2];
             (lpWorkBuffer -> arrCellP[i]).rB.y=maxYmark(markConst[i2],(int *)&prevMax,lpWorkBuffer);
             }


       (lpWorkBuffer ->indUnpB)=0;
       (lpWorkBuffer ->indUnpE)=indxMarkC-1;

/* -------------- M recognition !!!!  */
       if( (lpWorkBuffer->flgBarC) && (indxMarkC>2) ) {

          j = indxMarkC;
          for(i=0;i<indxMarkC-1;i++) {
            tHm=45;
            rm=0;
            y  = max((lpWorkBuffer -> arrCellP[i]).lT.y,(lpWorkBuffer -> arrCellP[i]).rT.y);
            yB = max((lpWorkBuffer -> arrCellP[i]).lB.y,(lpWorkBuffer -> arrCellP[i]).rB.y);

            while( (y<yB) && (rm<tHm) ) {
               for(x=(lpWorkBuffer -> arrCellP[i]).lT.x;x<(lpWorkBuffer -> arrCellP[i]).rT.x;x++)  {
                if(whatIsThis(lpWorkBuffer,x,y)) {
                   rm++;
                   }
                }
               y++;
               }
            if(rm>=tHm) {  j = i;break;}
            }
           if(j+2<indxMarkC-1)  {
             for(i=indxMarkC-1;i>j;i--) {
               (lpWorkBuffer -> arrCellP[i]).lT.x=(lpWorkBuffer -> arrCellP[i-1]).lT.x;
               (lpWorkBuffer -> arrCellP[i]).lT.y=(lpWorkBuffer -> arrCellP[i-1]).lT.y;

               (lpWorkBuffer -> arrCellP[i]).lB.x=(lpWorkBuffer -> arrCellP[i-1]).lB.x;
               (lpWorkBuffer -> arrCellP[i]).lB.y=(lpWorkBuffer -> arrCellP[i-1]).lB.y;

               (lpWorkBuffer -> arrCellP[i]).rT.x=(lpWorkBuffer -> arrCellP[i-1]).rT.x;
               (lpWorkBuffer -> arrCellP[i]).rT.y=(lpWorkBuffer -> arrCellP[i-1]).rT.y;

               (lpWorkBuffer -> arrCellP[i]).rB.x=(lpWorkBuffer -> arrCellP[i-1]).rB.x;
               (lpWorkBuffer -> arrCellP[i]).rB.y=(lpWorkBuffer -> arrCellP[i-1]).rB.y;
               }

             (lpWorkBuffer -> arrCellP[j]).rT.x=(lpWorkBuffer -> arrCellP[j+2]).rT.x;
             (lpWorkBuffer -> arrCellP[j]).rT.y=(lpWorkBuffer -> arrCellP[j+2]).rT.y;

             (lpWorkBuffer -> arrCellP[j]).rB.x=(lpWorkBuffer -> arrCellP[j+2]).rB.x;
             (lpWorkBuffer -> arrCellP[j]).rB.y=(lpWorkBuffer -> arrCellP[j+2]).rB.y;
             (lpWorkBuffer ->indUnpE)++;
             }

          }
/* -------------- M recognition !!!!  */
       return(0);
} /* of segmUNP() */
/***************************************************************************
 *  PURPOSE:       check beginning spaces for special segmentation version *
 *  RETURN VALUE :                                                         *
 *                 number of spaces,                                       *
 *                                                                         *
 ***************************************************************************/
int firstSpaces(STRUCTWB *lpWorkBuffer){
 int begB, lenL, lm, rm, dd;
 int lC, rC, massL, massR;
    begB = (lpWorkBuffer -> begRect.x);
    lenL = (lpWorkBuffer->  lettWidth);
    if(lenL <= 0) {return(0);}
    lm   = (lpWorkBuffer->mark[0]).l_m;
    rm   = (lpWorkBuffer->mark[0]).r_m;
    dd   = rm - lm;
    lm  -= begB;
    rm  -= begB;
    if(dd < 2) {return(0);}
    lC = lm/lenL;
    rC = rm/lenL;
    if(lC == rC)  {return(lC);}
    if(rC > lC+1) {return(lC+1);}
    massL = (lC+1)*lenL - lm;
    massR = rm - rC*lenL;
    if(massL > massR) {return(lC);}
    return(rC);
}
/***************************************************************************
 *  PURPOSE      : search for right marker                                 *
 *                                                                         *
 ***************************************************************************/
int rightM(STRUCTWB *lpWorkBuffer, int crL){
  int i;
    for(i=crL;i<(lpWorkBuffer->markNumber);i++) {
         if( (lpWorkBuffer->mark[i]).r_m>0 ) return(i);
         }
    return((lpWorkBuffer->markNumber)-1);
} /* of rightM() */
/***************************************************************************
 *  PURPOSE      :search index marks array for x                           *
 ***************************************************************************/
int srIndMark(STRUCTWB *lpWorkBuffer, int crL, int crR, int x){
 int i, lm, rm;
      for(i=crL;i<=crR;i++)  {
        lm=(lpWorkBuffer->mark[i]).l_m;
        if(lm<0) lm=-lm;
        rm=(lpWorkBuffer->mark[i]).r_m;
        if(rm<0) rm=-rm;
        if( (x>=lm) && (x<rm) ) return(i);
        }
      for(i=0;i<crL;i++)  {
        lm=(lpWorkBuffer->mark[i]).l_m;
        if(lm<0) lm=-lm;
        rm=(lpWorkBuffer->mark[i]).r_m;
        if(rm<0) rm=-rm;
        if( (x>=lm) && (x<rm) ) return(i);
        }
      return(-1);
 } /* of srIndMark */

int sendCh(int scal,STRUCTWB *lpWorkBuffer,int kindD){
  int i, y, y1, tr, isAdj, rm, Ok, Ok1, isAdj2;
  int  add, crR, crL, flgI, tmpTr;
  int *lpTr;
  int *lpLen;

     isAdj=0;
     isAdj2=0;
     flgI=1;
   if(lpWorkBuffer->lengthTraceR >= MAXLTRACE)
      lpWorkBuffer->lengthTraceR = MAXLTRACE-1;
   if(lpWorkBuffer->lengthTraceL >= MAXLTRACE)
      lpWorkBuffer->lengthTraceL = MAXLTRACE-1;
   if(lpWorkBuffer->lengthTraceC >= MAXLTRACE)
      lpWorkBuffer->lengthTraceC = MAXLTRACE-1;

    if(lpWorkBuffer->traceL[0] < 0)
      lpWorkBuffer->traceL[0] = 0;
   if(lpWorkBuffer->traceL[0] >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->traceL[0] = lpWorkBuffer -> hightImg-1;
   if(lpWorkBuffer->traceL[0]+lpWorkBuffer->lengthTraceL >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->lengthTraceL = 0;

    if(lpWorkBuffer->traceC[0] < 0)
      lpWorkBuffer->traceC[0] = 0;
   if(lpWorkBuffer->traceC[0] >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->traceC[0] = lpWorkBuffer -> hightImg-1;
   if(lpWorkBuffer->traceC[0]+lpWorkBuffer->lengthTraceC >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->lengthTraceC = 0;

// --------------------------------------------------------

   if(lpWorkBuffer->lengthTraceL <= 0) {
       lpWorkBuffer->lengthTraceL++;
       if(lpWorkBuffer->traceL[1] < (lpWorkBuffer->infFrame[0]).x2 -1)
          lpWorkBuffer->traceL[1]++;
       else
         return(3);
       if(lpWorkBuffer->lengthTraceC <= 0)
          lpWorkBuffer->lengthTraceC++;
       return(-200);
       }
   if(lpWorkBuffer->lengthTraceC <= 0)  {
       lpWorkBuffer->lengthTraceC++;
       return(-200);
       }
// -----------------------------------

ADJ: y=(lpWorkBuffer->traceL[0]);
     y1=(lpWorkBuffer->traceC[0]);
     if(y>y1) {
        lpTr=(int *)(&(lpWorkBuffer->traceL[0]));
        lpLen=(int *)(&(lpWorkBuffer->lengthTraceL));
        add=y-y1;
        }
     else{
        lpTr=(int *)(&(lpWorkBuffer->traceC[0]));
        lpLen=(int *)(&(lpWorkBuffer->lengthTraceC));
        add=y1-y;
        }
     if(y!=y1)  adjY(lpTr,lpLen,add);
     y=(lpWorkBuffer->lengthTraceL);
     y1=(lpWorkBuffer->lengthTraceC);
     if(y1>y) {
        lpTr=(int *)(&(lpWorkBuffer->traceL[0]));
        lpLen=(int *)(&(lpWorkBuffer->lengthTraceL));
        add=y1-y;
        }
     else{
        lpTr=(int *)(&(lpWorkBuffer->traceC[0]));
        lpLen=(int *)(&(lpWorkBuffer->lengthTraceC));
        add=y-y1;
        }
     if(y!=y1)  adjL(lpTr,lpLen,add);

   if(lpWorkBuffer->lengthTraceR >= MAXLTRACE)
      lpWorkBuffer->lengthTraceR = MAXLTRACE-1;
   if(lpWorkBuffer->lengthTraceL >= MAXLTRACE)
      lpWorkBuffer->lengthTraceL = MAXLTRACE-1;
   if(lpWorkBuffer->lengthTraceC >= MAXLTRACE)
      lpWorkBuffer->lengthTraceC = MAXLTRACE-1;

    if(lpWorkBuffer->traceL[0] < 0)
      lpWorkBuffer->traceL[0] = 0;
   if(lpWorkBuffer->traceL[0] >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->traceL[0] = lpWorkBuffer -> hightImg-1;
   if(lpWorkBuffer->traceL[0]+lpWorkBuffer->lengthTraceL >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->lengthTraceL = 0;

    if(lpWorkBuffer->traceC[0] < 0)
      lpWorkBuffer->traceC[0] = 0;
   if(lpWorkBuffer->traceC[0] >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->traceC[0] = lpWorkBuffer -> hightImg-1;
   if(lpWorkBuffer->traceC[0]+lpWorkBuffer->lengthTraceC >= lpWorkBuffer -> hightImg)
      lpWorkBuffer->lengthTraceC = 0;

// --------------------------------------------------------

   if(flgUNPROP) goto ENDUNP;
   if(!isAdj2) {
    tmpTr=(lpWorkBuffer->trush);
    if((lpWorkBuffer->lettWidth)>0) {
     if((add=insideBox(lpWorkBuffer,kindD))==0) {
        (lpWorkBuffer->trush)=tmpTr;
        goto INSB;
        }
     if(add==-50) {
         isAdj2=1;
         goto ADJ;
         }
     }
     add=apostrof(lpWorkBuffer,0,kindD);

//     if(add==-2) return(1001);
     if(add==-2)  return(-200);
     if(add==-1) return(45);
     if(add==0) return(-200);
     if( tmpTr ) {
           if(checkITrush(lpWorkBuffer)) {
                return(-200);
                }
        }

     if((kindD==5)||(kindD==7)) coupleLett(lpWorkBuffer);

     if((!isAdj)&&((lpWorkBuffer->boundLeft)==0)) {
        y=(lpWorkBuffer->traceL[1]);
        y1=(lpWorkBuffer->traceC[1]);
        add=y1-y;
        y=(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]);
        y1=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
        add+=(y1-y);
        add/=2;
        if((kindD==5)||(kindD==7)) {
            tr=((lpWorkBuffer->highGlob)*2)/3;
            }
        else {
            tr=min(25,(lpWorkBuffer->lengthTraceL)/2);
            }

        if(add<tr) {
           switch(kindD)  {
               case 5: case 7:
                       Ok=checkIOCRsm(lpWorkBuffer);
                       break;
               case 6: Ok=checkIOCR(lpWorkBuffer);
                       break;
               default:Ok=checkI(lpWorkBuffer);break;
               }
           if(Ok)  {
           switch(kindD)  {
               case 5: case 7:
                       Ok1=checkIROCRsm(scal,lpWorkBuffer);
                       break;
               case 6: Ok1=checkIRightOCR(scal,lpWorkBuffer);
                       break;
               case 2: Ok1=checkIRight(scal,lpWorkBuffer);
                       break;
               default:Ok1=0;break;
               }
            if(Ok1)  {
INSB:         rm=(lpWorkBuffer->mark[(lpWorkBuffer->curMark2)]).r_m;
              if(rm<0) rm=-rm;
              y=(lpWorkBuffer->traceC[1]);
              y1=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
              tr=(y+y1)/2;
              add=rm-tr;

/**********************************************************************/
              if((lpWorkBuffer->lettWidth)>0) {
                y=(lpWorkBuffer->lettWidth)-2;
                }
              else {
                y=(lpWorkBuffer->lengthTraceL)+((lpWorkBuffer->lengthTraceL)/3);
                }
/**********************************************************************/

              if(add<4) {
                    if((lpWorkBuffer->curMark1)<((lpWorkBuffer->markNumber)-1)) {
                        crL=(lpWorkBuffer->curMark2)+1;
                        crR=rightM(lpWorkBuffer,crL);
                        rm=(lpWorkBuffer->mark[crR]).r_m;

                        if( (rm>0)&&((rm-tr)<y) )  {
                            tr=(lpWorkBuffer->mark[crR]).d_m-(lpWorkBuffer->mark[crR]).g_m+1;
                            for(i=1;i<=tr;i++) {
                                 (lpWorkBuffer->traceC[i])=rm;
                                 }
                            (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crR]).g_m;
                            (lpWorkBuffer->lengthTraceC)=tr;
                            (lpWorkBuffer->curMark1)=crR+1;
                            (lpWorkBuffer->curMark2)=crR;
                            (lpWorkBuffer->newSegm)=1;
                            isAdj=1;
                            goto ADJ;
                            }
                        else {
                            if((lpWorkBuffer->indxBound)==1) {
                                (lpWorkBuffer->indxBound)=0;
                                (lpWorkBuffer->curMark2)=crR;
                                goto OMIT;
                                }
                            y=(lpWorkBuffer->mark[crL]).l_m-(lpWorkBuffer->traceL[1]);
                            y1=(lpWorkBuffer->mark[crL]).l_m-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]);
                            (lpWorkBuffer->boundLeft)=max(y,y1);
                            (lpWorkBuffer->lengthTraceR)=(lpWorkBuffer->mark[crL]).d_m-(lpWorkBuffer->mark[crL]).g_m;
                            (lpWorkBuffer->curMark2)=crR;
                  (lpWorkBuffer->traceR[0])=(lpWorkBuffer->mark[crL]).g_m;
                            for(i=1;i<=(lpWorkBuffer->lengthTraceR);i++)  {
                                 (lpWorkBuffer->traceR[i])=(lpWorkBuffer->mark[crR]).r_m;
                                 }
                            (lpWorkBuffer->mark[crL]).l_m=(lpWorkBuffer->mark[(lpWorkBuffer->curMark1)]).l_m;
                            (lpWorkBuffer->curMark1)=crL;
                            (lpWorkBuffer->indxBound)=1;
                            return(-100);
                            }
                        }
                    } /* add<4 */
               else {
                  if(add<max(((lpWorkBuffer->lettWidth)/5),10) ) {
OMIT:               for(i=1;i<=(lpWorkBuffer->lengthTraceC);i++) {
                        (lpWorkBuffer->traceC[i])=rm;
                        }
                    (lpWorkBuffer->traceC[0])=(lpWorkBuffer->traceR[0]);
                    (lpWorkBuffer->curMark1)=(lpWorkBuffer->curMark2)+1;
                    (lpWorkBuffer->newSegm)=1;
                    isAdj=1;
                    goto ADJ;
                  }
                else {
                    for(i=1;i<=(lpWorkBuffer->lengthTraceR);i++) {
                        (lpWorkBuffer->traceR[i])=rm;
                        }
                    if((add=massCh(lpWorkBuffer)) == 0) {
                        return(-200);
                        }
                    y=(lpWorkBuffer->traceC[1])-(lpWorkBuffer->traceL[1]);
                    y1=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)])-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]);

                    if((y<4)||(y1<4))  {
                       (lpWorkBuffer->boundLeft)=max(y,y1)+1;
                       }
                    else {

                       (lpWorkBuffer->boundLeft)=min(y,y1)+1;
                       y-=y1;
                       if(y<0) y=-y;
                       y=min(y,max((lpWorkBuffer->lettWidth)/5,5));
                       (lpWorkBuffer->boundLeft)+=y;
                       }
                      if((lpWorkBuffer->indxBound)==1) {
                          (lpWorkBuffer->indxBound)=0;
                          goto OMIT;
                          }
                    (lpWorkBuffer->indxBound)=1;
                    return(-100);
                    }
                }
             }
            else {
                flgI=4;
                }

            } /* chI */
         }
        }
       }/* isAdj2 */

ENDUNP:;  return(flgI);
} /* of sendCh() */

int coupleLett(STRUCTWB *lpWorkBuffer){
   int i, minLenTr, widthTr, x, y, cc, highTr, th1, th2;

     minLenTr=min((lpWorkBuffer->lengthTraceL),(lpWorkBuffer->lengthTraceC));
     y=(lpWorkBuffer->traceL[0]);
     highTr=0;
     for(i=1;i<=minLenTr;i++)  {
        if(y<=(lpWorkBuffer->bottBord)) {
          cc=0;
          for(x=(lpWorkBuffer->traceL[i]);x<(lpWorkBuffer->traceC[i]);x++)  {
            if(whatIsThis(lpWorkBuffer,x,y)) {
                cc++;
                }
            }
          if(cc) highTr++;
          y++;
          }
       }

     widthTr=(lpWorkBuffer->traceC[1])-(lpWorkBuffer->traceL[1]);
     widthTr+=((lpWorkBuffer->traceC[minLenTr])-(lpWorkBuffer->traceL[minLenTr]));
     widthTr/=2;
     widthTr++;
     th1=((lpWorkBuffer->highGlob)+(lpWorkBuffer->highGlob)/4)+1;
     th2=((lpWorkBuffer->highGlob)+(2*(lpWorkBuffer->highGlob))/3)+1;
     return(0);

} /* coupleLett() */

int apostrof(STRUCTWB *lpWorkBuffer, int clean, int kindD){
   int  x, len, nFr, lW, tH, y, i, mass, lH, lH1, lH2, cc;
   int  k, xBox, mid, lHUp;
   float rtH, rtW;

     len=(lpWorkBuffer->lengthTraceL);
     x=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceC[1]))/2;
     mid=x;
     nFr=whichFrame(lpWorkBuffer,x);
     lH=(lpWorkBuffer->curLine[nFr]).y2-(lpWorkBuffer->curLine[nFr]).y1;
     lW=(lpWorkBuffer->traceC[1])-(lpWorkBuffer->traceL[1]);
     lW+=((lpWorkBuffer->traceC[len-1])-(lpWorkBuffer->traceL[len-1]));
     lW/=2;
     lHUp=lH/5;
     if(kindD>3) lHUp=lH/8;


     if((len<1)||(lW<1)) return(0);
     rtH=((float)lH/(float)len);

     rtW=((float)(lpWorkBuffer->lettWidth)/(float)lW);

     if(clean) {
       if((lpWorkBuffer->lenLeftC)<=0) return(1);
       i=0;
       while(i<(lpWorkBuffer->lenLeftC)) {
            xBox=lpWorkBuffer->arrCellP[i].rT.x+2;
            if(mid<=xBox) break;
            i++;
            }


       if(i>=(lpWorkBuffer->lenLeftC)) return(1);
         rtW=((float)(lpWorkBuffer->arrCellP[i].rT.x-lpWorkBuffer->arrCellP[i].lT.x)/(float)lW);

         if( ((rtH>(float)2.0)&&(rtW>(float)4.0))||((rtH>(float)3.0)&&(rtW>(float)3.0)) ) {
           mid=0;
           for(k=1;k<=len;k++) {
                if(i!=0) {
                  if(abs((lpWorkBuffer->traceL[k])-(lpWorkBuffer->arrCellP[i].lT.x+DIFC_V))<3) {
                        mid=1;
                        break;
                        }
                  }
                if(i!=(lpWorkBuffer->lenLeftC-1)) {
                  if(abs(lpWorkBuffer->arrCellP[i].rT.x-DIFC_V-(lpWorkBuffer->traceC[k]))<3) {
                        mid=1;
                        break;
                        }
                  } else {
                        if((lpWorkBuffer->arrCellP[i].rT.x+DIFC_V)<(lpWorkBuffer->traceL[k])) {
                            mid=1;
                            break;
                            }
                        }
                }
               if(mid) return(0);
           }
       return(1);
       }
     if(rtH>(float)2.0) {
        if(rtH>(float)3.0) {
          if(rtW>(float)9.0) return(0);
          if(lW>len) {
               if((lpWorkBuffer->traceC[0])>(lpWorkBuffer->curLine[nFr]).y1+lHUp) {
                    if((lpWorkBuffer->traceC[0])+len<(lpWorkBuffer->curLine[nFr]).y2-lH/5) {
                      return(-1);
                      }
                    }
                }
            if(kindD<4) {
                return(-2);
                }
          }
          if(kindD<4) {
             if(rtW>(float)4.0) return(-2);
             }
        }
     tH=max((lpWorkBuffer->lettWidth)*4,240);
     y=(lpWorkBuffer->traceL[0]);
     mass=0;
     lH1=lH2=0;
     for(i=1;i<=len;i++)  {
        cc=0;
        for(x=(lpWorkBuffer->traceL[i]);x<(lpWorkBuffer->traceC[i]);x++)  {
            if(whatIsThis(lpWorkBuffer,x,y)) {
                cc++;
                }
            }
        if(cc>0) {
          mass+=cc;
          lH2=i;
          if(lH1==0) lH1=i;
          }
        if(mass>tH) return(1);
        y++;
        }
     cc=lH2-lH1+1;
     if(cc<1) return(0);

     if(((float)lH/(float)cc)>(float)3.0) {
        lW=(lpWorkBuffer->traceC[1])-(lpWorkBuffer->traceL[1]);
        lW+=((lpWorkBuffer->traceC[len-1])-(lpWorkBuffer->traceL[len-1]));
        lW/=2;
        if(lW>cc) {
                if((lpWorkBuffer->traceC[0])+lH1>(lpWorkBuffer->curLine[nFr]).y1+lHUp) {
                    if((lpWorkBuffer->traceC[0])+lH2<(lpWorkBuffer->curLine[nFr]).y2-lH/5) {
                      return(-1);
                      }
                    }
                }
          if(kindD<4) {
            return(-2);
            }
        }
     if(kindD>3) {
        if(mass>15) return(1);
        }
    else {
        if(mass>30) return(1);
        }
     (lpWorkBuffer->trush)=0;
     return(0);
} /* apostrof() */

int massCh(STRUCTWB *lpWorkBuffer){
   int i, mass, x, y, len, tH;

     len=(lpWorkBuffer->lengthTraceL);
     tH=max((lpWorkBuffer->lettWidth)/2,30);
     y=(lpWorkBuffer->traceL[0]);
     mass=0;
     for(i=1;i<=len;i++)  {
        for(x=(lpWorkBuffer->traceL[i]);x<(lpWorkBuffer->traceC[i]);x++)  {
            if(whatIsThis(lpWorkBuffer,x,y)) {
                mass++;
                }
            if(mass>tH) return(1);
            }
        y++;
        }
     (lpWorkBuffer->trush)=0;
     return(0);
} /* massCh() */
/***************************************************************************
 *  PURPOSE      :find spaces for one line                                 *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                length of font                                           *
 ***************************************************************************/
int isMonoSpace1(STRUCTWB *lpWorkBuffer, int kindD){
  int  i, k,  rm, cSp, lmNext, srS, srB, srT, tt;
  LONG sr;
       tt = 0;
       if(kindD < 4) {tt = 1;}
       cSp=0;
       sr=0L;
       for(i=0;i<(lpWorkBuffer->markNumber)-1;i++)  {
          lmNext=(lpWorkBuffer->mark[i+1]).l_m;
          if(lmNext<0) continue;
          rm=(lpWorkBuffer->mark[i]).r_m;
          if(rm<0) continue;
          k=lmNext-rm;
          if(k>0) {
                sr+=k;
                cSp++;
                }
          }
       if(cSp>4) {
        srT=(int)((float)sr/(float)cSp+0.5);
        }
       else return(-1);

       cSp=0;
       sr=0L;
       for(i=0;i<(lpWorkBuffer->markNumber)-1;i++)  {
          lmNext=(lpWorkBuffer->mark[i+1]).l_m;
          if(lmNext<0) continue;
          rm=(lpWorkBuffer->mark[i]).r_m;
          if(rm<0) continue;
          k=lmNext-rm;
          if((k>tt)&&(k<=srT)) {
                sr+=k;
                cSp++;
                }
          }
       if(cSp>0) {
        srS=(int)((float)sr/(float)cSp+0.5);
        }
       else return(-1);

       cSp=0;
       sr=0L;
       for(i=0;i<(lpWorkBuffer->markNumber)-1;i++)  {
          lmNext=(lpWorkBuffer->mark[i+1]).l_m;
          if(lmNext<0) continue;
          rm=(lpWorkBuffer->mark[i]).r_m;
          if(rm<0) continue;
          k=lmNext-rm;
          if(k>srT) {
                sr+=k;
                cSp++;
                }
          }
       if(cSp>0) {
        srB=(int)((float)sr/(float)cSp+0.5);
        }
       else srB=srS;

        if((srB>8)&&(srB>3*srS) ) {
             if((kindD < 4) && (srB <= 4*srS) ) {
                 (lpWorkBuffer->OCRSpace)=3*srT;
                 }
              else {
                 (lpWorkBuffer->OCRSpace)=(srS+3*srB)/4;
                 }
             }
        else {
             (lpWorkBuffer->OCRSpace)=3*srB;
             }
       (lpWorkBuffer->monoSpace)=(srS+3*srT)/4;

if(kindD <= 3) {
       cSp=0;
       sr=0L;
       for(i=0;i<(lpWorkBuffer->markNumber)-1;i++)  {
          lmNext=(lpWorkBuffer->mark[i]).l_m;
          if(lmNext<0) continue;
          rm=(lpWorkBuffer->mark[i]).r_m;
          if(rm<0) continue;
          k=rm - lmNext;
          if(k>0) {
                sr+=k;
                cSp++;
                }
          }
       if(cSp>4) {
        srB=(int)((float)sr/(float)cSp+0.5);
        }
        if(srT <= 12) {
             (lpWorkBuffer->OCRSpace)  = srB -1;
             (lpWorkBuffer->monoSpace) = srB -1;
             }
        else {
             (lpWorkBuffer->OCRSpace)=(int)((float)srB*(float)1.2);
             (lpWorkBuffer->monoSpace)=(int)((float)srB*(float)1.2);
             }
       }
       return(0);
} /* of isMonoSpace() */
/***************************************************************************
 *  PURPOSE      :divide big frame                                         *
 ***************************************************************************/
int divideFr(int scal,STRUCTWB *lpWorkBuffer, int kindD, int divCh){
  int i, indMM, j, xn, yn, i_d, flgI;
  int crL, crLL, crLC, crR, lTrR, lTrL;
  int pom_trace[4], ind_tr[4];
  int ind_tr_p;
      flgI=1;
      crLL=crL=(lpWorkBuffer->curMark1);
      crR=(lpWorkBuffer->curMark2);
      indMM=(lpWorkBuffer->indMaxMark);
      lTrR=(lpWorkBuffer->lengthTraceR);
      lTrL=(lpWorkBuffer->lengthTraceL);
      i=(lpWorkBuffer->indCurMaxMark);
      while( (i<indMM)&&((lpWorkBuffer->maxMark[i])<((lpWorkBuffer->traceR[lTrR])-4)) ) {
            if( (lpWorkBuffer->maxMark[i])<((lpWorkBuffer->traceL[lTrL])+4+(lpWorkBuffer->boundLeft)) ) {
                     i++;
//                     (lpWorkBuffer->indCurMaxMark)++;
                     continue;
                     }

              crL=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->maxMark[i]));

              if(crL<0) {
//ReleaseDC(hWnd,hDCC);
                return(-1);
                }
              (lpWorkBuffer->curMark1)=crL;
              yn=(lpWorkBuffer->mark[crL]).d_m;
              xn=(lpWorkBuffer->maxMark[i]);
              (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crL]).g_m;
              (lpWorkBuffer->traceC[1])=xn;
              (lpWorkBuffer->lengthTraceC)=1;
              if(!searchTrMax(scal,lpWorkBuffer,xn,yn)) {

AA:
                invTraceMax(lpWorkBuffer);
/*-------------------------------------------------------*/
/*    checking if current segment has size too big       */
/*-------------------------------------------------------*/
                i_d=1;
                crLC=(lpWorkBuffer->curMark1);
                if(isBigSegm(lpWorkBuffer)) {
                         (lpWorkBuffer->flgTraceR)=1;
                         for(j=0;j<=(lpWorkBuffer->lengthTraceC);j++) {
                            (lpWorkBuffer->traceR[j])=(lpWorkBuffer->traceC[j]);
                            }
                         (lpWorkBuffer->lengthTraceR)=(lpWorkBuffer->lengthTraceC);
                         (lpWorkBuffer->curMark1)=crLL;
                         i_d=divideMin(scal,lpWorkBuffer);
                if(isBigSegm(lpWorkBuffer)&&divCh) {
                         for(j=0;j<=(lpWorkBuffer->lengthTraceC);j++) {
                            (lpWorkBuffer->traceR[j])=(lpWorkBuffer->traceC[j]);
                            }
                         (lpWorkBuffer->lengthTraceR)=(lpWorkBuffer->lengthTraceC);
                         (lpWorkBuffer->curMark1)=crLL;
                         i_d=strongDiv(scal,lpWorkBuffer,kindD);
                                }

                         }
                if( (flgI=sendCh(scal,lpWorkBuffer,kindD))==-100) return(-100);
                (lpWorkBuffer->indCurMaxMark)=i;
                if(i_d&&((lpWorkBuffer->newSegm)==0)) {
                        (lpWorkBuffer->curMark1)=crLC;
                        (lpWorkBuffer->flgCut)=1;
                        }
                 return(flgI);
                 }
              else {
/* walking from bottom to top is not successful */
                   if((lpWorkBuffer->lengthTraceC)<5) {goto BB;}
                   ind_tr_p=(lpWorkBuffer->lengthTraceC);
                   for(i_d=0;i_d<3;i_d++) {
                      ind_tr[i_d]=(int)(((float)((lpWorkBuffer->lengthTraceC)-1)*(float)(3-i_d))/4.0);
                      if(ind_tr[i_d]<2) {ind_tr[i_d]=2;}
                      pom_trace[i_d]=(lpWorkBuffer->traceC[ind_tr[i_d]]);
                      }

                   for(i_d=0;i_d<3;i_d++) {
                      (lpWorkBuffer->lengthTraceC)=ind_tr[i_d];
                      yn=(lpWorkBuffer->mark[crL]).d_m-(lpWorkBuffer->lengthTraceC);
                      xn=pom_trace[i_d];
                      while( !whatIsThis(lpWorkBuffer,++xn,yn)&&(xn<((lpWorkBuffer->maxMark[i])+30))&&(xn<((lpWorkBuffer->traceR[max((lpWorkBuffer->lengthTraceR)-(lpWorkBuffer->lengthTraceC)+2,1)])-4)) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                           if(searchTrMax(scal,lpWorkBuffer,xn,yn)) {
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                           else {
                              goto AA;
                              }
                           }
                        }/* while ++x */
                      xn=pom_trace[i_d];

                      while( !whatIsThis(lpWorkBuffer,--xn,yn)&&(xn>((lpWorkBuffer->maxMark[i])-30))&&(xn>((lpWorkBuffer->traceL[max((lpWorkBuffer->lengthTraceL)-(lpWorkBuffer->lengthTraceC)+2,1)])+6+(lpWorkBuffer->boundLeft))) ) {
                        if( (xn%2)==(i_d/2) ) { /* just save time */
                          if(searchTrMax(scal,lpWorkBuffer,xn,yn)) {
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        }/* while --x */

                     }/* for(i_d */

BB:              i++;
                 }
              }/* while */

             for(j=0;j<=(lpWorkBuffer->lengthTraceR);j++) {
                     (lpWorkBuffer->traceC[j])=(lpWorkBuffer->traceR[j]);
                     }
             (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceR);
/*-------------------------------------------------------*/
/*    checking if current segment has size too big       */
/*-------------------------------------------------------*/
             i_d=1;
             (lpWorkBuffer->curMark1)=crLL;
             if(isBigSegm(lpWorkBuffer)||(divCh==0)) {
                      i_d=divideMin(scal,lpWorkBuffer);
                      }
                if(isBigSegm(lpWorkBuffer)&&divCh) {
                         if(i_d==0) {
                           for(j=0;j<=(lpWorkBuffer->lengthTraceC);j++) {
                              (lpWorkBuffer->traceR[j])=(lpWorkBuffer->traceC[j]);
                              }
                           (lpWorkBuffer->lengthTraceR)=(lpWorkBuffer->lengthTraceC);
                           (lpWorkBuffer->curMark1)=crLL;
                           (lpWorkBuffer->flgTraceR)=1;
                           }
                         i_d=strongDiv(scal,lpWorkBuffer,kindD);

                         }

             if( (flgI=sendCh(scal,lpWorkBuffer,kindD))==-100) return(-100);
//        printf("flgI = %d, lpWorkBuffer->flgTraceR= %d, i_d= %d\n", flgI, lpWorkBuffer->flgTraceR,i_d);

        (lpWorkBuffer->curMark1)=(lpWorkBuffer->curMark2)+1;
        (lpWorkBuffer->newSegm)=1;
// ????????????????? bug????

             if( i_d ) {
                (lpWorkBuffer->flgCut)=1;
               if((lpWorkBuffer->flgTraceR)) {
                     }
               else {
                   (lpWorkBuffer->curMark1)=(lpWorkBuffer->curMark2)+1;
                   (lpWorkBuffer->newSegm)=1;
                   }
               }
             (lpWorkBuffer->indCurMaxMark)=max(0,i-1);
             return(flgI);
} /* of divideFr() */

/***************************************************************************
 *  PURPOSE      :divide big frame with respect minimas                    *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                0 - dividing was done                                    *
 *                1 - dividing was not done                                *
 ***************************************************************************/
int divideMin(int scal,STRUCTWB  *lpWorkBuffer){
  int i, indMM, xn, yn, i_d;
  int crL, crLL, crR;
  int pom_trace[4], ind_tr[4];
  int ind_tr_p;
      crLL=crL=(lpWorkBuffer->curMark1);
      crR=(lpWorkBuffer->curMark2);
      indMM=(lpWorkBuffer->indMinMark);

      i=(lpWorkBuffer->indCurMinMark);
      while( (i<indMM)&&((lpWorkBuffer->minMark[i])<((lpWorkBuffer->traceR[1])-4)) ) {
            if( (lpWorkBuffer->minMark[i])<((lpWorkBuffer->traceL[1])+4+(lpWorkBuffer->boundLeft)) ) {
                     i++;
                     continue;
                     }
              crL=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->minMark[i]));
              if(crL<0) {
                return(-1);
                }
              (lpWorkBuffer->curMark1)=crL;
              yn=(lpWorkBuffer->mark[crL]).g_m;
              xn=(lpWorkBuffer->minMark[i]);
              (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crL]).g_m;
              (lpWorkBuffer->traceC[1])=xn;
              (lpWorkBuffer->lengthTraceC)=1;
              if(!searchTrMin(scal,lpWorkBuffer,xn,yn)) {
AA:
                (lpWorkBuffer->indCurMinMark)=i;
                return(0);
                }
              else {
                   if((lpWorkBuffer->lengthTraceC)<5) {goto BB;}

                   ind_tr_p=(lpWorkBuffer->lengthTraceC);
                   for(i_d=0;i_d<3;i_d++) {
                      ind_tr[i_d]=(int)(((float)((lpWorkBuffer->lengthTraceC)-1)*(float)(3-i_d))/4.0);
                      if(ind_tr[i_d]<2) {ind_tr[i_d]=2;}
                      pom_trace[i_d]=(lpWorkBuffer->traceC[ind_tr[i_d]]);
                      }

                   for(i_d=0;i_d<3;i_d++) {
                      (lpWorkBuffer->lengthTraceC)=ind_tr[i_d];
                      yn=(lpWorkBuffer->mark[crL]).g_m+(lpWorkBuffer->lengthTraceC);
                      xn=pom_trace[i_d];
                      while( !whatIsThis(lpWorkBuffer,--xn,yn)&&(xn>((lpWorkBuffer->minMark[i])-30))&&(xn>((lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceC)])+4+(lpWorkBuffer->boundLeft))) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                          if(searchTrMin(scal,lpWorkBuffer,xn,yn)) {
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        } /* while --x */

                      xn=pom_trace[i_d];
                      while( !whatIsThis(lpWorkBuffer,++xn,yn)&&(xn<((lpWorkBuffer->minMark[i])+30))&&(xn<((lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceC)])-5)) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                          if(searchTrMin(scal,lpWorkBuffer,xn,yn)) {
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        } /* while ++x */
                     }/* for(i_d */

BB:              i++;
                 }
              }/* while */

              for(i_d=0;i_d<=(lpWorkBuffer->lengthTraceR);i_d++) {
                  (lpWorkBuffer->traceC[i_d])=(lpWorkBuffer->traceR[i_d]);
                  }
              (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceR);

             (lpWorkBuffer->curMark1)=crLL;
             (lpWorkBuffer->indCurMinMark)=i-1;
              return(1);
} /* of divideMin() */

int strongDiv(int scal, STRUCTWB *lpWorkBuffer, int kindD)
{
  int Ok, begMin, begMax, crL, oldMin, oldMax, whatM, i, ii;
  int lm, rm, thT;
      thT=(6*(lpWorkBuffer->Dpi).x+4)/200;
      crL=(lpWorkBuffer->curMark1);
      Ok=firstIndex(lpWorkBuffer,(int *)&begMin,(int *)&begMax);
      if(Ok<0) return(Ok);
      ii=1;
      do {
         oldMax=begMax;
         oldMin=begMin;
         whatM=nextIndex(lpWorkBuffer,(int *)&begMin,(int *)&begMax,kindD);
         if(whatM<0) {
                for(i=0;i<=(lpWorkBuffer->lengthTraceR);i++) {
                    (lpWorkBuffer->traceC[i])=(lpWorkBuffer->traceR[i]);
                    }
                (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceR);
                (lpWorkBuffer->curMark1)=crL;
                if(kindD==6) {
                  lm=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]))/2;
                  rm=((lpWorkBuffer->traceR[1])+(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]))/2;
                  if( (rm-lm)>(lpWorkBuffer->thWid) )  {
                        if(testOCRCp1(lpWorkBuffer)==1) return(0);
                        if( (rm-lm)>(int)((float)(lpWorkBuffer->thWid)*(float)1.5) )  {
                                i=(rm+lm)/2;
                                if(testOCRCp2(ii,lpWorkBuffer,max(lm+4,i-thT),min(i+thT,rm-4))==1) return(0);
                                }
                        }
                  }
                return(whatM);
                }
              if(kindD==6) {
                lm=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]))/2;
                if(whatM==0) {
                        rm=(lpWorkBuffer->maxMark[oldMax]);
                        }
                else {
                        rm=(lpWorkBuffer->minMark[oldMin]);
                        }
                if( (rm-lm)>(lpWorkBuffer->thWid) )  {
                        if(testOCRCp1(lpWorkBuffer)==1) return(0);
                        if( (rm-lm)>(int)((float)(lpWorkBuffer->thWid)*(float)1.5) )  {
                                i=(rm+lm)/2;
                                if(testOCRCp2(ii,lpWorkBuffer,max(lm+4,i-thT),min(i+thT,rm-4))==1) return(0);
                                }
                        }
                 }
         indTopBott=0;
         indBottTop=0;
         switch(whatM) {
                case 0:

                        Ok=traceBtTp(scal,lpWorkBuffer,oldMax);
                        if(Ok==0) return(0);
                        switch(kindD) {
                             case 6:case 5:case 4: case 7:
                                Ok=cutOCRCpL(scal,lpWorkBuffer,oldMax,0,kindD,ii);
                                break;
                             default:
                                Ok=cutDec(scal,lpWorkBuffer,oldMax,0,ii);
                                break;
                             }
                        if(Ok) return(0);
                        break;
                case 1:

                        Ok=traceTpBt(scal,lpWorkBuffer,oldMin);
                        if(Ok==0)  {
                                 return(0);
                                 }

                        switch(kindD) {
                             case 6:case 5:case 4: case 7:
                                Ok=cutOCRCpL(scal,lpWorkBuffer,0,oldMin,kindD,ii);
                                break;
                             default:
                                Ok=cutDec(scal,lpWorkBuffer,0,oldMin,ii);
                                break;
                             }
                        if(Ok) {
                                return(0);
                                }
                        break;
                case 2:
                        Ok=traceBtTp(scal,lpWorkBuffer,oldMax);
                        if(Ok==0) return(0);
                        Ok=traceTpBt(scal,lpWorkBuffer,oldMin);
                        if(Ok==0) return(0);
                        Ok=cutDec(scal,lpWorkBuffer,oldMax,oldMin,ii);
                        if(Ok) return(0);
                        break;
                }
                ii++;
         } while(1);
      return(1);
   }/* of strongDiv() */
/***************************************************************************
 *                                                                         *
 *  PURPOSE      :first Max and Min indexes for current segment            *
 ***************************************************************************/
int  firstIndex(STRUCTWB *lpWorkBuffer, int *lpkMin, int *lpkMax){
  int i, isMax, isMin, indMM;
  int lTrL, lTrR;

     i=0;
     isMax=0;
     indMM=(lpWorkBuffer->indMaxMark);
     lTrR=(lpWorkBuffer->lengthTraceR);
     lTrL=(lpWorkBuffer->lengthTraceL);

     while( (!isMax)&&(i<indMM)&&((lpWorkBuffer->maxMark[i])<((lpWorkBuffer->traceR[lTrR])-4)) ) {
            if( (lpWorkBuffer->maxMark[i])>((lpWorkBuffer->traceL[lTrL])+4+(lpWorkBuffer->boundLeft)) ) {
                 isMax=1;
                 }
            else  {
                 i++;
                 }
            }
     *lpkMax=i;

     i=0;
     isMin=0;
     indMM=(lpWorkBuffer->indMinMark);

     while( (!isMin)&&(i<indMM)&&((lpWorkBuffer->minMark[i])<((lpWorkBuffer->traceR[1])-4)) ) {
            if( (lpWorkBuffer->minMark[i])>((lpWorkBuffer->traceL[1])+4+(lpWorkBuffer->boundLeft)) ) {
                 isMin=1;
                 }
            else  {
                 i++;
                 }
            }
      *lpkMin=i;
      if((isMax==0)&&(isMin==0)) return(-1);
      return(0);

} /* of firstIndex() */

/***************************************************************************
 *  PURPOSE      :next Max and/or Min indexes for current segment          *
 *                                                                         *
 ***************************************************************************/
int  nextIndex(STRUCTWB *lpWorkBuffer, int *lpkMin, int *lpkMax, int kindD){
  int  isMax, isMin, indMM, kMax, kMin;
  int  lTrL, lTrR;

     kMax=*lpkMax;
     isMax=0;
     indMM=(lpWorkBuffer->indMaxMark);
     lTrR=(lpWorkBuffer->lengthTraceR);
     lTrL=(lpWorkBuffer->lengthTraceL);
     if( (kMax<indMM)&&((lpWorkBuffer->maxMark[kMax])<((lpWorkBuffer->traceR[lTrR])-4)) ) {
        isMax=1;
        }
     kMin=*lpkMin;
     isMin=0;
     indMM=(lpWorkBuffer->indMinMark);
     if( (kMin<indMM)&&((lpWorkBuffer->minMark[kMin])<((lpWorkBuffer->traceR[1])-4)) ) {
        isMin=1;
        }
     if( (!isMax)&&(!isMin) ) {
        return(-1);
        }
     if( isMax&&(!isMin) )    {
        *lpkMax=kMax+1;
        return(0);
        }
     if( (!isMax)&&isMin )    {
        *lpkMin=kMin+1;
        return(1);
        }
     if(kindD<4) {
        indMM=(lpWorkBuffer->minMark[kMin])-(lpWorkBuffer->maxMark[kMax]);
        if(indMM<0) indMM=-indMM;
        if( indMM<20 )  {
           if( (lpWorkBuffer->maxMark[kMax])<(lpWorkBuffer->minMark[kMin]) ) {
              *lpkMax=kMax+1;
              }
           else {
              *lpkMin=kMin+1;
              }
           return(2);
           }
        }
     if( (lpWorkBuffer->maxMark[kMax])<(lpWorkBuffer->minMark[kMin]) ) {
        *lpkMax=kMax+1;
        return(0);
        }
     else {
        *lpkMin=kMin+1;
        return(1);
        }
} /* of nextIndex() */

int testW(STRUCTWB *lpWorkBuffer){
 int halfTr, i,  minH, x, y, dL, cntL, dR, cntR, flgB;
 int part, xOld, flgF;

     halfTr=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceR[1]))/2-4;
     minH=(lpWorkBuffer->lengthTraceL)-1;
     y=(lpWorkBuffer->traceL[0]);
     cntL=0;
     dL=0;
     flgF=0;
     xOld=0;
     for(i=2;i<minH;i++)  {
        flgB=0;
        for(x=(lpWorkBuffer->traceL[i])+2;x<halfTr;x++)  {
                if(flgB==0) {
                   if(whatIsThis(lpWorkBuffer,x,y)) {
                       if(flgF) {
                           part=xOld-x;
                           if(part<0) part=-part;
                           if(part>dL) dL=part;
                           }
                       xOld=x;
                       flgF=1;
                       flgB=1;
                       cntL++;
                       }
                   }
                }
        y++;
        }
     if(minH>4) {
         part=(int)(((float)dL/(float)minH)*(float)1000);
         }
     else { return(0);}
     if(part>200) return(0);

     part=(int)(((float)cntL/(float)minH)*(float)1000);
     if(part<700) return(0);
     halfTr+=8;
     minH=(lpWorkBuffer->lengthTraceR)-1;
     y=(lpWorkBuffer->traceR[0]);
     cntR=0;
     dR=0;
     flgF=0;
     xOld=0;
     for(i=2;i<minH;i++)  {
        flgB=0;
        for(x=(lpWorkBuffer->traceR[i])-2;x>halfTr;x--)  {
                if(flgB==0) {
                   if(whatIsThis(lpWorkBuffer,x,y)) {
                       if(flgF) {
                           part=xOld-x;
                           if(part<0) part=-part;
                           if(part>dR) dR=part;
                           }
                       xOld=x;
                       flgF=1;
                       flgB=1;
                       cntR++;
                       }
                  }
                }
        y++;
        }
     if(minH>4) {
         part=(int)(((float)dR/(float)minH)*(float)1000);
         }
     else { return(0);}
     if(part>200) return(0);

     part=(int)(((float)cntR/(float)minH)*(float)1000);
     if(part<700) return(0);
       return(1);
} /*testW() */

int maxYmark(int xx, int *prevI, STRUCTWB *lpWorkBuffer)
 {
   int i, max, imax;
   int lm, rm, fr;
       fr=0;
    if(lpWorkBuffer -> txtLineVer == 65) {
       if(lpWorkBuffer -> txtLineHight == 0)
        return((lpWorkBuffer->curLine[fr]).y2 - 1);
       else
        return((lpWorkBuffer->curLine[fr]).y2);
    }
       if((lpWorkBuffer->frNumber)>0) {
           fr=whichFrame(lpWorkBuffer,xx);
           if(fr<0) {fr=0;}
           if(fr>=(lpWorkBuffer->frNumber)) {fr=(lpWorkBuffer->frNumber)-1;}
           }
       if((lpWorkBuffer ->markNumber)<=0) {
        return((lpWorkBuffer->curLine[fr]).y2);
        }
       max=0;
       imax=-1;
       for(i=*prevI;i<(lpWorkBuffer ->markNumber);i++) {
          lm=(lpWorkBuffer-> mark[i]).l_m;
          rm=(lpWorkBuffer-> mark[i]).r_m;

          if(abs(lm-xx)<(lpWorkBuffer->  lettWidth)) {
                if((lpWorkBuffer-> mark[i]).d_m>max) {
                    max=(lpWorkBuffer-> mark[i]).d_m;
                    imax=i;
                    }
                }
          if(abs(rm-xx)<(lpWorkBuffer->  lettWidth)) {
                if((lpWorkBuffer-> mark[i]).d_m>max) {
                    max=(lpWorkBuffer-> mark[i]).d_m;
                    imax=i;
                    }
                }

          }
       if(imax<0) {
          max=(lpWorkBuffer-> mark[(*prevI)]).d_m;
          imax=*prevI;
          }
       max=min(max+1,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-2);
       *prevI=imax;

       if(max > (lpWorkBuffer->curLine[fr]).y2)
               max = (lpWorkBuffer->curLine[fr]).y2;
       return(max);

 } /* of maxYmark() */

void adjY(int *lpTr, int * lpLen, int add)
{
 int i, vLn;
   vLn=*lpLen;
   for(i=vLn;i>0;i--) {
      if(i+add < MAXLTRACE)
         lpTr[i+add]=lpTr[i];
        }
   if(add < MAXLTRACE-1)
    for(i=1;i<=add;i++) {
        lpTr[i]=lpTr[add+1];
        }
   lpTr[0]=lpTr[0]-add;
   lpLen[0]=vLn+add;

} /* adjY() */

void adjL(int *lpTr, int *lpLen, int add)
{
 int i, vLn;
   vLn=*lpLen;
   for(i=vLn+1;i<=(vLn+add);i++) {
        if((i < MAXLTRACE) && (vLn < MAXLTRACE))
         lpTr[i]=lpTr[vLn];
        }
   lpLen[0]=vLn+add;

} /* adjL() */
//-----------------------------------------------
int minYmark(int xx, int *prevI, STRUCTWB *lpWorkBuffer){
   int i, min, imin;
   int lm, rm, fr;
       fr=0;
     if(lpWorkBuffer -> txtLineVer == 65)
        return((lpWorkBuffer->curLine[fr]).y1);

       if((lpWorkBuffer->frNumber)>0) {
           fr=whichFrame(lpWorkBuffer,xx);
           if(fr<0) {fr=0;}
           if(fr>=(lpWorkBuffer->frNumber)) {fr=(lpWorkBuffer->frNumber)-1;}
           }
       if((lpWorkBuffer ->markNumber)<=0) {
        return((lpWorkBuffer->curLine[fr]).y1);
        }
       min=32000;
       imin=-1;
       for(i=*prevI;i<(lpWorkBuffer ->markNumber);i++) {
          lm=(lpWorkBuffer-> mark[i]).l_m;
          rm=(lpWorkBuffer-> mark[i]).r_m;
          if(lm<0) lm=-lm;
          if(rm<0) rm=-rm;
          if(abs(lm-xx)<(lpWorkBuffer->  lettWidth)) {
                if((lpWorkBuffer-> mark[i]).g_m<min) {
                    min=(lpWorkBuffer-> mark[i]).g_m;
                    imin=i;
                    }
                }
          if(abs(rm-xx)<(lpWorkBuffer->  lettWidth)) {
                if((lpWorkBuffer-> mark[i]).g_m<min) {
                    min=(lpWorkBuffer-> mark[i]).g_m;
                    imin=i;
                    }
                }

          }
       if(imin<0) {
          min=(lpWorkBuffer-> mark[(*prevI)]).g_m;
          imin=*prevI;
          }
       min=max(min-1,(lpWorkBuffer -> begRect.y));
       *prevI=imin;
       if(min < (lpWorkBuffer->curLine[fr]).y1)
               min = (lpWorkBuffer->curLine[fr]).y1;
       return(min);
 } /* of minYmark() */
//--------------------------------------------------------------
int insideBox(STRUCTWB *lpWorkBuffer, int kindD){
  int i, j, jj, cc, y, len, mid, dl1, dl2;
  int xBox, xBox1, xBox2, xEps1, xEps2, tH, tH1, tHB, tH2x;
  int minY, maxY, maxH, bottY, upY, Eps, black, brk;
  int crT, wCur, wNext, wImg, minTrC, maxTrC;
  int tD1, tD2, tD11, maxC, tHind;
      tH2x=(int)((float)6*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
      tD1=(int)((float)10*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
      tD11=(int)((float)15*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
      tD2=(int)((float)9*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
      tHB=min(tD11,7);
      (lpWorkBuffer->trush)=0;
      wImg=(lpWorkBuffer->widthImg)*8-2;
      mid=(lpWorkBuffer->traceL[1])+(lpWorkBuffer->lettWidth)/2;
      xEps1=(lpWorkBuffer->lettWidth)/tD11;
      xEps2=(lpWorkBuffer->lettWidth)/tD2;
      xEps1=max(xEps1,2);
      xEps2=max(xEps2,3);
      tH1=(lpWorkBuffer->lettWidth)/3;
      xBox=(lpWorkBuffer->numbOfChar);
      i=0;j=1;
      if(((lpWorkBuffer ->CellW)>0)) {
        while((mid>xBox)&&j) {
          if(i<(lpWorkBuffer->lenLeftC)) {
            xBox=lpWorkBuffer->arrCellP[i].rT.x+2;
            i++;
            }
          else {
            j=0;
            }
          }
        }
      else {
        j=0;
        }
      if(!j) {
         xBox=(lpWorkBuffer->numbOfChar);
         while(mid>xBox ) {
           xBox+=(lpWorkBuffer->lettWidth);
           }
         }
      xBox=min(xBox,wImg);
      (lpWorkBuffer->numbOfChar)=xBox;
      xBox1=xBox-xEps1;
      xBox2=xBox-xEps2;

      len=(lpWorkBuffer->lengthTraceC);
      wCur=(lpWorkBuffer->traceC[1])-(lpWorkBuffer->traceL[1]);
      wCur+=((lpWorkBuffer->traceC[len])-(lpWorkBuffer->traceL[len]));
      wCur/=2;
      wNext=-100;
      cc=(lpWorkBuffer->mark[(lpWorkBuffer->curMark2)]).r_m;
      if(cc<0) cc=-cc;
      wNext=cc-(lpWorkBuffer->traceR[1]);
      if( (wNext<=0)&&((lpWorkBuffer->curMark2)+1)<(lpWorkBuffer->markNumber)) {
         cc=(lpWorkBuffer->curMark2)+1;
         while( ((lpWorkBuffer->mark[cc]).r_m<0)&&(cc<(lpWorkBuffer->markNumber))) cc++;
         if((lpWorkBuffer->mark[cc]).r_m>0) {
             wNext=(lpWorkBuffer->mark[cc]).r_m-(lpWorkBuffer->traceR[1]);
             }
         }

      if( wCur>((lpWorkBuffer->lettWidth)+4*xEps2) ) {
        len=max(len,(lpWorkBuffer->lengthTraceL));
        len=max(len,(lpWorkBuffer->lengthTraceR));
        for(i=1;i<=len;i++) {
            (lpWorkBuffer->traceC[i])=xBox;
            }
        (lpWorkBuffer->traceC[0])=min((lpWorkBuffer->traceC[0]),(lpWorkBuffer->traceL[0]));
        (lpWorkBuffer->traceC[0])=min((lpWorkBuffer->traceC[0]),(lpWorkBuffer->traceR[0]));
        (lpWorkBuffer->flgCut)=1;
        (lpWorkBuffer->flgTraceR)=1;
        return(-50);
        }
      if( (wCur>2*tH1)&&(wNext>2*tH1) ) return(1);
      cc=0;maxC=0;
      for(i=1;i<len;i++) {
          if((lpWorkBuffer->traceC[i])<xBox1) cc++;
          if((lpWorkBuffer->traceC[i])>maxC) maxC=(lpWorkBuffer->traceC[i]);
          }

      mid=0;
      for(i=1;i<len;i++) {
          if((lpWorkBuffer->traceC[i])<xBox2) mid++;
          }
      tH=6;
      tHind=6;
      if(mid>4) {tH=12;tHind=12;}
      tH=min(tH,xBox-maxC-1);

        maxTrC=0;
        minTrC=wImg;
        for(i=1;i<=len;i++) {
            if((lpWorkBuffer->traceC[i])>maxTrC) maxTrC=(lpWorkBuffer->traceC[i]);
            if((lpWorkBuffer->traceC[i])<minTrC) minTrC=(lpWorkBuffer->traceC[i]);
            }
      if( (cc>4)&&(maxTrC+tD1<wImg)&&(minTrC-tD1>0)) {
        cc=0;
        for(i=1;i<len;i++) {
           y=(lpWorkBuffer->traceC[0])+i-1;
           dl1=100;
           j=1;
           while( j<tD1 ) {
               jj=(lpWorkBuffer->traceC[i])+j;
               if(jj>=xBox) break;
               if(whatIsThis(lpWorkBuffer,jj,y)) {
                   dl1=j;
                   j=tD1;
                   }
               j++;
               }/* j */
           dl2=100;
           j=1;
           while( j<tD1 ) {
               jj=(lpWorkBuffer->traceC[i])-j;
               if(whatIsThis(lpWorkBuffer,jj,y)) {
                   dl2=j;
                   j=tD1;
                   }
                j++;
               }/* j */
           if( ((dl1+dl2)<tH)&&((lpWorkBuffer->traceC[i])<xBox1) ) cc++;

           }/* i */
        if( (cc>2)||((len<15)&&(cc>0))) {
                return(0);
                }
        tHB=min(tHB,xBox-maxC-1);
        if((tHind==12)&&(maxTrC+tD1<wImg)&&(minTrC-tD1>0)) {
               cc=0;
               minY=(lpWorkBuffer->curLine[(lpWorkBuffer->indTopCurLine)]).y1;
               maxY=(lpWorkBuffer->curLine[(lpWorkBuffer->indTopCurLine)]).y2;

               if((lpWorkBuffer->traceC[0])-minY>tD1) {
                  minY=-tD1;
                  if((lpWorkBuffer->traceC[0])-tD1-1<0) minY=1;
                  }
               else {
                  minY=1;
                  }
               if(maxY-((lpWorkBuffer->traceC[0])+len-1)>tD1) {
                  maxY=len+tD1;
                  if((lpWorkBuffer->traceC[0])+maxY-1>(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)) maxY=len;
                  }
               else {
                  maxY=len;
                  }

               for(i=minY;i<maxY;i++) {
                  y=(lpWorkBuffer->traceC[0])+i-1;
                  for(j=1;j<tHB;j++) {
                     if(i<1)  {
                        jj=(lpWorkBuffer->traceC[1])+j;
                        }
                     else {
                        if(i>=len) {
                            jj=(lpWorkBuffer->traceC[len-1])+j;
                            }
                        else {
                            jj=(lpWorkBuffer->traceC[i])+j;
                            }
                        }
                     if(whatIsThis(lpWorkBuffer,jj,y)) {
                       if((lpWorkBuffer->traceC[i])<xBox2) cc++;
                       }
                     }/* j */
                  }/* i */
               if( (cc>2)||((len<15)&&(cc>0))) {
                 return(0);
                 }
               mid=0;dl2=0;
               }
        else {
            mid=0;
            dl1=dl2=0;
            if((maxTrC+tD1<wImg)&&(minTrC-tD1>0)) {
              for(i=1;i<len;i++) {
                y=(lpWorkBuffer->traceC[0])+i-1;
                cc=0;
                for(j=1;j<6;j++) {
                   jj=(lpWorkBuffer->traceC[i])+j;
                   if(whatIsThis(lpWorkBuffer,jj,y)) {
                     cc++;
                     }
                   }/* j */
                if(dl1<cc) {
                        dl1=cc;
                        dl2=i;
                        }
                mid+=cc;
                }/* i */
              }
           }
           if(mid>1) {
                (lpWorkBuffer->trush)=1;
                }
        }
      else {
            mid=0;
            dl1=dl2=0;
            if((maxTrC+tD1<wImg)&&(minTrC-tD1>0)) {
              for(i=1;i<len;i++) {
                y=(lpWorkBuffer->traceC[0])+i-1;
                cc=0;
                for(j=1;j<6;j++) {
                   jj=(lpWorkBuffer->traceC[i])+j;
                   if(whatIsThis(lpWorkBuffer,jj,y)) {
                     cc++;
                     }
                   }/* j */
                if(dl1<cc) {
                        dl1=cc;
                        dl2=i;
                        }
                mid+=cc;
                  }/* i */
                }
           if(mid>1) {
                (lpWorkBuffer->trush)=1;
                }

             }
        if((mid>2)&&(maxTrC+tD1<wImg)&&(minTrC-tD1>0)) {
                cc=0;
                for(i=max(1,dl2-4);i<min(len,dl2+20);i++)  {
                   y=(lpWorkBuffer->traceC[0])+i-1;
                   for(j=1;j<5;j++) {
                      jj=(lpWorkBuffer->traceC[i])-j;
                      if(whatIsThis(lpWorkBuffer,jj,y)) {
                         cc++;
                         }
                      }/* j */
                   }/* i */
               if(cc>2)  {
                j=1;
                dl1=0;
                while( (j<6)&&(!dl1) ) {
                    for(i=max(1,dl2-2);i<min(len,dl2+3);i++)  {
                       y=(lpWorkBuffer->traceC[0])+i-1;
                       jj=(lpWorkBuffer->traceC[i])+j;
                       if(whatIsThis(lpWorkBuffer,jj,y)) {
                          dl1=j;
                          dl2=i;
                          }
                       }
                    j++;
                    }
                if(dl1>0) {
                       j=dl1;
                       xEps2*=4;
                       brk=0;
                       Eps=1;
                       maxH=0;
                       while( (j<xEps2)&&(brk<3) ) {
                        upY=1000;
                        i=dl2;
                        black=1;
                        while( (i>(-4))&&black ) {
                            y=(lpWorkBuffer->traceC[0])+i-1;
                            if(y<=(lpWorkBuffer -> begRect.y)) black=0;
                            if(i<1) {
                                jj=(lpWorkBuffer->traceC[1])+j;
                                }
                            else {
                                jj=(lpWorkBuffer->traceC[i])+j;
                                }
                            if(whatIsThis(lpWorkBuffer,jj,y)) {
                                upY=i;
                                }
                            else {
                                if(i<(dl2-Eps)) black=0;
                                }
                            i--;
                            }
                        minY=i+1;
                        bottY=1000;
                        i=dl2;
                        black=1;
                        while( (i<len+4)&&black ) {
                            y=(lpWorkBuffer->traceC[0])+i-1;
                            if(y>=(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)) black=0;
                            if(i<len) {
                                jj=(lpWorkBuffer->traceC[i])+j;
                                }
                            else {
                                jj=(lpWorkBuffer->traceC[len])+j;
                                }
                            if(whatIsThis(lpWorkBuffer,jj,y)) {
                                bottY=i;
                                }
                            else {
                                if(i>(dl2+Eps)) black=0;
                                }
                            i++;
                            }
                        maxY=i-1;
                        if( (bottY==1000)&&(upY==1000) )  {
                                brk++;
                                }
                        else    {
                                if(maxH<maxY-minY) maxH=maxY-minY;
                                }
                        if( (bottY!=1000)&&(upY!=1000) )  {
                                 Eps=(bottY-upY)/2+4;
                                 dl2=(bottY+upY)/2;
                                 }
                        else {
                                Eps=4;
                                if(bottY!=1000) dl2=bottY;
                                if(upY!=1000)   dl2=upY;
                                }
                        j++;
                        }
                dl2=j-dl1;
                crT=-1;
                cc=(lpWorkBuffer->mark[(lpWorkBuffer->curMark2)]).r_m;
                if(cc<0) cc=-cc;
                crT=cc-(lpWorkBuffer->traceR[1]);
                if( (crT<=0)&&((lpWorkBuffer->curMark2)+1)<(lpWorkBuffer->markNumber)) {
                    cc=(lpWorkBuffer->curMark2)+1;
                    while( ((lpWorkBuffer->mark[cc]).r_m<0)&&(cc<(lpWorkBuffer->markNumber))) cc++;
                    if((lpWorkBuffer->mark[cc]).r_m>0) {
                       crT=(lpWorkBuffer->mark[cc]).r_m-(lpWorkBuffer->traceR[1]);
                       }
                    }
                if( (maxH<tH1)&&(dl2<tH1)&&(crT<(xEps1*5)) ) {
                        (lpWorkBuffer->trush)=0;
                        return(0);
                       }
                }
               }
              }
        return(1);
} /* insideBox() */
//------------------------------------
int checkI(STRUCTWB * lpWorkBuffer){
  int i, j, cc, y, count_v, c_j, flg_b, t7, t10, t4;
  int len;

           len=(lpWorkBuffer->lengthTraceL);
           t4=(int)((float)len/(float)7.0);
           t7=(int)((float)len/(float)3.4);
           if( t7>7) t7=7;
           t10=(int)((float)len/(float)2.4);
           if( t10>10) t10=10;
           count_v=0;
           if(len<15) return(0);
           for(i=1;i<=len;i++) {
             y=(lpWorkBuffer->traceL[0])+i-1;
             cc=0;flg_b=c_j=0;
             traceI[i]=(lpWorkBuffer->traceL[i]);

             for(j=(lpWorkBuffer->traceL[i]);j<=(lpWorkBuffer->traceC[i]);j++) {
               if(whatIsThis(lpWorkBuffer,j,y)) {
                   if(flg_b==0) {
                        flg_b=1;
                        }
                   cc++;
                   traceI[i]=j;
                   }
                else {
                     flg_b=0;
                     }
               }
            if( cc>t10 ) {
                  return(0);
                  }

            if( ( (cc>0)|| (i<t4) || (i>(len-t4)) ) ) {
                count_v++;
                }
            }
          if( count_v<(len-t7) ) return(0);
          return(1);
   } /* checkI() */
//----------------------------------------
int checkITrush(STRUCTWB  *lpWorkBuffer){
  int i, j, cc, y, count_v, c_j, flg_b, t7, t10, t4;
  int len, massW;
           len=(lpWorkBuffer->lengthTraceL);
           if( len>((((lpWorkBuffer->curLine[(lpWorkBuffer->indTopCurLine)]).y2-(lpWorkBuffer->curLine[(lpWorkBuffer->indTopCurLine)]).y1)*3)/4) ) return(0);
           t4=(int)((float)len/(float)7.0);
           t7=(int)((float)len/(float)3.4);
           if( t7>7) t7=7;
           t10=(int)((float)len/(float)2.4);
           if( t10>10) t10=10;
           count_v=0;
           if(len<15) return(0);
           for(i=1;i<=len;i++) {
             y=(lpWorkBuffer->traceL[0])+i-1;
             cc=0;flg_b=c_j=0;
             massW=0;
             traceI[i]=(lpWorkBuffer->traceL[i]);
             for(j=(lpWorkBuffer->traceL[i]);j<=(lpWorkBuffer->traceC[i]);j++) {
               if(whatIsThis(lpWorkBuffer,j,y)) {
                   if(flg_b==0) {
                        c_j++;
                        if( (c_j>1) && (i>t7) && (i<(len-t7))&&(massW>2) ) {
                              return(0);
                              }
                        flg_b=1;
                        }
                   cc++;
                   traceI[i]=j;
                   }
                else {
                     flg_b=0;
                     if(c_j>0) massW++;
                     }
               }
            if( cc>t10 ) {
                  return(0);
                  }
            if( ( (cc>0)|| (i<t4) || (i>(len-t4)) ) ) {
                count_v++;
                }
            }
          massW=0;
          cc=3;
          j=cc+1;
          for(i=1;i<len-j;i++) {
                c_j=abs(traceI[i]-traceI[i+cc]);
                if(c_j>massW) massW=c_j;
                }
          if( massW>t10 ) return(0);
          if( count_v<(len-t7) ) return(0);
          return(1);
   } /* checkITrush() */
//-----------------------------------------------
int checkIRight(int scal, STRUCTWB  *lpWorkBuffer){
  int i, j, jj, cc, s4, count_v, y, len;
           len=(lpWorkBuffer->lengthTraceL);
           if(len<15) return(0);
             s4=count_v=0;
           for(i=1;i<len;i++) {
             cc=0;
             y=(lpWorkBuffer->traceL[0])+i-1;
             for(j=1;j<6;j++) {
               jj=traceI[i]+j;
               if(whatIsThis(lpWorkBuffer,jj,y)) {
                   cc++;
                   }
                }/* j */
             s4+=cc;
             if(cc>0) count_v++;
            }/* i */
           if( (s4>2)) return(1);
          return(0);
   } /* checkIRight() */
//----------------------------------------------
int checkIOCR(STRUCTWB *lpWorkBuffer){
  int i, j, cc, y, count_v, c_j, flg_b, t7, t10, t4;
  int len;

           len=(lpWorkBuffer->lengthTraceL);

           t4=(int)((float)len/(float)7.0);
           t7=(int)((float)len/(float)3.4);
           if( t7>7) t7=7;
           t10=(int)((float)len/(float)2.0);
           if( t10>10) t10=14;
           count_v=0;
           if(len<15) return(0);
           for(i=4;i<=len-4;i++) {
             y=(lpWorkBuffer->traceL[0])+i-1;
             cc=0;flg_b=c_j=0;
             traceI[i]=(lpWorkBuffer->traceL[i]);

             for(j=(lpWorkBuffer->traceL[i]);j<=(lpWorkBuffer->traceC[i]);j++) {
               if(whatIsThis(lpWorkBuffer,j,y)) {
                   if(flg_b==0) {
                        flg_b=1;
                        }
                   cc++;
                   traceI[i]=j;
                   }
                else {
                     flg_b=0;
                     }
               }
            if( cc>t10 ) {
                  return(0);
                  }
            if( ( (cc>0)|| (i<t4) || (i>(len-t4)) ) ) {
                count_v++;
                }
            }
          if( count_v<(len-9) ) return(0);
          return(1);
   } /* checkIOCR() */
//---------------------------------------------------
int checkIRightOCR(int scal, STRUCTWB *lpWorkBuffer){
  int i, j, jj, cc, s4, count_v, y, len;

           len=(lpWorkBuffer->lengthTraceL);
           if(len<15) return(0);
             s4=count_v=0;
           for(i=1+4;i<len-4;i++) {
             cc=0;
             y=(lpWorkBuffer->traceL[0])+i-1;
             for(j=1;j<5;j++) {
               jj=traceI[i]+j;
               if(whatIsThis(lpWorkBuffer,jj,y)) {
                   cc++;
                   }
                }/* j */
             s4+=cc;
             if(cc>0) count_v++;

            }/* i */
           if( (s4>2)) return(1);
          return(0);
   } /* checkIRightOCR() */
//----------------------------------------------------
int checkIOCRsm(STRUCTWB *lpWorkBuffer){
  int i, j, cc, y, count_v, flg_b, flg_b2, c_j, th;
  int len;
           len=(lpWorkBuffer->lengthTraceL);
           th=(lpWorkBuffer->highGlob)/2-1;
           count_v=0;
           flg_b2=0;
           if(len<(lpWorkBuffer->highGlob)-4) return(0);
           for(i=1;i<=len;i++) {
             y=(lpWorkBuffer->traceL[0])+i-1;
             cc=0;flg_b=0;
             c_j=0;
             traceI[i]=(lpWorkBuffer->traceL[i]);

             for(j=(lpWorkBuffer->traceL[i]);j<(lpWorkBuffer->traceC[i]);j++) {
               if(whatIsThis(lpWorkBuffer,j,y)) {
                   if( (flg_b==0)||(c_j>1) ) {
                        c_j++;
                        if( (c_j>2)&&(i>2)&&(i<len-2) )  {
                              return(0);
                              }
                        flg_b=1;
                        }
                   cc++;
                   traceI[i]=j;
                   }
               else {
                        flg_b=0;
                   }
                }
            if( cc>th ) {
                  return(0);
                  }
            if(cc>0) {
                flg_b2=1;
                count_v++;
                }
            if(  (cc==0)&&(flg_b2==1) ) {
                return(0);
                }
            }
          if( count_v<th+1 ) return(0);
          countI=count_v;
          return(1);
   } /* checkIOCR() */
//---------------------------------------
int checkIROCRsm(int scal, STRUCTWB *lpWorkBuffer){
  int i, j, jj, cc, s4, y, len, tdf, df, maxdf;

           len=(lpWorkBuffer->lengthTraceL);
           s4=0;
           maxdf=0;
           tdf=traceI[1];
           for(i=1;i<len;i++) {
             df=traceI[i]-tdf;
             tdf=traceI[i];
             if(maxdf<df) maxdf=df;
             cc=0;
             y=(lpWorkBuffer->traceL[0])+i-1;
             for(j=1;j<3;j++) {
               jj=traceI[i]+j;
               if(whatIsThis(lpWorkBuffer,jj,y)) {
                   cc++;
                   }
                }/* j */
             s4+=cc;
            }/* i */
           if( (s4>0)&&rightGlue(lpWorkBuffer)) {
                return(1);
                }
          return(0);
   } /* checkIROCRsm() */
//-------------------------------------------------
int searchTrMax(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn){
  int flg_r, indTr, markInd, rm, lm, Pix, lPix, rPix, xx;
              xx=xn;
              markInd=(lpWorkBuffer->curMark1);
              indTr=(lpWorkBuffer->lengthTraceC);
              if(whatIsThis(lpWorkBuffer,xn,yn)) {return(1);}
              flg_r=0;
              do {
                Pix=whatIsThis(lpWorkBuffer,xn,yn-1);
                lPix=whatIsThis(lpWorkBuffer,xn-1,yn-1);
                rPix=whatIsThis(lpWorkBuffer,xn+1,yn-1);
                if( (Pix||lPix)&&(Pix||rPix) ) {
                  if(!flg_r) {
                     if(whatIsThis(lpWorkBuffer,xn+1,yn)||((xn-xx)>(lpWorkBuffer->thWid))||(xn>((lpWorkBuffer->traceR[max((lpWorkBuffer->lengthTraceR)-indTr+1,1)])-4))) {
                        flg_r=1;
                        xn=(lpWorkBuffer->traceC[indTr]);
                        } else {
                            xn++;
                            rm=(lpWorkBuffer->mark[markInd]).r_m;
                            if(rm<0) rm=-rm;
                            if(xn>rm)  {
                                if( (markInd+1)<(lpWorkBuffer->markNumber) ) {
                                        markInd++;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                }
                            }
                      } else {
                         if(whatIsThis(lpWorkBuffer,xn-1,yn)||((xx-xn)>(lpWorkBuffer->thWid))||(xn<((lpWorkBuffer->traceL[max((lpWorkBuffer->lengthTraceL)-indTr+1,1)])+4+(lpWorkBuffer->boundLeft)))) {
                              (lpWorkBuffer->lengthTraceC)=indTr;
                              (lpWorkBuffer->curMark1)=markInd;

                              return(1);
                              } else {
                                xn--;
                                lm=(lpWorkBuffer->mark[markInd]).l_m;
                                if(lm<0) lm=-lm;
                                if(xn<lm)  {
                                   if( (markInd-1)>=0 ) {
                                        markInd--;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                   else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                   }
                                }
                         }

                    } else {
                        yn--;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        flg_r=0;
                        }
               } while( (yn>=(lpWorkBuffer->mark[markInd]).g_m)&&((indTr+1)<MAXLTRACE));
        (lpWorkBuffer->curMark1)=markInd;
        indTr = min(indTr,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-(lpWorkBuffer->traceC[0]));
        (lpWorkBuffer->lengthTraceC)=indTr;
        return(0);
} /* of searchTrMax() */
//-------------------------------
void invTraceMax(STRUCTWB *lpWorkBuffer)
{
 int i, mem, iend;
   iend=(lpWorkBuffer->lengthTraceC)/2;
   for(i=1;i<=iend;i++)  {
        mem=(lpWorkBuffer->traceC[i]);
        (lpWorkBuffer->traceC[i])=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)-i+1]);
        (lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)-i+1])=mem;
        }
} /* of invTraceMax() */
/***************************************************************************
 *  PURPOSE      :checking if current segment is big                       *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                0 - segment is not big                                   *
 *                1 - segment is  big                                      *
 ***************************************************************************/
int isBigSegm(STRUCTWB *lpWorkBuffer){
 int higLL, higC;
 int lm, rm, wid;
      higLL=(lpWorkBuffer->lengthTraceL);
      higC=(lpWorkBuffer->lengthTraceC);
      lm=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceL[higLL]))/2;
      rm=((lpWorkBuffer->traceC[1])+(lpWorkBuffer->traceC[higC]))/2;

      wid=rm-lm+1;
      if( (wid>(lpWorkBuffer->thWid)) ) return(1);
      return(0);
 } /* of isBigSegm() */
//-----------------------------------------------------
int searchTrMin(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn){
  int flg_l, indTr, markInd, rm, lm, Pix, lPix, rPix, xx;
              xx=xn;
              markInd=(lpWorkBuffer->curMark1);
              indTr=(lpWorkBuffer->lengthTraceC);
              if(whatIsThis(lpWorkBuffer,xn,yn)) {return(1);}
              flg_l=0;
              do {
                Pix=whatIsThis(lpWorkBuffer,xn,yn+1);
                lPix=whatIsThis(lpWorkBuffer,xn-1,yn+1);
                rPix=whatIsThis(lpWorkBuffer,xn+1,yn+1);
                if( (Pix||lPix)&&(Pix||rPix) ) {
                  if(!flg_l) {
                     if(whatIsThis(lpWorkBuffer,xn-1,yn)||((xx-xn)>(lpWorkBuffer->thWid))||(xn<((lpWorkBuffer->traceL[min(indTr,(lpWorkBuffer->lengthTraceL))])+6+(lpWorkBuffer->boundLeft)))) {
                        flg_l=1;
                        xn=(lpWorkBuffer->traceC[indTr]);
                        } else {
                            xn--;
                            lm=(lpWorkBuffer->mark[markInd]).l_m;
                            if(lm<0) lm=-lm;
                            if(xn<lm)  {
                                if( (markInd-1)>=0 ) {
                                   markInd--;
                                   (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                   }
                                else {
                                   (lpWorkBuffer->lengthTraceC)=indTr;
                                   (lpWorkBuffer->curMark1)=markInd;
                                    return(1);
                                     }
                               }
                            }
                      } else {
                         if(whatIsThis(lpWorkBuffer,xn+1,yn)||((xn-xx)>(lpWorkBuffer->thWid))||(xn>((lpWorkBuffer->traceR[min(indTr,(lpWorkBuffer->lengthTraceR))])-4))) {
                              (lpWorkBuffer->lengthTraceC)=indTr;
                              (lpWorkBuffer->curMark1)=markInd;

                              return(1);
                              } else {
                                xn++;
                                rm=(lpWorkBuffer->mark[markInd]).r_m;
                                if(rm<0) rm=-rm;
                                if(xn>rm)  {
                                    if( (markInd+1)<(lpWorkBuffer->markNumber) ) {
                                         markInd++;
                                         (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                         }
                                    else {
                                         (lpWorkBuffer->lengthTraceC)=indTr;
                                         (lpWorkBuffer->curMark1)=markInd;
                                         return(1);
                                         }
                                   }
                                }
                         }

                    } else {
                        yn++;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        flg_l=0;
                        }
               } while( (yn<=(lpWorkBuffer->mark[markInd]).d_m)&&((indTr+1)<MAXLTRACE));
        (lpWorkBuffer->curMark1)=markInd;
        indTr = min(indTr,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-(lpWorkBuffer->traceC[0]));
        (lpWorkBuffer->lengthTraceC)=indTr;
        return(0);
} /* of searchTrMin() */
//----------------------------------------------
int testOCRCp1(STRUCTWB *lpWorkBuffer){
  int x, y, x1, thT, rnx, dif, dm ,gm, crL, i;
  int flg_r1, flg_r3, flg_s, len_s, x_s, max_r2, cnt_s = 0;
     x1=(lpWorkBuffer->traceL[1])+2;
     thT=(7*(lpWorkBuffer->Dpi).y+4)/200;
     rnx=min(x1+2*(lpWorkBuffer->thWid),(lpWorkBuffer->traceR[1]));
     len_s=0;
     max_r2=0;
     x_s=x1;
     for(x=x1;x<rnx;x++)  {
        crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
        if(crL<0) return(-1);
        dm=(lpWorkBuffer->mark[crL]).d_m;
        gm=(lpWorkBuffer->mark[crL]).g_m;

        y=gm;
        flg_r1=0;
        flg_r3=0;
        flg_s=1;
        while( (y<=dm)&&flg_s ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   if(y<(gm+thT)) {
                        flg_r1=y;
                        }
                   if( (y>=(gm+thT))&&(y<=(dm-thT)) ) {
                        flg_s=0;
                        }
                   if( y>(dm-thT) ) {
                        if(flg_r3==0) {
                                flg_r3=y;
                                }
                        }
                   }
              y++;
              }
        if(flg_s&&flg_r1) {
                len_s++;
                if(flg_r3==0) flg_r3=dm+1;
                dif=flg_r3-flg_r1-1;
                if(max_r2<dif) {
                        max_r2=dif;
                        x_s=x;
                        cnt_s=(flg_r1+flg_r3)/2;
                        }
                }
        else {
                if(len_s>0) {
                 if( (x_s-x1>2)&&(len_s>2)&&(dm-gm+1-max_r2<6)&&(rnx-x_s>4) ) {
                        if(testOCRCpL(lpWorkBuffer,dm,gm,x_s)==0) {
                          if(testOCRO(lpWorkBuffer,dm,gm,x1,rnx,x_s,cnt_s,max_r2)!=1) {
                            for(i=1;i<=(lpWorkBuffer->lengthTraceL);i++) {
                                (lpWorkBuffer->traceC[i])=x_s;
                                }
                            (lpWorkBuffer->traceC[0])=gm;
                            (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                            return(1);
                            }
                          }
                        }
                 }
                len_s=0;
                max_r2=0;
                }
        }
        if( (x_s-x1>2)&&(len_s>2)&&(dm-gm+1-max_r2<6)&&(rnx-x_s>4) ) {
                if(testOCRCpL(lpWorkBuffer,dm,gm,x_s)==0) {
                   if(testOCRO(lpWorkBuffer,dm,gm,x1,rnx,x_s,cnt_s,max_r2)!=1) {
                     for(i=1;i<=(lpWorkBuffer->lengthTraceL);i++) {
                        (lpWorkBuffer->traceC[i])=x_s;
                        }
                     (lpWorkBuffer->traceC[0])=gm;
                     (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                     return(1);
                     }
                   }
                }
     return(-1);
 } /* of testOCRCp1() */

int testOCRCp2(int ii, STRUCTWB *lpWorkBuffer, int x1, int x2){
  int i, isMax, isMin, indMM, kMax, kMin;
  int crL, dm, gm, x, y, yMax, yMin, dMax, dMin;
     i=0;
     isMax=0;
     indMM=(lpWorkBuffer->indMaxMark);

     while( (!isMax)&&(i<indMM)&&((lpWorkBuffer->maxMark[i])<x2) ) {
            if( (lpWorkBuffer->maxMark[i])>x1 ) {
                 isMax=1;
                 }
            else  {
                 i++;
                 }
            }
     kMax=i;
     i=0;
     isMin=0;
     indMM=(lpWorkBuffer->indMinMark);
     while( (!isMin)&&(i<indMM)&&((lpWorkBuffer->minMark[i])<x2) ) {
            if( (lpWorkBuffer->minMark[i])>x1 ) {
                 isMin=1;
                 }
            else  {
                 i++;
                 }
            }
      kMin=i;

      if((isMax==0)&&(isMin==0)) goto MAYBE;

      dMax=100;
      dMin=100;
      yMax=0;
      yMin=0;
      if(isMax) {
         x=(lpWorkBuffer->maxMark[kMax]);
         crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
         if(crL<0) return(-1);
         dm=(lpWorkBuffer->mark[crL]).d_m;
         gm=(lpWorkBuffer->mark[crL]).g_m;
         dMax=dm-gm;
         y=dm;
         while( !whatIsThis(lpWorkBuffer,x,y) )  {
             y--;
             if(y<=gm) break;
             }
         yMax=dm-y;
         }

      if(isMin) {
         x=(lpWorkBuffer->minMark[kMin]);
         crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
         if(crL<0) return(-1);
         dm=(lpWorkBuffer->mark[crL]).d_m;
         gm=(lpWorkBuffer->mark[crL]).g_m;
         dMin=dm-gm;
         y=gm;
         while( !whatIsThis(lpWorkBuffer,x,y) )  {
             y++;
             if(y>=dm) break;
             }
         yMin=y-gm;
         }
        if(isMax||isMin) {
          if(yMax>dMax/4) {
                if(yMin>2) {
                        x=((lpWorkBuffer->minMark[kMin])+(lpWorkBuffer->maxMark[kMax]))/2;
                        }
                else {
                        x=(lpWorkBuffer->maxMark[kMax]);
                        }
                crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
                if(crL<0) return(-1);
                dm=(lpWorkBuffer->mark[crL]).d_m;
                gm=(lpWorkBuffer->mark[crL]).g_m;
                if(testOCRCpL(lpWorkBuffer,dm,gm,x)==0) {
                     for(i=1;i<=(lpWorkBuffer->lengthTraceL);i++) {
                        (lpWorkBuffer->traceC[i])=x;
                        }
                     (lpWorkBuffer->traceC[0])=gm;
                     (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                     return(1);
                     }
                }

          if(yMin>dMin/4) {
                if(yMax>2) {
                        x=((lpWorkBuffer->minMark[kMin])+(lpWorkBuffer->maxMark[kMax]))/2;
                        }
                else {
                        x=(lpWorkBuffer->maxMark[kMax]);
                        }
                crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
                if(crL<0) return(-1);
                dm=(lpWorkBuffer->mark[crL]).d_m;
                gm=(lpWorkBuffer->mark[crL]).g_m;
                if(testOCRCpL(lpWorkBuffer,dm,gm,x)==0) {
                     for(i=1;i<=(lpWorkBuffer->lengthTraceL);i++) {
                        (lpWorkBuffer->traceC[i])=x;
                        }
                     (lpWorkBuffer->traceC[0])=gm;
                     (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                     return(1);
                     }
                }
          }

MAYBE:
      dMax=100;
      yMax=0;
      for(x=x1+1;x<x2;x++) {
         crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
         if(crL<0) return(-1);
         dm=(lpWorkBuffer->mark[crL]).d_m;
         gm=(lpWorkBuffer->mark[crL]).g_m;
         dMax=dm-gm;
         y=dm;
         while( !whatIsThis(lpWorkBuffer,x,y) )  {
             y--;
             if(y<=gm) break;
             }
         if(yMax<(dm-y)) {
                yMax=dm-y;
                kMax=x;
                }
         }

      dMin=100;
      yMin=0;
      for(x=x1+1;x<x2;x++) {
         crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
         if(crL<0) return(-1);
         dm=(lpWorkBuffer->mark[crL]).d_m;
         gm=(lpWorkBuffer->mark[crL]).g_m;
         dMin=dm-gm;
         y=gm;
         while( !whatIsThis(lpWorkBuffer,x,y) )  {
             y++;
             if(y>=dm) break;
             }
         if(yMin<(y-gm)) {
                yMin=y-gm;
                kMin=x;
                }
         }

         if(yMax>dMax/4) {
                if(yMin>2) {
                        x=(kMin+kMax)/2;
                        }
                else {
                        x=kMax;
                        }
                crL=srIndMark(lpWorkBuffer,0,(lpWorkBuffer->curMark2),x);
                if(crL<0) return(-1);
                dm=(lpWorkBuffer->mark[crL]).d_m;
                gm=(lpWorkBuffer->mark[crL]).g_m;
                if(testOCRCpL(lpWorkBuffer,dm,gm,x)==0) {
                     for(i=1;i<=(lpWorkBuffer->lengthTraceL);i++) {
                        (lpWorkBuffer->traceC[i])=x;
                        }
                     (lpWorkBuffer->traceC[0])=gm;
                     (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                     return(1);
                     }
                }

      return(-1);
} /* of testOCRCp2() */
//------------------------------------------------------------
int testOCRO(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1, int x2, int x_s, int y_c, int y_max){
  int x, y, xL, xR, x_c, i, xLm, xRm, inside;
     y=y_c;
     x=x_s;
     while( (!whatIsThis(lpWorkBuffer,x,y))&&(x>x1) )  {
                x--;
                }
     xL=x+1;
     x=x_s;
     while( (!whatIsThis(lpWorkBuffer,x,y))&&(x<x2) )  {
                x++;
                }
     xR=x-1;
     x_c=(xL+xR)/2;
     xLm=x_c-xL;
     xRm=y_max/2-xLm;
     if(xRm<0) xRm=-xRm;
     if(xRm>xLm+1) return(-1);
     i=1;
     y=y_c;
     inside=0;
     while(i&&(y>gm)) {
        y--;
        x=x_c;
        while( (!whatIsThis(lpWorkBuffer,x,y))&&(x>x1) )  {
                x--;
                }
        xL=x+1;
        if(xL>=x_c) i=0;
        x=x_s;
        while( (!whatIsThis(lpWorkBuffer,x,y))&&(x<x2) )  {
                x++;
                }
        xR=x-1;
        if(xR<=x_c) i=0;
        if((xL>=0)&&(xR>=0)) {
                xRm=xR-x_c;
                xLm=x_c-xL;
                xRm-=xLm;
                if(xRm<0) xRm=-xRm;
                if(xRm>2) return(-1);
                }
        inside++;
         }
     if(inside<4)  return(-1);
     i=1;
     y=y_c;
     inside=0;
     while(i&&(y<dm)) {
        y++;
        x=x_c;
        while( (!whatIsThis(lpWorkBuffer,x,y))&&(x>x1) )  {
                x--;
                }
        xL=x+1;
        if(xL>=x_c) i=0;
        x=x_s;
        while( (!whatIsThis(lpWorkBuffer,x,y))&&(x<x2) )  {
                x++;
                }
        xR=x-1;
        if(xR<=x_c) i=0;
        if((xL>=0)&&(xR>=0)) {
                xRm=xR-x_c;
                xLm=x_c-xL;
                xRm-=xLm;
                if(xRm<0) xRm=-xRm;
                if(xRm>2) return(-1);
                }
        inside++;
         }
         if(inside<4)  return(-1);
         return(1);
 } /* of testOCRO() */
//---------------------------------------------------------
int cutOCRCpL(int scal, STRUCTWB *lpWorkBuffer, int kMax, int kMin, int kindD, int ii){
 int whatM, maxMin = 0, imaxMin = 0, minMax = 0, iminMax = 0, i, xCut;
 int crL, crR, crLMax, crLMin, rangeMax, rangeMin = 0, rMn = 0, rMx = 0, rMn1 = 0, rMx1 = 0;
 int thT, lenY, Ok;

  thT=(6*(lpWorkBuffer->Dpi).x+4)/200;
/*  kind of cutting */
  if( (indBottTop==0)&&(indTopBott==0) ) return(0);
  whatM=0;
  if(indTopBott!=0) whatM++;
  if( (indBottTop!=0)&&(indTopBott!=0) ) whatM++;

/*  place of cutting in y */
  crL=0;
  crR=(lpWorkBuffer->curMark2);
  crLMax=-1;
  if((whatM==2)||(whatM==0)) {
    crLMax=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->maxMark[kMax]));

    if(crLMax<0) {
        return(0);
        }
    minMax=(lpWorkBuffer->mark[crLMax]).d_m;
    iminMax=0;
    for(i=0;i<indBottTop;i++) {
        if(bottTopEndTr[i].y<minMax) {
                minMax=bottTopEndTr[i].y;
                iminMax=i;
                }
        }
    rangeMax=(lpWorkBuffer->mark[crLMax]).d_m-(lpWorkBuffer->mark[crLMax]).g_m;
    rMx=(lpWorkBuffer->mark[crLMax]).g_m+(2*rangeMax)/5;
    rMx1=(lpWorkBuffer->mark[crLMax]).g_m+(2*rangeMax)/3;
    }
  crLMin=-1;
  if((whatM==2)||(whatM==1)) {
    crLMin=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->minMark[kMin]));
    if(crLMin<0) {
        return(0);
        }
    maxMin=0;
    imaxMin=0;
    for(i=0;i<indTopBott;i++) {
        if(topBottEndTr[i].y>maxMin) {
                maxMin=topBottEndTr[i].y;
                imaxMin=i;
                }
        }
    rangeMin=(lpWorkBuffer->mark[crLMin]).d_m-(lpWorkBuffer->mark[crLMin]).g_m;
    rMn=(lpWorkBuffer->mark[crLMin]).d_m-(2*rangeMin)/5;
    rMn1=(lpWorkBuffer->mark[crLMin]).d_m-(2*rangeMin)/3;
    }

switch (kindD)  {
    case 6:case 4:
     if(crLMax>=0) {

       if( ((minMax-(lpWorkBuffer->mark[crLMax]).g_m)<thT) ) {
           xCut=(lpWorkBuffer->maxMark[kMax]);
           if(testOCRCpL(lpWorkBuffer,(lpWorkBuffer->mark[crLMax]).d_m,(lpWorkBuffer->mark[crLMax]).g_m,xCut)<0) return(0);
           if(tryCutTb(scal,lpWorkBuffer,xCut,crLMax,rMx)>0)  {
                 return(1);
               }
           }
       }
    if(crLMin>=0) {
       if( (((lpWorkBuffer->mark[crLMin]).d_m-maxMin)<thT)&&((topBottEndTr[imaxMin].x-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]))>(((rangeMin*2)/3)+2)) ) {
           xCut=(lpWorkBuffer->minMark[kMin]);
           if(tryCutBt(scal,lpWorkBuffer,xCut,crLMin,rMn)>0)  {
                  return(1);
                  }
           }
       }
       break;
    case 5: case 7:
     if(crLMax>=0) {
            i=whichFrame(lpWorkBuffer,bottTopEndTr[iminMax].x);
            if(i>=0) {
               lenY=minMax-(lpWorkBuffer->curLine[i]).ym1;
               }
            else {
               lenY=-100;
               }
           if((lenY<8)&&(lenY>=0)) {
            if((Ok=highTestOCR(lpWorkBuffer,bottTopEndTr[iminMax].x,minMax,0,crLMax,i,ii))>0) {
               if(Ok==2) {
                  xCut=(lpWorkBuffer->linkRM[1]);
                  }
               else {
                  xCut=bottTopEndTr[iminMax].x;
                  }
                if(tryCutTb(scal,lpWorkBuffer,xCut,crLMax,rMx1)>0)  {
                     if(massRight(lpWorkBuffer,crLMax)>0) {
                        return(1);
                        }
                     }
               }
            }
           return(0);
             }
     if(crLMin>=0) {
            i=whichFrame(lpWorkBuffer,topBottEndTr[imaxMin].x);
            if(i>=0) {
               lenY=(lpWorkBuffer->curLine[i]).ym2-maxMin;
               }
            else {
               lenY=-100;
               }

           if((lenY<8)&&(lenY>=0)) {
            if(highTestOCR(lpWorkBuffer,topBottEndTr[imaxMin].x,maxMin,1,crLMin,i,ii)==1) {
               xCut=topBottEndTr[imaxMin].x;
               if(tryCutBt(scal,lpWorkBuffer,xCut,crLMin,rMn1)>0)  {
                    if(massRight(lpWorkBuffer,crLMin)>0) {
                       return(1);
                       }
                    }
               }
            }
          }
     break;
     }
return(0);
} /* of cutOCRCpL() */
//----------------------------------------------
int traceBtTp(int scal,STRUCTWB *lpWorkBuffer, int kMax){
  int i, xn, yn, i_d;
  int crL, crR ;
  int pom_trace[4], ind_tr[4];
  int ind_tr_p, flg_M, whFr, whY;
      crL=max((lpWorkBuffer->curMark1)-1,0);
      crR=(lpWorkBuffer->curMark2);
      i=kMax;
      crL=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->maxMark[i]));
      if(crL<0) {
//ReleaseDC(hWnd,hDCC);
          return(-1);
          }
       (lpWorkBuffer->curMark1)=crL;
       yn=(lpWorkBuffer->mark[crL]).d_m;
       xn=(lpWorkBuffer->maxMark[i]);
       flg_M=0;

AA1:   (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crL]).g_m;
       (lpWorkBuffer->traceC[1])=xn;
       (lpWorkBuffer->lengthTraceC)=1;
       if(!searchTrMax(scal,lpWorkBuffer,xn,yn)) {

/* walking from bottom to top is successful */
AA:
                invTraceMax(lpWorkBuffer);
                return(0);
                }
      else {
/* walking from bottom to top is not successful */
                bottTopEndTr[indBottTop].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
                bottTopEndTr[indBottTop].y=yn-(lpWorkBuffer->lengthTraceC);
                indBottTop++;
                if((lpWorkBuffer->lengthTraceC)<4) {
                  if(flg_M==0) {
                        xn-=2;
                        flg_M++;
                        goto AA1;
                        }
                  else  {
                      return(1);
                      }
                  }
                ind_tr_p=(lpWorkBuffer->lengthTraceC);
                for(i_d=0;i_d<3;i_d++) {
                    ind_tr[i_d]=(int)(((float)((lpWorkBuffer->lengthTraceC)-1)*(float)(3-i_d))/4.0);
                    if(ind_tr[i_d]<2) {ind_tr[i_d]=2;}
                    pom_trace[i_d]=(lpWorkBuffer->traceC[ind_tr[i_d]]);
                    }

                for(i_d=0;i_d<3;i_d++) {
                    (lpWorkBuffer->lengthTraceC)=ind_tr[i_d];
                    yn=(lpWorkBuffer->mark[crL]).d_m-(lpWorkBuffer->lengthTraceC);
                    xn=pom_trace[i_d];
                    whFr=whichFrame(lpWorkBuffer,xn);
                    if(whFr>=0) {
                             whY=(lpWorkBuffer->curLine[whFr]).ym2;
                             }
                    else {
                             whY=-100;
                             }
                    if((whY>0)&&(yn>whY)) continue;
                    while( !whatIsThis(lpWorkBuffer,++xn,yn)&&(xn<((lpWorkBuffer->maxMark[i])+(lpWorkBuffer->thWid)))&&(xn<((lpWorkBuffer->traceR[max((lpWorkBuffer->lengthTraceR)-(lpWorkBuffer->lengthTraceC)+1,1)])-4)) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                           if(searchTrMax(scal,lpWorkBuffer,xn,yn)) {
                              if(indBottTop<MAXLTRACE) {
                                     bottTopEndTr[indBottTop].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
                                     bottTopEndTr[indBottTop].y=(lpWorkBuffer->mark[crL]).d_m-(lpWorkBuffer->lengthTraceC);
                                     indBottTop++;
                                     }
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                           else {
                              goto AA;
                              }
                           }
                        }/* while ++x */
                    xn=pom_trace[i_d];

                    while( !whatIsThis(lpWorkBuffer,--xn,yn)&&(xn>((lpWorkBuffer->maxMark[i])-(lpWorkBuffer->thWid)))&&(xn>((lpWorkBuffer->traceL[max((lpWorkBuffer->lengthTraceL)-(lpWorkBuffer->lengthTraceC)+1,1)])+6+(lpWorkBuffer->boundLeft))) ) {
                        if( (xn%2)==(i_d/2) ) { /* just save time */
                          if(searchTrMax(scal,lpWorkBuffer,xn,yn)) {
                                if(indBottTop<MAXLTRACE) {
                                     bottTopEndTr[indBottTop].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
                                     bottTopEndTr[indBottTop].y=(lpWorkBuffer->mark[crL]).d_m-(lpWorkBuffer->lengthTraceC);
                                     indBottTop++;
                                     }
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        }/* while --x */

                     }/* for(i_d */
               }
             return(1);
} /* of traceBtTp() */
//-------------------------------------
int traceTpBt(int scal, STRUCTWB *lpWorkBuffer, int kMin){
  int i,  xn, yn, i_d;
  int crL,crR;
  int pom_trace[4], ind_tr[4];
  int ind_tr_p, flg_M, whFr, whY;

      crL=max((lpWorkBuffer->curMark1)-1,0);
      crR=(lpWorkBuffer->curMark2);
      i=kMin;
      crL=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->minMark[i]));
      if(crL<0) {
          return(-1);
          }
      (lpWorkBuffer->curMark1)=crL;
      yn=(lpWorkBuffer->mark[crL]).g_m;
      xn=(lpWorkBuffer->minMark[i]);
      flg_M=0;

AA1:  (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crL]).g_m;
      (lpWorkBuffer->traceC[1])=xn;
      (lpWorkBuffer->lengthTraceC)=1;
      if(!searchTrMin(scal,lpWorkBuffer,xn,yn)) {
AA:
                return(0);
                }
       else {
               topBottEndTr[indTopBott].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
               topBottEndTr[indTopBott].y=yn+(lpWorkBuffer->lengthTraceC);
               indTopBott++;
               if((lpWorkBuffer->lengthTraceC)<5) {
                  if(flg_M==0) {
                        xn+=2;
                        flg_M=1;
                        goto AA1;
                        }
                  else  {
                      return(1);
                      }
                  }

               ind_tr_p=(lpWorkBuffer->lengthTraceC);
               for(i_d=0;i_d<3;i_d++) {
                      ind_tr[i_d]=(int)(((float)((lpWorkBuffer->lengthTraceC)-1)*(float)(3-i_d))/4.0);
                      if(ind_tr[i_d]<2) {ind_tr[i_d]=2;}
                      pom_trace[i_d]=(lpWorkBuffer->traceC[ind_tr[i_d]]);
                      }

               for(i_d=0;i_d<3;i_d++) {
                      (lpWorkBuffer->lengthTraceC)=ind_tr[i_d];
                      yn=(lpWorkBuffer->mark[crL]).g_m+(lpWorkBuffer->lengthTraceC);
                      xn=pom_trace[i_d];
                      whFr=whichFrame(lpWorkBuffer,xn);
                      if(whFr>=0) {
                             whY=(lpWorkBuffer->curLine[whFr]).ym1;
                             }
                      else {
                             whY=-100;
                             }
                      if((whY>0)&&(yn<whY)) continue;

                      while( !whatIsThis(lpWorkBuffer,--xn,yn)&&(xn>((lpWorkBuffer->minMark[i])-(lpWorkBuffer->thWid)))&&(xn>((lpWorkBuffer->traceL[min((lpWorkBuffer->lengthTraceC),(lpWorkBuffer->lengthTraceL))])+4+(lpWorkBuffer->boundLeft))) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                          if(searchTrMin(scal,lpWorkBuffer,xn,yn)) {
                                if(indTopBott<MAXLTRACE) {
                                   topBottEndTr[indTopBott].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
                                   topBottEndTr[indTopBott].y=(lpWorkBuffer->mark[crL]).g_m+(lpWorkBuffer->lengthTraceC);
                                   indTopBott++;
                                     }
                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        } /* while --x */

                      xn=pom_trace[i_d];
                      while( !whatIsThis(lpWorkBuffer,++xn,yn)&&(xn<((lpWorkBuffer->minMark[i])+(lpWorkBuffer->thWid)))&&(xn<((lpWorkBuffer->traceR[min((lpWorkBuffer->lengthTraceC),(lpWorkBuffer->lengthTraceR))])-5)) ) {
                        if( (xn%2)==(i_d/2) ) {  /* just save time */
                          if(searchTrMin(scal,lpWorkBuffer,xn,yn)) {
                                if(indTopBott<MAXLTRACE) {
                                   topBottEndTr[indTopBott].x=(lpWorkBuffer->traceC[(lpWorkBuffer->lengthTraceC)]);
                                   topBottEndTr[indTopBott].y=(lpWorkBuffer->mark[crL]).g_m+(lpWorkBuffer->lengthTraceC);
                                   indTopBott++;
                                     }

                              (lpWorkBuffer->lengthTraceC)=((ind_tr_p-1)*(3-i_d))/4;
                              if((lpWorkBuffer->lengthTraceC)<2) {(lpWorkBuffer->lengthTraceC)=2;}
                              }
                          else {
                              goto AA;
                              }
                          }
                        } /* while ++x */
                     }/* for(i_d */
               }
              return(1);
} /* of traceTpBt() */
//----------------------------------------------------
int cutDec(int scal,STRUCTWB *lpWorkBuffer, int kMax, int kMin, int ii){
 int whatM, whatPlace, maxMin = 0, imaxMin, minMax = 0, iminMax, i, xCut, xx, cc;
 int crL, crR, crLMax, crLMin, rangeMax = 0, rangeMin = 0, rMn = 0, rMx = 0, xMn, xMx, dt;

/*  kind of cutting */
  if( (indBottTop==0)&&(indTopBott==0) ) return(0);
  whatM=0;
  if(indTopBott!=0) whatM++;
  if( (indBottTop!=0)&&(indTopBott!=0) ) whatM++;

/*  place of cutting in y */
  crL=0;
  crR=(lpWorkBuffer->curMark2);
  crLMax=-1;
  if((whatM==2)||(whatM==0)) {
    crLMax=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->maxMark[kMax]));
    if(crLMax<0) {
        return(0);
        }
    minMax=(lpWorkBuffer->mark[crLMax]).d_m;
    iminMax=0;
    for(i=0;i<indBottTop;i++) {
        if(bottTopEndTr[i].y<minMax) {
                minMax=bottTopEndTr[i].y;
                iminMax=i;
                }
        }
    rangeMax=(lpWorkBuffer->mark[crLMax]).d_m-(lpWorkBuffer->mark[crLMax]).g_m;
    rMx=(lpWorkBuffer->mark[crLMax]).g_m+(2*rangeMax)/5;
    }

  crLMin=-1;
  if((whatM==2)||(whatM==1)) {
    crLMin=srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->minMark[kMin]));
    if(crLMin<0) {
        return(0);
        }
    maxMin=0;
    imaxMin=0;
    for(i=0;i<indTopBott;i++) {
        if(topBottEndTr[i].y>maxMin) {
                maxMin=topBottEndTr[i].y;
                imaxMin=i;
                }
        }
    rangeMin=(lpWorkBuffer->mark[crLMin]).d_m-(lpWorkBuffer->mark[crLMin]).g_m;
    rMn=(lpWorkBuffer->mark[crLMin]).d_m-(2*rangeMin)/5;
    }


  whatPlace=-1;

  if((crLMax>=0)&&(minMax<rMx)) {
        whatPlace=0;
        }
  if((crLMin>=0)&&(maxMin>rMn)) {
        whatPlace=1;
        }
  if((whatPlace<0)&&(crLMax>=0)&&(crLMin>=0)) {
     if( (minMax<((lpWorkBuffer->mark[crLMax]).d_m-rangeMax/3))&& (maxMin>((lpWorkBuffer->mark[crLMin]).g_m+rangeMin/3)) ) {
        whatPlace=2;
        }
     }
     switch(whatPlace) {

         case 0:if(whatM==1)   return(0);                            // top
                dt=(lpWorkBuffer->maxMark[kMax])-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]-(lpWorkBuffer->boundLeft));
                if( dt<(rangeMax/8) ) return(0);
                if(lastTestMax(lpWorkBuffer,(lpWorkBuffer->mark[crLMax]).d_m,(lpWorkBuffer->mark[crLMax]).g_m,(lpWorkBuffer->maxMark[kMax]))<0) return(0);
                if( (whatM==2)&&(dt>(rangeMax/6)) ) {
                     xMn=(lpWorkBuffer->minMark[kMin]);
                     xMx=(lpWorkBuffer->maxMark[kMax]);
                     xCut=(xMx-xMn)/4;
                     xCut+=xMn;
                     xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                     if( xx>(rangeMax/4) ) {
                       xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                       if( xx>(rangeMax/4) ) {
                          if(tryCutTb(scal,lpWorkBuffer,xCut,crLMax,rMx)>0)  {
                               return(1);
                               }
                          }
                       }
                     }
                if(whatM==0) {
                     xCut=testLineTop(lpWorkBuffer,(lpWorkBuffer->mark[crLMax]).d_m,(lpWorkBuffer->mark[crLMax]).g_m,(lpWorkBuffer->maxMark[kMax]));
                     if(xCut<0) {
                        if( dt<(rangeMax/6) ) return(0);
                        if(maxminBottTop(lpWorkBuffer,&xMn,&xMx)<0) return(0);
                        xCut=maxGlob(lpWorkBuffer,(lpWorkBuffer->mark[crLMax]).g_m,rMx,xMn,xMx);
                        if(xCut<0) {
                            xCut=(xMx+xMn)/2;
                            }
                        }
                     xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                     if( xx<(rangeMax/4) ) return(0);
                     xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                     if( xx<(rangeMax/4) ) return(0);
                     if(tryCutTb(scal,lpWorkBuffer,xCut,crLMax,rMx)>0)  {
                             return(1);
                             }
                     }

                break;

         case 1:if(whatM==0)   return(0);                           // bottom
                dt=(lpWorkBuffer->minMark[kMin])-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft);
                if( dt<(rangeMin/8) ) return(0);
                if(lastTestMin(lpWorkBuffer,(lpWorkBuffer->mark[crLMin]).d_m,(lpWorkBuffer->mark[crLMin]).g_m,(lpWorkBuffer->minMark[kMin]))<0) return(0);
                if( (whatM==2)&&(dt>(rangeMin/6)) ) {
                     xMn=(lpWorkBuffer->minMark[kMin]);
                     xMx=(lpWorkBuffer->maxMark[kMax]);
                     xCut=(xMn-xMx)/4;
                     xCut+=xMx;
                     xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                     if( xx>(rangeMin/4) )  {
                        xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                        if( xx>(rangeMin/4) ) {
                           if(tryCutBt(scal,lpWorkBuffer,xCut,crLMin,rMn)>0)  {
                              return(1);
                              }
                           }
                        }
                     }
                if(whatM==1) {
                     xCut=testLineBott(lpWorkBuffer,(lpWorkBuffer->mark[crLMin]).d_m,(lpWorkBuffer->mark[crLMin]).g_m,(lpWorkBuffer->minMark[kMin]));
                     if(xCut<0) {
                        if( dt<(rangeMin/6) ) return(0);
                        if(maxminTopBott(lpWorkBuffer,&xMn,&xMx)<0) return(0);
                        xCut=minGlob(lpWorkBuffer,(lpWorkBuffer->mark[crLMin]).d_m,rMn,xMn,xMx);
                        if(xCut<0) {
                            xCut=(xMx+xMn)/2;
                            }
                        }
                     xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                     if( xx<(rangeMin/4) ) return(0);
                     xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                     if( xx<(rangeMin/4) ) return(0);
                     if(tryCutBt(scal,lpWorkBuffer,xCut,crLMin,rMn)>0)  {
                             return(1);
                             }
                     }
                break;
         case 2:                                                    // medium
                cc=0;
                if(whatM==2) {
                   if((lpWorkBuffer->minMark[kMin])<(lpWorkBuffer->maxMark[kMax])) cc=1;
                   }
                for(i=0;i<2;i++)  {
                    switch(cc) {
                      case 0:
                          if((whatM==0)||(whatM==2)) {
                             dt=(lpWorkBuffer->maxMark[kMax])-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft);
                             if( dt<(rangeMax/6) ) break;
                             xCut=testLineMid(ii,lpWorkBuffer,(lpWorkBuffer->mark[crLMax]).d_m,(lpWorkBuffer->mark[crLMax]).g_m,(lpWorkBuffer->maxMark[kMax]),1);
                             if(xCut>0) {
                                xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                                if( xx<(rangeMax/3) ) return(0);
                                xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                                if( xx<(rangeMax/3) ) return(0);
                                if(tryCutTb(scal,lpWorkBuffer,xCut,crLMax,rMn)>0)  {
                                  return(1);
                                  }
                                }
                             }
                           break;
                     case 1:
                          if((whatM==1)||(whatM==2)) {
                             dt=(lpWorkBuffer->minMark[kMin])-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft);
                             if( dt<(rangeMin/6) ) break;
                             xCut=testLineMid(ii+2,lpWorkBuffer,(lpWorkBuffer->mark[crLMin]).d_m,(lpWorkBuffer->mark[crLMin]).g_m,(lpWorkBuffer->minMark[kMin]),0);
                             if(xCut>0) {
                                xx=min(xCut-(lpWorkBuffer->traceL[1])-(lpWorkBuffer->boundLeft),xCut-(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])-(lpWorkBuffer->boundLeft));
                                if( xx<(rangeMin/3) ) return(0);
                                xx=min((lpWorkBuffer->traceR[1])-xCut,(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-xCut);
                                if( xx<(rangeMax/3) ) return(0);
                                if(tryCutBt(scal,lpWorkBuffer,xCut,crLMin,rMx)>0)  {
                                  return(1);
                                  }
                                }
                              }
                           break;
                    }
                  if(cc) cc--; else cc++;
                  }/* for i */
                break;
        }
  if(whatPlace<0) return(0);
return(0);
} /* of cutDec() */
//----------------------------------
int rightGlue(STRUCTWB *lpWorkBuffer){
  int i, j, cc, y, count_v, flg_b, flg_b2, c_j, th;
  int len, indd, rLmt, cntW, wasB;
           len=min((lpWorkBuffer->lengthTraceC),(lpWorkBuffer->lengthTraceR));
           th=(lpWorkBuffer->highGlob/2)-1;
           count_v=0;
           flg_b2=0;
           wasB=0;
           if(len<(lpWorkBuffer->highGlob)-4) return(0);
           indd=0;
           for(i=1;i<len;i++) {
               if( (lpWorkBuffer->traceR[i])-(lpWorkBuffer->traceC[i])<2 ) indd=1;
               }

           for(i=2;i<len-1;i++) {
             y=(lpWorkBuffer->traceC[0])+i-1;
             cc=0;flg_b=0;
             c_j=0;
             cntW=0;
             if(indd) {
                if((lpWorkBuffer->curMark2)+1<(lpWorkBuffer->markNumber)) {
                    rLmt=min((lpWorkBuffer->traceC[i])+(2*(lpWorkBuffer->highGlob))/3+2,(lpWorkBuffer->mark[(lpWorkBuffer->curMark2)+1]).r_m);
                    }
                else {
                    rLmt=(lpWorkBuffer->traceC[i])+(2*(lpWorkBuffer->highGlob))/3+2;
                    }
                }
             else {
                 rLmt=(lpWorkBuffer->traceR[i]);
                }
             for(j=(lpWorkBuffer->traceC[i]);j<rLmt;j++) {
               if(whatIsThis(lpWorkBuffer,j,y)) {
                   if( (flg_b==0)||(c_j>1) ) {
                        c_j++;
                        if( (c_j>2)&&(i>2)&&(i<len-2)&&(cntW>2) )  {
                              return(0);
                              }
                        flg_b=1;
                        }
                   cc++;
                   }
                else {
                        if(flg_b==1) cntW=0;
                        cntW++;
                        flg_b=0;
                   }
                }
            if( cc>=th ) {
                  return(0);
                  }
            if(cc>0) {
                if(wasB) {
                   return(0);
                   }
                flg_b2=1;
                count_v++;
                }
            if(  (cc==0)&&(flg_b2==1) ) {
                wasB=1;
                flg_b2=0;
                }
            }
          if( count_v<th+1 ) return(0);
          if( (countI>=(lpWorkBuffer->highGlob))&&(count_v>=(lpWorkBuffer->highGlob)) ) return(0);
          return(1);
   } /* rightGlue() */
//-------------------------------------------
int testOCRCpL(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1){
  int x, y, c_gr, thT, rnx, i;

     thT=4;
     rnx=max(x1-(lpWorkBuffer->thWid),(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]));
     c_gr=0;
     for(x=x1;x>rnx;x--)  {
        y=(dm+gm)/2;
        i=0;
        while( (i<4)&&(y<dm) ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) goto NXT;
              y++;
              i++;
              }
        }
     return(-1);

NXT:
     rnx=min(x1+(lpWorkBuffer->thWid),(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]));
     c_gr=0;
     for(x=x1;x<rnx;x++)  {
        y=(dm+gm)/2;
        i=0;
        while( (i<4)&&(y<dm) ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) return(0);
              y++;
              i++;
              }
        }
     return(-1);

 } /* of TestOCRCpL() */
//-------------------------------[-------------------------
int tryCutTb(int scal,STRUCTWB *lpWorkBuffer, int xx, int crL, int rx){
 int crR, xn, thT;

         thT=(4*(lpWorkBuffer->Dpi).x+4)/200;
         crR=(lpWorkBuffer->curMark2);
         xn=srIndMark(lpWorkBuffer,0,crR,xx);
         if(xn<0) {
           xx-=2;
           xn=srIndMark(lpWorkBuffer,0,crR,xx);
           if(xn<0) return(-1);
           }
         (lpWorkBuffer->curMark1)=xn;
         (lpWorkBuffer->traceC[1])=xx;
         (lpWorkBuffer->lengthTraceC)=1;
         if(!searchTrMinC(scal,lpWorkBuffer,xx,(lpWorkBuffer->mark[crL]).g_m,rx) ) {
            return(1);
            }
         if(xx-thT>(lpWorkBuffer->traceL[1])+2) {
           (lpWorkBuffer->curMark1)=xn;
           (lpWorkBuffer->traceC[1])=xx-thT;
           (lpWorkBuffer->lengthTraceC)=1;
           if(!searchTrMinC(scal,lpWorkBuffer,xx-thT,(lpWorkBuffer->mark[crL]).g_m,rx) ) {
              return(1);
              }
           }
         if(xx+thT<(lpWorkBuffer->traceR[1])-2) {
           (lpWorkBuffer->curMark1)=xn;
           (lpWorkBuffer->traceC[1])=xx+thT;
           (lpWorkBuffer->lengthTraceC)=1;
           if(!searchTrMinC(scal,lpWorkBuffer,xx+thT,(lpWorkBuffer->mark[crL]).g_m,rx) ) {
              return(1);
              }
           }
         return(-1);
 } /* of tryCutTb() */
//-----------------------------------------------------------------------
int tryCutBt(int scal, STRUCTWB *lpWorkBuffer, int xx, int crL, int rn){
 int crR, xn, thT;
         thT=(4*(lpWorkBuffer->Dpi).x+4)/200;
         crR=(lpWorkBuffer->curMark2);
         xn=srIndMark(lpWorkBuffer,0,crR,xx);
         if(xn<0) {
           xx-=2;
           xn=srIndMark(lpWorkBuffer,0,crR,xx);
           if(xn<0) return(-1);
           }
         (lpWorkBuffer->curMark1)=xn;
         (lpWorkBuffer->traceC[1])=xx;
         (lpWorkBuffer->lengthTraceC)=1;
         if(!searchTrMaxC(scal,lpWorkBuffer,xx,(lpWorkBuffer->mark[crL]).d_m+1,rn) ) {
            invTraceMax(lpWorkBuffer);
            return(1);
            }

         if(xx-thT>(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)])+2) {
           (lpWorkBuffer->curMark1)=xn;
           (lpWorkBuffer->traceC[1])=xx-thT;
           (lpWorkBuffer->lengthTraceC)=1;
           if(!searchTrMaxC(scal,lpWorkBuffer,xx-thT,(lpWorkBuffer->mark[crL]).d_m+1,rn) ) {
              invTraceMax(lpWorkBuffer);
              return(1);
              }
           }

         if(xx+thT<(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)])-2) {
           (lpWorkBuffer->curMark1)=xn;
           (lpWorkBuffer->traceC[1])=xx+thT;
           (lpWorkBuffer->lengthTraceC)=1;
           if(!searchTrMaxC(scal,lpWorkBuffer,xx+thT,(lpWorkBuffer->mark[crL]).d_m+1,rn) ) {
              invTraceMax(lpWorkBuffer);
              return(1);
              }
           }
         return(-1);
 } /* of tryCutBt() */
//-------------------------------------------------------------
int highTestOCR(STRUCTWB *lpWorkBuffer, int x, int y, int what, int crL, int indFr, int ii){
  int botYL, botYL2, botYR, upYL, upYR, iy, ix, blacky, blackx, xx, yy;
  int yL, yR, mass, lenW, high, lenY;
      if(what==0) {
        lenY=y-(lpWorkBuffer->curLine[indFr]).ym1;
        }
      else {
        lenY=(lpWorkBuffer->curLine[indFr]).ym2-y;
        }
      botYL=y;
      botYL2=0;
      botYR=y;
      upYL =y;
      upYR =y;
      xx=0;
      yy=y;
      iy=y;
      yL=x-(lpWorkBuffer->thWid)/2;
      yR=x+(lpWorkBuffer->thWid)/2;
      if(what==0) yR+=2;
      blacky=1;
      while( (iy<(lpWorkBuffer->mark[crL]).d_m)&&blacky ) {
        ix=x;
        blackx=1;
        while( (ix>yL)&&blackx ) {
              if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                botYL=iy;
                blackx=0;
                }
              ix--;
              }
        if(blackx) blacky=0;
        iy++;
        }

      iy=y;
      blacky=1;
      while( (iy<(lpWorkBuffer->mark[crL]).d_m)&&blacky ) {
        ix=x;
        blackx=1;
        while( (ix>yL)&&blackx ) {
              if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                botYL2=iy;
                blackx=0;
                }
              ix--;
              }
        iy++;
        }
      iy=y;
      blacky=1;
      while( (iy>(lpWorkBuffer->mark[crL]).g_m)&&blacky ) {
        ix=x;
        blackx=1;
        while( (ix>yL)&&blackx ) {
              if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                upYL=iy;
                if(upYL<yy) {
                        yy=upYL;
                        xx=ix-1;
                        }
                blackx=0;
                }
              ix--;
              }
        iy--;
        }
      iy=y;
      blacky=1;
      while( (iy<(lpWorkBuffer->mark[crL]).d_m)&&blacky ) {
        ix=x;
        blackx=1;
        while( (ix<yR)&&blackx ) {
              if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                botYR=iy;
                blackx=0;
                }
              ix++;
              }
        iy++;
        }
      iy=y;
      blacky=1;
      while( (iy>(lpWorkBuffer->mark[crL]).g_m)&&blacky ) {
        ix=x;
        blackx=1;
        while( (ix<yR)&&blackx ) {
              if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                upYR=iy;
                blackx=0;
                }
              ix++;
              }
        iy--;
        }
        yL=botYL-upYL;
        yR=botYR-upYR;
        blackx=max(yL,yR);

        switch(what) {

          case 1:   /* minimum */

             mass=-1;
             if( (yL>(lpWorkBuffer->avLineHigh))&&(xx>0)) {
               iy=yy;
               mass=0;
               while( iy<(lpWorkBuffer->mark[crL]).d_m ) {
                  if( whatIsThis(lpWorkBuffer,xx,iy) )  {
                     mass++;
                     }
                  iy++;
                  }
               }
             iy=y-2;
             ix=x;
             blacky=1;
             lenW=0;
             while( (ix<(x+(lpWorkBuffer->thWid)/2))&&blacky ) {
               if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                 blacky=0;
                 }
                else {
                 lenW++;
                 }
               ix++;
               }

             ix=x-1;
             blacky=1;
             while( (ix>(x-(lpWorkBuffer->thWid)/2))&&blacky ) {
               if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                 blacky=0;
                 }
                else {
                 lenW++;
                 }
               ix--;
               }
             if( (yL>(lpWorkBuffer->avLineHigh))&&(yR>(lpWorkBuffer->avLineHigh)) ) {
                  lenW+=2;
                  }
             iy=1;
             if(lenW<4) iy=0;
             if(mass!=-1)  {
                if(mass<(2*yL)/3) iy=0;
                }

             if( (blackx>(lpWorkBuffer->avLineHigh))&&iy) {
                return(1);
                }

             ix=x;
             blackx=1;
             high=(lpWorkBuffer->mark[crL]).d_m;
             while( (ix>(x-(lpWorkBuffer->thWid)/2-2))&&blackx ) {
                 iy=min((lpWorkBuffer->mark[crL]).d_m,y+lenY);
                 blacky=1;
                 while( (iy>=(lpWorkBuffer->mark[crL]).g_m)&&blacky ) {
                    if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                       blacky=0;
                       }
                    if(high>iy) {
                       high=iy;
                       xx=ix;
                       }
                    iy--;
                    }
                 ix--;
                 }
             ix=(int)(((float)(lpWorkBuffer->avLineHigh)/(float)5.0)+(float)0.5);
             ix=(ix*(lpWorkBuffer->Dpi).y+4)/200+1;

             if( ((y+lenY)-high)<=ix ) {
                return(1);
                }
             ix=(x+(lpWorkBuffer->thWid)/2);
             high=0;
             iy=(lpWorkBuffer->mark[crL]).d_m+1;
             blacky=0;
             while( (iy>=(lpWorkBuffer->mark[crL]).g_m-1) ) {
                 if( whatIsThis(lpWorkBuffer,ix-1,iy)&&whatIsThis(lpWorkBuffer,ix,iy)&&whatIsThis(lpWorkBuffer,ix+1,iy) )  {
                      if(blacky==0) {
                        high++;
                        blacky=1;
                        }
                      }
                 if( !whatIsThis(lpWorkBuffer,ix-1,iy)&&!whatIsThis(lpWorkBuffer,ix,iy)&&!whatIsThis(lpWorkBuffer,ix+1,iy) )  {
                        blacky=0;
                        }
                 iy--;
                 }
             if( high>2 ) {
                return(1);
                }
             break;
         case 0: /* maximum */
             if( (yR>(lpWorkBuffer->avLineHigh)+1) )  return(1);
             if( (yL>(lpWorkBuffer->avLineHigh))&&(yR>(lpWorkBuffer->avLineHigh)) ) return(1);
             if( (botYL2>(lpWorkBuffer->curLine[indFr]).ym2+1)&&((lpWorkBuffer->mark[crL]).d_m>(lpWorkBuffer->curLine[indFr]).ym2+1) ) {
                return(1);
                }
                    mass=0;
                    xx=0;
                    iy=y+1;
                    for(high=0;high<6;high++) {
                       ix=x;
                       blackx=1;
                       while( (ix<x+(lpWorkBuffer->thWid)/2+10)&&blackx ) {
                            if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                              blackx=0;
                              continue;
                              }
                            ix++;
                            }
                       if(high>0) {
                         yy=ix-x-xx;
                         if(yy<0) yy=-yy;
                         if(mass<yy) mass=yy;
                         }
                       xx=ix-x;
                       iy++;
                       }
                    if(mass>3) return(1);
                    iy=y+4;
                    ix=x;
                    blackx=1;
                    yR=min(x+(lpWorkBuffer->thWid)/2+10,(lpWorkBuffer->traceR[1]));
                    while( (ix<yR)&&blackx ) {
                         if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                              blackx=0;
                              continue;
                              }
                         ix++;
                         }
                    xx=ix;
                    ix=x;
                    blackx=1;
                    yR=max(x-(lpWorkBuffer->thWid)/2-10,(lpWorkBuffer->traceL[1]));
                    while( (ix>yR)&&blackx ) {
                         if( whatIsThis(lpWorkBuffer,ix,iy) )  {
                              blackx=0;
                              continue;
                              }
                         ix--;
                         }
                    xx-=ix;
                    if((lpWorkBuffer->linkRM[0])<5) {
                        if((lpWorkBuffer->linkRM[0])==2) (lpWorkBuffer->linkRM[1])=x;
                        (lpWorkBuffer->linkRM[(lpWorkBuffer->linkRM[0])])=xx;
                        (lpWorkBuffer->linkRM[0])++;
                        }
                    if((lpWorkBuffer->linkRM[0])==5) {
                        xx=(lpWorkBuffer->linkRM[2])-(lpWorkBuffer->linkRM[3]);
                        if(xx<0) xx=-xx;
                        yy=(lpWorkBuffer->linkRM[3])-(lpWorkBuffer->linkRM[4]);
                        if(yy<0) yy=-yy;
                        (lpWorkBuffer->linkRM[0])=2;
                        if((xx>2)&&(yy<3)) return(2);
                        }

             ix=(x+(lpWorkBuffer->thWid)/2);
             high=0;
             iy=(lpWorkBuffer->mark[crL]).d_m+1;
             blacky=0;
             while( (iy>=(lpWorkBuffer->mark[crL]).g_m-1) ) {
                 if( whatIsThis(lpWorkBuffer,ix-1,iy)&&whatIsThis(lpWorkBuffer,ix,iy)&&whatIsThis(lpWorkBuffer,ix+1,iy) )  {
                      if(blacky==0) {
                        high++;
                        blacky=1;
                        }
                      }
                 else {
                      if( !whatIsThis(lpWorkBuffer,ix-1,iy)&&!whatIsThis(lpWorkBuffer,ix,iy)&&!whatIsThis(lpWorkBuffer,ix+1,iy) )  {
                          blacky=0;
                          }
                      }
                 iy--;
                 }
                if(high>2) {
                return(1);
                }
             break;
             }
      return(0);
} /* highTestOCR() */
//------------------------------------
int massRight(STRUCTWB *lpWorkBuffer, int crL){
  int ix, iy, mass, massY, indM, th1, j, len;
    mass=0;
    massY=0;
    th1=(lpWorkBuffer->avLineHigh)/3+2;
    len=min((lpWorkBuffer->lengthTraceC),(lpWorkBuffer->lengthTraceR));
    for(j=1;j<=len;j++) {
        iy=(lpWorkBuffer->mark[crL]).g_m+j-1;
        indM=0;
        for(ix=(lpWorkBuffer->traceC[j]);ix<(lpWorkBuffer->traceR[j]);ix++) {
            if( whatIsThis(lpWorkBuffer,ix,iy) ) {
                mass++;
                indM=1;
                }
            }
        if(indM>0) massY++;
        }
      if((massY<th1)||(mass<10) ) return(-1);
    mass=0;
    massY=0;
    len=min((lpWorkBuffer->lengthTraceL),(lpWorkBuffer->lengthTraceC));
    for(j=1;j<=len;j++) {
        iy=(lpWorkBuffer->mark[crL]).g_m+j-1;
        indM=0;
        for(ix=(lpWorkBuffer->traceL[j]);ix<(lpWorkBuffer->traceC[j]);ix++) {
            if( whatIsThis(lpWorkBuffer,ix,iy) ) {
                mass++;
                indM=1;
                }
            }
        if(indM>0) massY++;
        }
      if((massY<th1)||(mass<10) ) return(-1);
  return(1);
} /* of massRight() */

int testLineTop(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1)
{
  int x, y, c_gr, thT, thT1, flg_gr, rnx, flg_beg, flg_b;
  int xBeg, xEnd;

     thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
     thT1=max((dm-gm)/8,4);
     flg_beg=0;
     xBeg=xEnd=x1;
     rnx=min(x1+(dm-gm),(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]));
     for(x=x1;x<rnx;x++)  {
        y=gm;
        c_gr=0;
        flg_gr=0;
        flg_b=1;
        while( y<dm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                if( flg_gr || (y>((dm+2*gm)/3)) ) {
                        if(flg_beg) {
                            xEnd=x;
                            goto THRP;
                            }
                        else {
                            flg_b=0;
                            }
                        }
                c_gr++;
                }
              else {
                flg_gr=c_gr;
                }
              if(c_gr>thT)  {
                    if(flg_beg) {
                        xEnd=x;
                        goto THRP;
                        }
                    else {
                        flg_b=0;
                        }
                    }
              y++;
              }
              if( !flg_beg ) {
                  if( (x1-x)>thT1 ) {
                        xEnd=x1;
                        xBeg=x1;
                        goto THRP;
                        }
                  if(flg_b) {
                        flg_beg=1;
                        xBeg=x;
                        }
                  }
        }

THRP:
      if( (xEnd-xBeg)>=thT1 ) return((xEnd+xBeg)/2);

     return(-1);
} /* of testLineTop() */
//----------------------------------------------------
int testLineBott(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1)
{
  int x, y, c_gr, thT, thT1, flg_gr, rnx;
     thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
     thT1=x1+(dm-gm)/8;
     rnx=min(x1+(dm-gm),(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]));
     for(x=x1;x<rnx;x++)  {
        y=dm;
        c_gr=0;
        flg_gr=0;
        while( y>gm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                if( flg_gr || (y<((2*dm+gm)/3)) ) {
                        if(x>thT1) return((x1+x)/2);
                        return(-1);
                        }
                c_gr++;
                }
              else {
                flg_gr=c_gr;
                }
              if(c_gr>thT)  {
                    if(x>thT1) return((x1+x)/2);
                    return(-1);
                    }
              y--;
              }
        }
     return(-1);
} /* of testLineBott() */

int testLineMid(int ii, STRUCTWB *lpWorkBuffer, int dm, int gm, int x1, int ww){
  int x, y, c_gr, thT, thT1, flg_gr, i, xBeg, xEnd, thT2, flg_beg, flg_b;
  int xBegE, xEndB, rnx;

     thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
     thT1=max((dm-gm)/14,4);
     thT2=3;
     xBeg=x1;
     xBegE=x1;
     flg_beg=0;
     x=max(x1-(dm-gm),(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]));
     rnx=max(x,(lpWorkBuffer->traceL[1]));
     for(x=x1;x>rnx;x--)  {
        y=gm;
        if(ww) y=dm;
        c_gr=0;
        flg_gr=0;
        i=0;
        flg_b=1;
        while( i<(dm-gm+1) ) {
           if( whatIsThis(lpWorkBuffer,x,y) )  {
                if( (flg_gr || (y<((dm+2*gm)/3)) || (y>((2*dm+gm)/3))) ) {
                     if(flg_beg){
                        xBeg=x;
                        goto SECP;
                        }
                     else {
                        flg_b=0;
                        }
                     }
                c_gr++;
                }

           else {
                flg_gr=c_gr;
                }
              if(c_gr>thT)  {
                     if(flg_beg){
                        xBeg=x;
                        goto SECP;
                        }
                     else {
                        flg_b=0;
                        }
                     }
              if(ww) {
                y--;
                }
              else {
                y++;
                }
              i++;
              }
        if( !flg_beg ) {
                if( (x1-x)>thT1 ) {
                        xBeg=x1;
                        xBegE=x1;
                        goto SECP;
                        }
                if(flg_b) {
                        flg_beg=1;
                        xBegE=x;
                        }
                }
        }

      return(-1);
SECP: xEnd=x1;
      xEndB=x1;
      flg_beg=0;
      x=min(x1+(dm-gm),(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]));
      rnx=min(x,(lpWorkBuffer->traceR[1]));
      for(x=x1;x<rnx;x++)  {
        y=gm;
        if(ww) y=dm;
        c_gr=0;
        flg_gr=0;
        i=0;
        flg_b=1;
        while( i<(dm-gm+1) ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                if( flg_gr || (y<((dm+2*gm)/3)) || (y>((2*dm+gm)/3)) ) {
                        if(flg_beg){
                           xEnd=x;
                           goto THRP;
                           }
                        else {
                           flg_b=0;
                           }
                        }
                c_gr++;
                }
              else {
                flg_gr=c_gr;
                }
              if(c_gr>thT)  {
                     if(flg_beg){
                        xEnd=x;
                        goto THRP;
                        }
                     else {
                        flg_b=0;
                        }
                    }
              if(ww) {
                y--;
                }
              else {
                y++;
                }
              i++;
              }
              if( !flg_beg ) {
                  if( (x1-x)>thT1 ) {
                        xEnd=x1;
                        xEndB=x1;
                        goto THRP;
                        }
                  if(flg_b) {
                        flg_beg=1;
                        xEndB=x;
                        }
                  }
        }
        return(-1);
THRP:
        if(xBegE==xEndB) {
             x=xBeg;
             y=xEnd;
             }
        else {
             if( (xBegE-xBeg)>(xEnd-xEndB) ){
                  x=xBeg;
                  y=xBegE;
                  }
             else {
                  x=xEndB;
                  y=xEnd;
                  }
              }
      if( (y-x)>=thT1 ) return((y+x)/2);
      return(-1);
} /* of testLineMid() */
//------------------------------------------------
int maxminBottTop(STRUCTWB *lpWorkBuffer, int *xMn, int *xMx){
 int i, Bln, Blx;

  *xMn=2*MAXSIZE;
  *xMx=0;
  Bln=Blx=0;
  for(i=0;i<indBottTop;i++) {
        if(bottTopEndTr[i].x<(*xMn)) {
                *xMn=bottTopEndTr[i].x;
                Bln=1;
                }
        if(bottTopEndTr[i].x>(*xMx)) {
                *xMx=bottTopEndTr[i].x;
                Blx=1;
                }
        }
  if( Bln&&Blx ) return(1);
  return(-1);
} /* of maxminBottTop() */
//---------------------------------------------
int maxGlob(STRUCTWB *lpWorkBuffer, int gm, int range, int xMn, int xMx){
 int x, y, mn, xmn;
      mn=gm;
      xmn=-1;
      for(x=xMn;x<xMx;x++) {
        y=gm;
        while( !whatIsThis(lpWorkBuffer,x,y) ) {
                y++;
                if(y>range) return(-1);
                }
        if(y>mn) {
                mn=y;
                xmn=x;
                }
        }
      return(xmn);
} /* of maxGlob() */
//---------------------------------------
int lastTestMin(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1){
  int x, y, c_gr, thT, rnx;

     thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
     rnx=max(x1-(dm-gm),(lpWorkBuffer->traceL[1]));
     c_gr=0;
     for(x=x1;x>rnx;x--)  {
        y=(dm+gm)/2;
        while( y>gm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) goto NXT;
              y--;
              }
        }
     return(-1);
NXT:
     rnx=min(x1+(dm-gm),(lpWorkBuffer->traceR[1]));
     c_gr=0;
     for(x=x1;x<rnx;x++)  {
        y=(dm+gm)/2;
        while( y>gm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) return(0);
              y--;
              }
        }
     return(-1);
  }/* of lastTestMin() */
//----------------------------------------------
int lastTestMax(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1){
  int x, y, c_gr, thT, rnx;
     thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
     rnx=max(x1-(dm-gm),(lpWorkBuffer->traceL[(lpWorkBuffer->lengthTraceL)]));
     c_gr=0;
     for(x=x1;x>rnx;x--)  {
        y=(dm+gm)/2;
        while( y<dm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) goto NXT;
              y++;
              }
        }
     return(-1);
NXT:
     rnx=min(x1+(dm-gm),(lpWorkBuffer->traceR[(lpWorkBuffer->lengthTraceR)]));
     c_gr=0;
     for(x=x1;x<rnx;x++)  {
        y=(dm+gm)/2;
        while( y<dm ) {
              if( whatIsThis(lpWorkBuffer,x,y) )  {
                   c_gr++;
                }
              if(c_gr>thT) return(0);
              y++;
              }
        }
     return(-1);
 } /* of lastTestMax() */
//---------------------------------------------------------------------------
int minGlob(STRUCTWB *lpWorkBuffer, int dm, int range, int xMn, int xMx)
{
 int x, y, mn, xmn;
      mn=dm;
      xmn=-1;
      for(x=xMn;x<xMx;x++) {
        y=dm;
        while( !whatIsThis(lpWorkBuffer,x,y) ) {
                y--;
                if(y<range) return(-1);
                }
        if(y<mn) {
                mn=y;
                xmn=x;
                }
        }
      return(xmn);
} /* of minGlob() */
//--------------------------------------------------
int maxminTopBott(STRUCTWB *lpWorkBuffer, int *xMn, int *xMx){
 int i, Bln, Blx;

  *xMn=2*MAXSIZE;
  *xMx=0;
  Bln=Blx=0;
  for(i=0;i<indTopBott;i++) {
        if(topBottEndTr[i].x<(*xMn)) {
                *xMn=topBottEndTr[i].x;
                Bln=1;
                }
        if(topBottEndTr[i].x>(*xMx)) {
                *xMx=topBottEndTr[i].x;
                Blx=1;
                }
        }

  if( Bln&&Blx ) return(1);
  return(-1);
} /* of maxminTopBott() */
//---------------------------------------------
int searchTrMinC(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn, int rx){
  int flg_l, flg_gray, count_g, markInd, rm, lm, indTr, thT;
  int flg_rg, nrTr = 0, yy, flg_rgN;

              flg_rg=0;
              flg_rgN=0;
              yy=yn;
              thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
              markInd=(lpWorkBuffer->curMark1);
              (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
              indTr=(lpWorkBuffer->lengthTraceC);
              flg_l=0;
              flg_gray=count_g=0;
              do {
                if( whatIsThis(lpWorkBuffer,xn,yn+1) ) {
                  if( (count_g<thT) && (!flg_gray) && (yn<rx) ) {
                        count_g++;
                        yn++;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        flg_l=0;
                   } else {
                     flg_gray=1;

                     if(!flg_l) {
                     if(whatIsThis(lpWorkBuffer,xn-1,yn)||(xn<((lpWorkBuffer->traceL[min(indTr,(lpWorkBuffer->lengthTraceL))])+4+(lpWorkBuffer->boundLeft)))) {
                        flg_l=1;
                        xn=(lpWorkBuffer->traceC[indTr]);
                        } else {
                            xn--;
                            lm=(lpWorkBuffer->mark[markInd]).l_m;
                            if(lm<0) lm=-lm;
                            if(xn<lm)  {
                                if( (markInd-1)>=0 ) {
                                        markInd--;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                }

                            }
                       } else {
//                         yy=yy;
                    	   ;
AAA:
                         if( whatIsThis(lpWorkBuffer,xn+1,yn)||(xn>((lpWorkBuffer->traceR[min(indTr,(lpWorkBuffer->lengthTraceR))])-4))) {
                              if((flg_rgN==0)&&(indTr>6)&&(xn<=((lpWorkBuffer->traceR[min(indTr,(lpWorkBuffer->lengthTraceR))])-4))) {
                                   if(flg_rg==0) {
                                        nrTr=(indTr*2)/3+1;
                                        }
                                   if(flg_rg<6) {
                                        flg_rg++;
                                        flg_rgN++;
                                        indTr=nrTr;
                                        xn=(lpWorkBuffer->traceC[indTr]);
                                        (lpWorkBuffer->traceC[indTr])++;
                                        yn=yy+indTr-1;
                                        goto AAA;
                                        }
                                   }
                              (lpWorkBuffer->lengthTraceC)=indTr;
                              (lpWorkBuffer->curMark1)=markInd;
                              return(1);
                              } else {
                                xn++;
                                rm=(lpWorkBuffer->mark[markInd]).r_m;
                                if(rm<0) rm=-rm;
                                if(xn>rm)  {
                                   if((markInd+1)<(lpWorkBuffer->markNumber) ) {
                                        markInd++;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                   else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                   }

                                }
                         }
                     }

                  } else {
                        yn++;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        if(count_g>0) {flg_gray=1;}
                        flg_l=0;
                        flg_rgN=0;
                        }
               } while((yn<=(lpWorkBuffer->mark[markInd]).d_m)&&((indTr+1)<MAXLTRACE));
        (lpWorkBuffer->curMark1)=markInd;
        indTr = min(indTr,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-(lpWorkBuffer->traceC[0]));
        (lpWorkBuffer->lengthTraceC)=indTr;
        return(0);
} /* of searchTrMinC() */
//-------------------------------------------------
int searchTrMaxC(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn, int rn){
  int flg_r, flg_gray, count_g, markInd, rm, lm, indTr, thT;
              thT=(6*(lpWorkBuffer->Dpi).y+4)/200;
              markInd=(lpWorkBuffer->curMark1);
              (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
              indTr=(lpWorkBuffer->lengthTraceC);
              flg_r=0;
              flg_gray=count_g=0;
              do {
                if(whatIsThis(lpWorkBuffer,xn,(yn-1))) {
                  if( (count_g<thT) && (!flg_gray) && (yn>rn) ) {
                        count_g++;
                        yn--;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        flg_r=0;
                   } else {
                     flg_gray=1;

                     if(!flg_r) {
                     if(whatIsThis(lpWorkBuffer,xn+1,yn)||(xn>((lpWorkBuffer->traceR[max((lpWorkBuffer->lengthTraceR)-indTr+1,1)])-4))) {
                        flg_r=1;
                        xn=(lpWorkBuffer->traceC[indTr]);
                        } else {
                            xn++;
                            rm=(lpWorkBuffer->mark[markInd]).r_m;
                            if(rm<0) rm=-rm;
                            if(xn>rm)  {
                                if( (markInd+1)<(lpWorkBuffer->markNumber) ) {
                                        markInd++;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                }

                            }
                       } else {
                         if(whatIsThis(lpWorkBuffer,xn-1,yn)||(xn<((lpWorkBuffer->traceL[max((lpWorkBuffer->lengthTraceL)-indTr+1,1)])+4+(lpWorkBuffer->boundLeft)))) {
                              (lpWorkBuffer->lengthTraceC)=indTr;
                              (lpWorkBuffer->curMark1)=markInd;
                              return(1);
                              } else {
                                xn--;
                                lm=(lpWorkBuffer->mark[markInd]).l_m;
                                if(lm<0) lm=-lm;
                                if(xn<lm)  {
                                   if( (markInd-1)>=0 ) {
                                        markInd--;
                                        (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[markInd]).g_m;
                                        }
                                   else {
                                     (lpWorkBuffer->lengthTraceC)=indTr;
                                     (lpWorkBuffer->curMark1)=markInd;
                                     return(1);
                                     }
                                   }

                                }
                         }
                     }

                  } else {
                        yn--;
                        (lpWorkBuffer->traceC[indTr+1])=xn;
                        indTr++;
                        if(count_g>0) {flg_gray=1;}
                        flg_r=0;
                        }
               } while((yn>=(lpWorkBuffer->mark[markInd]).g_m)&&((indTr+1)<MAXLTRACE));
        (lpWorkBuffer->curMark1)=markInd;
        indTr = min(indTr,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-(lpWorkBuffer->traceC[0]));
        (lpWorkBuffer->lengthTraceC)=indTr;
        return(0);
} /* of searchTrMaxC() */
