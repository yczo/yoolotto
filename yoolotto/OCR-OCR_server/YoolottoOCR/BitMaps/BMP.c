/*
 * BMP.c
 *
 *  Created on: Jul 24, 2012
 *      Author: Wicek
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include  "../headers/Functions.h"
#include "../headers/logging.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"

//-----------------------------------------------------------------------------
//      Function to load image from disk in the memory block.
//      Input:   image structure .
//      Returns: address of memory with the DIB, including file header.
//
//      Function reads the file and recognizes its type
//      regardless of the file extension (BMP, PCX, etc.).
//
//	    If the file type is a BMP then function saves all important data to the image structure
//
//      This function handles all types of color BMP files (without RLE
//      compression) independently. All other file types will be handled by the
//      separate  module.
//void saveLog(char *str1, char *str2);

static void saveFile (LPIMGDATA img, char *szIMGName, LPBYTE ptr, int len)
{
//	LOGD("****** saveFile called??? ********");

	FILE *f;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;
	char *szFileName = szIMGName;
	char *szPathName = img->szOutPath;

	if (!szFileName) return;  // test only
	strcpy(fileName, szPathName);
	strcat(fileName, szFileName);

	printf("save file %s\n",fileName);

	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMP (\"%s\") failed: %s\n",fileName, strerror(errno));
		return;
	}

	fSize = fwrite(ptr, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			sprintf(img->error,"saveFile (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
			sprintf(img->error,"saveFile (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		return;
	}

	close_file(f);
}
void saveTxtFile (char *buff, char *szIMGName, char *szOutPath)
{
//	LOGD("****** saveFile called??? ********");

	FILE *f;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;
	char *szFileName = szIMGName;
	char *szPathName = szOutPath;
	int len = strlen(buff);

	if (!szFileName) return;  // test only
	strcpy(fileName, szPathName);
	strcat(fileName, szFileName);

	printf("save file %s\n",fileName);

	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMP (\"%s\") failed: %s\n",fileName, strerror(errno));
		return;
	}

	fSize = fwrite(buff, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			printf("saveFile (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
			printf("saveFile (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		return;
	}

	close_file(f);
}

LPBYTE LoadBMPFile (LPIMGDATA img)
{
	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	LPBYTE ptr;
	size_t fSize = 0;
	char *szFileName = img->szIMGName;
	char *szPathName = img->szIMGPath;

//	char bufflog[500];

	img->error[0] = 0;
	if (!szFileName) return NULL;  // test only
	strcpy(fileName, szPathName);
	strcat(fileName, szFileName);


	//	printf("open file %s\n",fileName);
//	saveLog("\n","\n");
//	sprintf(bufflog,"open file %s\n",fileName);
//	saveLog(bufflog,"\n");

	f= fopen(fileName, "r");
	if(f == NULL) {
		sprintf(img->error,"LoadImage (\"%s\") failed: %s\n",fileName, strerror(errno));
		//    	fprintf(stderr,"LoadImage (\"%s\") failed: %s\n",fileName, strerror(errno));
		return NULL;
	}

	fseek(f, 0L,SEEK_END);
	//    printf("fseek %s\n",fileName);

	if(ferror(f)){
		sprintf(img->error,"LoadImage (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
		//    	fprintf(stderr,"LoadImage (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
		return NULL;
	}
	//    printf("ftell %s\n",fileName);

	len = ftell(f);
	//    printf("fseek %s\n",fileName);

	fseek(f, 0L,SEEK_SET);


	//    printf("file %s length= %d\n",fileName, len);

	ptr = yl_calloc( "BMP_A", len, sizeof(BYTE));
	if(ptr == NULL) {
		printf("yl_calloc failed:loadBMP file\n");
		return NULL;
	}
	fSize = fread((char *)ptr, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			sprintf(img->error,"LoadImage (\"%s\") read failed: %s\n",fileName, strerror(errno));
		//        	fprintf(stderr,"LoadImage (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
			sprintf(img->error,"LoadImage (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		//           fprintf(stderr,"LoadImage (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		yl_free(ptr);
		return NULL;
	}

	//    LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(ptr);
	//    char w = 'B';
	//    char bType = (char)bf->bfType;
	//    printf("bf->bfType= %c, bType= %c, w= %c \n",bf->bfType, bType, w);
	//  printf("fSize=  %d\n",fSize);

	if(testBMP (ptr, fSize*len, img ) < 0) {
		yl_free(ptr);
		ptr = NULL;
	}

	//	printLines(img, 0, 1);
	//     testColorArray(img);

	//     printBMPData ( img);

	//     saveFile (img, "test_WB.BMP", img->ptr, img->bf.bfSize);

	close_file(f);
	return ptr;
}



int close_file(FILE *f){
	int s = 0;
	if(f == NULL) return 0;
	errno = 0;
	fflush(f);
	s = fclose(f);
	if(s == EOF) perror("Close failed");
	return s;
}


//      Loading of Windows BMP file. Compressed bitmaps are NOT loaded.
//      No actions pertaining to the color processing.

int testBMP (LPBYTE ptr, size_t fSize, LPIMGDATA img) {

	copyHeadersBMP(ptr, img);
	//    printBMPData ( img);

	if (!isItBMP(img)) {
		strcpy(img->error,  "this is not BMP Image");
		return -5;
	}

	{
		unsigned long uFlth = img->bf.bfOffBits + ((img->bi.biWidth * img->bi.biBitCount + 31)
				/ 32) * 4 * img->bi.biHeight;

		if ((img->bi.biWidth  > (LONG)MAX_WIDTH) ||
				(img->bi.biHeight > (LONG)MAX_HEIGHT)) {
			//  printf("bi->biWidth= %ld, bi->biHeight= %ld\n",bi->biWidth, bi->biHeight);
			//  printf("MAX_WIDTH= %d, MAX_HEIGHT= %ld\n",MAX_WIDTH, MAX_HEIGHT);

			strcpy(img->error,  "Image size exceeds allowed limit");
			return -5;
		}

		// verify if the the length of file is the same as in the bmp header
		if (fSize < (LONG)uFlth) {
			// printf("uFlth= %ld, fSize= %ld\n",uFlth, (LONG)fSize);
			strcpy(img->error,  "Image/file length problem");
			return -5;
		}
		// verify that bmp is not compressed

		//   if (((LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER)))->
		//                                                biCompression != BI_RGB) {
		if(img->bi.biCompression != BI_RGB) {
			strcpy(img->error,  "bmp is compressed");
			return -5;
		}
	}
	return 0;
}



void printBMPData (LPIMGDATA img) {
	saveLog( "", "printBMPData");

	char buf[1024];

	//    printf("bf.bfType= %d \n",img->bf.bfType);
	sprintf(buf, "bf.bfType= %x \n",img->bf.bfType);
	saveLog( "", buf );
	sprintf(buf,"bf.bfSize= %lu, bf.bfOffBits= %lu \n",img->bf.bfSize, img->bf.bfOffBits);
	saveLog( "", buf );

	sprintf(buf,"bi.biSize= %lu, bi.biWidth= %d, bi.biHeight= %d  \n",img->bi.biSize, img->bi.biWidth, img->bi.biHeight);
	saveLog( "", buf );

	sprintf(buf,"bi.biPlanes= %d, bi.biBitCount= %d, bi.biCompression= %d  \n",img->bi.biPlanes, img->bi.biBitCount, img->bi.biCompression);
	saveLog( "", buf );

	sprintf(buf,"bi.biSizeImage= %lu, bi.biXPelsPerMeter= %d, bi.biYPelsPerMeter= %d  \n",img->bi.biSizeImage, img->bi.biXPelsPerMeter, img->bi.biYPelsPerMeter);
	saveLog( "", buf );

	sprintf(buf,"bi.biClrUsed= %d bi.biClrImportant= %d \n",img->bi.biClrUsed, img->bi.biClrImportant);
	saveLog( "", buf );

	sprintf(buf,"bi.biWidthPad= %u \n",img->bi.biWidthPad);
	saveLog( "", buf );

	/*
    {int i;
    BYTE test;
       for(i=0;i<20;i++) {
         test = *(img->ptr + 54+i);
         printf("i= %d, test= %c testx= %x\n",i, test, test);
       }

    for(i=0;i<8;i++) {
      test = *(img->ptr + 62+i);
      printf("i= %d, test= %d \n",i, test);
    }

    }
	 */


}
void printBMPSize (LPIMGDATA img) {

	printf("imgName= %s \n",img->szIMGName);
	printf("bi.biSize= %lu, bi.biWidth= %d, bi.biHeight= %d  \n",img->bi.biSize, img->bi.biWidth, img->bi.biHeight);
	printf("bi.biWidthPad= %u \n",img->bi.biWidthPad);

}

int isItBMP(LPIMGDATA img) {
	//    int w = 0x4d42;

	if(img->bf.bfType != 0x4d42)
		return 0;
	//    printf("bf->bfType= %x, w= %x\n",(unsigned int)bf->bfType, w);

	return 1;
}
int getLeftBytes(int* ptr, int num_bytes){
	unsigned int mask=LEFT_BYTE;
	unsigned int *iPtr = (unsigned int*)ptr;
	unsigned int iTmp1, iTmp2;
	int i;
	iTmp1 =   *iPtr;
	iTmp1 = iTmp1&mask;
	i = 1;
	while(i<num_bytes) {
		iTmp2 =   *iPtr;
		mask <<= 8;
		iTmp2 = iTmp2&mask;
		iTmp1 = iTmp1|iTmp2;
		i++;
	}
	return iTmp1;
}
BYTE getBitsFromTo(LPBYTE ptr, int fromB, int toB){
	BYTE mask=LEFT_BIT;
	BYTE iTmp1, iTmp2;
	int i;
	iTmp1 =   *ptr;
	mask >>= fromB;

	printf("------------------------------------------------------\n");
	printf("iTmp1= %d, mask= %d\n",iTmp1, mask);
	printf("iTmp1= %x, mask= %x\n",iTmp1, mask);

	iTmp1 = iTmp1&mask;

	printf("iTmp1= %d\n",iTmp1);

	i = 1;
	while(i<toB-fromB+1) {
		iTmp2 =   *ptr;
		mask >>= 1;
		printf("iTmp2= %d, mask= %d\n",iTmp2, mask);
		printf("iTmp2= %x, mask= %x\n",iTmp2, mask);

		iTmp2 = iTmp2&mask;
		iTmp1 = iTmp1|iTmp2;
		printf("iTmp2= %d, iTmp1= %d\n",iTmp2, iTmp1);
		printf("iTmp2= %x, iTmp1= %x\n",iTmp2, iTmp1);

		i++;
	}
	iTmp1 >>= (7-toB);
	return iTmp1;


}

void copyHeadersBMP(LPBYTE ptr, LPIMGDATA img) {
	//	 LPBYTE biPtr = ptr + BFOOFBITSOFF+4;
	LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(ptr);
	LPBITMAPINFOHEADER bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));

	img->ptr = ptr;
	img->bf.bfType = getLeftBytes((int*)bf->bfType, 2);
	img->bf.bfSize = getLeftBytes((int*)bf->bfSize, 4);
	img->bf.bfReserved1 =  getLeftBytes((int*)bf->bfReserved1, 2);
	img->bf.bfReserved2 =  getLeftBytes((int*)bf->bfReserved2, 2);
	img->bf.bfOffBits =   getLeftBytes((int*)bf->bfOffBits, 4);
	img->pixPtr = ptr +img->bf.bfOffBits;

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
	img->colorPtr = ptr + 14 + img->bi.biSize;
	img->width = img->bi.biWidth;
	img->height = img->bi.biHeight;

	/*
    img->ptr = ptr;
    img->bf.bfType = *(LPWORD)(ptr+BFTYPEOFF);
    img->bf.bfSize = *(LPDWORD)(ptr+BFSIZEOFF);
    img->bf.bfReserved1 =  *(LPWORD)(ptr+BFRESERVED1OFF);
    img->bf.bfReserved2 =  *(LPWORD)(ptr+BFRESERVED2OFF);
    img->bf.bfOffBits =   *(LPWORD)(ptr+BFOOFBITSOFF);
    img->pixPtr = ptr +img->bf.bfOffBits;


     img->bi.biSize = *(LPDWORD)(biPtr+BISIZEOFF);
     img->bi.biWidth = *(LPDWORD)(biPtr+BIWIDTHOFF);
     img->bi.biHeight = *(LPDWORD)(biPtr+BIHEIGHTOFF);
     img->bi.biPlanes = *(LPWORD)(biPtr+BIPLANESOFF);
     img->bi.biBitCount = *(LPWORD)(biPtr+BIBITCOUNTOFF);
     img->bi.biCompression = *(LPDWORD)(biPtr+BICOMPRESSIONOFF);
     img->bi.biSizeImage = *(LPDWORD)(biPtr+BISIZEIMAGEOFF);
     img->bi.biXPelsPerMeter = *(LPDWORD)(biPtr+BIXPELSPERMETEROFF);
     img->bi.biYPelsPerMeter = *(LPDWORD)(biPtr+BIYPELSPERMETEROFF);
     img->bi.biClrUsed = *(LPDWORD)(biPtr+BICLRUSEDOFF);
     img->bi.biClrImportant = *(LPDWORD)(biPtr+BICLRIMPORTANTOFF);

     img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
     img->colorPtr = ptr + 14 + img->bi.biSize;
     img->width = img->bi.biWidth;
     img->height = img->bi.biHeight;
	 */
}




//  Function returning pixel xn, yn

int whatIsThisImg(LPIMGDATA img, int xn, int yn){
	//    short mask, ch, w;
	int  w;
	long nY;
	long nX;
	LPBYTE ptr;
	int sh= img->bi.biBitCount/8;
	int ret;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;


	nY=yn*w;
	if(img->bi.biBitCount < 8) {
		ptr=img->pixPtr+nY+(LONG)(xn/8);
		//           ret  = getLeftBits(ptr, bi.biBitCount);
		ret = 0;
	}
	else {
		nX = (long)(xn*sh);
		ptr=img->pixPtr+nY+nX;
		ret  = getLeftBytes((int *)ptr, sh);
	}
	return ret;
} /* whatIsThisImg() */
void getColor32(LPIMGDATA img, int xn, int yn, LPBYTE R, LPBYTE G, LPBYTE B){
	int  w;
	long nY;
	long nX;
	//    int idx;
	int sh = 4;
	LPBYTE ptr;
	//    int i;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=img->pixPtr+nY+nX;
	//        idx = *ptr;
	//        idx = idx*4;
	*B=*ptr;
	*G=*(ptr+1);
	*R=*(ptr+2);
	//        printf("B= %d, G= %d, R= %d ", *B, *G, *R);
}


void getColor24(LPIMGDATA img, int xn, int yn, LPBYTE R, LPBYTE G, LPBYTE B){
	int  w;
	long nY;
	long nX;
	//    int idx;
	int sh = 3;
	LPBYTE ptr;
	//    int i;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=img->pixPtr+nY+nX;
	//        idx = *ptr;
	//        idx = idx*4;
	*B=*ptr;
	*G=*(ptr+1);
	*R=*(ptr+2);
	//        printf("B= %d, G= %d, R= %d ", *B, *G, *R);
}

void getColor8(LPIMGDATA img, int xn, int yn, LPBYTE R, LPBYTE G, LPBYTE B){
	int  w;
	long nY;
	long nX;
	int idx;
	LPBYTE ptr;
	//    int i;

	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)xn;
	ptr=img->pixPtr+nY+nX;
	idx = *ptr;
	//        *B=(BYTE)(*(img->colorPtr +idx));
	//        *G=(BYTE)(*(img->colorPtr +idx+1));
	//        *R=(BYTE)(*(img->colorPtr +idx+2));
	//        printf("idx= %d, B= %d, G= %d, R= %d \n",idx, *B, *G, *R);
	idx = idx*4;
	*B=(BYTE)(*(img->colorPtr +idx));
	*G=(BYTE)(*(img->colorPtr +idx+1));
	*R=(BYTE)(*(img->colorPtr +idx+2));
	//        printf("idx= %d, B= %d, G= %d, R= %d ",idx, *B, *G, *R);

	//        printf("idx= %d, value= %d ",idx, *(img->colorPtr +idx));
	//        for(i=0;i<256;i++){
	//        	printf("idx= %d, value= %d \n",i, *(img->colorPtr +i));
	//        }
	//        return (*(img->colorPtr +idx));
}
void getRGB(LPIMGDATA img, int xn, int yn, BYTE *R, BYTE *G, BYTE *B){
	int what;
	if(img->bi.biBitCount < 8) {
		what = whatIsThisImg(img, xn, yn);
		*R = what;
		*G = what;
		*B = what;
		return;
	}
	if(img->bi.biBitCount == 8) {
		getColor8(img, xn, yn, R, G, B);
		//		*G = getColor8(img, xn, yn);
		//		*B = getColor8(img, xn, yn);
		return;
	}
	if(img->bi.biBitCount == 24) {
		getColor24(img, xn, yn, R, G, B);
		return;
	}
	if(img->bi.biBitCount == 32) {
		getColor32(img, xn, yn, R, G, B);
		return;
	}

}

void setPixelWB(LPIMGDATA img, int xn, int yn, int Th){
	BYTE R, G, B;
	getRGB(img, xn, yn, &R, &G, &B);

	// printf("xn= %d, yn= %d ",xn, yn);
	// printf("R= %d, G= %d, B= %d \n",R, G, B);

	if(R < Th/2 && G < Th && B < Th/2)
		setPixel(img, xn, yn, 0);
	else
		setPixel(img, xn, yn, 255);

}

unsigned int setColorRGB ( BYTE R, BYTE G, BYTE B) {
	// 	 unsigned int mask=RIGHT_BYTE;
	unsigned int uiV = 0x0;
	unsigned int iTmp;
	uiV =   R;
	iTmp =  G;
	iTmp <<= 8;
	uiV = uiV|iTmp;
	iTmp =  B;
	iTmp <<= 16;
	uiV = uiV|iTmp;
	return uiV;

}


void setLeftBytes (LPBYTE ptr, int n, int v) {
	unsigned int mask=LEFT_BYTE;
	unsigned int uiV = (unsigned int)v;
	unsigned int iTmp;
	int i;
	i = 0;
	while(i<n) {
		iTmp =   uiV;
		iTmp = iTmp&mask;
		*ptr = (BYTE)iTmp;
		mask <<= 8;
		i++;
	}
}
void setPixel8(LPIMGDATA img, int xn, int yn, BYTE value){
	int  w;
	long nY;
	long nX;
	LPBYTE ptr;
	int sh= img->bi.biBitCount/8;
	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;

	nY=yn*w;
	nX = (long)(xn*sh);
	ptr=img->pixPtr+nY+nX;
	*ptr = value;
}
void setPixelWB24(LPBYTE ptr, BYTE value){
	*ptr = value;
	*(ptr+1) = value;
	*(ptr+2) = value;
}
void setPixel(LPIMGDATA img, int xn, int yn, int value){
	int  w;
	long nY;
	long nX;
	LPBYTE ptr;
	int sh= img->bi.biBitCount/8;
	w=img->bi.biWidthPad;
	yn=img->bi.biHeight-1-yn;

	nY=yn*w;
	if(img->bi.biBitCount < 8) {
		ptr=img->pixPtr+nY+(LONG)(xn/8);
		//           ret  = getLeftBits(ptr, bi.biBitCount);
		return;
	}
	if(img->bi.biBitCount == 24) {
		nX = (long)(xn*sh);
		ptr=img->pixPtr+nY+nX;
		setPixelWB24(ptr, value);
	}
	else {
		nX = (long)(xn*sh);
		ptr=img->pixPtr+nY+nX;
		setLeftBytes(ptr, sh, value);
	}
}
void printLines(LPIMGDATA img, int h, int n) {
	int i, k;
	int out;
	for(i=0;i<n;i++){
		for(k=0;k<img->bi.biWidth;k++){
			out = whatIsThisImg(img, k, h+i);
			printf("%d ",out);
			if(k%50==0)
				printf("\n ");
		}
	}
}

void changeLines(LPIMGDATA img, int h, int n, int value) {
	int i, k;
	for(i=0;i<n;i++){
		for(k=0;k<img->bi.biWidth;k++){
			setPixel(img, k, h+i, value);
		}
	}
}
void convertToWB(LPIMGDATA img, int Th) {
	int i, k;
	for(i=0;i<img->bi.biHeight;i++){
		//	for(i=2;i<3;i++){
		for(k=0;k<img->bi.biWidth;k++){
			//	    for(k=2;k<200;k++){
			setPixelWB(img, k, i, Th);
		}
	}
}

void setColors(LPIMGDATA img) {
	int i, k;
	//	for(i=0;i<img->bi.biHeight;i++){
	for(i=0;i<20;i++){
		for(k=0;k<img->bi.biWidth;k++){
			setPixel8(img, k, i, RED8);
		}
	}
	for(i=20;i<40;i++){
		for(k=0;k<img->bi.biWidth;k++){
			setPixel8(img, k, i, GREEN8);
		}
	}
	for(i=40;i<80;i++){
		for(k=0;k<img->bi.biWidth;k++){
			setPixel8(img, k, i, BLUE8);
		}
	}

}

void saveBMP (LPIMGDATA img)
{
	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	LPBYTE ptr;
	size_t fSize = 0;
	char *szFileName = img->szIMGName;
	char *szPathName = img->szOutPath;

	if (!szFileName) return;  // test only
	strcpy(fileName, szPathName);
	strcat(fileName, szFileName);
	//	strcat(fileName, "tst");

	//	printf("save file %s\n",fileName);

	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMP (\"%s\") failed: %s\n",fileName, strerror(errno));
		return;
	}

	ptr = img->ptr;
	len = img->bf.bfSize;
	fSize = fwrite((char *)ptr, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			fprintf(stderr,"saveBMP (\"%s\") fwrite failed: %s\n",fileName, strerror(errno));
		else
			fprintf(stderr,"saveBMP (\"%s\") fwrite failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		return;
	}

	close_file(f);
}
void saveBMPbuffer (LPBYTE buff, char* name, char* path)
{
	//	char buffTmp[500];
//		LPBITMAPFILEHEADER bf;
//	    LPBITMAPINFOHEADER bi;

	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;

	if (!name) return;

	strcpy(fileName, path);
	strcat(fileName, name);


//	printf("fileName= %s\n",fileName);
//	fflush(stdout);



//	    bf = (LPBITMAPFILEHEADER) buff;
//	    bi = (LPBITMAPINFOHEADER)(buff + sizeof (BITMAPFILEHEADER));
	//    len = bf->bfSize;
	//    len = getLeftBytes((int*)bf->bfSize, 4);
	len = *(LPDWORD)(buff + BFSIZEOFF);

//printf("len= %d\n", len);
//fflush(stdout);

//  printf("bfType= %x, bfSize= %d\n",*(LPWORD)(buff + BFTYPEOFF), len);

//    {   	LPBYTE biPtr = buff + BFOOFBITSOFF+4;
//	printf("save file %s, size= %d\n",fileName, len);
//	fflush(stdout);
//	printf("bi.biSize= %d, bi.biWidth= %d, bi.biHeight= %d \n",*(LPDWORD)(biPtr + BISIZEOFF), *(LPDWORD)(biPtr + BIWIDTHOFF),*(LPDWORD)(biPtr + BIHEIGHTOFF));
//	fflush(stdout);
//    }

	// 	  sprintf(buffTmp,"  len= %d\n",len);
	//	 saveLog(buffTmp, "");
	/*
    {   	LPBYTE biPtr = buff + BFOOFBITSOFF+4;

        printf("bfType= %x, bfSize= %d\n",*(LPWORD)(buff + BFTYPEOFF), len);
    	printf("bf.bfReserved1= %d, bf.bfReserved2= %d, bf.bfOffBits= %d  \n",*(LPWORD)(buff + BFRESERVED1OFF), *(LPWORD)(buff + BFRESERVED2OFF), *(LPDWORD)(buff + BFOOFBITSOFF));

//#define BISIZEOFF 0
//#define BIWIDTHOFF 4
//#define BIHEIGHTOFF 8
//#define BIPLANESOFF 12
//#define BIBITCOUNTOFF 14
//#define BICOMPRESSIONOFF 16
//#define BISIZEIMAGEOFF 20
//#define BIXPELSPERMETEROFF 24
//#define BIYPELSPERMETEROFF 28
//#define BICLRUSEDOFF 32
//#define BICLRIMPORTANTOFF 36

    	printf("bi.biSize= %d, bi.biWidth= %d, bi.biHeight= %d \n",*(LPDWORD)(biPtr + BISIZEOFF), *(LPDWORD)(biPtr + BIWIDTHOFF),*(LPDWORD)(biPtr + BIHEIGHTOFF));
    	printf("bi.biPlanes= %d, bi.biBitCount= %d, bi.biCompression= %d  \n",*(LPWORD)(biPtr + BIPLANESOFF), *(LPWORD)(biPtr + BIBITCOUNTOFF), *(LPDWORD)(biPtr + BICOMPRESSIONOFF));
    	printf("bi.biSizeImage= %d, bi.biXPelsPerMeter= %d, bi.biYPelsPerMeter= %d  \n",*(LPDWORD)(biPtr + BISIZEIMAGEOFF), *(LPDWORD)(biPtr + BIXPELSPERMETEROFF), *(LPDWORD)(biPtr + BIYPELSPERMETEROFF));

    	printf("bi.biClrUsed= %d bi.biClrImportant= %d \n",*(LPDWORD)(biPtr + BICLRUSEDOFF), *(LPDWORD)(biPtr + BICLRIMPORTANTOFF));

//    printf("bfType= %x, bfSize= %d\n",getLeftBytes((int*)bf->bfType, 2), getLeftBytes((int*)bf->bfSize, 4));
//	printf("bf.bfReserved1= %d, bf.bfReserved2= %d, bf.bfOffBits= %d  \n",getLeftBytes((int*)bf->bfReserved1, 2), getLeftBytes((int*)bf->bfReserved1, 2), getLeftBytes((int*)bf->bfOffBits, 4));
//	printf("bi.biSize= %d, bi.biWidth= %d, bi.biHeight= %d \n",getLeftBytes((int*)bi->biSize, 4), getLeftBytes((int*)bi->biWidth, 4),getLeftBytes((int*)bi->biHeight, 4));
//	printf("bi.biPlanes= %d, bi.biBitCount= %d, bi.biCompression= %d  \n",getLeftBytes((int*)bi->biPlanes, 2), getLeftBytes((int*)bi->biBitCount, 2), getLeftBytes((int*)bi->biCompression, 4));
//	printf("bi.biSizeImage= %d, bi.biXPelsPerMeter= %d, bi.biYPelsPerMeter= %d  \n",getLeftBytes((int*)bi->biSizeImage, 4), getLeftBytes((int*)bi->biXPelsPerMeter, 4), getLeftBytes((int*)bi->biYPelsPerMeter, 4));

//	printf("bi.biClrUsed= %d bi.biClrImportant= %d \n",getLeftBytes((int*)bi->biClrUsed, 4), getLeftBytes((int*)bi->biClrImportant, 4));
    }
	 */

	//printf("save file %s, size= %d\n",fileName, len);
	//  sprintf(buffTmp,"  fopen fileName= %s\n", fileName);
	// saveLog(buffTmp, "");

	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMPbuffer (\"%s\") failed: %s\n",fileName, strerror(errno));

		//    	 sprintf(buffTmp,"saveBMPbuffer (\"%s\") failed: %s\n",fileName, strerror(errno));
		//    	 saveLog(buffTmp, "");

		return;
	}
	//	 saveLog("after open", "\n");

	fSize = fwrite((char *)buff, len, 1, f);
	//saveLog("after write", "\n");
	if(ferror(f) || fSize*len != len){
		if(ferror(f)) {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: %s\n",fileName, strerror(errno));
			//       	 sprintf(buffTmp,"saveBMPbuffer (\"%s\") fwrite failed: %s\n",fileName, strerror(errno));
			//       	 saveLog(buffTmp, "");

		}
		else {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
			//         	 sprintf(buffTmp,"saveBMPbuffer (\"%s\") fwrite failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
			//         	 saveLog(buffTmp, "");

		}
		return;
	}
	//    saveLog("close_file", "\n");
	close_file(f);
}
void saveBMPbuffer2 (LPBYTE buff, char* name)
{
	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;

	if (!name) return;

	strcpy(fileName, name);
//	strcat(fileName, name);

	//	 saveLog("\nfileName", fileName);

	len = *(LPDWORD)(buff + BFSIZEOFF);


	f= fopen(fileName, "w");
	if(f == NULL) {
		fprintf(stderr,"saveBMPbuffer (\"%s\") failed: %s\n",fileName, strerror(errno));
		return;
	}

	fSize = fwrite((char *)buff, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f)) {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: %s\n",fileName, strerror(errno));
		}
		else {
			fprintf(stderr,"saveBMPbuffer (\"%s\") fwrite failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		}
		return;
	}
	close_file(f);
}


void testColorArray(LPIMGDATA img){
	int y, x;
	int  w;
	long nY;
	//				    long nX;
	LPBYTE ptrTest;

	//		for(y=0;y<img->bi.biHeight;y++)
	for(y=0;y<5;y++)
	{
		for(x=0;x<244;x++){
			//				for(k=0;k<30;k++){
			{
				w=244;
				y=img->bi.biHeight-1-y;
				nY=y*w;
				//			      	    nX = (long)(x/8);

				ptrTest = img->pixPtr+nY+x;

				// printf("ptr-imgOutp->ptr= %d \n", ptr - imgOutp->ptr);
				*ptrTest = 255;
				//					   testWhite(imgOutp, k, i);
			}
		}
	}
}
int getWBmonoBMPValue(BYTE R, BYTE G, BYTE B, int Th){
	//if(R < Th/2 && G < Th && B < Th/2)

	//if(abs(R-128)<4 && abs(G-128)<4 && abs(B-128)<4)
	//	return 0;
	//if(abs(R-169)<4 && abs(G-169)<4 && abs(B-169)<4)
	//	return 0;
	//if(abs(R-105)<4 && abs(G-105)<4 && abs(B-105)<4)
	//	return 0;
	//if(R>103 && R<173 && G>103 && G<173 && B>103 && B<173){
	//if(abs(R-G)<4 && abs(R-B)<4 && abs(G-B)<4)
	//		return 0;
	//	}

	if(R < Th && G < Th && B < Th)
		return 0;
	else
		return 255;
}


static void setOnePixelWBFromRGBA(LPIMGDATA imgOutp, int xn, int yn, int v){
	int  w;
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptr;
	w=imgOutp->bi.biWidthPad;
	yn=imgOutp->bi.biHeight-1-yn;
	nY=yn*w;
	nX = (long)(xn/8);

	ptr = imgOutp->pixPtr+nY+nX;

	// printf("ptr-imgOutp->ptr= %d \n", ptr - imgOutp->ptr);

	mask=LEFT_BIT;
	mask >>=(xn%8);
	if(v!=0){
		*ptr = *ptr | mask;
	}
	else {
		*ptr = *ptr & (~mask);
	}
}

void setPixelArrayFromBMPTomonoWB(LPIMGDATA imgOutp, LPIMGDATA img, int tH){
	BYTE R, G, B;
	int i, k;
	int v;
	for(i=0;i<img->bi.biHeight;i++)
	{
		for(k=0;k<img->bi.biWidth;k++){
			getRGB(img, k, i, &R, &G, &B);
			if(i < 400)
				v = getWBmonoBMPValue(R, G, B, tH/2);
			else
				v = getWBmonoBMPValue(R, G, B, tH);
			setOnePixelWBFromRGBA(imgOutp, k, i, v);
		}
	}
}

void setPixelArrayFromBMPTomonoWB_tH3(LPIMGDATA imgOutp, LPIMGDATA img, int tH1, int tH2, int tH3){
	BYTE R, G, B;
	int i, k;
	int v = 0, i1, i2;
	i1 = img->bi.biHeight/3;
	i2 = (2*img->bi.biHeight)/3;
	for(i=0;i<img->bi.biHeight;i++)
	{
		for(k=0;k<img->bi.biWidth;k++){
			getRGB(img, k, i, &R, &G, &B);
			if(i < i1)
				v = getWBmonoBMPValue(R, G, B, tH1);
			if(i >= i1 && i< i2)
				v = getWBmonoBMPValue(R, G, B, tH2);
			if(i >= i2)
				v = getWBmonoBMPValue(R, G, B, tH3);
			setOnePixelWBFromRGBA(imgOutp, k, i, v);
		}
	}
}

void convertBMPTo_mono_BMP(LPIMGDATA img, LPIMGDATA imgOutp, int tH1, int tH2, int tH3){
	int hSize = 54;
	int bufforSize;
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage, biWidthPad;

	biWidthPad = ((img->bi.biWidth+31)/32)*4;

	//printf("biWidthPad= %d, img->bi.biWidth= %d \n", biWidthPad, img->bi.biWidth);

	biSizeImage = biWidthPad*img->bi.biHeight;
	bufforSize= biSizeImage+hSize+8;

	// printf("bufforSize= %d \n", bufforSize);

	ptr = yl_calloc( "BMP_B", bufforSize, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertRGBAToBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}
	bf = (LPBITMAPFILEHEADER)(ptr);
	bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));

	bf->bfType[0]= 0x42;
	bf->bfType[1]= 0x4d;
	set4Bytes(bf->bfSize, bufforSize);

	//  printf("bf->bfSize= %d \n",  *(int*)bf->bfSize);
	//   printf("bf->bfType= %x, bf->bfSize= %x \n", *(short*)bf->bfType, *bf->bfSize);
	//   printf("bf->bfType= %d, bf->bfSize= %d \n", *(short*)bf->bfType, *bf->bfSize);


	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	bf->bfReserved2[0] = 0;
	bf->bfReserved2[1] = 0;
	set4Bytes(bf->bfOffBits, hSize+8);

	//printf("bf->bfOffBits= %d \n",  *(int*)bf->bfOffBits);


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

	//		printf("bi->biWidth= %d, img->width= %d \n",  *(int*)bi->biWidth, img->width);


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

	//printf("bi->biWidth= %d, imgOutp.bi.biWidth = %d \n",  *(int*)bi->biWidth, imgOutp.bi.biWidth);

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

	//	      printf("imgOutp.bi.biWidth= %d, imgOutp.bi.biBitCount= %d, imgOutp.bi.biWidthPad= %d \n",  imgOutp.bi.biWidth, imgOutp.bi.biBitCount, imgOutp.bi.biWidthPad);

	//      setPixelArrayFromBMPTomonoWB(imgOutp, img, tH);
	setPixelArrayFromBMPTomonoWB_tH3(imgOutp, img, tH1, tH2, tH3);
	//      saveFile (img, img->szIMGName, ptr, bufforSize);

	//      yl_free(ptr);
}

int getPixelMono(LPBYTE ptr, int xn, int yn, int wPad){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;
	BYTE b;

	//	        yn=imgOutp->bi.biHeight-1-yn;
	nY=yn*wPad;
	nX = (long)(xn/8);
	ptrP = ptr+nY+nX;

	// printf("ptr-imgOutp->ptr= %d \n", ptr - imgOutp->ptr);

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

void setPixelMono(LPBYTE ptr, int xn, int yn, int wPad, int v){
	long nY;
	long nX;
	BYTE mask;
	LPBYTE ptrP;

	//	        yn=imgOutp->bi.biHeight-1-yn;
	nY=yn*wPad;
	nX = (long)(xn/8);
	ptrP = ptr+nY+nX;

	// printf("ptr-imgOutp->ptr= %d \n", ptr - imgOutp->ptr);

	mask=LEFT_BIT;
	mask >>=(xn%8);
	if(v!=0){
		*ptrP = *ptrP | mask;
	}
	else {
		*ptrP = *ptrP & (~mask);
	}
}

void convertDPI_in_BMP(LPIMGDATA img, int dpiX_Otp, int dpiY_Otp){
	int wInp, wOtp, wPadOtp;
	int hInp, hOtp;
	int dpiX_Inp, dpiY_Inp;
	float hX, hY, xf, yf;
	int   x, y, i, j, v;
	LPBYTE ptrInp, ptrOtp;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;


	if(dpiX_Otp == 0 || dpiY_Otp == 0)
		return;
	wInp = img->bi.biWidth;
	hInp = img->bi.biHeight;
	dpiX_Inp = img->bi.biXPelsPerMeter;
	dpiY_Inp = img->bi.biYPelsPerMeter;
//!!!!!!!!!!!!!!!!?????????
	if(wInp > 1700 && hInp > 1700) {
	   hX = (float)dpiX_Inp/(float)dpiX_Otp;
	   hY = (float)dpiY_Inp/(float)dpiY_Otp;
	}
	else{
		   hX = 1;
		   hY = 1;
	}
	// calculate wOtp
	wOtp = 0;
	xf = 0;x = 0;
	while(x<wInp){xf += hX;x = (int)xf;wOtp++;}

	//printf("hX= %e, wOtp= %d \n", hX, wOtp);

	// calculate hOtp
	hOtp = 0;
	yf = 0;y=0;
	while(y<hInp){yf += hY;y = (int)yf;hOtp++;}
	ptrInp = img->pixPtr;
	ptrOtp = img->pixPtr;
	wPadOtp = ((wOtp+31)/32)*4;
	// change image
	yf = 0;
	for(i=0;i<hOtp;i++){
		y = (int)yf;
		xf= 0;
		for(j=0;j<wOtp;j++){
			x = (int)xf;
			v = getPixelMono(ptrInp, x, y, img->bi.biWidthPad);
			setPixelMono(ptrOtp, j, i, wPadOtp, v);
			xf += hX;
		}
		yf += hY;
	}
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
double calculateDarkness(LPIMGDATA img, int* R, int* G, int* B){
	double drk = 0;
	BYTE R1, G1, B1;
	int R2, G2, B2;
	int i, k, cc = 0;
	int lW = img->bi.biWidth/5;
	int lH = img->bi.biHeight/5;
	int startW = (img->bi.biWidth - lW)/2;
	int startH = (img->bi.biHeight - lH)/2;
	R2 = 0;G2 = 0;B2 = 0;
	for(i=startH;i<startH+lH;i++)
	{
		for(k=startW;k<startW+lW;k++){
			getRGB(img, k, i, &R1, &G1, &B1);
			drk += ((R1+G1+B1)/3);
			R2 += R1;G2 += G1;B2 += B1;
			//     printf(" R1= %d, G1= %d, B1= %d \n", R1, G1, B1);
			//     printf(" R2= %d, G2= %d, B2= %d \n", R2, G2, B2);
			cc++;
		}
	}
	if(cc > 0) {
		drk /= cc;
		R2 /= cc;G2 /= cc;B2 /= cc;
	}
	*R = R2;*G = G2;*B = B2;
	return drk;
}
double calculateDarknessMonoTop(LPIMGDATA img){
	double d, mass = 0;
	int i, k;
	int lW = img->bi.biWidth/5;
	int lH = img->bi.biHeight/5;
	int startW = (img->bi.biWidth - lW)/2;
	int startH = lH;

	for(i=startH;i<startH+lH;i++)
	{
		for(k=startW;k<startW+lW;k++){
			mass +=whatIsThisPreP(img->pixPtr,img->bi.biWidthPad,img->bi.biHeight,k,i);
		}
	}
	d = (lW*lH)/32;
	mass /= d;
	return mass;
}
double calculateDarknessMonoCenter(LPIMGDATA img){
	double d, mass = 0;
	int i, k;
	int lW = img->bi.biWidth/5;
	int lH = img->bi.biHeight/5;
	int startW = (img->bi.biWidth - lW)/2;
	int startH = (img->bi.biHeight - lH)/2;

	for(i=startH;i<startH+lH;i++)
	{
		for(k=startW;k<startW+lW;k++){
			mass +=whatIsThisPreP(img->pixPtr,img->bi.biWidthPad,img->bi.biHeight,k,i);
		}
	}
	d = (lW*lH)/32;
	mass /= d;
	return mass;
}
double calculateDarknessMonoBottom(LPIMGDATA img){
	double d, mass = 0;
	int i, k;
	int lW = img->bi.biWidth/5;
	int lH = img->bi.biHeight/5;
	int startW = (img->bi.biWidth - lW)/2;
	int startH = img->bi.biHeight - 2*lH;

	for(i=startH;i<startH+lH;i++)
	{
		for(k=startW;k<startW+lW;k++){
			mass +=whatIsThisPreP(img->pixPtr,img->bi.biWidthPad,img->bi.biHeight,k,i);
		}
	}
	d = (lW*lH)/32;
	mass /= d;
	return mass;
}


int changeLowDark(int tH, double mass){
	int ttH = tH;
	//	if(mass < 0.4) {ttH += ((255 - ttH)/2); if(ttH > 255) ttH = 240; return ttH;}
	if(mass < 1) {ttH += ((255 - ttH)/3); if(ttH > 255) ttH = 240; return ttH;}
	if(mass < 1.5) {ttH += ((255 - ttH)/4); if(ttH > 255) ttH = 240; return ttH;}
	return ttH;
}
void convertInp24BMPtoMonochrome(LPIMGDATA img, LPIMGDATA imgOut){
	//	  char searchName[500];
	char fileName[500];
	char lottoName[500];
	//	  char error[500];
	char num[4];
	LPBYTE buff;
	int i, tH, idx;
	double drk;
	//	  int R, G, B;
	strcpy(lottoName, "tM_");
	for(i=1;i<=50;i++){
		strcpy(fileName, lottoName);
		if(i<10){
			num[0] = '0'+i;
			num[1] = 0;
		}else
		{
			num[0] = '0'+i/10;
			num[1] = '0'+i%10;
			num[2] = 0;
		}
		strcat(fileName, num);
		strcat(fileName, ".bmp");
		strcpy(img->szIMGName, fileName);
		strcpy(imgOut->szIMGName, fileName);
		//	         strcpy(searchName, img->szIMGName);
		//	         strcat(searchName, fileName);
		buff = LoadBMPFile (img);
		if (buff == NULL || strlen(img->error) > 0) {
			if (buff == NULL)
				printf("buff = null;fileName= %s\n", fileName);
			if (strlen(img->error) > 0)
				printf("-------error= %s, fileName= %s\n", img->error, fileName);
			break;
		}
		else {
			printf("\n load successful; fileName= %s \n",fileName);
		}
		//    	     drk = (int)calculateDarkness(img, &R, &G, &B);
		//    	     printf(" ----------------------- fileName= %s, R= %d, G= %d, B= %d \n",fileName, R, G, B);
		//    	     printf(" ----------------------- fileName= %s, drk= %d \n",fileName, drk);
		idx = 0;
		tH = 80;
		while(idx < 5){
			if(imgOut->ptr != NULL)
				yl_free(imgOut->ptr);
			imgOut->ptr = NULL;
			convertBMPTo_mono_BMP(img, imgOut, tH, tH, tH);
			drk = calculateDarknessMonoCenter(imgOut);
			printf(" ----------------------- fileName= %s, mass= %f \n",fileName, drk);
			if(drk < 1.5 )
				tH = changeLowDark(tH, drk);
			else
				break;
			//               if(drk > 2.5 ) {
			//            	   yl_free(imgOut.ptr);
			//            	   tH = changeLowDark(tH, drk);
			//            	   convertBMPTo_monoB_BMP(img, imgOut, tH);
			//            	   break;
			//               }

			idx++;
		}

		convertDPI_in_BMP(imgOut, 300, 300);
		saveBMP (imgOut);
	}
}


void convert24BMPtoMonochromeIter(LPIMGDATA img, LPIMGDATA imgOut){
	int tH1, tH2, tH3, idx;
	double drk1, drk2, drk3;
	LPBITMAPINFOHEADER bi;

	idx = 0;
	tH2 = 80;
	while(idx < 5){
		if(imgOut->ptr != NULL)
			yl_free(imgOut->ptr);
		imgOut->ptr = NULL;
		convertBMPTo_mono_BMP(img, imgOut, tH2, tH2, tH2);
		drk1 = calculateDarknessMonoTop(imgOut);
		drk2 = calculateDarknessMonoCenter(imgOut);
		drk3 = calculateDarknessMonoBottom(imgOut);
		//   printf(" ----------------------- fileName= %s, mass= %f, tH= %d \n",img->szIMGName, drk1, tH);
		//   printf(" ----------------------- fileName= %s, massTop= %f, masCenter= %f, massBottom= %f, tH= %d \n",img->szIMGName, drk1, drk2, drk3, tH2);

		if(drk2 < 1.5 )
			tH2 = changeLowDark(tH2, drk2);
		else {
			tH1 = tH2;tH3 = tH2;
			if(drk1/drk2 > 2.5 ) tH1 = tH2 - tH2/3 - 10;
			if(drk3/drk2 > 2.5 ) tH3 = tH2 - tH2/4;
			if(tH1 != tH2 || tH3 != tH2) {
				//  		   printf(" ----------------------- fileName= %s, tH1= %d, tH2= %d, tH3= %d \n",img->szIMGName, tH1, tH2, tH3);
				convertBMPTo_mono_BMP(img, imgOut, tH1, tH2, tH3);

			}
			break;
		}
		idx++;
	}
	//    	      printBMPData (imgOut);
	if(imgOut->bi.biWidth > 1400)
		convertDPI_in_BMP(imgOut, 300, 300);
	else{
		imgOut->bi.biXPelsPerMeter=300;
		imgOut->bi.biYPelsPerMeter=300;
		bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biXPelsPerMeter, imgOut->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, imgOut->bi.biYPelsPerMeter);

	}
}

void reloadByteLine(LPBYTE ptrOut, int xOut, int yOut, int wHeightOut, LPBYTE ptrInp, int xInp, int yInp, int wWidth,  int wHeightInp){
	long nYInp, nYOut;

	nYInp = (wHeightInp - yInp)*wWidth;
	nYOut = (wHeightOut - yOut)*wWidth;
	*(ptrOut + nYOut +xOut) = *(ptrInp + nYInp +xInp);
}
void setLinePixels(LPBYTE ptrOut, int y1, int y2, int wHeightOut, LPBYTE ptrInp, int wWidth, int wHeightInp){
	int x, y;
	int w = (((wWidth + 31) /32) * 4);
	for(y=0;y<wHeightInp;y++){
		for(x=0;x<w;x++){
			if(y>=y1 && y<=y2)
				reloadByteLine(ptrOut, x, y-y1-1, wHeightOut, ptrInp, x, y, w, wHeightInp);
		}
	}
}
void reloadByteLineX(LPBYTE ptrOut, int xOut, int yOut, int wHeightOut, LPBYTE ptrInp, int xInp, int yInp, int wWidth,  int wHeightInp){
	long nYInp, nYOut;

	nYInp = (wHeightInp - yInp)*wWidth;
	nYOut = (wHeightOut - yOut)*wWidth;
	*(ptrOut + nYOut +xOut) = 0;
}

void setLinePixelsX(LPBYTE ptrOut, int y1, int y2, int wHeightOut, int x1, int x2, LPBYTE ptrInp, int wWidth, int wHeightInp){
	int x, y;
	int w = (((wWidth + 31) /32) * 4);
	for(y=0;y<wHeightInp;y++){
		for(x=0;x<w;x++){
			if(y>=y1 && y<=y2){
				if(x ==x1 || x ==x2)
				    reloadByteLineX(ptrOut, x, y-y1-1, wHeightOut,ptrInp, x, y, w, wHeightInp);
				else
			        reloadByteLine(ptrOut, x, y-y1-1, wHeightOut, ptrInp, x, y, w, wHeightInp);
			}
		}
	}
}

int writeMonoBMPHeader( FILE *file, int width, int height ){
	BITMAPFILEHEADER fileHeader;
	LPBITMAPFILEHEADER bf = &fileHeader;

	BITMAPINFOHEADER infoHeader;
	LPBITMAPINFOHEADER bi = &infoHeader;

//	LPRGBQUAD          qd;
//	int j, indFr = 0, len;
//	LPBYTE ptrLine;
	int len;

//	LOGD("wm1");

	int wHeightL = height;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;

	int dpiX = 300;
	int dpiY = 300;

//	LOGD("wm2");

	len = offBits + (((width + 31) /32) * 4) * height;

	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;

	int bfSize = (((width + 31) /32) * 4) * wHeightL;
	set4Bytes(bf->bfSize, bfSize );
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;


//	LOGD("off bits: %d", offBits);

	set4Bytes(bf->bfOffBits, offBits);

//	LOGD("wm3");

	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, width);
	set4Bytes(bi->biHeight, wHeightL);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 1;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);

//	LOGD("wm4");

	set4Bytes(bi->biSizeImage, bfSize);
	set4Bytes(bi->biXPelsPerMeter, dpiX);
	set4Bytes(bi->biYPelsPerMeter, dpiY);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);
	//qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	//*(LPDWORD)qd       = 0x00000000;
	//*(LPDWORD)(qd + 1) = 0x00ffffff;
//	LOGD("wm5");


//	LOGD("wm6");
	int written = 0;

	written += fwrite( bf, 1, sizeof(BITMAPFILEHEADER), file );

//	LOGD("wm7");

	written += fwrite( bi, 1, sizeof(BITMAPINFOHEADER), file );

//	LOGD("wm8 returning %d", written);

	return written;
}

void saveLinesTest(LPBYTE testFieldBufferBMP, STRUCTWB *lpWorkBuffer, char* imgName){
	char name[200];
	char num[10];
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	int j,  indFr = 0, len;
	LPBYTE ptrLine;

	LPBYTE biPtr = testFieldBufferBMP + BFOOFBITSOFF+4;
	int wWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	int wHeightL;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	int dpiX = *(LPDWORD)(biPtr + BIXPELSPERMETEROFF);
	int dpiY = *(LPDWORD)(biPtr + BIYPELSPERMETEROFF);


	len = offBits + (((wWidth + 31) /32) * 4) * wHeight;

	ptrLine = yl_calloc( "BMP_C", len, sizeof(BYTE));
	if(ptrLine == NULL) {
		printf("yl_calloc failed: save lineTest");
		return;
	}
	//   printf("---------------------wWidth= %d, wHeight= % d \n",wWidth, wHeight);
for(indFr=0;indFr<(lpWorkBuffer -> frNumber);indFr++)  {
	for(j=0;j<(lpWorkBuffer->infFrame[indFr]).nLines;j++) {
		wHeightL = (lpWorkBuffer->line[indFr]).y2[j] - (lpWorkBuffer->line[indFr]).y1[j]+1;
//		wWidthL = (lpWorkBuffer->infFrame)[indFr].x2 - (lpWorkBuffer->infFrame)[indFr].x1 +1;

		printf("---------------------indFr= %d, line= %d, x1= %d, x2= %d \n",indFr, j, (lpWorkBuffer->infFrame)[indFr].x1, (lpWorkBuffer->infFrame)[indFr].x2);

		bf = (LPBITMAPFILEHEADER) ptrLine;
		bf->bfType[0] = 0x42;
		bf->bfType[1] = 0x4d;
		set4Bytes(bf->bfSize, offBits + (((wWidth + 31) /32) * 4) * wHeightL);
//		set4Bytes(bf->bfSize, offBits + (((wWidthL + 31) /32) * 4) * wHeightL);
		bf->bfReserved1[0] = 0;
		bf->bfReserved1[1] = 0;
		set4Bytes(bf->bfOffBits, offBits);

		//	    printf("---------------------j= %d, wWidth= %d, wHeight= %d, dpiX= %d, dpiY= %d \n", j, wWidth, wHeightL, dpiX, dpiY);

		bi = (LPBITMAPINFOHEADER)(ptrLine + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
		set4Bytes(bi->biWidth, wWidth);
//		set4Bytes(bi->biWidth, wWidthL);
		set4Bytes(bi->biHeight, wHeightL);
		bi->biPlanes[0] = 1;
		bi->biPlanes[1] = 0;
		bi->biBitCount[0]= 1;
		bi->biBitCount[1]= 0;
		set4Bytes(bi->biCompression, BI_RGB);
		set4Bytes(bi->biSizeImage, 0);
		set4Bytes(bi->biXPelsPerMeter, dpiX);
		set4Bytes(bi->biYPelsPerMeter, dpiY);
		set4Bytes(bi->biClrUsed, 0);
		set4Bytes(bi->biClrImportant, 0);
		qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
		*(LPDWORD)qd       = 0x00000000;
		*(LPDWORD)(qd + 1) = 0x00ffffff;
//		{char buffLog[500];
//          sprintf(buffLog, "j= %d, lpWorkBuffer->line[indFr]).y1[j]= %d\n", j, (lpWorkBuffer->line[indFr]).y1[j]);

//  saveLog("setLinePixels=  ", buffLog);

//		}
		setLinePixels(ptrLine+offBits, (lpWorkBuffer->line[indFr]).y1[j], (lpWorkBuffer->line[indFr]).y2[j], wHeightL, testFieldBufferBMP+offBits,
				wWidth,  wHeight);
//		setLinePixelsX(ptrLine+offBits, (lpWorkBuffer->line[indFr]).y1[j], (lpWorkBuffer->line[indFr]).y2[j], wHeightL,
//				(lpWorkBuffer->infFrame)[indFr].x1, (lpWorkBuffer->infFrame)[indFr].x2,
//				testFieldBufferBMP+offBits,
//				wWidth,  wHeight);

		name[0] = 0;
		if(strlen(imgName) > 0){
			strcpy(name, imgName);
			//	    	name[strlen(name)-4] = 0;
			strcat(name, "_");
		}
		strcat(name, "line_");
		if(j<10){
			num[0] = '0'+j;
			num[1] = 0;
		}else{
			num[0] = '0'+j/10;
			num[1] = '0'+j%10;
			num[2] = 0;
		}
		strcat(name, num);
		strcat(name, "_Fr_");
		if(indFr<10){
			num[0] = '0'+indFr;
			num[1] = 0;
		}else{
			num[0] = '0'+indFr/10;
			num[1] = '0'+indFr%10;
			num[2] = 0;
		}

		strcat(name, num);
		strcat(name, ".bmp");
//  saveLog(name, "\n");
		saveBMPbuffer (ptrLine, name, "c:\\Users\\Wicek\\OCR\\output\\");
	}
} // indFr
	if(ptrLine)
		yl_free(ptrLine);
}
void extractLineAndSave(LPBYTE testFieldBufferBMP, int x1, int x2, int y1, int y2, char *name){
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

	LPBYTE biPtr = testFieldBufferBMP + BFOOFBITSOFF+4;
	int wWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);

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

    angle = 0;

	err =
			extractField(testFieldBufferBMP+offBits, wWidth, wHeight,
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
// printf("------------ err= %d\n", err);
	if(err < 0) return;

//	printf("---------------x1 = %d, y1= %d\n",x1, y1);
//	printf("---------------x2 = %d, y1= %d\n",x2, y1);
//	printf("---------------x1 = %d, y1= %d\n",x1, y2);
//	printf("---------------width = %d, height= %d\n",width, height);
//	printf("---------------testFieldBufferBMP+offBits = %lu, wWidth= %d, height=%d\n",(unsigned long)(testFieldBufferBMP+offBits), wWidth, wHeight);
//	printf("---------------angleFlag= %d, angle= %f\n",angleFlag, angle);


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

//   	printf("2---------------bfSize= %d\n",offBits + (((width + 31) /32) * 4) * height);
//   	printf("2---------------offBits= %d\n",offBits);
//   	printf("2---------------biWidthPad= %d, biSizeImage=%d\n",biWidthPad, biSizeImage);

//   	printf("2---------------dpiX= %d\n",*(LPDWORD)(biPtr + BIXPELSPERMETEROFF));


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
	set4Bytes(bi->biXPelsPerMeter, *(LPDWORD)(biPtr + BIXPELSPERMETEROFF));
	set4Bytes(bi->biYPelsPerMeter, *(LPDWORD)(biPtr + BIYPELSPERMETEROFF));
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	// generate pair of RGBQUADs (black and white)
	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;
	{
//printf("------------ name= %s\n", name);


 	    saveBMPbuffer (hpFieldH.ptr, name, "c:\\Users\\Wicek\\OCR\\output\\");

	}
	yl_free(hpFieldH.ptr);

}

void saveLinesTest2(LPBYTE testFieldBufferBMP, STRUCTWB *lpWorkBuffer, char* imgName){
	char name[500];
	char num[10];
	int line,  indFr = 0;

for(indFr=0;indFr<(lpWorkBuffer -> frNumber);indFr++)  {
	for(line=0;line<(lpWorkBuffer->infFrame[indFr]).nLines;line++) {
		name[0] = 0;
		if(strlen(imgName) > 0){
			strcpy(name, imgName);
			//	    	name[strlen(name)-4] = 0;
			strcat(name, "_");
		}
		strcat(name, "line_");
		if(line<10){
			num[0] = '0'+line;num[1] = 0;
		}else{
			num[0] = '0'+line/10;num[1] = '0'+line%10;num[2] = 0;
		}
		strcat(name, num);
		strcat(name, "_Fr_");
		if(indFr<10){
			num[0] = '0'+indFr;num[1] = 0;
		}else{
			num[0] = '0'+indFr/10;num[1] = '0'+indFr%10;num[2] = 0;
		}
		strcat(name, num);
		strcat(name, ".bmp");
//  saveLog(name, "\n");
//		if(line == 2)
		extractLineAndSave(testFieldBufferBMP, (lpWorkBuffer->infFrame)[indFr].x1, (lpWorkBuffer->infFrame)[indFr].x2,
				           (lpWorkBuffer->line[indFr]).y1[line], (lpWorkBuffer->line[indFr]).y2[line], name);

	}
} // indFr

}
void extractLineAndSave3(STRUCTWB *lpWorkBuffer, int x1, int x2, int y1, int y2, char *name){
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

//	LPBYTE biPtr = testFieldBufferBMP + BFOOFBITSOFF+4;
//    printf("------------ lpWorkBuffer ->Width= %d, lpWorkBuffer -> widthImg= %d\n", lpWorkBuffer ->Width, lpWorkBuffer -> widthImg);

	int wWidth = lpWorkBuffer -> Width;
	int wHeight = lpWorkBuffer -> hightImg;

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

    angle = 0;

	err =
			extractField(lpWorkBuffer->lpRBuffer, wWidth, wHeight,
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
// printf("------------ err= %d\n", err);
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

	set4Bytes(bi->biXPelsPerMeter, lpWorkBuffer ->Dpi.x);
	set4Bytes(bi->biYPelsPerMeter, lpWorkBuffer ->Dpi.y);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	// generate pair of RGBQUADs (black and white)
	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;
	{
//printf("------------ name= %s\n", name);


 	    saveBMPbuffer (hpFieldH.ptr, name, "c:\\Users\\Wicek\\OCR\\output\\");

	}
	yl_free(hpFieldH.ptr);

}

void saveLinesTest3(STRUCTWB *lpWorkBuffer, int indFr, char *imgName){
int line;
char name[500];
char num[10];

	for(line=0;line<(lpWorkBuffer->infFrame[indFr]).nLines;line++) {
		name[0] = 0;
		strcpy(name, imgName);
		strcat(name, "_");

		strcat(name, "line_");
		if(line<10){
			num[0] = '0'+line;num[1] = 0;
		}else{
			num[0] = '0'+line/10;num[1] = '0'+line%10;num[2] = 0;
		}
		strcat(name, num);
		strcat(name, "_Fr_");
		if(indFr<10){
			num[0] = '0'+indFr;num[1] = 0;
		}else{
			num[0] = '0'+indFr/10;num[1] = '0'+indFr%10;num[2] = 0;
		}
		strcat(name, num);
		strcat(name, ".bmp");
//  saveLog(name, "\n");
		extractLineAndSave3(lpWorkBuffer, (lpWorkBuffer->infFrame)[indFr].x1, (lpWorkBuffer->infFrame)[indFr].x2,
				           (lpWorkBuffer->line[indFr]).y1[line], (lpWorkBuffer->line[indFr]).y2[line], name);

	}

}

void saveFramesTest(LPBYTE testFieldBufferBMP, STRUCTWB *lpWorkBuffer, char* imgName){
	char name[500];
	char num[10];
	int  indFr;
	LPBYTE biPtr = testFieldBufferBMP + BFOOFBITSOFF+4;
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);

for(indFr=0;indFr<(lpWorkBuffer -> frNumber);indFr++)  {
//	for(line=0;line<(lpWorkBuffer->infFrame[indFr]).nLines;line++) {
		name[0] = 0;
		if(strlen(imgName) > 0){
			strcpy(name, imgName);
			//	    	name[strlen(name)-4] = 0;
			strcat(name, "_");
		}
		strcat(name, "_Frame_");
		if(indFr<10){
			num[0] = '0'+indFr;num[1] = 0;
		}else{
			num[0] = '0'+indFr/10;num[1] = '0'+indFr%10;num[2] = 0;
		}
		strcat(name, num);
		strcat(name, ".bmp");
//  saveLog(name, "\n");
		extractLineAndSave(testFieldBufferBMP, (lpWorkBuffer->infFrame)[indFr].x1, (lpWorkBuffer->infFrame)[indFr].x2,
				           0, wHeight-1, name);

  } // indFr

}


void saveField(LPIMGDATA img, LPTICKETDATA ticketData){

	char name[100];
	strcpy(name, img->szIMGName);
	name[strlen(name)-4] = 0;
	strcat(name, "_");
	strcat(name, ticketData->fieldAddress->name);
	strcat(name, ".BMP");
	saveBMPbuffer (ticketData->fieldBMPPtr, name, img->szOutPath);


}
void copyAddPixels(LPBYTE ptrOut, LPIMGDATA img, int dd){
	int i, k, idxInp, idxOut;
	int biWidthPad = ((img->bi.biWidth+31)/32)*4;
	for(i=0;i<img->bi.biHeight;i++){
		for(k=0;k<biWidthPad;k++){
			idxInp = (img->bi.biHeight-1-i)*biWidthPad+k;
			idxOut = (img->bi.biHeight+dd-1-i)*biWidthPad+k;
			*(ptrOut+idxOut) = *(img->pixPtr+idxInp);
		}
	}
	for(i=0;i<dd;i++){
		for(k=0;k<biWidthPad;k++){
			idxOut = (dd-1-i)*biWidthPad+k;
			*(ptrOut+idxOut) = 0;
		}
	}


}
void addBottomPart(LPIMGDATA img, int dd){
	int hSize = 54;
	int bufforSize;
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage, biWidthPad;
	int offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;

	biWidthPad = ((img->bi.biWidth+31)/32)*4;

	//printf("biWidthPad= %d, img->bi.biWidth= %d \n", biWidthPad, img->bi.biWidth);

	biSizeImage = biWidthPad*img->bi.biHeight+biWidthPad*dd;
	bufforSize= biSizeImage+hSize+8;

	// printf("bufforSize= %d \n", bufforSize);

	ptr = yl_calloc( "BMP_D", bufforSize, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertRGBAToBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}
	bf = (LPBITMAPFILEHEADER)(ptr);
	bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));

	bf->bfType[0]= 0x42;
	bf->bfType[1]= 0x4d;
	set4Bytes(bf->bfSize, bufforSize);

	//  printf("bf->bfSize= %d \n",  *(int*)bf->bfSize);
	//   printf("bf->bfType= %x, bf->bfSize= %x \n", *(short*)bf->bfType, *bf->bfSize);
	//   printf("bf->bfType= %d, bf->bfSize= %d \n", *(short*)bf->bfType, *bf->bfSize);


	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	bf->bfReserved2[0] = 0;
	bf->bfReserved2[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	//printf("bf->bfOffBits= %d \n",  *(int*)bf->bfOffBits);


	set4Bytes(bi->biSize, hSize-14);
	set4Bytes(bi->biWidth, img->width);
	set4Bytes(bi->biHeight, img->height);
	*bi->biPlanes = (short)1;
	*bi->biBitCount= (short)1;

	//		printf("bi->biWidth= %d, img->width= %d \n",  *(int*)bi->biWidth, img->width);


	set4Bytes(bi->biCompression, 0);
	set4Bytes(bi->biSizeImage, biSizeImage);
	set4Bytes(bi->biXPelsPerMeter, 512);
	set4Bytes(bi->biYPelsPerMeter, 512);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);


	//printf("bi->biWidth= %d, imgOutp.bi.biWidth = %d \n",  *(int*)bi->biWidth, imgOutp.bi.biWidth);

	*(ptr+hSize) = 0;
	*(ptr+hSize+1) = 0;
	*(ptr+hSize+2) = 0;
	*(ptr+hSize+3) = 0;
	*(ptr+hSize+4) = 255;
	*(ptr+hSize+5) = 255;
	*(ptr+hSize+6) = 255;
	*(ptr+hSize+7) = 0;

	//	      printf("imgOutp.bi.biWidth= %d, imgOutp.bi.biBitCount= %d, imgOutp.bi.biWidthPad= %d \n",  imgOutp.bi.biWidth, imgOutp.bi.biBitCount, imgOutp.bi.biWidthPad);

	copyAddPixels((LPBYTE)(ptr+offBits), img, dd);
	saveFile (img, img->szIMGName, ptr, bufforSize);

	//      yl_free(ptr);

}





/*

Conversion to rgb565 can be done using this technique, taken from here:


R5 = ( R8 * 249 + 1014 ) >> 11;
G6 = ( G8 * 253 +  505 ) >> 10;
B5 = ( B8 * 249 + 1014 ) >> 11;


int nPos16 = 0;
int Size = Width*Height*3;
for ( int nPos24 = 0 ; nPos24 < Size ; nPos24+=3 ,nPos16+=2)
{
  BYTE Red24   = lpBits24[nPos24+2]; // 8-bit red
  BYTE Green24 = lpBits24[nPos24+1]; // 8-bit green
  BYTE Blue24  = lpBits24[nPos24+0]; // 8-bit blue

  BYTE Red16   = Red24   >> 3;  // 5-bit red
  BYTE Green16 = Green24 >> 2;  // 6-bit green
  BYTE Blue16  = Blue24  >> 3;  // 5-bit blue

  unsigned short RGB2Bytes = Blue16 + (Green16<<5) + (Red16<<(5+6));

  lpBits16[nPos16]  = LOBYTE(RGB2Bytes);
  lpBits16[nPos16+1]= HIBYTE(RGB2Bytes);
}
}

*/
char buffLog[500];
#define eRGB565_R  0xf800 ///< Mask for blue	(1111100000000000)
#define eRGB565_G  0x07e0 ///< Mask for green	(0000011111100000)
#define eRGB565_B  0x001f ///< Mask for red	(0000000000011111)
void getColorRGB565(LPIMGDATA img, int x, int y, LPBYTE R, LPBYTE G, LPBYTE B){
	long nY;
	long nX;
    WORD *ptr;
	WORD wr;

	   nY=y*img->width;
	   nX = x;
	   ptr = (WORD*)img->pixPtr+nY+nX;
	   wr =  *ptr;

	   *B = ((wr & eRGB565_R ) >> 11);
	   *G = ((wr & eRGB565_G) ) >> 5;
	   *R = (wr & eRGB565_B );
}

void setOnePixelRGB_565(LPIMGDATA img, int x, int y, BYTE R, BYTE G, BYTE B) {

  long nY;
  long nX;
  WORD *ptr;
  WORD wrR, wrG, wrB;

   nY=y*img->width;
   nX = x;
   ptr = (WORD*)img->pixPtr+nY+nX;
   wrB = (WORD)B; wrB <<= 11;
   wrG = (WORD)G; wrG <<= 5;
   wrR = (WORD)R;
   *ptr =  ((wrB & eRGB565_R ) |  (wrG & eRGB565_G ) | (wrR & eRGB565_B ));
//   *ptr =  ((wrB & eRGB565_B ) | (wrR &  eRGB565_R));

//   BYTE Red16   = R   >> 3;  // 5-bit red
//   BYTE Green16 = G >> 2;  // 6-bit green
//   BYTE Blue16  = B  >> 3;  // 5-bit blue

//   *ptr = Blue16 + (Green16<<5) + (Red16<<(5+6));
//   *ptr = Red16 + (Green16<<5) + (Blue16<<(5+6));


}

BYTE scaleOneColorToRGB565(BYTE color, BYTE scale) {
	return ((color*scale)/255);
}

void scaleToRGB565(BYTE *R, BYTE *G, BYTE *B){
	*R = scaleOneColorToRGB565(*R, 31);
	*B = scaleOneColorToRGB565(*B, 31);
	*G = scaleOneColorToRGB565(*G, 63);
}
void setPixelArrayFromBMPToRGB565(LPIMGDATA imgOutp, LPIMGDATA img){
	BYTE R, G, B;
	int y, x;
	for(y=0;y<img->bi.biHeight;y++)	{
		for(x=0;x<img->bi.biWidth;x++) {
			getColor24(img, x, y, &R, &G, &B);

//if(y > 300 && y < 310 && x > 100 && x < 400) {
//   sprintf(buffLog," BMP; y= %d, x= %d, R= %d, G= %d, B= %d\n",y,x, R,G,B);
//   saveLog(buffLog, "");
//}
            scaleToRGB565(&R, &G, &B);
			setOnePixelRGB_565(imgOutp, x, y, R, G, B);

//	getColorRGB565(imgOutp, x, y, &R, &G, &B);

//if(y > 300 && y < 310 && x > 100 && x < 400) {

//	   sprintf(buffLog," RGB; y= %d, x= %d, R= %d, G= %d, B= %d\n",y,x, R,G,B);
//	   saveLog(buffLog, "");
//}


	}
  }
}
BYTE scaleOneColorToBMP(BYTE color, BYTE scale) {
	return ((color*255)/scale);
}
void scaleToBMP(BYTE *R, BYTE *G, BYTE *B){
	*R = scaleOneColorToBMP(*R, 31);
	*B = scaleOneColorToBMP(*B, 31);
	*G = scaleOneColorToBMP(*G, 63);
}
void setOnePixelBMP24(LPIMGDATA img, int x, int y, BYTE R, BYTE G, BYTE B){
	int  w;
	long nY;
	long nX;
	int sh = 3;
	LPBYTE ptr;

	w=img->bi.biWidthPad;
	y=img->bi.biHeight-1-y;
	nY=y*w;
	nX = (long)x*sh;
	ptr=img->pixPtr+nY+nX;
	*ptr = B;*(ptr+1) = G;*(ptr+2) = R;
}

void setPixelArrayFromRGB565ToBMP(LPIMGDATA imgOutp, LPIMGDATA img){
	BYTE R, G, B;
	int y, x;
	for(y=0;y<img->bi.biHeight;y++)	{
		for(x=0;x<img->bi.biWidth;x++) {
			getColorRGB565(img, x, y, &R, &G, &B);

			scaleToBMP(&R, &G, &B);
//				if(y > 300 && y < 330 && x > 100 && x < 400) {
//					R = 255;G=255;B=255;
//				}
//	if(y > 300 && y < 310 && x > 100 && x < 400) {
//	   sprintf(buffLog," y= %d, x= %d, R= %d, G= %d, B= %d\n",y,x, R,G,B);
//	   saveLog(buffLog, "");
//	}

			setOnePixelBMP24(imgOutp, x, y, R, G, B);
		}
	}
}

void copyBMPHeaderBinary(LPBYTE ptrOut, LPBYTE ptrInp, int size){
	int i;
	for(i= 0;i<size;i++){
		ptrOut[i] = ptrInp[i];
	}
}

void convertBMPTo_RGB565(LPIMGDATA img, LPIMGDATA imgOutp){
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage;

//	int biWidthInp, biWidthPadInp, biHeightInp;
	int biWidthInp, biHeightInp;
	int hSize;
//	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthInp = img->bi.biWidth;
//	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	bf = (LPBITMAPFILEHEADER)(img->ptr);
	bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));

//printf("bf.bfSize= %lu, bf.bfOffBits= %lu \n",img->bf.bfSize, img->bf.bfOffBits);
//fflush(stdout);

    hSize = getLeftBytes((int*)bf->bfOffBits, 4);
//	printf("------------------, hSize= %d \n",hSize);
//	fflush(stdout);


	biSizeImage = biWidthInp*img->bi.biHeight*2 + hSize;

// printf("1-----BMP img->bi.biSizeImage= %lu, img->bi.biSize= %lu \n", img->bi.biSizeImage, img->bi.biSize);

	// printf("bufforSize= %d \n", bufforSize);

	ptr = yl_calloc( "BMP_B", biSizeImage, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertBMPTo_RGB565 ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, biSizeImage, strerror(errno));
		return;
	}

	copyBMPHeaderBinary(ptr, img->ptr, hSize);
	copyHeadersBMP(ptr, imgOutp);

//	printf("copyBMPHeaderBinary;, hSize= %d \n",hSize);
//	fflush(stdout);
	imgOutp->ptr = ptr;
	imgOutp->pixPtr = ptr + hSize;
	imgOutp->width = img->bi.biWidth;
	imgOutp->height = img->bi.biHeight;

//	   sprintf(buffLog," setPixelArrayFromBMPToRGB565--------------------\n");
//	   saveLog(buffLog, "");

	setPixelArrayFromBMPToRGB565(imgOutp, img);

//    printf("----- imgOutp->bi.biHeight= %d, biWidthInp= %d \n",  imgOutp->bi.biHeight, biWidthInp);

//	printf("setPixelArrayFromBMPToRGB_565 \n");
//	fflush(stdout);

}


void convertRGB565To_BMP(LPIMGDATA img, LPIMGDATA imgOutp){
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage;

	int biWidthInp, biWidthPadInp, biHeightInp;
	int hSize;
//	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;


	bf = (LPBITMAPFILEHEADER)(img->ptr);
	bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
    hSize = getLeftBytes((int*)bf->bfOffBits, 4);

    printf("-----BMP biWidthPadInp= %d, img->bi.biHeight= %d, biWidthInp= %d, img->bi.biBitCount= %d \n", biWidthPadInp, img->bi.biHeight, biWidthInp, img->bi.biBitCount);

	biSizeImage = biWidthPadInp*img->bi.biHeight + hSize;

    printf("-----BMP biSizeImage= %d \n", biSizeImage);

//printf("bf.bfSize= %lu, bf.bfOffBits= %lu \n",img->bf.bfSize, img->bf.bfOffBits);
//fflush(stdout);

	ptr = yl_calloc( "BMP_B", biSizeImage, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertRGB565To_BMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, biSizeImage, strerror(errno));
		return;
	}

	copyBMPHeaderBinary(ptr, img->ptr, hSize);

	imgOutp->ptr = ptr;
	imgOutp->pixPtr = ptr + hSize;
	imgOutp->width = img->bi.biWidth;
	imgOutp->height = img->bi.biHeight;
//	setPixelArrayFromBMPToRGB_565(imgOutp, img);
//	   sprintf(buffLog," setPixelArrayFromRGB565ToBMP--------------------\n");
//	   saveLog(buffLog, "");

	setPixelArrayFromRGB565ToBMP(imgOutp, img);
}

void setOnePixelRGBA(LPIMGDATA img, int x, int y, BYTE R, BYTE G, BYTE B) {

  int  w;
  long nY;
  long nX;
  LPBYTE ptr;
  int sh= 4;

//  DWORD *ptr;
//       nY=y*img->width;
//       nX = x;
//       ptr = (DWORD*)img->pixPtr+nY+nX;


       w=img->width*sh;
       nY=y*w;
       nX = (long)x*sh;
       ptr=img->pixPtr+nY+nX;

      *ptr= B;
      *(ptr+1)= G;
      *(ptr+2)= R;
      *(ptr+3)= 255;

}

void setPixelArrayFromBMPToRGBA(LPIMGDATA imgOutp, LPIMGDATA img){
	BYTE R, G, B;
	int y, x;
	for(y=0;y<img->bi.biHeight;y++)	{
		for(x=0;x<img->bi.biWidth;x++) {
			getColor24(img, x, y, &R, &G, &B);

			setOnePixelRGBA(imgOutp, x, y, R, G, B);
	}
  }
}

void convertBMPTo_RGBA(LPIMGDATA img, LPIMGDATA imgOutp){
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage;

//	int biWidthInp, biWidthPadInp, biHeightInp;
	int biWidthInp, biHeightInp;
	int hSize;
//	int biWidthOut, biWidthPadOut, biHeightOut;

	biWidthInp = img->bi.biWidth;
//	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;

	bf = (LPBITMAPFILEHEADER)(img->ptr);
	bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));

//printf("bf.bfSize= %lu, bf.bfOffBits= %lu \n",img->bf.bfSize, img->bf.bfOffBits);
//fflush(stdout);

    hSize = getLeftBytes((int*)bf->bfOffBits, 4);
//	printf("------------------, hSize= %d \n",hSize);
//	fflush(stdout);


	biSizeImage = biWidthInp*img->bi.biHeight*4 + hSize;

// printf("1-----BMP img->bi.biSizeImage= %lu, img->bi.biSize= %lu \n", img->bi.biSizeImage, img->bi.biSize);

	// printf("bufforSize= %d \n", bufforSize);

	ptr = yl_calloc( "BMP_B", biSizeImage, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertBMPTo_RGB565 ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, biSizeImage, strerror(errno));
		return;
	}

	copyBMPHeaderBinary(ptr, img->ptr, hSize);
	copyHeadersBMP(ptr, imgOutp);

//	printf("copyBMPHeaderBinary;, hSize= %d \n",hSize);
//	fflush(stdout);
	imgOutp->ptr = ptr;
	imgOutp->pixPtr = ptr + hSize;
	imgOutp->width = img->bi.biWidth;
	imgOutp->height = img->bi.biHeight;

//	   sprintf(buffLog," setPixelArrayFromBMPToRGB565--------------------\n");
//	   saveLog(buffLog, "");

	setPixelArrayFromBMPToRGBA(imgOutp, img);

//    printf("----- imgOutp->bi.biHeight= %d, biWidthInp= %d \n",  imgOutp->bi.biHeight, biWidthInp);

//	printf("setPixelArrayFromBMPToRGB_565 \n");
//	fflush(stdout);

}
void getColorRGBA (LPIMGDATA img, int x,int y, LPBYTE R, LPBYTE G, LPBYTE B) {
	        int  w;
	        long nY;
	        long nX;
	        int sh= 4;
	        LPBYTE ptr;

//	        DWORD *ptr;
//	             nY=y*img->width;
//	             nX = x;
//	             ptr = (DWORD*)img->pixPtr+nY+nX;

	            w=img->width*sh;
	            nY=y*w;
	       	    nX = (long)x*sh;
	            ptr=img->pixPtr+nY+nX;
	            *B = *ptr;
	            *G = *(ptr+1);
	            *R = *(ptr+2);
//	            *R = *ptr;
//	            *G = *(ptr+1);
//	            *B = *(ptr+2);

//	            rgba->A = *(ptr+3);

	    }


void setPixelArrayFromRGBAToBMP(LPIMGDATA imgOutp, LPIMGDATA img){
	BYTE R, G, B;
	int y, x;
	for(y=0;y<img->height;y++)	{
		for(x=0;x<img->width;x++) {
			getColorRGBA(img, x, y, &R, &G, &B);

//				if(y > 300 && y < 330 && x > 100 && x < 400) {
//					R = 255;G=255;B=255;
//				}
//	if(y > 300 && y < 310 && x > 100 && x < 400) {
//	   sprintf(buffLog," y= %d, x= %d, R= %d, G= %d, B= %d\n",y,x, R,G,B);
//	   saveLog(buffLog, "");
//	}
			setOnePixelBMP24(imgOutp, x, y, R, G, B);
		}
	}
}

void convertRGBAToBMP(LPIMGDATA img, LPIMGDATA imgOutp){
	LPBYTE ptr ;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int  biSizeImage, dpi;

	int biWidthInp, biWidthPadInp, biHeightInp;
	int offBits;
	int biWidthOut, biWidthPadOut, biHeightOut;

	printf("-----convertRGBAToBMP; start \n");
	fflush(stdout);

	biWidthInp=img->bi.biWidth;
	biWidthPadInp = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	biHeightInp = img->bi.biHeight;


	biWidthOut = img->bi.biWidth;
	biWidthPadOut = ((img->bi.biWidth*24+31)/32)*4;
	biHeightOut = img->bi.biHeight;
	dpi = img->dpiX;

//	bf = (LPBITMAPFILEHEADER)(img->ptr);
//	bi = (LPBITMAPINFOHEADER)(img->ptr + sizeof (BITMAPFILEHEADER));
//    hSize = getLeftBytes((int*)bf->bfOffBits, 4);
	offBits = img->bf.bfOffBits;

    printf("-----BMP biWidthPadInp= %d, img->bi.biHeight= %d, biWidthInp= %d, img->bi.biBitCount= %d \n", biWidthPadInp, img->bi.biHeight, biWidthInp, img->bi.biBitCount);
    fflush(stdout);

	biSizeImage = biWidthPadOut*img->bi.biHeight + offBits;

    printf("-----BMP biSizeImage= %d \n", biSizeImage);

printf("bf.bfSize= %lu, bf.bfOffBits= %lu \n",img->bf.bfSize, img->bf.bfOffBits);
fflush(stdout);

	ptr = yl_calloc( "BMP_B", biSizeImage, sizeof(BYTE));
	if(ptr == NULL) {
		sprintf(img->error,"convertRGB565To_BMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, biSizeImage, strerror(errno));
		return;
	}

//	copyBMPHeaderBinary(ptr, img->ptr, offBits);

	printf("offBits= %d \n",offBits);
	fflush(stdout);


	imgOutp->ptr = ptr;
	imgOutp->pixPtr = ptr + offBits;
	imgOutp->width = img->bi.biWidth;
	imgOutp->height = img->bi.biHeight;
	imgOutp->bf.bfType = 0x4d42;
	imgOutp->bf.bfSize = biSizeImage;
	imgOutp->bf.bfReserved1 =  0;
	imgOutp->bf.bfReserved2 =  0;
	imgOutp->bf.bfOffBits =   offBits;

	imgOutp->bi.biBitCount = 24;
	imgOutp->bi.biWidthPad = biWidthPadOut;
	imgOutp->bi.biHeight = biHeightOut;
	imgOutp->bi.biWidth = biWidthOut;


	imgOutp->bi.biSize = sizeof (BITMAPINFOHEADER);
	imgOutp->bi.biPlanes = 1;

	imgOutp->bi.biCompression = BI_RGB;
	imgOutp->bi.biSizeImage = biSizeImage - offBits;

	imgOutp->bi.biXPelsPerMeter = dpi;
	imgOutp->bi.biYPelsPerMeter = dpi;
	imgOutp->bi.biClrUsed = 0;
	imgOutp->bi.biClrImportant = 0;


	bf = (LPBITMAPFILEHEADER) ptr;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, biSizeImage);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, biWidthOut);
	set4Bytes(bi->biHeight, biHeightOut);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 24;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, biSizeImage - offBits);
	set4Bytes(bi->biXPelsPerMeter, dpi);
	set4Bytes(bi->biYPelsPerMeter, dpi);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);


//	setPixelArrayFromBMPToRGB_565(imgOutp, img);
//	   sprintf(buffLog," setPixelArrayFromRGB565ToBMP--------------------\n");
//	   saveLog(buffLog, "");

	printf(" imgOutp \n");
	fflush(stdout);

	setPixelArrayFromRGBAToBMP(imgOutp, img);

	printf(" setPixelArrayFromRGBAToBMP \n");
	fflush(stdout);

}
 void repairImageHeader(LPIMGDATA img){
//		int biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;

		LPBITMAPFILEHEADER bf;
		LPBITMAPINFOHEADER bi;


		bf = (LPBITMAPFILEHEADER) img->ptr;
		bf->bfType[0] = 0x42;
		bf->bfType[1] = 0x4d;
		set4Bytes(bf->bfSize, img->bf.bfSize);
		bf->bfReserved1[0] = 0;
		bf->bfReserved1[1] = 0;
		set4Bytes(bf->bfOffBits, img->bf.bfOffBits);

		bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
		set4Bytes(bi->biWidth, img->bi.biWidth);
		set4Bytes(bi->biHeight, img->bi.biHeight);
		bi->biPlanes[0] = 1;
		bi->biPlanes[1] = 0;
		bi->biBitCount[0]= img->bi.biBitCount;
		bi->biBitCount[1]= 0;
		set4Bytes(bi->biCompression, img->bi.biCompression);
		set4Bytes(bi->biSizeImage, img->bi.biSizeImage);
		set4Bytes(bi->biXPelsPerMeter, img->bi.biXPelsPerMeter);
		set4Bytes(bi->biYPelsPerMeter, img->bi.biYPelsPerMeter);
		set4Bytes(bi->biClrUsed, 0);
		set4Bytes(bi->biClrImportant, 0);



 }
