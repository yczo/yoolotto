/*
 * jpgEncoder.c
 *
 *  Created on: Aug 19, 2014
 *      Author: Wicek
 */



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include  "../headers/Image.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
//#include  "../headers/Functions.h"
//#include  "../headers/ocr.h"


#define MAXJSAMPLE	255
#define CENTERJSAMPLE	128

#define SCALEBITS	16	/* speediest right-shift on some machines */
#define CBCR_OFFSET	((INT32) CENTERJSAMPLE << SCALEBITS)
#define ONE_HALF	((INT32) 1 << (SCALEBITS-1))
#define FIX(x)		((INT32) ((x) * (1L<<SCALEBITS) + 0.5))

#define R_Y_OFF		0			/* offset to R => Y section */
#define G_Y_OFF		(1*(MAXJSAMPLE+1))	/* offset to G => Y section */
#define B_Y_OFF		(2*(MAXJSAMPLE+1))	/* etc. */

#define R_CB_OFF	(3*(MAXJSAMPLE+1))
#define G_CB_OFF	(4*(MAXJSAMPLE+1))
#define B_CB_OFF	(5*(MAXJSAMPLE+1))

#define R_CR_OFF	B_CB_OFF		/* B=>Cb, R=>Cr are the same */
#define G_CR_OFF	(6*(MAXJSAMPLE+1))
#define B_CR_OFF	(7*(MAXJSAMPLE+1))

#define TABLE_SIZE	(8*(MAXJSAMPLE+1))
#define QUANT_LEVEL 50

#define SBYTE signed char
#define SDWORD signed long int
#define SWORD signed short int


void getColorRGBA (LPIMGDATA img, int x,int y, LPBYTE R, LPBYTE G, LPBYTE B);
int ColorSpaceConversion_RGBA_JPG(LPIMGDATA imgInp);
int ColorSpaceConversion_RGBA_JPG_2(LPIMGDATA imgInp);
void DownSampling(int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR);
int DCT_Quant_Encode(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR,
		int quantLevel, LPBYTE *jpgOut);
int DCT_Quant_Decode(LPBYTE jpgIn, int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR, int quantLevel);
void printCodeBits(BYTE *code, int len);
void printCodeBitsFrom(BYTE *code, int bitFrom, int len);
int close_file(FILE *f);
void getColor24(LPIMGDATA img, int xn, int yn, LPBYTE R, LPBYTE G, LPBYTE B);
void setColor24(LPIMGDATA img, int xn, int yn, BYTE R, BYTE G, BYTE B);
int ColorSpaceConversion_JPG_BMP(LPIMGDATA imgOut, int num_rows, int num_cols);


void printBlocksFrom_RGBA(LPIMGDATA imgInp, int rowInp, int colInp);
void printBlocksFrom_RGB(LPIMGDATA imgInp, int rowInp, int colInp);
void printBlocksFrom_JPG(int num_rows, int num_cols, int rowInp, int colInp);
void printOneBlocksFrom_JPG(BYTE *outptr, int num_rows, int num_cols, int rowInp, int colInp, char *str);
int UpSampling(int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR);

void createQuantLTest(int *Q, int quantLevel);
void createQuantChTest(int *Q, int quantLevel);
void getDCcodeClasic(int a, int *lenb, DWORD *b, int isLum);
void getACcodeClasic(int r, int a, int *lenb, DWORD *b, int isLum);
int DCT_Quant_EncodeClasic(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR,
		int quantLevel, LPBYTE *jpgOut);
//int reloadCodeBits_buff(DWORD *b, int bLen,  BYTE *code, int numBits, BYTE *byteBuff, int *bitInBuff, int buffLen);
int reloadCodeBits_buff(DWORD *b, int bLen,  SBYTE *code, int numBits, BYTE *byteBuff, int *bitInBuff, int buffLen);
void DownSamplingAll(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR);

void setHufTbls();
//void saveLogFile(char *str1, char *str2, char *fName);
void printCodeBitsToBuff(BYTE *code, int len, char *buff);
void printCodeBitsToBuff_0X(BYTE *code, int len, char *buff);

SBYTE *outptr0, *outptr1, *outptr2;

static SDWORD YRtab[256],YGtab[256],YBtab[256];
static SDWORD CbRtab[256],CbGtab[256],CbBtab[256];
static SDWORD CrRtab[256],CrGtab[256],CrBtab[256];

#define  Y(R,G,B) ((BYTE)( (YRtab[(R)]+YGtab[(G)]+YBtab[(B)])>>16 ) - 128)
#define Cb(R,G,B) ((BYTE)( (CbRtab[(R)]+CbGtab[(G)]+CbBtab[(B)])>>16 ) )
#define Cr(R,G,B) ((BYTE)( (CrRtab[(R)]+CrGtab[(G)]+CrBtab[(B)])>>16 ) )

//static float fdtbl_Y[64];
//static float fdtbl_Cb[64]; //the same with the fdtbl_Cr[64]



static BYTE zigzagM[64]={ 0, 1, 5, 6,14,15,27,28,
		  2, 4, 7,13,16,26,29,42,
		  3, 8,12,17,25,30,41,43,
		  9,11,18,24,31,40,44,53,
		 10,19,23,32,39,45,52,54,
		 20,22,33,38,46,51,55,60,
		 21,34,37,47,50,56,59,61,
		 35,36,48,49,57,58,62,63 };

static BYTE zigzagInv[64]=
        {
       		0, 1, 8, 16, 9, 2, 3, 10,
       		17, 24, 32, 25, 18, 11, 4, 5,
       		12, 19, 26, 33, 40, 48, 41, 34,
       		27, 20, 13, 6, 7, 14, 21, 28,
       		35, 42, 49, 56, 57, 50, 43, 36,
       		29, 22, 15, 23, 30, 37, 44, 51,
       		58, 59, 52, 45, 38, 31, 39, 46,
       		53, 60, 61, 54, 47, 55, 62, 63};

int  getzigzagIdx(int i){
	int j;
	for (j=0;j<64;j++){
		if(zigzagM[j] == i) return j;
	}
	return -1;
}
void printzigzagInverse(){
	int i, n;
	char buff[500];
	char buff2[500];
	buff[0] = 0;
	for (i=0;i<64;i++){
		n = getzigzagIdx(i);
	    sprintf(buff2,"%d, ",n);
	    strcat(buff, buff2);
	    if(i > 0 && i%8==0)
	    	strcat(buff, "\n");
	}
	strcat(buff, "\n");
	printf(buff);
}
//------------------------------------------------
int compressImage_JPG(LPIMGDATA imgInp, LPBYTE *jpgOut, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR){

//	 Color space conversion ( RGB to YCbCr).
  int num_rows = imgInp->height;
  int num_cols = imgInp->width;
  int byteSize = imgInp->width*imgInp->height;
  int ret;

  byteSize = 5000000;

outptr0 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
outptr1 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
outptr2 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
if(outptr0 == NULL || outptr1 == NULL || outptr2 == NULL) return -100;

printf(" byteSize= %d \n", byteSize);
fflush(stdout);

//printf(" encode: num_rows= %d, num_cols= %d, downSize_VCB= %d, downSize_HCB= %d, downSize_VCR= %d, downSize_HCR= %d  \n", num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR);

// printBlocksFrom_RGBA(imgInp, 0, 0);

 printBlocksFrom_RGB(imgInp, 0, 0);

//ret = ColorSpaceConversion_RGBA_JPG(imgInp);
ret = ColorSpaceConversion_RGBA_JPG_2(imgInp);

//printBlocksFrom_JPG(num_rows, num_cols, 332, 172);

//printf(" ret= %d \n", ret);
//fflush(stdout);

//DownSampling(num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);
DownSamplingAll(num_rows, num_cols, downSize_VL, downSize_HL, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);

//printf(" DownSampling--------- \n");
//printBlocksFrom_JPG(num_rows, num_cols);

ret = DCT_Quant_Encode(num_rows, num_cols, downSize_VL, downSize_HL, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR, QUANT_LEVEL, jpgOut);
//ret = DCT_Quant_EncodeClasic(num_rows, num_cols, downSize_VL, downSize_HL, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR, QUANT_LEVEL, jpgOut);

printf(" ret= %d \n", ret);
fflush(stdout);

yl_free(outptr0);
printf(" yl_free(outptr0); \n");
fflush(stdout);

yl_free(outptr1);
printf(" yl_free(outptr1); \n");
fflush(stdout);

//yl_free(outptr2);
printf(" yl_free(outptr2); \n");
fflush(stdout);

return ret;

}
void precalculate_YCbCr_tables_2()
{
 WORD R,G,B;
 for (R=0;R<=255;R++) {YRtab[R]=(SDWORD)(65536*0.299+0.5)*R;
			   CbRtab[R]=(SDWORD)(65536*-0.16874+0.5)*R;
			   CrRtab[R]=(SDWORD)(32768)*R;
			  }
 for (G=0;G<=255;G++) {YGtab[G]=(SDWORD)(65536*0.587+0.5)*G;
			   CbGtab[G]=(SDWORD)(65536*-0.33126+0.5)*G;
			   CrGtab[G]=(SDWORD)(65536*-0.41869+0.5)*G;
			  }
 for (B=0;B<=255;B++) {YBtab[B]=(SDWORD)(65536*0.114+0.5)*B;
			   CbBtab[B]=(SDWORD)(32768)*B;
			   CrBtab[B]=(SDWORD)(65536*-0.08131+0.5)*B;
			  }
}
int ColorSpaceConversion_RGBA_JPG_2(LPIMGDATA imgInp){

	int idx;

	 precalculate_YCbCr_tables_2();

	  BYTE r, g, b;
	  LPBYTE inptr;
//	  LPBYTE outptr0, outptr1, outptr2;
	  int row, col;
	  int num_rows = imgInp->height;
	  int num_cols = imgInp->width;
	  inptr = imgInp->pixPtr;
//	  while (--num_rows >= 0) {
	  switch(imgInp->bi.biBitCount){
	  case 24:
	      for (row = 0; row < num_rows; row++) {
		    for (col = 0; col < num_cols; col++) {
		    	getColor24(imgInp, col, row, &r, &g, &b);
		   	  idx = row*num_cols+col;

		   	outptr0[idx]=(SBYTE)Y(r,g,b);
		   	outptr1[idx]=(SBYTE)Cb(r,g,b);
		   	outptr2[idx]=(SBYTE)Cr(r,g,b);
		    }
		  }

	      break;

	  case 32:
      for (row = 0; row < num_rows; row++) {
	    for (col = 0; col < num_cols; col++) {
	    	getColorRGBA(imgInp, col, row, &r, &g, &b);
	   	    idx = row*num_cols+col;

		   	outptr0[idx]=(BYTE)Y(r,g,b);
		   	outptr1[idx]=(BYTE)Cb(r,g,b);
		   	outptr2[idx]=(BYTE)Cr(r,g,b);

	    }
	  }

      break;
    }
  return 0;

}
void fdct_2(SBYTE *data,float *datafloat)
{
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp10, tmp11, tmp12, tmp13;
  float z1, z2, z3, z4, z5, z11, z13;
  float *dataptr;
//  float datafloat[64];
//  float temp;
  SBYTE ctr;
  BYTE i;
  for (i=0;i<64;i++) datafloat[i]=data[i];

  // Pass 1: process rows.
  dataptr=datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
	tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];

	// Even part

    tmp10 = tmp0 + tmp3;	// phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[4] = tmp10 - tmp11;

    z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
	dataptr[2] = tmp13 + z1;	// phase 5
    dataptr[6] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

	// The rotator is modified from fig 4-8 to avoid extra negations
    z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	// phase 6
    dataptr[3] = z13 - z2;
	dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += 8;		//advance pointer to next row
  }

  // Pass 2: process columns

  dataptr = datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
    tmp0 = dataptr[0] + dataptr[56];
    tmp7 = dataptr[0] - dataptr[56];
    tmp1 = dataptr[8] + dataptr[48];
    tmp6 = dataptr[8] - dataptr[48];
    tmp2 = dataptr[16] + dataptr[40];
    tmp5 = dataptr[16] - dataptr[40];
    tmp3 = dataptr[24] + dataptr[32];
    tmp4 = dataptr[24] - dataptr[32];

    //Even part/

    tmp10 = tmp0 + tmp3;	//phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[32] = tmp10 - tmp11;

	z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
    dataptr[16] = tmp13 + z1; // phase 5
    dataptr[48] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    // The rotator is modified from fig 4-8 to avoid extra negations.
	z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;
    dataptr[40] = z13 + z2; // phase 6
	dataptr[24] = z13 - z2;
    dataptr[8] = z11 + z4;
    dataptr[56] = z11 - z4;

    dataptr++;			// advance pointer to next column
  }

/*
// Quantize/descale the coefficients, and store into output array
 for (i = 0; i < 64; i++) {
 // Apply the quantization and scaling factor
			   temp = datafloat[i] * fdtbl[i];

   //Round to nearest integer.
   //Since C does not specify the direction of rounding for negative
   //quotients, we have to force the dividend positive for portability.
   //The maximum coefficient size is +-16K (for 12-bit data), so this
   //code should work for either 16-bit or 32-bit ints.

	   outdata[i] = (SWORD) ((SWORD)(temp + 16384.5) - 16384);
			  }
*/
}
void quantDCT_2(float *datafloat, float *fdtbl, int *outdata){
	  float temp;
	  int i;
	// Quantize/descale the coefficients, and store into output array
	 for (i = 0; i < 64; i++) {
	 // Apply the quantization and scaling factor
				   temp = datafloat[i] * fdtbl[i];

	   //Round to nearest integer.
	   //Since C does not specify the direction of rounding for negative
	   //quotients, we have to force the dividend positive for portability.
	   //The maximum coefficient size is +-16K (for 12-bit data), so this
	   //code should work for either 16-bit or 32-bit ints.

		   outdata[i] = (SWORD) ((SWORD)(temp + 16384.5) - 16384);
	  }

}
/*
void prepare_quant_tables_1()
{
 double aanscalefactor[8] = {1.0, 1.387039845, 1.306562965, 1.175875602,
			   1.0, 0.785694958, 0.541196100, 0.275899379};
 BYTE row, col;
 BYTE i = 0;
 for (row = 0; row < 8; row++)
 {
   for (col = 0; col < 8; col++)
     {
       fdtbl_Y[i] = (float) (1.0 / ((double) DQTinfo.Ytable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));
       fdtbl_Cb[i] = (float) (1.0 / ((double) DQTinfo.Cbtable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));

	   i++;
     }
 }
}
*/

 int initRgbYcc (INT32 **prgb_ycc_tab) {
  INT32 * rgb_ycc_tab ;
  INT32 i;

  *prgb_ycc_tab = NULL;
  /* Allocate and fill in the conversion tables. */
 rgb_ycc_tab = (INT32 *)yl_calloc("PreP2", TABLE_SIZE, sizeof(INT32));
 if(rgb_ycc_tab == NULL) return -1;

  for (i = 0; i <= MAXJSAMPLE; i++) {
    rgb_ycc_tab[i+R_Y_OFF] = FIX(0.299) * i;
    rgb_ycc_tab[i+G_Y_OFF] = FIX(0.587) * i;
    rgb_ycc_tab[i+B_Y_OFF] = FIX(0.114) * i   + ONE_HALF;

    rgb_ycc_tab[i+R_CB_OFF] = (-FIX(0.168735892)) * i;
    rgb_ycc_tab[i+G_CB_OFF] = (-FIX(0.331264108)) * i;
    /* We use a rounding fudge-factor of 0.5-epsilon for Cb and Cr.
     * This ensures that the maximum output will round to MAXJSAMPLE
     * not MAXJSAMPLE+1, and thus that we don't have to range-limit.
     */
    rgb_ycc_tab[i+B_CB_OFF] = FIX(0.5) * i    + CBCR_OFFSET + ONE_HALF-1;
/*  B=>Cb and R=>Cr tables are the same
    rgb_ycc_tab[i+R_CR_OFF] = FIX(0.5) * i    + CBCR_OFFSET + ONE_HALF-1;
*/
    rgb_ycc_tab[i+G_CR_OFF] = (-FIX(0.418687589)) * i;
    rgb_ycc_tab[i+B_CR_OFF] = (-FIX(0.081312411)) * i;
  }
  *prgb_ycc_tab = rgb_ycc_tab;
  return 0;
}

int ColorSpaceConversion_RGBA_JPG(LPIMGDATA imgInp){
	INT32 * rgb_ycc_tab;
	int idx;
	int ret = 0;
//	int sh = 3;
//	BYTE testY, testCb;

	ret =  initRgbYcc (&rgb_ycc_tab);
	if(ret < 0) return ret;

	  BYTE r, g, b;
	  LPBYTE inptr;
//	  LPBYTE outptr0, outptr1, outptr2;
	  int row, col;
	  int num_rows = imgInp->height;
	  int num_cols = imgInp->width;
	  inptr = imgInp->pixPtr;
//	  while (--num_rows >= 0) {
	  switch(imgInp->bi.biBitCount){
	  case 24:
	      for (row = 0; row < num_rows; row++) {
		    for (col = 0; col < num_cols; col++) {
//		    	getColorRGBA(imgInp, col, row, &r, &g, &b);
		    	getColor24(imgInp, col, row, &r, &g, &b);
		   	  idx = row*num_cols+col;

		      /* Y */
		      outptr0[idx] = (BYTE)
				((rgb_ycc_tab[r+R_Y_OFF] + rgb_ycc_tab[g+G_Y_OFF] + rgb_ycc_tab[b+B_Y_OFF])
				 >> SCALEBITS);
		      /* Cb */
		      outptr1[idx] = (BYTE)
				((rgb_ycc_tab[r+R_CB_OFF] + rgb_ycc_tab[g+G_CB_OFF] + rgb_ycc_tab[b+B_CB_OFF])
				 >> SCALEBITS);
		      /* Cr */
		      outptr2[idx] = (BYTE)
				((rgb_ycc_tab[r+R_CR_OFF] + rgb_ycc_tab[g+G_CR_OFF] + rgb_ycc_tab[b+B_CR_OFF])
				 >> SCALEBITS);
		    }
		  }

	      break;

	  case 32:
      for (row = 0; row < num_rows; row++) {
	    for (col = 0; col < num_cols; col++) {
	    	getColorRGBA(imgInp, col, row, &r, &g, &b);
	      /* If the inputs are 0..MAXJSAMPLE, the outputs of these equations
	       * must be too; we do not need an explicit range-limiting operation.
	       * Hence the value being shifted is never negative, and we don't
	       * need the general RIGHT_SHIFT macro.
	       */

	   	  idx = row*num_cols+col;

	      /* Y */
	      outptr0[idx] = (BYTE)
			((rgb_ycc_tab[r+R_Y_OFF] + rgb_ycc_tab[g+G_Y_OFF] + rgb_ycc_tab[b+B_Y_OFF])
			 >> SCALEBITS);
//	      Y'  =   0 + (0.299    * R'_D) + (0.587    * G'_D) + (0.114    * B'_D)
//	      testY = (BYTE)( (0.299    * r) + (0.587    * g) + (0.114    * b) +0.5);


	      /* Cb */
	      outptr1[idx] = (BYTE)
			((rgb_ycc_tab[r+R_CB_OFF] + rgb_ycc_tab[g+G_CB_OFF] + rgb_ycc_tab[b+B_CB_OFF])
			 >> SCALEBITS);
//	      testCb = (BYTE)(128.0 -  (0.168736 * r) - (0.331264 * g) + (0.5      * b) +0.5);
// if(row < 10 && col <10)
//	      printf("  outptr1[idx]= %d, testCb= %d \n", outptr1[idx], testCb);
//	      fflush(stdout);

	      /* Cr */
	      outptr2[idx] = (BYTE)
			((rgb_ycc_tab[r+R_CR_OFF] + rgb_ycc_tab[g+G_CR_OFF] + rgb_ycc_tab[b+B_CR_OFF])
			 >> SCALEBITS);
	    }
	  }

      break;
    }



      printf(" yl_free(rgb_ycc_tab) \n");
      fflush(stdout);

      yl_free(rgb_ycc_tab);

	return ret;
}
void DownSamplingCX(int num_rows, int num_cols, SBYTE *outptr, int downSize_V, int downSize_H){
	int xInp, yInp, iInp, jInp;
	int xOut, yOut, iOut, jOut;

	int num_rowsOut = num_rows/downSize_V;
	int num_colsOut = num_cols/downSize_H;

    if(downSize_V < 2 && downSize_H < 2) return;

	  iInp = 0;
	  for(iOut=0;iOut<num_rowsOut;iOut++){
	      jInp = 0;
	      for(jOut=0;jOut<num_colsOut;jOut++){
	    	  yInp=iInp*num_cols;xInp = jInp;
	    	  yOut=iOut*num_colsOut;xOut = jOut;
//			  yInp=jInp*num_cols; xInp = iInp;
//			  yOut=jOut*num_colsOut; xOut = iOut;
			  outptr[yInp+xInp] = outptr[yOut+xOut];
			  jInp += downSize_H;
		  }
		  iInp  += downSize_V;
	  }
}
void DownSampling(int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR){
	DownSamplingCX(num_rows, num_cols, outptr1, downSize_VCB, downSize_HCB);
	DownSamplingCX(num_rows, num_cols, outptr2, downSize_VCR, downSize_HCR);
}
void DownSamplingAll(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR){
	DownSamplingCX(num_rows, num_cols, outptr0, downSize_VL, downSize_HL);
	DownSamplingCX(num_rows, num_cols, outptr1, downSize_VCB, downSize_HCB);
	DownSamplingCX(num_rows, num_cols, outptr2, downSize_VCR, downSize_HCR);
}

void createCosMatrix(float *T){
	int i, j;
	for(j=0;j<8;j++) T[0*8+j] = 0.3536;
	T[1*8+0] = 0.4904;T[1*8+1] = 0.4157;T[1*8+2] = 0.2778;T[1*8+3] = 0.0975;
	T[2*8+0] = 0.4619;T[2*8+1] = 0.1913;T[2*8+2] = -0.1913;T[2*8+3] = -0.4619;
	T[3*8+0] = 0.4157;T[3*8+1] = -0.0975;T[3*8+2] = -0.4904;T[3*8+3] = -0.2778;
	T[4*8+0] = 0.3536;T[4*8+1] = -0.3536;T[4*8+2] = -0.3536;T[4*8+3] = 0.3536;
	T[5*8+0] = 0.2778;T[5*8+1] = -0.4904;T[5*8+2] = 0.0975;T[5*8+3] = 0.4157;
	T[6*8+0] = 0.1913;T[6*8+1] = -0.4619;T[6*8+2] = 0.4619;T[6*8+3] = -0.1913;
	T[7*8+0] = 0.0975;T[7*8+1] = -0.2778;T[7*8+2] = 0.4157;T[7*8+3] = -0.4904;
	for(i=1;i<8;i+=2)
	   for(j=4;j<8;j++)
		   T[i*8+j] = -T[i*8+(7-j)];
	for(i=2;i<8;i+=2)
	   for(j=4;j<8;j++)
	       T[i*8+j] = T[i*8+(7-j)];
}
void transposeMatrix(float *T, float *Ttrans){
	int i, j;
	for(i=0;i<8;i++)
	   for(j=0;j<8;j++)
		   Ttrans[i*8+j] = T[j*8+i];

}
void multiplyDCTB(float *T1, char *T2, float *D){
	int i, j, k;
	float sum;
	for(i=0;i<8;i++){
	   for(j=0;j<8;j++){
		   sum = 0;
		   for(k=0;k<8;k++){
			   sum += T1[i*8+k]*(float)T2[k*8+j];
		   }
		   D[i*8+j] = sum;
	   }
	}
}
void multiplyDCTI(float *T1, int *T2, float *D){
	int i, j, k;
	float sum;
	for(i=0;i<8;i++){
	   for(j=0;j<8;j++){
		   sum = 0;
		   for(k=0;k<8;k++){
			   sum += T1[i*8+k]*(float)T2[k*8+j];
		   }
		   D[i*8+j] = sum;
	   }
	}
}

void multiplyDCT(float *T1, float *T2, float *D){
	int i, j, k;
	float sum;
	for(i=0;i<8;i++){
	   for(j=0;j<8;j++){
		   sum = 0;
		   for(k=0;k<8;k++){
			   sum += T1[i*8+k]*T2[k*8+j];
		   }
		   D[i*8+j] = sum;
	   }
	}
}

 void printMatrix(float *D, char *str){
	 char buff[100];
	 char buff2[100];
	 int i, j;
	 strcpy(buff,"------------------- matrix - ");
	 strcat(buff,str);
	 printf(buff);printf("\n");
		for(i=0;i<8;i++){
			buff[0] = 0;
		   for(j=0;j<8;j++){
			   sprintf(buff2,"%f, ",D[i*8+j]);
			   strcat(buff, buff2);
		   }
		   strcat(buff, "\n");
		   printf(buff);
		}
	printf("\n");
 }
 void printMatrixByte(BYTE *D, char *str){
 	 char buff[100];
 	 char buff2[100];
 	 int i, j;
 	 strcpy(buff,"------------------- matrix - ");
 	 strcat(buff,str);
 	 printf(buff);printf("\n");

 		for(i=0;i<8;i++){
 			buff[0] = 0;
 		   for(j=0;j<8;j++){
 			   sprintf(buff2,"%d, ",D[i*8+j]);
 			   strcat(buff, buff2);
 		   }
 		   strcat(buff, "\n");
 		   printf(buff);
 		}
 	printf("\n");
  }

 void printMatrixB(char *D, char *str){
	 char buff[100];
	 char buff2[100];
	 int i, j;
	 strcpy(buff,"------------------- matrix - ");
	 strcat(buff,str);
	 printf(buff);printf("\n");

		for(i=0;i<8;i++){
			buff[0] = 0;
		   for(j=0;j<8;j++){
			   sprintf(buff2,"%d, ",D[i*8+j]);
			   strcat(buff, buff2);
		   }
		   strcat(buff, "\n");
		   printf(buff);
		}
	printf("\n");
 }
 void printMatrixI(int *D, char *str){
	 char buff[100];
	 char buff2[100];
	 int i, j;
	 strcpy(buff,"------------------- matrix - ");
	 strcat(buff,str);
	 printf(buff);printf("\n");

		for(i=0;i<8;i++){
			buff[0] = 0;
		   for(j=0;j<8;j++){
			   sprintf(buff2,"%d, ",D[i*8+j]);
			   strcat(buff, buff2);
		   }
		   strcat(buff, "\n");
		   printf(buff);
		}
	printf("\n");
 }
 void printVectorI(int *D, char *str){
	 char buff[100];
	 char buff2[100];
	 int i;
	 strcpy(buff,"------------------- matrix - ");
	 strcat(buff,str);
	 printf(buff);printf("\n");

		buff[0] = 0;
		for(i=0;i<130;i++){
			   sprintf(buff2,"%d, ",D[i]);
			   strcat(buff, buff2);
			   if(i> 0 && i%10==0){
				   strcat(buff, "\n");
				   printf(buff);
					buff[0] = 0;
			   }
	   }
	printf("\n");
 }

 void printVectorIToBuff(int *D, char *str, char *buffOut){
//	 char buff[100];
	 char buff2[100];
	 int i;
	 strcpy(buffOut,"------------------- matrix - ");
	 strcat(buffOut,str);
	 strcat(buffOut,"\n");
//	 printf(buff);printf("\n");

		buffOut[0] = 0;
		for(i=0;i<130;i++){
			   sprintf(buff2,"%d, ",D[i]);
			   strcat(buffOut, buff2);
//			   if(i> 0 && i%10==0){
//				   strcat(buff, "\n");
//				   printf(buff);
//					buff[0] = 0;
//			   }
	   }
	 strcat(buffOut,"\n");
//	printf("\n");
 }


 void DCT(int *block, float *D, float *T, float *Ttrans){
	 float Dtmp[8*8];
	 multiplyDCTI(T, block, Dtmp);
//if(printFlg==1)
// printMatrix(Dtmp,"Dtmp");
	 multiplyDCT(Dtmp, Ttrans, D);
 }
 float C(int u){
   if(u==0)
     return (1.0/sqrt(2.0));
   else
     return 1.0;
 }
 void DCTcos(int *f, float *D)  {
   float a;
   int x, y, u, v;
   for(u=0;u<8;u++)
     for(v=0;v<8;v++)  {
 	    a = 0.0;
 	   for(x=0;x<8;x++)
 	     for(y=0;y<8;y++)
 	        a += (float)(f[x*8+y]) *cos((double)(2.0*(float)x+1.0)*(float)u*3.14/16.0)*cos((double)(2.0*(float)y+1.0)*(float)v*3.14/16.0);
 	  D[u*8+v] = (int)(0.25*C(u)*C(v)*a);
   }
}
 void createQuantL(int *Q, int quantLevel){
	 float c = 1;
	 int row, i, j;
	 if(quantLevel < 1) quantLevel = 1;
	 if(quantLevel > 99) quantLevel = 99;
	 if(quantLevel < 50) c = 50.0/(float)quantLevel;
	 if(quantLevel > 50) c = (100-(float)quantLevel)/50.0;
	 row = 0*8;
	 Q[row+0] = 16;Q[row+1] = 11;Q[row+2] = 10;Q[row+3] = 16;Q[row+4] = 24;Q[row+5] = 40;Q[row+6] = 51;Q[row+7] = 61;
	 row = 1*8;
	 Q[row+0] = 12;Q[row+1] = 12;Q[row+2] = 14;Q[row+3] = 19;Q[row+4] = 26;Q[row+5] = 58;Q[row+6] = 60;Q[row+7] = 55;
	 row = 2*8;
	 Q[row+0] = 14;Q[row+1] = 13;Q[row+2] = 16;Q[row+3] = 24;Q[row+4] = 40;Q[row+5] = 57;Q[row+6] = 69;Q[row+7] = 56;
	 row = 3*8;
	 Q[row+0] = 14;Q[row+1] = 17;Q[row+2] = 22;Q[row+3] = 29;Q[row+4] = 51;Q[row+5] = 87;Q[row+6] = 80;Q[row+7] = 62;
	 row = 4*8;
	 Q[row+0] = 18;Q[row+1] = 22;Q[row+2] = 37;Q[row+3] = 56;Q[row+4] = 68;Q[row+5] = 109;Q[row+6] = 103;Q[row+7] = 77;
	 row = 5*8;
	 Q[row+0] = 24;Q[row+1] = 35;Q[row+2] = 55;Q[row+3] = 64;Q[row+4] = 81;Q[row+5] = 104;Q[row+6] = 113;Q[row+7] = 92;
	 row = 6*8;
	 Q[row+0] = 49;Q[row+1] = 64;Q[row+2] = 78;Q[row+3] = 87;Q[row+4] = 103;Q[row+5] = 121;Q[row+6] = 120;Q[row+7] = 101;
	 row = 7*8;
	 Q[row+0] = 72;Q[row+1] = 92;Q[row+2] = 95;Q[row+3] = 98;Q[row+4] = 112;Q[row+5] = 100;Q[row+6] = 103;Q[row+7] = 99;
		for(i=0;i<8;i++){
		   for(j=0;j<8;j++){
		       row = i*8+j;
			   Q[row] = (int)(c*Q[row]);
			   if(Q[row]<1) Q[row] = 1;
			   if(Q[row]>255) Q[row] = 255;
		   }
		}

 }
 /*
 16 11 10 16 24 40 51 61
 12 12 14 19 26 58 60 55
 14 13 16 24 40 57 69 56
 14 17 22 29 51 87 80 62
 18 22 37 56 68 109 103 77
 24 35 55 64 81 104 113 92
 49 64 78 87 103 121 120 101
 72 92 95 98 112 100 103 99
*/
 void createQuantLTest(int *Q, int quantLevel){
	 int row, i, j;
	 for(i=0;i<8;i++){
	   for(j=0;j<8;j++){
		       row = i*8+j;
			   Q[row] = 1;
		   }
		}

 }

 void createQuantCh(int *Q, int quantLevel){
	 float c = 1;
	 int row, i, j;
	 if(quantLevel < 1) quantLevel = 1;
	 if(quantLevel > 99) quantLevel = 99;
	 if(quantLevel < 50) c = 50.0/(float)quantLevel;
	 if(quantLevel > 50) c = (100.0-(float)quantLevel)/50.0;
		for(i=0;i<8;i++){
		   for(j=0;j<8;j++){
			   Q[i*8+j] = 99;
		   }
		}
	 row = 0*8;
	 Q[row+0] = 17;Q[row+1] = 18;Q[row+2] = 24;Q[row+3] = 47;
	 row = 1*8;
	 Q[row+0] = 18;Q[row+1] = 21;Q[row+2] = 26;Q[row+3] = 66;
	 row = 2*8;
	 Q[row+0] = 24;Q[row+1] = 26;Q[row+2] = 56;
	 row = 3*8;
	 Q[row+0] = 47;Q[row+1] = 66;
	 for(i=0;i<8;i++){
	    for(j=0;j<8;j++){
	      row = i*8+j;
		   Q[row] = (int)(c*Q[row]);
		   if(Q[row]<1) Q[row] = 1;
		   if(Q[row]>255) Q[row] = 255;
	    }
	 }
 }

/*
 17 18 24 47 99 99 99 99
 18 21 26 66 99 99 99 99
 24 26 56 99 99 99 99 99
 47 66 99 99 99 99 99 99
 99 99 99 99 99 99 99 99
 99 99 99 99 99 99 99 99
 99 99 99 99 99 99 99 99
 99 99 99 99 99 99 99 99
*/
 void createQuantChTest(int *Q, int quantLevel){
	 int i, j;
	for(i=0;i<8;i++){
	   for(j=0;j<8;j++){
		   Q[i*8+j] = 1;
	   }
	}
 }

void quantDCT(int *Q, float *D, int *DQ){
    int i, j, idx;
	 for(i=0;i<8;i++){
	    for(j=0;j<8;j++){
	    	idx = i*8+j;
	    	DQ[idx] = (int)(D[idx]/(float)Q[idx] +0.5);
	    }
	 }
}

void zigZag(int *DQ, int *ZZ){
	  int i=0,j=0,k=0,d=0;
	  while(k<36)  {
	      ZZ[k++] = DQ[i*8+j];
	      if((i==0)&&(j%2==0)) {
		    j++;
		    d=1;
		  }
	      else if((j==0)&&(i%2==1)){
		    i++;
		    d=0;
		  }
	      else if(d==0){
		    i--;
		    j++;
		  }
	      else{
		    i++;
		    j--;
		  }
	    }
	  i = 7;
	  j = 1;
	  d = 0;
	  while(k<64)	    {
		  ZZ[k++] = DQ[i*8+j];
	      if((i==7)&&(j%2==0)) {
		    j++;
		    d=0;
		  }
	      else if((j==7)&&(i%2==1)){
		    i++;
		    d=1;
		  }
	      else if(d==0){
		    i--;
		    j++;
		  }
	      else{
		    i++;
		    j--;
		  }
	    }
}
int RLE(int *ZZ, int *RL) {
  int rl=1;
  int i=1;
  int k = 0;
  int ok = 1;
  RL[0] = ZZ[0];
  while(i<64)   {
      k=0;
      while((i<64)&&(ZZ[i]==0)&&(k<15))	{
	    i++;
	    k++;
	  }
      if(i==64)	{
	    RL[rl++] = 0;
	    RL[rl++] = 0;
	  }
      else{
	     RL[rl++] = k;
	     RL[rl++] = ZZ[i++];
	  }
  }
  if(!(RL[rl-1]==0 && RL[rl-2]==0)) {
      RL[rl++] = 0;
      RL[rl++] = 0;
  }
  while(ok && rl-4>0) {
     if((RL[rl-4]==15)&&(RL[rl-3]==0)) {
         RL[rl-4]=0;
         rl-=2;
     }else ok = 0;

  }

  return rl;
}

int getCat(int a)
{
  if(a==0)
	return 0;
  else if(abs(a)<=1)
	return 1;
  else if(abs(a)<=3)
	return 2;
  else if(abs(a)<=7)
	return 3;
  else if(abs(a)<=15)
	return 4;
  else if(abs(a)<=31)
	return 5;
  else if(abs(a)<=63)
	return 6;
  else if(abs(a)<=127)
	return 7;
  else if(abs(a)<=255)
	return 8;
  else if(abs(a)<=511)
	return 9;
  else if(abs(a)<=1023)
	return 10;
  else if(abs(a)<=2047)
	return 11;
  else if(abs(a)<=4095)
	return 12;
  else if(abs(a)<=8191)
	return 13;
  else if(abs(a)<=16383)
	return 14;
  else
	return 15;
}

void getDCcodeOld(int a, int *lenb, char *b) {

  int codeLen[12] = {3,4,5,5,7,8,10,12,14,16,18,20};
  char* code[12] = {"010","011","100","00","101","110","1110","11110","111110","1111110","11111110","111111110"};
  int cat = getCat(a);
  *lenb = codeLen[cat];
  strcpy(b,code[cat]);
  int j;
  int c = a;
  if(a<0)
     c+=(int)pow(2,cat)-1;

  for(j=*lenb-1;j>*lenb-cat-1;j--) {
     if(c%2==1)
	b[j] = '1';
     else
	b[j] = '0';
     c/=2;
  }
  b[*lenb] = '\0';
}

void setCodeBits(DWORD *b, int start, char *code){
	int i, len;
	char ch;
	len = strlen(code);

	int nX, xn;
	BYTE mask;
	LPBYTE ptr;
	xn = start;
	for(i=0;i<len;i++){
		ch = code[i];
		nX = xn/8;
		ptr = (LPBYTE)b+nX;
		mask=LEFT_BIT;
		mask >>=(xn%8);
		if(code[i]!='0'){
			*ptr = *ptr | mask;
		}
		else {
			*ptr = *ptr & (~mask);
		}
		xn++;
	}
}
void setCodeBit(BYTE *b, int xn, int v){
	int nX;
	BYTE mask;
	LPBYTE ptr;

		nX = xn/8;
		ptr = b+nX;
		mask=LEFT_BIT;
		mask >>=(xn%8);
		if(v!=0){
			*ptr = *ptr | mask;
		}
		else {
			*ptr = *ptr & (~mask);
		}
}
int getCodeBit(BYTE *ptr, int xn){
	int nX, v;
	BYTE mask;
	LPBYTE ptrP;

		nX = xn/8;
		ptrP = ptr+nX;
		mask=LEFT_BIT;
		mask >>=(xn%8);
		v=(~(*ptrP))&mask;
		if(v!=0) {
			return(0);
		}
		else {
			return(1);
		}
}

void getDCcode(int a, int *lenb, DWORD *b) {
  char bStr[32];
  int codeLen[12] = {3,4,5,5,7,8,10,12,14,16,18,20};
  char* code[12] = {"010","011","100","00","101","110","1110","11110","111110","1111110","11111110","111111110"};
  int cat = getCat(a);
  int startBit = 0;
  int j, k;
  int c = a;
   if(cat> 11) cat = 11;
  *lenb = codeLen[cat];

  setCodeBits(b, startBit, code[cat]);

//printCodeBits((BYTE *)b, strlen(code[cat]));
//  strcpy(b,code[cat]);

  if(a<0)
     c+=(int)pow(2,cat)-1;
  k= cat-1;
  for(j=*lenb-1;j>*lenb-cat-1;j--) {
     if(c%2==1)
    	 bStr[k] = '1';
     else
    	 bStr[k] = '0';
     k--;
     c/=2;
  }
  bStr[cat] =  '\0';
  startBit = *lenb-cat;
//  printf("startBit= %d\n", startBit);
  setCodeBits(b, startBit, bStr);

//  printCodeBits((BYTE *)b, *lenb);
//  return b;
}

void getACcode(int n,int a, int *lenb, DWORD *b) {
  int codeLen[16][11] = {
  {4 ,3 ,4 ,6 ,8 ,10,12,14,18,25,26},
  {0 ,5 ,8 ,10,13,16,22,23,24,25,26},
  {0 ,6 ,10,13,20,21,22,23,24,25,26},
  {0 ,7 ,11,14,20,21,22,23,24,25,26},
  {0 ,7 ,12,19,20,21,22,23,24,25,26},
  {0 ,8 ,12,19,20,21,22,23,24,25,26},
  {0 ,8 ,13,19,20,21,22,23,24,25,26},
  {0 ,9 ,13,19,20,21,22,23,24,25,26},
  {0 ,9 ,17,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,11,18,19,20,21,22,23,24,25,26},
  {0 ,12,18,19,20,21,22,23,24,25,26},
  {0 ,13,18,19,20,21,22,23,24,25,26},
  {12,17,18,19,20,21,22,23,24,25,26}
  };
  char* code[16][11] = {
		  {"1010",  "00",  "01",  "100",  "1011",  "11010",  "111000",  "1111000",  "1111110110",  "1111111110000010",  "1111111110000011"},
		  {"","1100","111001","1111001","111110110","11111110110","1111111110000100","1111111110000101","1111111110000110","1111111110000111","1111111110001000"},
		  {"","11011","11111000","1111110111","1111111110001001","1111111110001010","1111111110001011","1111111110001100","1111111110001101","1111111110001110","1111111110001111"},
		  {"","111010","111110111","11111110111","1111111110010000","1111111110010001","1111111110010010","1111111110010011","1111111110010100","1111111110010101","1111111110010110"},
    	  {"","111011","1111111000","1111111110010111","1111111110011000","1111111110011001","1111111110011010","1111111110011011","1111111110011100","1111111110011101","1111111110011110"},
		  {"","1111010","1111111001","1111111110011111","1111111110100000","1111111110100001","1111111110100010","1111111110100011","1111111110100100","1111111110100101","1111111110100110"},
		  {"","1111011","11111111000","1111111110100111","1111111110101000","1111111110101001","1111111110101010","1111111110101011","1111111110101100","1111111110101101","1111111110101110"},
		  {"","11111001","11111111001","1111111110101111","1111111110110000","1111111110110001","1111111110110010","1111111110110011","1111111110110100","1111111110110101","1111111110110110"},
          {"","11111010","111111111000000","1111111110110111","1111111110111000","1111111110111001","1111111110111010","1111111110111011","1111111110111100","1111111110111101","1111111110111110"},
		  {"","111111000","1111111110111111","1111111111000000","1111111111000001","1111111111000010","1111111111000011","1111111111000100","1111111111000101","1111111111000110","1111111111000111"},
		  {"","111111001","1111111111001000","1111111111001001","1111111111001010","1111111111001011","1111111111001100","1111111111001101","1111111111001110","1111111111001111","1111111111010000"},
	      {"","111111010","1111111111010001","1111111111010010","1111111111010011","1111111111010100","1111111111010101","1111111111010110","1111111111010111","1111111111011000","1111111111011001"},
		  {"","1111111010","1111111111011010","1111111111011011","1111111111011100","1111111111011101","1111111111011110","1111111111011111","1111111111100000","1111111111100001","1111111111100010"},
     	  {"","11111111010","1111111111100011","1111111111100100","1111111111100101","1111111111100110","1111111111100111","1111111111101000", "1111111111101001","1111111111101010","1111111111101011"},
	      {"","111111110110","1111111111101100","1111111111101101","1111111111101110","1111111111101111","1111111111110000","1111111111110001","1111111111110010","1111111111110011","1111111111110100"},
		  {"111111110111","1111111111110101","1111111111110110","1111111111110111","1111111111111000","1111111111111001","1111111111111010","1111111111111011","1111111111111100","1111111111111101","1111111111111110"}
  };
  int j, k;
  int c = a;
  int cat = getCat(a);
  int startBit = 0;
  char bStr[32];

  if(cat> 10) cat = 10;
  *lenb = codeLen[n][cat];

  setCodeBits(b, startBit, code[n][cat]);
//  printCodeBits((BYTE *)b, *lenb);
//  strcpy(b,code[n][cat]);

  if(a<0)
     c+=(int)pow(2,cat)-1;
  k= cat-1;
  for(j=*lenb-1;j>*lenb-cat-1;j--) {
     if(c%2==1)
    	 bStr[k] = '1';
     else
    	 bStr[k] = '0';
     c/=2;
     k--;
  }
  bStr[cat] =  '\0';
  startBit = *lenb-cat;
  setCodeBits(b, startBit, bStr);
//  printCodeBits((BYTE *)b, *lenb);

//  return b;
}


void getACcodeOld(int n,int a, int *lenb, char *b ) {
  int codeLen[16][11] = {
  {4 ,3 ,4 ,6 ,8 ,10,12,14,18,25,26},
  {0 ,5 ,8 ,10,13,16,22,23,24,25,26},
  {0 ,6 ,10,13,20,21,22,23,24,25,26},
  {0 ,7 ,11,14,20,21,22,23,24,25,26},
  {0 ,7 ,12,19,20,21,22,23,24,25,26},
  {0 ,8 ,12,19,20,21,22,23,24,25,26},
  {0 ,8 ,13,19,20,21,22,23,24,25,26},
  {0 ,9 ,13,19,20,21,22,23,24,25,26},
  {0 ,9 ,17,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,10,18,19,20,21,22,23,24,25,26},
  {0 ,11,18,19,20,21,22,23,24,25,26},
  {0 ,12,18,19,20,21,22,23,24,25,26},
  {0 ,13,18,19,20,21,22,23,24,25,26},
  {12,17,18,19,20,21,22,23,24,25,26}
  };
  char* code[16][11] = {
		  {"1010",  "00",  "01",  "100",  "1011",  "11010",  "111000",  "1111000",  "1111110110",  "1111111110000010",  "1111111110000011"},
		  {"","1100","111001","1111001","111110110","11111110110","1111111110000100","1111111110000101","1111111110000110","1111111110000111","1111111110001000"},
		  {"","11011","11111000","1111110111","1111111110001001","1111111110001010","1111111110001011","1111111110001100","1111111110001101","1111111110001110","1111111110001111"},
		  {"","111010","111110111","11111110111","1111111110010000","1111111110010001","1111111110010010","1111111110010011","1111111110010100","1111111110010101","1111111110010110"},
    	  {"","111011","1111111000","1111111110010111","1111111110011000","1111111110011001","1111111110011010","1111111110011011","1111111110011100","1111111110011101","1111111110011110"},
		  {"","1111010","1111111001","1111111110011111","1111111110100000","1111111110100001","1111111110100010","1111111110100011","1111111110100100","1111111110100101","1111111110100110"},
		  {"","1111011","11111111000","1111111110100111","1111111110101000","1111111110101001","1111111110101010","1111111110101011","1111111110101100","1111111110101101","1111111110101110"},
		  {"","11111001","11111111001","1111111110101111","1111111110110000","1111111110110001","1111111110110010","1111111110110011","1111111110110100","1111111110110101","1111111110110110"},
          {"","11111010","111111111000000","1111111110110111","1111111110111000","1111111110111001","1111111110111010","1111111110111011","1111111110111100","1111111110111101","1111111110111110"},
		  {"","111111000","1111111110111111","1111111111000000","1111111111000001","1111111111000010","1111111111000011","1111111111000100","1111111111000101","1111111111000110","1111111111000111"},
		  {"","111111001","1111111111001000","1111111111001001","1111111111001010","1111111111001011","1111111111001100","1111111111001101","1111111111001110","1111111111001111","1111111111010000"},
	      {"","111111010","1111111111010001","1111111111010010","1111111111010011","1111111111010100","1111111111010101","1111111111010110","1111111111010111","1111111111011000","1111111111011001"},
		  {"","1111111010","1111111111011010","1111111111011011","1111111111011100","1111111111011101","1111111111011110","1111111111011111","1111111111100000","1111111111100001","1111111111100010"},
     	  {"","11111111010","1111111111100011","1111111111100100","1111111111100101","1111111111100110","1111111111100111","1111111111101000", "1111111111101001","1111111111101010","1111111111101011"},
	      {"","111111110110","1111111111101100","1111111111101101","1111111111101110","1111111111101111","1111111111110000","1111111111110001","1111111111110010","1111111111110011","1111111111110100"},
		  {"111111110111","1111111111110101","1111111111110110","1111111111110111","1111111111111000","1111111111111001","1111111111111010","1111111111111011","1111111111111100","1111111111111101","1111111111111110"}
  };
  int j;
  int c = a;
  int cat = getCat(a);

  *lenb = codeLen[n][cat];
  strcpy(b,code[n][cat]);

  if(a<0)
     c+=(int)pow(2,cat)-1;

  for(j=*lenb-1;j>*lenb-cat-1;j--) {
     if(c%2==1)
    	 b[j] = '1';
     else
    	 b[j] = '0';
     c/=2;
  }
  b[*lenb] = '\0';
}
int reloadCodeBits_buff(DWORD *b, int bLen,  SBYTE *code, int numBits, BYTE *byteBuff, int *bitInBuff, int buffLen){
	int i, v, d;
	LPBYTE ptr = (BYTE*)b;
	int nBuff = *bitInBuff;

//	d = buffLen*8-nBuff;
//    if(d< bLen) {
//    	printf("buffLen*8-bitInBuff ( %d ) > bLen ( %d )\n", d, bLen);
//    	return numBits;
//    }
	for(i=0;i<bLen;i++){
		v = getCodeBit(ptr, i);
		setCodeBit(byteBuff, nBuff, v);
		nBuff++;
		if(nBuff == 8){
			code[numBits/8] = byteBuff[0];
			nBuff = 0;
			numBits += 8;
			if(byteBuff[0] == 0xFF) {
				code[numBits/8] = 0x00;
				numBits += 8;
			}
		}
		d = buffLen*8-nBuff;
		if(d< nBuff) {
		 	printf("buffLen*8-bitInBuff ( %d ) < nBuff ( %d )\n", d, nBuff);
		   	return numBits;
		}

//		setCodeBit(code, numBits, v);
//		numBits++;
	}
	return numBits;

}
int reloadCodeBits_Bytes(DWORD *b, int bLen,  SBYTE *code, int numBits){
	int i, v;
	int b_8 = bLen/8;
	int r = bLen%8;
	int n = numBits/8;
	LPBYTE ptr = (BYTE*)b;

	for(i=0;i<b_8;i++){
		code[i+n] = ptr[i];
	}
	numBits += (b_8*8);

	printf("numBits=%d, r= %d\n ",numBits, r);

	if(r>0){
		ptr = &ptr[b_8];
		for(i=0;i<r;i++){
			v = getCodeBit(ptr, i);
			setCodeBit((BYTE*)code, numBits, v);
			numBits++;
		}
	}
	printf("numBits=%d\n ",numBits);
	return numBits;
}

int reloadCodeBits(DWORD *b, int bLen,  BYTE *code, int numBits){
	int i, v;
	LPBYTE ptr = (BYTE*)b;

	for(i=0;i<bLen;i++){
		v = getCodeBit(ptr, i);
		setCodeBit(code, numBits, v);
		numBits++;
	}
	return numBits;
}
int Encode(int *RL, int rl, BYTE *code){
//  char b[32];
  DWORD b;
  int bLen, numBits = 0;
  int i;

//  LPBYTE ptr;

  getDCcode(RL[0], &bLen, &b);
//  ptr = (BYTE*)&b;
//  printf("b= %d\n",*ptr);

  numBits = reloadCodeBits(&b, bLen, code, numBits);

//  printf("numBits= %d\n",numBits);
//  printCodeBits(code, numBits);

  for(i=1;i<rl;i+=2) {
	getACcode(RL[i],RL[i+1], &bLen, &b);
//		  ptr = (BYTE*)&b;
//		  printf("i= %d, b= %d\n",i, *ptr);

	numBits = reloadCodeBits(&b, bLen, code, numBits);


//	  printf("numBits= %d\n",numBits);
//	  printCodeBits(code, numBits);


  }
  return numBits;
}
int EncodeClasic(int *RL, int rl, BYTE *code, int isLum){
   DWORD b;
  int bLen, numBits = 0;
  int i;
  int count = 0;

//  LPBYTE ptr;

  getDCcodeClasic(RL[0], &bLen, &b, isLum);

  count++;
//  ptr = (BYTE*)&b;
//  printf("b= %d\n",*ptr);

  numBits = reloadCodeBits(&b, bLen, code, numBits);

//  printf("numBits= %d\n",numBits);
//  printCodeBits(code, numBits);


  for(i=1;i<rl;i+=2) {
	getACcodeClasic(RL[i],RL[i+1], &bLen, &b, isLum);
	if(RL[i] == 0)
		count++;
	if(RL[i] < 0 || RL[i]>15)
		 printf("ERROR---------------------------- RL[i]= %d\n", RL[i]);
	if(RL[i] > 0)
		count += RL[i];
//	  ptr = (BYTE*)&b;
//	  printf("i= %d, b= %d\n",i, *ptr);
	numBits = reloadCodeBits(&b, bLen, code, numBits);

//	  printf("numBits= %d\n",numBits);
//	  printCodeBits(code, numBits);
  }
//  if(count > 64) {
//  	 printf("ERROR---------------------------- count= %d, rl= %d, numBits= %d\n",count, rl, numBits);
// 	printVectorI(RL, "RL");
// 	printCodeBits(code, numBits);
//  }

  return numBits;
}
/*
int EncodeClasicTest(int *RL, int rl, BYTE *code, int isLum, int row, int col){
   DWORD b;
  int bLen, numBits = 0;
  int i;
  int count = 0;
  char buff[4000];

//  LPBYTE ptr;

  getDCcodeClasic(RL[0], &bLen, &b, isLum);

  count++;
//  ptr = (BYTE*)&b;
//  printf("b= %d\n",*ptr);

  numBits = reloadCodeBits(&b, bLen, code, numBits);

//  printf("numBits= %d\n",numBits);
//  printCodeBits(code, numBits);


  for(i=1;i<rl;i+=2) {
	getACcodeClasic(RL[i],RL[i+1], &bLen, &b, isLum);
	if(RL[i] == 0)
		count++;
	if(RL[i] < 0 || RL[i]>15)
		 printf("ERROR---------------------------- RL[i]= %d\n", RL[i]);
	if(RL[i] > 0)
		count += RL[i];
//	  ptr = (BYTE*)&b;
//	  printf("i= %d, b= %d\n",i, *ptr);
	numBits = reloadCodeBits(&b, bLen, code, numBits);

//	  printf("numBits= %d\n",numBits);
//	  printCodeBits(code, numBits);
  }
  if(count > 64) {
     sprintf(buff,"ERROR---------------------------- count= %d, rl= %d, numBits= %d, row= %d, col= %d\n",count, rl, numBits, row, col);
     saveLogFile("", buff, "log_test.txt");
     printVectorIToBuff(RL, "RL", buff);
     saveLogFile("", buff, "log_test.txt");

     printCodeBitsToBuff(code, numBits, buff);
	 strcat(buff,"T\n");
     saveLogFile("", buff, "log_test.txt");
     printCodeBitsToBuff_0X(code, numBits, buff);
	 strcat(buff,"T\n");
     saveLogFile("", buff, "log_test.txt");

// 	printVectorI(RL, "RL");
// 	printCodeBits(code, numBits);
  }

  return numBits;
}
*/
void EncodeOld(char *RL, int rl, char *code){
  char b[32];
  int bLen;
  getDCcodeOld(RL[0], &bLen, b);
  	printf(" RL[0]= %d, bLen= %d\n",RL[0], bLen);

  strcpy(code,b);
  int i;
  for(i=1;i<rl;i+=2) {
	getACcodeOld(RL[i],RL[i+1], &bLen,b);
	printf(" RL[ %d ]= %d, RL[ %d ]= %d, bLen= %d\n",i,RL[i],i+1,RL[i+1], bLen);
	strcat(code,b);
  }
  printf("encode; code= %s\n",code);
}

void addToTheOutput(LPBYTE codeGlb, int lenGlb, char *codeLoc, int lenLoc){
  int i;
  for(i=0;i<lenLoc;i++)
	  codeGlb[lenGlb+i] = codeLoc[i];
}

int setJPGHeader(SBYTE *jpgOut, int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR){
	SBYTE *ptr;
	WORD word;
	WORD *ptrW;
	BYTE *ptrB;
	int i;
//	BYTE v;

	int test = 0;

	ptr = jpgOut;

// start of image
	*ptr = 0xff;ptr++;
	*ptr = 0xd8;ptr++;
// application marker
	*ptr = 0xff;ptr++;
	*ptr = 0xe0;ptr++;
// length
	*ptr = 0x00;ptr++;
	*ptr = 0x10;ptr++;

// Identifier
	*ptr = 0x4A;ptr++;
	*ptr = 0x46;ptr++;
	*ptr = 0x49;ptr++;
	*ptr = 0x46;ptr++;
	*ptr = 0x00;ptr++;
// Version
	*ptr = 0x01;ptr++;
	*ptr = 0x01;ptr++;
// Density units
	*ptr = 0x00;ptr++;
//x density
	*ptr = 0x00;ptr++;
	*ptr = 0x01;ptr++;
//y density
	*ptr = 0x00;ptr++;
	*ptr = 0x01;ptr++;
// thumbnail width
	*ptr = 0x00;ptr++;
// thumbnail height
	*ptr = 0x00;ptr++;

//	Quantization Table
	*ptr = 0xff;ptr++;
	*ptr = 0xdb;ptr++;
// length
	*ptr = 0x00;ptr++;
//	*ptr = 0x43;ptr++;
	*ptr = 0x84;ptr++;
// element precision / identifier
	*ptr = 0x00;ptr++;
// elements
	{BYTE Qelem[64] =
	 {16, 11, 10, 16, 24, 40, 51, 61,
	 12, 12, 14, 19, 26, 58, 60, 55,
	 14, 13, 16, 24, 40, 57, 69, 56,
	 14, 17, 22, 29, 51, 87, 80, 62,
	 18, 22, 37, 56, 68, 109, 103, 77,
	 24, 35, 55, 64, 81, 104, 113, 92,
	 49, 64, 78, 87, 103, 121, 120, 101,
	 72, 92, 95, 98, 112, 100, 103, 99};
	    for(i=0;i<64;i++){
		    *ptr = Qelem[zigzagInv[i]];ptr++;
	    }
	}

//	Quantization Table
// 	*ptr = 0xff;ptr++;
// 	*ptr = 0xdb;ptr++;
// length
// 	*ptr = 0x00;ptr++;
// 	*ptr = 0x43;ptr++;
// element precision / identifier
 	*ptr = 0x01;ptr++;
// elements
 	{BYTE Qelem[64] =
	 {17, 18, 24, 47, 99, 99, 99, 99,
	 18, 21, 26, 66, 99, 99, 99, 99,
	 24, 26, 56, 99, 99, 99, 99, 99,
	 47, 66, 99, 99, 99, 99, 99, 99,
	 99, 99, 99, 99, 99, 99, 99, 99,
	 99, 99, 99, 99, 99, 99, 99, 99,
	 99, 99, 99, 99, 99, 99, 99, 99,
	 99, 99, 99, 99, 99, 99, 99, 99};
 	    for(i=0;i<64;i++){
		    *ptr = Qelem[zigzagInv[i]];ptr++;
	    }
    }
// 	Start of frame
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc0;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x11;ptr++;
// sample precision
 	*ptr = 0x08;ptr++;
// number of rows
 	ptrW = &word;
	*ptrW = (WORD)num_rows;
	ptrB = (BYTE *)&word;ptrB++;
	*ptr = *ptrB;ptr++;
	ptrB = (BYTE *)&word;
    *ptr = *ptrB;ptr++;

// number of cols
 	ptrW = &word;
	*ptrW = (WORD)num_cols;
	ptrB = (BYTE *)&word;ptrB++;
	*ptr = *ptrB;ptr++;
	ptrB = (BYTE *)&word;
    *ptr = *ptrB;ptr++;

// 	*ptrW = (WORD)num_cols;ptrW++;
// 	ptr = (LPBYTE)ptrW;
// number of components in farme
 	*ptr = 0x03;ptr++;
//frame component specification
//Y
// identifier
 	*ptr = 0x01;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;
//Quantization table selector
 	*ptr = 0x00;ptr++;
//Cb
// identifier
 	*ptr = 0x02;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;
//Quantization table selector
 	*ptr = 0x01;ptr++;
//Cr
// identifier
 	*ptr = 0x03;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;
//Quantization table selector
 	*ptr = 0x01;ptr++;
// Define Huffman Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc4;ptr++;
// length
 	*ptr = 0x00;ptr++;
// 	*ptr = 0x1f;ptr++;
 	*ptr = 0xa2;ptr++;
// Table class/identifier
 	*ptr = 0x00;ptr++; // Y DC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
 	{ 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[12] =
 	  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
 	    for(i=0;i<12;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Define Huffman Table
// 	*ptr = 0xff;ptr++;
// 	*ptr = 0xc4;ptr++;
// length
// 	*ptr = 0x00;ptr++;
// 	*ptr = 0xb5;ptr++;
// 	*ptrW = 0x113;ptr++;ptr++; ???
// Table class/identifier
 	*ptr = 0x10;ptr++; // Y AC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	  {  0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[162] =
	  { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	    0xf9, 0xfa };
 	    for(i=0;i<162;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Define Huffman Table
// 	*ptr = 0xff;ptr++;
// 	*ptr = 0xc4;ptr++;
// length
// 	*ptr = 0x00;ptr++;
// 	*ptr = 0x1f;ptr++;
// Table class/identifier
 	*ptr = 0x01;ptr++; // Ch DC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	  {  0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[12] =
	  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
 	    for(i=0;i<12;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }

// Define Huffman Table
// 	*ptr = 0xff;ptr++;
// 	*ptr = 0xc4;ptr++;
// length
// 	*ptr = 0x00;ptr++;
// 	*ptr = 0xb5;ptr++;
// 	*ptrW = 0x113;ptr++;ptr++; ???
// Table class/identifier
 	*ptr = 0x11;ptr++; // Ch AC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	      {  0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
 	 	    for(i=0;i<16;i++){
 			    *ptr = Qelem[i];ptr++;
 		    }
 	    }
// Huffman values
 	{BYTE Qelem[162] =
	  { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	    0xf9, 0xfa };
 	 	    for(i=0;i<162;i++){
 			    *ptr = Qelem[i];ptr++;
 		    }
 	    }
// scan header structure
 	*ptr = 0xff;ptr++;
 	*ptr = 0xda;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x0c;ptr++;
// number of components
 	*ptr = 0x03;ptr++;
// scan component selector
 	*ptr = 0x01;ptr++;
//DC and AC coding table selector --
 	*ptr = 0x00;ptr++;
// scan component selector
 	*ptr = 0x02;ptr++;
//DC and AC coding table selector
 	*ptr = 0x11;ptr++;
// scan component selector
 	*ptr = 0x03;ptr++;
//DC and AC coding table selector
 	*ptr = 0x11;ptr++;
// start of spectral selector or predictor selection
 	*ptr = 0x00;ptr++;
// end of spectral selection
 	*ptr = 0x3f;ptr++;
// succesive approximation bit position high/ low
 	*ptr = 0x00;ptr++;

 	test = ptr - jpgOut;
    return test;

// 	test = ptr - jpgOut;
// 	printf("end---------------lenH= %d\n",test);

/*

	ptrW = (WORD *)ptr;
	*ptrW = (WORD)num_cols;ptrW++;
	*ptrW = (WORD)num_rows;ptrW++;
	ptr = (LPBYTE)ptrW;
	*ptr = 3;ptr++;
	*ptr = 1;ptr++;
	*ptr = downSize_VCB;ptr++;
	*ptr = downSize_HCB;ptr++;
	*ptr = downSize_VCR;ptr++;
	*ptr = downSize_HCR;ptr++;
*/
// 12
}


int setJPGHeaderTest(LPBYTE jpgOut, int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR){
	LPBYTE ptr;
	WORD word;
	WORD *ptrW;
	BYTE *ptrB;
	int i;
//	BYTE v;

	int test = 0;

	ptr = jpgOut;

// start of image
	*ptr = 0xff;ptr++;
	*ptr = 0xd8;ptr++;
// application marker
	*ptr = 0xff;ptr++;
	*ptr = 0xe0;ptr++;
// length
	*ptr = 0x00;ptr++;
	*ptr = 0x10;ptr++;

// Identifier
	*ptr = 0x4A;ptr++;
	*ptr = 0x46;ptr++;
	*ptr = 0x49;ptr++;
	*ptr = 0x46;ptr++;
	*ptr = 0x00;ptr++;
// Version
	*ptr = 0x01;ptr++;
	*ptr = 0x02;ptr++;
// Density units
	*ptr = 0x00;ptr++;
//x density
	*ptr = 0x00;ptr++;
	*ptr = 0x0a;ptr++;
//y density
	*ptr = 0x00;ptr++;
	*ptr = 0x0a;ptr++;
// thumbnail width
	*ptr = 0x00;ptr++;
// thumbnail height
	*ptr = 0x00;ptr++;

//	Quantization Table
	*ptr = 0xff;ptr++;
	*ptr = 0xdb;ptr++;
// length
	*ptr = 0x00;ptr++;
	*ptr = 0x43;ptr++;
// element precision / identifier
	*ptr = 0x00;ptr++;
// elements

	    for(i=0;i<64;i++){
		    *ptr = 1;ptr++;
	    }


//	Quantization Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xdb;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x43;ptr++;
// element precision / identifier
 	*ptr = 0x01;ptr++;
// elements
 	{
 	    for(i=0;i<64;i++){
		    *ptr = 1;ptr++;
	    }
    }
// 	Start of frame
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc0;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x11;ptr++;
// sample precision
 	*ptr = 0x08;ptr++;
// number of rows
 	ptrW = &word;
	*ptrW = (WORD)num_rows;
	ptrB = (BYTE *)&word;ptrB++;
	*ptr = *ptrB;ptr++;
	ptrB = (BYTE *)&word;
    *ptr = *ptrB;ptr++;

// number of cols
 	ptrW = &word;
	*ptrW = (WORD)num_cols;
	ptrB = (BYTE *)&word;ptrB++;
	*ptr = *ptrB;ptr++;
	ptrB = (BYTE *)&word;
    *ptr = *ptrB;ptr++;

// 	*ptrW = (WORD)num_cols;ptrW++;
// 	ptr = (LPBYTE)ptrW;
// number of components in farme
 	*ptr = 0x03;ptr++;
//frame component specification
//Y
// identifier
 	*ptr = 0x01;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;
// 	*ptr = 0x22;ptr++;
//Quantization table selector
 	*ptr = 0x00;ptr++;
//Cb
// identifier
 	*ptr = 0x02;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;

//Quantization table selector
 	*ptr = 0x01;ptr++;
//Cr
// identifier
 	*ptr = 0x03;ptr++;
//Horizontal/Vertical sampling factor
 	*ptr = 0x11;ptr++;

//Quantization table selector
 	*ptr = 0x01;ptr++;
// Define Huffman Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc4;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x1f;ptr++;
// Table class/identifier
 	*ptr = 0x00;ptr++; // Y DC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
 	{ 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[12] =
 	  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
 	    for(i=0;i<12;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Define Huffman Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc4;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0xb5;ptr++;
// 	*ptrW = 0x113;ptr++;ptr++; ???
// Table class/identifier
 	*ptr = 0x10;ptr++; // Y AC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	  {  0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[162] =
	  { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	    0xf9, 0xfa };
 	    for(i=0;i<162;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Define Huffman Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc4;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x1f;ptr++;
// Table class/identifier
 	*ptr = 0x01;ptr++; // Ch DC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	  {  0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
 	    for(i=0;i<16;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }
// Huffman values
 	{BYTE Qelem[12] =
	  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
 	    for(i=0;i<12;i++){
		    *ptr = Qelem[i];ptr++;
	    }
    }

// Define Huffman Table
 	*ptr = 0xff;ptr++;
 	*ptr = 0xc4;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0xb5;ptr++;
// 	*ptrW = 0x113;ptr++;ptr++; ???
// Table class/identifier
 	*ptr = 0x11;ptr++; // Ch AC
// number of Huffman codes of length
 	{BYTE Qelem[16] =
	      {  0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
 	 	    for(i=0;i<16;i++){
 			    *ptr = Qelem[i];ptr++;
 		    }
 	    }
// Huffman values
 	{BYTE Qelem[162] =
	  { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	    0xf9, 0xfa };
 	 	    for(i=0;i<162;i++){
 			    *ptr = Qelem[i];ptr++;
 		    }
 	    }
// scan header structure
 	*ptr = 0xff;ptr++;
 	*ptr = 0xda;ptr++;
// length
 	*ptr = 0x00;ptr++;
 	*ptr = 0x0c;ptr++;
// number of components
 	*ptr = 0x03;ptr++;
// scan component selector
 	*ptr = 0x01;ptr++;
//DC and AC coding table selector --
 	*ptr = 0x00;ptr++;
// scan component selector
 	*ptr = 0x02;ptr++;
//DC and AC coding table selector
 	*ptr = 0x11;ptr++;
// scan component selector
 	*ptr = 0x03;ptr++;
//DC and AC coding table selector
 	*ptr = 0x11;ptr++;
// start of spectral selector or predictor selection
 	*ptr = 0x00;ptr++;
// end of spectral selection
 	*ptr = 0x3f;ptr++;
// succesive approximation bit position high/ low
 	*ptr = 0x00;ptr++;


 	test = ptr - jpgOut;
// 	printf("end---------------lenH= %d\n",test);
 	return test;

}


void setJPGHeaderOld(LPBYTE jpgOut, int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR){
	LPBYTE ptr;
	WORD *ptrW;

	ptr = jpgOut;

	*ptr = 0xff;ptr++;

	*ptr = 0xd8;ptr++;
	ptrW = (WORD *)ptr;
	*ptrW = (WORD)num_cols;ptrW++;
	*ptrW = (WORD)num_rows;ptrW++;
	ptr = (LPBYTE)ptrW;
	*ptr = 3;ptr++;
	*ptr = 1;ptr++;
	*ptr = downSize_VCB;ptr++;
	*ptr = downSize_HCB;ptr++;
	*ptr = downSize_VCR;ptr++;
	*ptr = downSize_HCR;ptr++;

// 12
}

void setCodeBitTest(BYTE *b, int xn, int v){
	int nX;
	BYTE mask;
	LPBYTE ptr;

		nX = xn/8;
		ptr = b+nX;

printf("ptr+xn= %lu \n", (long unsigned)(ptr+xn%8));

		mask=LEFT_BIT;
		mask >>=(xn%8);
		if(v!=0){
			*ptr = *ptr | mask;
		}
		else {
			*ptr = *ptr & (~mask);
		}
}

int reloadCodeBitsTest(DWORD *b, int bLen,  BYTE *code, int numBits){
	int i, v;
	LPBYTE ptr = (BYTE*)b;

//	for(i=0;i<bLen;i++){
		for(i=0;i<30;i++){
		v = getCodeBit(ptr, i);
//if(i<30)
  printf("code= %lu, v= %d, numBits= %d \n", (long unsigned)(code), v, numBits);
		setCodeBitTest(code, numBits, v);
//if(i<30)
  printCodeBits(code, i+1);
		numBits++;
	}
	return numBits;
}

int DCT_Quant_Encode(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR,
		int quantLevel, LPBYTE *jpgOut){

	int block[8*8];
	float T[8*8];
	float Ttrans[8*8];
	float D[8*8];
	int DQ[8*8];
	int QL[8*8];
	int QCh[8*8];
	int ZZ[8*8];
	int RL[200];
//	char code[1000];
	BYTE code[1000];
	int rl;
    int row, col, idx;
    int i, j;
    SBYTE *jpgOutTmp, *ptrOutp;
    SBYTE *jpgOutReal;
    int byteSize = num_rows*num_cols;
    int lenLoc, lenGlb, lenGlbCb, lenGlbCr, lenH;
    int num_rowsL, num_colsL, num_rowsCh, num_colsCh;
    int DQlast = 0, DQnew;

    BYTE byteBuff[8];
    int bitInBuff = 0;
    int rowLimit, colLimit;


    jpgOutTmp = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
    if(jpgOutTmp == NULL) return -200;


//    printf(" encode2: num_rows= %d, num_cols= %d, downSize_VCB= %d, downSize_HCB= %d, downSize_VCR= %d, downSize_HCR= %d  \n", num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR);
    createCosMatrix(T);
    transposeMatrix(T, Ttrans);


    createQuantL(QL, quantLevel);
    createQuantCh(QCh, quantLevel);

//    createQuantLTest(QL, quantLevel);
//    createQuantChTest(QCh, quantLevel);


//    printMatrixI(QL, "QL");
//    printMatrixI(QCh,"QCh");
    num_rowsL = num_rows/downSize_VL;
    num_colsL = num_cols/downSize_HL;


   	rowLimit = num_rowsL/8; colLimit = num_colsL/8;
   	if(num_rowsL%8 != 0) rowLimit++;
   	if(num_colsL%8 != 0) colLimit++;

   printf("Y rowLimit= %d, colLimit= %d\n", rowLimit, colLimit);


    lenGlb = 0;
// Y
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsL)&&(col*8+j<num_colsL)){
	    	    	idx = (row*8+i)*num_cols+col*8+j;
//	    	    	block[i*8+j] = outptr0[idx]-128;
	    	    	block[i*8+j] = outptr0[idx];
	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//  	if(row == 35 && col == 39){

	    	  DCT(block, D, T, Ttrans);
//	    	  fdct_2(block, D);
//	    	  DCTcos(block, D);

	    	  quantDCT(QL, D, DQ);
//	    	  quantDCT_2(D, float *fdtbl, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;
	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

//	    	  EncodeOld(RL,rl,code);
	    	  lenLoc= Encode(RL, rl, code);
//	    	  lenGlb= reloadCodeBits((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb);
	    	  lenGlb= reloadCodeBits_buff((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb, byteBuff, &bitInBuff, 8);


if(row == 0 && col == 0){
	printf("Y row= %d, col= %d\n",row, col);
	printMatrixI(block, "block");

      	printMatrix(D,"D");
    	printMatrixI(DQ, "DQ");
    	printMatrixI(ZZ, "ZZ");
    	printVectorI(RL, "RL");
    	printf(" out; rl= %d\n",rl);
//    	printf(" lenLoc= %d\n",lenLoc);
//    	printCodeBits(code, lenLoc);

}

//    	printCodeBits(code, lenLoc);
//  	}

	    }
    }

    num_rowsCh = num_rows/downSize_VCB;
    num_colsCh = num_cols/downSize_HCB;

   	rowLimit = num_rowsCh/8; colLimit = num_colsCh/8;
   	if(num_rowsCh%8 != 0) rowLimit++;
   	if(num_colsCh%8 != 0) colLimit++;

//  	printf("Lum;  lenGlb= %d, bitInBuff= %d\n",lenGlb, bitInBuff);

	   printf("Cb rowLimit= %d, rowLimit= %d\n",rowLimit, colLimit);

    lenGlbCb = 0; lenGlbCr = 0;
    ptrOutp= outptr0;
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsCh)&&(col*8+j<num_colsCh)){
	    	    	idx = (row*8+i)*num_colsCh+col*8+j;
//	    	    	block[i*8+j] = outptr1[idx]-128;
	    	    	block[i*8+j] = outptr1[idx];
	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//    	printMatrixB(block, "block");

	    	  DCT(block, D, T, Ttrans);
//	    	  DCTcos(block, D);
	    	  quantDCT(QL, D, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;
	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

	    	  lenLoc= Encode(RL, rl, code);
//	    	  lenGlbCb= reloadCodeBits((DWORD *)code, lenLoc,  ptrOutp, lenGlbCb);
	    	  lenGlb= reloadCodeBits_buff((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb, byteBuff, &bitInBuff, 8);


if(row == 0 && col == 0){
  	printf("Cb row= %d, col= %d\n",row, col);
  	printMatrixI(block, "block");

   	printMatrix(D,"D");
	printMatrixI(DQ, "DQ");
	printMatrixI(ZZ, "ZZ");
	printVectorI(RL, "RL");
	printf(" out; rl= %d\n",rl);
//    	printf(" lenLoc= %d\n",lenLoc);
//	printCodeBits(code, lenLoc);

}

//  	}

	    }
    }
//    printf("Cr; row= %d, col= %d, lenGlbCb= %d\n", row, col, lenGlbCb);
//    fflush(stdout);


    num_rowsCh = num_rows/downSize_VCR;
    num_colsCh = num_cols/downSize_HCR;

   	rowLimit = num_rowsCh/8; colLimit = num_colsCh/8;
   	if(num_rowsCh%8 != 0) rowLimit++;
   	if(num_colsCh%8 != 0) colLimit++;

//    printf("Cr; num_rowsCh= %d, num_colsCh= %d\n", num_rowsCh, num_colsCh);
//    fflush(stdout);

    ptrOutp= outptr1;
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsCh)&&(col*8+j<num_colsCh)){
	    	    	idx = (row*8+i)*num_colsCh+col*8+j;
//	    	    	block[i*8+j] = outptr2[idx]-128;
	    	    	block[i*8+j] = outptr2[idx];

	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//    if(row == 0 && col < 2){

//    	printMatrixB(block, "block");

	    	  DCT(block, D, T, Ttrans);
//	    	  DCTcos(block, D);
	    	  quantDCT(QL, D, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;
	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

	    	  lenLoc= Encode(RL, rl, code);
//	    	  lenGlbCr= reloadCodeBits((DWORD *)code, lenLoc,  ptrOutp, lenGlbCr);
	    	  lenGlb= reloadCodeBits_buff((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb, byteBuff, &bitInBuff, 8);


if(row == 0 && col == 0){

   	printf("Cr row= %d, col= %d\n",row, col);
   	printMatrixI(block, "block");
   	printMatrix(D,"D");
 	printMatrixI(DQ, "DQ");
 	printMatrixI(ZZ, "ZZ");
  	printVectorI(RL, "RL");
  	printf(" out; rl= %d, lenGlbCr= %d, lenLoc= %d\n",rl, lenGlbCr, lenLoc);
// 	printCodeBits(code, lenLoc);

}

//  	}

	    }
    }

//     printf("Cr; row= %d, col= %d, lenGlbCr= %d\n", row, col, lenGlbCr);
//     printf("Cr; num_rowsCh= %d, num_colsCh= %d\n", num_rowsCh, num_colsCh);
//     fflush(stdout);



//	printf(" lenGlb= %d \n",lenGlb);
//	printf(" lenGlbCb= %d\n",lenGlbCb);
//	printf(" lenGlbCr= %d\n",lenGlbCr);
//fflush(stdout);

	lenH = 623;
	byteSize = lenH + (lenGlb+lenGlbCb+lenGlbCr)/8+20;
	jpgOutReal = (SBYTE*)yl_calloc("JPG1", byteSize, sizeof(BYTE));
    if(jpgOutReal == NULL) return -201;


	*jpgOut = (BYTE *)jpgOutReal;


	lenH = setJPGHeader(jpgOutReal, num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);

//	lenH = setJPGHeaderTest(jpgOutReal, num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);
printf("setJPGHeader; ------------------ lenH= %d\n", lenH);
    lenLoc = lenH*8;
//    printf("1. Y lenLoc= %d, lenGlb= %d\n",lenLoc, lenGlb);

    lenLoc= reloadCodeBits_Bytes((DWORD *)jpgOutTmp, lenGlb,  jpgOutReal, lenLoc);

//    printf("2. Y lenLoc= %d, lenGlb= %d\n",lenLoc, lenGlb);

//    printCodeBits(jpgOutReal+lenH, 30);
//    printf("Cb; lenLoc= %d\n",lenLoc);

//   printf("1. Cb lenLoc= %d, lenGlbCb= %d\n",lenLoc, lenGlbCb);

//    lenLoc= reloadCodeBits((DWORD *)outptr0, lenGlbCb,  jpgOutReal, lenLoc);
    lenLoc= reloadCodeBits_Bytes((DWORD *)outptr0, lenGlbCb,  jpgOutReal, lenLoc);

//   printf("2. Cb lenLoc= %d, lenGlbCb= %d\n",lenLoc, lenGlbCb);

// printf("1. Cr lenLoc= %d, lenGlbCr= %d\n",lenLoc, lenGlbCr);

//   lenLoc= reloadCodeBits((DWORD *)outptr1, lenGlbCr,  jpgOutReal, lenLoc);
   lenLoc= reloadCodeBits_Bytes((DWORD *)outptr1, lenGlbCr,  jpgOutReal, lenLoc);

//  printf("2. Cr lenLoc= %d, lenGlbCr= %d\n",lenLoc, lenGlbCr);

   lenGlb = lenLoc/8;
printf(" lenGlb= %d, byteSize= %d\n",lenGlb, byteSize);
   {// add EOI
   	int r = lenLoc%8;
   	printf(" r= %d\n",r);
       if(r != 0) lenGlb++;
      jpgOutReal[lenGlb++] = 0xff;jpgOutReal[lenGlb++] = 0xd9;
   }

    yl_free(jpgOutTmp);

   return lenGlb;
}
int DCT_Quant_EncodeClasic(int num_rows, int num_cols, int downSize_VL, int downSize_HL, int downSize_VCB, int downSize_HCB, int downSize_VCR,  int downSize_HCR,
		int quantLevel, LPBYTE *jpgOut){

	int block[8*8];
	float T[8*8];
	float Ttrans[8*8];
	float D[8*8];
	int DQ[8*8];
	int QL[8*8];
	int QCh[8*8];
	int ZZ[8*8];
	int RL[200];
//	char code[1000];
	BYTE code[1000];
	int rl;
    int row, col, idx;
    int i, j;
    SBYTE *jpgOutTmp, *ptrOutp;
    SBYTE *jpgOutReal;
    int byteSize = num_rows*num_cols;
    int lenLoc, lenGlb, lenGlbCb, lenGlbCr, lenH;
    int num_rowsL, num_colsL, num_rowsCh, num_colsCh;
    int DQlast = 0, DQnew;

    BYTE byteBuff[8];
    int bitInBuff = 0;
    int rowLimit, colLimit;


    jpgOutTmp = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
    if(jpgOutTmp == NULL) return -200;


//    printf(" encode2: num_rows= %d, num_cols= %d, downSize_VCB= %d, downSize_HCB= %d, downSize_VCR= %d, downSize_HCR= %d  \n", num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR);

    createCosMatrix(T);
    transposeMatrix(T, Ttrans);

//    printMatrix(T, "T");
//    printMatrix(Ttrans,"Ttrans");

//    createQuantL(QL, quantLevel);
//    createQuantCh(QCh, quantLevel);

    createQuantLTest(QL, quantLevel);
    createQuantChTest(QCh, quantLevel);


//    printMatrixI(QL, "QL");
//    printMatrixI(QCh,"QCh");

    setHufTbls();
    lenGlb = 0;
    num_rowsL = num_rows/downSize_VL;
    num_colsL = num_cols/downSize_HL;


   	rowLimit = num_rowsL/8; colLimit = num_colsL/8;
   	if(num_rowsL%8 != 0) rowLimit++;
   	if(num_colsL%8 != 0) colLimit++;

   printf("Y rowLimit= %d, colLimit= %d\n", rowLimit, colLimit);

// Y
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsL)&&(col*8+j<num_colsL)){
	    	    	idx = (row*8+i)*num_colsL+col*8+j;
	    	    	block[i*8+j] = outptr0[idx]-128;
	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//  	if(row == 35 && col == 39){

	    	  DCT(block, D, T, Ttrans);
//	    	  DCTcos(block, D);

	    	  quantDCT(QL, D, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;
	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

//	    	  EncodeOld(RL,rl,code);
//	    	  lenLoc= Encode(RL, rl, code);

    	      lenLoc= EncodeClasic(RL, rl, code, 1);

//	    	  lenLoc=  EncodeClasicTest(RL, rl, code, 1, row, col);

//	    	  lenGlb= reloadCodeBits((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb);
	    	  lenGlb= reloadCodeBits_buff((DWORD *)code, lenLoc,  jpgOutTmp, lenGlb, byteBuff, &bitInBuff, 8);

//if(RL[0] == -495)
//  	   printf("Y row= %d, col= %d\n",row, col);
/*
if(row == 0 && col == 0){
//if(row == 332 && col == 172){
	printf("Y row= %d, col= %d\n",row, col);
	printMatrixI(block, "block");
//      	printMatrix(D,"D");
//    	printMatrixI(DQ, "DQ");
//    	printMatrixI(ZZ, "ZZ");
    	printVectorI(RL, "RL");
    	printf(" out; rl= %d\n",rl);
//    	printf(" lenLoc= %d\n",lenLoc);
    	printCodeBits(code, lenLoc);
}
*/
//    	printCodeBits(code, lenLoc);
//  	}

	    }
    }

    num_rowsCh = num_rows/downSize_VCB;
    num_colsCh = num_cols/downSize_HCB;
    lenGlbCb = 0; lenGlbCr = 0;

   	rowLimit = num_rowsCh/8; colLimit = num_colsCh/8;
   	if(num_rowsCh%8 != 0) rowLimit++;
   	if(num_colsCh%8 != 0) colLimit++;

//  	printf("Lum;  lenGlb= %d, bitInBuff= %d\n",lenGlb, bitInBuff);

	   printf("Cb rowLimit= %d, num_colsCh/8= %d\n",rowLimit, colLimit);
    DQlast = 0;
    ptrOutp= outptr0;
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsCh)&&(col*8+j<num_colsCh)){
	    	    	idx = (row*8+i)*num_colsCh+col*8+j;
	    	    	block[i*8+j] = outptr1[idx]-128;
	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//    	printMatrixB(block, "block");

	    	  DCT(block, D, T, Ttrans);
//	    	  DCTcos(block, D);
	    	  quantDCT(QL, D, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;

	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

//	    	  lenLoc= Encode(RL, rl, code);
	    	  lenLoc= EncodeClasic(RL, rl, code, 0);
//	    	  lenGlbCb= reloadCodeBits((DWORD *)code, lenLoc,  ptrOutp, lenGlbCb);
	    	  lenGlbCb= reloadCodeBits_buff((DWORD *)code, lenLoc,  ptrOutp, lenGlbCb, byteBuff, &bitInBuff, 8);

//    	  if(RL[0] == -495)
//   	    	   printf("Cb row= %d, col= %d\n",row, col);

/*
if(row == 0 && col == 0){
//if(row == 332 && col == 172){
  	printf("Cb row= %d, col= %d\n",row, col);
//  	printMatrixI(block, "block");
//   	printMatrix(D,"D");
//	printMatrixI(DQ, "DQ");
//	printMatrixI(ZZ, "ZZ");
	printVectorI(RL, "RL");
	printf(" out; rl= %d\n",rl);
    	printf(" lenLoc= %d\n",lenLoc);
	printCodeBits(code, lenLoc);
}
*/
//  	}

	    }
    }
//    printf("Cb; row= %d, col= %d, lenGlbCb= %d\n", row, col, lenGlbCb);
//    fflush(stdout);
  	printf("Cb;  lenGlbCb= %d, bitInBuff= %d\n",lenGlbCb, bitInBuff);

    num_rowsCh = num_rows/downSize_VCR;
    num_colsCh = num_cols/downSize_HCR;

   	rowLimit = num_rowsCh/8; colLimit = num_colsCh/8;
   	if(num_rowsCh%8 != 0) rowLimit++;
   	if(num_colsCh%8 != 0) colLimit++;


    printf("Cr rowLimit= %d, num_colsCh/8= %d\n", rowLimit, colLimit);
//    printf("Cr; num_rowsCh= %d, num_colsCh= %d\n", num_rowsCh, num_colsCh);
//    fflush(stdout);

    DQlast = 0;
    ptrOutp= outptr1;
    for (row = 0; row < rowLimit; row++) {
	    for (col = 0; col < colLimit; col++) {

	    	for(i=0;i<8;i++)
	    	  for(j=0;j<8;j++)

	    	    if((row*8+i<num_rowsCh)&&(col*8+j<num_colsCh)){
	    	    	idx = (row*8+i)*num_colsCh+col*8+j;
	    	    	block[i*8+j] = outptr2[idx]-128;
	    	    }
	    	    else
	    	    	block[i*8+j] = 0;
// compress block
//  	if(row == 150 && col == 154){
//    if(row == 0 && col < 2){

//    	printMatrixB(block, "block");

	    	  DCT(block, D, T, Ttrans);
//	    	  DCTcos(block, D);
	    	  quantDCT(QL, D, DQ);
	    	  DQnew = DQ[0];
	    	  DQ[0] -= DQlast;
	    	  DQlast = DQnew;

	    	  zigZag(DQ, ZZ);
	    	  rl = RLE(ZZ, RL);

//	    	  lenLoc= Encode(RL, rl, code);
	    	  lenLoc= EncodeClasic(RL, rl, code, 0);
//	    	  lenGlbCr= reloadCodeBits((DWORD *)code, lenLoc,  ptrOutp, lenGlbCr);
	    	  lenGlbCr= reloadCodeBits_buff((DWORD *)code, lenLoc,  ptrOutp, lenGlbCr, byteBuff, &bitInBuff, 8);

//   	  if(RL[0] == -495)
//  	    	   printf("Cr row= %d, col= %d\n",row, col);

/*
if(row == 0 && col == 0){
//if(row == 332 && col == 172){
   	printf("Cr row= %d, col= %d\n",row, col);
//   	printMatrixI(block, "block");
//   	printMatrix(D,"D");
// 	printMatrixI(DQ, "DQ");
// 	printMatrixI(ZZ, "ZZ");
  	printVectorI(RL, "RL");
  	printf(" out; rl= %d, lenGlbCr= %d, lenLoc= %d\n",rl, lenGlbCr, lenLoc);
 	printCodeBits(code, lenLoc);
}
*/
//  	}

	    }
    }

    printf("1. Cr;  lenGlbCr= %d, bitInBuff= %d\n",lenGlbCr, bitInBuff);
    lenGlbCr= reloadCodeBits((DWORD *)byteBuff, bitInBuff,  (BYTE *)ptrOutp, lenGlbCr);
    printf("2. Cr;  lenGlbCr= %d, bitInBuff= %d\n",lenGlbCr, bitInBuff);
    fflush(stdout);


//	printf(" lenGlb= %d \n",lenGlb);
//	printf(" lenGlbCb= %d\n",lenGlbCb);
//	printf(" lenGlbCr= %d\n",lenGlbCr);
//fflush(stdout);

	lenH = 623;
	byteSize = lenH + (lenGlb+lenGlbCb+lenGlbCr)/8+20;
	jpgOutReal = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
    if(jpgOutReal == NULL) return -201;


//printf(" byteSize= %d\n", byteSize);
//fflush(stdout);

//    if(jpgOut == NULL ){
//    	printf(" NULL\n");
//    	fflush(stdout);
//    }
	*jpgOut = (BYTE *)jpgOutReal;

//	printf(" byteSize= %d\n", byteSize);
//	fflush(stdout);


//    setJPGHeader(jpgOutReal, num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);
	lenH = setJPGHeaderTest((BYTE *)jpgOutReal, num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);
printf(" lenH= %d\n", lenH);
    lenLoc = lenH*8;
//    printf("header; lenLoc= %d \n", lenLoc);
//    printCodeBits(jpgOutTmp, 30);

//   printf("Y lenLoc= %d, lenGlb= %d\n",lenLoc, lenGlb);
//    lenLoc= reloadCodeBits((DWORD *)jpgOutTmp, lenGlb,  jpgOutReal, lenLoc);
    printf("1. Y lenLoc= %d, lenGlb= %d\n",lenLoc, lenGlb);

    lenLoc= reloadCodeBits_Bytes((DWORD *)jpgOutTmp, lenGlb,  jpgOutReal, lenLoc);

    printf("2. Y lenLoc= %d, lenGlb= %d\n",lenLoc, lenGlb);

//    printCodeBits(jpgOutReal+lenH, 30);
//    printf("Cb; lenLoc= %d\n",lenLoc);

   printf("1. Cb lenLoc= %d, lenGlbCb= %d\n",lenLoc, lenGlbCb);

//    lenLoc= reloadCodeBits((DWORD *)outptr0, lenGlbCb,  jpgOutReal, lenLoc);
    lenLoc= reloadCodeBits_Bytes((DWORD *)outptr0, lenGlbCb,  jpgOutReal, lenLoc);

   printf("2. Cb lenLoc= %d, lenGlbCb= %d\n",lenLoc, lenGlbCb);

 printf("1. Cr lenLoc= %d, lenGlbCr= %d\n",lenLoc, lenGlbCr);

//   lenLoc= reloadCodeBits((DWORD *)outptr1, lenGlbCr,  jpgOutReal, lenLoc);
   lenLoc= reloadCodeBits_Bytes((DWORD *)outptr1, lenGlbCr,  jpgOutReal, lenLoc);

  printf("2. Cr lenLoc= %d, lenGlbCr= %d\n",lenLoc, lenGlbCr);

   lenGlb = lenLoc/8;
printf(" lenGlb= %d, byteSize= %d\n",lenGlb, byteSize);
   {// add EOI
   	int r = lenLoc%8;
   	printf(" r= %d\n",r);
       if(r != 0) lenGlb++;
      jpgOutReal[lenGlb++] = 0xff;jpgOutReal[lenGlb++] = 0xd9;

   }
//   lenGlb = shiftAlloXFF(jpgOutReal, lenGlb, lenH);

  printf("after shift; lenGlb= %d, byteSize= %d\n",lenGlb, byteSize);
//    lenGlb = lenLoc/8;
//    if(lenLoc%8 != 0) lenGlb++;
//    printf(" lenGlb= %d, byteSize= %d\n",lenGlb, byteSize);
    fflush(stdout);

    yl_free(jpgOutTmp);

   return lenGlb;
}
void addZeroByte(LPBYTE jpgOutReal, int n, int lenGlb){
	int i;
	for(i=lenGlb;i>=n;i--){
		jpgOutReal[i+1] = jpgOutReal[i];
	}
	jpgOutReal[n] = 0;
}
int shiftAlloXFF(LPBYTE jpgOutReal, int lenGlb, int lenH){
	int i;
	int d, count = 0;
	for(i=lenH;i<lenGlb-2;i++){

		if(jpgOutReal[i] == 0xFF){
			count++;
			d = i-lenH;

//if(count < 100)
// printf(" shift; i= %d, i-lenH= %d\n",i, d);

            addZeroByte(jpgOutReal, i+1, lenGlb);
//if(count < 100)
//  printCodeBits(&jpgOutReal[i], 16);
            lenGlb++;
if(i%100000 == 0){
	printf(" shift; i= %d, lenGlb= %d\n",i, lenGlb);
    fflush(stdout);
}
		}
	}
 printf(" count= %d\n",count);

	return lenGlb;
}



int deCompressImage_JPG(LPBYTE jpgIn, LPIMGDATA imgOut){

//	 Color space conversion ( RGB to YCbCr).
  int num_rows;
  int num_cols;
  int byteSize;
  int downSize_VCB, downSize_VCR, downSize_HCB, downSize_HCR;
  int ret;
  LPBYTE ptr;
  WORD *ptrW;

	ptr = jpgIn;

	if(*ptr != 0xff || *(ptr+1) != 0xd8)
		return -7;

    ptr += 2;
	ptrW = (WORD *)ptr;

	num_cols = *ptrW;ptrW++;
	num_rows = *ptrW;ptrW++;
	ptr = (LPBYTE)ptrW;
//	*ptr = 3;ptr++;
//	*ptr = 1;ptr++;
	ptr++;ptr++;
	downSize_VCB = *ptr; ptr++;downSize_HCB = *ptr;ptr++;
	downSize_VCR = *ptr; ptr++;downSize_HCR = *ptr;ptr++;


printf(" num_rows= %d, num_cols= %d, downSize_VCB= %d, downSize_HCB= %d, downSize_VCR= %d, downSize_HCR= %d  \n", num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR);
fflush(stdout);

    byteSize = num_cols*num_rows;


outptr0 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
outptr1 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
outptr2 = (SBYTE *)yl_calloc("JPG1", byteSize, sizeof(BYTE));
if(outptr0 == NULL || outptr1 == NULL || outptr2 == NULL) return -100;

printf(" byteSize= %d \n", byteSize);
fflush(stdout);

ret = DCT_Quant_Decode(jpgIn, num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR, QUANT_LEVEL);

UpSampling(num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR,  downSize_HCR);

//printBlocksFrom_JPG(num_rows, num_cols, 332, 172);

ColorSpaceConversion_JPG_BMP(imgOut, num_rows, num_cols);

//printBlocksFrom_RGB(imgOut, 332, 172);

//printf(" ret= %d \n", ret);
//fflush(stdout);


//printf(" ret= %d \n", ret);
//fflush(stdout);

yl_free(outptr0);
yl_free(outptr1);
yl_free(outptr2);
return ret;
}

void reloadBytesExpand(LPBYTE ptrIn, int num_rowsIn, int num_colsIn, LPBYTE ptrOut, int num_rowsOut, int num_colsOut){
	int rowIn, colIn;
	int rowOut, colOut;
	int ratioRow, ratioCol;
	int rR, rC;
	if(num_rowsIn <= 0 || num_colsIn <= 0) return;
	ratioRow = (int)((float)num_rowsOut/(float)num_rowsIn+0.5); ratioCol = (int)((float)num_colsOut/(float)num_colsIn+0.5);
	if(ratioRow <= 1 && ratioCol <= 1) return;

	rowOut = 0;
	for(rowIn = 0;rowIn<num_rowsIn;rowIn++ ){
		for(rR = 0;rR<ratioRow;rR++ ){
		  colOut = 0;
		  for(colIn = 0;colIn<num_colsIn;colIn++ ){
			  for(rC = 0;rC<ratioCol;rC++ ){
				  ptrOut[rowOut*num_colsOut+colOut] = ptrIn[rowIn*num_colsIn+colIn];
				  colOut++;
			  }
		  }
		  rowOut++;
		}

	}
}
void reloadBytes(LPBYTE ptrIn, LPBYTE ptrOut, int num_rows, int num_cols){
	int i;
	for(i=0;i<num_rows*num_cols;i++)
		ptrOut[i] = ptrIn[i];
}
int UpSampling(int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR){
    LPBYTE jpgOutTmp;
    int byteSize = num_rows*num_cols;
    int num_rowsCh, num_colsCh;
    jpgOutTmp = (LPBYTE)yl_calloc("JPG1", byteSize, sizeof(BYTE));
    if(jpgOutTmp == NULL) return -200;

    if(downSize_VCB > 1 || downSize_HCB > 1){
       num_rowsCh = num_rows/downSize_VCB;
       num_colsCh = num_cols/downSize_HCB;
       reloadBytes((BYTE *)outptr1, jpgOutTmp, num_rowsCh, num_colsCh);
       reloadBytesExpand(jpgOutTmp, num_rowsCh, num_colsCh, (BYTE *)outptr1, num_rows, num_cols);
    }
    if(downSize_VCR > 1 || downSize_HCR > 1){
       num_rowsCh = num_rows/downSize_VCR;
       num_colsCh = num_cols/downSize_HCR;
       reloadBytes((BYTE *)outptr2, jpgOutTmp, num_rowsCh, num_colsCh);
       reloadBytesExpand(jpgOutTmp, num_rowsCh, num_colsCh, (BYTE *)outptr2, num_rows, num_cols);
    }

	yl_free(jpgOutTmp);

	return 0;
}
int getDCvalue(LPBYTE ptr, int bit, int *DCval){
	  int a,len,i,j,p, v;
      int bitOut = bit;
//      LPBYTE ptrB = ptr+bitOut;
	  int codeLen[12] = {3,4,5,5,7,8,10,12,14,16,18,20};
	  char* code[12] = {"010\0","011\0","100\0","00\0","101\0","110\0","1110\0",
			    "11110\0","111110\0","1111110\0","11111110\0","111111110\0"};
	  *DCval = 0;
	  for(i=0;i<12;i++)   {
	      a = 1;
	      len = strlen(code[i]);
	      for(j=0;j<len;j++){
	    	  v = getCodeBit(ptr, j+bitOut);
		      if(code[i][j]-'0' != v)
		      a = 0;
	      }
	      if (a==1)	{
	    	  bitOut += len;
		      len = codeLen[i]-len;
		      if(len==0)
		         return bitOut;

		      p=0;
		      for(j=0;j<len;j++)  {
		         p *= 2;
		         v = getCodeBit(ptr, j+bitOut);
		         if(v == 1)
			        p++;
		      }
		      v = getCodeBit(ptr, bitOut);
		      if(v == 0)
		             p = p + 1 - (int)pow(2,len);
		      bitOut += len;
		      *DCval = p;
		      return bitOut;
		}
      }
	  return bitOut;
}


int  getACvalue(LPBYTE ptr, int bit, int *n, int *p){
  int a,len,i,j,k, v;
  int bitOut = bit;
  int codeLen[16][11] = {
    {4 ,3 ,4 ,6 ,8 ,10,12,14,18,25,26},
    {0 ,5 ,8 ,10,13,16,22,23,24,25,26},
    {0 ,6 ,10,13,20,21,22,23,24,25,26},
    {0 ,7 ,11,14,20,21,22,23,24,25,26},
    {0 ,7 ,12,19,20,21,22,23,24,25,26},
    {0 ,8 ,12,19,20,21,22,23,24,25,26},
    {0 ,8 ,13,19,20,21,22,23,24,25,26},
    {0 ,9 ,13,19,20,21,22,23,24,25,26},
    {0 ,9 ,17,19,20,21,22,23,24,25,26},
    {0 ,10,18,19,20,21,22,23,24,25,26},
    {0 ,10,18,19,20,21,22,23,24,25,26},
    {0 ,10,18,19,20,21,22,23,24,25,26},
    {0 ,11,18,19,20,21,22,23,24,25,26},
    {0 ,12,18,19,20,21,22,23,24,25,26},
    {0 ,13,18,19,20,21,22,23,24,25,26},
    {12,17,18,19,20,21,22,23,24,25,26}
  };
  char* code[16][11] = {
		  {"1010\0",  "00\0",  "01\0",  "100\0",  "1011\0",  "11010\0",  "111000\0",  "1111000\0",  "1111110110\0",  "1111111110000010\0",  "1111111110000011\0"},
		  {"\0","1100\0","111001\0","1111001\0","111110110\0","11111110110\0","1111111110000100\0","1111111110000101\0","1111111110000110\0","1111111110000111\0","1111111110001000\0"},
		  {"\0","11011\0","11111000\0","1111110111\0","1111111110001001\0","1111111110001010\0","1111111110001011\0","1111111110001100\0","1111111110001101\0","1111111110001110\0","1111111110001111\0"},
		  {"\0","111010\0","111110111\0","11111110111\0","1111111110010000\0","1111111110010001\0","1111111110010010\0","1111111110010011\0","1111111110010100\0","1111111110010101\0","1111111110010110\0"},
		  {"\0","111011\0","1111111000\0","1111111110010111\0","1111111110011000\0","1111111110011001\0","1111111110011010\0","1111111110011011\0","1111111110011100\0","1111111110011101\0","1111111110011110\0"},
		  {"\0","1111010\0","1111111001\0","1111111110011111\0","1111111110100000\0","1111111110100001\0","1111111110100010\0","1111111110100011\0","1111111110100100\0","1111111110100101\0","1111111110100110\0"},
		  {"\0","1111011\0","11111111000\0","1111111110100111\0","1111111110101000\0","1111111110101001\0","1111111110101010\0","1111111110101011\0","1111111110101100\0","1111111110101101\0","1111111110101110\0"},
		  {"\0","11111001\0","11111111001\0","1111111110101111\0","1111111110110000\0","1111111110110001\0","1111111110110010\0","1111111110110011\0","1111111110110100\0","1111111110110101\0","1111111110110110\0"},
		  {"\0","11111010\0","111111111000000\0","1111111110110111\0","1111111110111000\0","1111111110111001\0","1111111110111010\0","1111111110111011\0","1111111110111100\0","1111111110111101\0","1111111110111110\0"},
		  {"\0","111111000\0","1111111110111111\0","1111111111000000\0","1111111111000001\0","1111111111000010\0","1111111111000011\0","1111111111000100\0","1111111111000101\0","1111111111000110\0","1111111111000111\0"},
		  {"\0","111111001\0","1111111111001000\0","1111111111001001\0","1111111111001010\0","1111111111001011\0","1111111111001100\0","1111111111001101\0","1111111111001110\0","1111111111001111\0","1111111111010000\0"},
		  {"\0","111111010\0","1111111111010001\0","1111111111010010\0","1111111111010011\0","1111111111010100\0","1111111111010101\0","1111111111010110\0","1111111111010111\0","1111111111011000\0","1111111111011001\0"},
		  {"\0","1111111010\0","1111111111011010\0","1111111111011011\0","1111111111011100\0","1111111111011101\0","1111111111011110\0","1111111111011111\0","1111111111100000\0","1111111111100001\0","1111111111100010\0"},
		  {"\0","11111111010\0","1111111111100011\0","1111111111100100\0","1111111111100101\0","1111111111100110\0","1111111111100111\0","1111111111101000\0", "1111111111101001\0","1111111111101010\0","1111111111101011\0"},
		  {"\0","111111110110\0","1111111111101100\0","1111111111101101\0","1111111111101110\0","1111111111101111\0","1111111111110000\0","1111111111110001\0","1111111111110010\0","1111111111110011\0","1111111111110100\0"},
		  {"111111110111\0","1111111111110101\0","1111111111110110\0","1111111111110111\0","1111111111111000\0","1111111111111001\0","1111111111111010\0","1111111111111011\0","1111111111111100\0","1111111111111101\0","1111111111111110\0"}
  };
  for(k=0;k<16;k++)
    for(i=0;i<11;i++) {
	   a = 1;
	   len = strlen(code[k][i]);
	   for(j=0;j<len;j++) {
    	  v = getCodeBit(ptr, j+bitOut);
	      if(code[k][i][j]-'0' != v)
	         a = 0;
	   }
	   if ((a==1)&&(!(k!=0 && k!=15 && i==0)))  {
		   bitOut += len;
	       len = codeLen[k][i]-len;
	       if(len==0) {
		     *n=k;
		     *p=0;
		     return bitOut;
	      }
	      *n = k;
	      *p=0;
	      for(j=0;j<len;j++)  {
		     (*p) *=2;
		     v = getCodeBit(ptr, j+bitOut);
		     if(v == 1)
		        (*p)++;
	      }
	      v = getCodeBit(ptr, bitOut);
	      if(v == 0)
	         *p = *p + 1 - (int)pow(2,len);
	      bitOut += len;
	      return bitOut;
	  }
    }
    return bitOut;
}
int RLED(int *ZZ, int *RL){
  int rl=1;
  int i=1;
  int k = 0;
  int ret = 0;
  ZZ[0] = RL[0];
  while(i<64)   {
      if(RL[rl]==0 && RL[rl+1]==0){
	     for(k=i;k<64;k++)
	       ZZ[k] = 0;
	     return ret;
	  }
      for(k=0;k<RL[rl];k++)
	     ZZ[i++] = 0;
      ZZ[i++] = RL[rl+1];
      rl+=2;
      if(i==64) ret = -1;
  }
  return ret;
}
void quantDCTD(int *Q, int *DQ, int *D){
    int i, j, idx;
	 for(i=0;i<8;i++){
	    for(j=0;j<8;j++){
	    	idx = i*8+j;
	    	D[idx] = DQ[idx]*Q[idx];
	    }
	 }
}

void DCTD(float *block, int *D, float *T, float *Ttrans){
	 float Dtmp[8*8];
	 multiplyDCTI(Ttrans, D, Dtmp);
//if(printFlg==1)
// printMatrix(Dtmp,"Dtmp");
	 multiplyDCT(Dtmp, T, block);
}
float CD(int u){
  if(u==0)
    return (1.0/sqrt(8.0));
  else
    return (1.0/2.0);
}

void DCTDcos(float *f, int *D)
{
  float a;
  int x, y, u, v;
  for(x=0;x<8;x++)
    for(y=0;y<8;y++)     {
	    a = 0.0;
	    for(u=0;u<8;u++)
	      for(v=0;v<8;v++)
	         a += CD(u)*CD(v)*(float)(D[u*8+v])*cos((double)(2.0*(float)x+1.0)*(float)u*3.14/16.0)*cos((double)(2.0*(float)y+1.0)*(float)v*3.14/16.0);
	f[x*8+y] = a;
   }
}

void ZigZagD(int *QF,int *ZZ){
  int i=0,j=0,k=0,d=0;
  while(k<36)  {
      QF[i*8+j] = ZZ[k++];
      if((i==0)&&(j%2==0)){
	    j++;
	    d=1;
	  }
      else if((j==0)&&(i%2==1)){
	    i++;
	    d=0;
	  }
      else if(d==0)	{
	    i--;
	    j++;
	  }
      else	{
	    i++;
	    j--;
	  }
    }
  i = 7;
  j = 1;
  while(k<64)   {
      QF[i*8+j] = ZZ[k++];
      if((i==7)&&(j%2==0)){
	    j++;
	    d=0;
	  }
      else if((j==7)&&(i%2==1))	{
	    i++;
	    d=1;
	  }
      else if(d==0)	{
  	    i--;
	    j++;
	  }
      else	{
	    i++;
	    j--;
	  }
   }
}
void quantDCTDTest(int *Q, int *DQ, int *D){
    int i, j, idx;
	 for(i=0;i<8;i++){
	    for(j=0;j<8;j++){
	    	idx = i*8+j;
	    	D[idx] = DQ[idx]*Q[idx];
printf("idx = %d, Q[idx]= %d, DQ[idx]= %d, D[idx]= %d\n", idx, Q[idx], DQ[idx], D[idx]);
	    }
	 }
}


int DCT_Quant_Decode(LPBYTE jpgIn, int num_rows, int num_cols, int downSize_VCB, int downSize_HCB, int downSize_VCR, int downSize_HCR, int quantLevel) {
	float block[8*8];
	float T[8*8];
	float Ttrans[8*8];
	int D[8*8];
	int DQ[8*8];
	int QL[8*8];
	int QCh[8*8];
	int ZZ[8*8];
	int RL[200];
	int lenH = 12;
    LPBYTE ptr = jpgIn;
	int end = 0, rl = 0;
	int DCval = 0;
	int n,p,i,j;
	int row, col, idx;
	int bitNum = 0;
	int num_rowsCh, num_colsCh, ret;
	int ddTest;

//	int count = 0;

//	printf(" num_rows= %d, num_cols= %d, downSize_VCB= %d, downSize_HCB= %d, downSize_VCR= %d, downSize_HCR= %d  \n", num_rows, num_cols, downSize_VCB, downSize_HCB, downSize_VCR, downSize_HCR);
    createCosMatrix(T);
    transposeMatrix(T, Ttrans);

    createQuantL(QL, quantLevel);
    createQuantCh(QCh, quantLevel);

//    createQuantLTest(QL, quantLevel);
//    createQuantChTest(QCh, quantLevel);

    printf("\nDecompress -------------------------------------------------------\n");
//    printf("Decompress -------------------------------------------------------\n");
//    printMatrixI(QL, "QL");
//    printMatrixI(QCh,"QCh");

    row = 0;col = 0;
   printf(" num_cols= %d num_rows= %d\n\n---\n", num_cols, num_rows);
   fflush(stdout);
    bitNum = lenH*8;
	while(end == 0){


          rl=0;
          ddTest = bitNum;
//    if(row == 332 && col == 172){
//      printf("Y row= %d col= %d\n", row, col);
//      	   printCodeBitsFrom(ptr, bitNum, 200);
//   }
          bitNum = getDCvalue(ptr, bitNum, &DCval);

	      n=1;
	      RL[rl++] = DCval;
	      while(!(n==0 && p==0)){
	    	  bitNum = getACvalue(ptr, bitNum, &n, &p);
//     if(row == 145 && col == 142){
//    	   printf(" rl= %d n= %d, p= %d\n", rl, n, p);
//     }
		      RL[rl++] = n;
		      RL[rl++] = p;
		 }
	     end = 1;

//	     if(row == 145 && col == 142){
//	    	   printf(" rl= %d n= %d, p= %d\n", rl, n, p);
//	     }

	     RL[rl++] = n;
	     RL[rl++] = p;

//	     if(row == 332 && col == 172){
//	    	    printf(" rl= %d\n", rl);
//	    	    printVectorI(RL, "RL");
//	     }

	     ret = RLED(ZZ,RL);
//	     if(ret < 0) 	   printf(" Y RLED() > 64 !!! --------------- row= %d col= %d, rl= %d\n", row, col, rl);
	     ZigZagD(DQ,ZZ);
	     quantDCTD(QL, DQ, D);
  	     DCTD(block, D, T, Ttrans);
//  	     DCTDcos(block, D);
         {int bTest;
         for(i=0;i<8;i++)
   	        for(j=0;j<8;j++){
   	     	    idx = (row*8+i)*num_cols+col*8+j;
   	     	bTest = (int)(block[i*8+j]+0.5)+128;
   	     	if(bTest<0) bTest = 0;
   	     	if(bTest> 255) bTest = 255;
//   	     if(row == 145 && col == 142){
//    printf(" x= %d y= %d, i= %d, j= %d, idx= %d, bTest= %d\n",  (row*8+i), col*8+j, i, j, idx, bTest);
//   	     }
   	        	outptr0[idx] = (BYTE)bTest;
   	        }
         }

//         if(QCh[1] != 3 && count == 0){
//        	 printf("QCh[1]= %d, row= %d col= %d\n", QCh[1], row, col);
//        	 count++;
//         }

/*
// if(row == 0 && col < 2){
   if(row == 332 && col == 172){
     printf("Y row= %d col= %d\n", row, col);
   	   printCodeBitsFrom(ptr, ddTest, bitNum-ddTest);
   	   printVectorI(RL, "RL");
       printMatrixI(ZZ, "ZZ");
       printMatrixI(DQ, "DQ");
       printMatrixI(D,"D");
       printMatrix(block, "block");
//       printMatrixByte(outptr0, "outptr0");
       printOneBlocksFrom_JPG(outptr0, num_rows, num_cols, row, col, "outptr0");
       fflush(stdout);
}
*/
         col++;
         if(col == (num_cols/8))  	{
   	        col = 0;
   	        row++;
   	     }
         if(row < (num_rows/8))
        	 end = 0;
	}
//	printMatrixI(QCh,"QCh");
// Cb
	printf("Cb num_rows= %d num_cols= %d, row= %d, col= %d\n", num_rows, num_cols, row, col);
    num_rowsCh = num_rows/downSize_VCB;
    num_colsCh = num_cols/downSize_HCB;
    end = 0;
    row = 0; col = 0;

 printf("Cb num_rowsCh= %d num_colsCh= %d, bitNum= %d\n", num_rowsCh, num_colsCh, bitNum);

	while(end == 0){
          rl=0;
          ddTest = bitNum;
          bitNum = getDCvalue(ptr, bitNum, &DCval);
	      n=1;
	      RL[rl++] = DCval;
	      while(!(n==0 && p==0)){
	    	  bitNum = getACvalue(ptr, bitNum, &n, &p);

		      RL[rl++] = n;
		      RL[rl++] = p;
		 }
	     end = 1;
	     RL[rl++] = n;
	     RL[rl++] = p;

//	     printf("-------- row= %d col= %d, rl= %d\n", row, col, rl);

	     ret = RLED(ZZ,RL);
//	     if(ret < 0) 	   printf("Cb RLED() > 64 !!! --------------- row= %d col= %d, rl= %d\n", row, col, rl);
	     ZigZagD(DQ,ZZ);
//	     if(row == 0 && col < 2)
//	    	 quantDCTDTest(QCh, DQ, D);
//	     else
	        quantDCTD(QCh, DQ, D);
  	     DCTD(block, D, T, Ttrans);
//  	     DCTDcos(block, D);

         {int bTest;
         for(i=0;i<8;i++)
   	        for(j=0;j<8;j++){
   	     	    idx = (row*8+i)*num_colsCh+col*8+j;
   	     	     bTest = (int)(block[i*8+j]+0.5)+128;
   	   	     	if(bTest<0) bTest = 0;
   	   	     	if(bTest> 255) bTest = 255;
   	        	outptr1[idx] = (BYTE)(bTest);
   	        }
         }

/*
//if(row == 0 && col < 2){
if(row == 332/downSize_VCB && col == 172/downSize_HCB){
	  printf("Cb row= %d col= %d\n", row, col);
	           printCodeBitsFrom(ptr, ddTest, bitNum-ddTest);
	           printVectorI(RL, "RL");
               printMatrixI(ZZ, "ZZ");
               printMatrixI(DQ, "DQ");
               printMatrixI(D,"D");
               printMatrix(block, "block");
//               printMatrixByte(outptr1, "outptr1");
               printOneBlocksFrom_JPG(outptr1, num_rows, num_cols, row, col, "outptr1");
               fflush(stdout);
}
*/
         col++;
         if(col == (num_colsCh/8))  	{
   	        col = 0;
   	        row++;
   	     }
         if(row < (num_rowsCh/8))
        	 end = 0;

	}
// Cr
	    num_rowsCh = num_rows/downSize_VCR;
	    num_colsCh = num_cols/downSize_HCR;
	    end = 0;
	    row = 0; col = 0;
	    printf("Cr num_rowsCh= %d num_colsCh= %d, bitNum= %d\n", num_rowsCh, num_colsCh, bitNum);
		while(end == 0){
	          rl=0;
	          ddTest = bitNum;

//              printf("Cr row= %d col= %d, bitNum= %d\n", row, col, bitNum);

//	                	   printCodeBitsFrom(ptr, bitNum, 200);


	          bitNum = getDCvalue(ptr, bitNum, &DCval);

//		   	  printf("Cr row= %d col= %d, DCval= %d\n", row, col, DCval);
//		   	  fflush(stdout);

		      n=1;
		      RL[rl++] = DCval;
		      while(!(n==0 && p==0)){
		    	  bitNum = getACvalue(ptr, bitNum, &n, &p);

			      RL[rl++] = n;
			      RL[rl++] = p;
			 }
		     end = 1;
		     RL[rl++] = n;
		     RL[rl++] = p;

//		     printf("rl= %d\n", rl);
//             printVectorI(RL, "RL");

		     ret = RLED(ZZ,RL);

//		     printMatrixI(ZZ, "ZZ");

//		     if(ret < 0) 	   printf("Cb RLED() > 64 !!! --------------- row= %d col= %d, rl= %d\n", row, col, rl);
		     ZigZagD(DQ,ZZ);

//		     printMatrixI(DQ, "DQ");

		     quantDCTD(QCh, DQ, D); //printMatrixI(D,"D");
	  	     DCTD(block, D, T, Ttrans); //printMatrix(block, "block");

	         {int bTest;
	         for(i=0;i<8;i++)
	   	        for(j=0;j<8;j++){
	   	     	    idx = (row*8+i)*num_colsCh+col*8+j;
	   	     	bTest = (int)(block[i*8+j]+0.5)+128;
   	   	     	if(bTest<0) bTest = 0;
   	   	     	if(bTest> 255) bTest = 255;

	   	        	outptr2[idx] = (BYTE)(bTest);
	   	        }
	         }

/*
//if(row == 0 && col < 2){
 if(row == 332/downSize_VCR && col == 172/downSize_HCR){
	  printf("Cr row= %d col= %d\n", row, col);
	                        printCodeBitsFrom(ptr, ddTest, bitNum-ddTest);
	                        printVectorI(RL, "RL");
	                        printMatrixI(ZZ, "ZZ");
	                        printMatrixI(DQ, "DQ");
	                        printMatrixI(D,"D");
	                        printMatrix(block, "block");
//	                        printMatrixByte(outptr2, "outptr2");
	                        printOneBlocksFrom_JPG(outptr2, num_rows, num_cols, row, col, "outptr2");
	                        fflush(stdout);
}
*/
	         col++;
	         if(col == (num_colsCh/8))  	{
	   	        col = 0;
	   	        row++;
	   	     }
	         if(row < (num_rowsCh/8))
	        	 end = 0;

	//      if(row == 0 && col < 2){
	//          printf("end; col*8= %d row*8= %d, end = %d\n", col*8, row*8, end);
	//          fflush(stdout);
	// 	   	}
		}
   printf("end Cr num_rowsCh= %d num_colsCh= %d, bitNum= %d\n", num_rowsCh, num_colsCh, bitNum);

	return 0;
}
int createBMPHeaderFromJPG(LPIMGDATA imgOut, int num_rows, int num_cols){
	int widthOut, heightOut;
	int biWidthPadOut,  bufforSize;
	int offBits = sizeof (BITMAPFILEHEADER) +
				sizeof (BITMAPINFOHEADER);
	LPBYTE ptrH;
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	int dpi = 500;


	widthOut = num_cols;
	heightOut = num_rows;
	biWidthPadOut = ((widthOut*24+31)/32)*4;

	bufforSize = biWidthPadOut*heightOut+offBits;
    ptrH = yl_calloc("OCR_A", bufforSize, sizeof(BYTE));
    if(ptrH == NULL) {
    	printf("createBMPHeaderFromJPG();   yl_calloc ( %d ) failed: %s\n",bufforSize, strerror(errno));
    	return -1;
    }

	bf = (LPBITMAPFILEHEADER) ptrH;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, bufforSize);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, widthOut);
	set4Bytes(bi->biHeight, heightOut);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 24;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, bufforSize - offBits);
	set4Bytes(bi->biXPelsPerMeter, dpi);
	set4Bytes(bi->biYPelsPerMeter, dpi);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);


	imgOut->ptr = ptrH;
	imgOut->pixPtr = ptrH + offBits;

	imgOut->bf.bfType = 0x4d42;
	imgOut->bf.bfSize = bufforSize;
	imgOut->bf.bfReserved1 =  0;
	imgOut->bf.bfReserved2 =  0;
	imgOut->bf.bfOffBits =   offBits;


	imgOut->bi.biSize = sizeof (BITMAPINFOHEADER);
	imgOut->width = widthOut;
	imgOut->bi.biWidth = imgOut->width;
	imgOut->height = heightOut;
	imgOut->bi.biHeight = imgOut->height;
	imgOut->bi.biPlanes = 1;
	imgOut->bi.biBitCount = 24;

	imgOut->bi.biCompression = BI_RGB;
	imgOut->bi.biSizeImage = bufforSize - offBits;

	imgOut->bi.biXPelsPerMeter = dpi;
	imgOut->bi.biYPelsPerMeter = dpi;
	imgOut->bi.biClrUsed = 0;
	imgOut->bi.biClrImportant = 0;
	imgOut->bi.biWidthPad = ((imgOut->bi.biWidth*imgOut->bi.biBitCount+31)/32)*4;


	return 0;
}

void cconvertToBMPFromJPG(LPIMGDATA imgOut, int num_rows, int num_cols){
	int widthOut, heightOut;
	int biWidthPadOut;
	int row, col, idx; //, idxCb, idxCr;
//	int rowCb, colCb;
//	int rowCr, colCr;
//	int num_rowsCb, num_colsCb;
//	int num_rowsCr, num_colsCr;
    BYTE r, g, b;
    BYTE R, G, B;
    double Cb, Cr;

	widthOut = num_cols;
	heightOut = num_rows;
	biWidthPadOut = ((widthOut*24+31)/32)*4;


//	num_rowsCb = num_rows/downSize_VCB;
//	num_colsCb = num_cols/downSize_HCB;
//	num_rowsCr = num_rows/downSize_VCR;
//	num_colsCr = num_cols/downSize_HCR;

    for (row = 0; row < num_rows; row++) {
	    for (col = 0; col < num_cols; col++) {
//	    	rowCb = row/downSize_VCB;colCb = col/downSize_HCB;
//	    	rowCr = row/downSize_VCR;colCr = col/downSize_HCR;
	    	idx = row*num_cols+col;
//	    	idxCb = rowCb*num_colsCb+colCb; idxCr = rowCr*num_colsCr+colCr;
    	    Cb =(double)outptr1[idx]-128.0;
	    	Cr= (double)outptr2[idx]-128.0;

	    	R  = (int)((double)outptr0[idx]                             + 1.402   * Cr);
	    	G  = (int)((double)outptr0[idx]    - 0.34414 * Cb - 0.71414 * Cr);
	    	B  = (int)((double)outptr0[idx]    + 1.772   * Cb);
	    	if(R<0) R= 0;if(R>255) R= 255;
	    	if(G<0) G= 0;if(G>255) G= 255;
	    	if(B<0) B= 0;if(B>255) B= 255;
	    	r = R;g = G;b = B;
//if(row == 332+7 && col == 172+7){
//	printf("idx= %d, idxCb= %d, idxCr= %d\n",idx, idxCb, idxCr);
//	printf("Y= %d, Cb= %d, Cr= %d\n",outptr0[idx], outptr1[idxCb], outptr2[idxCr]);
//	printf("row= %d, col= %d\n",row, col);
//	printf("R= %d, G= %d, B= %d\n",R, G, B);
//}
	    	setColor24(imgOut, col, row, r, g, b);

//	    	getColorRGBA(imgInp, col, row, &r, &g, &b);
//	    	R  = Y                             + 1.402   * (C_R-128)
//	    	G  = Y    - 0.34414 * (C_B-128) - 0.71414 * (C_R-128)
//	    	B  = Y    + 1.772   * (C_B-128)

	    }
    }





}
int ColorSpaceConversion_JPG_BMP(LPIMGDATA imgOut, int num_rows, int num_cols){
//	INT32 * rgb_ycc_tab;
	int ret = 0;

	  createBMPHeaderFromJPG(imgOut, num_rows, num_cols);
	  cconvertToBMPFromJPG(imgOut, num_rows, num_cols);

	return ret;
}

void printCodeBitsToBuff(BYTE *code, int len, char *buff){
	int i,  v;
// printf("len= %d\n", len);
// fflush(stdout);
	for(i=0;i<len;i++){
		v = getCodeBit(code, i);
//		 printf("i= %d, v= %d\n", i, v);
//		 fflush(stdout);

		buff[i] = v+'0';
	}
    buff[len] = 0;
}
void printCodeBitsToBuff_0X(BYTE *code, int len, char *buff){
	int i,  v, n;
	char buffT[500];
	buff[0] = 0;
	for(i=0;i<len/8;i++){
	     sprintf(buffT,"%x, ",code[i]);
	     strcat(buff,buffT);
	}
	n = i;
	buff[n++] = ' ';
	buff[n++] = 'B';
	buff[n++] = ' ';
	for(i=0;i<len%8;i++){
		v = getCodeBit(code, i+len/8);
		buff[n++] = v+'0';
	}

    buff[n] = 0;
}

void printCodeBits(BYTE *code, int len){
	int i, v;
	printf("code:---\n");
	for(i=0;i<len;i++){
		v = getCodeBit(code, i);
		printf("%d",v);
	}
	printf("end code:---\n");
//	v= code[0];
//	printf("v= %d\n", v);
}
void printCodeBitsFrom(BYTE *code, int bitFrom, int len){
	int i, v;
	printf("code:---\n");
	for(i=0;i<len;i++){
		v = getCodeBit(code, i+bitFrom);
		printf("%d",v);
	}
	printf("end code:---\n");
//	v= code[0];
//	printf("v= %d\n", v);
}



void printBlocksFrom_RGBA(LPIMGDATA imgInp, int rowInp, int colInp){

//	int idx;
	 char buff[100];
	 char buff2[100];

	  BYTE r, g, b;
//	  LPBYTE inptr;
	  int row, col, i, j;
	  int num_rows = imgInp->height;
	  int num_cols = imgInp->width;
//	  inptr = imgInp->pixPtr;

	  strcpy(buff,"------------------- RGB  ");
	  printf(buff);printf("\n");

      for (row = 0; row < num_rows/8; row++) {
	    for (col = 0; col < num_cols/8; col++) {


	      if(row==rowInp && col==colInp){
    		 printf("row=%d, col= %d\n", row, col);
	         for (i = 0; i < 8; i++) {
	 			buff[0] = 0;
	  	        for (j = 0; j < 8; j++) {
	           	    getColorRGBA(imgInp, col*8+j, row*8+i, &r, &g, &b);
	 			   sprintf(buff2,"(%d,%d,%d) ",r,g,b);
	 			   strcat(buff, buff2);
	  	        }
	 		   strcat(buff, "\n");
	 		   printf(buff);
	         }
	     	printf("\n");	printf("---------------------------------------------\n");
	     }

	    }
      }
}
void printBlocksFrom_RGB(LPIMGDATA imgInp, int rowInp, int colInp){

//	int idx;
	 char buff[100];
	 char buff2[100];

	  BYTE r, g, b;
//	  LPBYTE inptr;
	  int row, col, i, j;
	  int num_rows = imgInp->height;
	  int num_cols = imgInp->width;
//	  inptr = imgInp->pixPtr;

	  strcpy(buff,"------------------- RGB  ");
	  printf(buff);printf("\n");

      for (row = 0; row < num_rows/8; row++) {
	    for (col = 0; col < num_cols/8; col++) {


	      if(row==rowInp && col==colInp){
    		 printf("row=%d, col= %d\n", row, col);
	         for (i = 0; i < 8; i++) {
	 			buff[0] = 0;
	  	        for (j = 0; j < 8; j++) {
//	           	    getColorRGBA(imgInp, col*8+j, row*8+i, &r, &g, &b);
	           	    getColor24(imgInp, col*8+j, row*8+i, &r, &g, &b);
	 			   sprintf(buff2,"(%d,%d,%d) ",r,g,b);
	 			   strcat(buff, buff2);
	  	        }
	 		   strcat(buff, "\n");
	 		   printf(buff);
	         }
	     	printf("\n");	printf("---------------------------------------------\n");
	     }

	    }
      }
}

void printBlocksFrom_JPG(int num_rows, int num_cols, int rowInp, int colInp){
	  BYTE r, g, b;
	  int row, col, i, j, idx;
	  char buff[100];
	  char buff2[100];


	  strcpy(buff,"------------------- JPG  ");
	  printf(buff);printf("\n");

    for (row = 0; row < num_rows/8; row++) {
	    for (col = 0; col < num_cols/8; col++) {


	      if(row==rowInp && col==colInp){
  		 printf("row=%d, col= %d\n", row, col);
	         for (i = 0; i < 8; i++) {
	 			buff[0] = 0;
	  	        for (j = 0; j < 8; j++) {
	  	        	idx = (row*8+i)*num_cols+col*8+j;
	  	        	r= outptr0[idx];
	  	        	g= outptr1[idx];
	  	        	b= outptr2[idx];
	 			   sprintf(buff2,"(%d,%d,%d) ",r,g,b);
	 			   strcat(buff, buff2);
	  	        }
	 		   strcat(buff, "\n");
	 		   printf(buff);
	         }
	     	printf("\n");	printf("---------------------------------------------\n");
	     }

	    }
    }
}
void printOneBlocksFrom_JPG(BYTE *outptr, int num_rows, int num_cols, int rowInp, int colInp, char *str){
	  BYTE r;
	  int row, col, i, j, idx;
	  char buff[100];
	  char buff2[100];


	  strcpy(buff,"------------------- JPG --  ");
	  strcat(buff,str);
	  printf(buff);printf("\n");

    for (row = 0; row < num_rows/8; row++) {
	    for (col = 0; col < num_cols/8; col++) {


	      if(row==rowInp && col == colInp){
  		 printf("row=%d, col= %d\n", row, col);
	         for (i = 0; i < 8; i++) {
	 			buff[0] = 0;
	  	        for (j = 0; j < 8; j++) {
	  	        	idx = (row*8+i)*num_cols+col*8+j;
	  	        	r= outptr[idx];
	 			   sprintf(buff2,"%d, ",r);
	 			   strcat(buff, buff2);
	  	        }
	 		   strcat(buff, "\n");
	 		   printf(buff);
	         }
	     	printf("\n");	printf("---------------------------------------------\n");
	     }

	    }
    }
}

/*
void getJPGInfo( LPBYTE jpgBytes1, int fSize1,  LPBYTE jpgBytes2, int fSize2){
BYTE b;
int i;
char buff[500];

  printf("------- printzigzagInverse \n");
 printzigzagInverse();
 printf("after ------- printzigzagInverse \n");
 fflush(stdout);

  for(i=0;i<10000;i++) {
     b = jpgBytes1[i];
//     printf("------- jpgBytes[ %d ]= %x \n",i,b);
     sprintf(buff,"------- jpgBytes[ %d ]= %x \n",i,b);
     saveLogFile("", buff, "log_jpg1.txt");
  }

  for(i=0;i<10000;i++) {
     b = jpgBytes2[i];
//     printf("------- jpgBytes[ %d ]= %x \n",i,b);
     sprintf(buff,"------- jpgBytes[ %d ]= %x \n",i,b);
     saveLogFile("", buff, "log_jpg2.txt");
  }
  for(i=0;i<1000;i++) {
     if(jpgBytes1[i] != jpgBytes2[i]){
       sprintf(buff,"------- jpgBytes1[ %d ]= %x, jpgBytes2[ %d ]= %x \n",i,jpgBytes1[i],i,jpgBytes2[i]);
     saveLogFile("", buff, "log_diff.txt");
     }
  }
*/
/*
// create DC code
 {int lenb, n;
  char bStr[32];
//   for(i=0;i<2047;i++){
	 for(i=0;i<1023;i++){
//    getDCcodeOld(i, &lenb, bStr);
		 for(n=0;n<16;n++){
          getACcodeOld(n, i, &lenb, bStr );
          sprintf(buff,"------- n= %d, i= %d, i= %x, lenb= %d, b=%s \n",n, i,i, lenb, bStr);
         saveLog("", buff);
         }

   }
  fflush(stdout);
 }
*/
//{int lenb, n, i;
// char bStr[64];
// /DWORD b;
//   getDCcodeClasic(n, &lenb, bStr);
//   setHufTbls();

//   for(i=0;i<100;i++){
//     for(n=0;n<16;n++){

//       printf("---------------n= %d\n",n);
//       fflush(stdout);
//     getDCcodeClasic(n, &lenb, &b, 1);
//      getACcodeClasic(n, i, &lenb, &b, 1);
//      printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
//      printf("bStr= %s\n",bStr);
//      fflush(stdout);

//       sprintf(buff,"------- n= %d, i= %d, i= %x, lenb= %d, b=%s \n",n, i,i, lenb, bStr);
//       saveLog("", buff);
//     }
//  }

 //}
//}
void getACcodeClasic2(int r, int a, int *lenb, DWORD *b, int isLum) ;
void JPGTest1(){
	 int lenb;
     char bStr[32];
     DWORD b;
     int n, a;
     n = 26; a = 5;
     printf("-------------  n= %d, a= %d\n", n, a);
     getACcodeOld(15, 0,&lenb, bStr);
     printf(" n= %d, a= %d,  lenb= %d, code=%s \n", 15, 0, lenb, bStr);
     getACcodeOld(11, a,&lenb, bStr);
     printf("n= %d, a= %d,  lenb= %d, code=%s \n", 11, a, lenb, bStr);


     setHufTbls();
     printf("clasic-------------  n= %d, a= %d\n", n, a);
     getACcodeClasic(15, 0, &lenb, &b, 1);
     printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
     printf(" n= %d, a= %d,  lenb= %d, code=%s \n", 15, 0, lenb, bStr);
     getACcodeClasic(11, a, &lenb, &b, 1);
     printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
     printf(" n= %d, a= %d,  lenb= %d, code=%s \n", 11, a, lenb, bStr);
     fflush(stdout);

     setHufTbls();
     printf("clasic2-------------  n= %d, a= %d\n", n, a);
     getACcodeClasic2(15, 0, &lenb, &b, 1);
     printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
     printf(" n= %d, a= %d,  lenb= %d, code=%s \n", 15, 0, lenb, bStr);
     getACcodeClasic2(11, a, &lenb, &b, 1);
     printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
     printf(" n= %d, a= %d,  lenb= %d, code=%s \n", 11, a, lenb, bStr);
     fflush(stdout);



//	 int a = -183;
/*
    getDCcodeOld(a, &lenb, bStr);
    printf("-------  a= %d,  lenb= %d, b=%s \n", a, lenb, bStr);

    setHufTbls();
    getDCcodeClasic(a, &lenb, &b, 1);
    printCodeBitsToBuff((BYTE *)&b, lenb, bStr);
    printf("-------  a= %d,  lenb= %d, b=%s \n", a, lenb, bStr);
//    printf("bStr= %s\n",bStr);
    {
    int DCval, numBit;
    BYTE tabB[8];

    tabB[0] = 0xf9;
    tabB[1] = 0x23;
    tabB[2] = 0xfc;
    tabB[3] = 0xb5;
    tabB[4] = 0xfe;

    numBit = getDCvalue(tabB, 0, &DCval);
    printf("1-------  DCval= %d\n",DCval);

    tabB[0] = 0xfc;
    tabB[1] = 0x10;
    tabB[2] = 0xf8;
    tabB[3] = 0x7a;
    tabB[4] = 0x9b;

    numBit = getDCvalue(tabB, 0, &DCval);
    printf("2-------  DCval= %d\n",DCval);

    }
    fflush(stdout);
*/

}

// -------------- clasic Huffman for JPEG --------------

static BYTE bits_dc_luminance[17] =
  { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static BYTE val_dc_luminance[12] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static  BYTE bits_dc_chrominance[17] =
  { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static BYTE val_dc_chrominance[12] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static BYTE bits_ac_luminance[17] =
  { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static BYTE val_ac_luminance[162] =
  { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

static BYTE bits_ac_chrominance[17] =
  { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
static BYTE val_ac_chrominance[162] =
  { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

#define MAX_COEF_BITS 10
#define HufL 255
unsigned int ehufco_dcL[HufL];
BYTE ehufsi_dcL[HufL];
unsigned int ehufco_dcCh[HufL];
BYTE ehufsi_dcCh[HufL];
unsigned int ehufco_acL[HufL];
BYTE ehufsi_acL[HufL];
unsigned int ehufco_acCh[HufL];
BYTE ehufsi_acCh[HufL];

/*
add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[0],
		 bits_dc_luminance, val_dc_luminance);
add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[0],
		 bits_ac_luminance, val_ac_luminance);
add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[1],
		 bits_dc_chrominance, val_dc_chrominance);
add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[1],
		 bits_ac_chrominance, val_ac_chrominance);
*/

 void derivedValuesHuff(int isDC, int isLum){
	  char huffsize[257];
	  unsigned int huffcode[257];
	  unsigned int code;
	  int p, i, l, lastp, si, maxsymbol;
	  LPBYTE htbl_b, htbl_v;
	  unsigned int *phufco;
	  BYTE *phufsi;


	  if(isDC > 0){
		  if(isLum > 0){
			  htbl_b = bits_dc_luminance;
			  htbl_v = val_dc_luminance;
			  phufco = ehufco_dcL;
			  phufsi = ehufsi_dcL;

		  }
		  else{
			  htbl_b = bits_dc_chrominance;
			  htbl_v = val_dc_chrominance;
			  phufco = ehufco_dcCh;
			  phufsi = ehufsi_dcCh;
		  }
	  }
	  else{
		  if(isLum > 0){
			  htbl_b = bits_ac_luminance;
			  htbl_v = val_ac_luminance;
			  phufco = ehufco_acL;
			  phufsi = ehufsi_acL;
		  }
		  else{
			  htbl_b = bits_ac_chrominance;
			  htbl_v = val_ac_chrominance;
			  phufco = ehufco_acCh;
			  phufsi = ehufsi_acCh;
		  }

	  }
	  /* Figure C.1: make table of Huffman code length for each symbol */

	  p = 0;
	  for (l = 1; l <= 16; l++) {
	    i = (int) htbl_b[l];
	    if (i < 0 || p + i > 256){	/* protect against table overrun */
	      printf("------------------ bad Huffman table \n");
	      return;
	    }
	    while (i--)
	      huffsize[p++] = (char) l;
	  }
	  huffsize[p] = 0;
	  lastp = p;

// printf("lastp= %d\n", lastp);

	  /* Figure C.2: generate the codes themselves */
	  /* We also validate that the counts represent a legal Huffman code tree. */

	  code = 0;
	  si = huffsize[0];
	  p = 0;
	  while (huffsize[p]) {
	    while (((int) huffsize[p]) == si) {
	      huffcode[p++] = code;
	      code++;
	    }
	    /* code is now 1 more than the last code used for codelength si; but
	     * it must still fit in si bits, since no code is allowed to be all ones.
	     */
	    if (((INT32) code) >= (((INT32) 1) << si)){
		      printf("------------------ bad Huffman table; code= %d\n", code);
		      return;
	    }
	    code <<= 1;
	    si++;
	  }
	  /* Figure C.3: generate encoding tables */
	  /* These are code and size indexed by symbol value */

	  /* Set all codeless symbols to have code length 0;
	   * this lets us detect duplicate VAL entries here, and later
	   * allows emit_bits to detect any attempt to emit such symbols.
	   */
//	  MEMZERO(dtbl->ehufsi, SIZEOF(dtbl->ehufsi));
	  for (p = 0; p < HufL; p++) {
		  phufsi[p] = 0;
	  }

	  /* This is also a convenient place to check for out-of-range
	   * and duplicated VAL entries.  We allow 0..255 for AC symbols
	   * but only 0..15 for DC.  (We could constrain them further
	   * based on data depth and mode, but this seems enough.)
	   */
	  maxsymbol = isDC ? 15 : 255;
//  { int minI = HufL, maxI = 0;
	  for (p = 0; p < lastp; p++) {
	    i = htbl_v[p];
	    if (i < 0 || i > maxsymbol || phufsi[i]){
	      printf("------------------ bad Huffman table; htbl_v= %d\n", i);
	      return;
	    }
	    phufco[i] = huffcode[p];
	    phufsi[i] = huffsize[p];

//	    ehufco[i] = huffcode[p];
//	    ehufsi[i] = huffsize[p];
//	    if(i > maxI) maxI = i;
//	    if(i < minI) minI = i;
	  }
//      printf("---minI= %d, maxI= %d\n", minI, maxI);
//	  for (p = 0; p < maxsymbol; p++) {
//		  printf("---p= %d, ehufsi= %d, ehufco= %d\n", p, phufsi[p], phufco[p]);

//	  }
//  }

 }

void setHufTbls(){
	int isDC, isLum;
	isDC = 1; isLum = 1;
//	printf("---derivedValuesHuff; isDC= %d, maxI= %d\n", isDC, isLum);
    derivedValuesHuff(isDC, isLum);

	isDC = 1; isLum = 0;
//	printf("---derivedValuesHuff; isDC= %d, maxI= %d\n", isDC, isLum);
    derivedValuesHuff(isDC, isLum);

	isDC = 0; isLum = 1;
//	printf("---derivedValuesHuff; isDC= %d, maxI= %d\n", isDC, isLum);
    derivedValuesHuff(isDC, isLum);

	isDC = 0; isLum = 0;
//	printf("---derivedValuesHuff; isDC= %d, maxI= %d\n", isDC, isLum);
    derivedValuesHuff(isDC, isLum);
}

void createHclasicCodeStr(char *bStr, unsigned int code, int size){
//int cTbl[4];
//printf("---sizeofCode= %d\n", sizeof(code));
//cTbl[0] = code;
//cTbl[1] = 256;
//printf("---size= %d, code %d\n", size, code);
//fflush(stdout);
//	printCodeBits((BYTE*)cTbl, 32);
//fflush(stdout);
int j;
unsigned int c = code;
  for(j=size-1;j>=0;j--) {
     if(c%2==1)
    	 bStr[j] = '1';
     else
    	 bStr[j] = '0';
     c/=2;
  }
  bStr[size] = '\0';

}

void getDCcodeClasic(int a, int *lenb, DWORD *b, int isLum) {
//	int isDC, isLum;

    int temp, temp2;
    int nbits;
	unsigned int *phufco;
	BYTE *phufsi;

	*lenb = 0;

	  if(isLum > 0){
			  phufco = ehufco_dcL;
			  phufsi = ehufsi_dcL;

	  }
	  else{
			  phufco = ehufco_dcCh;
			  phufsi = ehufsi_dcCh;
	  }


    /* Encode the DC coefficient difference per section F.1.2.1 */

    temp = temp2 = a;

    if (temp < 0) {
      temp = -temp;		/* temp is abs value of input */
      /* For a negative input, want temp2 = bitwise complement of abs(input) */
      /* This code assumes we are on a two's complement machine */
//      int cat = getCat(temp);
//      temp2+=(int)pow(2,cat)-1;

      temp2--;
    }

    /* Find the number of bits needed for the magnitude of the coefficient */
    nbits = 0;
    while (temp) {
      nbits++;
      temp >>= 1;
    }
    /* Check for out-of-range coefficient values.
     * Since we're encoding a difference, the range limit is twice as much.
     */
//   printf("---nbits= %d\n",nbits);
//   fflush(stdout);

    if (nbits > MAX_COEF_BITS+1){
    	printf("---bad DCT coef; nbits= %d\n",nbits);
    	fflush(stdout);
    	return;
    }

    {char bStr[32];
     int startBit = 0;
//     printf("---phufco[nbits]= %d, phufsi[nbits]= %d\n",phufco[nbits], phufsi[nbits]);

      createHclasicCodeStr(bStr, phufco[nbits], phufsi[nbits]);

//     printf("---bStr= %s\n",bStr);

      setCodeBits(b, startBit, bStr);
      startBit += phufsi[nbits];

      if (nbits){			/* emit_bits rejects calls with size 0 */

//   	  printf("---startBit= %d\n",startBit);

        createHclasicCodeStr(bStr, temp2, nbits);

//      printf("---bStr= %s\n",bStr);

        setCodeBits(b, startBit, bStr);
        startBit += nbits;
      }
//      printf("startBit= %d\n",startBit);
//      printCodeBits((BYTE *)b, startBit);

      fflush(stdout);
      *lenb = startBit;


    }



//    setCodeBits(b, startBit, code[cat]);
    /* Emit the Huffman-coded symbol for the number of bits */
//    if (! emit_bits(state, dctbl->ehufco[nbits], dctbl->ehufsi[nbits]))
//      return FALSE;

    /* Emit that number of bits of the value, if positive, */
    /* or the complement of its magnitude, if negative. */
//    if (nbits)			/* emit_bits rejects calls with size 0 */
//      if (! emit_bits(state, (unsigned int) temp2, nbits))
//        return FALSE;

}

void getACcodeClasic(int r, int a, int *lenb, DWORD *b, int isLum) {
//	int isDC, isLum;

    int temp, temp2;
    int nbits;
	unsigned int *phufco;
	BYTE *phufsi;

    int i;

	  if(isLum > 0){
			  phufco = ehufco_acL;
			  phufsi = ehufsi_acL;

	  }
	  else{
			  phufco = ehufco_acCh;
			  phufsi = ehufsi_acCh;
	  }


	  /* Encode the AC coefficients*/

      temp2 = temp = a;
      if (temp < 0) {
		temp = -temp;		/* temp is abs value of input */
		/* This code assumes we are on a two's complement machine */
	      int cat = getCat(temp);
	      temp2+=(int)pow(2,cat)-1;

//		temp2--;
      }

	      /* Find the number of bits needed for the magnitude of the coefficient */
      nbits = 1;		/* there must be at least one 1 bit */
      while ((temp >>= 1))
		nbits++;
	      /* Check for out-of-range coefficient values */
      if (nbits > MAX_COEF_BITS){
      	 printf("---bad DCT coef; nbits= %d\n",nbits);
      	 fflush(stdout);
      	 return;
      }
  /* Emit Huffman symbol for run length / number of bits */
      i = (r << 4) + nbits;
      {char bStr[32];
       int startBit = 0;
//       i = 240;
        createHclasicCodeStr(bStr, phufco[i], phufsi[i]);
  //     printf("---bStr= %s\n",bStr);
        setCodeBits(b, startBit, bStr);
        startBit += phufsi[i];
        if (nbits){			/* emit_bits rejects calls with size 0 */
          createHclasicCodeStr(bStr, temp2, nbits);
  //      printf("---bStr= %s\n",bStr);
          setCodeBits(b, startBit, bStr);
          startBit += nbits;
        }
  //      printf("startBit= %d\n",startBit);
  //      printCodeBits((BYTE *)b, startBit);
  //      fflush(stdout);
        *lenb = startBit;
      }

	  /* If the last coef(s) were zero, emit an end-of-block code ?????*/

}

void getACcodeClasic2(int r, int a, int *lenb, DWORD *b, int isLum) {
//	int isDC, isLum;

    int temp, temp2;
    int nbits;
	unsigned int *phufco;
	BYTE *phufsi;

    int i;

	  if(isLum > 0){
			  phufco = ehufco_acL;
			  phufsi = ehufsi_acL;

	  }
	  else{
			  phufco = ehufco_acCh;
			  phufsi = ehufsi_acCh;
	  }


	  /* Encode the AC coefficients*/

	  if(r == 15 && a == 0){
		  char bStr[32];
		  createHclasicCodeStr(bStr, phufco[0xF0], phufsi[0xF0]);
          setCodeBits(b, 0, bStr);
          *lenb = phufsi[0xF0];
	  }
      temp2 = temp = a;
      if (temp < 0) {
		temp = -temp;		/* temp is abs value of input */
		/* This code assumes we are on a two's complement machine */
	      int cat = getCat(temp);
	      temp2+=(int)pow(2,cat)-1;

//		temp2--;
      }

	      /* Find the number of bits needed for the magnitude of the coefficient */
      nbits = 1;		/* there must be at least one 1 bit */
      while ((temp >>= 1))
		nbits++;
	      /* Check for out-of-range coefficient values */
      if (nbits > MAX_COEF_BITS){
      	 printf("---bad DCT coef; nbits= %d\n",nbits);
      	 fflush(stdout);
      	 return;
      }
  /* Emit Huffman symbol for run length / number of bits */
      i = (r << 4) + nbits;
      {char bStr[32];
       int startBit = 0;
//       i = 240;
        createHclasicCodeStr(bStr, phufco[i], phufsi[i]);
  //     printf("---bStr= %s\n",bStr);
        setCodeBits(b, startBit, bStr);
        startBit += phufsi[i];
        if (nbits){			/* emit_bits rejects calls with size 0 */
          createHclasicCodeStr(bStr, temp2, nbits);
  //      printf("---bStr= %s\n",bStr);
          setCodeBits(b, startBit, bStr);
          startBit += nbits;
        }
  //      printf("startBit= %d\n",startBit);
  //      printCodeBits((BYTE *)b, startBit);
  //      fflush(stdout);
        *lenb = startBit;
      }

	  /* If the last coef(s) were zero, emit an end-of-block code ?????*/

}


/*
JFIF usage of JPEG allows Y'CbCr where Y', CB and CR have the full 8-bit range of 0-255

Y'  =   0 + (0.299    * R'_D) + (0.587    * G'_D) + (0.114    * B'_D)
C_B = 128 - (0.168736 * R'_D) - (0.331264 * G'_D) + (0.5      * B'_D)
C_R = 128 + (0.5      * R'_D) - (0.418688 * G'_D) - (0.081312 * B'_D)

And back:

R  = Y                             + 1.402   * (C_R-128)
G  = Y    - 0.34414 * (C_B-128) - 0.71414 * (C_R-128)
B  = Y    + 1.772   * (C_B-128)

OR

Y = 0.257*R+0.504*G+0.098*B+16
Cb=-0.148*R-0.291*G+0.439*B+128
Cr = 0.439*B=0.368*G-0.071*B+128

R = 1.164(Y-16)+1.596(Cr-128)
G = 1.164(Y-16)-0.813(Cr-128)-0.391(Cb-128)
B = 1.164(Y-16)+2.018(Cb-128)


NOTE - Not all image file formats store image samples in the order R0, G0,
B0, ... Rn, Gn, Bn. Be sure to verify the sample order before converting
an RGB file to JFIF.




*/
