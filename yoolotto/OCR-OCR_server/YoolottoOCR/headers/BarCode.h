/*
 * BarCode.h
 *
 *  Created on: Feb 20, 2013
 *      Author: Wicek
 */

#ifndef BARCODE_H_
#define BARCODE_H_


#define LOTTO_TYPES_NUM_PA 10
#define LOTTO_TYPES_MULTI_NUM_PA  3
#define LOTTO_TYPES_NUM_CA 8
#define LOTTO_TYPES_NUM_NY 8

#define NUM_STATES 4

#define DM_CODE_LEN  50

struct MASK {
     BYTE            info;
     BYTE            realMask[6];
     };

#define MASKDEF  struct MASK
typedef MASKDEF  * LPMASK;

typedef char * String;
//String typeOfLotteryTXT[LOTTO_TYPES_NUM_TX];
//String prototypeNameTXT[LOTTO_TYPES_NUM_TX];

String typeOfLotteryPAT[LOTTO_TYPES_NUM_PA];
String prototypeNamePAT[LOTTO_TYPES_NUM_PA];

String typeOfLotteryCAT[LOTTO_TYPES_NUM_CA];
String prototypeNameCAT[LOTTO_TYPES_NUM_CA];

String typeOfLotteryNYT[LOTTO_TYPES_NUM_NY];
String prototypeNameNYT[LOTTO_TYPES_NUM_NY];

int ticketTypeInt;
int ticketTypeIntMy;
int wrongTypeTest;
String USstate[NUM_STATES];
float fieldAngleFromBarCode;
int retLogoClass;
int scratchOff_flg;
int multiDraw;

//----------------------------------------------------
void setTXlotteryTypes(void);
void setPAlotteryTypes(void);
void getTXLottoType(char *strResult, int *typeInt, char* typeChar);

#endif /* BARCODE_H_ */
