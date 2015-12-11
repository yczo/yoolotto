/*
 * UtilRecognition.c
 *
 *  Created on: Aug 31, 2012
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//#include  "../headers/Image.h"
#include  "../headers/Functions.h"
#include  "../headers/UtilRecognition.h"
#include "../headers/memmgr.h"
#include "../headers/util.h"
//#include "../headers/BarCode.h"

void histHTest_X1_X1(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2, int *hHist);
extern int hHistPreProc[H_SIZE_PREPROC];
extern int US_State;
void selectOneTopDateLine(STRUCTWB *lpWorkBuffer, int idF, int sizeWImg, int sizeHImg);
int ticketTypeInt;

void replaceField(STRUCTWB *lpWorkBuffer, LPBYTE *fieldBMPPtr);
int correctTrapezoid(STRUCTWB *lpWorkBuffer, LPBYTE *fieldBMPPtr, char* imgdName);
int SegmCharsNumbers_2(STRUCTWB *lpWorkBuffer, int kindD, int *lpLwidth, char *fieldName, int sizeW);

int  initFieldR(
	 char* imgdName,
	 char* fieldName,
	 int sizeWImg,
	 int sizeHImg,
	 LPBYTE *fieldBMPPtr,
	 LPIMAGEL  lpDataIm,     /* image structure */
     int        iTypeField,    /* filed type */
   int         iCharType,    /* type of expected characters */
   int         iSegmType,    /* segmentation type                */

   PREPROCINFO *lpPreprocInfo, /* structure for frames and lines
                                      removal functions (old staff) */
   TEXTLINE    *lpTextLine,    /* structure for text lines
                                      recognition                   */
   int             HLRecog,        /* horizontal lines recognition
                                      switch
                                      0 - off
                                      1 - on                        */
   int             VLRecog,        /* vertical lines recognition
                                      switch
                                      0 - off
                                      1 - on                        */
   SEGMENT     *lpSegment,     /* structure for segmentation    */

   SPECK       *lpSpeck,       /* structure for specks removal  */
   STRUCTWB    *lpWorkBuffer,  /* pointer to work area -
                                      at least 10Kbytes             */
// ??????????????????????????????????????????????????????
   int      smSignPer,             /* area percentage to ignore
                                      small signs                   */
// ??????????????????????????????????????????????????????
   RECCHARSTRUCT  *lpRecCharStruct, /* structure for character
                                           recognition              */
   LPBYTE   lpAlphaMapA
   )

{

int frWidth, i, wdt, wdtH, countL, ret, rAdd;
BYTE *lpPointer;
int wLineWidth, wImgHeight, wXBegin, wYBegin, wWidth, wHeight;
//int *lWdt;
int frameWMax;
float alphH;
SREMOVAL *lpData;
SREMOVAL Data;
int cellLineR;
int frWWT;
int lRecalc;
int horLineM;
int smallSSt;
int anyLines;
int lHAndVertLrem;

//char buffLog[500];


//printf("1------------ initFieldR; smSignPer= %d \n", smSignPer);
//printf("inside initFieldR; lpDataIm->lpPointer= %lu\n",(unsigned long)lpDataIm->lpPointer);
//fflush(stdout);


   lpAlphaMap = lpAlphaMapA;
   if((lpTextLine->iVer) == 2)  (lpTextLine->iVer) = 11;
   if((lpSegment->iVer)  == 2)  (lpSegment->iVer)  = 11;


    if(smSignPer < 0)
                smSignPer = 0;

    if(smSignPer > 100)
                smSignPer = 100;

    lpData=(SREMOVAL *)&Data;


    lpData->lpPointer   = lpDataIm->lpPointer;
    lpData->iLineWidth  = lpDataIm->iLineWidth;
    lpData->iImgHeight  = lpDataIm->iImgHeight;
    lpData->iXBegin     = lpDataIm->iXBegin;
    lpData->iYBegin     = lpDataIm->iYBegin;
    lpData->iWidth      = lpDataIm->iWidth;
    lpData->iHeight     = lpDataIm->iHeight;
    lpData->iMinLH      = lpPreprocInfo->iMinLH;
    lpData->iMinLV      = lpPreprocInfo->iMinLV;
    lpData->iSpeckSizeH = lpSpeck->iHSize;
    lpData->iSpeckSizeV = lpSpeck->iVSize;

   lHAndVertLrem = 0;
   horLineM = 0;
   anyLines = 0;
   smallSSt = 1;


    lRecalc = 0;
    initRemStruct(lpData);

    frWWT = FRAMEWIDTH;
    if( ((lpTextLine->iVer) != 3) && ((lpTextLine->iVer) != 13) ) {
         frWWT = 300;
        }

    lpPointer  = lpData->lpPointer;
    wLineWidth = lpData->iLineWidth;
    wImgHeight = lpData->iImgHeight;
    wXBegin    = lpData->iXBegin;
    wYBegin    = lpData->iYBegin;
    wWidth     = lpData->iWidth;
    wHeight    = lpData->iHeight;


    cellLineR = 0;

     lpData->iRemoval      = iSegmType;


/* check size of rectangle */
//     printf("wImgHeight = %d, wLineWidth= %d \n",wImgHeight, wLineWidth);
//     printf("wYBegin = %d, wHeight= %d, wWidth= %d \n",wYBegin, wHeight, wWidth);
//     fflush(stdout);


//    printf(" -------=== lpWorkBuffer -> Width=%d \n",lpWorkBuffer -> Width);


    if( (wXBegin+wWidth>=wLineWidth) ) wWidth=wLineWidth-wXBegin-1;
    if( (wImgHeight<MINSIZE) || (wLineWidth<MINSIZE) ) return(SIZE_ERR1);
    if( (wYBegin+wHeight>=wImgHeight) ) wHeight=wImgHeight-wYBegin-1;
    if( (wHeight<MINSIZE) || (wWidth<MINSIZE) ) return(SIZE_ERR1);
    if( (wHeight>MAXSIZE) || (wWidth>MAXSIZE) ) return(SIZE_ERR3);


 /* save data */

    memset(lpWorkBuffer,0x00,(unsigned int)sizeof(STRUCTWB));

//    printf(" -------=== wWidth=%d, wXBegin= %d, wLineWidth= %d \n",wWidth,wXBegin,wLineWidth);

    (lpWorkBuffer -> lpRBuffer) =lpPointer;
    (lpWorkBuffer -> widthImg)  =((wLineWidth+31)/32)*4;
    (lpWorkBuffer -> hightImg)  = wImgHeight;
    (lpWorkBuffer -> begRect.x) = wXBegin;
    (lpWorkBuffer -> begRect.y) = wYBegin;
    (lpWorkBuffer -> Height)    = wHeight;
    (lpWorkBuffer -> Width)     = wWidth;
    (lpWorkBuffer -> OffH)      = 0;
    (lpWorkBuffer -> OffW)      = 0;
    (lpWorkBuffer -> Dpi.x)     = lpDataIm->iDpiX;
    (lpWorkBuffer -> Dpi.y)     = lpDataIm->iDpiY;
    (lpWorkBuffer -> newLine)   = 1;
    (lpWorkBuffer -> newSegm)   = 1;
    (lpWorkBuffer -> flgTraceR) = 0;
    (lpWorkBuffer -> blank)     = 1;
    (lpWorkBuffer -> flgCut)    = 0;
    (lpWorkBuffer -> fieldType) = iTypeField;
    (lpWorkBuffer -> segmType)  = iSegmType;
    (lpWorkBuffer->numbOfChar)  = wXBegin;
    (lpWorkBuffer->flgUNP)      = 0;
    (lpWorkBuffer->flgItalic)   = 0;
    (lpWorkBuffer->flgSpSegm)   = 0;
    (lpWorkBuffer->realWidth)   = wLineWidth ;
    (lpWorkBuffer->chType)      = iCharType;
    (lpWorkBuffer->horLineMM)   = 0;
    lpWorkBuffer -> txtLineVer  = lpTextLine->iVer;
    lpWorkBuffer -> txtLineHight = lpTextLine->iLineHeight;


    lpWorkBuffer ->nOffCut600    = 0;
    (lpWorkBuffer->smallSignPer)= smSignPer;
    (lpWorkBuffer->recChar) = lpRecCharStruct;

    (lpWorkBuffer -> heightForVert) = 0;
    lpWorkBuffer->smallSignStrat = smallSSt;


//    printf("1------------ lpSegment->iVer= %d \n", lpSegment->iVer);


   (lpWorkBuffer->hndOrOCR) = 0;

   {
        (lpWorkBuffer->  Method)  = 1;
        if(lpSegment->iVer == 1) {
           if(lpSegment->iCellWidth==998) { (lpWorkBuffer->flgUNP)=4;  lpSegment->iCellWidth = 0;/* (998) proportional pitch for OCR*/ }
           else {
                  if(lpSegment->iCellInfo==0) {
                     if(lpSegment->iCellWidth==0) {(lpWorkBuffer->flgUNP)=1; /* unknown cell width for unproportional pitch */ }
                     else                        {(lpWorkBuffer->flgUNP)=5; /* estimated cell width for unproportional pitch */ }
                     }
                  else {
                     (lpWorkBuffer->flgUNP)=3; /* known cell width for unproportional pitch */
                     }
//                  }
                }
           }

        else  {
               if(lpSegment->iVer == 2) {  (lpWorkBuffer->flgUNP)=2;  /* (998) proportional pitch for OCR */ }
                }

        }

//     if((strcmp(fieldName, "DATEN") == 0 ||
//   	    strcmp(fieldName, "RET_NUMN") == 0)){
//               printf("1. fieldName= %s, lpWorkBuffer->flgUNP= %d, lpWorkBuffer->  Method= %d\n", fieldName, lpWorkBuffer->flgUNP, lpWorkBuffer->  Method);
//               fflush(stdout);
//                 }


    lpWorkBuffer->horLineMM = 0;
    if(horLineM == 5)
           lpWorkBuffer->horLineMM = 1;
    if(horLineM == 6)
           lpWorkBuffer->horLineMM = 4;
    if(horLineM == 8)
           lpWorkBuffer->horLineMM = 2;
    if(horLineM == 9)
           lpWorkBuffer->horLineMM = 3;
    if(horLineM >= 10)
           lpWorkBuffer->horLineMM = horLineM;


    (lpWorkBuffer->  lettWidth) = lpSegment->iCellWidth;
    (lpWorkBuffer->  lettWidthG) =lpSegment->iCellWidth;
    (lpWorkBuffer->  CellW)     =lpSegment->iCellWidth;


/* find top and bottom lines */
    (lpWorkBuffer ->newLeftTopP).x    =wXBegin;
    (lpWorkBuffer ->newLeftTopP).y    =wYBegin;
    (lpWorkBuffer ->newLeftBottomP).x =wXBegin;
    (lpWorkBuffer ->newLeftBottomP).y =wYBegin+wHeight-1;
    (lpWorkBuffer ->newRightTopP).x   =wXBegin+wWidth-1;
    (lpWorkBuffer ->newRightTopP).y   =wYBegin;
    (lpWorkBuffer ->newRightBottomP).x=wXBegin+wWidth-1;
    (lpWorkBuffer ->newRightBottomP).y=wYBegin+wHeight-1;


    (lpWorkBuffer->lenLeftC)=0;
    if( ((lpWorkBuffer->flgSpSegm) == 0) && (lpWorkBuffer -> segmType != 55)) {
      if(((iTypeField == MULTILINEBOXD)||
         (iTypeField == DROPBOX))&&((lpWorkBuffer->lettWidth)!=0)) {
         searchCloseP(lpData,(POINT *)&(lpWorkBuffer ->newLeftTopP),(POINT *)&(lpWorkBuffer ->newLeftBottomP),(POINT *)&(lpWorkBuffer ->newRightTopP),(POINT *)&(lpWorkBuffer ->newRightBottomP),0);
         }
      }

      if(anyLines) {
         cleanLines(lpData);
      }

    if((iTypeField ==  MULTILINEBOXD)) {
        if( (lpWorkBuffer->flgSpSegm) == 0) {
          wXBegin=(lpWorkBuffer ->newLeftTopP).x;
          wWidth=max((lpWorkBuffer ->newRightTopP).x-(lpWorkBuffer ->newLeftTopP).x,(lpWorkBuffer ->newRightBottomP).x-(lpWorkBuffer ->newLeftBottomP).x);

          (lpWorkBuffer -> begRect.x)=wXBegin;
          (lpWorkBuffer -> Width)=wWidth;
          }
        (lpWorkBuffer->numbOfChar)=wXBegin;
        ret=0;
        }



/* calculate number of frames */

    frameWMax=frWWT;
    if((lpTextLine->iVer)==1) {frameWMax=5000;}
    else {
        if((lpTextLine->iVer)==0) {
            (lpTextLine->iLineHeight)=0;
            }
        }

    wdt=wWidth;
//    frWidth=(int)((float)frameWMax*((float)(lpWorkBuffer -> Dpi.x)/(float)200.0));

    frWidth=(int)((float)frameWMax*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
    i=1;
    while( (i<MAXFRAMES) && ((wdt/i)>frWidth) ) i++;
    if(i>1) i--;
    (lpWorkBuffer -> frNumber) =i;

//    lpWorkBuffer -> frNumber = 1;
    wdtH=wdt/i;
    rAdd=wdt%i;
    if(rAdd<2) rAdd=0;

/*
    if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0){
//       frameWMax=frWWT;
       lpWorkBuffer -> frNumber = 4;
       wdtH=wdt/4;
       rAdd=0;
    }
*/
/* calculate lines for each frames */
// printf("------------ fieldName= %s, lpWorkBuffer -> frNumber= %d \n", fieldName, lpWorkBuffer -> frNumber);
//fflush(stdout);

    if(trapezoidVer == 1 && US_State == 2 && strcmp(fieldName, "NUMBERS") == 0) {
         if(correctTrapezoid(lpWorkBuffer, fieldBMPPtr, imgdName) < 0) {
        	trapezoidVer = 0;
            return(TRAPEZOID_ERR);
        }
	    lpData->lpPointer   = lpWorkBuffer -> lpRBuffer;
	    lpData->iLineWidth  = lpWorkBuffer -> Width;
	    lpData->iImgHeight  = lpWorkBuffer -> hightImg;
	    lpData->iWidth      = lpWorkBuffer -> Width;
	    lpData->iHeight     = lpWorkBuffer -> hightImg;

    }

    {//int tryAgain = 0, countAgain = 0;
//    for(countAgain=0;countAgain<2;countAgain++) {
//        for(countAgain=0;countAgain<1;countAgain++) {
/*
    	if(tryAgain == 1){
    		replaceField(lpWorkBuffer, fieldBMPPtr);

    	    lpData->lpPointer   = lpWorkBuffer -> lpRBuffer;
    	    lpData->iLineWidth  = lpWorkBuffer -> Width;
    	    lpData->iImgHeight  = lpWorkBuffer -> hightImg;
    	    lpData->iWidth      = lpWorkBuffer -> Width;
    	    lpData->iHeight     = lpWorkBuffer -> hightImg;
    	}
*/
    countL=0;
    for(i=0;i<(lpWorkBuffer -> frNumber);i++)  {
        (lpWorkBuffer->infFrame)[i].x1=wXBegin+i*wdtH;
        (lpWorkBuffer->infFrame)[i].x2=wXBegin+(i+1)*wdtH;


        if(i==(lpWorkBuffer -> frNumber)-1) (lpWorkBuffer->infFrame)[i].x2+=rAdd;

//        if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0){
//      printf("---------------i = %d, x1= %d, x2= %d\n",i, (lpWorkBuffer->infFrame)[i].x1, (lpWorkBuffer->infFrame)[i].x2);
//        }

//printf("3------------ calculate lines for each frames, frameWMax= %d frWWT= %d \n", frameWMax, frWWT);

        if(frameWMax==frWWT) {
           hist_h(lpWorkBuffer,wXBegin+i*wdtH,wYBegin,wXBegin+(i+1)*wdtH,wYBegin+wHeight);
           }


        if(frameWMax!=frWWT) {
        if((lpWorkBuffer->infFrame)[(lpWorkBuffer -> frNumber)-1].x2>=(int)(wXBegin+wWidth)) {
                (lpWorkBuffer->infFrame)[(lpWorkBuffer -> frNumber)-1].x2=wXBegin+wWidth-1;
                }


              if(HLRecog || VLRecog) {
//                 alphH=(float)0.2;
                 alphH=(float)0.4;
                 }
              else {
                 alphH=(float)0.0;
                 }

            	  lpData->iSpeckSizeH = 4;
            	  lpData->iSpeckSizeV = 4;

            	  if(US_State == 2)
            		  cleanSpecksOut(lpData);
            	  else {
//            if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 ||
//     		   strcmp(fieldName, "RET_NUMA") == 0 || strcmp(fieldName, "RET_NUMN") == 0)){
//               printf("cleanSpecks; fieldName= %s, maxLH= %d, horLineMM= %d, iSpeckSizeH= %d, iSpeckSizeV= %d\n", fieldName, lpWorkBuffer->maxLH, lpWorkBuffer->horLineMM, lpData->iSpeckSizeH, lpData->iSpeckSizeV);
//              printf("cleanSpecks; fieldName= %s, iVer= %d, iLineHeight= %d, HLRecog= %d, VLRecog= %d, alphH = %f\n", fieldName, lpTextLine->iVer, lpTextLine->iLineHeight, HLRecog, VLRecog, alphH);
//              }
                      cleanSpecks(lpData);
            	  }




          	  lpData->iSpeckSizeH = 0;
          	  lpData->iSpeckSizeV = 0;

// if(strcmp(fieldName, "NUMBERS") == 0){
//printf("4------------ alphH= %e skewHist= %d \n", alphH, skewHist);
//printf("4------------ lpData->iSpeckSizeH= %d lpData->iSpeckSizeV= %d \n", lpData->iSpeckSizeH, lpData->iSpeckSizeV);
  //}
//              lpWorkBuffer->begRect.x += 500;


              if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0){
//                  printf("------------histHSkewedFrame; countAgain= %d\n", countAgain);
//                  fflush(stdout);

                  histHSkewedFrame(lpWorkBuffer,alphH,(float)0.1,(int *)hntHist,(int *)&hntHist[0],(LONG *)&angleH,(int *)&scAngleH, i);
              }
              else
              histHSkewed(lpWorkBuffer,alphH,(float)0.1,(int *)hntHist,(int *)&hntHist[0],(LONG *)&angleH,(int *)&scAngleH);


//              printf("------------ angleH= %ld scAngleH= %d \n", angleH, scAngleH);

            lpWorkBuffer->horLineMM = 0;
            if(horLineM == 5)
                    lpWorkBuffer->horLineMM = 1;
            if(horLineM == 8)
                    lpWorkBuffer->horLineMM = 2;
            if(horLineM == 9)
                    lpWorkBuffer->horLineMM = 3;
            if(horLineM >= 10)
                    lpWorkBuffer->horLineMM = horLineM;

//printf("4------------ linesForFonts, HLRecog= %d horLineM= %d \n", HLRecog, horLineM);


            if(strcmp(fieldName, "DATEN1") == 0 || strcmp(fieldName, "DATEA1") == 0){
            	lpTextLine->iLineHeight = sizeHImg/26;
            }

//       if(strcmp(fieldName, "DATEA") == 0 )
//          	 printf("-----------  fieldName: %s,  lpWorkBuffer->horLineMM= %d \n", fieldName,lpWorkBuffer->horLineMM);

                     if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0){

                         linesForFontsFrame((int *)hntHist, hntHist[0], lpWorkBuffer, lpTextLine->iLineHeight, angleH, scAngleH, (int)iCharType, HLRecog, VLRecog, horLineM, i);
//                        saveLinesTest2(fieldBufferBMP, (STRUCTWB *)lpWorkBuffer, fieldName);
                     }
                     else{
            	    if(strcmp(fieldName, "RET_NUMN") == 0 || strcmp(fieldName, "RET_NUMA") == 0 ){
//               	    if(strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "DATEA") == 0 )
                        linesForFontsRet_Num((int *)hntHist, hntHist[0], lpWorkBuffer, lpTextLine->iLineHeight, angleH, scAngleH, (int)iCharType, HLRecog, VLRecog, horLineM);
            	    }
            	    else
                       linesForFonts((int *)hntHist, hntHist[0], lpWorkBuffer, lpTextLine->iLineHeight, angleH, scAngleH, (int)iCharType, HLRecog, VLRecog, horLineM);
                     }


// if(strcmp(fieldName, "DATEA") == 0 ){
//   	 printf("-----------  lpWorkBuffer->infFrame[0]).nLines= %d \n", (lpWorkBuffer->infFrame[0]).nLines);
// }

            if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0){
/*
            	 {int kkk, ddk;
            	   for(kkk= 0;kkk<(lpWorkBuffer->infFrame[i]).nLines;kkk++){
            		   ddk = lpWorkBuffer->line[i].y2[kkk]-lpWorkBuffer->line[i].y1[kkk];
            		   if(ddk > 80 && countAgain == 0){
            			   if(kkk > 0 || lpWorkBuffer->line[i].y1[kkk] > 50) {
            			     tryAgain = 1;
//            	      	 printf("---------tryAgain; kkk = %d, y2-y1= %d, y1= %d,  countAgain = %d\n",kkk, ddk, lpWorkBuffer->line[i].y1[kkk], countAgain);
            	      	   break;
            			   }

            		   }
            	   }
           	    }
*/

//            	 printf("------------  lpWorkBuffer->infFrame[0]).nLines= %d \n", (lpWorkBuffer->infFrame[0]).nLines);

//            	if(tryAgain == 0)
       	      switch(US_State){
    	         case 0: // TX
       	         case 1: // TX
         	      switch(ticketTypeInt){
      		         case 1: // POWER BALL
         		     case 2: // MEGA MILLIONS
         	         case 3: // LOTTO TEXAS
         		     case 4: // TWO STEP
         		     case 7: // CASH FIVE
         		     case 8: // ALL OR NOTHING
                	   break;
               	     default:
                	   secondTestForLine(lpWorkBuffer, 0, sizeWImg);
         	           break;
         	     }
         	     break;
   	            case 2: // CA
         	      switch(ticketTypeInt){
         	      case 6: // Hot Spot from CA
         	      case 7: // DAILY DERBY CA
                	   break;
               	   default:
                	secondTestForLine(lpWorkBuffer, 0, sizeWImg);
         	      break;
         	     }
         	     break;
   	            case 3: // NY
   	            	break;
         	     default:
         	      secondTestForLine(lpWorkBuffer, 0, sizeWImg);
          	      break;
              }

//       	 printf("2-----------  lpWorkBuffer->infFrame[0]).nLines= %d \n", (lpWorkBuffer->infFrame[0]).nLines);

//              if(US_State != 2 || ticketTypeInt != 6) // Hot Spot from CA
//                     secondTestForLine(lpWorkBuffer, 0, sizeWImg);
//          	 printf("2-----------  lpWorkBuffer->infFrame[0]).nLines= %d \n", (lpWorkBuffer->infFrame[0]).nLines);
            }


           if(strcmp(fieldName, "DATEN1") == 0 || strcmp(fieldName, "DATEA1") == 0){
   	           	   selectOneTopDateLine(lpWorkBuffer, 0, sizeWImg, sizeHImg);
           }


            }

//        if(strcmp(fieldName, "NUMBERS") == 0){
//           printf("18 line x1= %d, x2= %d\n", (lpWorkBuffer->infFrame)[0].x1, (lpWorkBuffer->infFrame)[0].x2);
//           printf("18 line y1= %d, y2= %d\n", (lpWorkBuffer->line[0]).y1[18], (lpWorkBuffer->line[0]).y2[18]);
//        }


        if((lpWorkBuffer->infFrame[i]).nLines>0) {
//   	printf("8------------ maxLineH, i= %d \n", i);
                     maxLineH(lpWorkBuffer,i,(int *)hntHist,hntHist[0]);
                     countL++;
                     }
//printf("1------------ tryAgain= %d, countAgain= %d \n", tryAgain, countAgain);
//        if(tryAgain == 1) break;
        }
//   if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0)
//        printf("2------------ tryAgain= %d, countAgain= %d \n", tryAgain, countAgain);
//       if(tryAgain == 0) break;
//       } //countAgain
     }

     if(countL == 0) {
           return(LINE_ERR1);
        }

     i=1;
//     lpWorkBuffer->maxLH = (int)(((float)(lpWorkBuffer->maxLH)*(float)30)/(float)100);
     {int bigFlag = 0;int nnn = 1000;
     if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
  		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) bigFlag = 1;
//     if((US_State == 2 || US_State == 3) && (strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0)) bigFlag = 1;
     if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 ||
    		 strcmp(fieldName, "RET_NUMA") == 0 || strcmp(fieldName, "RET_NUMN") == 0)) bigFlag = 1;

     if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
  		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) {

        if(lpWorkBuffer->infFrame[0].nLines > 0)
		    nnn = lpWorkBuffer->hightImg/lpWorkBuffer->infFrame[0].nLines;

 //printf("------ nnn= %d, lpWorkBuffer->hightImg= %d, lpWorkBuffer->infFrame[0].nLines= %d \n", nnn, lpWorkBuffer->hightImg, lpWorkBuffer->infFrame[0].nLines);
        if(nnn < 60 && lpWorkBuffer->infFrame[0].nLines > 16) {bigFlag = 0;lpWorkBuffer->nOffCut600 = 77;}
     }

       if(bigFlag == 1)
        lpWorkBuffer->maxLH = 20;
       else
   	     lpWorkBuffer->maxLH = 10;
     }

//   printf("13------------ lpWorkBuffer->hndOrOCR= %d \n", lpWorkBuffer->hndOrOCR);


//     if(strcmp(fieldName, "DATEA") == 0) {
//        printf("------------ lpWorkBuffer->maxLH= %d \n", lpWorkBuffer->maxLH);
//     }

       if((lpWorkBuffer->hndOrOCR) == 1 ) {
    	   return 0;
//           return(ourNextForm(NULL,lpPointer,lpWorkBuffer,(int *)&lWdt,(LPBYTE)NULL,0,0,0,1));
        }

//   if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 ||
//		   strcmp(fieldName, "RET_NUMA") == 0 || strcmp(fieldName, "RET_NUMN") == 0)){

//  printf("fieldName= %s, maxLH= %d, horLineMM= %d, iSpeckSizeH= %d, iSpeckSizeV= %d\n", fieldName, lpWorkBuffer->maxLH, lpWorkBuffer->horLineMM, lpData->iSpeckSizeH, lpData->iSpeckSizeV);
//  printf("fieldName= %s, iVer= %d, iLineHeight= %d, HLRecog= %d, VLRecog= %d, alphH = %f\n", fieldName, lpTextLine->iVer, lpTextLine->iLineHeight, HLRecog, VLRecog, alphH);
//    }

      if(((lpData->iSpeckSizeH)>0)&&((lpData->iSpeckSizeV)>0)) {
//  printf("14------------ cleanSpecks \n");
        cleanSpecks(lpData);
        }

//      if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0)){
//        printf("after linesForFonts; lpWorkBuffer->infFrame[0]).nLines= %d\n", (lpWorkBuffer->infFrame[0]).nLines);
//        fflush(stdout);
//      }
//       	    if(strcmp(fieldName, "RET_NUMN") == 0)
//		        printf(" lpWorkBuffer->infFrame[0]).nLines= %d\n", (lpWorkBuffer->infFrame[0]).nLines);
//      if((strcmp(fieldName, "DATEN") == 0 ||
//    	    strcmp(fieldName, "RET_NUMN") == 0)){
//                printf("2. fieldName= %s, lpWorkBuffer->flgUNP= %d, lpWorkBuffer->  Method= %d\n", fieldName, lpWorkBuffer->flgUNP, lpWorkBuffer->  Method);
//                fflush(stdout);
//                  }

     return(i);

}  /* of initFieldR() */


int  nextFieldR(
   char* fieldName,
   int sizeW,
   BYTE *lpBuffer,         /* pointer to image */

   STRUCTWB * lpWorkBuffer, /* pointer to work area - at least 6.7Kbytes         */
   int      *lpLwidth,      /* pointer to output cellWidth */
   LPBYTE lpExtractCh,          /* pointer where extracted character is sent     */
                                /* at least
                                   ((wWidth+31)/32)*4* wHeight
                                   bytes ( use 1.2KByte)                         */
   WORD   wWidth,               /* max width of extracted charcter (in pixels)     */
                                /* use 96 */
   WORD   wHeight,              /* max height of extracted charcter  (in pixels    */
                                /* use 96 */
   WORD   wCharType            /* type of expected characters */
   )
{
	int nFr, i, crR, higR, rm;
  int ret;
  int scal = 1;

//  char buffLog[500];

//  if(strcmp(fieldName, "NUMBERS") == 0) {
//  if(strcmp(fieldName, "DATEA") == 0) {
//  printf("------------0  (lpWorkBuffer->curLine[0]).y1= %d, (lpWorkBuffer->curLine[0]).y2= %d\n", (lpWorkBuffer->curLine[0]).y1, (lpWorkBuffer->curLine[0]).y2);
//  }
      (lpWorkBuffer-> lpRBuffer)=lpBuffer;
//      tHMass = (int)((float)50*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));  // ????????????????????????????
      nFr=0;
      wCharType=6;
/*----------------------------------------------------------------*/
/* one step segmentation for un-proportional fonts */
//      if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0) {
//      if(strcmp(fieldName, "RET_NUMN") == 0){
//               printf("------------lpWorkBuffer->  lettWidth= %d, lpWorkBuffer->  Method= %d\n", lpWorkBuffer->  lettWidth,lpWorkBuffer->  Method);
//               printf("------------lpWorkBuffer->newLine= %d\n", lpWorkBuffer->newLine);
//      }


     if( ((lpWorkBuffer->  lettWidth)>0)&&((lpWorkBuffer->  Method)==1) ) {
        if( (lpWorkBuffer->newLine) == 1 ) {

//  if(strcmp(fieldName, "NUMBERS") == 0)
//  if(strcmp(fieldName, "RET_NUMN") == 0)
//      printf("------------1  lpWorkBuffer->  lettWidth= %d\n", lpWorkBuffer->  lettWidth);

             (lpWorkBuffer -> trush) = 0;
             if((lpWorkBuffer -> fieldType)==MULTILINEBOXD) {

NLINE1:          if( (nFr=maxYstruct(lpWorkBuffer))>=0 )  {

                   searchBeg(lpWorkBuffer,nFr);
                   searchEnd(lpWorkBuffer,nFr);
                   }
               else {
                  return(0);
                  }
             if(!isLineValid(lpWorkBuffer,wCharType)) {
//          	  if(strcmp(fieldName, "DATEA") == 0)
//            	      printf("------------1  isLineValid\n");

                     goto NLINE1;
                     }

               }

             goto NXT;
             }

       ret=sendNextFont(lpWorkBuffer, fieldName);

//       if(strcmp(fieldName, "RET_NUMN") == 0)
//           printf("------------sendNextFont;  ret= %d\n", ret);

       if(ret==3) {
                if((lpWorkBuffer -> fieldType)!=MULTILINEBOXD) {
                        return(0);
                        }

                (lpWorkBuffer->newLine)=1;
                (lpWorkBuffer->  lettWidth) = (lpWorkBuffer->  lettWidthG);
                }
        nFr=ret;
//        if(lpWorkBuffer -> segmType == 177)
//                    if(strongMassTest(lpWorkBuffer, tHMass )) {
//                       setEmptyBuff(lpExtractCh,wWidth,wHeight);
//                       return(1001);
//                       }

        if(ret==1)  goto SNT;

        if(ret>1000) {
                 setEmptyBuff(lpExtractCh,wWidth,wHeight);
                    ret = 1001;
                 }
        return(ret);

        }

/*----------------------------------------------------------------*/

aBC:

     if( (lpWorkBuffer->newLine)==1 ) {

//     	  if(strcmp(fieldName, "NUMBERS") == 0)
//       if(strcmp(fieldName, "RET_NUMN") == 0)
//      	      printf("------------  new Line\n");

          (lpWorkBuffer -> trush) = 0;
          if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) {

NLINE:       if( (nFr=maxYstruct(lpWorkBuffer))>=0 )  {

                searchBeg(lpWorkBuffer,nFr);
                searchEnd(lpWorkBuffer,nFr);
                }
             else {
                return(0);
                }
             if(!isLineValid(lpWorkBuffer,wCharType)) {
//             	  if(strcmp(fieldName, "DATEA") == 0)
//              	      printf("2------------  new Line, isLineValid\n");

                     goto NLINE;
                     }
             }
          }

//     if(strcmp(fieldName, "RET_NUMN") == 0)
//     	   printf("2------------  lpWorkBuffer->blank= %d, lpWorkBuffer -> fieldType= %d, lpWorkBuffer->newLine= %d\n",lpWorkBuffer->blank,lpWorkBuffer -> fieldType,lpWorkBuffer->newLine);

        if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)&&((lpWorkBuffer->newLine)==1)) goto NXT;
        if((lpWorkBuffer->blank)==0) {

             for(i=0;i<=(lpWorkBuffer->lengthTraceC);i++) {
                  (lpWorkBuffer->traceL[i])=(lpWorkBuffer->traceC[i]);
                  }
             (lpWorkBuffer->lengthTraceL)=(lpWorkBuffer->lengthTraceC);

//         if(strcmp(fieldName, "RET_NUMN") == 0)
//        	  printf("lpWorkBuffer->lengthTraceC= %d, lpWorkBuffer->flgCut= %d, lpWorkBuffer->flgTraceR= %d\n",lpWorkBuffer->lengthTraceC,lpWorkBuffer->flgCut,lpWorkBuffer->flgTraceR);

             if((lpWorkBuffer->flgCut)) {
                  if((lpWorkBuffer->flgTraceR)) {
                     crR=(lpWorkBuffer->curMark2);
                     higR=min( ((lpWorkBuffer->mark[crR]).d_m-(lpWorkBuffer->mark[crR]).g_m)+2,MAXLTRACE-1);
                     (lpWorkBuffer->lengthTraceR)=higR;
                     (lpWorkBuffer->traceR[0])=(lpWorkBuffer->mark[crR]).g_m;
                     rm=(lpWorkBuffer->mark[crR]).r_m;
                     if(rm<0) rm=-rm;
                     for(i=1;i<=higR;i++)  {
                             (lpWorkBuffer->traceR[i])=rm;
                             }
                     (lpWorkBuffer->lengthTraceR)=higR;
                     (lpWorkBuffer->flgTraceR)=0;

//          if(strcmp(fieldName, "RET_NUMN") == 0)
//           	  printf("crR= %d, higR= %d, lpWorkBuffer->flgTraceR= %d\n",crR,higR,lpWorkBuffer->flgTraceR);

                     }
                   }

             }

NXT:
   (lpWorkBuffer->flgCut)=0;
   (lpWorkBuffer->linkRM[0])=2;
   (lpWorkBuffer->boundLeft)=0;
   lpWorkBuffer->flgUNP = 1;
   {int segmMode = 0;
    if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
 		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) {

        if(lpWorkBuffer->nOffCut600 != 77) segmMode = 1;
    }
//    if((US_State == 2 || US_State == 3) && (strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "LOGO3") == 0)) segmMode = 1;
    if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "LOGO3") == 0)) segmMode = 1;

//    if(strcmp(fieldName, "NUMBERS") == 0){
//        if( (lpWorkBuffer->newLine)==1 )
//             printf("------------          newLine, segmMode= %d\n",segmMode);
// if(strcmp(fieldName, "NUMBERS") == 0)
//    printf("------------          segmMode= %d\n",segmMode);
//    }

    if((US_State <= 1  || US_State == 3) &&
    		(strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "LOGO3") == 0 ||
				 strcmp(fieldName, "RET_NUMN") == 0 || strcmp(fieldName, "RET_NUMA") == 0)) {segmMode = 0; lpWorkBuffer->flgUNP = 0;}
    if((US_State <= 1 ) && (strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0)){
	   switch(ticketTypeInt){
//	    case 0: // LOTTO TEXAS
//	   break;
//	case 1: // PICK 3
//	case 4: // DAILY 4
//	   break;
      case 1: // POWER BALL
	  case 2: // MEGA MILLIONS
      case 3: // LOTTO TEXAS
	  case 4: // TWO STEP
  	  case 5: // PICK 3
	  case 7: // CASH FIVE
      case 8: // ALL OR NOTHING TX
	       segmMode = 0;
//	       segmMode = 1;
	   break;
//	   break;
	  }
    }

    if(US_State == 2 && (strcmp(fieldName, "NUMBERS") == 0 )){
	   switch(ticketTypeInt){
      case 7: // Daily Derby
	       segmMode = 0;
	   break;
	  }
    }

//     if(strcmp(fieldName, "NUMBERS") == 0)
//        printf("1------------          segmMode= %d\n",segmMode);

    if(US_State == 3 && (strcmp(fieldName, "NUMBERS") == 0 )){
	   switch(ticketTypeInt){
      case 7: // Pick 10
	       segmMode = 0;
	   break;
	  }
    }

	   if(strcmp(fieldName, "segmMode0") == 0) segmMode = 0;
	   if(strcmp(fieldName, "segmMode1") == 0) segmMode = 1;

//	   if((strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0))
//           printf("-----segmentationSwitchFlag= %d, fieldName= %s\n",segmentationSwitchFlag, fieldName);

	   if(segmentationSwitchFlag > 0 && (strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0)){
//        printf("------------          segmMode= %d, fieldName= %s\n",segmMode, fieldName);
		    if(segmMode == 1) segmMode = 0;
		    else  segmMode = 1;
//        printf("------------          segmMode= %d, fieldName= %s\n",segmMode, fieldName);
	   }

//   if((strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0))
//   if((strcmp(fieldName, "RET_NUMN") == 0 || strcmp(fieldName, "RET_NUMA") == 0))
//        printf("2------------          segmMode= %d, fieldName= %s\n",segmMode, fieldName);


//   segmMode = 1;

//	     if((strcmp(fieldName, "DATEN") == 0 ||
//	   	    strcmp(fieldName, "RET_NUMN") == 0)){
//	               printf("Next. fieldName= %s, lpWorkBuffer->flgUNP= %d, lpWorkBuffer->  Method= %d\n", fieldName, lpWorkBuffer->flgUNP, lpWorkBuffer->  Method);
//	                 }

//  	    if(strcmp(fieldName, "RET_NUMN") == 0)
//  	    	       printf("NXT------------          segmMode= %d, fieldName= %s\n",segmMode, fieldName);

   if(segmMode == 1) {

    if(trapezoidVer == 1 && US_State == 2 && strcmp(fieldName, "NUMBERS") == 0) {
       while( (nFr=SegmCharsNumbers_2(lpWorkBuffer,wCharType,lpLwidth, fieldName, sizeW))==-100 );
   }
     else{
       while( (nFr=SegmChars2(lpWorkBuffer,scal,wCharType,lpLwidth, fieldName, sizeW))==-100 );
   }


   }else{
	   while( (nFr=SegmChars(lpWorkBuffer,scal,wCharType,lpLwidth, fieldName, sizeW))==-100 );
   }
   }
//	    if(strcmp(fieldName, "RET_NUMN") == 0)
//	    	       printf("SNT------------          nFr= %d\n",nFr);

SNT:;
   if((lpWorkBuffer->hndOrOCR) == 0 ) {
//  printf("------------SNT:; nFr= %d\n", nFr);
      if(nFr==-200) goto aBC;

       if(nFr>0 && nFr<100 && nFr!=3){

             if(lpWorkBuffer->traceL[0] + (lpWorkBuffer->lengthTraceL)  >=
               lpWorkBuffer -> begRect.y + lpWorkBuffer -> Height - 1)
                                    lpWorkBuffer->lengthTraceL = lpWorkBuffer -> begRect.y + lpWorkBuffer -> Height - lpWorkBuffer->traceL[0] -1;
             if(lpWorkBuffer->traceC[0] + (lpWorkBuffer->lengthTraceC)  >=
               lpWorkBuffer -> begRect.y + lpWorkBuffer -> Height - 1)
                                    lpWorkBuffer->lengthTraceC = lpWorkBuffer -> begRect.y + lpWorkBuffer -> Height - lpWorkBuffer->traceC[0] -1;
             if(lpWorkBuffer->traceL[0]  < lpWorkBuffer -> begRect.y)
                                    lpWorkBuffer->traceL[0] = lpWorkBuffer -> begRect.y;
             if(lpWorkBuffer->traceC[0]  < lpWorkBuffer -> begRect.y)
                                    lpWorkBuffer->traceC[0] = lpWorkBuffer -> begRect.y;

            i= sendToBuffer(lpWorkBuffer,lpExtractCh,wWidth,wHeight,wCharType);
//printf("------------sendToBuffer; i= %d\n", i);
            if(i!=0) nFr=i;
             }
       }


//?????????????        ONLY ONE SPACE
   if(nFr > 1000) {
              setEmptyBuff(lpExtractCh,wWidth,wHeight);
              nFr = 1001;
              }

   return(nFr);

} /* of nextFieldR() */
int testForLongWhite(int *vHist, int h_size, int th, int start){
	 int mass;
	 int k, k1, k2, stop, cc;
	   mass = 0;
	   start += 3;
	   stop = start+50;
	   if(stop >= h_size) return start;
	   k1 = 0;
       for(k=start;k<stop;k++){
		   mass = (vHist[k-2]+vHist[k-1]+vHist[k])/3;
		   if(mass == 0) {k1 = k; break;}
       }
// printf(" ---------------------k1= %d, start= %d \n", k1, start);
       if(k1 == 0) return start;
   	   cc = 0;
   	   k2 = 0;
       for(k=k1+3;k<h_size;k++){
	       mass = (vHist[k-2]+vHist[k-1]+vHist[k])/3;
		   if(mass < th) cc++;
           else          {k2 = k; break;}
       }
//      printf(" ---------------------k2= %d, cc= %d \n", k2, cc);
       if(cc < 100) return start;
       return k2;

}

int getLeftSideStartIdx(int *vHist, int h_size, int th){
	 int mass;
	 int i, k ;
	   mass = 0;
	   for(i=3;i<h_size;i++){
		   mass = (vHist[i-2]+vHist[i-1]+vHist[i])/3;
		   if(mass > th){
               k = testForLongWhite(vHist,  h_size, th, i);
               if(k - i > 100 ) {i = k;continue;}
			   return i;
			   }
		   }
	   return h_size;
}
int getRightSideStartIdx(int *vHist, int h_size, int th){
	 int mass;
	 int i;
	   mass = 0;
	   for(i=h_size-4;i>0;i--){
		   mass = (vHist[i+2]+vHist[i+1]+vHist[i])/3;
		   if(mass > th){
			   return i;
			   }
		   }
	   return 0;
}
void shiftLines(STRUCTWB *lpWorkBuffer, int idx, int idF){
	int j;
	 for(j=idx;j<(lpWorkBuffer->infFrame[idF]).nLines-1;j++) {
		 (lpWorkBuffer->line[idF]).y1[j] = (lpWorkBuffer->line[idF]).y1[j+1];
		 (lpWorkBuffer->line[idF]).y2[j] = (lpWorkBuffer->line[idF]).y2[j+1];
	 }
	 (lpWorkBuffer->infFrame[idF]).nLines--;
}
extern     int       gr_ar[];
void histHPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2, int *hHist) {
	  int sww, i ,j, dx, x1D8, dy, sum, ind, w_bt;
	  LONG nY;
	  BYTE  *hptr;
	  int  *pp;

	              w_bt=widthImg;
	              x1D8=x1/8;
	              pp=(int *)hHist;
	              dy=y2-y1+1;
	              dx=x2/8-x1D8-1;
	              *pp=dy;
	              pp++;i=0;
	              while(i<dy)  {
	                sum=0;
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                hptr=ptrPix+nY+(LONG)x1D8;

	                ind=(int)(*hptr);
	                if(ind<0) {ind+=256;}
	                ind=255-ind;
	                sum+=FirstZer(ind,x1%8);
	                hptr++;
	                for(j=0;j<dx;j++)  {
	                        ind=(int)(*hptr);
	                        if(ind<0) {ind+=256;}
	                        ind=255-ind;
	                        sum+=gr_ar[ind];
	                        hptr++;
	                        }
	                if( (j+x1D8+1)<widthImg) {
	                  ind=(int)(*hptr);
	                  if(ind<0) {ind+=256;}
	                  ind=255-ind;
	                  sum+=LastZer(ind,8-x2%8);
	                  }
	//                hptr++;

	                *pp=sum;
	                i++;
	                pp++;
	                }

    }/* of histHPtrPix()*/
void histHTest(LPBYTE ptrPix, int widthImg, int hightImg,  int y1, int y2, int *hHist){
	  int sww, i ,j, dy, sum, ind, w_bt;
	  LONG nY;
	  BYTE  *hptr;
	  int  *pp;

	              w_bt=widthImg;
	              pp=(int *)hHist;
	              dy=y2-y1+1;
	              *pp=dy;
	              pp++;i=0;
	              while(i<dy)  {
	                sum=0;
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                for(j = 0;j<w_bt-1;j++) {
	                    hptr=ptrPix+nY+j;
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        sum+=gr_ar[ind];
	                }
	                *pp=sum;
	                i++;
	                pp++;
                }

    }/* of histHPtrPix()*/
void histHTest_X1_X1(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2, int *hHist){
	  int sww, i ,j, dy, sum, ind, w_bt;
	  int xStart, xStop;
	  LONG nY;
	  BYTE  *hptr;
	  int  *pp;

	              w_bt=widthImg;
	              pp=(int *)hHist;
	              dy=y2-y1+1;
	              *pp=dy;
	              pp++;i=0;
	              xStart = x1/8;
	              xStop =  x2/8;
	              while(i<dy)  {
	                sum=0;
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                for(j = xStart;j<=xStop;j++) {
	                    hptr=ptrPix+nY+j;
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        sum+=gr_ar[ind];
	                }
	                *pp=sum;
	                i++;
	                pp++;
                }

    }/* of histHPtrPix()*/
int maxBlackWidth(LPBYTE ptrPix, int widthImg, int hightImg,  int x1, int x2, int y1, int y2, int *hHist, int maxLen){
	  int sww, i ,j, dy, w_bt;
	  int xStart, xStop;
	  LONG nY;
	  BYTE  *hptr;
	  int max = 0;
	  int maxTmp = 0;
	  int  *pp;

	              w_bt=widthImg;
	              pp=(int *)hHist;
	              dy=y2-y1+1;
	              *pp=dy;
	              pp++;i=0;
	              xStart = x1/8;
	              xStop =  x2/8;
	              while(i<dy)  {
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                maxTmp = 0;
	                max = 0;
	                for(j = xStart;j<=xStop;j++) {
	                    hptr=ptrPix+nY+j;
                        if((*hptr) == 0)
                              maxTmp++;
                        else{
                       	 if(max < maxTmp) max = maxTmp;
                       	 maxTmp = 0;
                        }
	                }
	                if(i >= maxLen) break;
	                *pp=max;
	                i++;
	                pp++;
                }
          return max;
    }/* of maxBlackWidth()*/
int GetRealTH(int *tab, int ccF, int thW){
	int i, av, cc, d;
	int min = 2000;

//printf(" -----------------------------------  thW= %d \n", thW);
	for(i= 0;i<ccF;i++){
		d = abs(thW - tab[i]);
	   if(d  < min)
		   min = d;
	}
	min += 50;
	min += (min/2);
//	printf("  min= %d \n", min);


	av = 0;cc = 0;
	for(i= 0;i<ccF;i++){
//printf(" thW - tab[i]= %d, min= %d \n", thW - tab[i], min);
	   if(thW - tab[i]  < min){
		   av += tab[i];cc++;
	   }
	}
	if(cc > 0)
		av /= cc;
//printf("  av= %d \n", av);
	av -= 60;
//printf(" -----------------------------------  av= %d, thW= %d \n", av, thW);
	if(av > thW || av == 0) return thW;
	return av;
}
void secondTestForLine(STRUCTWB *lpWorkBuffer, int idF, int sizeWImg){
#define	NUM_LINESTOFILTER 40
 int h_size = 1000;
 int h_size1 = 900;
 int th = 4;
 int vHist[h_size];
 int i1, i2;
 int  j, jj, start, stop;
 int idx[NUM_LINESTOFILTER];
 int k = 0, n;
 int av = 0;
 int thW = sizeWImg/2;
 int thWr = thW;
 int tabi1_2[NUM_LINESTOFILTER];
 int ccFilter = 0;


// char buffLog[500];
//int kk;
/*
        for(j= 0;j<(lpWorkBuffer->infFrame[idF]).nLines;j++){
//            	 printf("---------------kkk = %d, y2-y1= %d\n",kkk, lpWorkBuffer->line[i].y2[kkk]-lpWorkBuffer->line[i].y1[kkk]);
        	if(lpWorkBuffer->line[idF].y2[j]-lpWorkBuffer->line[idF].y1[j] > 60){
//    		  histHTest(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);
    		    histHPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, 0, lpWorkBuffer -> Width/4,
    		  			     (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);

 		     sprintf(buffLog," ---------------------- j= %d, idF= %d, y2-y1= %d\n", j, idF, lpWorkBuffer->line[idF].y2[j]-lpWorkBuffer->line[idF].y1[j]);
 		     saveLog("", buffLog);

    		  for(n=0;n<vHist[0];n++) {
    		     sprintf(buffLog," -----  vHist[ %d ]= %d\n", n,vHist[n]);
    		     saveLog("", buffLog);
    		  }


        	}

       }
*/



 for(j=0;j<NUM_LINESTOFILTER;j++){
	 idx[j] = -1;
	 tabi1_2[j] = 0;
 }

 for(j=0;j<(lpWorkBuffer->infFrame[idF]).nLines;j++) {

	 histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, h_size1-50, 50, h_size1, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);
	 i1 = getLeftSideStartIdx(vHist, h_size1-50, th)+50;
	 start = lpWorkBuffer -> widthImg*8-h_size1;
	 stop = lpWorkBuffer -> widthImg*8-50;
	 histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, h_size1-50, start, stop, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);
	 i2 = getRightSideStartIdx(vHist, h_size1-50, th);
     i2 = start + i2;
     if(k < NUM_LINESTOFILTER){
        tabi1_2[ccFilter] = i2-i1;
        ccFilter++;
     }
// sprintf(buffLog," -----------------------------------  j= %d, i1= %d, i2= %d, i2-i1= %d \n", j, i1, i2, i2-i1);
// saveLog("", buffLog);

//  printf(" -----------------------------------  j= %d, i1= %d, i2= %d, i2-i1= %d \n", j, i1, i2, i2-i1);
 }

 thWr = GetRealTH(tabi1_2, ccFilter, thW);
 if((US_State == 0 || US_State == 1) && ticketTypeInt == 7){
	 thWr = (2*thWr)/3;
 }
 else
	 thWr -= 40;
//  printf(" -----------------------------------  thWr= %d \n", thWr);
 k = 0;
 for(j=0;j<ccFilter;j++) {
//  printf(" -----------------------------------  j= %d, i2-i2= %d, thWr= %d \n", j, tabi1_2[j], thWr);
	    if(tabi1_2[j] < thWr){
	    	idx[k] = j;
	    	k++;
//    	printf(" -----------------------------------  Filter: j= %d \n", j);
    }
 }
//return;

 //remove lines
 j = 0;
 while(j<k){
	 if(idx[j] >= 0) {
// sprintf(buffLog," ---------------------shiftLines  j= %d, idx[j]= %d \n", j, idx[j]);
// saveLog("", buffLog);

		 shiftLines(lpWorkBuffer, idx[j], idF);
	     jj = j+1;
	     while(jj<k) {
	        idx[jj]--;
	        jj++;
	    }
	    j++;
     }
 }
 for(j=0;j<10;j++){
	 idx[j] = -1;
 }
// sprintf(buffLog," ---------------------lpWorkBuffer->infFrame[idF]).nLines= %d\n",(lpWorkBuffer->infFrame[idF]).nLines);
// saveLog("", buffLog);

 k = 0;
 for(j=0;j<(lpWorkBuffer->infFrame[idF]).nLines;j++) {
//	  histHPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, 0, lpWorkBuffer -> widthImg-1,
//			     (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);
	  histHTest(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], vHist);
      for(n=1;n<=vHist[0];n++){
    	  av += vHist[n];
//if(j == 2)
//  printf(" -----------------------------------  j= %d, n= %d, vHist[n]= %d \n", j, n, vHist[n]);
      }
      if(vHist[0] > 0)
    	  av /= vHist[0];
//  printf(" -----------------------------------  j= %d, av= %d \n", j, av);
      if(av < 30){
     	 idx[k] = j;
     	 k++;
      }
 }

 //remove lines
// sprintf(buffLog," ---------------------remove lines= %d\n",k);
// saveLog("", buffLog);

 for(j=0;j<k;j++){
	 if(idx[j] >= 0) {
		 shiftLines(lpWorkBuffer, idx[j], idF);
	     jj = j+1;
	     while(jj<k) {
	         idx[jj]--;
	         jj++;
	     }
	 }
 }


}

int histH_alpha2(LPBYTE buffPtr, int widthImg, int hightImg,
		          int x1, int y1, int x2, int y2, LONG alpha, float tG, int *histH, int *lenH)
{
  int   sww, i ,j, dx, x1D8, dy, sum, ind, w_bt;
  int   kDirect, sumP, swtP2, swtP, yC, xx, x28;
  LONG  nY;
  BYTE   *hptr;
  int  *pp;

              if((tG<EPSTG)&&(tG>(-EPSTG))) {
                swtP2=32000;
                swtP=0;
                }
              else {
                swtP2=(int)((float)1/tG);
                if(swtP2<0) {swtP2=-swtP2;}
                swtP=swtP2/2;
                }
              kDirect=1;
              if(alpha>0) {kDirect=-1;}
              w_bt= widthImg;
              x1D8=x1/8;
              dy=y2-y1+1;
              dx=x2/8-x1D8-1;
              *lenH=dy;
              pp=histH;
              xx=dx+x1D8+1;
              x28=8-x2%8;
              i=0;
              while(i<dy)  {
                sum=0;
                sww=y1+i;
                yC=sww;

                sww= hightImg-1-sww;
                nY=(LONG)sww*(LONG)w_bt;
                hptr=buffPtr+nY+(LONG)x1D8;

                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                sumP=x1%8;
                sum+=FirstZer(ind, sumP);
                sumP=8-sumP;
                sumP+=swtP;
                hptr++;
                for(j=0;j<dx;j++)  {
                        if(sumP>swtP2) {
                           sumP=0;
                           yC+=kDirect;
                           if((yC<y1)||(yC>y2)) {break;}
                           hptr=hptr-(LONG)(kDirect*w_bt);
                           }
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        sum+=gr_ar[ind];
                        hptr++;
                        sumP+=8;
                        }
                if( xx<widthImg) {
                  ind=(int)(*hptr);
                  if(ind<0) {ind+=256;}
                  ind=255-ind;
                  sum+=LastZer(ind, x28);
                  }
                *pp=sum;
                i++;
                pp++;
                }
        return(0);

 }/* end of histH_alpha2() */

int getAngleFromHistH(LPBYTE buffPtr, int widthImg, int hightImg,
		              int x1, int y1, int x2, int y2,
		         float maxAlpha, float stepAlpha, float *ang){
  int   i, nH, lenHH, minH, wH, kD;
  float alP;

   angleHist[0]=0L;
   nH=1;
   kD=1;
   for(i=1;i<=MAXHIST/2;i++) {
        if(nH<MAXHIST) {
                alP=(float)i*stepAlpha;
                if(alP<=maxAlpha) {
// printf("------------ linesForFonts, alP= %e \n", alP);
                    angleHist[nH]=(LONG)(alP*(float)SCALEAN);
                    tGAngle[nH] = (float)tan((double)(alP*PI)/180.0);
                    nH++;
                    if(nH<MAXHIST) {
                        angleHist[nH]= -angleHist[nH-1];
                        tGAngle[nH]  = -tGAngle[nH-1];
                        nH++;
                        }
                    }
                else {break;}
                }
        }

//  printf("------------ linesForFonts, maxAlpha= %e nH= %d \n", maxAlpha, nH);
//  printf("------------ x1= %d, y1= %d, x2= %d, y2= %d \n", x1, y1, x2, y1);
   minH=32000;
   for(i=0;i<nH;i++) {
         lpHistHSk[i]=(int *)&histHSk[i][1];
         histH_alpha2(buffPtr, widthImg, hightImg,
       		          x1, y1, x2, y2,
                     angleHist[i],tGAngle[i],lpHistHSk[i],(int *)&lenHH);
         if(minH>lenHH) {minH=lenHH;}
         }
   wH=whichHist((WORD)(widthImg),(int  * *)lpHistHSk,(int)nH,(int)minH,
             (float *)&tGAngle[0]);

//   for(i=1;i<minH;i++) {
//        histH[i]=histHSk[wH][i];
//        }
//    if(minH >= (lpWorkBuffer->Height)) {minH = (lpWorkBuffer->Height)-1;}
//   *lenH     =minH;
//   *tG   = tGAngle[wH];
   *ang  = (float)angleHist[wH]/(float)SCALEAN;
//    printf("------------ tGAngle[wH]= %e \n", tGAngle[wH]);
//   printf("------------ wH= %d, angleHist[wH]= %ld, minH= %d \n", wH, angleHist[wH], minH);

   return(0);

}  /* getAngleFromHistH() */

int  testLineUpperEdge(int *hHist, int start){
	int shift = 0;
	int i, kMax;
	int av1, av2, iRatioTab[10], idxR = 0;
	float ratio, ratioTab[10], max;

// find all possible edges
	for(i=hHist[0];i>5;i--){
		av1 = (hHist[i] + hHist[i-1] + hHist[i-2])/3;
		av2 = (hHist[i-3] + hHist[i-4] + hHist[i-5])/3;
		if(av2 == 0) av2  = 1;
		if(av2 > 0){
          	ratio = av1/av2;
          	if(ratio > 1.9 && idxR < 10){
          		ratioTab[idxR] = ratio;
          		iRatioTab[idxR] = i-3;
          		idxR++;
          	}
		}
	}
    if(idxR > 0){
    	for(i=0;i<idxR;i++){
    		if(abs(iRatioTab[i] - start) < 2)
    			return shift;
    	}
    	kMax = 0;
    	max = ratioTab[i];
    	for(i=1;i<idxR;i++){
    		if(max < ratioTab[i]){max = ratioTab[i]; kMax = i;}
// printf("------------i= %d, ratioTab[i]= %2.4f, iRatioTab[i]= %d \n", i, ratioTab[i], iRatioTab[i]);
    	}
    	if(max > 5)
    	  shift = iRatioTab[kMax] - start;
    }
	return shift;
}

int  testLinelowerEdge(int *hHist, int start){
	int shift = 0;
	int i, kMax;
	int av1, av2, iRatioTab[10], idxR = 0;
	float ratio, ratioTab[10], max;

// find all possible edges
	for(i=1;i<=hHist[0]-5;i++){
		av1 = (hHist[i] + hHist[i+1] + hHist[i+2])/3;
		av2 = (hHist[i+3] + hHist[i+4] + hHist[i+5])/3;
		if(av2 > 0){
          	ratio = av1/av2;
          	if(ratio > 1.9 && idxR < 10){
          		ratioTab[idxR] = ratio;
          		iRatioTab[idxR] = i+3;
          		idxR++;
          	}
		}
	}
    if(idxR > 0){
    	for(i=0;i<idxR;i++){
    		if(abs(iRatioTab[i] - start) < 2)
    			return shift;
    	}
    	kMax = 0;
    	max = ratioTab[i];
    	for(i=1;i<idxR;i++){
    		if(max < ratioTab[i]){max = ratioTab[i]; kMax = i;}
// printf("------------i= %d, ratioTab[i]= %2.4f, iRatioTab[i]= %d \n", i, ratioTab[i], iRatioTab[i]);
    	}
    	if(max > 5)
    	  shift = iRatioTab[kMax] - start;
    }
	return shift;
}
void selectOneTopDateLine(STRUCTWB *lpWorkBuffer, int idF, int sizeWImg, int sizeHImg){

 double mass = 0, massN, massMax;
 int i, j, stop, jMax;
 int byteMass, dd;
 stop = 3;
 if(stop > (lpWorkBuffer->infFrame[idF]).nLines) stop = (lpWorkBuffer->infFrame[idF]).nLines;
 jMax = 0;massMax=0;

// printf("------------ stop= %d \n", stop);
// fflush(stdout);
 for(j=0;j<stop;j++) {
//	 printf("------------ (lpWorkBuffer->line[idF]).y1[j]= %d, (lpWorkBuffer->line[idF]).y2[j]= %d \n", (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j]);
//	 fflush(stdout);
	 histHTest_X1_X1(lpWorkBuffer->lpRBuffer, lpWorkBuffer->widthImg, lpWorkBuffer->hightImg,  20, lpWorkBuffer->widthImg*8-20, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], hHistPreProc);

//	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, 20, lpWorkBuffer -> widthImg*8-20, lpWorkBuffer -> widthImg*8, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], hHistPreProc);
//	for(i=1;i<50;i++){
//	  printf("------------ i= %d, hHistPreProc[i]= %d\n", i, hHistPreProc[i]);
//	}
	mass = 0;
    for(i=1;i<hHistPreProc[0];i++){
		    mass += hHistPreProc[i];
    }
    massN = ((double)(((lpWorkBuffer->line[idF]).y2[j]-(lpWorkBuffer->line[idF]).y1[j])))*mass;
    maxBlackWidth(lpWorkBuffer->lpRBuffer, lpWorkBuffer->widthImg, lpWorkBuffer->hightImg,  20, lpWorkBuffer->widthImg*8-20, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], hHistPreProc, 300);
    byteMass = 0;
   	for(i=1;i<=hHistPreProc[0];i++){
   		byteMass += hHistPreProc[i];
       }
   	dd = ((lpWorkBuffer->line[idF]).y2[j]-(lpWorkBuffer->line[idF]).y1[j]);
   	if(dd > 0)
   		byteMass /= dd;
//   printf("------------ byteMass= %d, hHistPreProc[0]= %d\n", byteMass, hHistPreProc[0]);
//   fflush(stdout);
//       	for(i=1;i<=hHistPreProc[0];i++){
//       	  printf("------------ i= %d, hHistPreProc[i]= %d\n", i, hHistPreProc[i]);
//       	fflush(stdout);
//       }


       if(massN > massMax && byteMass < 2) {massMax = massN;jMax = j;}
//    printf("------------ j= %d, mass= %e, massN= %e \n", j, mass, massN);
//   	fflush(stdout);
 }
//  {int printIdx = ((lpWorkBuffer->line[idF]).y2[jMax]-(lpWorkBuffer->line[idF]).y1[jMax]);
//     printf("------------ jMax= %d, printIdx= %d\n", jMax, printIdx);
//	 fflush(stdout);
// }

//	 (lpWorkBuffer->line[idF]).y1[0] = (lpWorkBuffer->line[idF]).y1[jMax];
//	 (lpWorkBuffer->line[idF]).y2[0] = (lpWorkBuffer->line[idF]).y2[jMax];
//	 (lpWorkBuffer->infFrame[idF]).nLines = 1;
 {int shift1, shift2, start, y1, y2;
  int yy1, yy2, xx1;

    start = sizeHImg/63;
    start += 3;
    yy1 = (lpWorkBuffer->line[idF]).y1[jMax]-start;
    if(yy1 <0 ) yy1 = 0;
    yy2 = (lpWorkBuffer->line[idF]).y1[jMax]+start;
    if(yy2 >= lpWorkBuffer->hightImg ) yy2 = lpWorkBuffer->hightImg-1;
    xx1 = lpWorkBuffer->widthImg*8-20;
    if(xx1 < 0) xx1 = 0;
//    histHTest_X1_X1(lpWorkBuffer->lpRBuffer, lpWorkBuffer->widthImg, lpWorkBuffer->hightImg,  20, lpWorkBuffer->widthImg*8-20, (lpWorkBuffer->line[idF]).y1[jMax]-start, (lpWorkBuffer->line[idF]).y1[jMax]+start, hHistPreProc);
    histHTest_X1_X1(lpWorkBuffer->lpRBuffer, lpWorkBuffer->widthImg, lpWorkBuffer->hightImg,  20, xx1, yy1, yy2, hHistPreProc);
//    printf("-----------histHTest_X1_X1(); hHistPreProc[0]= %d \n", hHistPreProc[0]);
//    fflush(stdout);

    shift1= testLineUpperEdge(hHistPreProc, start);

//     printf("------------ shift1= %d \n", shift1);
//     fflush(stdout);
//    	for(i=1;i<=hHistPreProc[0];i++){
//    	  printf("------------ i= %d, hHistPreProc[i]= %d\n", i, hHistPreProc[i]);
//    }

    y1 = (lpWorkBuffer->line[idF]).y2[jMax]-start;
    y2 = (lpWorkBuffer->line[idF]).y2[jMax]+start;
    if(y1<0) y1 = 0;
    if(y2>=lpWorkBuffer->hightImg) y2=lpWorkBuffer->hightImg-1;
    histHTest_X1_X1(lpWorkBuffer->lpRBuffer, lpWorkBuffer->widthImg, lpWorkBuffer->hightImg,  20, lpWorkBuffer->widthImg*8-20, y1, y2, hHistPreProc);
    shift2= testLinelowerEdge(hHistPreProc, start);

//     printf("------------ shift2= %d \n", shift2);
//     fflush(stdout);

//	for(i=1;i<=hHistPreProc[0];i++){
//	  printf("------------ i= %d, hHistPreProc[i]= %d\n", i, hHistPreProc[i]);
//}

//	  printf("------------ shift= %d, \n", shift2);
	 (lpWorkBuffer->line[idF]).y1[0] = (lpWorkBuffer->line[idF]).y1[jMax]+shift1;
	 (lpWorkBuffer->line[idF]).y2[0] = (lpWorkBuffer->line[idF]).y2[jMax]+shift2;
	 (lpWorkBuffer->infFrame[idF]).nLines = 1;
 }
}

int testIfThisIsNLine(int *hHistPreProc){
	int n, cc, tH = 4;
	int iStart = 0, iStop = 0;
	float rst = 0;
	  cc = 0;
	  for(n=1;n<hHistPreProc[0];n++) {
		  if(hHistPreProc[n] > tH) cc++;
		  else cc = 0;
		  if(cc > 3) break;
		  iStart = n;
	  }
	  cc = 0;
	  for(n=hHistPreProc[0];n>0;n--) {
		  if(hHistPreProc[n] > tH) cc++;
		  else cc = 0;
		  if(cc > 3) break;
		  iStop = n;
	  }
	  rst = (float)(iStop-iStart)/(float)hHistPreProc[0];
	  if(rst < 0.2) return -1;
	  if(rst > 0.6) return 1;
//     printf(" -----iStop-iStart= %d,  hHistPreProc[0]= %d, rst= %f4.2\n", iStop-iStart, hHistPreProc[0], rst);
     return 0;
}
void createMarks(int *hHistPreProc, int *Marks, int *nM){
    int n, tH = 4, k1, k2;
      *nM = 0;
      k1 = 0;
      while(k1<hHistPreProc[0]){

	     for(n=k1;n<hHistPreProc[0];n++) {
		     if(hHistPreProc[n] > tH && hHistPreProc[n+1] > tH){
			    k1 = n+1;
			    break;
		    }
	     }
         k2 = k1;
	     for(n=k1;n<hHistPreProc[0];n++) {
		     if(hHistPreProc[n] == 0 && hHistPreProc[n+1] == 0){
			    k2 = n;
			    break;
		    }
	    }
	    if(k2-k1 < 20) break;
	    Marks[*nM] = k2;(*nM)++;
	    k1 = k2+1;
      }

}

double getAngleForNewFieldN(STRUCTWB *lpWorkBuffer, int idF, int line){
	double angle = 0;
	int maxSize = H_SIZE_PREPROC;
	int x1, x2;
	int lMarks[20], nLM;
	int rMarks[20], nRM;
	int lx1, rx2;
	int n;
	if(maxSize > lpWorkBuffer -> Width-1) maxSize = lpWorkBuffer -> Width-1;

	   x1 = 20;lx1= x1;
	   x2 = lpWorkBuffer -> Width/5; if(x1>=x2) return angle;
	   histHPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, x1, x2,
	    		(lpWorkBuffer->line[idF]).y1[line], (lpWorkBuffer->line[idF]).y2[line], hHistPreProc);

//		 		     printf(" -----line= %d --------------------------------\n", line);

//		   		  for(n=0;n<hHistPreProc[0];n++) {
//		   		     printf(" -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
//		   		  }
	   	createMarks(hHistPreProc, lMarks, &nLM);

	    x1 = lpWorkBuffer -> Width - lpWorkBuffer -> Width/5;
		x2 = lpWorkBuffer -> Width - 20; if(x1>=x2) return angle;
        rx2 = x2;
	   histHPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, x1, x2,
		    		(lpWorkBuffer->line[idF]).y1[line], (lpWorkBuffer->line[idF]).y2[line], hHistPreProc);

	   	createMarks(hHistPreProc, rMarks, &nRM);

//   	    printf(" -----  nLM= %d\n", nLM);
//	   	for(n=0;n<nLM;n++) {
//	   	    printf(" -----  lMarks[ %d ]= %d\n", n, lMarks[n]);
//	   	}
//   	    printf(" -----  nRM= %d\n", nRM);
//	   	for(n=0;n<nRM;n++) {
//	   	    printf(" -----  rMarks[ %d ]= %d\n", n, rMarks[n]);
//	   	}
	   	{ double tg = 0;
	   	  int cc = 0;double d = (double)(rx2-lx1);
          if(nLM > nRM) nLM = nRM;

   	      for(n=0;n<nLM;n++) {
	   		tg += ((double)(rMarks[n]-lMarks[n])/d);cc++;
	   	  }
  	      if(cc>0)
   	        tg /= (double)cc;

	      angle = atan(tg);
	      angle = (angle*180)/PI;
// printf("---------------tg = %e, angle= %f\n",tg, angle);
	   	}

	return angle;
}
void addPartOfNewFieldFromImg(LPBYTE ptrNext, STRUCTWB *lpWorkBuffer, int idF, int line, int heightBigImg, WORD *heightCurImg){
	int x1, x2, x, w;
	int y1, y2, y;
	int h, yy, yI, yO;
	long nYI, nYO;
	LPBYTE hptr, ptr = ptrNext;

	  w =  lpWorkBuffer -> widthImg;
	  x1 = 0;x2 = w;
	  y1 = (lpWorkBuffer->line[idF]).y1[line];y2 = (lpWorkBuffer->line[idF]).y2[line];
//	  h =  y2-y1+1;
	  h = heightBigImg;
// printf("---------------*heightCurImg= %d\n",*heightCurImg);

	  for(y=y1;y<=y2;y++){
		 yI= lpWorkBuffer -> hightImg-1-y;
		 nYI=(long)yI*(long)w;

		 yy = y-y1 + *heightCurImg;
//		 yy = y-y1;
		 yO= h-1-yy;

// printf("---------------yO= %d, yy= %d\n",yO, yy);
// fflush(stdout);


		 nYO=(long)yO*(long)w;
		 hptr=lpWorkBuffer->lpRBuffer+nYI;
		 ptr=ptrNext+nYO;
		 for(x=x1;x<=x2;x++)
		 {
			 *ptr = *hptr;
			 ptr++;hptr++;
		  }
	  }
	  (*heightCurImg) += (y2-y1+1);
//	  return ptr;
}
void addPartOfNewFieldFromExtract(LPBYTE ptrNext, LPBYTE  ptrExtract, WORD width, WORD height, int widthBigImg, int heightBigImg,
		                           WORD *heightCurImg){
	int x, w, wBimg;
	int  y, yI, yO;
	long nYI, nYO, h;
	LPBYTE hptr, ptr = ptrNext;

	  w =  (((width + 31) /32) * 4);
	  wBimg =  (((widthBigImg + 31) /32) * 4);


//	  x1 = 0;x2 = w;
//	  y1 = (lpWorkBuffer->line[idF]).y1[line];y2 = (lpWorkBuffer->line[idF]).y2[line];
       h = heightBigImg;
//  printf("---------------*heightCurImg= %d\n",*heightCurImg);
// printf("---------------heightBigImg= %d\n",heightBigImg);

	  for(y=0;y<height;y++){
		  yI= height-1-y;
		  nYI=(long)yI*(long)w;
		  hptr=ptrExtract+nYI;

		  yO= h-1-(y+*heightCurImg);
		  nYO=(long)yO*(long)wBimg;
		  ptr=ptrNext+nYO;

//printf("---------------yO= %d, y+*heightCurImg= %d\n",yO, y+*heightCurImg);
//fflush(stdout);

		  for(x=0;x<wBimg;x++){
//			 if(x>=wBimg) break;
			 *ptr = *hptr;
			 ptr++;hptr++;
		  }
		  if(w<wBimg) {
			 for(x=w;x<wBimg;x++){
				 *ptr = 255;
				 ptr++;
			 }
		 }
	  }
	  (*heightCurImg) += height;
//	  return ptr;
}

void createRestWhite(LPBYTE ptrNext, WORD widthByte, int heightBigImg, int heightCurImg){
	int x1, x2, x, w;
	int  y;
	int h,  yO;
	long nYO;
	LPBYTE ptr = ptrNext;

	  w =  widthByte;
	  x1 = 0;x2 = w;

	  h = heightBigImg;
// printf("---------------*heightCurImg= %d\n",*heightCurImg);

	  for(y=heightCurImg;y<heightBigImg;y++){

		 yO= h-1-y;
// printf("---------------yO= %d, yy= %d\n",yO, yy);
// fflush(stdout);

		 nYO=(long)yO*(long)w;
		 ptr=ptrNext+nYO;
		 for(x=0;x<=w;x++)
		 {
			 *ptr = 255;
			 ptr++;
		  }
	  }
}

void addWhiteImg(LPBYTE ptrNext, WORD widthByte, int heightBigImg, WORD *heightCurImg){
	int x1, x2, x, w;
	int  y;
	int h, h1, h2, yO;
	long nYO;
	LPBYTE ptr = ptrNext;

	  w =  widthByte;
	  x1 = 0;x2 = w;

	  h = heightBigImg;
	  h1 = *heightCurImg;
	  h2 = h1 + 8;
	  if(h2 >= h) h2 = h-1;
// printf("---------------*heightCurImg= %d\n",*heightCurImg);

	  for(y=h1;y<h2;y++){

		 yO= h-1-y;
// printf("---------------yO= %d, yy= %d\n",yO, yy);
// fflush(stdout);

		 nYO=(long)yO*(long)w;
		 ptr=ptrNext+nYO;
		 for(x=0;x<=w;x++)
		 {
			 *ptr = 255;
			 ptr++;
		  }
	  }
	  *heightCurImg = h2;
}

void replaceField(STRUCTWB *lpWorkBuffer, LPBYTE *fieldBMPPtr){

     int j, jK, k, n, idF = 0;
     int startLine = -1, stopLine = -1;
     int maxSize = H_SIZE_PREPROC;
     int lineOk[10];
//     char buffLog[500];

     if(maxSize > lpWorkBuffer -> Width-1) maxSize = lpWorkBuffer -> Width-1;

 	 printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!---------replaceField\n");
 	fflush(stdout);

// find first line from the top
        jK = 0;
        for(j= 0;j<(lpWorkBuffer->infFrame[idF]).nLines;j++){
//            	 printf("---------------kkk = %d, y2-y1= %d\n",kkk, lpWorkBuffer->line[i].y2[kkk]-lpWorkBuffer->line[i].y1[kkk]);
//        	if(startLine < 0){
      		   histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
      				maxSize, 20, lpWorkBuffer -> Width-20, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], hHistPreProc);
      		   lineOk[jK++] = testIfThisIsNLine(hHistPreProc);
//   	      printf(" -----lineOk[jK]= %d\n", lineOk[jK-1]);
      		   if(j>5)break;
//      		   if(lineOk > 0) startLine = j;
//        	}
      		 // 		     sprintf(buffLog," -----j= %d --------------------------------\n", j);
      		 // 		     saveLog("", buffLog);

      		 //  		  for(n=0;n<hHistPreProc[0];n++) {
      		 //  		     sprintf(buffLog," -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
      		 //  		     saveLog("", buffLog);
      		 //  		  }


        }
       	for(n=0;n<jK;n++){
//    	    printf(" -----lineOk[n]= %d\n", lineOk[n]);
       		if(lineOk[n] > 0) {startLine = n;break;}
       	}
//      printf(" -----startLine= %d\n", startLine);
       	k = 0;
       	for(n=startLine;n>0;n--){
//   	    printf(" -----lineOk[n]= %d\n", lineOk[n]);
       		if(lineOk[n-1] < 0) {k = n;break;}
       	}
       	startLine = k;

// find first line from the bottom
       	jK = 0;
        for(j= (lpWorkBuffer->infFrame[idF]).nLines-1;j>=0;j--){
      		   histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
      				maxSize, 20, lpWorkBuffer -> Width-20, (lpWorkBuffer->line[idF]).y1[j], (lpWorkBuffer->line[idF]).y2[j], hHistPreProc);
      		   lineOk[jK++] = testIfThisIsNLine(hHistPreProc);
//   	      printf(" -----lineOk[jK]= %d\n", lineOk[jK-1]);
      		   if(jK>5)break;


        }
       	for(n=0;n<jK;n++){
       		if(lineOk[n] > 0) {stopLine = n;break;}
       	}
       	k = 0;
       	for(n=stopLine;n>0;n--){
       		if(lineOk[n-1] < 0) {k = n;break;}
       	}
       	stopLine = (lpWorkBuffer->infFrame[idF]).nLines-1- k;





// create new field
       	{
    	int err;
    	LPBYTE ptrBigImg;
    	WORD widthBigImg;
    	WORD heightBigImg;
    	WORD heightCurImg;
    	LPBYTE ptrNext;

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
//        double angle;
        int  biSizeImage, biWidthPad;
        int bigSize;

//        width = lpWorkBuffer -> Width;
//        height= lpWorkBuffer -> hightImg;
    	offBits = sizeof (BITMAPFILEHEADER) +
    			sizeof (BITMAPINFOHEADER) +
    			sizeof (RGBQUAD) * 2;
        bigSize = offBits + lpWorkBuffer -> widthImg * (lpWorkBuffer -> hightImg+8*20);

    	ptrBigImg = yl_calloc("OCR_B", bigSize, sizeof(BYTE));


    	widthBigImg = lpWorkBuffer -> Width;
    	heightBigImg = lpWorkBuffer -> hightImg+8*20;
    	heightCurImg = 0;

    	bf = (LPBITMAPFILEHEADER) ptrBigImg;
    	bf->bfType[0] = 0x42;
    	bf->bfType[1] = 0x4d;
//    	set4Bytes(bf->bfSize, offBits + (((widthBigImg + 31) /32) * 4) * heightBigImg);
    	bf->bfReserved1[0] = 0;
    	bf->bfReserved1[1] = 0;
    	set4Bytes(bf->bfOffBits, offBits);


    	// fill in BITMAPINFOHEADER

//    	biSizeImage = biWidthPad*heightBigImg;

    	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
    	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
    	set4Bytes(bi->biWidth, widthBigImg);
    	set4Bytes(bi->biHeight, heightBigImg);
    	bi->biPlanes[0] = 1;
    	bi->biPlanes[1] = 0;
    	bi->biBitCount[0]= 1;
    	bi->biBitCount[1]= 0;
    	set4Bytes(bi->biCompression, BI_RGB);
//    	set4Bytes(bi->biSizeImage, biSizeImage);
    	set4Bytes(bi->biXPelsPerMeter, lpWorkBuffer->Dpi.x);
    	set4Bytes(bi->biYPelsPerMeter, lpWorkBuffer->Dpi.y);
    	set4Bytes(bi->biClrUsed, 0);
    	set4Bytes(bi->biClrImportant, 0);

    	// generate pair of RGBQUADs (black and white)
    	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
    	*(LPDWORD)qd       = 0x00000000;
    	*(LPDWORD)(qd + 1) = 0x00ffffff;

    	ptrNext = ptrBigImg+offBits;

//    	printf("---------------lpWorkBuffer -> widthImg = %d, lpWorkBuffer -> Width= %d\n",lpWorkBuffer -> widthImg, lpWorkBuffer -> Width);

       	{double angle = 0;
           for(j= startLine;j<=stopLine;j++){
//               j= 2;{

//printf("---------------j = %d, y2-y1= %d\n",j, lpWorkBuffer->line[idF].y2[j]-lpWorkBuffer->line[idF].y1[j]);
//fflush(stdout);


      	   if((lpWorkBuffer->line[idF]).y1[j] > 20 && (lpWorkBuffer->line[idF]).y2[j]<lpWorkBuffer -> hightImg-20 &&
        	   (lpWorkBuffer->line[idF]).y2[j] - (lpWorkBuffer->line[idF]).y1[j] > 80) {
        		    angle = getAngleForNewFieldN(lpWorkBuffer, idF, j);


	          ptr = yl_calloc("OCR_B", 8, sizeof(BYTE));
	          if (ptr == NULL) {
		           return;     // out of memory
	          }
	          hpFieldH.ptr = ptr;
	          hpFieldH.size = 8;
	          lpImage = (LPDWORD) ptr;
	          *lpImage = 0;
	          *(lpImage + 1) = offBits;
	          angleFlag = 0;

	          angle = -angle;
//	          angle = -1;
//	          printf(" angle= %f\n",angle);
//		       	fflush(stdout);

//		       	printf("---------------x1 = %d, x2= %d\n",(lpWorkBuffer->infFrame)[idF].x1, (lpWorkBuffer->infFrame)[idF].x2);
//		       	printf("---------------y1 = %d, y2= %d\n",lpWorkBuffer->line[idF].y1[j], lpWorkBuffer->line[idF].y2[j]);
//		       	printf("---------------Width = %d, hightImg= %d\n",lpWorkBuffer -> Width, lpWorkBuffer -> hightImg);
//		       	fflush(stdout);

 	err =
			extractField(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> Width, lpWorkBuffer -> hightImg,
					(lpWorkBuffer->infFrame)[idF].x1, lpWorkBuffer->line[idF].y1[j],
					(lpWorkBuffer->infFrame)[idF].x2, lpWorkBuffer->line[idF].y1[j],
					(lpWorkBuffer->infFrame)[idF].x1, lpWorkBuffer->line[idF].y2[j],
					&hpFieldH,
					&width,
					&height,
					8,
					"name",
					angleFlag,
					&angle
			);


//   	printf("-------------err= %d, width = %d, height= %d, widthBigImg= %d\n",err, width, height, widthBigImg);
//   	fflush(stdout);

	if(err < 0) return;


//	   saveBMPbuffer (hpFieldH.ptr, "testField.bmp", "c:\\Users\\Wicek\\OCR\\output\\");


//	printf("1 Ext---------------j = %d, heightBigImg= %d, heightCurImg= %d\n",j, heightBigImg, heightCurImg);
//   	fflush(stdout);

    addPartOfNewFieldFromExtract(ptrNext, hpFieldH.ptr+offBits, width, height, widthBigImg, heightBigImg, &heightCurImg);
    addWhiteImg(ptrNext, lpWorkBuffer -> widthImg, heightBigImg, &heightCurImg);

//	printf("2 Ext ---------------j = %d, heightBigImg= %d, heightCurImg= %d\n",j, heightBigImg, heightCurImg);
// 	fflush(stdout);

	           yl_free(hpFieldH.ptr);
        	   }
        	   else{

// add to new field
//        			printf("1 Img---------------j = %d, heightBigImg= %d, heightCurImg= %d\n",j, heightBigImg, heightCurImg);
//        	     	fflush(stdout);

        		   addPartOfNewFieldFromImg(ptrNext, lpWorkBuffer, idF, j, heightBigImg, &heightCurImg);
        		   addWhiteImg(ptrNext, lpWorkBuffer -> widthImg, heightBigImg, &heightCurImg);

//        		   printf("Img---------------j = %d, heightBigImg= %d, heightCurImg= %d\n",j, heightBigImg, heightCurImg);
//       	     	fflush(stdout);

        	   }

//printf("---------------j = %d, heightBigImg= %d\n",j, heightBigImg);
//	fflush(stdout);

           }

//   printf("createRestWhite---------------j = %d, heightBigImg= %d, heightCurImg= %d\n",j, heightBigImg, heightCurImg);
//           	fflush(stdout);

           createRestWhite(ptrNext, lpWorkBuffer -> widthImg, heightBigImg, heightCurImg);

       	   set4Bytes(bi->biHeight, heightBigImg);
       	   biWidthPad = ((widthBigImg+31)/32)*4;
       	   biSizeImage = biWidthPad*heightBigImg;
       	   set4Bytes(bf->bfSize, offBits + biSizeImage);
       	   set4Bytes(bi->biSizeImage, biSizeImage);


//printf("------------ heightBigImg= %d, lpWorkBuffer -> hightImg= %d\n", heightBigImg, lpWorkBuffer -> hightImg);
//printf("------------ widthBigImg= %d, heightBigImg= %d\n", widthBigImg, heightBigImg);
//fflush(stdout);

//       	   saveBMPbuffer (ptrBigImg, "testField.bmp", "c:\\Users\\Wicek\\OCR\\output\\");
       	}
//        printf("------------ heightBigImg= %d, lpWorkBuffer -> hightImg= %d\n", heightBigImg, lpWorkBuffer -> hightImg);
//        printf("------------ widthBigImg= %d, heightBigImg= %d\n", widthBigImg, heightBigImg);
//        fflush(stdout);

        if(*fieldBMPPtr != NULL)
       	           yl_free(*fieldBMPPtr);
        *fieldBMPPtr = ptrBigImg;
        (lpWorkBuffer -> lpRBuffer) = ptrBigImg+offBits;
        (lpWorkBuffer -> widthImg)  = biWidthPad;
        (lpWorkBuffer -> hightImg)  = heightBigImg;
//        (lpWorkBuffer -> begRect.x) = wXBegin;
//        (lpWorkBuffer -> begRect.y) = wYBegin;
        (lpWorkBuffer -> Height)    = heightBigImg;
        (lpWorkBuffer -> Width)     = widthBigImg;

//        printf("------------ heightBigImg= %d, lpWorkBuffer -> hightImg= %d\n", heightBigImg, lpWorkBuffer -> hightImg);
//        printf("------------ widthBigImg= %d, heightBigImg= %d\n", widthBigImg, heightBigImg);
//        fflush(stdout);

 	}

// replace data


	}
int getXTrapezoid_TL_corr(int *hHist){
	int i, th = 5;
	int k = 0;
	for(i=0;i<hHist[0]-2;i++){
		if(hHist[i] > th && hHist[i+1] > th && hHist[i+2] > th){
			k = i; break;
		}
	}
	i = k+20;
	if(i>hHist[0]) return k;
	if(hHist[i] < th || hHist[i+1] < th || hHist[i+2] < th)  return k;
	for(i=k+20;i<hHist[0]-2;i++){
		if(hHist[i] < th && hHist[i+1] < th){
			k = i; break;
		}
	}
	for(i=k;i<hHist[0]-2;i++){
		if(hHist[i] > th && hHist[i+1] > th && hHist[i+2] > th){
			k = i; break;
		}
	}

	return k;
}

int getXTrapezoid_TL(int *hHist){
	int i, th = 5;
	for(i=1;i<hHist[0]-2;i++){
		if(hHist[i] > th && hHist[i+1] > th && hHist[i+2] > th)
			return i;
	}
	return 0;
}
int getXTrapezoid_TR(int *hHist){
	int i, th = 8, k;
	int sum;

	sum = 0; k = hHist[0];
	for(i=hHist[0];i>2;i--){
		if(hHist[i] > th) sum++;
		else sum = 0;
        if(sum > 30) {k = i;break;}
	}
    if(k < hHist[0])
       return k+sum;
    return k;

//	for(i=hHist[0];i>2;i--){
//		if(hHist[i] > th && hHist[i-1] > th && hHist[i-2] > th)
//			return i;
//	}
//	return 0;
}
int selectPY1(int *py, int idxPy){
	int y = -1, i;
	int flag, stop = 0;
	int diffa_1, diffa_2;
	int diffb_1, diffb_2;
	int cc = 0;
	int th = 5;
	flag = 0;
	for(i=0;i<idxPy;i++){
		if(py[i] > 0) flag++;
		stop = i;
		if(py[i] == 0 && flag > 0) break;
	}

// printf(" -----  stop= %d\n",  stop);

	diffa_1 = -1;
	for(i=1;i<stop;i++){
		if(abs(py[i]-py[i-1]) < th) {diffa_1 = i-1;break;}
	}
//printf(" -----  diffa_1= %d\n",  diffa_1);

    if(diffa_1 < 0) return y;
    diffa_2 = stop-1;
	for(i=diffa_1+1;i<stop;i++){
		if(abs(py[i]-py[i-1]) >= th) {diffa_2 = i;break;}
	}
//printf(" -----  diffa_2= %d\n",  diffa_2);

    y = 0;
	for(i=diffa_1;i<=diffa_2;i++){
		y +=py[i];cc++;
	}
	if(cc>0) y /= cc;
//printf(" -----  y= %d, cc= %d\n",  y, cc);


	diffb_1 = -1;
	for(i=diffa_2+1;i<stop;i++){
		if(abs(py[i]-py[i-1]) < th) {diffb_1 = i-1;break;}
	}
//printf(" -----  diffb_1= %d\n",  diffb_1);

    if(diffb_1 < 0) return y;
    diffb_2 = stop-1;
	for(i=diffb_1+1;i<stop;i++){
		if(abs(py[i]-py[i-1]) >= th) {diffb_2 = i;break;}
	}
//printf(" -----  diffb_2= %d\n",  diffb_2);

    y = 0;cc = 0;
	for(i=diffb_1;i<=diffb_2;i++){
		y +=py[i];cc++;
	}
	if(cc>0) y /= cc;
//printf(" -----  y= %d, cc= %d\n",  y, cc);

    return y;
}

int getOriginalImageValue(LPBYTE lpRBuffer, int biWidthPad, int hightImg, int X, int Y){
	int v = 0;
	long nY;

	long nX;
	BYTE mask;
	LPBYTE ptrP;
	BYTE b;

	Y=hightImg-1-Y;
	nY=Y*biWidthPad;
	nX = (long)(X/8);
	ptrP = lpRBuffer+nY+nX;

	mask=LEFT_BIT;
	mask >>=(X%8);

	b=(~(*ptrP))&mask;
	if(b!=0)
		v = 0;
	else
		v = 1;

	return v;
}
void setNewValue(LPBYTE lpRBuffer, int biWidthPad, int hightImg, int X, int Y, int value){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;

//	Y=hightImg-1-Y;
	nY=Y*biWidthPad;
	nX = (long)(X/8);
	ptrP = lpRBuffer+nY+nX;

	mask=LEFT_BIT;
	mask >>=(X%8);
	if(value!=0)
		*ptrP = *ptrP | mask;
	else
		*ptrP = *ptrP & (~mask);
}

void lProfilePtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2, int *prf) {
	  int sww, i ,j, dx, x1D8, dy, sum, ind, w_bt;
	  LONG nY;
	  BYTE  *hptr;
	  int  *pp;
	  int tH = 3, r;

	              w_bt=widthImg;
	              x1D8=x1/8;
	              pp=(int *)prf;
	              dy=y2-y1+1;
	              dx=x2/8-x1D8-1;
	              *pp=dy;
	              pp++;i=0;
	              r = x1-x1D8*8;
	              r=8-r;
	              while(i<dy)  {
	                sum=0;
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                hptr=ptrPix+nY+(LONG)x1D8;

//	                ind=(int)(*hptr);
//	                if(ind<0) {ind+=256;}
//	                ind=255-ind;
//	                sum+=FirstZer(ind,x1%8);
	                hptr++;
	                *pp= -1;
	                for(j=0;j<dx;j++)  {
	                        ind=(int)(*hptr);
	                        if(ind<0) {ind+=256;}
	                        ind=255-ind;
	                        if(gr_ar[ind] > tH)
	                        	sum++;
	                        else
	                        	sum = 0;
	                        if(sum >1)
	                        	{*pp=r+j*8;break;}
	                        sum+=gr_ar[ind];
	                        hptr++;
	                        }
//	                if( (j+x1D8+1)<widthImg) {
//	                  ind=(int)(*hptr);
//	                  if(ind<0) {ind+=256;}
//	                  ind=255-ind;
//	                  sum+=LastZer(ind,8-x2%8);
//	                  }

//	                *pp=sum;
	                i++;
	                pp++;
	                }

    }/* of lProfilePtrPix*/

void rProfilePtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2, int *prf) {
	  int sww, i ,j, dx, x2D8, dy, sum, ind, w_bt;
	  LONG nY;
	  BYTE  *hptr;
	  int  *pp;
	  int tH = 3, r;

	              w_bt=widthImg;
	              x2D8=x2/8;
	              pp=(int *)prf;
	              dy=y2-y1+1;
	              dx=x2D8-x1/8-1;
	              *pp=dy;
	              pp++;i=0;
	              r = x2-x2D8*8;
//	              r=8-r;
	              while(i<dy)  {
	                sum=0;
	                sww=y1+i;
	                sww=hightImg-1-sww;
	                nY=(LONG)sww*(LONG)w_bt;
	                hptr=ptrPix+nY+(LONG)x2D8;

//	                ind=(int)(*hptr);
//	                if(ind<0) {ind+=256;}
//	                ind=255-ind;
//	                sum+=FirstZer(ind,x1%8);
	                hptr--;
	                *pp= -1;
	                for(j=0;j<dx;j++)  {
	                        ind=(int)(*hptr);
	                        if(ind<0) {ind+=256;}
	                        ind=255-ind;
	                        if(gr_ar[ind] > tH)
	                        	sum++;
	                        else
	                        	sum = 0;
	                        if(sum >1)
	                        	{*pp=r+j*8;break;}
	                        sum+=gr_ar[ind];
	                        hptr--;
	                        }
//	                if( (j+x1D8+1)<widthImg) {
//	                  ind=(int)(*hptr);
//	                  if(ind<0) {ind+=256;}
//	                  ind=255-ind;
//	                  sum+=LastZer(ind,8-x2%8);
//	                  }

//	                *pp=sum;
	                i++;
	                pp++;
	                }

    }/* of rProfilePtrPix*/

int getProfileShift(int *prf, int start, int n, int w){
	int i, j, k=start, cc;
	int stop, av, sh;
	int avT[n];
	float rt;

	stop = k+n;
	if(stop +w > prf[0]) return 0;
	for(i=k;i<stop;i++){
		av = 0;cc = 0;
		for(j=i;j<i+w;j++){
			av += prf[j];
			cc++;
		}
		if(cc > 0) av /= cc;
		avT[i-k] = av;
	}
	for(i=0;i<n;i++){
//  printf(" -----------------------------------\n");
		sh = 0;
		for(j=i+1;j<n;j++){
			rt = 0;
			if(avT[j] > 0)
				rt = (float)avT[i]/(float)avT[j];
			if(rt> 1.5){sh = 1;break;}
//		   printf(" ----  i= %d, j= %d, rt= %e\n",  i, j, rt);
		}
		if(sh == 0) return i;

	}
	return 0;
}
int getProfileShift_B(int *prf, int start, int n, int w){
int i, j, k=start, cc;
int stop, av, sh;
int avT[n];
float rt;

stop = k-n;
if(stop -w < 1) return 0;
for(i=k;i>stop;i--){
	av = 0;cc = 0;
	for(j=i;j>i-w;j--){
		av += prf[j];
		cc++;
	}
	if(cc > 0) av /= cc;
	avT[i-k] = av;
}
for(i=0;i<n;i++){
//  printf(" -----------------------------------\n");
	sh = 0;
	for(j=i+1;j<n;j++){
		rt = 0;
		if(avT[j] > 0)
			rt = (float)avT[i]/(float)avT[j];
		if(rt> 1.5){sh = 1;break;}
//		   printf(" ----  i= %d, j= %d, rt= %e\n",  i, j, rt);
	}
	if(sh == 0) return i;

}
return 0;
}

int calcYlFromProfile(int *prf, int *py1, int *py2){
	int i, k, sum, sum1, k1;
	int ret = -1;
	int stop = prf[0]/2+20;
	int shift;

	if(stop >= prf[0]) return ret;
	sum = 0;k = 0;
	for(i=0;i<stop;i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;

        if(sum > 60)
		   break;
	}
// printf(" 1----  sum= %d, k= %d\n",  sum, k);
	if(sum < 60) return ret;

	k1 = k;sum1 = 0;
	for(i=k+sum;i<prf[0]-10;i++){
		if(prf[i]>0)  {
			if(sum1 == 0) k1 = i;
			sum1++;
		}
		else sum1 = 0;

        if(sum1 > 10)
		   break;
	}
//printf(" 2----  sum1= %d, k1= %d\n",  sum1, k1);
    if(k1 == k) return ret;
    shift = getProfileShift(prf, k1, 10, 5);
	*py1 = k1+shift;
//	*px1 = prf[k1];

	sum = 0;k = k1;
	for(i=k1+sum1;i<prf[0];i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;
        if(sum > 30)
		   break;
	}
//printf(" 3----  sum= %d, k= %d\n",  sum, k);
    if(k1 == k) return ret;
    shift = getProfileShift_B(prf, k, 10, 5);
	*py2 = k-shift; ret = 1;
//	*px2 = prf[k];

	return ret;
}
int getAvFrom_profile(int *prf, int start, int n, int w){
	int i, j, k=start, cc;
	int stop, av, min;
	for(i=start;i<prf[0];i++){
		k = i;
		if(prf[i]>0) break;
	}
	stop = k+n;
	if(stop +w > prf[0]) return -1;
	min = 50000;
	for(i=k;i<stop;i++){
		av = 0;cc = 0;
		for(j=i;j<i+w;j++){
			av += prf[j];
			cc++;
		}
		if(cc > 0) av /= cc;
		if(av<min) min = av;
	}
	return min;
}

int calcYrFromProfile(int *prf, int *py1, int *py2, int w){
	int i, k, sum, sum1, k1, stop;
	int ret = -1;
	int av_1, av_2;
	int shift;

	stop = prf[0]/2+50;
	if(stop >= prf[0]) return ret;


	for(i=1;i<prf[0];i++){
		if(w-prf[i]<w/7)
			prf[i] = -1;
	}


	sum = 0;k = 0;
	for(i=1;i<stop ;i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;

        if(sum > 40)
		   break;
	}
 //printf(" 1----  sum= %d, k= %d\n",  sum, k);
	if(sum < 40) return ret;

	av_1 = 0;av_2 = 0;
	if(sum < 60) {
       av_1 = getAvFrom_profile(prf, k+sum, 15, 10);
       k1 = k;sum1 = 0;
   	   for(i=k+sum;i<stop;i++){
   		if(prf[i]<0)  {
   			if(sum1 == 0) k1 = i;
   			sum1++;
   		}
   		else sum1 = 0;
           if(sum1 > 40)
   		   break;
   	   }
//printf(" 1.5----  sum1= %d, k1= %d\n",  sum1, k1);
   	   if(sum1 > 40 && av_1 > 0) {
   		  av_2 = getAvFrom_profile(prf, k1+sum1, 15, 10);
//printf(" 1.5----  av_1= %d, av_2= %d\n",  av_1, av_2);
          if(av_2 < av_1 && av_2 > 0){
        	  sum = sum1;k = k1;
          }
   	   }
	}

//printf(" 1.6----  sum= %d, k= %d\n",  sum, k);

	k1 = k;sum1 = 0;
	for(i=k+sum;i<prf[0]-10;i++){
		if(prf[i]>0)  {
			if(sum1 == 0) k1 = i;
			sum1++;
		}
		else sum1 = 0;

        if(sum1 > 10)
		   break;
	}
//printf(" 2----  sum1= %d, k1= %d\n",  sum1, k1);
    if(k1 == k) return ret;

	sum = 0;k = k1;
	for(i=k1+sum1;i<prf[0];i++){
		if(w-prf[i]<w/8)  {
			prf[i] = -1;
		}
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;
        if(sum > 1)
		   break;
	}

//printf(" 3----  sum= %d, k= %d\n",  sum, k);
    if(k1 == k) return ret;
	k1 = k;sum1 = 0;
	stop = k+sum+20;
	for(i=k+sum;i<prf[0]-10;i++){
		if(prf[i]>0)  {
			if(sum1 == 0) k1 = i;
			sum1++;
		}
		else sum1 = 0;
        if(sum1 > 1)
		   break;
	}

//printf(" 4----  sum1= %d, k1= %d\n",  sum1, k1);
    if(k1 == k) return ret;
	*py1 = k1;
//	*px1 = prf[k1];

	sum = 0;k = k1;
	for(i=k1+sum1;i<prf[0];i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;
        if(sum > 30)
		   break;
	}
//printf(" 5----  sum= %d, k= %d\n",  sum, k);
    if(k1 == k) return ret;
    shift = getProfileShift_B(prf, k, 10, 5);
	*py2 = k-shift; ret = 1;
//	*px2 = prf[k];
	return ret;
}
int calcYrFromProfileFantasy5(int *prf, int *py1, int *py2, int w){
	int i, k, sum, sum1, k1, stop;
	int ret = -1;
	int av_1, av_2;
	int shift;

	stop = prf[0]/2+50;
	if(stop >= prf[0]) return ret;


	for(i=1;i<prf[0];i++){
		if(w-prf[i]<w/7)
			prf[i] = -1;
	}


	sum = 0;k = 0;
	for(i=1;i<stop ;i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;

        if(sum > 40)
		   break;
	}
// printf(" 1----  sum= %d, k= %d\n",  sum, k);
	if(sum < 40) return ret;

	av_1 = 0;av_2 = 0;
	if(sum < 60) {
       av_1 = getAvFrom_profile(prf, k+sum, 15, 10);
       k1 = k;sum1 = 0;
   	   for(i=k+sum;i<stop;i++){
   		if(prf[i]<0)  {
   			if(sum1 == 0) k1 = i;
   			sum1++;
   		}
   		else sum1 = 0;
           if(sum1 > 40)
   		   break;
   	   }
//printf(" 1.5----  sum1= %d, k1= %d\n",  sum1, k1);
   	   if(sum1 > 40 && av_1 > 0) {
   		  av_2 = getAvFrom_profile(prf, k1+sum1, 15, 10);
//printf(" 1.5----  av_1= %d, av_2= %d\n",  av_1, av_2);
          if(av_2 < av_1 && av_2 > 0){
        	  sum = sum1;k = k1;
          }
   	   }
	}

//printf(" 1.6----  sum= %d, k= %d\n",  sum, k);

	k1 = k;sum1 = 0;
	for(i=k+sum;i<prf[0]-10;i++){
		if(prf[i]>0)  {
			if(sum1 == 0) k1 = i;
			sum1++;
		}
		else sum1 = 0;

        if(sum1 > 10)
		   break;
	}
//printf(" 2----  sum1= %d, k1= %d\n",  sum1, k1);
    if(k1 == k) return ret;
     shift = getProfileShift(prf, k1, 10, 5);
//  printf(" ----  shift= %d\n",  shift);
	*py1 = k1+shift;

	sum = 0;k = k1;
	for(i=k1+sum1;i<prf[0];i++){
		if(prf[i]<0)  {
			if(sum == 0) k = i;
			sum++;
		}
		else sum = 0;
        if(sum > 30)
		   break;
	}
//printf(" 5----  sum= %d, k= %d\n",  sum, k);
    if(k1 == k) return ret;
    shift = getProfileShift_B(prf, k, 10, 5);
	*py2 = k-shift; ret = 1;
//	*px2 = prf[k];
	return ret;
}

int correctTrapezoid(STRUCTWB *lpWorkBuffer, LPBYTE *fieldBMPPtr, char* imgdName){
	{
    int maxSize = H_SIZE_PREPROC;
//    int n;
    int px1, py1, px2, py2;
    int px3, py3, px4, py4;
//    int py[20], idxPy, xx;
    int hh;

//     char buffLog[500];

    if(maxSize > lpWorkBuffer -> Width-1) maxSize = lpWorkBuffer -> Width-1;


    { int lProfile[lpWorkBuffer -> hightImg];
      int rProfile[lpWorkBuffer -> hightImg];
      int ret, wPrf;

        wPrf = lpWorkBuffer -> Width/2-100;
    	lProfilePtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
      		           8, wPrf, 10, lpWorkBuffer -> hightImg-10, lProfile);

    	wPrf = lpWorkBuffer -> Width/2+100;
    	rProfilePtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    			wPrf, lpWorkBuffer -> Width-8, 10, lpWorkBuffer -> hightImg-10, rProfile);

/*
    	{int n;
    	char buffLog[500];
   		  for(n=0;n<lProfile[0];n++) {
		   		     sprintf(buffLog," -----  lProfile[ %d ]= %d\n", n, lProfile[n]);
		   		     saveLog("", buffLog);
		      }
    	}
*/
    	ret = calcYlFromProfile(lProfile, &py1,&py4);
// 	  printf(" 1-----  ret= %d\n",  ret);
    	if(ret < 0) return ret;
    	 if(ticketTypeInt == 5)
    		 ret = calcYrFromProfileFantasy5(rProfile, &py2,&py3, wPrf-8);
    	 else
    	    ret = calcYrFromProfile(rProfile, &py2,&py3, wPrf-8);
/*
    	{int n;
    	char buffLog[500];
   		  for(n=0;n<rProfile[0];n++) {
		   		     sprintf(buffLog," -----  rProfile[ %d ]= %d\n", n, rProfile[n]);
		   		     saveLog("", buffLog);
		      }
    	}
*/
// x
    	 {int yStart = py1-50;
    	    if(yStart<10) yStart = 10;
    	  	hh = (2*lpWorkBuffer -> hightImg)/3;
    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    	    		   maxSize, 20, lpWorkBuffer -> Width/2, yStart, py3, hHistPreProc);
//    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
//    	    		   maxSize, 20, lpWorkBuffer -> Width/2, 20, hh, hHistPreProc);
/*
        	{int n;
        	char buffLog[500];

        	 		   		  for(n=0;n<hHistPreProc[0];n++) {
        	   		   		     sprintf(buffLog," -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
        	    		   		     saveLog("", buffLog);
        	    		   		  }

        	}
*/
    	  	px1 = getXTrapezoid_TL(hHistPreProc);
    	//  printf(" -----  px1= %d\n",  px1);

       	    yStart = py2-50;
       	    if(yStart<10) yStart = 10;

    	  	hh = (2*lpWorkBuffer -> hightImg)/3;
    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    	    		   maxSize, lpWorkBuffer -> Width/2, lpWorkBuffer -> Width-20, yStart, py3, hHistPreProc);
//    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
//    	    		   maxSize, lpWorkBuffer -> Width/2, lpWorkBuffer -> Width-20, 20, hh, hHistPreProc);
    	  	px2 = getXTrapezoid_TR(hHistPreProc);
    	//    sprintf(buffLog," \n--------------------------  px2= %d\n", px2);
    	//    saveLog("", buffLog);

    	    px2 += lpWorkBuffer -> Width/2;


    	// 		   		  for(n=0;n<hHistPreProc[0];n++) {
    	//   		   		     sprintf(buffLog," -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
    	//    		   		     saveLog("", buffLog);
    	//    		   		  }

       	    yStart = py4+50;
       	    if(yStart>lpWorkBuffer -> hightImg-10) yStart = lpWorkBuffer -> hightImg-10;

    	  	hh = lpWorkBuffer -> hightImg/3;
    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    	    		   maxSize, 20, lpWorkBuffer -> Width/2, py2, yStart, hHistPreProc);
//    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
//    	    		   maxSize, 20, lpWorkBuffer -> Width/2, hh, lpWorkBuffer -> hightImg-20, hHistPreProc);

    	  	px4 = getXTrapezoid_TL(hHistPreProc);


       	    yStart = py3+50;
       	    if(yStart>lpWorkBuffer -> hightImg-10) yStart = lpWorkBuffer -> hightImg-10;
    	  	hh = lpWorkBuffer -> hightImg/3;
    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    	    		   maxSize, lpWorkBuffer -> Width/2, lpWorkBuffer -> Width-20, py2, yStart, hHistPreProc);
//    	  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
//    	    		   maxSize, lpWorkBuffer -> Width/2, lpWorkBuffer -> Width-20, hh, lpWorkBuffer -> hightImg-20, hHistPreProc);
    	  	px3 = getXTrapezoid_TR(hHistPreProc);

    	//    sprintf(buffLog," \n--------------------------  px3= %d\n", px3);
    	//    saveLog("", buffLog);

    	    px3 += lpWorkBuffer -> Width/2;

    	//    printf(" -----  px3= %d, lpWorkBuffer -> Width-px3= %d\n",  px3, lpWorkBuffer -> Width-px3);
    	 }

//     	  printf(" 2-----  ret= %d\n",  ret);
    	if(ret < 0) return ret;
        py1 += 10; py4 += 10;
        py2 += 10; py3 += 10;

//            printf(" -----  px1= %d, px2= %d, px3= %d, px4= %d\n", px1, px2, px3, px4);
//            printf(" -----  py1= %d, py2= %d, py3= %d, py4= %d\n", py1, py2, py3, py4);

//  printf(" -----  lpWorkBuffer -> hightImg-py3= %d, lpWorkBuffer -> hightImg-py4= %d\n",  lpWorkBuffer -> hightImg-py3, lpWorkBuffer -> hightImg-py4);
  {int rtn = 0;
  if(abs(py1 -py2)> 60)  rtn = -1;
  if(abs(py3 -py4)> 60)  rtn = -1;
  if(abs(px1 -px4)> 60)  rtn = -1;
  if(abs(px2 -px3)> 60)  rtn = -1;
  if(rtn < 0) return rtn;
  }

/*
		     sprintf(buffLog," \n--------------------------  lProfile, w= %d\n", lpWorkBuffer -> Width/2-8);
		     saveLog("", buffLog);

    	  		   		  for(n=0;n<lProfile[0];n++) {
    	    		   		     sprintf(buffLog," -----  lProfile[ %d ]= %d\n", n, lProfile[n]);
    	     		   		     saveLog("", buffLog);
    	     		   		  }

            sprintf(buffLog," -----  rProfile, w= %d\n", lpWorkBuffer -> Width/2-8);
            saveLog("", buffLog);

    	  		   		  for(n=0;n<rProfile[0];n++) {
    	  	   		   		     sprintf(buffLog," -----  rProfile[ %d ]= %d\n", n, rProfile[n]);
    	  	   		   		     saveLog("", buffLog);
    	  	   		      }

*/
    }



//  	printf(" -----  px1= %d, px2= %d, px3= %d, px4= %d\n", px1, px2, px3, px4);
//   		   		  for(n=0;n<hHistPreProc[0];n++) {
//     		   		     sprintf(buffLog," -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
//     		   		     saveLog("", buffLog);
//     		   		  }

//  tranformation from trapezoid to rectangle
//    printf(" -----  px1= %d, px2= %d, px3= %d, px4= %d\n", px1, px2, px3, px4);
//    printf(" -----  py1= %d, py2= %d, py3= %d, py4= %d\n", py1, py2, py3, py4);

    {
       int Nx, Ny;


   	LPBYTE ptrNewImg;
   	WORD widthNewImg;
   	WORD heightNewImg;
   	LPBYTE ptrNext;

   	DWORD        offBits;
   	LPBITMAPFILEHEADER bf;
   	LPBITMAPINFOHEADER bi;
   	LPRGBQUAD          qd;
    int  biSizeImage, biWidthPad;
    int newSize;

   	offBits = sizeof (BITMAPFILEHEADER) +
   			sizeof (BITMAPINFOHEADER) +
   			sizeof (RGBQUAD) * 2;





          Nx = (px2-px1+px3-px4)/2;
          Ny = (py4-py1+py3-py2)/2;
//          Nx = (px1-px2+px4-px3)/2;
//          Ny = (py4-py1+py3-py2)/2;

//          printf(" -----  Nx= %d, Ny= %d\n", Nx, Ny);
//fflush(stdout);

   	  newSize = offBits + Nx * Ny;
   	  ptrNewImg = yl_calloc("OCR_B", newSize, sizeof(BYTE));

  	   widthNewImg = Nx;
  	   heightNewImg = Ny;
   	   biWidthPad = ((widthNewImg+31)/32)*4;


  	  bf = (LPBITMAPFILEHEADER) ptrNewImg;
  	  bf->bfType[0] = 0x42;
  	  bf->bfType[1] = 0x4d;
   	  set4Bytes(bf->bfSize, offBits + biWidthPad * heightNewImg);
  	  bf->bfReserved1[0] = 0;
  	  bf->bfReserved1[1] = 0;
  	  set4Bytes(bf->bfOffBits, offBits);


  	// fill in BITMAPINFOHEADER

   	  biSizeImage = biWidthPad*heightNewImg;

  	  bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
  	  set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
  	  set4Bytes(bi->biWidth, widthNewImg);
  	  set4Bytes(bi->biHeight, heightNewImg);
  	  bi->biPlanes[0] = 1;
  	  bi->biPlanes[1] = 0;
  	  bi->biBitCount[0]= 1;
  	  bi->biBitCount[1]= 0;
  	  set4Bytes(bi->biCompression, BI_RGB);
   	  set4Bytes(bi->biSizeImage, biSizeImage);
  	  set4Bytes(bi->biXPelsPerMeter, lpWorkBuffer->Dpi.x);
  	  set4Bytes(bi->biYPelsPerMeter, lpWorkBuffer->Dpi.y);
  	  set4Bytes(bi->biClrUsed, 0);
  	  set4Bytes(bi->biClrImportant, 0);

  	// generate pair of RGBQUADs (black and white)
  	  qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
  	  *(LPDWORD)qd       = 0x00000000;
  	  *(LPDWORD)(qd + 1) = 0x00ffffff;

  	  ptrNext = ptrNewImg+offBits;

  	  {int newX, newY;
  	   int oldX, oldY, m;
  	   int p1y, p1x, p2y, p2x, p3y, p3x, p4y, p4x;
  	   int value;
//  	   int maxX= 0, maxY= 0;
  		  for(newY=0;newY<Ny;newY++){
  	  		  for(newX=0;newX<Nx;newX++){
//  	  			   p1y = (newY*px2+(Nx-newY)*px1)/Nx;
//  	  			   p1x = (newY*py2+(Nx-newY)*py1)/Nx;

  	  			   p1y = (newX*px2+(Nx-newX)*px1)/Nx;
  	  			   p1x = (newX*py2+(Nx-newX)*py1)/Nx;

//  	  			   p2y = (newX*px2+(Ny-newX)*px3)/Ny;
//  	  			   p2x = (newX*py2+(Ny-newX)*py3)/Ny;

  	  			   p2y = (newY*px2+(Ny-newY)*px3)/Ny;
  	  			   p2x = (newY*py2+(Ny-newY)*py3)/Ny;

//  	  			   p3y = (newY*px3+(Nx-newY)*px4)/Nx;
//  	  			   p3x = (newY*py3+(Nx-newY)*py4)/Nx;

  	  			   p3y = (newX*px3+(Nx-newX)*px4)/Nx;
  	  			   p3x = (newX*py3+(Nx-newX)*py4)/Nx;

//  	  			   p4y = (newX*px1+(Ny-newX)*px4)/Ny;
//  	  			   p4x = (newX*py1+(Ny-newX)*py4)/Ny;

  	  			   p4y = (newY*px1+(Ny-newY)*px4)/Ny;
  	  			   p4x = (newY*py1+(Ny-newY)*py4)/Ny;

  	  			   oldX =(p3y*(p3x-p1x)*(p4y-p2y) - p3x*(p3y-p1y)*(p4y-p2y) -
  	  					  p4y*(p4x-p2x)*(p3y-p1y) + p4x*(p3y-p1y)*(p4y-p2y));
  	  			   m = (p3x-p1x)*(p4y-p2y)-(p4x-p2x)*(p3y-p1y);


  	  			   if(m != 0) oldX /= m;
  	  			   oldY =(p3x*(p3y-p1y)*(p4x-p2x) - p3y*(p3x-p1x)*(p4x-p2x) -
  	  					  p4x*(p4y-p2y)*(p3x-p1x) + p4y*(p3x-p1x)*(p4x-p2x));
  	  			   m = (p3y-p1y)*(p4x-p2x)-(p4y-p2y)*(p3x-p1x);
  	  			   if(m != 0) oldY /= m;

//if(maxX < oldX) maxX= oldX;
//if(maxY < oldY) maxY= oldY;

  	  			 value = getOriginalImageValue(lpWorkBuffer->lpRBuffer,lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg, oldX, oldY);
  	  			 setNewValue(ptrNext, biWidthPad, heightNewImg, newX, newY, value);
  	  		  }
  		  }
// printf(" -----  maxX= %d, maxY= %d\n", maxX, maxY);
//fflush(stdout);

  	  }
      if(*fieldBMPPtr != NULL)
     	           yl_free(*fieldBMPPtr);
      *fieldBMPPtr = ptrNewImg;
      (lpWorkBuffer -> lpRBuffer) = ptrNewImg+offBits;
      (lpWorkBuffer -> widthImg)  = biWidthPad;
      (lpWorkBuffer -> hightImg)  = heightNewImg;
      (lpWorkBuffer -> Height)    = heightNewImg;
      (lpWorkBuffer -> Width)     = widthNewImg;


//      saveBMPbuffer (ptrNewImg, imgdName, "c:\\Users\\Wicek\\OCR\\output\\");
//      printf(" -----  trapezoid\n");
//      fflush(stdout);



    }

	}
	return 1;
}

 int getNextMarkBlack(int *hHistPreProc, int mPrev){
	 int mark = -1;
	 int i, sum, th1 = 3, th2 = 3;
	 sum = 0;
	 for(i = mPrev;i<hHistPreProc[0];i++){
		 if(hHistPreProc[i] > th1) sum++;
		 else sum = 0;
		 if(sum > th2) {mark = i;break;}
	 }
	 return mark-sum;
 }
 int getNextMarkWhite(int *hHistPreProc, int mPrev){
	 int mark = -1;
	 int i, sum, th1 = 1, th2 = 1;
	 sum = 0;
	 for(i = mPrev;i<hHistPreProc[0];i++){
		 if(hHistPreProc[i] < th1) sum++;
		 else sum = 0;
		 if(sum > th2) {mark = i;break;}
	 }
	 return mark-sum;
 }

 void setMark( STRUCTWB *lpWorkBuffer, int mL, int mR, int y1, int y2){

     if(lpWorkBuffer ->indUnpE >= NUMBBOX) return;

	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].lT.y = y1;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].lB.y = y1;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].rT.y = y2;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].rB.y = y2;

	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].lT.x = mL;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].lB.x = mL;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].rT.x = mR;
	 lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpE)].rB.x = mR;
     lpWorkBuffer ->indUnpE++;


 }
 int getMassW(int *hHistPreProc, int mL, int mR, int *iStart, int *iStop){
	 int mass = 0, n;
	 int th = 2;
	  *iStart = 0;
	  *iStop = 0;
	  for(n=mL;n<mR;n++) {
		  if(hHistPreProc[n] > th){
			  if(*iStart == 0)
			      *iStart = n-1;
			  *iStop = n+1;
		  }
		   mass += hHistPreProc[n];
	  }

	 return mass;
 }
	void cleanImageW(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2, int y1, int y2){
		int   j, kk, curInd, yy;
		int  yD; //, ch;
		int lByte, rByte, rL, rR, w_bt;
		long nY;
		BYTE *hptr= ptrPix;
		BYTE mask;

		w_bt= widthImg;
		lByte=x1/8;
		rL   =x1%8;
		if(rL!=0) {lByte++;}

		rByte=x2/8;
		rR   =x2%8;
		if(rR!=7) {rByte--;}

		for(yy=y1;yy<=y2;yy++) {
			yD=yy;
			yD= hightImg-1-yD;
			nY=(long)yD*(long)w_bt;
			hptr=ptrPix+nY+(long)(lByte-1);

			curInd=1;
			if(rL!=0) {
				curInd=9-rL;
					mask=LEFT_BIT;
					mask >>=rL;
					for(kk=1;kk<curInd;kk++) {
						hptr[0] = hptr[0]  | mask;
						mask >>=1;
					}
			}

			for(j=lByte;j<=rByte;j++) {
				hptr+=1L;
					mask=LEFT_BIT;
					for(kk=0;kk<8;kk++) {
						hptr[0] = hptr[0]  | mask;
						mask >>=1;
					}
				}

			if(rR!=7) {
				hptr+=1L;
					mask=LEFT_BIT;
					for(kk=0;kk<=rR;kk++) {
						hptr[0] = hptr[0]  | mask;
						mask >>=1;
					}
			}
		}
	}/* of histVPtrPix()*/

 void printSegm(STRUCTWB * lpWorkBuffer, char *str);
int SegmCharsNumbers_2(STRUCTWB *lpWorkBuffer, int kindD, int *lpLwidth, char *fieldName, int sizeW){
    int maxSize = H_SIZE_PREPROC;
    int markL, markR, dd, maxW = 40, maxSp = 38;
    int thLinkSp = 17, thLinkBlck = 28;
    int idxL, idxR;
    int mR, mL, i, n, ret, flag;
    int ySt1, ySt2, mass;

    int flgPrnt;

     char buffLog[1000];

     lpWorkBuffer ->indUnpSp = 0;
     lpWorkBuffer ->indUnpB = 0;
     lpWorkBuffer ->indUnpE = 0;


//		     printf(" -----  (lpWorkBuffer->curLine[0]).y1= %d\n", (lpWorkBuffer->curLine[0]).y1);
//		     sprintf(buffLog," -----  (lpWorkBuffer->curLine[0]).y1= %d\n", (lpWorkBuffer->curLine[0]).y1);
//		     saveLog(NULL, buffLog);
     flgPrnt = 0;
//     if((lpWorkBuffer->curLine[0]).y1 == 1) flgPrnt = 1;

    if(maxSize > lpWorkBuffer -> Width-1) maxSize = lpWorkBuffer -> Width-1;

    ySt1 = (lpWorkBuffer->curLine[0]).y1;
    if(ySt1 > 10) ySt1 -= 10;
    ySt2 = (lpWorkBuffer->curLine[0]).y2;
    if(ySt2 < lpWorkBuffer -> hightImg - 21) ySt2 += 20;


  	histVPtrPix(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
    		   maxSize, (lpWorkBuffer->curLine[0]).x1, (lpWorkBuffer->curLine[0]).x2,
    		   ySt1, ySt2, hHistPreProc);


  	if(flgPrnt == 1){
//  		buffLog[0] = 0;
  		saveLog(" \n "," ");
	     sprintf(buffLog," ----- \n");
	     saveLog(NULL, buffLog);
  	   		   		  for(n=0;n<hHistPreProc[0];n++) {
  	     		   		     sprintf(buffLog," -----  hHistPreProc[ %d ]= %d\n", n, hHistPreProc[n]);
  	     		   		     saveLog(NULL, buffLog);
  	     		   		  }
  	}


    markL = 0;flag = 0;idxL = 1;
  	while(markL >= 0 && lpWorkBuffer ->indUnpE < NUMBBOX) {
  	   if(flag == 0) {
  	     idxR = getNextMarkBlack(hHistPreProc, idxL);

if(flgPrnt)
  printf(" ----- flag == 0; idxL= %d, idxR= %d\n", idxL, idxR);

         if(idxR < 0) {if(idxL < hHistPreProc[0]-10) idxR = hHistPreProc[0]; else break;}
         markR = idxR-1;
         mR = markR-1;
         mL = markL;
         dd = mR-mL;
         n = 1;
         if(dd > 40){
        	 int iStart, iStop;
            mass = getMassW(hHistPreProc, mL+6, mR-10, &iStart, &iStop);
            if(mass > 10 && mass < 40 && iStop - iStart < 10) {
if(flgPrnt)
  printf(" ----- mass= %d, iStart= %d, iStop= %d\n", mass, iStart, iStop);

            	cleanImageW(lpWorkBuffer->lpRBuffer, lpWorkBuffer -> widthImg, lpWorkBuffer -> hightImg,
            			iStart, iStop, (lpWorkBuffer->curLine[0]).y1+1, (lpWorkBuffer->curLine[0]).y2-1);
            }
         }

	     if(dd > maxW){
		     n = dd/maxW;
	     }
	     if(n > 1) {
	        for(i=0;i<n-1;i++){
	    	   mR = mL + maxW;
	    	   setMark(lpWorkBuffer, mL+lpWorkBuffer->curLine[0].x1, mR+lpWorkBuffer->curLine[0].x1, (lpWorkBuffer->curLine[0]).y1, (lpWorkBuffer->curLine[0]).y2);
	    	   mL= mR;
	        }
	     }
         mR = markR-1;

if(flgPrnt)
  printf(" -----setMark;  mL= %d, mR= %d\n", mL, mR);

 	     setMark(lpWorkBuffer, mL+lpWorkBuffer->curLine[0].x1, mR+lpWorkBuffer->curLine[0].x1, (lpWorkBuffer->curLine[0]).y1, (lpWorkBuffer->curLine[0]).y2);
	     markL = mR;
	     idxL = idxR;
  	   }


if(flgPrnt)
  printf(" -----getNextMarkWhite;  idxL= %d\n",  idxL);

	   idxR = getNextMarkWhite(hHistPreProc, idxL);

if(flgPrnt)
 printf(" -----  idxL= %d, idxR= %d\n", idxL, idxR);

       if(idxR < 0) {if(idxL < hHistPreProc[0]-10) idxR = hHistPreProc[0];else break;}
       markR = idxR;

  	   mL = idxR;
  	   mR = getNextMarkBlack(hHistPreProc, mL);
       if(mR < 0) {if(mL <= hHistPreProc[0]) mR = hHistPreProc[0];else break;}

  if(flgPrnt)
     printf(" -----  mR-mL= %d, maxSp= %d\n", mR-mL, maxSp);



       if(flag == 1){
          if(markR-markL < thLinkBlck && mR-mL < thLinkSp){
        	  int linkR = getNextMarkWhite(hHistPreProc, mR);
              if(linkR-mR < thLinkBlck){
            	  markR = linkR;
            	  idxR = linkR;

       	  if(flgPrnt)
           	   printf(" -----  linkR= %d\n", linkR);

             	  mL = idxR;
             	  mR = getNextMarkBlack(hHistPreProc, mL);
                  if(mR < 0) {if(mL <= hHistPreProc[0]) mR = hHistPreProc[0];else break;}
              }
          }
       }

       if(flag == 0){
    	   int idxR_2, idxR_w;
    	   int linkR;
    	   idxR_2 = getNextMarkWhite(hHistPreProc, mR);
    	   idxR_w = getNextMarkBlack(hHistPreProc, idxR_2);

 if(flgPrnt)
  	   printf(" --flag == 0;---  markR-markL= %d, mR-mL= %d, idxR_2- mR= %d, idxR_w - idxR_2= %d\n", markR-markL, mR-mL, idxR_2- mR, idxR_w - idxR_2);

           if(markR-markL < thLinkBlck && mR-mL < thLinkSp && idxR_2- mR < thLinkBlck && idxR_w - idxR_2 < thLinkSp){
        	   linkR = idxR_2;
           	   markR = linkR;
               idxR = linkR;

       	  if(flgPrnt)
           	   printf(" --flag == 0;---  linkR= %d\n", linkR);

           	  mL = idxR;
           	  mR = idxR_w;
          }
       }


       flag = 0;
  	   if(mR-mL < maxSp) {
  		 markR += (mR-mL)/2;
  		 flag = 1;
  	   }

if(flgPrnt)
 printf(" ----setMark;  mR-mL= %d, maxSp= %d, markL= %d, markR= %d, flag= %d\n", mR-mL, maxSp, markL, markR, flag);

 	   setMark(lpWorkBuffer, markL+lpWorkBuffer->curLine[0].x1, markR+lpWorkBuffer->curLine[0].x1, (lpWorkBuffer->curLine[0]).y1, (lpWorkBuffer->curLine[0]).y2);
 	   markL = markR;
 	   idxL = idxR;
 	   if(flag == 1)
 		  idxL = mR;

 if(flgPrnt)
     printf(" -----  idxL= %d, markL= %d-------end\n", idxL, markL);


  }
  	lpWorkBuffer->  lettWidth = maxW;
  	lpWorkBuffer->newLine = 0;
    ret=sendNextFont(lpWorkBuffer, fieldName);


/*
//  	 printSegm(lpWorkBuffer, " test ");
    {
       	DWORD        offBits;
       	LPBITMAPFILEHEADER bf;
       	LPBITMAPINFOHEADER bi;

       	offBits = sizeof (BITMAPFILEHEADER) +
       			sizeof (BITMAPINFOHEADER) +
       			sizeof (RGBQUAD) * 2;


    saveBMPbuffer (lpWorkBuffer -> lpRBuffer-offBits, "testClenW.bmp", "c:\\Users\\Wicek\\OCR\\output\\");
    }
*/

  	return ret;
}

void printSegm(STRUCTWB * lpWorkBuffer, char *str){
	char buff[500];
	int i;
	   sprintf(buff,"    ----------------------     new line     ------------------------------- %s            \n", str);
	   saveLog("", buff);

	   sprintf(buff,"indUnpE= %d, indUnpB= %d, indUnpSp= %d\n", lpWorkBuffer->indUnpE, lpWorkBuffer->indUnpB, lpWorkBuffer ->indUnpSp);
	   saveLog("", buff);

	for(i=0;i<lpWorkBuffer->indUnpE;i++){
	   sprintf(buff,"---   lT.x= %d, lT.y= %d\n", (lpWorkBuffer -> arrCellP[i]).lT.x, (lpWorkBuffer -> arrCellP[i]).lT.y);
	   saveLog("", buff);
	   sprintf(buff,"---   lB.x= %d, lB.y= %d\n", (lpWorkBuffer -> arrCellP[i]).lB.x, (lpWorkBuffer -> arrCellP[i]).lB.y);
	   saveLog("", buff);
	   sprintf(buff,"---   rT.x= %d, rT.y= %d\n", (lpWorkBuffer -> arrCellP[i]).rT.x, (lpWorkBuffer -> arrCellP[i]).rT.y);
	   saveLog("", buff);
	   sprintf(buff,"---   rB.x= %d, rB.y= %d\n", (lpWorkBuffer -> arrCellP[i]).rB.x, (lpWorkBuffer -> arrCellP[i]).rB.y);
	   saveLog("", buff);
	}
}

