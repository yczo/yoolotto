#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#include "../YLogoClassifier/ylogo_classifier.h"
#include  "../headers/Functions.h"
#include "../headers/util.h"
#include "../headers/ocr.h"
#include "../headers/postprocessing.h"
#include "../headers/memmgr.h"
#include "../headers/logging.h"
#include "../headers/BarCode.h"


char strGResultI25[64];
int cnfGResultI25[64];
QUADRANGLE qGResultI25[64];
char strGResultUPC[64];
int  cnfGResultUPC[64];
QUADRANGLE qGResultUPC[64];


//void addInfoToTestFile(LPIMGDATA img, int testBaraCodeInt);
void saveCurResultsBC(LPTICKETRESULTS ticketResults, int iter);
int testResultsBC();
void copyBackGResultsBC(LPTICKETRESULTS ticketResults);
void initGBC();

void getTypeStrForPRT(char *typeStr, int n);
void pre_ProcessColorImageSCR(LPIMGDATA img, LPIMGDATA imgOut, int byteSteps, int US_state, int iter);
//void setNewImageName(char *imgName, char *imgNInp, int loop);
//void repairImageHeader(LPIMGDATA img);
int createBMPfromJPEG(LPBYTE ptrJPG, int length,  LPIMGDATA img);
void setlotteryTypes(void);
void PrintOutputResults_new(LPTICKETRESULTS ticketResults);
void saveTxtFile (char *buff, char *szIMGName, char *szOutPath);
void setNewImageNameEXT(char *imgName, char *imgNInp, char *ext);
//int getLine(char* results, int n, int* iStart, int* iStop);
void getTicketJson( LPTICKETRESULTS p, char *s, int resultslength );
void getTicketJsonEmpty( char *s, int resultslength);
void calculateOutputConfidence(LPTICKETRESULTS ticketResults, int *ticketConfidenceV);
int calculateReturnValue(int *ticketConfidenceV);

//static COMPRESSED_IMG compressedImgBufferStruct;
//static int imgQuality;

void copyLocalTime(struct tm *mytmOut, struct tm *mytmInp){
	mytmOut->tm_hour = mytmInp->tm_hour; mytmOut->tm_min =  mytmInp->tm_min; mytmOut->tm_sec = mytmInp->tm_sec;
}
void getTimeDifference(struct tm *mytm1, struct tm *mytm2, int *hOut, int *mOut, int *sOut){
	int hour1, min1, sec1;
	int hour2, min2, sec2;
	int h, m, s;
	int dt, t1, t2, r;

	   hour1 = mytm1->tm_hour; min1 =  mytm1->tm_min; sec1 = mytm1->tm_sec;
	   hour2 = mytm2->tm_hour; min2 =  mytm2->tm_min; sec2 = mytm2->tm_sec;
//	   printf("hour1= %d, min1= %d, sec1= %d\n",hour1,min1,sec1);
//	   printf("hour2= %d, min2= %d, sec2= %d\n",hour2,min2,sec2);
	   t1 = hour1*3600+min1*60+sec1;
	   t2 = hour2*3600+min2*60+sec2;
	   dt = t2-t1;
//	   printf("t1= %d, t2= %d, dt= %d\n",t1,t2,dt);
	   h = dt/3600;r = dt-h*3600;
	   m = r/60;s = r-m*60;
//	   printf("h= %d, m= %d, s= %d\n",h,m,s);
	   *hOut = h;*mOut = m;*sOut=s;
}

void initTicketResults(LPTICKETRESULTS lpTicketResults){
	int i;
	lpTicketResults->status = 0;
	lpTicketResults->type = 0;
	lpTicketResults->typeCnf = 0;
	lpTicketResults->yShift = 0;

	for(i=0;i<NUMFIELDS_RESULTS;i++){
		lpTicketResults->fieldsResults[i].name[0] = 0;
	}

}

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
	const char SLASH_CHAR = '/';
#else
	const char SLASH_CHAR = '\\';
#endif
/*
static void addSlash(char *path){
	if(path[strlen(path)-1] != SLASH_CHAR ){
		int size = strlen(path);
		path[size] = SLASH_CHAR;
		path[size+1] = '\0';
	}
}
*/


void set_I_O_Path(LPIMGDATA img, char *mainPath, char *test){
	sprintf( img->szIMGPath, "%s%c", mainPath, SLASH_CHAR );
	strcpy( img->szOutPath, img->szIMGPath);
	strcpy(img->szWorkPath, img->szIMGPath);

	strcpy(img->szIMGName, test);
}

int saveSlices(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut, char *dirName){
	   int i, line;
	   LPOUTIMGS lpImgs;
	   char outBuffer[500];
//	   char num[10];

	    for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(lpTicketResults->fieldsResults[i].name[0] != 0){

		    	if(lpTicketResults->fieldsResults[i].ptrOutImages != NULL){
		    		lpImgs = lpTicketResults->fieldsResults[i].ptrOutImages;
		    		line = 0;
		    		while(lpImgs != NULL){
						sprintf( outBuffer, "%s_%s_slice%d.bmp", imgOut->szIMGName, lpTicketResults->fieldsResults[i].name, line );

		    			saveBMPbuffer (lpImgs->lpImgBuffer, outBuffer, dirName);
		    			lpImgs = lpImgs->ptrNext;
		    			line++;
		    		}
		    	}
	    	}
	    }

	    return NUMFIELDS_RESULTS;
}


void setGlobalDataPrototype(LPTICKETDATA ticketDataPtr, char *mainPath){
	sprintf(ticketDataPtr->prototypePath, "%s%c", mainPath, SLASH_CHAR);
	strcpy(ticketDataPtr->libraryPath, ticketDataPtr->prototypePath);

	ticketDataPtr->wCorrectionLevel = 20;
	ticketDataPtr->tgp = 20;                // distortion
	ticketDataPtr->wPar = 30;               // calibration
	ticketDataPtr->phi.phiMin = -50;
	ticketDataPtr->phi.phiMax =  50;
	ticketDataPtr->phi.stepPhi =  5;
}

static int getImageType(char *buffType){ // 0 - RGBA, 1 - RGB565; -1 - unknown
	int imgType = -1;
	if(stricmp(buffType, "RGBA") == 0)
		imgType = 0;
	if(stricmp(buffType, "RGB565") == 0)
		imgType = 1;
	if(stricmp(buffType, "BMP24") == 0)
		imgType = 24;
	return imgType;
}

void reloadMonoBufferBytes(LPBYTE ptrOut, LPBYTE ptrInp, int wWidth, int wHeight){
int i;
  for(i=0;i<wHeight*wWidth;i++){
	   ptrOut[i] = ptrInp[i];
  }
}
void reloadMonoBufferBytes_RGB(LPBYTE ptrOut, LPBYTE ptrInp, int wWidth, int wHeight, int nBytes){
	int i;
	  for(i=0;i<wHeight*wWidth*nBytes;i++){
		   ptrOut[i] = ptrInp[i];
	  }
}
static int setRGBAImgeHeader(unsigned char *ptrInp, int imgWidth, int imgHeight, LPIMGDATA img, int imgType){

	int bufforSize;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER);
	LPBYTE ptrH;
	int dpi = 500;
	int nBytes = 0;

 if(imgType == 24) {
//		int bufforSize;
		int biWidthPad = ((imgWidth*24+31)/32)*4;
//		int offBits = sizeof (BITMAPFILEHEADER) +
//				sizeof (BITMAPINFOHEADER);

		bufforSize = biWidthPad*imgHeight+offBits;

    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
    if(ptrH == NULL) {
//    	sprintf(error,"BMP_OCR_Call();   yl_calloc ( %d ) failed: %s\n",bufforSize, strerror(errno));
    	return -1;
    }

	img->ptr = ptrH;
	img->pixPtr = ptrH + offBits;
	reloadMonoBufferBytes(img->pixPtr, ptrInp, ((imgWidth*24+31)/32)*4, imgHeight);
 } else{

		switch(imgType){
		case 1: // 565  biWidthPad*imgHeight+offBits;
			nBytes = 2;	 break;
		default: // RGBA
			nBytes = 4;	 break;
		}
		bufforSize = imgWidth*imgHeight*nBytes+offBits;

	    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
	    if(ptrH == NULL) {
	    	return -1;
	    }

		img->ptr = ptrH;
		img->pixPtr = ptrH + offBits;


		reloadMonoBufferBytes_RGB(img->pixPtr, ptrInp, imgWidth, imgHeight, nBytes);



//	bufforSize = offBits;
//	img->ptr = NULL;
//	img->pixPtr = ptrInp;
 }
	img->bf.bfType = 0x4d42;
	img->bf.bfSize = bufforSize;
	img->bf.bfReserved1 =  0;
	img->bf.bfReserved2 =  0;
	img->bf.bfOffBits =   offBits;


	img->bi.biSize = offBits - 14;
	img->width = imgWidth;
	img->bi.biWidth = img->width;
	img->height = imgHeight;
	img->bi.biHeight = img->height;
	img->bi.biPlanes = 1;
	switch(imgType){
	case 1:  img->bi.biBitCount = 565; break;
	case 24: img->bi.biBitCount = 24; break;
	default: img->bi.biBitCount = 32; break;
	}

	img->bi.biCompression = 0;
	img->bi.biSizeImage = bufforSize - offBits;
	if(imgWidth < imgHeight)
	  dpi = (int)((double)imgWidth/3.1875);
	else
	  dpi = (int)((double)imgHeight/3.1875);
//	img->bi.biXPelsPerMeter = 500;
//	img->bi.biYPelsPerMeter = 500;
	img->bi.biXPelsPerMeter = dpi;
	img->bi.biYPelsPerMeter = dpi;
	img->bi.biClrUsed = 0;
	img->bi.biClrImportant = 0;
	img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	return 0;

}

static int setRGBAImgeHeaderNoCopy(unsigned char *ptrInp, int imgWidth, int imgHeight, LPIMGDATA img, int imgType){

	int bufforSize;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER);
	LPBYTE ptrH;
	int dpi = 500;
	int nBytes = 0;

 if(imgType == 24) {
//		int bufforSize;
		int biWidthPad = ((imgWidth*24+31)/32)*4;
//		int offBits = sizeof (BITMAPFILEHEADER) +
//				sizeof (BITMAPINFOHEADER);

		bufforSize = biWidthPad*imgHeight+offBits;

    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
    if(ptrH == NULL) {
//    	sprintf(error,"BMP_OCR_Call();   yl_calloc ( %d ) failed: %s\n",bufforSize, strerror(errno));
    	return -1;
    }


	img->ptr = ptrH;
	img->pixPtr = ptrH + offBits;
	reloadMonoBufferBytes(img->pixPtr, ptrInp, ((imgWidth*24+31)/32)*4, imgHeight);
 } else{
		switch(imgType){
		case 1: // 565  biWidthPad*imgHeight+offBits;
			nBytes = 2;	 break;
		default: // RGBA
			nBytes = 4;	 break;
		}
		bufforSize = imgWidth*imgHeight*nBytes+offBits;
/*
	    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
	    if(ptrH == NULL) {
	    	return -1;
	    }

		img->ptr = ptrH;
		img->pixPtr = ptrH + offBits;
		reloadMonoBufferBytes_RGB(img->pixPtr, ptrInp, imgWidth, imgHeight, nBytes);
*/
//	bufforSize = offBits;
	img->ptr = NULL;
	img->pixPtr = ptrInp;
 }
	img->bf.bfType = 0x4d42;
	img->bf.bfSize = bufforSize;
	img->bf.bfReserved1 =  0;
	img->bf.bfReserved2 =  0;
	img->bf.bfOffBits =   offBits;


	img->bi.biSize = offBits - 14;
	img->width = imgWidth;
	img->bi.biWidth = img->width;
	img->height = imgHeight;
	img->bi.biHeight = img->height;
	img->bi.biPlanes = 1;
	switch(imgType){
	case 1:  img->bi.biBitCount = 565; break;
	case 24: img->bi.biBitCount = 24; break;
	default: img->bi.biBitCount = 32; break;
	}

	img->bi.biCompression = 0;
	img->bi.biSizeImage = bufforSize - offBits;
	if(imgWidth < imgHeight)
	  dpi = (int)((double)imgWidth/3.1875);
	else
	  dpi = (int)((double)imgHeight/3.1875);
//	img->bi.biXPelsPerMeter = 500;
//	img->bi.biYPelsPerMeter = 500;
	img->bi.biXPelsPerMeter = dpi;
	img->bi.biYPelsPerMeter = dpi;
	img->bi.biClrUsed = 0;
	img->bi.biClrImportant = 0;
	img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	return 0;

}

static void adjustQuad(LPQUADRANGLE q, int width, int height){
	int leftE = 1, rightE = width-1;
	int topE = 1, bottomE = height-1;

	if(q->p1X < leftE) q->p1X = leftE;if(q->p2X < leftE) q->p2X = leftE;
	if(q->p3X < leftE) q->p3X = leftE;if(q->p4X < leftE) q->p4X = leftE;

	if(q->p1X > rightE) q->p1X = rightE;if(q->p2X > rightE) q->p2X = rightE;
	if(q->p3X > rightE) q->p3X = rightE;if(q->p4X > rightE) q->p4X = rightE;

	if(q->p1Y < topE) q->p1Y = topE;if(q->p2Y < topE) q->p2Y = topE;
	if(q->p3Y < topE) q->p3Y = topE;if(q->p4Y < topE) q->p4Y = topE;

	if(q->p1Y > bottomE) q->p1Y = bottomE;if(q->p2Y > bottomE) q->p2Y = bottomE;
	if(q->p3Y > bottomE) q->p3Y = bottomE;if(q->p4Y > bottomE) q->p4Y = bottomE;
}
static int isItDigit(char ch){
	if(ch < '0') return 0;
	if(ch > '9') return 0;
	return 1;
}
static int getRightSideOfTheSlice(LPFIELDSRESULTS lpFResults, LPQUADRANGLE q, int line){
	 int iStart, iStop, kIdx, x2;
	 int rightConstant = 63;

	 char *resultsStr = lpFResults->result;
	 if(getLine(resultsStr, line, &iStart, &iStop) < 0) return q->p2X;
	 kIdx = iStop;
	 if(ticketTypeInt != 1 && ticketTypeInt != 4) {

	    while(kIdx > iStart && isItDigit(lpFResults->result[kIdx]) == 0) kIdx--;
        x2 = lpFResults->flQuad[kIdx].p2X+rightConstant;
	 }
	 else x2 = q->p2X;

     if(x2 > q->p2X) x2 = q->p2X;
     return x2;
}

static void addNextLineImage(LPFIELDSRESULTS lpFResults, int iStatus, LPIMGDATA imgOut){
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
	LPQUADRANGLE q = &lpFResults->lineQuad[iStatus];
	int angleFlag = 0;
	int startX =  0, stopX = 0;
    int constantShift = 59;
    int kIdx = 0;

	//	    char buffLog[500];

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

//	 printf("-------lpFResults->name= %s, iStatus= %d\n", lpFResults->name, iStatus);
//	 printf("-------Width= %d, Height= %d\n", imgOut->bi.biWidth, imgOut->bi.biHeight);
//	 printf("-------q->p1X= %d, q->p1Y= %d\n", q->p1X, q->p1Y);
//	 printf("-------q->p2X= %d, q->p4Y= %d\n", q->p2X, q->p4Y);
//     fflush(stdout);

	//    saveLog("", buffLog);
	adjustQuad(q, imgOut->bi.biWidth, imgOut->bi.biHeight);
	startX = q->p1X;
	stopX  = q->p2X;
	kIdx = 1;
	while(kIdx < strlen(lpFResults->result) && lpFResults->result[kIdx] <= 32) kIdx++;
	 if(strcmp(lpFResults->name, "NUMBERS") == 0 && kIdx < 5)
		 startX =  lpFResults->flQuad[kIdx].p1X-constantShift;
	 if(startX<10) startX=10;
	 if(strcmp(lpFResults->name, "NUMBERS") == 0)
	    stopX = getRightSideOfTheSlice(lpFResults, q, iStatus);


// if(strcmp(lpFResults->name, "NUMBERS") == 0) {
//   printf("------------ addNextLineImage;  iStatus= %d, q->p2X= %d, stopX= %d\n",  iStatus, q->p2X, stopX);
//  }
// if(strcmp(lpFResults->name, "NUMBERS") == 0) {
//	 char buffLog[500];
///      sprintf(buffLog,"-------startX= %d, lpFResults->flQuad[kIdx].p1X= %d, kIdx= %d\n", startX, lpFResults->flQuad[kIdx].p1X, kIdx);
//          saveLog("", buffLog);
// }

//   printf("------------ startX= %d, stopX= %d, q->p1Y= %d, q->p2Y= %d, q->p4Y= %d \n",  startX, stopX, q->p1Y, q->p2Y, q->p4Y );
//   fflush(stdout);

	err =
			extractField(imgOut->pixPtr, imgOut->bi.biWidth, imgOut->bi.biHeight,
					startX, q->p1Y,
					stopX, q->p2Y,
					startX, q->p4Y,
					&hpFieldH,
					&width,
					&height,
					EXTRACTMARGIN,
					lpFResults->name,
					angleFlag,
					&lpFResults->angle
			);
/*
	err =
			extractField(imgOut->pixPtr, imgOut->bi.biWidth, imgOut->bi.biHeight,
					q->p1X, q->p1Y,
					q->p2X, q->p2Y,
					q->p4X, q->p4Y,
					&hpFieldH,
					&width,
					&height,
					EXTRACTMARGIN,
					lpFResults->name,
					angleFlag,
					&lpFResults->angle
			);
*/
	if(err < 0) return;

	bf = (LPBITMAPFILEHEADER) hpFieldH.ptr;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, offBits + (((width + 31) /32) * 4) * height);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	// fill in BITMAPINFOHEADER

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
	set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
	set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	// generate pair of RGBQUADs (black and white)
	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;
	{
		LPOUTIMGS lpImgs;
		LPOUTIMGS lpImgsTmp;
		lpImgs = yl_calloc("OCR_C", sizeof(OUTIMGS), sizeof(BYTE));
		if(lpImgs == NULL) return;
		lpImgs->width = width;
		lpImgs->height = height;
		lpImgs->ptrNext = NULL;
		lpImgs->lpImgBuffer = hpFieldH.ptr;

		//	    sprintf(buffLog,"-------new line image; lpImgs= %u, lpImgs->lpImgBuffer= %u\n", (unsigned)lpImgs, (unsigned)lpImgs->lpImgBuffer);
		//  	    saveLog("", buffLog);

		if(lpFResults->ptrOutImages == NULL) {

			//  sprintf(buffLog,"-------adding first image; lpImgs= %u, lpImgs->lpImgBuffer= %u\n", (unsigned)lpImgs, (unsigned)lpImgs->lpImgBuffer);
			//  saveLog("", buffLog);

			lpFResults->ptrOutImages = lpImgs;
		}
		else {

			lpImgsTmp =  lpFResults->ptrOutImages;
			// 	  sprintf(buffLog,"-------start adding next line image; lpImgsTmp= %u\n", (unsigned)lpImgsTmp);
			//      saveLog("", buffLog);

			while(lpImgsTmp->ptrNext != NULL) lpImgsTmp = lpImgsTmp->ptrNext;
			lpImgsTmp->ptrNext = lpImgs;

			//     	  sprintf(buffLog,"-------adding next line image; lpImgsTmp= %u\n", (unsigned)lpImgsTmp);
			//         saveLog("", buffLog);


		}
	}
}

static void addImagesToTheField(LPFIELDSRESULTS lpFResults, LPIMGDATA imgOut){
	int i;

	for(i=0;i<NUM_LINESTATUS;i++){
		if(lpFResults->lineQuadStatus[i] == 1){
			// 				yShift = lpFResults->lineQuad[NUM_LINESTATUS-1].p1Y;
			if(strlen(lpFResults->result) > 0) {
//				printf("----------lpFResults->result= %s\n", lpFResults->result);
//				fflush(stdout);
			   addNextLineImage(lpFResults, i, imgOut);
			}
			else
				lpFResults->lineQuadStatus[i] = 0;
		}
	}

}

void addImagesToTheOutput(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut){
	int i;

	for(i=0;i<NUMFIELDS_RESULTS;i++){
		lpTicketResults->fieldsResults[i].ptrOutImages = NULL;
		if(lpTicketResults->fieldsResults[i].name[0] != 0){

//			lpTicketResults->fieldsResults[i].ptrOutImages = NULL;
			if(strcmp(lpTicketResults->fieldsResults[i].name, "NUMBERS") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "DATETOP") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "DATE") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "PLAYER") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "DRAW") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "EXTRA") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "UPC") == 0 ||
			   strcmp(lpTicketResults->fieldsResults[i].name, "I25") == 0
			   ){
//printf("----------lpTicketResults->fieldsResults[i].name= %s\n",lpTicketResults->fieldsResults[i].name);
//fflush(stdout);
			addImagesToTheField(&lpTicketResults->fieldsResults[i], imgOut);
/*
//test-------------------------------------
			if(strcmp(lpTicketResults->fieldsResults[i].name, "DATE") == 0){
				LPOUTIMGS lpImgs;
				lpImgs = lpTicketResults->fieldsResults[i].ptrOutImages;
				if(lpImgs != NULL)
					saveBMPbuffer (lpImgs->lpImgBuffer, "DATE.bmp", imgOut->szOutPath);

				}
*/
			}
//-----------------------------------------

		}
	}
}


int Scratch_Off_Call(unsigned char *ptr, int imgWidth, int imgHeight, char *buffType, int byteSteps, int outimages,
		LPIMGDATA imgOut, char *mainPath,	LPTICKETRESULTS lpTicketResults, char *error, char *imgName, int US_state){
	IMGDATA img;
	LPTICKETDATA ticketData;
	HLPBYTE workAreaPtr;
//	LPBYTE iDStructPtr = NULL;
	int returnValue = 0;
	int imgType = 0;
	int iter = 0;

#define WORKAREA_MAXSIZE 4000000



	scratchOff_flg = 1;
	ticketTypeInt = -1;
	while(1){
//		initTicketResults(lpTicketResults);

		img.ptr = NULL;
		imgOut->ptr = NULL;
		ticketData = (LPTICKETDATA)yl_calloc( "OCR_D", sizeof(TICKETDATA), sizeof(BYTE));
		if(ticketData == NULL) {
			sprintf(error, "yl_calloc(ticketData) error= %s\n", strerror(errno));
			returnValue = -100;
			break;
		}

		setGlobalDataPrototype(ticketData, mainPath);
		ticketData->fieldBMPPtr = NULL;
	    ticketData->ptrTicketStruct = NULL;

		set_I_O_Path(&img, mainPath, imgName);
		set_I_O_Path(imgOut, mainPath, imgName);

		//		selecting image type
		//		saveLog("", "selecting image type\n");
		imgType = getImageType(buffType); // 0 - RGBA, 1 - RGB565, 24 - BMP24
		if(imgType < 0) {
//			yl_free(ticketData);
//			ticketData = NULL;
			sprintf(error, "not supported image type;\n");
			break;
		}
		img.error[0] = 0;
		returnValue = setRGBAImgeHeader(ptr, imgWidth, imgHeight, &img, imgType);
		if(returnValue < 0){
//			yl_free(ticketData);
//			ticketData = NULL;

			sprintf(error, "setRGBAImgeHeader(); error= %s\n", img.error);
			break;
		}

		workAreaPtr.ptr = yl_calloc("workAreaPtr", WORKAREA_MAXSIZE, sizeof(BYTE));
		if(workAreaPtr.ptr == NULL) {
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
//			if(imgOut->ptr != NULL) {
//				yl_free(imgOut->ptr);
//				imgOut->ptr = NULL;
//			}
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "workAreaPtr; yl_calloc failed: %s\n", strerror(errno));
			returnValue = -130;
			break;

		}
		workAreaPtr.size = WORKAREA_MAXSIZE;
		ticketData->workArea = workAreaPtr;
		initLib (ticketData);

		//		saveLog("", "preProcess color");
		initGBC();
     for(iter = 0;iter<3;iter++) {
 		initTicketResults(lpTicketResults);

		img.error[0] = 0;
//		pre_ProcessColorImageBMP(&img, imgOut, byteSteps, ticketData, US_state, scratchOff_flg);
		pre_ProcessColorImageSCR(&img, imgOut, byteSteps, US_state, iter);

		if(img.error[0] != 0){
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "pre_ProcessColorImageBMP; error= %s\n", img.error);
			returnValue = -110;
			break;
		}


//		 saveBMPbuffer (imgOut->ptr, imgOut->szIMGName, imgOut->szOutPath);
//		returnValue = -110;
//		break;


		ticketTypeInt = 77;
		searchBarcodes_SRC(imgOut, lpTicketResults, US_state, ticketData);
		saveCurResultsBC(lpTicketResults, iter);

//printf("iter= %d, strGResultI25= %s, strGResultUPC= %s\n", iter, strGResultI25, strGResultUPC);
//fflush(stdout);
		if(testResultsBC()>= 0) break;
		break;
     }


     if(iter>0){
// printf("iter= %d, strGResultI25= %s, strGResultUPC= %s\n", iter, strGResultI25, strGResultUPC);
// fflush(stdout);
    	 copyBackGResultsBC(lpTicketResults);
     }

		if(outimages > 0)
			addImagesToTheOutput(lpTicketResults, imgOut);

//	     printf(" 2 --- iter= %d, strGResultI25= %s, strGResultUPC= %s\n", iter, strGResultI25, strGResultUPC);
//	     fflush(stdout);

		if(img.ptr != NULL){
			yl_free(img.ptr);
			img.ptr = NULL;
		}

		releaseLibMemory();
//		ReleasePrototypes (ticketData);

//		if(ticketData->ptrTicketStruct)
//			yl_free(ticketData->ptrTicketStruct);
//		if(iDStructPtr)
//			yl_free(iDStructPtr);

//		ClearZones (ticketData);

		if(ticketData->fieldBMPPtr!= NULL)
			yl_free(ticketData->fieldBMPPtr);
		ticketData->fieldBMPPtr= NULL; // ????????????????


		if(ticketData->ptrOut != NULL)
			yl_free(ticketData->ptrOut);
		ticketData->ptrOut = NULL;


		if(ticketData->workArea.ptr != NULL && ticketData->workArea.size> 0) {
			yl_free(ticketData->workArea.ptr);
			ticketData->workArea.ptr = NULL;
			ticketData->workArea.size = 0;
		}

//		if(ticketData->prototypePtr != NULL) {
//			yl_free(ticketData->prototypePtr);
//			ticketData->prototypePtr = NULL;
//		}

		yl_free(ticketData);
		ticketData = NULL;

		if(outimages > 0) {
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
		}
		break;
	}

	return returnValue;

}

void saveResultsTodisk(char* buff, char * fileName){
	FILE *f;
	int len;
	errno = 0;
	size_t fSize = 0;

//LOGI( "------------------------------     fileName= %s", fileName);
printf("------------------------------     fileName= %s\n", fileName);
	if (!fileName) return;

	f= fopen(fileName, "w");
	if(f == NULL) {
//		LOGI( "saveResultsTodisk(fopen) (\"%s\") failed: %s\n",fileName, strerror(errno));
		printf("saveResultsTodisk(fopen) (\"%s\") failed: %s\n",fileName, strerror(errno));
		return;
	}
	//	 saveLog("after open", "\n");
	len = strlen(buff);
//	LOGI( "------------------------------     len= %d", len);
	printf("------------------------------     len= %d\n", len);

	fSize = fwrite((char *)buff, len, 1, f);
//	LOGI( "------------------------------     fSize= %d", fSize);
	printf("------------------------------     fSize= %d\n", fSize);
	if(ferror(f) || fSize*len != len){
		if(ferror(f)) {
//			LOGI( "saveResultsTodisk(fwrite) (\"%s\") failed: %s\n",fileName, strerror(errno));
			printf("saveResultsTodisk(fwrite) (\"%s\") failed: %s\n",fileName, strerror(errno));
		}
		else {
//			LOGI( "saveResultsTodisk(fwrite) (\"%s\") failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
			printf("saveResultsTodisk(fwrite) (\"%s\") failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		}
		return;
	}
	close_file(f);
}

void saveTicketTypeToBuff(int type, char * buff){
	char typeStr[100];
	getTypeStr(typeStr, ticketTypeInt);
	sprintf(buff,"ticket type=  %s\n",typeStr);
}
void saveFieldToBuff(LPFIELDSRESULTS lpFieldsResults, char *buff){
	  int n;
	  sprintf(buff,"\n"); n= strlen(buff);
	  sprintf(&buff[n],"Field= %s\n",lpFieldsResults->name); n= strlen(buff);
	  sprintf(&buff[n],"\n\n%s\n\n",lpFieldsResults->result); n= strlen(buff);
}

void saveResultsToBuff(LPTICKETRESULTS ticketResults, char* fileName){
	int i, n;
	char buff[5000];

	 sprintf(buff,"Status= %d\n",ticketResults->status);
     n= strlen(buff);
     saveTicketTypeToBuff(ticketResults->type, &buff[n]);
     n= strlen(buff);
     for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) {saveFieldToBuff(&ticketResults->fieldsResults[i], &buff[n]);n= strlen(buff);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "PLAYER") == 0) {saveFieldToBuff(&ticketResults->fieldsResults[i], &buff[n]);n= strlen(buff);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "EXTRA") == 0) {saveFieldToBuff(&ticketResults->fieldsResults[i], &buff[n]);n= strlen(buff);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "DRAW") == 0) {saveFieldToBuff(&ticketResults->fieldsResults[i], &buff[n]);n= strlen(buff);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATE") == 0) {saveFieldToBuff(&ticketResults->fieldsResults[i], &buff[n]);n= strlen(buff);}
    	}
    }

     saveResultsTodisk(buff, fileName);

}


void getDbgImgName(char * strN){
	int i;
    time_t mytime;
    mytime = time(NULL);
//    printf(ctime(&mytime));
    strcpy(strN, ctime(&mytime));
    for(i=0;i<strlen(strN);i++){
    	if(strN[i] < '0' ||
  		   (strN[i] > '9' && strN[i] < 'A') ||
  		   (strN[i] > 'Z' && strN[i] < 'a') ||
  		   (strN[i] > 'z'))
    		  strN[i] = '_';
    }
}

/*

char *getGameTypeStr( int gameType ){
	switch( gameType ){
		case 0: return "Lotto Texas"; break;
		case 1: return "Pick 3"; break;
		case 2: return "Cash 5"; break;
		case 3: return "Two Step"; break;
		case 4: return "Daily Four"; break;
		case 5: return "PowerBall"; break;
		case 6: return "Mega Millions"; break;
		default:
			return "Unknown type";

	}
}
*/

/*-------------------------
 * int OCR_Call_server()
 *
 OCR call on a server

  unsigned char *ptrJPG - 				compressed image buffer
  int length -						    length of the input buffer (ptrJPG)
  char *mainPath -						path where *prt, *.lib files are located
  int lotteryType -						type of lottery
  int US_state -						US state,
  char *results  -						pointer to buffer where function will return JSON format of results
  	  	  	  	  	  	  	  	  	  	it has to be allocated before call
  int resultslength	-					length of allocated buffer for output (JSON) results
  char *error -							output error message
  char *imgName - 						name of image, not obligatory, can be "", used only for debugging
-----------------------------

return value:
	 negative - error (check error message in char *error)
 	 77 - image is rejected from quality point of view; empty JSON file is created with field 'status' = 0,
 	   	   the idea is that after long enough software tuning rejected images will be automatically sent back to user
 	 >= 0 and not 77 - image is identify as good quality image, JSON file is created with OCR results
-----------------------------
current  US states:
US_state = 0 - Texas

-----------------------------
current  lottery types for TX:
lotteryType=
  1 - Powerball
  2 - Mega Millions
  3 - Lotto Texas
  4 - Texas Two Steps
  5 - Pick 3
  6 - Daily 4
  7 - Cash 5
  8 - All Or Nothing

-----------------------------


---------------------------*/

/*
  void saveLog_2(char *str1, char *str2, char *fName){
     FILE *f;
  //   int  len;
     char fileName[500];
     char buff[2000];
     strcpy(fileName, "c:\\Yoolotto\\zzz_project\\log\\");
     strcat(fileName, fName);
     f = fopen(fileName, "a");
      if(f == NULL) {
      	fprintf(stderr,"savelog (\"%s\") failed: %s\n",fileName, strerror(errno));
      	return;
      }
  //    fseek(f, 0L,SEEK_END);
  //    len = ftell(f);

     buff[0] = 0;
     if(str1) {
       strcat(buff, str1);
       strcat(buff, " ");
     }
     if(str2)
       strcat(buff, str2);

  //   if(buff[0]) fprintf (f, "%s\n", buff);
     if(buff[0]) fprintf (f, "%s", buff);
     fclose (f);
  }
*/


int OCR_Call_server(unsigned char *ptrJPG, int length, char *mainPath,	int lotteryType, int US_state, char *results, int resultslength,
		            char *error, char *imgName){
	IMGDATA img;
	IMGDATA imgOut;
	int returnValue = 0;
	int ret, byteSteps = 3;
	TICKETRESULTS ticketResults;

//	 saveLog_2("inside OCR_Call_server", "\n", "log_OCRtest.txt");
     US_State = US_state;

	img.ptr = NULL;
	imgOut.ptr = NULL;
	if(resultslength>0)  memset(results,0,resultslength);

//	sprintf("insidebuffLog,"---- adding files to the library; fileName= %s, searchName= %s\n", fileName, searchName);


		set_I_O_Path(&img, mainPath, imgName);
		set_I_O_Path(&imgOut, mainPath, imgName);

	img.error[0] = 0;
	imgOut.error[0] = 0;
	while(1){
//	printf("createBMPfromJPEG\n");
//	fflush(stdout);

		if(US_State >1 && (lotteryType < 1 || lotteryType > 2)){
	        getTicketJsonEmpty( results, resultslength);
			strcpy(error,"not implemented US state");
		    returnValue = -99;break;
		}
		ret = createBMPfromJPEG(ptrJPG, length, &img);
		if(ret < 0) {
		   sprintf(error, "decoding JPG error\n");
		   printf("decoding JPG error\n");
	       getTicketJsonEmpty( results, resultslength);
		   returnValue = -100;break;
		}
//		else{
//			if(img.ptr != NULL)
//		{
//				   char imgName[500];
//				 	setNewImageName(imgName, img.szIMGName, 0);

//			   saveBMPbuffer (img.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
//			}
//		}

// quality test
		{
//			printf("ImageTest_Quality\n");
//			fflush(stdout);
		ticketTypeInt = lotteryType;
		int imgTest = ImageTest_Quality(&img, byteSteps, &imgOut);
		    printf("imgTest= %d\n", imgTest);
		    fflush(stdout);
		    if(imgTest < 0 || imgOut.ptr == NULL || imgOut.pixPtr == NULL) {
		       getTicketJsonEmpty( results, resultslength);
//		        { char txtImgName[2000];
//		             setNewImageNameEXT(txtImgName, imgName, ".txt");
//		             saveTxtFile(results, txtImgName, "c:\\Users\\Wicek\\OCR\\output\\");
//		        }

		       return 177; // reject image
		    }

		}

//test for lotto type !!!!!!!!!!!!!!!!!!!!!!!!!!!!

		{
			   wrongTypeTest = 0;
			   setlotteryTypes();
			   retLogoClass = -1; // temporary to speed up the process
			   ticketTypeIntMy = getTypeOfTicket(&imgOut, mainPath, imgName);

//    	       printf("--------------typeIdx from input: %d, typeIdxMy= %d \n", ticketTypeInt, ticketTypeIntMy);
	    	   if(ticketTypeInt != ticketTypeIntMy){
//		    	   printf("--------------different type !!!! \n");
		    	   if(ticketTypeIntMy > 0 && ticketTypeInt <= LOTTO_TYPES_NUM_TX){
		    		   wrongTypeTest = 1;
		    	   }
	    	   }
		/*
					       if(ticketTypeInt < 0 || ticketTypeInt >= LOTTO_TYPES_NUM_TX) {
							   sprintf(error, "ticket type recognition: error= %s\n", "unknown type");
							   returnValue = -777;
							   goto SKIP_REC;
					       }
		*/

      }

//test for lotto type !!!!!!!!!!!!!!!!!!!!!!!!!!!!


//		printf("OCR_call\n");
//		fflush(stdout);
//  saveBMPbuffer (imgOut.ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
		segmentationSwitchFlag = 0;
		multiDraw = 0;
		returnValue = OCR_call(&imgOut, byteSteps, mainPath, &ticketResults, error, imgName);

//		printf("------------------returnValue= %d\n",returnValue);
//		if(returnValue >= 0) returnValue = 0;

//if(returnValue >= 0) {
//      PrintOutputResults_new(&ticketResults);
//      fflush(stdout);
//}

{// results out
	int version = 1;
	int ticketConfidenceV[10];
	int multiDrawTmp;
	multiDrawTmp = multiDraw;
	if(returnValue >= 0) {
       calculateOutputConfidence(&ticketResults, ticketConfidenceV);
       returnValue = calculateReturnValue(ticketConfidenceV);
	}
//printf("segmentationSwitchFlag = %d, wrongTypeTest= %d, ticketTypeIntMy= %d\n",segmentationSwitchFlag,wrongTypeTest, ticketTypeIntMy);
	if(segmentationSwitchFlag > 0 && wrongTypeTest == 0){
		TICKETRESULTS ticketResultsSegm;
		int returnValueSegm;

//printf("OCR again !!!\n");
		multiDraw = 0;
		returnValueSegm = OCR_call(&imgOut, byteSteps, mainPath, &ticketResultsSegm, error, imgName);
		if(returnValueSegm >= 0) {
	          calculateOutputConfidence(&ticketResultsSegm, ticketConfidenceV);
	          returnValueSegm = calculateReturnValue(ticketConfidenceV);
		}
//printf("returnValue= %d, returnValueSegm= %d\n",returnValue,returnValueSegm);
		if(segmentationCnf > 0){
            getTicketJson( &ticketResultsSegm, results, resultslength );
            version = 3;
            returnValue = returnValueSegm;

//            if(returnValue >= 0) {
//                printf("returnValue = %d, returnValueMy= %d, version= %d\n",returnValue,returnValueSegm,version);
//               PrintOutputResults_new(&ticketResultsSegm);
//          	  }

		}

	}

   if(	wrongTypeTest > 0){
		TICKETRESULTS ticketResultsMy;
		int returnValueMy;

		multiDraw = 0;
		ticketTypeInt = ticketTypeIntMy;
		returnValueMy = OCR_call(&imgOut, byteSteps, mainPath, &ticketResultsMy, error, imgName);
		if(returnValueMy >= 0) {
	          calculateOutputConfidence(&ticketResultsMy, ticketConfidenceV);
	          returnValueMy = calculateReturnValue(ticketConfidenceV);

//         printf("returnValue= %d, returnValueMy= %d\n",returnValue,returnValueMy);
//           if(returnValueMy >= returnValue){
//             if(returnValueMy >= 0){
               getTicketJson( &ticketResultsMy, results, resultslength );
        	   version = 2;
               if(returnValueMy < returnValue) returnValue = 0;
               else returnValue = returnValueMy;


//    if(returnValueMy >= 0) {
//        printf("returnValue = %d, returnValueMy= %d, version= %d\n",returnValue,returnValueMy,version);
//       PrintOutputResults_new(&ticketResultsMy);
//  	  }

//           }
		}
		if(returnValue > 0 && version == 1) returnValue = 0;
    }
    if(version == 1){

   	   multiDraw = multiDrawTmp;
       ticketTypeInt = lotteryType;
       if(returnValue >= 0) {
//    	   int ticketConfidenceV[10];
           getTicketJson( &ticketResults, results, resultslength );
//           calculateOutputConfidence(&ticketResults, ticketConfidenceV);
//           returnValue = calculateReturnValue(ticketConfidenceV);
       }
       else {
          getTicketJsonEmpty(results, resultslength );
          returnValue = 0;
       }

//     if(returnValue >= 0) {
//     	         PrintOutputResults_new(&ticketResults);
//     	  }

    }

}
/*
        { char txtImgName[2000];
             setNewImageNameEXT(txtImgName, imgName, ".txt");
             saveTxtFile(results, txtImgName, "c:\\Users\\Wicek\\OCR\\output\\");
        }
*/
        break;
	}
 	if(imgOut.ptr != NULL) {
 	   yl_free(imgOut.ptr);
 	   imgOut.ptr = NULL;
 	}

    if(img.ptr != NULL){
		yl_free(img.ptr);
    }
 return returnValue;
}


int OCR_call(LPIMGDATA img, int byteSteps, char *mainPath, LPTICKETRESULTS lpTicketResults,
		            char *error, char *imgName){
	int returnValue = 0;
	LPTICKETDATA ticketData;

	int ok;
	RECTANGLE area;
	int ver;
	HLPBYTE workAreaPtr;
	LPBYTE iDStructPtr;
	int  angle;
	RADON radonStruct;
	int imgType = 0;
	char typeStr[100];

//printf("------ start\n");
//	fflush(stdout);

#define WORKAREA_MAXSIZE 4000000


//	   US_State = US_state;

//  time_t mytime;
//  struct tm *mytm;
//  struct tm mytmTot, mytm1, mytm2;
//  int hDiff, mDiff, sDiff;
//	mytime = time(NULL);
//	mytm = localtime(&mytime);
//	copyLocalTime(&mytmTot, mytm);

//	  printf("Start     hour1= %d\n ",mytm->tm_hour);
//	  printf("Start     min1= %d\n ",mytm->tm_min);
//	  printf("Start     sec1= %d\n ",mytm->tm_sec);
//	  printf("---------------------------------\n ");
//	  printf("---------------------------------\n ");
//	   printf(ctime(&mytime));
//	  fflush(stdout);

	scratchOff_flg = 0;
	while(1){
		initTicketResults(lpTicketResults);

//printf("initTicketResults\n");
//fflush(stdout);

		ticketData = (LPTICKETDATA)yl_calloc( "OCR_D", sizeof(TICKETDATA), sizeof(BYTE));
		if(ticketData == NULL) {
			sprintf(error, "yl_calloc(ticketData) error= %s\n", strerror(errno));
			returnValue = -100;
			break;
		}

		setGlobalDataPrototype(ticketData, mainPath);
		ticketData->fieldBMPPtr = NULL;
	    ticketData->ptrTicketStruct = NULL;

		//		selecting image type
		//		saveLog("", "selecting image type\n");
		imgType = 24; // 0 - RGBA, 1 - RGB565, 24 - BMP24
		img->error[0] = 0;

		workAreaPtr.ptr = yl_calloc("workAreaPtr", WORKAREA_MAXSIZE, sizeof(BYTE));
		if(workAreaPtr.ptr == NULL) {
			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "workAreaPtr; yl_calloc failed: %s\n", strerror(errno));
			returnValue = -130;
			break;
		}
		workAreaPtr.size = WORKAREA_MAXSIZE;
		ticketData->workArea = workAreaPtr;
		initLib (ticketData);





// Black margin removal  ????????????????
//		printf("blackMarginRemoval\n");
//		fflush(stdout);

		if(US_State != 2)
		   blackMarginRemoval(img, ticketData->fieldAddress->name);

/*
		{
		   char imgName[500];
		 	setNewImageName(imgName, img->szIMGName, 1);
	        saveBMPbuffer (img->ptr, imgName, "c:\\Users\\Wicek\\OCR\\output\\");
		}
*/


		ticketData->error[0] = 0;
		//------------------------------------------
//		returnValue = LoadPrototypes (ticketData);
		//------------------------------------------

//		printf("LoadPrototypesForType\n");
//		fflush(stdout);

		//------------------------------------------
		{ char protBaseName[100];
		    getTypeStrForPROT(typeStr, ticketTypeInt);

	    printf("lottery type= %s\n", typeStr);
	    fflush(stdout);

		    getPROTName(typeStr, protBaseName);
//		    saveLog("------------", "\n");
//		    saveLog("protBaseName=  ", protBaseName);
//		    saveLog("--------------", "\n");

//		    printf("protBaseName= %s\n", protBaseName);
//		    fflush(stdout);

		    returnValue = LoadPrototypesForType (ticketData, protBaseName);
//		    printf("-------LoadPrototypesForType; returnValue= %d, protBaseName= %s\n", returnValue, protBaseName);
		}
		//------------------------------------------

		if(returnValue < 0){
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "LoadPrototypes(); error= %s\n", ticketData->error);
			returnValue = -120;
			break;
		}


//			saveLog("", "LoadPrototypes done\n");


		area.wOriginX = 0;area.wOriginY = 0;
		area.wWidth = img->bi.biWidth;area.wHeight = img->bi.biHeight;
		ver = 0;
		iDStructPtr = NULL;
		ok = ticketRecognitionInit(img, &radonStruct, &area, ver, &workAreaPtr,  &iDStructPtr,
				&angle, &ticketData->phi);
		if(ok < 0){
			ReleasePrototypes (ticketData);
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "ticketRecognitionInit(); error= %s\n", img->error);
			returnValue = -130;
			break;
		}

		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		//		saveLog(" ", "ticketRicknitionInit");
//		printf("-------ticketRecognitionInit; workAreaPtr.size= %ld\n", workAreaPtr.size);
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

//		  printf("IdentifyTicketCall(); error= %s\n", " unknown error");
//		  fflush(stdout);
			sprintf(error, "IdentifyTicketCall(); error= %s\n", " unknown error");
			returnValue = -140;
			break;
		}

//printf("reading prototype= %s\n", ticketData->prototypeName);

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

		ok = ticketRecognitionStruct(img, &radonStruct, ticketData->prototypePtr, &workAreaPtr, &ticketData->ptrTicketStruct,
				angle, ticketData->tgp);
		if(ok < 0 || workAreaPtr.size == 0){
//printf("ticketRecognitionStruct();  error\n");
			sprintf(error, "ticketRecognitionStruct(); error= %s\n", "unknown error");
			returnValue = -160;
			goto SKIP_REC;
		}

//printf("ticketRecognitionStruct() done \n");

		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		ticketData->workArea = workAreaPtr;
		//		saveLog(" ticketRecognitionStruct  ", "\n");
//printf("ProcessField(); \n");
//fflush(stdout);




		ok = ProcessField ( img, ticketData, &radonStruct, lpTicketResults);

//		mytime = time(NULL);
//		mytm = localtime(&mytime);
//		copyLocalTime(&mytm2, mytm);
//		getTimeDifference(&mytm1, &mytm2, &hDiff, &mDiff, &sDiff);
//	    printf("recognition     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);
//		  printf("---------------------------------\n ");
//		  fflush(stdout);

//		}

//printf("after ProcessField(); ticketData->returnCode= %d, ok= %d\n", ticketData->returnCode, ok);
//fflush(stdout);

		if(ticketData->returnCode < 0){
			sprintf(error, "ProcessField(); error= %s\n", "unknown error");
			returnValue = -170;
			goto SKIP_REC;

			break;
		}

		// postprocessing
	    switch(US_State) {
	       case 0:
		   case 1: // TX
				ticketPostProcessing_TX(img, ticketData, lpTicketResults);break;
		   default:
			   ticketPostProcessing_OtherStates(img, ticketData, lpTicketResults);break;
		}



//		if(outimages > 0)
//			addImagesToTheOutput(lpTicketResults, img);
		//            printResult(ticketData);

SKIP_REC:
		releaseLibMemory();
		ReleasePrototypes (ticketData);


		if(ticketData->ptrTicketStruct)
			yl_free(ticketData->ptrTicketStruct);
		if(iDStructPtr)
			yl_free(iDStructPtr);

		ClearZones (ticketData);

		if(ticketData->fieldBMPPtr!= NULL)
			yl_free(ticketData->fieldBMPPtr);
		ticketData->fieldBMPPtr= NULL; // ????????????????


		if(ticketData->ptrOut != NULL)
			yl_free(ticketData->ptrOut);
		ticketData->ptrOut = NULL;


		if(ticketData->workArea.ptr != NULL && ticketData->workArea.size> 0) {
			yl_free(ticketData->workArea.ptr);
			ticketData->workArea.ptr = NULL;
			ticketData->workArea.size = 0;
		}

		if(ticketData->prototypePtr != NULL) {
			yl_free(ticketData->prototypePtr);
			ticketData->prototypePtr = NULL;
		}

		yl_free(ticketData);
		ticketData = NULL;

		break;
	}
	ok = imgType; // to avoid compiler warning
//	mytime = time(NULL);
//	mytm = localtime(&mytime);
//	copyLocalTime(&mytm2, mytm);
//	getTimeDifference(&mytmTot, &mytm2, &hDiff, &mDiff, &sDiff);
//    printf("total time     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);

	return returnValue;




	return returnValue;
}
// buffer types:  RGBA,  RGB565; otherwise unknown
int OCR_Call_new(unsigned char *ptr, int imgWidth, int imgHeight, char *buffType, int byteSteps, int outimages,
		LPIMGDATA imgOut, char *mainPath,	LPTICKETRESULTS lpTicketResults, char *error, char *imgName, int US_state){
	IMGDATA img;
	LPTICKETDATA ticketData;

	int ok;
	RECTANGLE area;
	int ver;
	HLPBYTE workAreaPtr;
	LPBYTE iDStructPtr;
	int  angle;
	RADON radonStruct;
//		char bufLog[500];
	int returnValue = 0;
	int imgType = 0;
	char typeStr[100];

//printf("------ start\n");
//	fflush(stdout);

#define WORKAREA_MAXSIZE 4000000


//  time_t mytime;
//  struct tm *mytm;
//  struct tm mytmTot, mytm1, mytm2;
//  int hDiff, mDiff, sDiff;


//	mytime = time(NULL);
//	mytm = localtime(&mytime);
//	copyLocalTime(&mytmTot, mytm);


//	  printf("Start     hour1= %d\n ",mytm->tm_hour);
//	  printf("Start     min1= %d\n ",mytm->tm_min);
//	  printf("Start     sec1= %d\n ",mytm->tm_sec);
//	  printf("---------------------------------\n ");
//	  printf("---------------------------------\n ");
//	   printf(ctime(&mytime));
	  fflush(stdout);



	scratchOff_flg = 0;
	ticketTypeInt = -1;
	while(1){
		initTicketResults(lpTicketResults);

//printf("initTicketResults\n");
//fflush(stdout);

		img.ptr = NULL;
		imgOut->ptr = NULL;
		ticketData = (LPTICKETDATA)yl_calloc( "OCR_D", sizeof(TICKETDATA), sizeof(BYTE));
		if(ticketData == NULL) {
			sprintf(error, "yl_calloc(ticketData) error= %s\n", strerror(errno));
			returnValue = -100;
			break;
		}

		setGlobalDataPrototype(ticketData, mainPath);
		ticketData->fieldBMPPtr = NULL;
	    ticketData->ptrTicketStruct = NULL;

		set_I_O_Path(&img, mainPath, imgName);
		set_I_O_Path(imgOut, mainPath, imgName);

		//		selecting image type
		//		saveLog("", "selecting image type\n");
		imgType = getImageType(buffType); // 0 - RGBA, 1 - RGB565, 24 - BMP24
		if(imgType < 0) {
			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "not supported image type;\n");
			break;
		}
		img.error[0] = 0;
		if(US_state == 3)
		  returnValue = setRGBAImgeHeader(ptr, imgWidth, imgHeight, &img, imgType);
		else
		   returnValue = setRGBAImgeHeaderNoCopy(ptr, imgWidth, imgHeight, &img, imgType);

		if(returnValue < 0){
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "setRGBAImgeHeader(); error= %s\n", img.error);
			break;
		}

		workAreaPtr.ptr = yl_calloc("workAreaPtr", WORKAREA_MAXSIZE, sizeof(BYTE));
		if(workAreaPtr.ptr == NULL) {
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
//			if(imgOut->ptr != NULL) {
//				yl_free(imgOut->ptr);
//				imgOut->ptr = NULL;
//			}
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "workAreaPtr; yl_calloc failed: %s\n", strerror(errno));
			returnValue = -130;
			break;

		}
		workAreaPtr.size = WORKAREA_MAXSIZE;
		ticketData->workArea = workAreaPtr;
		initLib (ticketData);

		//		saveLog("", "preProcess color");
		img.error[0] = 0;
		pre_ProcessColorImageBMP(&img, imgOut, byteSteps, ticketData, US_state, scratchOff_flg);
		if(img.error[0] != 0){
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
			yl_free(ticketData);
			ticketData = NULL;
			sprintf(error, "pre_ProcessColorImageBMP; error= %s\n", img.error);
			returnValue = -110;
			break;
		}

//		 saveBMPbuffer (imgOut->ptr, imgOut->szIMGName, imgOut->szOutPath);

// release color bmp

		if(img.ptr != NULL){
			yl_free(img.ptr);
			img.ptr = NULL;
		}

//------------------------------------------
// skip OCR

/*

		releaseLibMemory();
		if(ticketData->workArea.ptr != NULL && ticketData->workArea.size> 0) {
			yl_free(ticketData->workArea.ptr);
			ticketData->workArea.ptr = NULL;
			ticketData->workArea.size = 0;
		}
		yl_free(ticketData);
		ticketData = NULL;
		if(outimages > 0) {
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
		}

		returnValue = -110;
		break;
*/
//------------------------------------------
//		mytime = time(NULL);
//		mytm = localtime(&mytime);
//		copyLocalTime(&mytm2, mytm);
//		getTimeDifference(&mytmTot, &mytm2, &hDiff, &mDiff, &sDiff);
//	    printf("mono conversion     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);
//	    copyLocalTime(&mytm1, mytm);
//		  printf("---------------------------------\n ");
//		  fflush(stdout);

//test for lotto type !!!!!!!!!!!!!!!!!!!!!!!!!!!!

		{//IMGDATA imgOut2;
//		   char bufLog[100];
			LPBYTE yLogoBinBufferPtr = NULL;
			int lenBinBuffer= 0;
//			int retLogoClass;

		   iDStructPtr = NULL;
		   US_State = US_state;

//		   retLogoClass = initLogoClassifier(ticketData, &yLogoBinBufferPtr, &lenBinBuffer);
//		   if(retLogoClass < 0)
//	         printf("--------------retLogoClass: %d, error= %s \n", retLogoClass, ticketData->error);
//		   else
//		   printf("--------------retLogoClass: %d\n", retLogoClass);
//      fflush(stdout);

		   retLogoClass = -1; // temporary to speed up the process
		   ticketTypeInt = getTypeOfTicket(imgOut, mainPath, imgName);


		   getTypeStr(typeStr, ticketTypeInt);

//    	   printf("--------------ticket type: %s, typeIdx= %d \n", typeStr, ticketTypeInt);
//    	   sprintf(bufLog,"--------------ticket type: %s, typeIdx= %d \n", typeStr, ticketTypeInt);
//           saveLog(bufLog, "");
//	       fflush(stdout);
	       if(yLogoBinBufferPtr != NULL && lenBinBuffer > 0)
	    	   yl_free(yLogoBinBufferPtr);
	       if(ticketTypeInt < 0 || ticketTypeInt >= LOTTO_TYPES_NUM_TX) {
			   sprintf(error, "ticket type recognition: error= %s\n", "unknown type");
			   returnValue = -777;
			   goto SKIP_REC;
	       }

		}


//		mytime = time(NULL);
//		mytm = localtime(&mytime);
//		copyLocalTime(&mytm2, mytm);
//		getTimeDifference(&mytm1, &mytm2, &hDiff, &mDiff, &sDiff);
//	    printf("type recog     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);

//		copyLocalTime(&mytm1, mytm);
//		  printf("---------------------------------\n ");
//		  fflush(stdout);



//test for lotto type !!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Black margin removal
		if(US_State != 2)
		   blackMarginRemoval(imgOut, ticketData->fieldAddress->name);

//		saveBMPbuffer (imgOut->ptr, imgOut->szIMGName, "c:\\Users\\Wicek\\OCR\\output\\");
		//		saveLog("", "LoadPrototypes");

		ticketData->error[0] = 0;
		//------------------------------------------
//		returnValue = LoadPrototypes (ticketData);
		//------------------------------------------

		//------------------------------------------
		{ char protBaseName[100];
		    getTypeStrForPRT(typeStr, ticketTypeInt);
		    getPrototypeName(typeStr, protBaseName);
		    saveLog("------------", "\n");
		    saveLog("protBaseName=  ", protBaseName);
		    saveLog("--------------", "\n");

		    returnValue = LoadPrototypesForType (ticketData, protBaseName);
//		    printf("-------LoadPrototypesForType; returnValue= %d, protBaseName= %s\n", returnValue, protBaseName);
		}
		//------------------------------------------

		if(returnValue < 0){
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "LoadPrototypes(); error= %s\n", ticketData->error);
			returnValue = -120;
			break;
		}


//			saveLog("", "LoadPrototypes done\n");


		area.wOriginX = 0;area.wOriginY = 0;
		area.wWidth = imgOut->bi.biWidth;area.wHeight = imgOut->bi.biHeight;
		ver = 0;
		iDStructPtr = NULL;
		ok = ticketRecognitionInit(imgOut, &radonStruct, &area, ver, &workAreaPtr,  &iDStructPtr,
				&angle, &ticketData->phi);
		if(ok < 0){
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
			ReleasePrototypes (ticketData);
			yl_free(ticketData);
			ticketData = NULL;

			sprintf(error, "ticketRecognitionInit(); error= %s\n", imgOut->error);
			returnValue = -130;
			break;
		}


//LOGD("0. size of workAreaPtr:  %ld\n", workAreaPtr.size);

		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		//		saveLog(" ", "ticketRicknitionInit");
//		printf("-------ticketRecognitionInit; workAreaPtr.size= %ld\n", workAreaPtr.size);
/*
		if(workAreaPtr.ptr != NULL) {
			yl_free(workAreaPtr.ptr);
			workAreaPtr.size = 0;
			workAreaPtr.ptr = NULL;
		}
*/
		ticketData->iAngle = angle;
		ok = IdentifyTicketCall (&radonStruct, &workAreaPtr, ticketData, iDStructPtr);
		if(ok < 0){
			if(ticketData->ptrOut != NULL)
				yl_free(ticketData->ptrOut);
			ticketData->ptrOut = NULL;
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
			ReleasePrototypes (ticketData);
			if(iDStructPtr)
				yl_free(iDStructPtr);
			if(workAreaPtr.ptr != NULL) {
				yl_free(workAreaPtr.ptr);
				workAreaPtr.size = 0;
			}

			yl_free(ticketData);
			ticketData = NULL;

			//			saveLog(" IdentifyTicketCall failed  ", "\n");
//		  printf("IdentifyTicketCall(); error= %s\n", " unknown error");
//		  fflush(stdout);
			sprintf(error, "IdentifyTicketCall(); error= %s\n", " unknown error");
			returnValue = -140;
			break;
		}

/*
		if(workAreaPtr.ptr != NULL) {
			yl_free(workAreaPtr.ptr);
			workAreaPtr.ptr = NULL;
			workAreaPtr.size = 0;
		}
*/
		//		saveLog("reading prototype= ", ticketData->prototypeName);
		//		saveLog("","\n");

		ticketData->error[0] = 0;
		ok = LoadPrototype (&ticketData, 0);
		if(ok < 0){
			if(ticketData->ptrOut != NULL)
				yl_free(ticketData->ptrOut);
			ticketData->ptrOut = NULL;
			if(img.ptr != NULL){
				yl_free(img.ptr);
				img.ptr = NULL;
			}
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
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

//				saveLog(" before ticketRecognitionStruct  ", "\n");
		ok = ticketRecognitionStruct(imgOut, &radonStruct, ticketData->prototypePtr, &workAreaPtr, &ticketData->ptrTicketStruct,
				angle, ticketData->tgp);
//				sprintf(buffLog,"ok = %d\n",ok);
//				saveLog("", buffLog);
		if(ok < 0 || workAreaPtr.size == 0){
//printf("ticketRecognitionStruct();  error\n");
			sprintf(error, "ticketRecognitionStruct(); error= %s\n", "unknown error");
			returnValue = -160;
			goto SKIP_REC;
		}

//printf("ticketRecognitionStruct() done \n");

		ticketData->zone->ZoneQuad =   *(LPQUADRANGLE)(workAreaPtr.ptr + 8);
		ticketData->workArea = workAreaPtr;
		//		saveLog(" ticketRecognitionStruct  ", "\n");
//	LOGD("1. size of workAreaPtr:  %ld\n", workAreaPtr.size);
//		initLib (ticketData);
//	LOGD("2. size of workAreaPtr:  %ld\n", workAreaPtr.size);
//				saveLog("before ProcessField\n", "");

//printf("ProcessField(); \n");
//fflush(stdout);

//		mytime = time(NULL);
//		mytm = localtime(&mytime);
//		copyLocalTime(&mytm2, mytm);
//		getTimeDifference(&mytm1, &mytm2, &hDiff, &mDiff, &sDiff);
//	    printf("prepare to recognition     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);
//		copyLocalTime(&mytm1, mytm);
//		  printf("---------------------------------\n ");
//		  fflush(stdout);



		ok = ProcessField ( imgOut, ticketData, &radonStruct, lpTicketResults);

//		mytime = time(NULL);
//		mytm = localtime(&mytime);
//		copyLocalTime(&mytm2, mytm);
//		getTimeDifference(&mytm1, &mytm2, &hDiff, &mDiff, &sDiff);
//	    printf("recognition     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);
//		  printf("---------------------------------\n ");
//		  fflush(stdout);

//		}

		//    saveLog(" ProcessField  ", "\n");
//printf("after ProcessField(); ticketData->returnCode= %d\n", ticketData->returnCode);
//fflush(stdout);

		if(ticketData->returnCode < 0){
			sprintf(error, "ProcessField(); error= %s\n", "unknown error");
			returnValue = -170;
			goto SKIP_REC;

			break;
		}

		// postprocessing
	    switch(US_State) {
		   case 0: // TX
	       case 1:
				ticketPostProcessing_TX(imgOut, ticketData, lpTicketResults);break;
//		   case 1: // PA
//			   ticketPostProcessing_PA(ticketData, lpTicketResults);break;
		   case 2: // CA
			   ticketPostProcessing_CA(ticketData, lpTicketResults);break;
		   case 3: // NY
			   ticketPostProcessing_NY(ticketData, lpTicketResults);break;
		   default:
			   ticketPostProcessing_TX(imgOut, ticketData, lpTicketResults);break;
		}



		if(outimages > 0)
			addImagesToTheOutput(lpTicketResults, imgOut);
		//            printResult(ticketData);

		SKIP_REC:
		releaseLibMemory();
		ReleasePrototypes (ticketData);


		if(ticketData->ptrTicketStruct)
			yl_free(ticketData->ptrTicketStruct);
		if(iDStructPtr)
			yl_free(iDStructPtr);

		ClearZones (ticketData);

		if(ticketData->fieldBMPPtr!= NULL)
			yl_free(ticketData->fieldBMPPtr);
		ticketData->fieldBMPPtr= NULL; // ????????????????


		if(ticketData->ptrOut != NULL)
			yl_free(ticketData->ptrOut);
		ticketData->ptrOut = NULL;


		if(ticketData->workArea.ptr != NULL && ticketData->workArea.size> 0) {
			yl_free(ticketData->workArea.ptr);
			ticketData->workArea.ptr = NULL;
			ticketData->workArea.size = 0;
		}

		if(ticketData->prototypePtr != NULL) {
			yl_free(ticketData->prototypePtr);
			ticketData->prototypePtr = NULL;
		}

		yl_free(ticketData);
		ticketData = NULL;

		if(outimages > 0) {
			if(imgOut->ptr != NULL) {
				yl_free(imgOut->ptr);
				imgOut->ptr = NULL;
			}
		}
		break;
	}

//	mytime = time(NULL);
//	mytm = localtime(&mytime);
//	copyLocalTime(&mytm2, mytm);
//	getTimeDifference(&mytmTot, &mytm2, &hDiff, &mDiff, &sDiff);
//    printf("total time     time= %d h %d m %d s\n ",hDiff, mDiff, sDiff);

	return returnValue;
}



void cleanChainOfFields(LPOUTIMGS lpImgs){
	LPOUTIMGS lpImgsTmp;
	LPOUTIMGS lpImgsNext;

	lpImgsTmp = lpImgs;
	while(lpImgsTmp) {
		lpImgsNext = lpImgsTmp->ptrNext;
		if(lpImgsTmp->lpImgBuffer != NULL){
			yl_free(lpImgsTmp->lpImgBuffer);
			lpImgsTmp->lpImgBuffer = NULL;
		}
		lpImgsTmp = lpImgsNext;
	}
}

void cleanStructureOut(LPOUTIMGS lpImgs){
	LPOUTIMGS lpImgsTmp;
	LPOUTIMGS lpImgsNext;

	lpImgsTmp = lpImgs;
	while(lpImgsTmp) {
		lpImgsNext = lpImgsTmp->ptrNext;
		yl_free(lpImgsTmp);
		lpImgsTmp = lpImgsNext;
	}
}



void FreeTicketResultsMemory_Call(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut){
	int i;
	LPOUTIMGS lpImgs;
	for(i=0;i<NUMFIELDS_RESULTS;i++){
		if(lpTicketResults->fieldsResults[i].ptrOutImages != NULL){
			lpImgs = lpTicketResults->fieldsResults[i].ptrOutImages;
			cleanChainOfFields(lpImgs);
			cleanStructureOut(lpImgs);
			lpTicketResults->fieldsResults[i].ptrOutImages = NULL;
		}
	}
	if(imgOut->ptr != NULL) {
		yl_free(imgOut->ptr);
		imgOut->ptr = NULL;
	}

}

void GetMonochromaticImage_Call(LPIMGDATA imgOut, BYTE **ptr, int *size ){
	 *ptr =  imgOut->ptr;
	 *size =  *(LPDWORD)(imgOut->ptr + BFSIZEOFF);
// printf("---------------*size= %d\n", *size );
}



