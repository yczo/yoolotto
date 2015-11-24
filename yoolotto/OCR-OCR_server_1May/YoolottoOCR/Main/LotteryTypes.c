/*
 * LotteryTypes.c
 *
 *  Created on: Dec 8, 2014
 *      Author: Wicek
 */

#include "../headers/ocr.h"


void getLottoType(char *typeStr, int n){
	   switch(US_State) {
	      case 0:
	      case 1: // TX
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_TX)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryTXT[n]);
	    		break;
	      default:
    			 strcpy(typeStr, "???");
	    		break;

	   }
}
/*
1 - Powerball
2- Mega Millions
3- Lotto Texas
4- Texas Two Steps
5- Pick 3
6- Daily 4
8 - All Or Nothing
*/
void setlotteryTypes(void){
    switch(US_State) {
    case 0:
    case 1: // TX
       typeOfLotteryTXT[1] = "POWER BALL TX"; //14
       typeOfLotteryTXT[2] = "MEGA MILLIONS TX"; //15
       typeOfLotteryTXT[3] = "LOTTO TEXAS"; //8
       typeOfLotteryTXT[4] = "TWO STEP TX"; //12
       typeOfLotteryTXT[5] = "PICK 3 TX";  //9
       typeOfLotteryTXT[6] = "DAILY 4 TX"; //13
       typeOfLotteryTXT[7] = "CASH FIVE TX"; //10
       typeOfLotteryTXT[8] = "ALL OR NOTHING TX"; //16
       break;
    default:
       break;
    }
}

/*
void setlotteryTypes(void){
    switch(US_State) {

    case 0: // TX
       typeOfLotteryTXT[0] = "LOTTO TEXAS"; //8
       typeOfLotteryTXT[1] = "PICK 3 TX";  //9
       typeOfLotteryTXT[2] = "CASH FIVE TX"; //10
       typeOfLotteryTXT[3] = "TWO STEP TX"; //12
       typeOfLotteryTXT[4] = "DAILY 4 TX"; //13
       typeOfLotteryTXT[5] = "POWER BALL TX"; //14
       typeOfLotteryTXT[6] = "MEGA MILLIONS TX"; //15
       typeOfLotteryTXT[7] = "ALL OR NOTHING TX"; //16
       break;
    default:
       break;
    }
}
*/
int changeThePROTNameTX(int n){
	if(n == 5) n = 6;
	if(n == 1) n = 2;
	return n;
}

void getTypeStrForPROT(char *typeStr, int n){
	   switch(US_State) {
	      case 0:
	      case 1: // TX
	    	    n = changeThePROTNameTX( n);
	    		if(n < 0 || n>=LOTTO_TYPES_NUM_TX)
	    			strcpy(typeStr, "???");
	    		else
	    			strcpy(typeStr, typeOfLotteryTXT[n]);
	    		break;
	      default:
 			 strcpy(typeStr, "???");
	    		break;

	   }
}

void getPROTName(char *typeStr, char *prototypeName){
	int len = strlen(typeStr);
	int i;
	strcpy(prototypeName, typeStr);
	for(i=0;i<len;i++){
		if(prototypeName[i] == ' ') prototypeName[i] = '_';

	}
}


