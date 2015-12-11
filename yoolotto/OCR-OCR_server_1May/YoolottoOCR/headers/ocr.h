#ifndef OCR_H_
#define OCR_H_

#include "../headers/Image.h"
#include "../headers/Prototype.h"

#define LOTTO_TYPES_NUM_TX 9

typedef struct                              // structure for output compressed  image
{
	unsigned char *ptrBuffer;              // pointer to buffer with images; NULL value at input
	int singleSize[3]; 					   // every image size
    int totalSize;                         // size of the buffer in bytes; 0 value at input
} COMPRESSED_IMGS;

typedef struct                              /* structure for output compressed  image    */
{
	unsigned char *ptrBuffer;              /* pointer to buffer with image; NULL value at input     */
	int size; 							   /* buffer size*/
} COMPRESSED_IMG;

int US_State;

char *typeOfLotteryTXT[LOTTO_TYPES_NUM_TX];
char *prototypeNameTXT[LOTTO_TYPES_NUM_TX];


int OCR_call(LPIMGDATA img, int byteSteps, char *mainPath, LPTICKETRESULTS lpTicketResults,
		            char *error, char *imgName);
int OCR_Call_server(unsigned char *ptrJPG, int length, char *mainPath,	int lotteryType, int US_state, char *results, int resultslength,
		            char *error, char *imgName);

void addImagesToTheOutput(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut);
int saveSlices(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut, char *dirName);

void FreeTicketResultsMemory_Call(LPTICKETRESULTS lpTicketResults, LPIMGDATA imgOut);

//int OCR_Call_new(unsigned char *ptr, int imgWidth, int imgHeight, char *buffType, int byteSteps, int outimages,
//		LPIMGDATA imgOut, char *mainPath,	LPTICKETRESULTS lpTicketResults, char *error, char *imgName, int US_state);

int Scratch_Off_Call(unsigned char *ptr, int imgWidth, int imgHeight, char *buffType, int byteSteps, int outimages,
		LPIMGDATA imgOut, char *mainPath,	LPTICKETRESULTS lpTicketResults, char *error, char *imgName, int US_state);

void GetMonochromaticImage_Call(LPIMGDATA imgOut, BYTE **ptr, int *size );

//int getCMPImagesForOCR(unsigned char *ptrInp, int imgWidth, int imgHeight, char *buffType, COMPRESSED_IMG *compressedImgBuffer, int *imgQuality,
//		char *error, char *mainPath, char *imgName, int US_state);
int getCMPImagesForOCR(unsigned char *ptrInp, int imgWidth, int imgHeight, char *buffType,
		char *error, char *mainPath, char *imgName, int US_state);


int compressMonoImages(LPIMGDATA *img, COMPRESSED_IMGS *outImgBuffer);
int compressMixedImages(LPIMGDATA img, COMPRESSED_IMGS *outImgBuffer);

//int decompressMonoImages(COMPRESSED_IMG *inpImgBuffer, LPIMGDATA *img);
int decompressMonoImages(LPBYTE inpImg, LPIMGDATA *img);
void testCompressImages(COMPRESSED_IMGS *outImgBuffer, char *iName);
void freeMemoryCMPImages(COMPRESSED_IMGS *outImgBuffer);
//void freeMemoryCMPImage(COMPRESSED_IMG *outImgBuffer);
void freeMemoryCMPImage(void);

int Huffman_Compress( unsigned char *in, unsigned char *out,
  unsigned int insize );
void Huffman_Uncompress( unsigned char *in, unsigned char *out,
  unsigned int insize, unsigned int outsize );




int barcodeTest_Quality(LPIMGDATA *imgInp);
//int ImageTest_Quality(LPIMGDATA img,  int byteSteps, int US_state);
int ImageTest_Quality(LPIMGDATA img,  int byteSteps, LPIMGDATA imgOut);
int createSmallColorImage(LPIMGDATA img, LPIMGDATA imgOut, int pixelStepsX, int pixelStepsY);
//int compressImage_JPG(LPIMGDATA imgInp, LPBYTE *jpgOut, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR);
int compressImage_JPG(LPIMGDATA imgInp, LPBYTE *jpgOut, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR);
int deCompressImage_JPG(LPBYTE jpgIn, LPIMGDATA imgInp);
void saveJPGbuffer (LPBYTE buff, int len, char* name, char* path);
void setNewImageNameJPG(char *imgName, char *imgNInp, int loop);
void SaveJpgFile(LPIMGDATA img, char* szJpgFileNameOut);
int createJpgImage(LPIMGDATA img, LPBYTE *jpgOut);
BYTE *returnPtrToCompressedBuffer();
int returnSizeOfCompressedBuffer();
int returnImageQualityValue();
void getTypeStrForPROT(char *typeStr, int n);
void getPROTName(char *typeStr, char *prototypeName);
void getLottoType(char *typeStr, int n);


#endif
