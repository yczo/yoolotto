/*
 * Test.c
 *
 *  Created on: Jan 9, 2014
 *      Author: Wicek
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


void savePixels (void* pixels,  int width, int height, int pixelsFormat) {
	FILE *f;
	int len;
	char fileName[500];
	size_t fSize = 0;

	strcpy(fileName, "c:\\KUNWAR\\Bitmap\\PixelsFile.bin");

	len = width*height;
	switch(pixelsFormat){
	case 0:// RGB565
		len *= 2;
		break;
	case 1:// RGBA
		len *= 4;
		break;
	case 3:// BMP
		len *= 3;
		break;
	}

	f= fopen(fileName, "w");
	if(f == NULL) {
		__android_log_print(ANDROID_LOG_VERBOSE, "KUNWAR ABHISHEK", "open file error; file name= %s", fileName);
		return;
	}

	fSize = fwrite((char *)pixels, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f)) {
			__android_log_print(ANDROID_LOG_VERBOSE, "KUNWAR ABHISHEK", "write file error; file name= %s, error= %s\n", fileName, strerror(errno));
		}
		else {
			__android_log_print(ANDROID_LOG_VERBOSE, "KUNWAR ABHISHEK", "write file error; file name= %s, size= %d != len= %d\n ", fileName, fSize*len, len);
		}
		return;
	}
	close_file(f);
}

other set of IO functions
//-----------------------------------------------------
OFSTRUCT of;
HFILE hFile;
hFile = OpenFile(
             fileName, &of, OF_CREATE | OF_WRITE
             );

lt = _lwrite (hFile, (char *)pixels, len);

_lclose (hFile);
//--------------------------------------------------
