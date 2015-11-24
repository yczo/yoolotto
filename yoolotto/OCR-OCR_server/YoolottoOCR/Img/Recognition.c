/*
 * Recognition.c
 *
 *  Created on: Aug 30, 2012
 *      Author: Wicek
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include  "../headers/Image.h"
#include  "../headers/Functions.h"
#include  "../headers/Prototype.h"
#include  "../headers/CommonRec.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include "../headers/logging.h"

int charSaveIndex;
int lineSaveIndex;
void saveCharacter(LPBYTE buffer, WORD width, WORD height, int lineIdx, int charIdx, char* imgName);

int Recognition(LPIMGDATA fieldImg, LPTICKETDATA ticketDataPtr, BYTE* pFieldDescr, WORD wDpiX0,  WORD wDpiY0,  WORD wCorrectionLevel,
		        STRINGS * pResults, WORD* pwConf, QUADRANGLE* pPattern){
 WORD wDpiX;
 WORD wDpiY;

// LOGD("In Recognition(, pResults=%p", pResults);

 int i;
 int err;
 long shift;
 WORD* pwFieldDescr;
 WORD* pwFieldContents;
 char* lpszFieldTitle;

 WORD originX0;
 WORD originY0;

 WORD width;
 WORD height;

 BYTE* lpFieldBuffer;

 LPBYTE lpWorkBuffer;
 LPBYTE lpExtractCh;
 BYTE normPict[300];
 LPBYTE lpNormPict;
 WORD nNormVersion;
 WORD size = 15;
 WORD wWidthSegment = 192;
 WORD wHeightSegment = 192;
// WORD wRecMode = 0;
// int bRecContinue = 0;
 char* lpCand1Buffer;
 char* lpCand2Buffer;
 char* lpResultBuffer;
 char* lpRejectBuffer;

 WORD* pxOrigin;
 WORD* pyOrigin;
 WORD* pxCorner;
 WORD * pyCorner;
 WORD xOrig;
 WORD yOrig;
 WORD xCorn;
 WORD yCorn;

 WORD wFieldNoFD;
 WORD widthRec0;
 WORD heightRec0;
 WORD wFieldType;
 WORD wFieldAlphabet;
 WORD wCellWidth;
 WORD wLineHeight;
 WORD wDictIndex;
 WORD wContext;

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
 WORD alphaMap[8];
 LPBYTE lpAlphaMap = (LPBYTE)&alphaMap[0];
 int iWidthFrame;
 int iHeightFrame;
 EXTRACTCHAR extractChar;
 EXTRACTCHAR * lpExtractChar;
 RECCHAR recChar;
 RECCHAR * lpRecChar;
 int iRecMode;
 int iRecContinue;
 STRINGS * lpStrings;
 COORDINATES coordinates;
 COORDINATES * lpCoordinates;

 WORD wCellConst;
 WORD wCellKnown;
 WORD wHorLineRem;
 WORD wVerLineRem;
 WORD wTextLineRec;
 WORD wHorSpecksSize;
 WORD wVerSpecksSize;
 WORD wIgnSubSigns;
 WORD wIgnSubSignsPar;

 WORD wFieldAlphabet1;

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

 lpFieldBuffer  = fieldImg ->pixPtr;
 width          = fieldImg -> bi.biWidth;
 height         = fieldImg -> bi.biHeight;
 wDpiX          = fieldImg -> bi.biXPelsPerMeter;
 wDpiY          = fieldImg -> bi.biYPelsPerMeter;



 if(((int)wDpiX < 80)||((int)wDpiX > 420)) return(-112);
 if(((int)wDpiY < 80)||((int)wDpiY > 420)) return(-112);

 pwFieldDescr = (WORD*)pFieldDescr;
 wFieldNoFD = *pwFieldDescr;
 originX0       = *(pwFieldDescr+1);
 originY0       = *(pwFieldDescr+2);
 widthRec0      = *(pwFieldDescr+3);
 heightRec0     = *(pwFieldDescr+4);


 shift = 6 + (DWORD)(*(pwFieldDescr+5)/2);
 pwFieldContents = pwFieldDescr+shift;


 lpszFieldTitle  = (LPSTR)pwFieldContents;
 pwFieldContents = pwFieldContents+16;

 wFieldType      = *(pwFieldContents+0);
 wFieldAlphabet  = *(pwFieldContents+1);
 wCellConst      = *(pwFieldContents+2);
 wCellKnown      = *(pwFieldContents+3);
 wCellWidth      = *(pwFieldContents+4);
 wHorLineRem     = *(pwFieldContents+5);
 wVerLineRem     = *(pwFieldContents+6);
 wTextLineRec    = *(pwFieldContents+7);
 wLineHeight     = *(pwFieldContents+8);
 wHorSpecksSize  = *(pwFieldContents+9);
 wVerSpecksSize  = *(pwFieldContents+10);
 wIgnSubSigns    = *(pwFieldContents+11);
 wIgnSubSignsPar = *(pwFieldContents+12);
 wDictIndex      = *(pwFieldContents+13);
 wContext        = *(pwFieldContents+14);
 wConfThreshold  = *(pwFieldContents+15);
 for(i=0;i<8;i++) alphaMap[i] = *(pwFieldContents+23+(long)i);
/*
 {char bufLog[200];
    sprintf(bufLog,"wFieldType= %d, wFieldAlphabet= %d",wFieldType, wFieldAlphabet);
    saveLog(bufLog, "\n");
    sprintf(bufLog,"wCellConst= %d, wCellKnown= %d, wCellWidth= %d",wCellConst, wCellKnown, wCellWidth);
    saveLog(bufLog, "\n");
    sprintf(bufLog,"wHorLineRem= %d, wVerLineRem= %d, wTextLineRec= %d",wHorLineRem, wVerLineRem, wTextLineRec);
    saveLog(bufLog, "\n");
    sprintf(bufLog,"wLineHeight= %d, wIgnSubSigns= %d, wIgnSubSignsPar= %d",wLineHeight, wIgnSubSigns, wIgnSubSignsPar);
    saveLog(bufLog, "\n");
    sprintf(bufLog,"wDictIndex= %d, wContext= %d, wConfThreshold= %d",wDictIndex, wContext, wConfThreshold);
    saveLog(bufLog, "\n");

    sprintf(bufLog,"wHorSpecksSize= %d, wVerSpecksSize= %d, wCorrectionLevel= %d",wHorSpecksSize, wVerSpecksSize, wCorrectionLevel);
    saveLog(bufLog, "\n");

    for(i=0;i<8;i++) {sprintf(bufLog,"alphaMap[ %d ]= %d",i, alphaMap[i]);saveLog(bufLog, "\n");}
 }
*/
 if(wCellWidth < 900)
                {if(wDpiX0 != 0) wCellWidth = wCellWidth*wDpiX/wDpiX0;
                            else wCellWidth = 1;
                }
 if(wDpiY0 != 0) wLineHeight = wLineHeight*wDpiY/wDpiY0;
            else wLineHeight = 1;

// wFieldAlphabet1 = wFieldAlphabet%256;
 wFieldAlphabet1 = wFieldAlphabet;
 if(((wFieldAlphabet1 >= 32)&&(wFieldAlphabet1 <= 35))||
    ((wFieldAlphabet1 >= 52)&&(wFieldAlphabet1 <= 55))) nNormVersion = 0;
 else nNormVersion = 1;

 lpCand1Buffer = pResults -> lpCand1;
 lpCand2Buffer = pResults -> lpCand2;
 lpResultBuffer = pResults -> lpResult;
 lpRejectBuffer = pResults -> lpReject;


 if((int)wConfThreshold > 100) wConfThreshold = 100;
 if((int)wConfThreshold < 0) wConfThreshold = 0;

 if(wCorrectionLevel != 0) wCorrectionLevel = 1;

 pxOrigin = &xOriginR[0];
 pyOrigin = &yOriginR[0];
 pxCorner = &xCornerR[0];
 pyCorner = &yCornerR[0];

 lpImageL -> iLineWidth = (int)width;
 lpImageL -> iImgHeight = (int)height; lpImageL -> iXBegin    = 0;
 lpImageL -> iYBegin    = 0;
 lpImageL -> iWidth     = (int)width;
 lpImageL -> iHeight    = (int)height;
 lpImageL -> iDpiX      = (int)wDpiX;
 lpImageL -> iDpiY      = (int)wDpiY;

 lpSegment -> iVer       = (int)wCellConst;
 lpSegment -> iCellInfo  = (int)wCellKnown;
 lpSegment -> iCellWidth = (int)wCellWidth;

 lpLineRem -> iHorLineRem = (int)wHorLineRem;
 lpLineRem -> iVerLineRem = (int)wVerLineRem;

 lpTextLine -> iVer        = (int)wTextLineRec;
 lpTextLine -> iLineHeight = (int)wLineHeight;

 lpSpeck -> iHSize         = (int)wHorSpecksSize;
 lpSpeck -> iVSize         = (int)wVerSpecksSize;

 lpIgnSubSigns -> iIgnSubSigns    = (int)wIgnSubSigns;
 lpIgnSubSigns -> iIgnSubSignsPar = (int)wIgnSubSignsPar;

 lpContext -> iContextOn  = (int)wContext;
 lpContext -> iDictIndx   = (int)wDictIndex;
 lpContext -> iCorrectionLevel = (int)wCorrectionLevel;


 iFieldType = (int)wFieldType;

 iAlphaIndx = (int)wFieldAlphabet;

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

 lpCoordinates -> xOrigin = (int*)pxOrigin;
 lpCoordinates -> yOrigin = (int*)pyOrigin;
 lpCoordinates -> xCorner = (int*)pxCorner;
 lpCoordinates -> yCorner = (int*)pyCorner;

 strcpy(ticketDataPtr->currentImgName, fieldImg->szIMGName);

// printf("recField() \n ");
//	  fflush(stdout);

// LOGD("calling recField (lpStrings=%p)", lpStrings);
     {lpImageL -> lpPointer  = lpFieldBuffer;
     err=recField(ticketDataPtr, lpImageL, lpSegment, lpLineRem, lpTextLine, lpSpeck, lpIgnSubSigns,
                  lpContext, iFieldType,  iAlphaIndx, lpAlphaMap, (int)wConfThreshold, iWidthFrame,
                  iHeightFrame, lpWorkBuffer, lpExtractChar, lpRecChar, iRecMode,
                  iRecContinue, lpStrings, pwConf,  lpCoordinates);

      if(err < 0) return(err);
     }

 if(wFieldType != 9)
 {for(i=0; i<1000; i++)
  {if(*(lpResultBuffer+(long)i) == 0) break;
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
 else
 {xOrig = (*pxOrigin)+EXTRACTMARGIN; yOrig = (*pyOrigin)+EXTRACTMARGIN;
  xCorn = (*pxCorner)-EXTRACTMARGIN; yCorn = (*pyCorner)-EXTRACTMARGIN;
  pPattern -> p1X = xOrig;
  pPattern -> p1Y = yOrig;
  pPattern -> p2X = xCorn;
  pPattern -> p2Y = yOrig;
  pPattern -> p3X = xCorn;
  pPattern -> p3Y = yCorn;
  pPattern -> p4X = xOrig;
  pPattern -> p4Y = yCorn;
 }


// pharmco bars
  if(iAlphaIndx == 299)  {
      int i,n;
      int pos;
      int start;
      char c;

      n = strlen(lpResultBuffer);

      pos = 0;
      start = -1;
      for(i=0;i<n;i++) {

          c = lpResultBuffer[i];

          if((c == '\r')||(c == 32)) start = i;
          if(c == '\n') break;

          if((c == '\r')||(((i-start)%2) == 0)) {
	          lpCand1Buffer[pos]  = lpCand1Buffer[i];
	          lpCand2Buffer[pos]  = lpCand2Buffer[i];
	          lpResultBuffer[pos]  = lpResultBuffer[i];
	          lpRejectBuffer[pos] = lpRejectBuffer[i];
	          pwConf[pos]         = pwConf[i];

	          (pPattern[pos]).p1X = (pPattern[i]).p1X;
	          (pPattern[pos]).p1Y = (pPattern[i]).p1Y;
	          (pPattern[pos]).p2X = (pPattern[i]).p2X;
	          (pPattern[pos]).p2Y = (pPattern[i]).p2Y;
	          (pPattern[pos]).p3X = (pPattern[i]).p3X;
	          (pPattern[pos]).p3Y = (pPattern[i]).p3Y;
	          (pPattern[pos]).p4X = (pPattern[i]).p4X;
	          (pPattern[pos]).p4Y = (pPattern[i]).p4Y;
	           pos++;
	      }
      }

      lpCand1Buffer[pos]  = 0;
      lpCand2Buffer[pos]  = 0;
      lpResultBuffer[pos]  = 0;
      lpRejectBuffer[pos] = 0;
  }

 return(1);
}
// ------------------------------------------------------------------------------------------------
BYTE * lpRectBuffer;
WORD wLineWidth;
WORD wImgHeight;
WORD wDpiX;
WORD wDpiY;
PREPROCINFO * lpPreprocInfo;
PREPROCINFO preprocInfo;
WORD wAlphaIndx1;
WORD wAlphaIndx2;
int iSegmType = 0;
WORD wCharTypeForSegm = 6;
WORD* pwConf;
LPBYTE pSubsetMap = NULL;

IMAGEL * lpImageL;
SEGMENT * lpSegment;
int iHorLineRem;
int iVerLineRem;
TEXTLINE * lpTextLine;
SPECK * lpSpeck;
int iIgnSubSigns;
int iIgnSubSignsPar;
WORD wDictIndx;
double correctionLevel;
WORD wTypeField;
WORD wAlphaIndx;
LPBYTE lpAlphaMap;
WORD confThreshold;

LPBYTE lpWorkBuffer;
LPBYTE lpExtractCh;
WORD wWidthSegment;
WORD wHeightSegment;
LPBYTE normPict;
WORD size;
int nNormVersion;
WORD wRecMode;


BYTE subsetMapSup[16];
BYTE subsetMapMid[16];
BYTE subsetMapSub[16];
BYTE subsetMapReg[16];
int alphabetBuffer[256];
int delta;
int factor;
int charCounter;
char* lpCand1Buffer;
char* lpCand2Buffer;
char* lpResultBuffer;
char* lpRejectBuffer;
char* lpLastWord1;
char* lpLastWord2;
char* lpLastWord;



BYTE  const mask[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
				        /* { 128,  64,  32,  16,   8,   4,   2,   1}; */
BYTE const mask1[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
				         /* {   1,   2,   4,   8,  16,  32,  64, 128}; */

DWORD  const scale  = 0x10000; /* 65536 = 2**16 */
int  const scaleexp = 16;
int  const maxDist  = 0x4000;	 /* 16384 = 2**14 */


#define NCHAR      5000
#define NEWRAPDLL   1
#define CHARSIZE_ERR	-100
#define MASS_ERR		-101
#define CHAR_ERR		-102
#define PAR_ERR			-103
#define DATA_ERR		-104
#define NTW_ERR			-105
#define NNB_ERR			-106
#define CDI_ERR			-107

long score[NCHAR];

// tets--------------------------------
//LPBYTE testFieldBufferBMP;
//char imgName[200];
//-------------------------------------

int recField(LPTICKETDATA ticketDataPtr, IMAGEL* lpImageLA, SEGMENT * lpSegmentA, LINEREM * lpLineRem, TEXTLINE  * lpTextLineA,
 SPECK *lpSpeckA, IGNSUBSIGNS  * lpIgnSubSigns, CONTEXT  * lpContext, int iTypeField, int iAlphaIndx,
 LPBYTE lpAlphaMapA, int iConfThreshold, int iWidthFrame, int iHeightFrame, LPBYTE lpWorkBufferA,
 EXTRACTCHAR * lpExtractChar, RECCHAR * lpRecChar, int iRecMode, int iRecContinue,
 STRINGS * lpStrings, WORD * pwConf0, COORDINATES * lpCoordinates){


//	LOGD("recField 1\n");
 int end = 0;
 int retValue;
 long position;
 long wordPosition;
 char* lpCand1Buffer0;
 char* lpCand2Buffer0;
 char* lpResultBuffer0;
 char* lpRejectBuffer0;
 WORD * xOrigin0;
 WORD * yOrigin0;
 WORD * xCorner0;
 WORD * yCorner0;


// char buff[500];

 end = 0;

 lpRectBuffer = lpImageLA -> lpPointer;
  wLineWidth = (WORD)(lpImageLA -> iLineWidth);
  wImgHeight = (WORD)(lpImageLA -> iImgHeight);
  wDpiX = (WORD)(lpImageLA -> iDpiX);
  wDpiY = (WORD)(lpImageLA -> iDpiY);
  lpImageL = lpImageLA;
  lpSegment = lpSegmentA;
  iHorLineRem  = lpLineRem -> iHorLineRem;
  iVerLineRem  = lpLineRem -> iVerLineRem;
  lpTextLine = lpTextLineA;
  lpSpeck = lpSpeckA;
  iIgnSubSigns =    lpIgnSubSigns -> iIgnSubSigns;
  iIgnSubSignsPar = lpIgnSubSigns -> iIgnSubSignsPar;
  wDictIndx =  (WORD)(lpContext -> iDictIndx);
  correctionLevel = 0.0;

//	LOGD("recField 2\n");
//  printf("recField 1\n");
//  fflush(stdout);

  if((lpContext -> iCorrectionLevel) == 1) correctionLevel = 0.005;
  wTypeField = (WORD)iTypeField;
  wAlphaIndx = (WORD)iAlphaIndx;
  lpAlphaMap = lpAlphaMapA;
  confThreshold = (WORD)iConfThreshold;
  lpPreprocInfo = &preprocInfo;
  lpPreprocInfo -> iMinLH = (int)((iWidthFrame*75L)/100);
  lpPreprocInfo -> iMinLV = (int)((iHeightFrame*75L)/100);
  lpWorkBuffer = lpWorkBufferA;
  lpExtractCh = lpExtractChar ->lpExtractCh;
  wWidthSegment =  (WORD)(lpExtractChar -> iWidthSegment);
  wHeightSegment = (WORD)(lpExtractChar -> iHeightSegment);
  normPict =     lpRecChar -> lpNormPict;
  size =  (WORD)(lpRecChar -> iSize);
  nNormVersion = lpRecChar -> iNormVersion;
  wRecMode = (WORD)iRecMode;
  bRecContinueFd = iRecContinue;
  lpCand1Buffer0 = lpStrings -> lpCand1;
  lpCand2Buffer0 = lpStrings -> lpCand2;
  lpResultBuffer0 = lpStrings -> lpResult;
  lpRejectBuffer0 = lpStrings -> lpReject;
  xOrigin0 = (WORD *)lpCoordinates -> xOrigin;
  yOrigin0 = (WORD *)lpCoordinates -> yOrigin;
  xCorner0 = (WORD *)lpCoordinates -> xCorner;
  yCorner0 = (WORD *)lpCoordinates -> yCorner;


//  printf("recField 2\n");
//  fflush(stdout);

  {int flag = 0;
   int i;
   if(lpAlphaMap) {
   	for(i=0;i<16;i++) {
      	 if(lpAlphaMap[i]) flag = 1;
  	 }
   	if(!flag) lpAlphaMap = NULL;
  	}
   }

//	LOGD("recField 4 (%p, %p, %p, %p)\n", lpCand1Buffer0, lpCand2Buffer0, lpResultBuffer0, lpRejectBuffer0);

//	LOGD( "%s, %s, %s\n", lpCand2Buffer0, lpResultBuffer0, lpRejectBuffer0);

  wAlphaIndx1 = wAlphaIndx;

  if(wAlphaIndx1 == 32)      wCharTypeForSegm = 0;
  else if(wAlphaIndx1 == 33) wCharTypeForSegm = 2;
  else if(wAlphaIndx1 == 34) wCharTypeForSegm = 1;
  else if(wAlphaIndx1 == 35) wCharTypeForSegm = 3;

  else if(wAlphaIndx1 == 36) wCharTypeForSegm = 0;
  else if(wAlphaIndx1 == 37) wCharTypeForSegm = 2;
  else if(wAlphaIndx1 == 38) wCharTypeForSegm = 1;
  else if(wAlphaIndx1 == 39) wCharTypeForSegm = 3;

  else if(wAlphaIndx1 == 42) wCharTypeForSegm = 4;
  else if(wAlphaIndx1 == 43) wCharTypeForSegm = 6;
  else if(wAlphaIndx1 == 44) wCharTypeForSegm = 5;
  else if(wAlphaIndx1 == 45) wCharTypeForSegm = 7;
  else                       wCharTypeForSegm = 6;


  if(!bRecContinueFd)
    {*lpCand1Buffer0  = 0;
     *lpCand2Buffer0  = 0;
     *lpResultBuffer0 = 0;
     *lpRejectBuffer0 = 0;
    }


//	LOGD("recField 5\n");

  position = (long)(strlen(lpResultBuffer0));
  if(wRecMode == 1) wordPosition = (long)(wordpos(lpResultBuffer0));
  else wordPosition = position;

  lpCand1Buffer = lpCand1Buffer0 + position;
  lpCand2Buffer = lpCand2Buffer0 + position;
  lpResultBuffer = lpResultBuffer0 + position;
  lpRejectBuffer = lpRejectBuffer0 + position;
  pwConf = pwConf0 + position;
  xOriginFd = xOrigin0 + position;
  yOriginFd = yOrigin0 + position;
  xCornerFd = xCorner0 + position;
  yCornerFd = yCorner0 + position;
  lpLastWord1 = lpCand1Buffer0 + wordPosition;
  lpLastWord2 = lpCand2Buffer0 + wordPosition;
  lpLastWord = lpResultBuffer0 + wordPosition;

  if((wRecMode != 0)&&(wRecMode != 1)&&(wRecMode != 2)&&(wRecMode != 3))
    return(-1);

//	LOGD("recField 6\n");

 /* new !!!!!!!!!!!!!!!! */
  {
   int index;
     lpLibrary = NULL;
 	index = wAlphaIndx1;
     lpLibrary = loadLibrary_G(ticketDataPtr, index, &libSize);
 	if(lpLibrary == NULL) {
/*
 		  {char bufLog[200];
 		     sprintf(bufLog,"lpLibrary == NULL; index= %d, libSize = %d\n",index, libSize);
 		     printf(bufLog);
 		     saveLog(bufLog, "");
 		  }
*/
 		return -1;
 	}
  }

//	LOGD("recField 7\n");

 /* new !!!!!!!!!!!!!!!! */
  lineSaveIndex = 0;

  while(end == 0)
       {
	  retValue = recNext(ticketDataPtr);


        if(retValue < 0) return(retValue);
        if((retValue != 0)&&(retValue != 1)&&(retValue <= 1000)&&(retValue != 3))
            return(retValue);

        if((retValue == 0)||
           ((wRecMode == 1)&&(retValue == 1))||
           ((wRecMode == 2)&&((retValue > 1000)||(retValue == 3)))||
           ((wRecMode == 3)&&(retValue == 3)))
           end = 1;
       }

  return(1);
}

int recNext(LPTICKETDATA ticketDataPtr){
 int flag;
 int i;

 short result[12];
 short * pResult;

 BYTE char1, char2;
 DWORD len;
 int j;
// int j1;
 int err;
 int retSegm;
 WORD nSpace;
 int cellWidth;
 int * lpCellWidth;
// int iAngle = 0;
 int index;
// int skewHist;
 int conf1, conf2, conf12;
 int confMin;

// char bufLog[1000];


 RECCHARSTRUCT * pRecCharStruct;
 RECCHARSTRUCT recCharStruct;

 pRecCharStruct = &recCharStruct;

 lpCellWidth = &cellWidth;

 pResult = &result[0];

 if(!bRecContinueFd){
   {int nChar;
    LPBYTE pHeader = (LPBYTE)&alphabetBuffer[0];

    if((wAlphaIndx1 < 32)||(wAlphaIndx1 > 54)) index = wAlphaIndx1;
    else index = wAlphaIndx1 - 32;

//    printf("index= %d\n",index);
//    fflush(stdout);

    if(index < 10000) {
		 err = -77;
  	     if(lpLibrary)
           err = readHeader(lpLibrary, pHeader);
         if(err < 0)
           {*lpCand1Buffer++ = 32;
            *lpCand2Buffer++ = 32;
            *lpResultBuffer++ = 32;
            *lpRejectBuffer++ = '*';
            *pwConf++ = 0;
            *lpCand1Buffer = 0;
            *lpCand2Buffer = 0;
            *lpResultBuffer = 0;
            *lpRejectBuffer = 0;

            err = getCoordinates1();
            if(err < 0) return(-301);

            return(0);
           }
         nChar = (int)pHeader[2] + 256*(int)pHeader[3];
         alphabet = pHeader + 28;
         alphabet[nChar] = 0;
    }

      if((pHeader[1] == 'N')&&(index<10000)) {
           int a[128];
           int i;
           for (i=0; i<128; i++) a[i] = pHeader[i];

           if(((int)pHeader[20] >= 1)&&((int)pHeader[20] <= 6)) {
                size =        (int)pHeader[4] + 256*(int)pHeader[5];;
                nNormVersion = (int)pHeader[20];
                delta       = (int)pHeader[21];
                factor =      (int)pHeader[22];
           }
           else {
                delta = 1;
                factor = 10;
           }
      }
      else {
           delta = 1;
           factor = 10;
      }

  }

 err = defineSubsets((char*)alphabet, lpAlphaMap);

// iAngle = 0;
// skewHist = 0;

 mapVersionFD = 1;
 pRecCharStruct -> size          = size;
 pRecCharStruct -> normVersion   = nNormVersion;
 pRecCharStruct -> mapVersion    = mapVersionFD ;
 pRecCharStruct -> confThreshold = confThreshold;
 pRecCharStruct -> iAlphaIndx    = (int)wAlphaIndx;

 //printf(" lpImageL->iLineWidth= %d,  lpImageL->iWidth= %d\n", lpImageL->iLineWidth, lpImageL->iWidth);

 charCounter  = 0;
 err =  initFieldR
           (
        		   ticketDataPtr->currentImgName,
   		    ticketDataPtr->fieldAddress->name,
   		    ticketDataPtr->sizeWImg,
   		    ticketDataPtr->sizeHImg,
   		    &ticketDataPtr->fieldBMPPtr,
            lpImageL,
            (int)wTypeField,
            (int)wCharTypeForSegm,
            iSegmType,
//            iAngle,
//            skewHist,
            lpPreprocInfo,
            lpTextLine,
            iHorLineRem,
            iVerLineRem,
            lpSegment,
            lpSpeck,
            (STRUCTWB *)lpWorkBuffer,
            iIgnSubSignsPar,
            pRecCharStruct,
            lpAlphaMap
           );

         if(err < 0)
           {*lpCand1Buffer++ = 32;
            *lpCand2Buffer++ = 32;
            *lpResultBuffer++ = 32;
            *lpRejectBuffer++ = '*';
            *pwConf++ = 0;
            *lpCand1Buffer = 0;
            *lpCand2Buffer = 0;
            *lpResultBuffer = 0;
            *lpRejectBuffer = 0;

            err = getCoordinates1();
            if(err < 0) return(-301);

            return(0);
           }
         // remeber lines

         	     setStatusQuadY(ticketDataPtr, (STRUCTWB *)lpWorkBuffer);


/*
                 {char imgName[200];

                 printf("saveLinesTest; \n");
                 fflush(stdout);

          strcpy(imgName, ticketDataPtr->currentImgName);
          imgName[strlen(imgName)-4] = 0;
          strcat(imgName, "_");
          strcat(imgName, ticketDataPtr->fieldAddress->name);

//           saveLog(imgName, "\n");

        if(ticketDataPtr->fieldBMPPtr == NULL) {
            printf(" ticketDataPtr->fieldBMPPtr == NULL \n");
            fflush(stdout);

        }

        saveLinesTest(ticketDataPtr->fieldBMPPtr, (STRUCTWB *)lpWorkBuffer, imgName);
        //charSaveIndex = 0;
        printf("after saveLinesTest; \n");
        fflush(stdout);

                 }
*/

/*
         {char imgName[200];
//        if(strcmp(ticketDataPtr->fieldAddress->name, "DATEA") == 0)
           if(strcmp(ticketDataPtr->fieldAddress->name, "RET_NUMN") == 0)
//         if(strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS") == 0 || strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS1") == 0)
//             if(strcmp(ticketDataPtr->fieldAddress->name, "DATEA") == 0 || strcmp(ticketDataPtr->fieldAddress->name, "DATEN") == 0)
         {
  strcpy(imgName, ticketDataPtr->currentImgName);
  imgName[strlen(imgName)-4] = 0;
  strcat(imgName, "_");
  strcat(imgName, ticketDataPtr->fieldAddress->name);

//   saveLog(imgName, "\n");

//printf("  imgName= %s \n", imgName);
//printf("  ticketDataPtr->fieldAddress->name= %s \n", ticketDataPtr->fieldAddress->name);
//saveLinesTest(ticketDataPtr->fieldBMPPtr, (STRUCTWB *)lpWorkBuffer, imgName);
saveLinesTest2(ticketDataPtr->fieldBMPPtr, (STRUCTWB *)lpWorkBuffer, imgName);
//saveFramesTest(ticketDataPtr->fieldBMPPtr, (STRUCTWB *)lpWorkBuffer, imgName);
//charSaveIndex = 0;
         }
         }
*/
         	    /*
         	                     {char imgName[200];
         	                     if(strcmp(ticketDataPtr->fieldAddress->name, "DATEN1") == 0)
         	                     {
         	              strcpy(imgName, ticketDataPtr->currentImgName);
         	              imgName[strlen(imgName)-4] = 0;
         	              strcat(imgName, "_");
         	              strcat(imgName, ticketDataPtr->fieldAddress->name);


         	            //printf("  imgName= %s \n", imgName);
         	            //printf("  ticketDataPtr->fieldAddress->name= %s \n", ticketDataPtr->fieldAddress->name);
         	            saveLinesTest(ticketDataPtr->fieldBMPPtr, (STRUCTWB *)lpWorkBuffer, imgName);
         	            //charSaveIndex = 0;
         	                     }
         	                     }
         	    */


         bRecContinueFd = 1;

         if((wTypeField == 4)||(wTypeField == 5))
           {if(err == 1)
              {*lpCand1Buffer++ = 'Y';
               *lpCand2Buffer++ = 'Y';
               *lpResultBuffer++ = 'Y';
               *lpRejectBuffer++ = 32;
               *pwConf++ = 100;
              }
            if(err == 0)
              {*lpCand1Buffer++ = 'N';
               *lpCand2Buffer++ = 'N';
               *lpResultBuffer++ = 'N';
               *lpRejectBuffer++ = 32;
               *pwConf++ = 100;
              }
            if(err < 0)
              {*lpCand1Buffer++ = 64;
               *lpCand2Buffer++ = 64;
               *lpResultBuffer++ = 64;
               *lpRejectBuffer++ = '*';
               *pwConf++ = 0;
              }
            *lpCand1Buffer = 0;
            *lpCand2Buffer = 0;
            *lpResultBuffer = 0;
            *lpRejectBuffer = 0;

            return(0);
           }

   }
 retSegm = 0;


 {STRUCTWB* lpB = (STRUCTWB *)lpWorkBuffer;
    lpRectBuffer = lpB-> lpRBuffer;
 }
 retSegm =
		 nextFieldR(
				ticketDataPtr->fieldAddress->name,
				ticketDataPtr->sizeWImg,
                lpRectBuffer,
                (STRUCTWB *)lpWorkBuffer,
                lpCellWidth,
                lpExtractCh,
                wWidthSegment,
                wHeightSegment,
                wCharTypeForSegm
               );

       if(retSegm < 0) return(-151);


   if(charCounter >= NCHAR_G) retSegm = 0;

   if((retSegm != 0)&&(retSegm != 3)&&(retSegm <= 1000))
   {
	   err = getCoordinates();
       if(err < 0) return(-301);

    charCounter++;
    wAlphaIndx1 = wAlphaIndx;

    if((wCharTypeForSegm>= 4)) {
       if((40 <= retSegm)&&(retSegm < 100)) {
         if((40 <= retSegm)&&(retSegm < 59)) pSubsetMap = (LPBYTE)&subsetMapSup; //index = 27;
         if((60 <= retSegm)&&(retSegm < 79)) pSubsetMap = (LPBYTE)&subsetMapMid; //index = 28;
         if((strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS") == 0 ||strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS1") == 0) && charSaveIndex > 2 && charSaveIndex < 20){
        	 if((80 < retSegm)&&(retSegm < 99)) pSubsetMap = (LPBYTE)&subsetMapSub;} //index = 29;
       	 else {
             if((80 <= retSegm)&&(retSegm < 99)) pSubsetMap = (LPBYTE)&subsetMapSub;} //index = 29;
       }
       else  pSubsetMap = (LPBYTE)&subsetMapReg;
    }
    else pSubsetMap = lpAlphaMap;

    index = wAlphaIndx1;

    index = wAlphaIndx1;

    if((retSegm == 4)&&(wAlphaIndx1 == 32))
         {*lpCand1Buffer++ = 49;
          *lpCand2Buffer++ = 49;
          *lpResultBuffer++ = 49;
          *lpRejectBuffer++ = 32;
          *pwConf++ = 100;
         }
    else if((retSegm == 4)&&(wAlphaIndx1 == 33))
         {*lpCand1Buffer++ = 73;
          *lpCand2Buffer++ = 73;
          *lpResultBuffer++ = 73;
          *lpRejectBuffer++ = 32;
          *pwConf++ = 100;
         }
    else if((retSegm == 4)&&(wAlphaIndx1 == 34))
         {*lpCand1Buffer++ = 108;
          *lpCand2Buffer++ = 108;
          *lpResultBuffer++ = 108;
          *lpRejectBuffer++ = 32;
          *pwConf++ = 100;
         }
    else if((retSegm == 5)&&(iIgnSubSigns != 0))
         {*lpCand1Buffer++ = 32;
          *lpCand2Buffer++ = 32;
          *lpResultBuffer++ = 32;
          *lpRejectBuffer++ = 32;
          *pwConf++ = 100;
         }
    else if((wCharTypeForSegm < 4)&&(retSegm == 45))
         {*lpCand1Buffer++ = 45;
          *lpCand2Buffer++ = 45;
          *lpResultBuffer++ = 45;
          *lpRejectBuffer++ = 32;
          *pwConf++ = 100;
         }

    else
    {

/*
        {char imgName[200];
 strcpy(imgName, ticketDataPtr->currentImgName);
 imgName[strlen(imgName)-4] = 0;
 strcat(imgName, "_");
 strcat(imgName, ticketDataPtr->fieldAddress->name);
 // if(strcmp(ticketDataPtr->fieldAddress->name, "DATEA") == 0)
     if(strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS") == 0 && lineSaveIndex == 6)
//         if(strcmp(ticketDataPtr->fieldAddress->name, "NUMBERS") == 0)
//      if(strcmp(ticketDataPtr->fieldAddress->name, "LOGO3") == 0)
    	saveCharacter(lpExtractCh, wWidthSegment, wHeightSegment, lineSaveIndex, charSaveIndex, imgName);  // debug !!!!!!!!!!

//    	charSaveIndex++;
//    	lineSaveIndex++;
        }
*/


    	charSaveIndex++;
    	err = recPictM(lpExtractCh,wWidthSegment,wHeightSegment,pResult,index);

//    	printf(" lineSaveIndex= %d, charSaveIndex= %d, result1= %c, conf1 = %d, err= %d\n", lineSaveIndex, charSaveIndex-1, (char)pResult[3], pResult[2], err);
//    }
     flag = 0;
     if(pSubsetMap) {
     	for(i=0;i<16;i++) {
     	   if(pSubsetMap[i]) {
       	      flag = 1;
       	      break;
        	}
     	}
     }
     else flag = 1;
     if((err < 0)||(flag == 0)) {
               *lpCand1Buffer++ = 32;
               *lpCand2Buffer++ = 32;
               *lpResultBuffer++ = 32;
               *lpRejectBuffer++ = '*';
               *pwConf++ = 0;
     }
     else
     {if((iIgnSubSigns != 0)&&(err < iIgnSubSignsPar)) {
        if(wCharTypeForSegm > 3) {
           *lpCand1Buffer++ = 32;
            *lpCand2Buffer++ = 32;
            *lpResultBuffer++ = 32;
            *lpRejectBuffer++ = 32;
            *pwConf++ = 100;
        }
        else ;
      }
      else
      {char1 = (char)pResult[3];
       char2 = (char)pResult[5];

       if((wAlphaIndx1==9)&&(char1==32))
           {char1 = 45; char2 = 45;}

       *lpCand1Buffer++ = char1;
       *lpCand2Buffer++ = char2;
       *lpResultBuffer++ = char1;


       conf1 = pResult[2];
       conf2 = pResult[4];

       confMin = confThreshold;

       if((wAlphaIndx1==9)&&(char1==32))
           confMin = 0;

       if(conf1>=99) conf12 = 100;
       else conf12 = (int)(100L*(conf1 - conf2)/(100L - conf1));
       if(conf1 < conf12) *pwConf++ = (WORD)conf1;
                    else *pwConf++ = (WORD)conf12;
       if((WORD)(*(pwConf-1L)) >= (WORD)confMin)
                              *lpRejectBuffer++ = 32;
       else
                              *lpRejectBuffer++ = 42;

      }
     }
    }


    *lpCand1Buffer = 0;
    *lpCand2Buffer = 0;
    *lpResultBuffer = 0;
    *lpRejectBuffer = 0;

    return(1);
   }
   else
   {len = lpResultBuffer - lpLastWord;
    if(retSegm > 1000)
      {nSpace = retSegm - 1000;
       charCounter += nSpace;
       for(j=0; j<(int)nSpace; j++)
          {*lpCand1Buffer++ = 32;
           *lpCand2Buffer++ = 32;
           *lpResultBuffer++ = 32;
           *lpRejectBuffer++ = 32;
           *pwConf++ = 100;

           *xOriginFd = 0;
           *yOriginFd = 0;
           *xCornerFd = 0;
           *yCornerFd = 0;

           xOriginFd++;
           yOriginFd++;
           xCornerFd++;
           yCornerFd++;
          }
      }

    if(retSegm == 3)
      {charCounter++;
       *lpCand1Buffer++ = 13;
       *lpCand2Buffer++ = 13;
       *lpResultBuffer++ = 13;
       *lpRejectBuffer++ = 13;
       *pwConf++ = 100;

       *xOriginFd = 0;
       *yOriginFd = 0;
       *xCornerFd = 0;
       *yCornerFd = 0;

       xOriginFd++;
       yOriginFd++;
       xCornerFd++;
       yCornerFd++;
 	  lineSaveIndex++;
 	  charSaveIndex = 0;
      }

    lpLastWord1 = lpCand1Buffer;
    lpLastWord2 = lpCand2Buffer;
    lpLastWord  = lpResultBuffer;

    *lpCand1Buffer = 0;
    *lpCand2Buffer = 0;
    *lpResultBuffer = 0;
    *lpRejectBuffer = 0;

    return(retSegm);

   }
}
void saveCharacter(LPBYTE buffer, WORD width, WORD height, int lineIdx, int charIdx, char* imgName){
	    LPBITMAPFILEHEADER bf;
	    LPBITMAPINFOHEADER bi;
	    LPRGBQUAD          qd;
        int size;
	    DWORD              offBits;
	    LPBYTE             charBMP, ptrOut;
	    int                i, j, w_bt;



        offBits = sizeof (BITMAPFILEHEADER) +
                  sizeof (BITMAPINFOHEADER) +
                  sizeof (RGBQUAD) * 2;
        w_bt = ((width  + 31) /32) * 4;
        size = offBits + w_bt*height;
        charBMP = yl_calloc( "r1", size, sizeof(BYTE));

        bf = (LPBITMAPFILEHEADER) charBMP;
        bf->bfType[0] = 0x42;
        bf->bfType[1] = 0x4d;
	    set4Bytes(bf->bfSize, size);
	    bf->bfReserved1[0] = 0;
	    bf->bfReserved1[1] = 0;
        set4Bytes(bf->bfOffBits, offBits);

        bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	    set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
        set4Bytes(bi->biWidth, width);
	    set4Bytes(bi->biHeight, height);
	     bi->biPlanes[0] = 1;
	     bi->biPlanes[1] = 0;
	     bi->biBitCount[0]= 1;
	     bi->biBitCount[1]= 0;
	    set4Bytes(bi->biCompression, BI_RGB);
		set4Bytes(bi->biSizeImage, 0);
		set4Bytes(bi->biXPelsPerMeter, 300);
	    set4Bytes(bi->biYPelsPerMeter, 300);
		set4Bytes(bi->biClrUsed, 0);
		set4Bytes(bi->biClrImportant, 0);

//printf("------------bfsize= %d, fieldH.offbits= %d \n",  *(LPDWORD)(ptr+BFSIZEOFF), *(LPWORD)(ptr+BFOOFBITSOFF));
//printf("------------*bi->biPlanes= %d, fieldH.wDpiY= %d \n",  *bi->biPlanes, *bi->biBitCount);
        // generate pair of RGBQUADs (black and white)
        qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
        *(LPDWORD)qd       = 0x00000000;
        *(LPDWORD)(qd + 1) = 0x00ffffff;


        ptrOut = charBMP + offBits;
        for(i=0;i<w_bt;i++){
            for(j=0;j<height;j++){
            	*(ptrOut +j*w_bt+i) = *(buffer +j*w_bt+i);
            }
        }
        {char name[200];
         char strNum[10];
         strcpy(name, imgName);
          strcat(name, "_char_");
          if(lineIdx < 9){
        	  strNum[0] = '0' + lineIdx;
        	  strNum[1] = 0;
          }else{
        	  strNum[0] = '0' + lineIdx/10;
        	  strNum[1] = '0' + lineIdx%10;
        	  strNum[2] = 0;
          }
          strcat(name, strNum);
          strcat(name, "_");
          if(charIdx < 9){
        	  strNum[0] = '0' + charIdx;
        	  strNum[1] = 0;
          }else{
        	  strNum[0] = '0' + charIdx/10;
        	  strNum[1] = '0' + charIdx%10;
        	  strNum[2] = 0;
          }
          strcat(name, strNum);
          strcat(name, ".bmp");
          saveBMPbuffer (charBMP, name, "c:\\Users\\Wicek\\OCR\\output\\");
        }
        yl_free(charBMP);

}

int recPictM(LPBYTE buffer, WORD width, WORD height, short *pResult, int index){

// WORD max = 0;
// unsigned short t;
 int err;
 int retValue;
 int nChar;
 int flag;
 int nCand;
 int i;
 int conf;
 PATT patt;
 LPPATT lpPatt = (LPPATT)&patt;
 PATT normPatt;
 LPPATT lpNormPatt = (LPPATT)&normPatt;


 err = pictWidthHeightSizeDeltaFactor(width, height, size, 1, 10);
              if(err < 0) return(-303);

 lpPatt->lpBuffer = buffer;
 lpPatt->width = width;
 lpPatt->height = height;

 lpNormPatt->lpBuffer = normPict;
 lpNormPatt->width = size;
 lpNormPatt->height = size;

 retValue = normalize(lpPatt,lpNormPatt,delta,factor,mapVersionFD,nNormVersion);

 if(index==6) {
      char c1,c2;
      int th,conf;
      PATT patt;
      LPPATT lpPatt = (LPPATT)&patt;

      lpPatt->lpBuffer = (LPBYTE)lpRectBuffer;
      lpPatt->width = wLineWidth;
      lpPatt->height = wImgHeight;

      retValue = getMass(lpPatt);


      if(confThreshold <= 100)  th = (int)(confThreshold*confThreshold);
      else                      th = 10000;
      c1 = 'Y';
      c2 = 'N';
      pResult[0] = 1;
      pResult[1] = 2;
      if(retValue < 0) retValue = 0;
      if(retValue >= th) {
           if(th==0) conf = 100; else conf = (int)((2000L*(retValue - th))/(th*confThreshold));
           pResult[2] = conf;
           pResult[3] = (int)c1;
           pResult[4] = 0;
           pResult[5] = (int)c2;
      }
      else {
           if(th==0) conf = 0; else conf = (int)((2000L*(th - retValue))/(th*confThreshold));
           pResult[2] = conf;
           pResult[3] = (int)c2;
           pResult[4] = 0;
           pResult[5] = (int)c1;
      }
      return(retValue);
 }

 if(index<10000)
    {
   err = -77;
   if(lpLibrary)
     err = recCharacter(normPict, lpLibrary, libSize, pSubsetMap, pResult);
                  if(err < 0) return(-306);

     flag = pResult[0];
     nChar = pResult[1];
     nCand = (nChar < 5 ? nChar : 5);

     if(flag==0)
        {for(i=0; i<nCand; i++)
             {conf = (int)((2*256L - pResult[2+2*i])*100L/(2*256L));
              if(conf < 0) conf = 0;
              pResult[2+2*i] = conf;
             }
         for(i=1; i<nCand; i++)
            {if(pResult[2+2*i]==0) pResult[2+2*i+1] = pResult[2+2*i-1];
            }
        }
     else if(flag==1)
        {for(i=0; i<nCand; i++)
             {conf = (int)((1000L - pResult[2+2*i]/3)*100L/1000L);      //  04-30-97 /3 -is a change
              if(conf < 0) conf = 0;
              pResult[2+2*i] = conf;
             }
        }
     else return(-1);

     if(nCand==1)
        {
         pResult[2] = 100;
         pResult[4] = 0;
         pResult[5]= pResult[3];
        }
    }
 return(retValue);
}

int recCharacter (BYTE  * pNormBuffer, BYTE  * pDataBuffer, long dataBufferLength, BYTE  * pSubsetMap, short * pResult){
 int err;
 int i,j,k;
 long iLong;
 int end;
 int nCand = 5;
 int bestScore,bestIndex = 0;

 WORD nChar,width,height,offset,index;
 char alphabet[NCHAR];
 char subAlphabet[NCHAR];
 int subsetIndex[NCHAR];
 WORD nSubsetChar = 0;
 long nSample;

 WORD * pwDataBuffer = (WORD *)pDataBuffer;
 BYTE * pNNBBuffer;
 long dist;
 WORD sqSize = size*size;

 nChar	= pwDataBuffer[1];
 height	= pwDataBuffer[2];
 width	= pwDataBuffer[3];

 offset	= pwDataBuffer[5];

 if ((width != size) || (height != size) || (nCand != 5))
	return (DATA_ERR);

 for(k=0;k<nChar;k++)
	alphabet[k] = pDataBuffer[28+k];
 alphabet[nChar] = 0;

 if(pSubsetMap) {
	nSubsetChar = 0;
	end = 0;
	for (k=0;k<nChar;k++) {
		if (pSubsetMap[k/8] & mask1[k%8]) {
			subsetIndex[k] = 1;
			nSubsetChar++;
			end = 1;
		}
		else
			subsetIndex[k] = 0;
	}
 }
 if((!pSubsetMap) || (nSubsetChar == 0)) {
	nSubsetChar = nChar;
	for (k=0;k<nChar;k++)
		subsetIndex[k] = 1;
 }
 i = 0;
 for(k=0;k<nChar;k++)
	 if(subsetIndex[k] == 1)
		subAlphabet[i++] = alphabet[k];
 subAlphabet[nSubsetChar] = 0;

 pNNBBuffer = pDataBuffer + offset;

 /* initialize scores */
 for (k=0;k<nChar;k++)
	if(subsetIndex[k] == 1)
		score[k] = maxDist;
	else
		score[k] = maxDist + 1;

 nSample = (dataBufferLength - offset)/(sqSize + 1);

 for (iLong=0;iLong<nSample;iLong++) {
	index = pNNBBuffer[0];
	if ((index < 0) && (index >= nChar))
		return (NNB_ERR);
	 if (!strchr (subAlphabet,alphabet[index])) {
		pNNBBuffer += 1 + sqSize;
		continue;
	}

	pNNBBuffer += 1;

	err = distanceFD (pNormBuffer,pNNBBuffer, sqSize, &dist);
	if (err < 0)
		return (NNB_ERR);

	if (dist < score[index])
		score[index] = dist;

	pNNBBuffer += sqSize;
 }

 pResult[0] = 1;							/* NNB flag */
 pResult[1] = nSubsetChar;
 for(k=2;k<12;k++)
	pResult[k] = 0;

 /* select best candidates */
 if(nSubsetChar < nCand) nCand = nSubsetChar;
 for(i=0;i<nCand;i++) {
	bestScore = maxDist + 1;
	for(j=0;j<nChar;j++) {
		if(score[j] < bestScore) {
			bestScore = score[j];
			bestIndex = j;
		}
	}
	pResult[2+2*i] = bestScore;
	pResult[3+2*i] = alphabet[bestIndex];
	score[bestIndex] = maxDist + 1;
 }

 return ((char)pResult[3] + 256*(char)pResult[5]);
}
int wordpos(char* lpStr){
int pos;
 int i;
 LPSTR p;

 p = lpStr;
 pos = 0;
 for(i=0; *p++ != 0; i++)
   {if((*p == 32)||(*p == 13)) pos = i+1;
   }
 return(pos);
}
int getCoordinates1(void){
*xOriginFd++ = 0;
 *yOriginFd++ = 0;
 *xCornerFd++ = wLineWidth;
 *yCornerFd++ = wImgHeight;

 return(0);
}

int defineSubsets(char* a, LPBYTE mapA){
 char* sup     = "\"'*";
 char* supOnly = "";
 char* mid     = "*+,-.=";
 char* midOnly = "";
 char* sub     = ",.";
 char* subOnly = "";

 BYTE mapSup[128];
 BYTE mapMid[128];
 BYTE mapSub[128];
 BYTE mapReg[128];

 BYTE mapBuff[16];
 LPBYTE map = (LPBYTE)&mapBuff[0];

 BYTE masc[] = {1, 2, 4, 8, 16, 32, 64, 128};

 int n;
 int i;
 char c;
 int flag;

 if(a == NULL) return(0);

 flag = 0;
 if(mapA) {
     map = mapA;
     for(i=0;i<16;i++) {
        if(map[i]) {
             flag = 1;
             break;
         }
     }
 }
 if(flag == 0) for(i=0;i<16;i++) map[i] = 255;

 n = strlen(a);
 for(i=0;i<n;i++) {
     c = a[i];
     if(strchr(sup,c) != NULL)  mapSup[i] = 1; else mapSup[i] = 0;
     if(strchr(mid,c) != NULL)  mapMid[i] = 1; else mapMid[i] = 0;
     if(strchr(sub,c) != NULL)  mapSub[i] = 1; else mapSub[i] = 0;

     if((strchr(a,c) != NULL)&&
        (strchr(supOnly,c) == NULL)&&
        (strchr(midOnly,c) == NULL)&&
        (strchr(subOnly,c) == NULL)) mapReg[i] = 1; else mapReg[i] = 0;
 }

 for(i=n;i<128;i++) {
     mapSup[i] = 0;
     mapMid[i] = 0;
     mapSub[i] = 0;
     mapReg[i] = 0;
 }

 for(i=0;i<n;i++) {
     if((map[i/8]&masc[i%8]) == 0) {
         mapSup[i] = 0;
         mapMid[i] = 0;
         mapSub[i] = 0;
         mapReg[i] = 0;
     }
 }

 for(i=0;i<16;i++) {
     subsetMapSup[i] = 0;
     subsetMapMid[i] = 0;
     subsetMapSub[i] = 0;
     subsetMapReg[i] = 0;
 }

 for(i=0;i<128;i++) {
      if(mapSup[i]==1) subsetMapSup[i/8] |= masc[i%8];
      if(mapMid[i]==1) subsetMapMid[i/8] |= masc[i%8];
      if(mapSub[i]==1) subsetMapSub[i/8] |= masc[i%8];
      if(mapReg[i]==1) subsetMapReg[i/8] |= masc[i%8];
 }
 return(0);
}

int getCoordinates(void){

 int top;
 int topL;
 int topR;
 int bottom;
 int bottomL;
 int bottomR;
 int left;
 int right;
 int lengthL = 0;
 int lengthR = 0;

 int i;
 STRUCTWB *lpstructWB;

 lpstructWB = (STRUCTWB *)lpWorkBuffer;
 lengthL = lpstructWB->lengthTraceL;
 lengthR = lpstructWB->lengthTraceC;


 topL = lpstructWB->traceL[0];
 topR = lpstructWB->traceC[0];

 bottomL = topL + lengthL;
 bottomR = topR + lengthR;



 left = 10000;
 for(i=1; i<lengthL ; i++)  left = min(left, lpstructWB->traceL[i]);
 right = 0;
 for(i=1; i<lengthR ; i++)  right = max(right, lpstructWB->traceC[i]);

 top = min(topL, topR);
 bottom = max(bottomL, bottomR);

//printf("------------------  xOriginFd= %d, yOriginFd= %d, xCornerFd= %d, yCornerFd= %d \n", (int)xOriginFd, (int)yOriginFd, (int)xCornerFd, (int)yCornerFd);
 *xOriginFd++ = (WORD)left;
 *yOriginFd++ = (WORD)top;
 *xCornerFd++ = (WORD) right;
 *yCornerFd++ = (WORD) bottom;

 return(0);
}

int pictWidthHeightSizeDeltaFactor (WORD widthPar, WORD heightPar, WORD sizePar, WORD deltaPar, WORD factorPar){
 widthFD  = widthPar;
 heightFD = heightPar;
 sizeFD   = sizePar;
 deltaFD  = deltaPar;
 factorFD = factorPar;

 return (0);
}
int normalize(LPPATT lpPatt, LPPATT lpNormPatt, int delta, int factor, int mapVersion, int normVersion){
  int err;
 int retValue = 0;

 {int i;
  int n = 1;

 if((normVersion>=90)&&(normVersion<=99)){normVersion = normVersion - 90; n = 2000;}

 for(i=0;i<n;i++) {

 if(normVersion < 3) {
      err = setMapVers((WORD)mapVersion);
              if(err < 0) return(-302);
      err = pictWidthHeightSizeDeltaFactor((WORD)lpPatt->width, (WORD)lpPatt->height,
                                      (WORD)lpNormPatt->width, (WORD)1, (WORD)10);
              if(err < 0) return(-303);
      err = setNormVers((WORD)normVersion);
              if(err < 0) return(-304);
      retValue =  pictFromNormal(lpPatt->lpBuffer, lpNormPatt->lpBuffer);
              if(retValue < 0) return(-305);

 }

 if((normVersion>=3)&&(normVersion<=6)) {
    retValue =  normalizeM(lpPatt,lpNormPatt,delta,factor,mapVersion,normVersion);
           if(retValue<0) return(-399);
 }

 }}

 return(retValue);
}
int getMass(LPPATT lpPatt){
 BYTE masc[] = {128,64,32,16,8,4,2,1};
 LPBYTE lpBuffer = lpPatt->lpBuffer;
 int width = lpPatt->width;
 int height = lpPatt->height;

 long s = 0;

 int m;

 int i;
 int j;
 int k;
 int byteWidth;
 int lastByte;
 LPBYTE p;
 int i8;

 BYTE byte;

 if((width > CHARSIZE_G)||(height > CHARSIZE_G)) return(-1);

 if(mapVersionFD == 0) {
    byteWidth = ((width+15)/16)*2;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + j*byteWidth;
 }
 else  {
    byteWidth = ((width+31)/32)*4;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + (height - j - 1)*byteWidth;
 }

 lastByte = width/8;

 s = 0L;
 for(j=0;j<height;j++) {
     p = lpLine[j];
     for(i=0;i<lastByte;i++) {
         i8 = 8*i;
         byte = p[i];
         if(byte != 255) {
             for(k=0;k<8;k++) {
                 if(!(byte&masc[k])) {
                    s++;
                 }
             }
         }
     }
     i8 = 8*lastByte;
     byte = p[lastByte];
     if(byte != 255) {
        for(k=0;k<width%8;k++) {
            if(!(byte&masc[k])) {
               s++;
            }
        }
     }
 }

 if(s > 0x8000 - 1) s = 0x8000 - 1;
 m = (int)s;

 return(m);
}

int distanceFD(BYTE * pNormBuffer, BYTE* pNNBSample, WORD sqSize, long* pDist){
 int i;
 long sum = 0;

 for (i=1;i<sqSize;i++)
	sum += abs ((*pNormBuffer++) - (*pNNBSample++));

 if (sum < maxDist)
	*pDist = sum;
 else
	*pDist = maxDist;

 return (0);
}

int setMapVers ( WORD mapVersionPar)
{
 mapVersionFD = mapVersionPar;

 return (0);
}
int setNormVers ( WORD normVersionPar)
{
	normVersionFD = normVersionPar;

 return (0);
}
int pictFromNormal (BYTE * pCharBuffer, BYTE * pNormBuffer){
 int err;
 int i,j,k;

 long mass;
 double mx,my,mxx,myy,mxy,sigx,sigy;
 double mnx,mny,signx,signy;
 double kSkew,bx,by,bxk,ax,ay;

 int x,y,off,yOff;

 int sizeNormBuffer;
 int byteWidth,lastByte,i8;
 BYTE * p;
 BYTE byte;

 double factor1;
 double doubleTmp;
 int intTmp;

 MOMENTS moments;
 LPMOMENTS pMoments = &moments;

 double delta2 = (double)delta/2;

 if (((mapVersionFD != 0) && (mapVersionFD != 1)) ||
	(size != 15) || (delta != 1) || (factor != 10) ||
	((normVersionFD != 0) && (normVersionFD != 1)))
	return (PAR_ERR);

 sizeNormBuffer = size*size;

 mnx   = (double)(size - 1)/2;
 mny   = mnx;
 signx = (double)size/6;
 signy = signx;

 err = getMoments (
	pCharBuffer,
	widthFD,
	heightFD,
	pMoments,
	mapVersionFD
 );
 if (err <0)
	return(err);

 mass   = pMoments->m;
 if (mass <= 0)
	return (MASS_ERR);

 mx  = pMoments->mx;
 my  = pMoments->my;
 mxx = pMoments->mxx;
 myy = pMoments->myy;
 mxy = pMoments->mxy;

 if((mxx <= 0) || (myy <= 0))
	return (CHAR_ERR);

 sigy = sqrt(myy);
 by = signy/sigy;

 if (normVersionFD) {					/* redefine signx */
	sigx = sqrt(mxx);
	signx = sigx * signy/sigy;
 }
 kSkew = -mxy/myy;

 if (normVersionFD == 2)
	bx = by;
 else {
	doubleTmp = mxx - kSkew*kSkew*myy;
	if(doubleTmp <= 0)
		return (CHAR_ERR);
	bx = signx/sqrt(doubleTmp);
 }

 bxk = bx*kSkew;

 ax = mnx - bx*(mx + kSkew*(my+1)) - delta2 +  0.5;
 ay = mny - by*(my+1)			   - delta2 +  0.5;

 for(k=0;k<sizeNormBuffer;k++)
	pNormBuffer[k] = 0;

 if (mapVersionFD == 0) {
	byteWidth = ((widthFD+15)/16)*2;
	for (j=0;j<heightFD;j++)
		pCharLine[j] = pCharBuffer + j*byteWidth;
 }
 else {
	byteWidth = ((widthFD+31)/32)*4;
	for (j=0;j<heightFD;j++)
		pCharLine[j] = pCharBuffer + (heightFD - j - 1)*byteWidth;
 }

 lastByte = widthFD/8;

 for (j=0;j<heightFD;j++) {
	ax += bxk;
	ay += by;
	y = (int)floor(ay);
	if ((y<0) || (y>size-2))
		continue;
	yOff = size*y;
	p = pCharLine[j];
	for (i=0;i<lastByte;i++) {
		i8 = 8*i;
		byte = p[i];
		if (byte != 255) {
			for (k=0;k<8;k++) {
				if (!(byte&mask[k])) {
					x = (int)(floor)(ax + bx*(i8+k));
					if ((x<0) || (x>size-2))
						continue;
					off = x + yOff;

					pNormBuffer[off]++;
					pNormBuffer[off+size]++;
					pNormBuffer[++off]++;
					pNormBuffer[off+size]++;
				}
			}
		}
	}
	i8 = 8*lastByte;
	byte = p[lastByte];
	if (byte != 255) {
		for (k=0;k<widthFD%8;k++) {
			if (!(byte&mask[k])) {
				x = (int)(floor)(ax + bx*(i8+k));
				if ((x<0) || (x>size-2))
					continue;
				off = x + yOff;

				pNormBuffer[off]++;
				pNormBuffer[off+size]++;
				pNormBuffer[++off]++;
				pNormBuffer[off+size]++;
			}
        }
	 }
 }

 factor1 = 256.0*factor/(4*mass*delta*delta);

 for(k=0;k<sizeNormBuffer;k++) {
	intTmp = (int)(floor)(factor1*pNormBuffer[k] + 0.5);
	if(intTmp<256)
		pNormBuffer[k] = intTmp;
	else
		pNormBuffer[k] = 255;
 }

 return (mass);
}

int getMoments ( BYTE* pCharBuffer, int	width, int height, LPMOMENTS pMoments, WORD	 mapVersion){
 long m   = 0;
 long mx  = 0;
 long my  = 0;
 long mxx = 0;
 long mxy = 0;
 long myy = 0;

 double cx;
 double cy;
 double cxx;
 double cyy;
 double cxy;

 long s;
 int i,j,k,i8;
 int byteWidth,lastByte;
 BYTE * p;
 BYTE byte;

 if ((width > CHARSIZE)||(height > CHARSIZE))
	return (CHARSIZE_ERR);

 if (mapVersion == 0) {
	byteWidth = ((width+15)/16)*2;
	for (j=0;j<height;j++)
		pCharLine[j] = pCharBuffer + j*byteWidth;
 }
 else {
	byteWidth = ((width+31)/32)*4;
	for (j=0;j<height;j++)
		pCharLine[j] = pCharBuffer + (height - j - 1)*byteWidth;
 }

 lastByte = width/8;

 for (i=0;i<width;i++) {
	hx[i] = 0;
	h [i]  = 0;
 }

 for (j=0;j<height;j++) {
     s = 0;
     p = pCharLine[j];
     for (i=0;i<lastByte;i++) {
         i8 = 8*i;
         byte = p[i];
         if (byte != 255) {
             for (k=0;k<8;k++) {
                 if (!(byte&mask[k])) {
                    s ++;
                    hx[i8+k] ++;
                    h [i8+k] += j;
                 }
             }
         }
     }
     i8 = 8*lastByte;
     byte = p[lastByte];
     if (byte != 255) {
        for (k=0;k<width%8;k++) {
            if (!(byte&mask[k])) {
               s ++;
               hx[i8+k] ++;
               h [i8+k] += j;
            }
        }
     }
     hy[j] = s;
     m += s;
 }

 for (i=0;i<width;i++)  {
	mx  += hx[i]*i;
	mxx += hx[i]*i*i;
	mxy += h[i]*i;
 }

 for (j=0;j<height;j++) {
	my  += hy[j]*j;
	myy += hy[j]*j*j;
 }

 if (m <= 0)
	return (MASS_ERR);

 cx  = (double)mx/m;
 cy  = (double)my/m;
 cxx = (double)mxx/m - cx*cx;
 cyy = (double)myy/m - cy*cy;
 cxy = (double)mxy/m - cx*cy;

 pMoments->m   = m;
 pMoments->mx  = cx;
 pMoments->my  = cy;
 pMoments->mxx = cxx;
 pMoments->myy = cyy;
 pMoments->mxy = cxy;

 return (0);
}
int normalizeM(LPPATT lpPatt, LPPATT lpNormPatt, int delta, int factor, int mapVersion, int normVersion){
 BYTE masc[] = {128,64,32,16,8,4,2,1};

 int i;
 int a;
 int n=1;
 int n0;

 double sum;

 int err;

 double m;
 double cx,cy,mx,my,mxx,mxy,myy;

 double cnx,cny;
 double r,rnx,rny,rplusrnx,rplusrny,rminusrnx,rminusrny,twominrrnx,twominrrny;
 double coeff;
 double coeffx,coeffy;
 double dx,dy;
 float * px;
 float * py;
 double x,y,xn,yn;
 double alpha;

 MOMENTSFD moments;
 LPMOMENTSFD lpMoments = (LPMOMENTSFD)&moments;

 LPBYTE lpBuffer;
 int width;
 int height;

 LPBYTE lpBufferN;
 int widthN;
 int heightN;
 int sizeN;

 int byteWidth;
 int lastByte;
 LPBYTE p;
 BYTE byte;

 int jx,jy;
 int ixmin, ixmax, ixnum, iymin, iymax, iynum;

 int ix,iy;
 int j;
 int k;
 int kmin,kmax;
 int b,b8,bmin,bmax;
 LPBYTE lpMemNorm;

 float * normBuff;

 int * jxmin;
 int * jxmax;
 int * jxnum;
 int * jymin;
 int * jymax;
 int * jynum;
 float * sx;
 float * sy;

 int jxrange,jyrange;

 int shift;

 if(!pMemNorm)
	 pMemNorm = yl_calloc( "r2", 0xFFFF, sizeof(BYTE));
 if (!pMemNorm) return (-399);
 lpMemNorm = pMemNorm;

 lpBuffer = lpPatt->lpBuffer;
 width    = lpPatt->width;
 height   = lpPatt->height;

 lpBufferN = lpNormPatt->lpBuffer;
 widthN    = lpNormPatt->width;
 heightN   = lpNormPatt->height;
 sizeN = widthN*heightN;

 if((widthN>NORMSIZE_G)&&(heightN>NORMSIZE_G)) return(-398);

 if(mapVersion == 0) {
    byteWidth = ((width+15)/16)*2;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + j*byteWidth;
 }
 else  {
    byteWidth = ((width+31)/32)*4;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + (height - j - 1)*byteWidth;
 }

 lastByte = width/8;

 n = 1;

 for(i=0;i<n;i++) {

 err = getMomentsFD(lpPatt, lpMoments, mapVersion);

 }
         if(err<0)   return(-397);
         if(err==0)  return(0);

 m  = lpMoments->m;
 cx = lpMoments->cx + 0.5;
 cy = lpMoments->cy + 0.5;
 mxx = lpMoments->mxx;
 mxy = lpMoments->mxy;
 myy = lpMoments->myy;

 if(myy < 0.1) alpha = 0.0; else alpha = mxy/myy;
 if(normVersion%2 != 0) alpha = 0.0;

 mx = sqrt(mxx - alpha*mxy);
 my = sqrt(myy);

 cnx   =  (double)widthN/2.0 + 0.5;
 cny   =  (double)heightN/2.0 + 0.5;

 if((mx == 0)||(my==0)) return(0);
 else {
      coeffx = ((double)widthN/mx)/6;
      coeffy = (double)heightN/my/6;
 }
 if(coeffx>2*coeffy) coeffx = 2*coeffy;
 if(coeffy>2*coeffx) coeffy = 2*coeffx;
 coeff = min(coeffx,coeffy);
 if((normVersion == 3)||(normVersion == 4)) {
      coeffx = coeff;
      coeffy = coeff;
 }

 r = 0.5*(double)delta/10.0;
 rnx = 0.5*coeffx;
 rny = 0.5*coeffy;

 rplusrnx = r + rnx;
 rminusrnx = fabs(r - rnx);
 twominrrnx = 2.0*min(r,rnx);
 rplusrny = r + rny;
 rminusrny = fabs(r - rny);
 twominrrny = 2.0*min(r,rny);

 jxrange = min(widthN,(int)ceil(2*rplusrnx));
 jyrange = min(heightN,(int)ceil(2*rplusrny));

 ixmin = max(0, (int)ceil(cx - 6*sqrt(mxx) - 1));
 ixmax = min(width-1, (int)ceil(cx + 6*sqrt(mxx) - 1));
 ixnum = ixmax - ixmin + 1;
 if(ixnum < 0 ) ixnum = 0;

 iymin = max(0, (int)ceil(cy - 6*my - 1));
 iymax = min(height-1, (int)ceil(cy + 6*my - 1));
 iynum = iymax - iymin + 1;
 if(iynum < 0 ) iynum = 0;

 if(4*(long)NORMSIZE_G*(long)NORMSIZE_G + 4*(long)(ixnum+iynum)*(long)jxrange +
    6*(long)(ixnum+iynum) > (long)0xFFFF)
      return(-396);

 normBuff = (float *)lpMemNorm;
 sx =                normBuff + (long)NORMSIZE_G*NORMSIZE_G;
 sy =                sx       + ixnum*jxrange;
 jxmin = (int *)(sy       + iynum*jyrange);
 jxmax = jxmin + ixnum;
 jxnum = jxmax + ixnum;
 jymin = jxnum + ixnum;
 jymax = jymin + iynum;
 jynum = jymax + iynum;

 for(k=0;k<sizeN;k++) normBuff[k] = (float)0.0;
 for(k=0;k<sizeN;k++) lpBufferN[k] = 0;

 for(ix=0;ix<ixnum;ix++) {
     x = (double)(ixmin+ix+1);
     xn = cnx + coeffx*(x - cx);
     jxmin[ix] = max(1, (int)ceil(xn - rplusrnx));
     jxmax[ix] = min(widthN,(int)floor(xn + rplusrnx));
     jxnum[ix] = jxmax[ix] - jxmin[ix] + 1;
     if(jxnum[ix] < 0)
          jxnum[ix] = 0;
     sxiFD[ix] = sx + jxrange*(long)ix;
     for(jx=0;jx<jxnum[ix];jx++) {
         dx = fabs(xn - (double)(jxmin[ix]+jx));
         if(dx > rminusrnx) sx[jxrange*(long)ix + jx] = (float)(rplusrnx - dx);
         else               sx[jxrange*(long)ix + jx] = (float)twominrrnx;
     }
 }

 for(iy=0;iy<iynum;iy++) {
     y = (double)(iymin+iy+1);
     yn = cny + coeffy*(y - cy);
     jymin[iy] = max(1, (int)ceil(yn - rplusrny));
     jymax[iy] = min(heightN,(int)floor(yn + rplusrny));
     jynum[iy] = jymax[iy] - jymin[iy] + 1;
     if(jynum[iy] < 0)
          jynum[iy] = 0;
     syiFD[iy] = sy + jyrange*(long)iy;
     for(jy=0;jy<jynum[iy];jy++) {
         dy = fabs(yn - (double)(jymin[iy]+jy));
         if(dy > rminusrny) sy[jyrange*(long)iy + jy] = (float)(rplusrny - dy);
         else               sy[jyrange*(long)iy + jy] = (float)twominrrny;
     }
 }

 bmin = ixmin/8;
 bmax = ixmax/8;
 kmin = ixmin - 8*bmin;
 kmax = ixmax - 8*bmax;


 for(iy=0;iy<iynum;iy++) {
     p = lpLine[iymin+iy];
     shift = (int)floor(coeffx*alpha*(iymin+iy-cy) + 0.5);
     b = bmin;
     b8 = 8*b;
     byte = p[b];
     if(byte != 255) {
         for(k=kmin;k<8;k++) {
             if(!(byte&masc[k])) {
                 ix = b8+k-ixmin;
                 py = syiFD[iy];
                 n0 = jxmin[ix]-1 - shift + widthN*(jymin[iy]-1);
                 for(jy=0;jy<jynum[iy];jy++) {
                      px = sxiFD[ix];
                      n = n0;
                      for(jx=0;jx<jxnum[ix];jx++) {
                           normBuff[n++] += px[jx]*py[jy];
                      }
                      n0 += widthN;
                 }
             }
         }
     }
     for(b=bmin+1;b<bmax;b++) {
         b8 = 8*b;
         byte = p[b];
         if(byte != 255) {
             for(k=0;k<8;k++) {
                 if(!(byte&masc[k])) {
                     ix = b8+k-ixmin;
                     py = syiFD[iy];
                     n0 = jxmin[ix]-1 - shift + widthN*(jymin[iy]-1);
                     for(jy=0;jy<jynum[iy];jy++) {
                          px = sxiFD[ix];
                          n= n0;
                          for(jx=0;jx<jxnum[ix];jx++) {
                               normBuff[n++] += px[jx]*py[jy];
                          }
                          n0 += widthN;
                     }
                 }
             }
         }
     }
     b = bmax;
     b8 = 8*b;
     byte = p[b];
     if(byte != 255) {
         for(k=0;k<=kmax;k++) {
             if(!(byte&masc[k])) {
                 ix = b8+k-ixmin;
                 py = syiFD[iy];
                 n0 = jxmin[ix]-1 - shift + widthN*(jymin[iy]-1);
                 for(jy=0;jy<jynum[iy];jy++) {
                      px = sxiFD[ix];
                      n = n0;
                      for(jx=0;jx<jxnum[ix];jx++) {
                           normBuff[n++] += px[jx]*py[jy];
                      }
                      n0 += widthN;
                 }
             }
         }
     }
 }

 sum = 0.0;
 for(k=0;k<sizeN;k++) sum += (double)normBuff[k];
 if(sum<0.9) coeff = 0.0;
 else coeff = (double)factor*255*widthN*heightN/(sum*15*15);

 for(k=0;k<sizeN;k++) {
      a = (int)floor(coeff*(double)normBuff[k] + 0.5);
      if(a > 255) a = 255;
      lpBufferN[k] = (BYTE)a;
 }

 return(0);
}


int getMomentsFD(LPPATT lpPatt,  LPMOMENTSFD lpMoments, int mapVersion){

 BYTE masc[] = {128,64,32,16,8,4,2,1};

 LPBYTE lpBuffer = lpPatt->lpBuffer;
 int width = lpPatt->width;
 int height = lpPatt->height;


 double m = 0.0;
 double cx = 0.0;
 double cy = 0.0;
 double mxx = 0.0;
 double mxy = 0.0;
 double myy = 0.0;

 float s;
 int i;
 int j;
 int k;
 int byteWidth;
 int lastByte;
 LPBYTE p;
 int i8;

 BYTE byte;

 float * hx;
 float * hy;
 float * h;
 float * lpMemMoments;


 if(!pMemMomentsFD)
	 pMemMomentsFD = yl_calloc( "r3", 12*CHARSIZE_G, sizeof(BYTE));
 if (!pMemMomentsFD) return (-395);
 lpMemMoments = (float  *)pMemMomentsFD;

 hx = lpMemMoments;
 hy = hx + CHARSIZE_G;
 h  = hy + CHARSIZE_G;

 if((width > CHARSIZE_G)||(height > CHARSIZE_G)) return(-4);

 if(mapVersion == 0) {
    byteWidth = ((width+15)/16)*2;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + j*byteWidth;
 }
 else  {
    byteWidth = ((width+31)/32)*4;
    for(j=0;j<height;j++) lpLine[j] = lpBuffer + (height - j - 1)*byteWidth;
 }

 lastByte = width/8;

 for(i=0;i<width;i++) {
      hx[i+1] = 0;
      h[i+1] = 0;
 }

 for(j=0;j<height;j++) {
     s = (float)0.0;
     p = lpLine[j];
     for(i=0;i<lastByte;i++) {
         i8 = 8*i;
         byte = p[i];
         if(byte != 255) {
             for(k=0;k<8;k++) {
                 if(!(byte&masc[k])) {
                    s += (float)1.0;
                    hx[i8+k+1] += (float)1.0;
                    h[i8+k+1] += (float)j + (float)0.5;
                 }
             }
         }
     }
     i8 = 8*lastByte;
     byte = p[lastByte];
     if(byte != 255) {
        for(k=0;k<width%8;k++) {
            if(!(byte&masc[k])) {
               s += (float)1.0;
               hx[i8+k+1] += (float)1.0;
               h[i8+k+1] += (float)j + (float)0.5;
            }
        }
     }
     hy[j+1] = s;
     m += (double)s;
 }

 if(m<0.9) return(0);

 for(i=1;i<=width;i++) cx += (double)hx[i]*((double)i - 0.5);
 cx = cx/m;

 for(i=1;i<=width;i++) {
     mxx += (double)hx[i]*((double)i - 0.5 - cx)*((double)i - 0.5 - cx);
 }
 mxx = mxx/m;

 for(j=1;j<=height;j++) cy += (double)hy[j]*((double)j - 0.5);
 cy = cy/m;

 for(j=1;j<=height;j++) {
     myy += (double)hy[j]*((double)j - 0.5 - cy)*((double)j - 0.5 - cy);
 }
 myy = myy/m;

 for(i=1;i<=width;i++) {
     mxy += (double)h[i]*((double)i - 0.5 - cx);
 }
 mxy = mxy/m;



 lpMoments->m = m;
 lpMoments->cx = cx;
 lpMoments->cy = cy;
 lpMoments->mxx = mxx;
 lpMoments->mxy = mxy;
 lpMoments->myy = myy;

 return(1);
}

void setStatusQuadY(LPTICKETDATA ticketDataPtr, STRUCTWB *lpWorkBuffer){
    int i, indFr = 0;
    int yShift1 = ticketDataPtr->lineQuad[NUM_LINESTATUS-1].p1Y-8;
    int yShift2 = ticketDataPtr->lineQuad[NUM_LINESTATUS-1].p2Y-8;

//    { char buffLog[500];
//sprintf(buffLog,"setStatusQuadY------------ linesForFonts, lpWorkBuffer->infFrame[0]).nLines= %d \n", (lpWorkBuffer->infFrame[0]).nLines);
//saveLog("", buffLog);
//    }

	  for(i=0;i<(lpWorkBuffer->infFrame[indFr]).nLines;i++) {
		  if(i < NUM_LINESTATUS) {
		    ticketDataPtr->lineQuadStatus[i] = 1;
		    ticketDataPtr->lineQuad[i].p1Y = (lpWorkBuffer->line[indFr]).y1[i]+yShift1-3;
		    ticketDataPtr->lineQuad[i].p2Y = (lpWorkBuffer->line[indFr]).y1[i]+yShift2-3;
		    ticketDataPtr->lineQuad[i].p3Y = (lpWorkBuffer->line[indFr]).y2[i]+yShift1+2;
		    ticketDataPtr->lineQuad[i].p4Y = (lpWorkBuffer->line[indFr]).y2[i]+yShift2+2;
		  }
	  }

}
