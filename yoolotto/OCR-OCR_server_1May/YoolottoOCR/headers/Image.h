/*
 * Image.h
 *
 *  Created on: Jul 26, 2012
 *      Author: Wicek
 */

#ifndef IMAGE_H_
#define IMAGE_H_
#include  "../headers/wintypes.h"
#define MAX_WIDTH 3500
#define MAX_HEIGHT 3500
//#define HEADER_LEN 54
#define LEFT_BYTE 255
#define RIGHT_BYTE 255
#define LEFT_BIT 0x80
#define BLUE8 0xE0
#define GREEN8 0x1C
#define RED8 0x7

#define BFTYPEOFF 0
#define BFSIZEOFF 2
#define BFRESERVED1OFF 6
#define BFRESERVED2OFF 8
#define BFOOFBITSOFF 10

#define BISIZEOFF 0
#define BIWIDTHOFF 4
#define BIHEIGHTOFF 8
#define BIPLANESOFF 12
#define BIBITCOUNTOFF 14
#define BICOMPRESSIONOFF 16
#define BISIZEIMAGEOFF 20
#define BIXPELSPERMETEROFF 24
#define BIYPELSPERMETEROFF 28
#define BICLRUSEDOFF 32
#define BICLRIMPORTANTOFF 36




typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef struct tagBITMAPFILEHEADERW {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADERW, *LPBITMAPFILEHEADERW;

typedef struct tagBITMAPFILEHEADER {
	BYTE	bfType[2];
	BYTE	bfSize[4];
	BYTE	bfReserved1[2];
	BYTE	bfReserved2[2];
	BYTE	bfOffBits[4];
} BITMAPFILEHEADER,*LPBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADERW {
  DWORD biSize;
  DWORD  biWidth;
  DWORD  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  DWORD  biXPelsPerMeter;
  DWORD  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADERW, *LPBITMAPINFOHEADERW;

typedef struct tagBITMAPINFOHEADER{
	BYTE	biSize[4];
	BYTE	biWidth[4];
	BYTE	biHeight[4];
	BYTE	biPlanes[2];
	BYTE	biBitCount[2];
	BYTE	biCompression[4];
	BYTE	biSizeImage[4];
	BYTE	biXPelsPerMeter[4];
	BYTE	biYPelsPerMeter[4];
	BYTE	biClrUsed[4];
	BYTE	biClrImportant[4];
} BITMAPINFOHEADER,*LPBITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER_NUM {
	int	    bfType;
	unsigned long	bfSize;
	int	    bfReserved1;
	int	    bfReserved2;
	unsigned long	bfOffBits;
} BITMAPFILEHEADER_NUM ,*LPBITMAPFILEHEADER_NUM ;

typedef struct tagBITMAPINFOHEADER_NUM {
	unsigned long biSize;
	int	    biWidth;
	int	    biWidthPad;
	int  	biHeight;
	int  	biPlanes;
	int 	biBitCount;
	int 	biCompression;
	unsigned long	biSizeImage;
	int 	biXPelsPerMeter;
	int 	biYPelsPerMeter;
	int 	biClrUsed;
	int 	biClrImportant;
} BITMAPINFOHEADER_NUM ,*LPBITMAPINFOHEADER_NUM ;


typedef struct tagIMGDATA {

    char        szIMGPath[200];    // path to image files
    char        szFilePrototypePath[200];     // path to Prototype files
    char        szLibPath[200];     // path to OCR Lib files
    char        szInpPath[200];    // path to input directory
    char        szOutPath[200];     // path to output files
    char        szWorkPath[200];
    BITMAPFILEHEADER_NUM bf;
    BITMAPINFOHEADER_NUM bi;
    char        error[200];


    int        dpiX;                 //  Img's DPI on X axis
    int        dpiY;                 //  Img's DPI on Y axis
	int	       width;               // Img's width
	int  	   height;              // Img's height



    LPBYTE      ptr;                // pointer to bmp
    LPBYTE      pixPtr;              // pointer to pixels
    LPBYTE      colorPtr;              // pointer to colors

    int         iAngle;             // global Img's angle
    int        wFlip;              // actual orient.(1=180,2=270,3=90)

    char        szIMGName[50];     // current image file name
//    char        szFDFName[13];      // extracted FDF-name
    int        wConfidence[2];     // Img's confidences
} IMGDATA, *LPIMGDATA;

typedef struct tagRGBA {
	BYTE	   R;
	BYTE	   G;
	BYTE	   B;
	BYTE	   A;
} RGBA ,*LPRGBA;


typedef struct tagFIMAGE {
  int height;
  int width;
  float *pixels;
} FImage, *LPFImage;

typedef struct tagPIXEL {
  int x;
  int y;
} PIXEL, *LPPIXEL;

typedef struct tagPIXELW {
  int x;
  int y;
  int w;
} PIXELW, *LPPIXELW;

struct PIXELS_STRUCT {
  PIXEL p;
  struct PIXELS_STRUCT* next;
};
#define PIXELS struct PIXELS_STRUCT
typedef PIXELS * LPPIXELS;
/*
struct PIXELSLIST_STRUCT {
  LPPIXELS pList;
  int size;
  struct PIXELSLIST_STRUCT* next;
};
#define PIXELSLIST struct PIXELSLIST_STRUCT
typedef PIXELSLIST * LPPIXELSLIST;

struct PIXELSLIST2_STRUCT {
  LPPIXEL lpPixel;
  int size;
  struct PIXELSLIST2_STRUCT* next;
};
#define PIXELSLIST2 struct PIXELSLIST2_STRUCT
typedef PIXELSLIST2 * LPPIXELSLIST2;
*/
struct PIXELSLIST_STRUCT {
  LPPIXEL lpPixel;
  int size;
};
#define PIXELSLIST struct PIXELSLIST_STRUCT
typedef PIXELSLIST * LPPIXELSLIST;

struct NODE_STRUCT {
	int rank;
	PIXEL parent;
};
#define NODE struct NODE_STRUCT
typedef NODE * LPNODE;

typedef struct tagLETT_RECT {
  float x;
  float y;
  float width;
  float height;
} LETT_RECT, *LPLETT_RECT;
typedef struct tagROTATED_RECT {
  float cx;
  float cy;
  float width;
  float height;
  float rotation;
} ROTATED_RECT, *LPROTATED_RECT;


struct LETTER_STRUCT {
  LPPIXEL lpPixel;
  float averageBrightness;
  PIXEL centroid;
  float medianStrokeWidth;
  LETT_RECT regularBoundingBox;
  int size;
};
#define LETTER struct LETTER_STRUCT
typedef LETTER * LPLETTER;

struct LETTERPAIR_STRUCT {
	LETTER lett1;
	LETTER lett2;
};
#define LETTERPAIR struct LETTERPAIR_STRUCT
typedef LETTERPAIR * LPLETTERPAIR;

typedef struct tagCALIPER {
  LPPIXEL convexHull;
  int size;
  int pointIndex;
  double currentAngle;
} CALIPER, *LPCALIPER;

struct NODEPAIR_STRUCT {
	int rank;
	int parent;
};
#define NODEPAIR struct NODEPAIR_STRUCT
typedef NODEPAIR * LPNODEPAIR;

struct LINECHAIN_STRUCT {
	LPLETTERPAIR p;
	int size;
};
#define LINECHAIN struct LINECHAIN_STRUCT
typedef LINECHAIN * LPLINECHAIN;

struct LINELETTERS_STRUCT {
	LPLETTER lpL;
	int size;
};
#define LINELETTERS struct LINELETTERS_STRUCT
typedef LINELETTERS * LPLINELETTERS;
struct WORDCAND_STRUCT {
	LPLETTER lpL;
	int size;
	LETT_RECT regularBoundingBox;
};
#define WORDCAND struct WORDCAND_STRUCT
typedef WORDCAND * LPWORDCAND;

#endif /* IMAGE_H_ */
