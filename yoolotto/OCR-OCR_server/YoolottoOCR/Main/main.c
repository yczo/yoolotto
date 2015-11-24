/*
 * main.c
 *
 *  Created on: Jul 24, 2012
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include  "../headers/Image.h"
#include  "../headers/Prototype.h"
#include  "../headers/Functions.h"
#include  "../headers/ocr.h"
#include "../headers/memmgr.h"
#include "../headers/BarCode.h"




//#include "../LogoClassifier/Statistics/VectorStorage.h"
//#include "../LogoClassifier/Statistics/Vector.h"

int waitForInput();
static void setGlobalDataPrototypeDbg(LPTICKETDATA ticketDataPtr, char *mainPath);
void setGlobalDataImgIdx(LPIMGDATA img, int idx, int inp, int mode);
void setGlobalDataImg(LPIMGDATA img);
void setGlobalDataImg2(LPIMGDATA img);
void setGlobalDataImg3(LPIMGDATA img);
//void saveLog(char *str1, char *str2);
void printResultsToTestTrainFile(char *imgName, LPTICKETRESULTS ticketResults);
int getLine(char* results, int n, int* iStart, int* iStop);
void statsBenchmark(void);
void statsBenchmarkBarCode(void);
void statsBenchmarkBarCodeQuality(void);
void testRGBAcolors (LPIMGDATA img) ;
void convertBMPTo_RGBA(LPIMGDATA img, LPIMGDATA imgOutp);
void saveLogFile(char *str1, char *str2, char *fName);
//void setNewImageName(char *imgName, char *imgNInp, int loop);
//void SaveJpgFile(char *szBmpFileNameIn, char* szJpgFileNameOut);
//void SaveJpgFile(LPIMGDATA img, char* szJpgFileNameOut);
//void convertRGBAToBMP(LPIMGDATA img, LPIMGDATA imgOutp);



//int RGB_OCR_FirstStepCall(unsigned char *ptr, int imgWidth, int imgHeight, LPIMGDATA lpImgOut, char *error);
//int BMP_OCR_FirstStepCall(unsigned char *ptr, int imgWidth, int imgHeight, LPIMGDATA lpImgOut, char *error);
//int RGB_OCR_ServerCall(LPIMGDATA lpImgOut, char *mainPath,
//		            LPTICKETRESULTS lpTicketResults, char *error, char *imgName);

int setHeaderDataForRGBA (LPBYTE ptr, size_t fSize, LPIMGDATA img);
LPBYTE LoadRGBAPFile (LPIMGDATA img);
static void addSlashDbg(char *path);
void printResultsToTestTrainFileBCode(char *imgName, char *typeStr);
//void convertRGB565To_BMP(LPIMGDATA img, LPIMGDATA imgOutp);
void convertBMPTo_RGB565(LPIMGDATA img, LPIMGDATA imgOutp);
//LPBYTE compressImage(LPIMGDATA img);
//int deCompressImage(LPBYTE ptrC, LPIMGDATA img);


void testRGBA_RGB(LPIMGDATA img);
//void PrintOutputResults(LPTICKETRESULTS ticketResults);
void PrintOutputResults_new(LPTICKETRESULTS ticketResults);
//void copyOutputResults(LPTICKETRESULTS lpTicketResults, LPTICKETRESULTS ticketResultsInp);
static void initTicketResultsDbg(LPTICKETRESULTS lpTicketResults);
void printResultsToLog_new(LPTICKETRESULTS ticketResults);
void saveResultsLines(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut);
//void addBottomPart(LPIMGDATA img, int dd);
//int saveBytesCMPToDisk(COMPRESSED_IMG *outImgBuffer, char *path, char *iName);
int createSmallColorImage(LPIMGDATA img, LPIMGDATA imgOut, int pixelStepsX, int pixelStepsY);

int CMPImages_Tests(unsigned char *ptrInp, int imgWidth, int imgHeight, char *buffType,
		char *error, char *mainPath, char *imgName, int US_state);

int LoadFile (LPBYTE *jpgBytes, char *fileName);
LPBYTE readBytebuffer (char* name, char* path, int *size);
int OCR_Call(unsigned char *ptrJPG, int length, char *mainPath,	 int lotteryType,
		            int US_state, char *results, int resultslength, char *error, char *imgName);
//void getJPGInfo( LPBYTE jpgBytes1, int fSize1,  LPBYTE jpgBytes2, int fSize2);
//void JPGTest1();
//void getJPGInfo( LPIMGDATA imgInp);
int getInputLotteryType(int state, int idx);

IMGDATA img;
IMGDATA imgOut;
LPTICKETDATA ticketData;
int imgIndx = 0;
int countImgGap = 0;
COMPRESSED_IMG outImgBuffer;

int main(int argc, char **argv) {
int inp;
LPBYTE buff = NULL;
int sizeBuff;
char globalPath[500];
int ret = 0;
int US_state = 0;
int lotteryType = 0;
//int scratchOff = 0;
int resultslength = 4096;
char results[resultslength];
//char buffLog[2000];

//int imgQuality;
//int filtrMode = 0;

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
	  strcpy(globalPath, "../");
#else
	  strcpy(globalPath, "c:\\Users\\Wicek\\OCR\\");
#endif
//  unsigned char *ptrMonoBMPImg_Out;


  printf("Start program...\n");
  inp = waitForInput() ;

  printf("\nuser input= %d \n", inp);

  if(inp == 1) {
//	  statsBenchmark();
//	  statsBenchmarkBarCode();
	  statsBenchmarkBarCodeQuality();
	  return 0;

	  setGlobalDataImg2(&img);
	  setGlobalDataImg3(&imgOut);
	  img.ptr = NULL;
	  imgOut.ptr = NULL;
	  printf("\nreading image= %s from path= %s\n", img.szIMGName,img.szIMGPath );

	  convertInp24BMPtoMonochrome(&img, &imgOut);
/*
//	  buff = LoadBMPFile (&img);
	  img.width = 1936;
	  img.height = 2592;
	  img.dpiX = 512;
	  img.dpiY = 512;
	  buff = LoadRGBAPFile (&img);
	  ok = 1;
	  if (buff == NULL) {
		printf("buff = null \n");
		ok = 0;
	  }

	  if (strlen(img.error) > 0) {
		printf("-------error= %s\n", img.error);
		ok = 0;
	  }
      if(ok){
    	  printf("\n load successful \n");
//    	  printf(" biSizeImage= %ld \n", img.bi.biSizeImage);
    	  convertRGBAcolor_to_RGBAwb(img.pixPtr, img.width, img.height);
//    	  saveRGBAasBMP(img.pixPtr, img.width, img.height, &img);
//    	   testRGBAcolors (&img);
    	   convertRGBAToBMP(&img);

//    	  changeLines(&img, 0, 200, 255);
//    	  convertToWB(&img, 150);
    	  convertBMPTo_mono_BMP(&img, &imgOut, 120);
//   	   printBMPData (&imgOut);
    	  convertDPI_in_BMP(&imgOut, 300, 300);


    	   printBMPData (&imgOut);
//    	   printBMPData (&img);

//    	   convertRGBATo_mono_BMP(&img, 120);
//    	   convertRGBAToBMP_filterWB(&img, 120);
//    	  setColors(&img);
//     	  saveBMP (&img);
    	  saveBMP (&imgOut);
      }
*/
      if(img.ptr != NULL)
    	  yl_free(img.ptr);
      if(imgOut.ptr != NULL)
    	  yl_free(imgOut.ptr);

  }if(inp > 2) inp = 6;
  if(inp == 2 || inp == 3 || inp == 4 || inp == 5 ) {
printf("\n-----------------------------------------------------------------Start\n");
saveLog("\n-----------------------------------------------------------------Start\n", NULL);
      imgIndx = 0;
      countImgGap = 0;
	  while(1) {
		  TICKETRESULTS ticketResults;
		  TICKETRESULTS ticketResultsOut;
		  initTicketResultsDbg(&ticketResults);
		  initTicketResultsDbg(&ticketResultsOut);

		  img.ptr = NULL;
		  imgOut.ptr = NULL;
	      ticketData = (LPTICKETDATA)yl_calloc("MAIN_A", sizeof(TICKETDATA), sizeof(BYTE));
	      if(ticketData == NULL) {
	  	     printf("-------ticketData, error= %s\n", strerror(errno));
	  	     break;
	      }

	         setGlobalDataPrototypeDbg(ticketData, globalPath);
	         ticketData->fieldBMPPtr = NULL;

	         setGlobalDataImgIdx(&img, imgIndx, inp, 0);
	         setGlobalDataImgIdx(&imgOut, imgIndx, inp, 0);
//	         printf("\nreading image= %s from path= %s\n", img.szIMGName,img.szIMGPath );

//	            buff = LoadBMPFile (&img);
	           img.error[0] = 0;
	            buff = readBytebuffer (img.szIMGName,img.szIMGPath, &sizeBuff);

             if (buff == NULL || strlen(img.error) > 0) {
//            	 	             printf("buff = null \n");
//		         if (buff == NULL)
//		             printf("buff = null \n");
//   	             if (strlen(img.error) > 0)
//			         printf("-------error= %s, countImgGap= %d\n", img.error, countImgGap);
//   	             if(countImgGap > 10)
   	             if(countImgGap > 1500){
//		         printf("-------countImgGap= %d, filtrMode= %d\n", countImgGap, filtrMode);
//   	   	               if(filtrMode > 0)
   	                       break;
//   	                   else{
//   	                	filtrMode++;
//   	                	countImgGap = 0;
//   	                	imgIndx = 0;
//   	   	               }
   	             }
   	             countImgGap++;
   	             imgIndx++;
   	             continue;
	         }
//	         else {
//    	         printf(" reading successful \n");
//	         }
             countImgGap = 0;

     printf("\nloaded image= %s from path= %s\n", img.szIMGName,img.szIMGPath );
    fflush(stdout);

     saveLog("\nloaded image= ",img.szIMGName);
     saveLog("\n","\n");

     if(inp == 2) {
    	 US_state = 1;
//    	 scratchOff = 0;
//    	 lotteryType = getInputLotteryType(US_state, imgIndx);
    	 lotteryType = 2;

    	 img.error[0] = 0;
   	     ret = OCR_Call_server(buff, sizeBuff, globalPath,	lotteryType, US_state, results, resultslength, img.error, img.szIMGName);
   	     printf("ret= %d\n",ret);
 	      if(buff != NULL){
 	    	  yl_free(buff);buff = NULL;
 	      }

	         if(ret == 77) {
	 		    printf("imgName= %s, - rejected\n",img.szIMGName);
	         }else{
	        	 if(ret < 0)
	        		 printf("imgName= %s, - Error= %s\n",img.szIMGName, img.error);
//        		 else
//                 	 printf("---------------results:\n  %s\n",results);
	         }


//         saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);
              fflush(stdout);

// 	      if(ret == 77) {
// 	 		sprintf(buffLog,"imgName= %s, imageQuality= %d;\n",img.szIMGName, ret);
// 	 		saveLogFile("", buffLog, "log_SWT.txt");
// 	      }



/*

    	 if(scratchOff == 1){
    	       ret = Scratch_Off_Call(img.pixPtr, img.width, img.height, "BMP24", 3, 0, &imgOut, globalPath,
    	     		    &ticketResults, img.error, img.szIMGName, US_state);
    	 }
    	 else{
    	       ret = OCR_Call_new(img.pixPtr, img.width, img.height, "BMP24", 3, 0, &imgOut, globalPath,
    	     		    &ticketResults, img.error, img.szIMGName, US_state);
//    	       if(ret >=0 )
//    	         addImagesToTheOutput(&ticketResults, &imgOut);
    	   fflush(stdout);

    	 }
	       if(ret >=0 )
	         addImagesToTheOutput(&ticketResults, &imgOut);
*/

//    	 saveSlices(&ticketResults, &imgOut, imgOut.szOutPath);
     }

     if(inp == 3) { // RGBA
//   	  testRGBA_RGB(&img);
//      RGB_OCR_Call(img.pixPtr, img.width, img.height, globalPath,
//    		     &ticketResults, img.error, img.szIMGName);
//      OCR_Call(img.pixPtr, img.width, img.height, "RGB", 1, 0, &imgOut, globalPath,
//    		    &ticketResults, img.error, img.szIMGName);
    	 img.error[0] = 0;

    	 convertBMPTo_RGBA(&img, &imgOut);

         if(img.error[0] != 0)
       	  printf("1 error= %s", img.error);
         if(img.ptr != NULL){
   	      yl_free(img.ptr);
   	      img.ptr = NULL;
         }
//    	 US_state = 2;
    	 US_state = 0;
//    	 scratchOff = 1;
//    	 US_state = 3;
//    	 scratchOff = 0;
//	 US_state = 4;


//    	 ret = getCMPImagesForOCR(imgOut.pixPtr, imgOut.width, imgOut.height, "RGBA",  &outImgBuffer, &imgQuality,
//    			 img.error, globalPath, img.szIMGName, US_state);

//    	 printf("\n   imgQuality= %d \n\n", imgQuality);

//      	 ret = getCMPImagesForOCR(imgOut.pixPtr, imgOut.width, imgOut.height, "RGBA",
//      			 img.error, globalPath, img.szIMGName, US_state);

/*
     	  outImgBuffer.ptrBuffer = returnPtrToCompressedBuffer();
     	  outImgBuffer.size = returnSizeOfCompressedBuffer();
      	  printf("\n   imgQuality= %d \n\n", returnImageQualityValue());
   	   fflush(stdout);


    	   {
    		 char fileNameJPG[500];
         setNewImageNameJPG(fileNameJPG, img.szIMGName,0);
        saveJPGbuffer (outImgBuffer.ptrBuffer, outImgBuffer.size, fileNameJPG, "c:\\Users\\Wicek\\OCR\\output\\");
    	   }
*/
//    	 testCompressImages(&outImgBuffer, img.szIMGName);

//    	 saveBytesCMPToDisk(&outImgBuffer, "c:\\Users\\Wicek\\OCR\\output\\", img.szIMGName);
    	   fflush(stdout);

//    	 freeMemoryCMPImage();

/*
      	 if(scratchOff == 1){
      	       ret = Scratch_Off_Call(imgOut.pixPtr, imgOut.width, imgOut.height, "RGBA", 3, 0, &img, globalPath,
      	     		    &ticketResults, img.error, img.szIMGName, US_state);
      	 }
      	 else{
      	       ret = OCR_Call_new(imgOut.pixPtr, imgOut.width, imgOut.height, "RGBA", 3, 0, &img, globalPath,
      	     		    &ticketResults, img.error, img.szIMGName, US_state);

      	   fflush(stdout);

      	 }

//       printf("\n   ret= %d, error= %s \n", ret, imgOut.error);
//       fflush(stdout);
	       if(ret >=0 )
	         addImagesToTheOutput(&ticketResults, &img);
*/

//       printf("\n   ret= %d, error= %s \n", ret, imgOut.error);

//              addImagesToTheOutput(&ticketResults, &img);
        fflush(stdout);
        ret = -1;

     }
     if(inp == 4) {  // RGB565
    	 img.error[0] = 0;
         convertBMPTo_RGB565(&img, &imgOut);

        if(img.ptr != NULL){
   	      yl_free(img.ptr);
   	      img.ptr = NULL;
         }

//       US_state = 2;
  	   US_state = 3;
//       US_state = 0;
//  	 scratchOff = 1;
//  	 scratchOff = 0;
//  	US_state = 4;
//	 ret = getCMPImagesForOCR(imgOut.pixPtr, imgOut.width, imgOut.height, "RGB565",  &outImgBuffer,  &imgQuality,
//			 img.error, globalPath, img.szIMGName, US_state);

//	 printf("\n   imgQuality= %d \n\n", imgQuality);


//  	 ret = getCMPImagesForOCR(imgOut.pixPtr, imgOut.width, imgOut.height, "RGB565",
//  			 img.error, globalPath, img.szIMGName, US_state);

/*
 	  outImgBuffer.ptrBuffer = returnPtrToCompressedBuffer();
 	  outImgBuffer.size = returnSizeOfCompressedBuffer();
  	  printf("\n   imgQuality= %d \n\n", returnImageQualityValue());
	   fflush(stdout);


 	   {
 		 char fileNameJPG[500];
      setNewImageNameJPG(fileNameJPG, img.szIMGName,0);
     saveJPGbuffer (outImgBuffer.ptrBuffer, outImgBuffer.size, fileNameJPG, "c:\\Users\\Wicek\\OCR\\output\\");
 	   }
*/
	 //    	 testCompressImages(&outImgBuffer, img.szIMGName);
//	     	 saveBytesCMPToDisk(&outImgBuffer, "c:\\Users\\Wicek\\OCR\\output\\", img.szIMGName);

//	     	 freeMemoryCMPImage();

/*
  	 if(scratchOff == 1){
  	       ret = Scratch_Off_Call(imgOut.pixPtr, imgOut.width, imgOut.height, "RGB565", 3, 0, &img, globalPath,
  	     		    &ticketResults, img.error, img.szIMGName, US_state);
  	 }
  	 else{
  	       ret = OCR_Call_new(imgOut.pixPtr, imgOut.width, imgOut.height, "RGB565", 3, 0, &img, globalPath,
  	     		    &ticketResults, img.error, img.szIMGName, US_state);
  	   fflush(stdout);

  	 }
       if(ret >=0 )
         addImagesToTheOutput(&ticketResults, &img);

*/
//       printf("\n   ret= %d, error= %s \n", ret, imgOut.error);
       fflush(stdout);
       ret = -1;

//       if(ret >=0 )
//              addImagesToTheOutput(&ticketResults, &img);
//        fflush(stdout);
//          saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);

     }
     if(inp == 5) { //converters
int pixelStepsX,  pixelStepsY;
    	  printf("\n   imgOut.ptr= %lu \n", (unsigned long)imgOut.ptr);
    	  pixelStepsX = 2,  pixelStepsY = 2;
//    	  if(img.bi.biWidth >img.bi.biHeight)
//    			  pixelStepsX = 4,  pixelStepsY = 3;

    	  createSmallColorImage(&img, &imgOut, pixelStepsX, pixelStepsY);
    	 saveBMPbuffer (imgOut.ptr, img.szIMGName, img.szOutPath);
//   	  printf("out;   imgOut.ptr= %lu \n", (unsigned long)imgOut.ptr);
ret = -1;
/*
           convertBMPTo_RGB565(&img, &imgOut);

           if(img.ptr != NULL){
     	      yl_free(img.ptr);
     	      img.ptr = NULL;
           }
           convertRGB565To_BMP(&imgOut, &img);
*/
//  saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);

//    	 convertRGBAToBMP(&imgOut, &img);
//         if(imgOut.error[0] != 0)
//       	  printf("2 error= %s", imgOut.error);
//        saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);

//         ret = -1;



  /*
         convertBMPTo_RGB565(&img, &imgOut);

         if(img.ptr != NULL){
   	      yl_free(img.ptr);
   	      img.ptr = NULL;
         }

    	 US_state = 0;
       ret = OCR_Call_new(imgOut.pixPtr, imgOut.width, imgOut.height, "RGB565", 3, 0, &img, globalPath,
     		    &ticketResults, img.error, img.szIMGName, US_state);

       printf("\n   ret= %d, error= %s \n", ret, img.error);

              addImagesToTheOutput(&ticketResults, &img);
        fflush(stdout);
          saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);
*/
/*
          if(img.error[0] != 0)
        	  printf("1 error= %s", img.error);
//         saveBMPbuffer (img.ptr, img.szIMGName, img.szOutPath);

         if(img.ptr != NULL){
   	      yl_free(img.ptr);
   	      img.ptr = NULL;
         }

    	 imgOut.error[0] = 0;
         convertRGB565To_BMP(&imgOut, &img);

         if(imgOut.error[0] != 0)
       	  printf("2 error= %s", imgOut.error);
*/

     }
//     if(ret >=0 && scratchOff == 0) {
    if(ret >=0) {
//       PrintOutputResults_new(&ticketResults);
//       printResultsToLog_new(&ticketResults);
       fflush(stdout);
//   printResultsToTestTrainFile(imgOut.szIMGName, &ticketResults);
//   printf("FreeTicketResultsMemory_Call\n");
//   fflush(stdout);
//     FreeTicketResultsMemory_Call(&ticketResults, &imgOut);

//     printf("after FreeTicketResultsMemory_Call\n");
//     fflush(stdout);
     }
     //     waitForInput() ;
//	 break;

            if(img.ptr != NULL){
      	      yl_free(img.ptr);
      	      img.ptr = NULL;
            }
            if(imgOut.ptr != NULL) {
      	     yl_free(imgOut.ptr);
      	     imgOut.ptr = NULL;
            }
		  imgIndx++;
		     printf("imgIndx= %d\n", imgIndx);
		     fflush(stdout);

//          break;
	    } // while
  }



	  printf("\ndone \n");


  return 0;
}

void setGlobalDataImg(LPIMGDATA img){
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../input/");
#else
	  strcpy(img->szIMGPath,"c:\\Users\\Wicek\\OCR\\input\\");
#endif
//	  strcpy(img->szIMGName,"IMG_0400.jpg");
//	  strcpy(img->szIMGName,"IMG_0400.bmp");
//	  strcpy(img->szIMGName,"IMG_wb.bmp");
//	  strcpy(img->szIMGName,"RGBA_WB.bmp");
//	  strcpy(img->szIMGName,"IMG_BLACK.bmp");
//	  strcpy(img->szIMGName,"IMG_R.bmp");
//	  strcpy(img->szIMGName,"IMG_B.bmp");
	  strcpy(img->szIMGName,"IMG_1.bmp");
//	  strcpy(img->szIMGName,"IMG_test1.bmp");

//	  strcpy(img->szIMGName,"IMG_RGBA.rgb");
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__  || LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../output/");
#else
	  strcpy(img->szOutPath,"c:\\Users\\Wicek\\OCR\\output\\");
#endif
}
void setGlobalDataImg2(LPIMGDATA img){
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../input/");
#else
	  strcpy(img->szIMGPath,"c:\\Users\\Wicek\\OCR\\input\\");
#endif

//	  strcpy(img->szIMGName,"IMG_0400.jpg");
//	  strcpy(img->szIMGName,"IMG_0400.bmp");
//	  strcpy(img->szIMGName,"IMG_wb.bmp");
//	  strcpy(img->szIMGName,"RGBA_WB.bmp");
//	  strcpy(img->szIMGName,"IMG_BLACK.bmp");
//	  strcpy(img->szIMGName,"IMG_R.bmp");
//	  strcpy(img->szIMGName,"IMG_B.bmp");
//	  strcpy(img->szIMGName,"IMG_1.bmp");
//	  strcpy(img->szIMGName,"IMG_test1.bmp");

	  strcpy(img->szIMGName,"IMG_RGBA.rgb");
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__  || LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../output/");
#else
	  strcpy(img->szOutPath,"c:\\Users\\Wicek\\OCR\\output\\");
#endif
}
void setGlobalDataImg3(LPIMGDATA img){
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../input/");
#else
	  strcpy(img->szIMGPath,"c:\\Users\\Wicek\\OCR\\input\\");
#endif
//	  strcpy(img->szIMGName,"IMG_0400.jpg");
//	  strcpy(img->szIMGName,"IMG_0400.bmp");
//	  strcpy(img->szIMGName,"IMG_wb.bmp");
	  strcpy(img->szIMGName,"RGBA_WB.bmp");
//	  strcpy(img->szIMGName,"IMG_BLACK.bmp");
//	  strcpy(img->szIMGName,"IMG_R.bmp");
//	  strcpy(img->szIMGName,"IMG_B.bmp");
//	  strcpy(img->szIMGName,"IMG_1.bmp");
//	  strcpy(img->szIMGName,"IMG_test1.bmp");

//	  strcpy(img->szIMGName,"IMG_RGBA.rgb");
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../output/");
#else
	  strcpy(img->szOutPath,"c:\\Users\\Wicek\\OCR\\output\\");
#endif
}


void setGlobalDataImgIdx(LPIMGDATA img, int idx, int inp, int mode){
	  char strNum[10];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__  || LINUX || linux || UNIX)
	  strcpy(img->szIMGPath,"../input/");
	  strcpy(img->szOutPath,"../output/");
	  strcpy(img->szWorkPath,"../work/");
#else
	  strcpy(img->szIMGPath,"c:\\Users\\Wicek\\OCR\\input\\");
	  strcpy(img->szOutPath,"c:\\Users\\Wicek\\OCR\\output\\");
	  strcpy(img->szWorkPath,"c:\\Users\\Wicek\\OCR\\work\\");
#endif
	  sprintf(strNum,"%d",idx);
/*
	  if(idx <10){
		 strNum[0] = '0'+idx;
		 strNum[1] = 0;
	  }else{
		  if(idx <100){
		    strNum[0] = '0'+idx/10;
		    strNum[1] = '0'+idx%10;
		    strNum[2] = 0;
		  } else {
		     if(idx <1000){
		       strNum[0] = '0'+idx/100;
		       strNum[1] = '0'+idx/10;
		       strNum[2] = '0'+idx%10;
		       strNum[3] = 0;
		     }
		  }

	  }
*/
//	  if(inp == 2 || inp == 5){
//	      strcpy(img->szIMGName,"IMG_BMP_FL_");
//	      strcpy(img->szIMGName,"IMG_BMP_NY_");
//	      strcpy(img->szIMGName,"IMG_BMP_C_");
//	      strcpy(img->szIMGName,"IMG_BMP_T_");
//	      strcpy(img->szIMGName,"IMG_BMP_SCR_");
//	      strcpy(img->szIMGName,"IMG_BMP_");
//	  if(mode == 0)
//	      strcpy(img->szIMGName,"IMG_BMP_TEST_");
	      strcpy(img->szIMGName,"IMG_TEST_");
//	  else
//		  strcpy(img->szIMGName,"IMG_BMP_TESTNF_");

		  strcat(img->szIMGName, strNum);
//		  strcat(img->szIMGName, ".bmp");
		  strcat(img->szIMGName, ".jpg");
//	  }
/*
	  if(inp == 3 || inp == 4){
	      strcpy(img->szIMGName,"IMG_RGBA_");
		  strcat(img->szIMGName, strNum);
		  if(mode == 0)
		     strcat(img->szIMGName, ".rgb");
		  if(mode == 1)
		     strcat(img->szIMGName, ".bmp");

	  }
*/
}

/*
void saveLog(char *str1, char *str2){
   FILE *f;
//   int  len;
   char fileName[500];
   char buff[2000];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
	   strcpy(fileName, "../log/log.txt");
#else
	   strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\log.txt");
#endif

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
void saveLogFile(char *str1, char *str2, char *fName){
   FILE *f;
//   int  len;
   char fileName[500];
   char buff[2000];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__  || LINUX || linux || UNIX)
	   strcpy(fileName, "../log/");
#else
	   strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\");
#endif
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

static void initTicketResultsDbg(LPTICKETRESULTS lpTicketResults){
	int i;
	lpTicketResults->status = 0;
	lpTicketResults->type = 0;
	lpTicketResults->typeCnf = 0;
	lpTicketResults->yShift = 0;

	for(i=0;i<NUMFIELDS_RESULTS;i++){
		lpTicketResults->fieldsResults[i].name[0] = 0;
	}

}
static void addSlashDbg(char *path){

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
	const char slash = '/';
#else
	const char slash = '\\';
#endif

	if(path[strlen(path)-1] != slash ){
		int size = strlen(path);
		path[size] = slash;
		path[size+1] = '\0';
	}
}

static void setGlobalDataPrototypeDbg(LPTICKETDATA ticketDataPtr, char *mainPath){
	strcpy(ticketDataPtr->prototypePath, mainPath);
	addSlashDbg( ticketDataPtr->prototypePath );
	strcpy(ticketDataPtr->libraryPath, mainPath);
	addSlashDbg( ticketDataPtr->libraryPath );
	ticketDataPtr->wCorrectionLevel = 20;
	ticketDataPtr->tgp = 20;                // distortion
	ticketDataPtr->wPar = 30;               // calibration
	ticketDataPtr->phi.phiMin = -50;
	ticketDataPtr->phi.phiMax =  50;
	ticketDataPtr->phi.stepPhi =  5;
}
LPBYTE LoadRGBAPFile (LPIMGDATA img)
{
	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	LPBYTE ptr;
	size_t fSize = 0;
	char *szFileName = img->szIMGName;
	char *szPathName = img->szIMGPath;
	int bufforSize = 0;
	int hSize = 0;

	img->error[0] = 0;
	if (!szFileName) return NULL;  // test input file name
	strcpy(fileName, szPathName);
	strcat(fileName, szFileName);

	printf("open file %s\n",fileName);

	f= fopen(fileName, "r");
	if(f == NULL) {
		sprintf(img->error,"LoadImage (\"%s\") failed: %s\n",fileName, strerror(errno));
		return NULL;
	}
	fseek(f, 0L,SEEK_END);
	//    printf("fseek %s\n",fileName);

	if(ferror(f)){
		sprintf(img->error,"LoadImage (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
		return NULL;
	}
	//    printf("ftell %s\n",fileName);

	len = ftell(f);
	//    printf("fseek %s\n",fileName);
	fseek(f, 0L,SEEK_SET);

	printf("file %s length= %d\n",fileName, len);

	hSize = sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER);
	bufforSize = len + hSize;
	ptr = yl_calloc("MAIN_B", bufforSize, sizeof(BYTE));
	if(ptr == NULL) {
		//    	printf("yl_calloc failed");
		sprintf(img->error,"LoadImage ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",fileName, bufforSize, strerror(errno));
		return NULL;
	}
	fSize = fread((char *)(ptr+hSize), len, 1, f);


	//    printf("fSize %ld \n",fSize*len);

	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			sprintf(img->error,"LoadImage (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
			sprintf(img->error,"LoadImage (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		return NULL;
	}
	if(setHeaderDataForRGBA (ptr, fSize*len, img ) < 0) {
		yl_free(ptr);
		ptr = NULL;
	}

	close_file(f);
	return ptr;
}

int setHeaderDataForRGBA (LPBYTE ptr, size_t fSize, LPIMGDATA img) {

	//	   LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(ptr);
	//	   LPBITMAPINFOHEADER bi = (LPBITMAPINFOHEADER)(ptr + sizeof (BITMAPFILEHEADER));
	int hSize = sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER);
	unsigned long uFlth;

	img->ptr = ptr;
	//	   img->bf.bfType = getLeftBytes((int*)bf->bfType, 2);
	img->bf.bfType = 0x4d42;
	img->bf.bfSize = fSize+hSize;
	img->bf.bfReserved1 =  0;
	img->bf.bfReserved2 =  0;
	img->bf.bfOffBits =   hSize;
	img->pixPtr = ptr +img->bf.bfOffBits;

	img->bi.biSize = hSize - 14;
	img->bi.biWidth = img->width;
	img->bi.biHeight = img->height;
	img->bi.biPlanes = 1;
	img->bi.biBitCount = 32;
	img->bi.biCompression = 0;
	img->bi.biSizeImage = fSize;
	img->bi.biXPelsPerMeter = img->dpiX;
	img->bi.biYPelsPerMeter = img->dpiY;
	img->bi.biClrUsed = 0;
	img->bi.biClrImportant = 0;
	img->bi.biWidthPad = ((img->bi.biWidth*img->bi.biBitCount+31)/32)*4;
	img->colorPtr = ptr + 14 + img->bi.biSize;


	uFlth = img->bi.biWidth * (img->bi.biBitCount/8)* img->bi.biHeight;

	if ((img->bi.biWidth  > (LONG)MAX_WIDTH) ||
			(img->bi.biHeight > (LONG)MAX_HEIGHT)) {
		//	    	strcpy(img->error,  "Image size exceeds allowed limit");
		sprintf(img->error,"Image width/height exceeds allowed limit; width= %d, height= %d\n",img->bi.biWidth, img->bi.biHeight);
		return -5;
	}
	// verify if the the length of file is the same as in the bmp header
	if (fSize != (LONG)uFlth) {
		//	    	strcpy(img->error,  "Image/file length problem");
		sprintf(img->error,"Image size problem; file size= %d, RGBA size= %ld\n", fSize, uFlth);
		return -5;
	}
	printBMPData (img);
	return 0;
}
void setRGBcolor (LPBYTE ptrOut, int wInp, int xn,int yn, BYTE r, BYTE g, BYTE b) {
	int  w;
	long nY;
	long nX;
	int sh= 3;
	LPBYTE ptrL;

	w = wInp*sh;
	nY=yn*w;
	nX = (long)xn*sh;
	ptrL=ptrOut+nY+nX;
	*ptrL = r;
	*(ptrL+1) = g;
	*(ptrL+2) = b;
}
void getRGBAcolor2 (LPBYTE ptrInp, int wInp, int xn,int yn, LPBYTE r, LPBYTE g, LPBYTE b) {
	int  w;
	long nY;
	long nX;
	int sh= 4;
	LPBYTE ptr;

	w=wInp*sh;
	nY=yn*w;
	nX = (long)xn*sh;
	ptr=ptrInp+nY+nX;
	*r = *ptr;
	*g = *(ptr+1);
	*b = *(ptr+2);
	//	            rgba->A = *(ptr+3);

}
void reloadImgBufferFromRGBAtoRGB(LPBYTE ptrInp, int w, int h, LPBYTE ptrOut){
	BYTE r, g, b;
	int i, k;
	for(i=0;i<h;i++)	{
		for(k=0;k<w;k++){
			getRGBAcolor2 (ptrInp,  w, k, i, &r, &g, &b);
			setRGBcolor (ptrOut, w, k, i, r, g, b);
			//  printf("A= %x, R= %x, G= %x, B=%x \n", rgba.A, rgba.R, rgba.G, rgba.B);
		}
	}
}

void testRGBA_RGB(LPIMGDATA img){
	int bufforSize;
	LPBYTE ptrT;
	//   int i;

	bufforSize = img->bi.biWidth*img->bi.biHeight*4;

	ptrT = yl_calloc("MAIN_C", bufforSize, sizeof(BYTE));
	if(ptrT == NULL) {
		sprintf(img->error,"convertRGBAToBMP ( \"%s\" ) yl_calloc ( %d ) failed: %s\n",img->szIMGName, bufforSize, strerror(errno));
		return;
	}


	reloadImgBufferFromRGBAtoRGB(img->pixPtr, img->bi.biWidth, img->bi.biHeight, ptrT);
	img->pixPtr = ptrT;


}
//void printBMPDataTest (LPBYTE imgPtr);
void saveResultsLines(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut){
	   int i, line;
	   LPOUTIMGS lpImgs;
	   char outBuffer[500];
//	   char num[10];

//	   char buffLog[500];

	    for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(lpTicketResults->fieldsResults[i].name[0] != 0){

//   sprintf(buffLog,"-------saveResultsLines; i=%d, lpTicketResults->fieldsResults[i].name= %s, \n", i, lpTicketResults->fieldsResults[i].name);
//   saveLog("", buffLog);

		    	if(lpTicketResults->fieldsResults[i].ptrOutImages != NULL){


//  	   sprintf(buffLog,"-------saveResultsLines; lpTicketResults->fieldsResults[i].ptrOutImages != NULL\n");
//  	   saveLog("", buffLog);


		    		lpImgs = lpTicketResults->fieldsResults[i].ptrOutImages;
		    		line = 0;
		    		while(lpImgs != NULL){
						sprintf( outBuffer, "%s_%s_slice%d.bmp", imgOut->szIMGName, lpTicketResults->fieldsResults[i].name, line );

//   		  	   saveLog("lpTicketResults->fieldsResults[i].name", lpTicketResults->fieldsResults[i].name);
//   		  	   saveLog("", "\n");

//  	   sprintf(buffLog,"-------saveResultsLines; outBuffer= %s, imgOut->szOutPath= %s\n", outBuffer, imgOut->szOutPath);
//  	   saveLog("", buffLog);

// printBMPDataTest (lpImgs->lpImgBuffer);
		    			saveBMPbuffer (lpImgs->lpImgBuffer, outBuffer, imgOut->szOutPath);
		    			lpImgs = lpImgs->ptrNext;
		    			line++;
		    		}
		    	}
	    	}
	    }
}

void saveCSVFile(int* v1, int* v2, int *v3, int len, char* t1, char* t2, char* t3, char *name){
   FILE *f;
   int i;
   char fileName[500];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__  || LINUX || linux || UNIX)
   strcpy(fileName, "../work/");
#else
	   strcpy(fileName, "c:\\Users\\Wicek\\OCR\\work\\");
#endif

//   strcpy(fileName, "c:\\Users\\Wicek\\OCR\\work\\");
   strcat(fileName, name);
   strcat(fileName, ".csv");

   f= fopen(fileName, "wt");
    if(f == NULL) {
    	fprintf(stderr,"saveCSVFile (\"%s\") failed: %s\n",fileName, strerror(errno));
    	return;
    }
   if(t1 && t2 && t3) fprintf (f, "%s,%s,%s\n", t1, t2, t3);
   if(t1 && t2 && t3==NULL) fprintf (f, "%s,%s\n", t1, t2);
   if(t1 && t2==NULL && t3==NULL) fprintf (f, "%s\n", t1);
   for(i=0;i<len;i++){
	   if(v1 && v2 && v3) fprintf (f, "%d,%d,%d\n", v1[i], v2[i], v3[i]);
	   if(v1 && v2 && v3==NULL) fprintf (f, "%d,%d\n", v1[i], v2[i]);
	   if(v1 && v2==NULL && v3==NULL) fprintf (f, "%d\n", v1[i]);
   }
   fclose (f);
}

void addTypeToBuff(int type, int cnf, char * buff){
    int len = strlen(buff);
	switch(type){
	   case 0: sprintf(&buff[len],"M,%d",cnf); break;
	   case 1: sprintf(&buff[len],"P,%d",cnf); break;
	   default: sprintf(&buff[len],"?"); break;
	}
}
void copyLineToBuff(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, char *buff, int idx){
	   char ch;
	   int i, k, len;
	   char resultL[500];
	   int spCount;

	   len = strlen(buff);
	   buff[len] = 'A'+idx;
	   buff[len+1] = ',';
	   buff[len+2] = 0;
	   spCount = 5;
	   k = 0;
	   for(i=iStart;i<=iStop;i++){
		   ch = lpFieldsResults->result[i];
		   if(ch == ' ')
			   spCount++;
		    if(ch >= '0' && ch <= '9')
			   spCount = 0;

// printf("ch= %c, spCount=%d \n",  ch, spCount);
		   if((ch >= '0' && ch <= '9') || (ch == ' ' && spCount == 1 && k > 0)) {
		     resultL[k] = ch;

//printf("k= %d, ch= %c, resultL= %s, spCount=%d \n", k, ch, resultL, spCount);
		     k++;
		     resultL[k] = 0;
		   }
	   }
	   resultL[k] = 0;
       strcat(buff, resultL);
       strcat(buff, "\n");
}
void addNumbersToBuff(LPFIELDSRESULTS lpFieldsResults, char *buffOut, char *buffStart){
	 char *resultsStr = lpFieldsResults->result;
	 int  i;
	 int iStart, iStop;
	 int len;

     i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		len = strlen(buffOut);
		strcpy(&buffOut[len], buffStart);
		copyLineToBuff(lpFieldsResults, iStart, iStop, &buffOut[len], i);
		i++;
	 }
}
void addSpaceToBuff(char *buff, int idx){
	int len = strlen(buff);
	int i;
	for(i=len;i>=idx;i--){
		buff[i+1] = buff[i];
	}
	buff[idx] = ' ';
}
void printResultsToTestTrainFile(char *imgName, LPTICKETRESULTS ticketResults){
	int i, len;
	char buff[2000];
	char buffStart[100];
	char buffTmp[200];
	char buffimgName[100];
	strcpy(buffimgName, imgName);
	buffimgName[strlen(buffimgName)-4] = 0;
	 sprintf(buff,"%s,TYPE,",buffimgName);
     addTypeToBuff(ticketResults->type, ticketResults->typeCnf, buff);
     len = strlen(buff);
     sprintf(&buff[len],"\n");
    for(i=0;i<NUMFIELDS_RESULTS;i++){
        len = strlen(buff);
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATETOP") == 0){
    			strcpy(buffTmp,ticketResults->fieldsResults[i].result);
    			addSpaceToBuff(buffTmp, 7);
    			sprintf(&buff[len],"%s,DATETOP,%s\n",buffimgName,buffTmp);
    		}
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0){
    			    sprintf(buffStart,"%s,NUMBERS,",buffimgName);
    				addNumbersToBuff(&ticketResults->fieldsResults[i], &buff[len], buffStart);
    		}
    		if(strcmp(ticketResults->fieldsResults[i].name, "PLAYER") == 0) {
    			sprintf(&buff[len],"%s,PLAYER,%s\n",buffimgName,ticketResults->fieldsResults[i].result);
    		}
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATE") == 0) {
    			strcpy(buffTmp,ticketResults->fieldsResults[i].result);
    			addSpaceToBuff(buffTmp, 7);
    			sprintf(&buff[len],"%s,DATE,%s\n",buffimgName,buffTmp);
    		}
    	}
    }

    {
    FILE *f;
   	char fileName[500];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
    strcpy(fileName, "../log/output.csv");
#else
   strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\output.csv");
#endif

//       strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\output.csv");

     f= fopen(fileName, "a");
     if(f == NULL) {
     	printf("saveCSVFile (\"%s\") failed: %s\n",fileName, strerror(errno));
     	return;
     }

     fprintf (f, "%s", buff);
     fclose (f);
    }
}
char buffOut[60000];
char buffTest[60000];
int isItEndOfLine(char ch){
	if(ch == 13 || ch == 10) return 1;
	return 0;
}
void getValueFromLine(char *txtL, int n, char *word){
	int startIdx = 0, i, len = strlen(txtL);
	int cc = 0;
	word[0] = 0;
	if(n > 0) {
	   startIdx = -1;
	   for(i=0;i<len;i++){
		   if(txtL[i] == ',') cc++;
		   if(cc >= n) {startIdx = i+1;break;}
	   }
	}
	if(startIdx < 0) return;
    for(i=startIdx;i<len;i++){
    	if(txtL[i] == ',') break;
    	word[i-startIdx] = txtL[i];
    }
    word[i-startIdx] = 0;
}
char* getCurrentLineTxt(char* ptr, char* textLineOut){
	int i, len, k;
	char ch;
	textLineOut[0] = 0;
	len = strlen(ptr);
	k = 0;
	for(i=0;i<len;i++){
		ch = ptr[i];
		if(isItEndOfLine(ptr[i])) {
			textLineOut[i] = 0;
			k = i;
			while(isItEndOfLine(ptr[k]) && k < len) k++;
			if(k<len)
				return &ptr[k];
			return NULL;
		}
		textLineOut[i] = ptr[i];
	}
	return NULL;
}
void trimEndOfStr(char *str){
	int i, len;
	len = strlen(str);
	for(i=len-1;i>=0;i--){
		if(str[i] <= 32) str[i] = 0;
		else break;
	}
}
void statsBenchmark(void) {
    FILE *f;
    int len, fSize;
   	char fileNameOut[500];
   	char fileNameTest[500];

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
    strcpy(fileNameOut, "../log/output.csv");
    strcpy(fileNameTest, "../log/Test.csv");
#else
    strcpy(fileNameOut, "c:\\Users\\Wicek\\OCR\\log\\output.csv");
    strcpy(fileNameTest, "c:\\Users\\Wicek\\OCR\\log\\Test.csv");
#endif

//       strcpy(fileNameOut, "c:\\Users\\Wicek\\OCR\\log\\output.csv");
//       strcpy(fileNameTest, "c:\\Users\\Wicek\\OCR\\log\\Test.csv");

       f= fopen(fileNameOut, "r");
       if(f == NULL) {
       	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameOut, strerror(errno));
       	return;
       }
   	  fseek(f, 0L,SEEK_END);
   	  len = ftell(f);
   	  fseek(f, 0L,SEEK_SET);
printf(" file= %s, len= %d\n",fileNameOut, len);
  	  fSize = fread(buffOut, len, 1, f);
  	  if(ferror(f) || fSize*len != len){
  		if(ferror(f))
  			printf("read file (\"%s\") read failed: %s\n",fileNameOut, strerror(errno));
  		else
  			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameOut, fSize*len, len);
       }
      fclose (f);


      f= fopen(fileNameTest, "r");
      if(f == NULL) {
      	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameTest, strerror(errno));
      	return;
      }
  	  fseek(f, 0L,SEEK_END);
  	  len = ftell(f);
  	  fseek(f, 0L,SEEK_SET);

  	printf(" file= %s, len= %d\n",fileNameTest, len);
 	  fSize = fread(buffTest, len, 1, f);
 	  if(ferror(f) || fSize*len != len){
 		if(ferror(f))
 			printf("read file (\"%s\") read failed: %s\n",fileNameTest, strerror(errno));
 		else
 			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameTest, fSize*len, len);
      }
     fclose (f);

     {// stats

    	 char *ptrOut = buffOut;
    	 char *ptrTest = buffTest;
      	 char *ptr;
         char word[100];

    	 char textLineOut[1000];
    	 char textLineTest[1000];

         char imgNameOut[100];
         char imgTypeOut[10];
         char imgDateTopOut[20];
         char imgNumbersOut[10][30];
         int  lineOut;
         char imgPlayerOut[10];
         char imgDateOut[20];


         char imgNameTest[100];
         char imgTypeTest[10];
         char imgDateTopTest[20];
         char imgNumbersTest[10][30];
         int  lineTest;
         char imgPlayerTest[10];
         char imgDateTest[20];

         int i;
         int fieldsTotal = 0;
         int linesTotal = 0;
         int typeErrors = 0, dateTopErrors= 0;
         int numbersErrors= 0, playerErrors = 0, dateErrors = 0;
         char buffLog[500];

 saveLog("-------------statistic--------------------------------", "\n");
     while(1) {

// load	 next image
//   out
         ptr = ptrOut;
         imgNameOut[0] = 0;
         imgTypeOut[0] = 0;
         imgDateTopOut[0] = 0;
         imgPlayerOut[0] = 0;
         imgDateOut[0] = 0;
         for(i=0;i<10;i++)
            imgNumbersOut[i][0] = 0;
         lineOut = 0;
         while(1) {
    	    ptrOut = getCurrentLineTxt(ptr, textLineOut);
    	    getValueFromLine(textLineOut, 0, word);
    	    if(strcmp(word, imgNameOut) != 0 && imgNameOut[0] != 0) {
//    	    	strcpy(imgNameOut, word);
    	    	ptrOut = ptr;
    	    	break;
    	    }
    	    if(imgNameOut[0] == 0)
    	    	strcpy(imgNameOut, word);
    	    getValueFromLine(textLineOut, 1, word);
    	    if(strcmp(word, "TYPE") == 0){
    	    	getValueFromLine(textLineOut, 2, imgTypeOut);
    	    }
    	    if(strcmp(word, "DATETOP") == 0){
    	    	getValueFromLine(textLineOut, 2, imgDateTopOut);
    	    }
    	    if(strcmp(word, "NUMBERS") == 0 && lineOut < 10){
    	    	getValueFromLine(textLineOut, 3, imgNumbersOut[lineOut]);
    	    	lineOut++;
    	    }
    	    if(strcmp(word, "PLAYER") == 0){
    	    	getValueFromLine(textLineOut, 2, imgPlayerOut);
    	    }
    	    if(strcmp(word, "DATE") == 0){
    	    	getValueFromLine(textLineOut, 2, imgDateOut);
    	    }
    	    if(ptrOut == NULL) break;
    	    ptr = ptrOut;
         }

//  test
         ptr = ptrTest;
         imgNameTest[0] = 0;
         imgTypeTest[0] = 0;
         imgDateTopTest[0] = 0;
         imgPlayerTest[0] = 0;
         imgDateTest[0] = 0;
         for(i=0;i<10;i++)
            imgNumbersTest[i][0] = 0;
         lineTest = 0;
         while(1) {
    	    ptrTest = getCurrentLineTxt(ptr, textLineTest);
    	    getValueFromLine(textLineTest, 0, word);
    	    if(strcmp(word, imgNameTest) != 0 && imgNameTest[0] != 0) {
//    	    	strcpy(imgNameTest, word);
    	    	ptrTest = ptr;
    	    	break;
    	    }
    	    if(imgNameTest[0] == 0)
    	    	strcpy(imgNameTest, word);
    	    getValueFromLine(textLineTest, 1, word);
    	    if(strcmp(word, "TYPE") == 0){
    	    	getValueFromLine(textLineTest, 2, imgTypeTest);
    	    }
    	    if(strcmp(word, "DATETOP") == 0){
    	    	getValueFromLine(textLineTest, 2, imgDateTopTest);
    	    }
    	    if(strcmp(word, "NUMBERS") == 0 && lineTest < 10){
    	    	getValueFromLine(textLineTest, 3, imgNumbersTest[lineTest]);
    	    	lineTest++;
    	    }
    	    if(strcmp(word, "PLAYER") == 0){
    	    	getValueFromLine(textLineTest, 2, imgPlayerTest);
    	    }
    	    if(strcmp(word, "DATE") == 0){
    	    	getValueFromLine(textLineTest, 2, imgDateTest);
    	    }
    	    if(ptrTest == NULL) break;
    	    ptr = ptrTest;
         }
// statistic
        sprintf(buffLog," -------------- image= %s --------------------\n\n",  imgNameTest);
        saveLog("", buffLog);

         trimEndOfStr(imgNameOut);trimEndOfStr(imgNameTest);
         if(strcmp(imgNameOut, imgNameTest) != 0) {
             sprintf(buffLog," different image name; out= %s, test= %s,\n",  imgNameOut, imgNameTest);
             saveLog("", buffLog);
         }
         fieldsTotal++;
         trimEndOfStr(imgTypeOut);trimEndOfStr(imgTypeTest);
         if(strcmp(imgTypeOut, imgTypeTest) != 0) {
            sprintf(buffLog," type error;     line= %d,      type= %s, recog= %s,\n",  0, imgTypeTest, imgTypeOut);
            saveLog("", buffLog);
        	typeErrors++;
         }
         trimEndOfStr(imgDateTopOut);trimEndOfStr(imgDateTopTest);
         if(strcmp(imgDateTopOut, imgDateTopTest) != 0) {
            sprintf(buffLog," date top error; line= %d,      dateTop= %s, recog= %s,\n", 0, imgDateTopTest,imgDateTopOut);
            saveLog("", buffLog);
            dateTopErrors++;
         }
         linesTotal += lineTest;
         for(i=0;i<10;i++) {
        	 trimEndOfStr(imgNumbersOut[i]);trimEndOfStr(imgNumbersTest[i]);
             if(strcmp(imgNumbersOut[i], imgNumbersTest[i]) != 0) {
                 sprintf(buffLog," numbers error;  line= %d,      numbers= %s, recog= %s,\n",  i, imgNumbersTest[i], imgNumbersOut[i]);
                 saveLog("", buffLog);
                 numbersErrors++;
             }
         }
         trimEndOfStr(imgPlayerOut);trimEndOfStr(imgPlayerTest);
         if(strcmp(imgPlayerOut, imgPlayerTest) != 0) {
            sprintf(buffLog," player error;   line= %d,      player= %s, recog= %s,\n", 0, imgPlayerTest,imgPlayerOut);
            saveLog("", buffLog);
            playerErrors++;
         }
         trimEndOfStr(imgDateOut);trimEndOfStr(imgDateTest);
         if(strcmp(imgDateOut, imgDateTest) != 0) {
            sprintf(buffLog," date error;     line= %d,      date= %s, recog= %s,\n",  0, imgDateTest,imgDateOut);
            saveLog("", buffLog);
            dateErrors++;
         }

          if(ptrTest == NULL || ptrOut == NULL) break;
       } // while

     { double percent;
        saveLog("------------------ TOTAL ERRORS ------------", "\n");
        percent = ((double)typeErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," type: errors= %d, fields= %d, errorPercent= %2.2f \n",  typeErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        percent = ((double)dateTopErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," date top: errors= %d, fields= %d, errorPercent= %2.2f \n",  dateTopErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        percent = ((double)numbersErrors/(double)linesTotal)*100.0;
        sprintf(buffLog," numbers: errors= %d, lines= %d, errorPercent= %2.2f \n",  numbersErrors, linesTotal, percent);
        saveLog("", buffLog);
        percent = ((double)playerErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," player: errors= %d, fields= %d, errorPercent= %2.2f \n",  playerErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        percent = ((double)dateErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," date: errors= %d, fields= %d, errorPercent= %2.2f \n",  dateErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        saveLog("---------------------------------------------", "\n");

     }

     }

    }


void statsBenchmarkBarCode(void) {
    FILE *f;
    int len, fSize;
   	char fileNameOut[500];
   	char fileNameTest[500];

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
    strcpy(fileNameOut, "../log/outputBcode.csv");
    strcpy(fileNameTest, "../log.TestBcode.csv");
#else
    strcpy(fileNameOut, "c:\\Users\\Wicek\\OCR\\log\\outputBcode.csv");
    strcpy(fileNameTest, "c:\\Users\\Wicek\\OCR\\log\\TestBcode.csv");
#endif

//       strcpy(fileNameOut, "c:\\Users\\Wicek\\OCR\\log\\outputBcode.csv");
//       strcpy(fileNameTest, "c:\\Users\\Wicek\\OCR\\log\\TestBcode.csv");

       f= fopen(fileNameOut, "r");
       if(f == NULL) {
       	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameOut, strerror(errno));
       	return;
       }
   	  fseek(f, 0L,SEEK_END);
   	  len = ftell(f);
   	  fseek(f, 0L,SEEK_SET);
printf(" file= %s, len= %d\n",fileNameOut, len);
  	  fSize = fread(buffOut, len, 1, f);
  	  if(ferror(f) || fSize*len != len){
  		if(ferror(f))
  			printf("read file (\"%s\") read failed: %s\n",fileNameOut, strerror(errno));
  		else
  			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameOut, fSize*len, len);
       }
      fclose (f);


      f= fopen(fileNameTest, "r");
      if(f == NULL) {
      	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameTest, strerror(errno));
      	return;
      }
  	  fseek(f, 0L,SEEK_END);
  	  len = ftell(f);
  	  fseek(f, 0L,SEEK_SET);

  	printf(" file= %s, len= %d\n",fileNameTest, len);
 	  fSize = fread(buffTest, len, 1, f);
 	  if(ferror(f) || fSize*len != len){
 		if(ferror(f))
 			printf("read file (\"%s\") read failed: %s\n",fileNameTest, strerror(errno));
 		else
 			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameTest, fSize*len, len);
      }
     fclose (f);

     {// stats

    	 char *ptrOut = buffOut;
    	 char *ptrTest = buffTest;
      	 char *ptr;
         char word[100];

    	 char textLineOut[1000];
    	 char textLineTest[1000];

         char imgNameOut[100];
         char imgTypeOut[100];


         char imgNameTest[100];
         char imgTypeTest[100];

//         int i;
         int fieldsTotal = 0;
         int typeErrors = 0;
         char buffLog[500];

 saveLog("-------------statistic--------------------------------", "\n");
     while(1) {

// load	 next image
//   out
         ptr = ptrOut;
         imgNameOut[0] = 0;
         imgTypeOut[0] = 0;
         while(1) {
    	    ptrOut = getCurrentLineTxt(ptr, textLineOut);
    	    getValueFromLine(textLineOut, 0, word);
    	    if(strcmp(word, imgNameOut) != 0 && imgNameOut[0] != 0) {
//    	    	strcpy(imgNameOut, word);
    	    	ptrOut = ptr;
    	    	break;
    	    }
    	    if(imgNameOut[0] == 0)
    	    	strcpy(imgNameOut, word);
    	    getValueFromLine(textLineOut, 1, word);
    	    if(strcmp(word, "TYPE") == 0){
    	    	getValueFromLine(textLineOut, 2, imgTypeOut);
    	    }
    	    if(ptrOut == NULL) break;
    	    ptr = ptrOut;
         }

//  test
         ptr = ptrTest;
         imgNameTest[0] = 0;
         imgTypeTest[0] = 0;
         while(1) {
    	    ptrTest = getCurrentLineTxt(ptr, textLineTest);
    	    getValueFromLine(textLineTest, 0, word);
    	    if(strcmp(word, imgNameTest) != 0 && imgNameTest[0] != 0) {
//    	    	strcpy(imgNameTest, word);
    	    	ptrTest = ptr;
    	    	break;
    	    }
    	    if(imgNameTest[0] == 0)
    	    	strcpy(imgNameTest, word);
    	    getValueFromLine(textLineTest, 1, word);
    	    if(strcmp(word, "TYPE") == 0){
    	    	getValueFromLine(textLineTest, 2, imgTypeTest);
    	    }
    	    if(ptrTest == NULL) break;
    	    ptr = ptrTest;
         }
// statistic
//        sprintf(buffLog," -------------- image= %s --------------------\n\n",  imgNameTest);
//        saveLog("", buffLog);

         trimEndOfStr(imgNameOut);trimEndOfStr(imgNameTest);
         if(strcmp(imgNameOut, imgNameTest) != 0) {
             sprintf(buffLog," different image name; out= %s, test= %s,\n",  imgNameOut, imgNameTest);
             saveLog("", buffLog);
         }
         fieldsTotal++;
         trimEndOfStr(imgTypeOut);trimEndOfStr(imgTypeTest);
         if(strcmp(imgTypeOut, imgTypeTest) != 0) {
             sprintf(buffLog," -------------- image= %s --------------------\n\n",  imgNameTest);
             saveLog("", buffLog);
            sprintf(buffLog," type error;     line= %d,      type= %s, recog= %s,\n",  0, imgTypeTest, imgTypeOut);
            saveLog("", buffLog);
        	typeErrors++;
         }
          if(ptrTest == NULL || ptrOut == NULL) break;
       } // while

     { double percent;
        saveLog("------------------ TOTAL ERRORS ------------", "\n");
        percent = ((double)typeErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," type: errors= %d, fields= %d, errorPercent= %2.2f \n",  typeErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        saveLog("---------------------------------------------", "\n");

     }
    }
   }

void statsBenchmarkBarCodeQuality(void) {
    FILE *f;
    int len, fSize;
   	char fileNameOut[500];
   	char fileNameTest[500];

    strcpy(fileNameOut, "c:\\Users\\Wicek\\OCR\\log\\outputBcodeTest_BM.csv");
    strcpy(fileNameTest, "c:\\Users\\Wicek\\OCR\\log\\outputBcodeTest.csv");


    printf(" file= %s \n",fileNameOut);
    printf(" file= %s \n",fileNameTest);
    fflush(stdout);

       f= fopen(fileNameOut, "r");
       if(f == NULL) {
       	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameOut, strerror(errno));
       	return;
       }
   	  fseek(f, 0L,SEEK_END);
   	  len = ftell(f);
   	  fseek(f, 0L,SEEK_SET);
printf(" file= %s, len= %d\n",fileNameOut, len);

  	  fSize = fread(buffOut, len, 1, f);
  	  if(ferror(f) || fSize*len != len){
  		if(ferror(f))
  			printf("read file (\"%s\") read failed: %s\n",fileNameOut, strerror(errno));
  		else
  			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameOut, fSize*len, len);
       }
      fclose (f);


      f= fopen(fileNameTest, "r");
      if(f == NULL) {
      	printf("saveCSVFile (\"%s\") failed: %s\n",fileNameTest, strerror(errno));
      	return;
      }
  	  fseek(f, 0L,SEEK_END);
  	  len = ftell(f);
  	  fseek(f, 0L,SEEK_SET);

  	printf(" file= %s, len= %d\n",fileNameTest, len);
 	  fSize = fread(buffTest, len, 1, f);
 	  if(ferror(f) || fSize*len != len){
 		if(ferror(f))
 			printf("read file (\"%s\") read failed: %s\n",fileNameTest, strerror(errno));
 		else
 			printf("read file (\"%s\") read failed: (size= %d) != (len= %d\n)",fileNameTest, fSize*len, len);
      }
     fclose (f);

     {// stats

    	 char *ptrOut = buffOut;
    	 char *ptrTest = buffTest;
      	 char *ptr;
         char word[100];

    	 char textLineOut[1000];
    	 char textLineTest[1000];

         char imgNameOut[100];
         char imgTypeOut[100];


         char imgNameTest[100];
         char imgTypeTest[100];

//         int i;
         int fieldsTotal = 0;
         int typeErrors = 0;
         char buffLog[500];

 saveLog("-------------statistic--------------------------------", "\n");
     while(1) {

// load	 next image
//   out
         ptr = ptrOut;
         imgNameOut[0] = 0;
         imgTypeOut[0] = 0;
//         while(1) {
    	    ptrOut = getCurrentLineTxt(ptr, textLineOut);
//  	    sprintf(buffLog," ------------textLineOut= %s\n",  textLineOut);
//  	    saveLog("", buffLog);

    	    getValueFromLine(textLineOut, 0, word);
//    sprintf(buffLog," 1------------word= %s,  imgNameOut= %s \n\n",  word, imgNameOut);
//    saveLog("", buffLog);

    	    if(strcmp(word, imgNameOut) != 0 && imgNameOut[0] != 0) {
//    	    	strcpy(imgNameOut, word);
    	    	ptrOut = ptr;
    	    	break;
    	    }
    	    if(imgNameOut[0] == 0)
    	    	strcpy(imgNameOut, word);
//    sprintf(buffLog," 2------------word= %s,  imgNameOut= %s \n\n",  word, imgNameOut);
//    saveLog("", buffLog);
    	    getValueFromLine(textLineOut, 1, imgTypeOut);
//    	    if(strcmp(word, "TYPE") == 0){
//    	    	getValueFromLine(textLineOut, 2, imgTypeOut);
//    	    }
//    	    if(ptrOut == NULL) break;
//    	    ptr = ptrOut;
//         }

//  test
         ptr = ptrTest;
         imgNameTest[0] = 0;
         imgTypeTest[0] = 0;
//         while(1) {
    	    ptrTest = getCurrentLineTxt(ptr, textLineTest);
//  sprintf(buffLog," ------------textLineTest= %s\n",  textLineTest);
//   saveLog("", buffLog);


    	    getValueFromLine(textLineTest, 0, word);
//   sprintf(buffLog," 1------------word= %s,  imgNameTest= %s \n\n",  word, imgNameTest);
//   saveLog("", buffLog);
    	    if(strcmp(word, imgNameTest) != 0 && imgNameTest[0] != 0) {
//    	    	strcpy(imgNameTest, word);
    	    	ptrTest = ptr;
    	    	break;
    	    }
    	    if(imgNameTest[0] == 0)
    	    	strcpy(imgNameTest, word);
//   sprintf(buffLog," 2------------word= %s,  imgNameTest= %s \n\n",  word, imgNameTest);
//   saveLog("", buffLog);
    	    getValueFromLine(textLineTest, 1, imgTypeTest);
//    	    if(strcmp(word, "TYPE") == 0){
//    	    	getValueFromLine(textLineTest, 2, imgTypeTest);
//    	    }
//    	    if(ptrTest == NULL) break;
//    	    ptr = ptrTest;
//         }
// statistic
//        sprintf(buffLog," -------------- image= %s --------------------\n\n",  imgNameTest);
//        saveLog("", buffLog);

         trimEndOfStr(imgNameOut);trimEndOfStr(imgNameTest);
         if(strcmp(imgNameOut, imgNameTest) != 0) {
             sprintf(buffLog," different image name; out= %s, test= %s,\n",  imgNameOut, imgNameTest);
             saveLog("", buffLog);
         }
         fieldsTotal++;
         trimEndOfStr(imgTypeOut);trimEndOfStr(imgTypeTest);

         if((strcmp(imgTypeOut, "any") != 0 && strcmp(imgTypeOut, imgTypeTest) != 0) || strcmp(imgTypeTest, "???") == 0) {
             sprintf(buffLog," -------------- image= %s --------------------\n",  imgNameTest);
             saveLog("", buffLog);
            sprintf(buffLog," type error;       type= %s, recog= %s,\n\n",  imgTypeOut, imgTypeTest );
            saveLog("", buffLog);
        	typeErrors++;
         }
          if(ptrTest == NULL || ptrOut == NULL) break;
       } // while

     { double percent;
        saveLog("------------------ TOTAL ERRORS ------------", "\n");
        percent = ((double)typeErrors/(double)fieldsTotal)*100.0;
        sprintf(buffLog," type: errors= %d, fields= %d, errorPercent= %2.2f \n",  typeErrors, fieldsTotal, percent);
        saveLog("", buffLog);
        saveLog("---------------------------------------------", "\n");

     }
    }
   }

void printResultsToTestTrainFileBCode(char *imgName, char *typeStr){
	int  len;
	char buff[2000];
	char buffimgName[100];

	strcpy(buffimgName, imgName);
	buffimgName[strlen(buffimgName)-4] = 0;
	sprintf(buff,"%s,TYPE,",buffimgName);
    len = strlen(buff);
    sprintf(&buff[len],"%s\n",typeStr);
    {
    FILE *f;
   	char fileName[500];
#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__	|| LINUX || linux || UNIX)
    strcpy(fileName, "../log/outputBcode.csv");
#else
    strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\outputBcode.csv");
#endif



     f= fopen(fileName, "a");
     if(f == NULL) {
     	printf("saveCSVFile (\"%s\") failed: %s\n",fileName, strerror(errno));
     	return;
     }

     fprintf (f, "%s", buff);
     fclose (f);
    }
}

//typedef struct tagRGBA {
//	BYTE	   R;
//	BYTE	   G;
//	BYTE	   B;
//	BYTE	   A;
//} RGBA ,*LPRGBA;

void getRGBAcolor (LPIMGDATA img, int xn,int yn, LPRGBA rgba) {
	        int  w;
	        long nY;
	        long nX;
	        int sh= 4;
	        LPBYTE ptr;

	            w=img->bi.biWidth*sh;
	            nY=yn*w;
	       	    nX = (long)xn*sh;
	            ptr=img->pixPtr+nY+nX;
	            rgba->B = *ptr;
	            rgba->G = *(ptr+1);
	            rgba->R = *(ptr+2);
	            rgba->A = *(ptr+3);

	    }

void testRGBAcolors (LPIMGDATA img) {
	   RGBA rgba;
		int y, x;
//		for(i=0;i<img->bi.biHeight;i++){
		for(y=150;y<151;y++){
//			for(k=0;k<img->bi.biWidth;k++){
				for(x=0;x<100;x++){
				 getRGBAcolor (img, x, y, &rgba);
printf("A= %d, R= %d, G= %d, B=%d \n", rgba.A, rgba.R, rgba.G, rgba.B);
			}
		}

}

void addInfoToTestFile(LPIMGDATA img, int testBaraCodeInt){

	int  len;
	char buff[2000];
	char buffimgName[100];

	strcpy(buffimgName, img->szIMGName);
	buffimgName[strlen(buffimgName)-4] = 0;
	sprintf(buff,"%s,",buffimgName);
    len = strlen(buff);
    if(testBaraCodeInt > 0)
        sprintf(&buff[len],"good\n");
    else{
   	    if(testBaraCodeInt < 0)
    	    sprintf(&buff[len],"bad\n");
   	    else
    	    sprintf(&buff[len],"???\n");
    }
    {
    FILE *f;
   	char fileName[500];
    strcpy(fileName, "c:\\Users\\Wicek\\OCR\\log\\outputBcodeTest.csv");

     f= fopen(fileName, "a");
     if(f == NULL) {
     	printf("saveCSVFile (\"%s\") failed: %s\n",fileName, strerror(errno));
     	return;
     }

     fprintf (f, "%s", buff);
     fclose (f);
    }
}

int LoadFile (LPBYTE *jpgBytes, char *fileName){
	FILE *f;
	int len;
//	LPBYTE ptr;
	size_t fSize = 0;

	errno = 0;
//	char bufflog[500];

	strcpy(&fileName[strlen(fileName)-3], "jpg");

	printf("open file %s\n",fileName);
fflush(stdout);
	f= fopen(fileName, "r");
	if(f == NULL) {
		printf("LoadImage (\"%s\") failed: %s\n",fileName, strerror(errno));
		return 0;
	}

	fseek(f, 0L,SEEK_END);
	if(ferror(f)){
		printf("LoadImage (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
		return 0;
	}
	//    printf("ftell %s\n",fileName);

	len = ftell(f);
//	   printf("ftell len= %n\n",len);

	fseek(f, 0L,SEEK_SET);
    printf("file %s length= %d\n",fileName, len);

 fflush(stdout);

    *jpgBytes = yl_calloc( "BMP_A", len, sizeof(BYTE));
	if(*jpgBytes == NULL) {
		printf("yl_calloc failed:loadFile (jpgBytes)\n");
		return 0;
	}

	fSize = fread(*jpgBytes, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
			printf("LoadImage (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
			printf("LoadImage (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
		return 0;
	}
	close_file(f);

printf("file %s close_file()\n",fileName);
 fflush(stdout);
	return len;

}

void saveBytebuffer (LPBYTE buff, int len, char* name, char* path) {
	FILE *f;
	errno = 0;
	char fileName[500];
	size_t fSize = 0;

	if (!name) return;

	strcpy(fileName, path);
	strcat(fileName, name);


//	printf("fileName= %s\n",fileName);
//	fflush(stdout);


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

LPBYTE readBytebuffer (char* name, char* path, int *size) {

	FILE *f;
	int len;
	errno = 0;
	char fileName[500];
	LPBYTE ptr;
	size_t fSize = 0;

	if (!name) return NULL;

	strcpy(fileName, path);
	strcat(fileName, name);


//	char bufflog[500];


	//	printf("open file %s\n",fileName);

	f= fopen(fileName, "r");
	if(f == NULL) {
//    	fprintf(stderr,"readBytebuffer (\"%s\") failed: %s\n",fileName, strerror(errno));
		return  NULL;
	}

	fseek(f, 0L,SEEK_END);
	//    printf("fseek %s\n",fileName);

	if(ferror(f)){
		fprintf(stderr,"LoadImage (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
		return  NULL;
	}
	//    printf("ftell %s\n",fileName);

	len = ftell(f);

	fseek(f, 0L,SEEK_SET);
	*size = len;

//	   printf("file %s length= %d\n",fileName, len);

	ptr = yl_calloc( "BMP_A", len, sizeof(BYTE));
	if(ptr == NULL) {
		printf("yl_calloc readBytebuffer error\n");
		return  NULL;
	}
	fSize = fread((char *)ptr, len, 1, f);
	if(ferror(f) || fSize*len != len){
		if(ferror(f))
		     	fprintf(stderr,"LoadImage (\"%s\") read failed: %s\n",fileName, strerror(errno));
		else
	        fprintf(stderr,"LoadImage (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);

		yl_free(ptr);
		return NULL;
	}
	close_file(f);
	return ptr;
}
int getInputLotteryTypeTX(int idx){
	int type = 0;
	if(idx < 200) return 1; // Power Ball
	if(idx < 400) return 2; // Mega Millions
	if(idx < 500) return 3; // Lotto Texas
	if(idx < 550) return 4; // Texas Two Step
	if(idx < 600) return 5; // Pick 3
	if(idx < 650) return 6; // Daily 4
	if(idx < 700) return 7; // Cash 5
	if(idx < 750) return 8; // All Or Nothing

    return type;
}
int getInputLotteryType(int state, int idx){
    int type = 0;

    switch(state){
    case 1: type = getInputLotteryTypeTX(idx); break;

    }
    return type;
}


int main_2(int argc, char **argv) {
   unsigned char * buff = NULL;
   int sizeBuff;

   char globalPath[500];
   char szIMGPath[500];
   char szIMGName[100];
   char  error[500];

   int ret = 0;
   int US_state = 0;
   int lotteryType = 0;
   int resultslength = 4096;
   char results[resultslength];
   int ok;

printf("Start program...\n");

    strcpy(szIMGName,"IMG_TEST_84.jpg");

#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
    strcpy(globalPath, "../");
    strcpy(szIMGPath,"../input/");
#else
    strcpy(globalPath, "c:\\Users\\Wicek\\OCR\\");
    strcpy(szIMGPath,"c:\\Users\\Wicek\\OCR\\input\\");
#endif


             buff = readBytebuffer (szIMGName,szIMGPath, &sizeBuff);
             if (buff == NULL || sizeBuff <= 0) {
		         if (buff == NULL)
		             printf("buff = null \n");
   	             if (sizeBuff <= 0)
		            printf("-------sizeBuff = %d\n", sizeBuff);
                      ok = 0;
                }
	        else {
    	              printf(" reading successful \n");
                      ok = 1;
	         }

               if(ok) {
                 printf("loaded image= %s from path= %s\n", szIMGName,szIMGPath );
    	        US_state = 1;
    	        lotteryType = 1;
      	        error[0] = 0;
   	        ret = OCR_Call_server(buff, sizeBuff, globalPath, lotteryType,
                                      US_state, results, resultslength, error, szIMGName);
   	        printf("ret= %d\n",ret);
 	        if(buff != NULL){
 	    	    yl_free(buff);buff = NULL;
 	         }
 	         if(ret == 77) {
 	 		printf("imgName= %s, image rejected\n",szIMGName);
 	         }
            	 printf("---------------results:\n  %s\n",results);

                 fflush(stdout);
              }
           return 0;

}
