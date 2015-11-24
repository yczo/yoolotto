/*
 * PostprocessingNY.c
 *
 *  Created on: Nov 12, 2013
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
#include "../headers/BarCode.h"
#include "../headers/util.h"

char *DailyNY_Names[] = { " STR ", " BOX ", " STR/BOX ", " COMB "};
int len_DailyNY_Names = 4;

int getNumLowConfidentChars(LPFIELDSRESULTS lpFieldsResults, int tH);
int getLine(char* results, int n, int* iStart, int* iStop);
int testLineConf(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
int getNumberOfGroups(char* results, int iStart, int iStop);
int getNumChars(char *resultsStr, int iStart, int iStop);
void testLeftSideForLineCA(LPFIELDSRESULTS lpFieldsResults, int *iStart, int iStop);
int testBeginCut(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void copyLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void copyResultsBack(LPFIELDSRESULTS lpFieldsResults);
void replaceChars(int iStart, int iStop, char* strT);
void getFirstCharFromNumLine(int* iStart, int* iStop);
void addLineToTmp(void);
int getBiggerGroups(char* results, int iStart, int iStop, int count);
int testBeginCutCA(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void  postProcessingDate_CA(LPTICKETRESULTS ticketResults);
void setDateSlices(LPTICKETRESULTS ticketResults);
void trimSpaces(LPFIELDSRESULTS lpFieldsResults);

void testLeftSideForLineTX(char *resultsStr, int *iStart, int iStop);
void getFirstNumberPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1, int nn, int kCombi);
void getDollarAmountCA(char *rStr, int start, int stop, char *dollarAm);
void addEOL(void);
void addLineToTmpSpace(void);
void getSecondStringPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1);
void copyLineWithSpace(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void matchDailyTerms(void);
void mergeTwoResults(LPFIELDSRESULTS lpFieldsResults, LPFIELDSRESULTS lpFieldsResults1, int nn);
int matchToStringResult(char *str, int *kM);
int testLine_Daily(char *resultN, int nn);
int getGGroupsFromLine(char *resultsStr, int iStart, int iStop);
int getFirstNumberDaily_3_4_CA(int* iStart1, int nn, int iBox);
void getDollarAmountCAnew(int iSt, int idx, char *dollarAm);
int getNumSpaces(int k);
void addLineToTmpSpaceN(int N);


extern char *Month_Names[];
extern char *Day_Names[];
extern int len_Day_Names;
extern int len_Month_Names;

extern char        resultTmp[MAXCHARS+2];
extern char result[200];
extern WORD flConf[200];
extern QUADRANGLE flQuad[200];
//extern QUADRANGLE  flQuadTmp[MAXCHARS+2];

/*
    typeOfLotteryNYT[0] = "LOTTO NY"; //8
    typeOfLotteryNYT[1] = "NUMBERS NY"; //9
    typeOfLotteryNYT[2] = "TAKE 5 NY"; //10
    typeOfLotteryNYT[3] = "SWEET MILLION NY"; //11
    typeOfLotteryNYT[4] = "MEGA MILLIONS NY"; //12
    typeOfLotteryNYT[5] = "WIN 4 NY"; //14
    typeOfLotteryNYT[6] = "POWER BALL NY"; //15
    typeOfLotteryNYT[7] = "PICK 10 NY"; //27
*/
int getFirstQPFromNumLineNYP10(int* iStart, int* iStop, int *yyD){
  int i, k, j, jStop, kStart, len;
  int yyT[10], yyIT[10];
  int yy1, yy2, ccSp, thGroups;

		len = strlen(result);
//printf("--- result= %s, len= %d\n",result, len);

		*iStart = 0; *iStop = 0; *yyD= 0;
		k = 0;
	    for(i=1;i<len;i++){
	       if(result[i] > 32) {
	    	   yyT[k] = flQuad[i].p3Y - flQuad[i].p1Y;
	    	   yyIT[k] = i;
	    	   k++;
	    	   if(k > 5) break;
	       }
		}
//	    for(i=0;i<k;i++){
// printf("--- yyT[ %d ]= %d\n",i,yyT[i]);
//	    }

	    kStart = 1;
	    for(i=1;i<k-1;i++){
	    	yy1 = yyT[i+1] - yyT[i];
	    	yy2 = yyT[i+1] - yyT[i];
	    	if(abs(yy1-yy2) < 10) {
	    		kStart = yyIT[i];break;}
	    }
//   printf("--- kStart= %d\n",kStart);
	    if(ticketTypeInt == 2) {thGroups = 4;}
	    if(ticketTypeInt == 7) {thGroups = 8;}
	    k = len;
	    ccSp = 0;
	    for(i=kStart;i<len;i++){
	    	if(result[i] == 32) ccSp++;
	    	if(ccSp == thGroups) {k=i;break;}
	    }

//  printf("--- k= %d, result[k]= %c, result[k+1]= %c\n",k, result[k],result[k+1]);
        yy2 = flQuad[kStart].p3Y - flQuad[kStart].p1Y;
//   printf("--- yy2= %d, kStart= %d\n",yy2,kStart);
        *yyD = yy2;
	    for(i=k;i<len;i++){
	        if(result[i] > 32) {
		       yy1 = flQuad[i].p3Y - flQuad[i].p1Y;
//printf("1--- result[ %d ]= %c, yy1= %d, yy2= %d\n",i, result[i],yy1, yy2);
		    	if(abs(yy1-yy2) > 15) {
		    		*iStart = i;*iStop = i;
		    		jStop = i+4;
		    		if(jStop >= len) jStop = len;
		    		for(j=i+1;j<jStop;j++){
		    		   if(result[j] > 32){
		    		       yy1 = flQuad[j].p3Y - flQuad[j].p1Y;
//printf("2--- result[ %d ]= %c, yy1= %d, yy2= %d\n",i, result[i],yy1, yy2);
		    		       if(abs(yy1-yy2) > 15)
		    			       *iStop = j;
		    		       else
	    		    	       break;
		    		   }
		    		}
		    		break;
		    	}
	        }
	    }
//printf("--- *iStart= %d, *iStop= %d\n",*iStart, *iStop);
	    if(*iStart > 0 && *iStop > 0 && *iStart <= *iStop)
	    	return 0;
	    return -1;
}
int getFirstQPFromNumLineNY(int* iStart, int* iStop, int *yyD){
  int i, k, j, jStop, kStart, len;
  int yyT[10], yyIT[10];
  int yy1, yy2, ccSp, th;

		len = strlen(result);
//printf("--- result= %s, len= %d\n",result, len);

		*iStart = 0; *iStop = 0; *yyD= 0;
		k = 0;
	    for(i=1;i<len;i++){
	       if(result[i] > 32) {
	    	   yyT[k] = flQuad[i].p3Y - flQuad[i].p1Y;
	    	   yyIT[k] = i;
	    	   k++;
	    	   if(k > 5) break;
	       }
		}

	    for(i=0;i<k;i++){
// printf("--- yyT[ %d ]= %d\n",i,yyT[i]);
	    }

	    kStart = 1;
	    for(i=1;i<k-1;i++){
	    	yy1 = yyT[i+1] - yyT[i];
	    	yy2 = yyT[i+1] - yyT[i];
	    	if(abs(yy1-yy2) < 10) {
	    		kStart = yyIT[i];break;}
	    }
//   printf("--- kStart= %d\n",kStart);

	    k = len;
	    ccSp = 0;
	    for(i=kStart;i<len;i++){
	    	if(result[i] == 32) ccSp++;
	    	if(ccSp == 4) {k=i;break;}
	    }
//  printf("--- k= %d, result[k]= %c, result[k+1]= %c\n",k, result[k],result[k+1]);
        yy2 = flQuad[kStart].p3Y - flQuad[kStart].p1Y;
//   printf("--- yy2= %d, kStart= %d\n",yy2,kStart);

        *yyD = yy2;
	    for(i=k;i<len;i++){
	        if(result[i] > 32) {
		       yy1 = flQuad[i].p3Y - flQuad[i].p1Y;
//printf("1--- result[ %d ]= %c, yy1= %d, yy2= %d\n",i, result[i],yy1, yy2);

		    	if(abs(yy1-yy2) > 15) {
		    		*iStart = i;*iStop = i;
		    		jStop = i+4;
		    		if(jStop >= len) jStop = len;
		    		ccSp = 0;
		    		for(j=i+1;j<jStop;j++){
		    		   if(result[j] > 32){
		    		       yy1 = flQuad[j].p3Y - flQuad[j].p1Y;
//printf("2--- result[ %d ]= %c, yy1= %d, yy2= %d\n",j, result[j],yy1, yy2);
                           th = 15;
                           if(ccSp == 0) th = 12;
		    		       if(abs(yy1-yy2) > th)
		    			       *iStop = j;
		    		       else
	    		    	       break;
		    		   }else
		    			   ccSp++;

		    		}
		    		break;
		    	}
	        }
	    }
//printf("--- *iStart= %d, *iStop= %d\n",*iStart, *iStop);

	    if(*iStart > 0 && *iStop > 0 && *iStart <= *iStop)
	    	return 0;
	    return -1;
}

int getNextQPFromNumLineNY(int* iStart, int* iStop, int yyD){
  int i, kStart, len;
  int yy1;

		len = strlen(result);
		kStart = *iStop+1;
		*iStart = 0; *iStop = 0;
// printf("--- kStart= %d, len= %d \n",kStart, len);

//  printf("--- k= %d, result[k]= %c, result[k+1]= %c\n",k, result[k],result[k+1]);
	    for(i=kStart;i<len;i++){
	        if(result[i] >= '0' && result[i] <= '9') {
//  printf("--- result[ %d ]= %c \n",i, result[i]);

		       yy1 = flQuad[i].p3Y - flQuad[i].p1Y;
//  printf("--- yy1= %d, yyD= %d \n",yy1, yyD);

		    	if(abs(yy1-yyD) > 15) {
		    		*iStart = i;*iStop = i;
		    		if(i+1<len && result[i] >= '0' && result[i] <= '9'){
		    		  yy1 = flQuad[i+1].p3Y - flQuad[i+1].p1Y;
		    		  if(abs(yy1-yyD) > 15)
		    			  *iStop = i+1;
		    		}
		    		break;
		    	}
	        }
	    }
//   printf("NEXT--- *iStart= %d, *iStop= %d\n",*iStart, *iStop);
	    if(*iStart > 0 && *iStop > 0 && *iStart <= *iStop)
	    	return 0;
	    return -1;
}
void replaceCharsForSpace(int iStart, int iStop){
	int i;
	for(i=iStart;i<=iStop;i++){
		if(result[i] > 32)
		     result[i] = ' ';
	}
}
int addPenalityForShortGroup(char* results, int iStart, int iStop){
	   int i, cc, pen = 0;
	   cc = 0;
	   for(i=iStart;i<iStop;i++){
		   if(results[i] > 32) cc++;
		   else {
			   if(cc<2) pen++;
			   cc = 0;
		   }
	   }
       return pen;
}
void cleanLineEnd_NY(){
	int k, len;
	int flg = 0, n;

	len = strlen(result);
	k = len-1;
	if(result[k]==13) {k--;flg = 1;}
//	printf("1   result= %s\n", result);
//	printf("--len = %d, result[len-1]= %d, result[len-2]= %c\n", len, result[len-1], result[len-2]);
	if(result[k]> 32 && result[k-1] == 32){
		n = k-1;
		while(n>0 && result[n] == 32) {result[n] = 0;n--;}
		len = strlen(result);
		if(flg>0){result[len] = 13;result[len+1] = 0;}

	}
//	printf("2   result= %s\n", result);
}
void getFirstCharFromNumLineNY(int* iStart, int* iStop){
	int i, k, st = 1, len;
	int thConf = 20;
	len = strlen(result);
	*iStart = 0;
	*iStop = 0;
	if(len< 6) return;
	 k = -1;
	 if(result[1] != 32) st = 0;
	for(i=st;i<5;i++){
//  printf("--- char: i= %d; result[i]= %c, flConf[i]= %d\n",i, result[i], flConf[i]);
       if(result[i] != 32 && flConf[i]> thConf-15) {
    	   k = i-st;
    	   break;
       }
	}
	if(k> 0 && result[k] == 32)
		k--;
	if(k > 1 && k <len-1){
		if(result[k-1] == 32 && result[k] != 32 && flConf[k]> thConf-15 && result[k+1] != 32 && flConf[k+1]> thConf-15) {
			k -= 2;
		}
	}
	*iStop = k;
}

void cleanTicketNumbersNY(LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, i, k;
	 int iStart, iStop;
	 char strT[10];
	 int ret;
	 int numLines = 0;

	 int thGroups = 4;
	 int numChars;
	 int beginCut = 0;
	 int numBGr = 0, groupNumG = 0;
	 int firstLineIdx;
	 resultTmp[0] = 0;

//		getLine(resultsStr, 2, &iStart, &iStop);
//		for(i=iStart;i<iStop;i++){
//			if(resultsStr[i] >32){
//				printf("--- resultsStr[ %d ]= %c, yy= %d\n",i,resultsStr[i],lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y );
//			}
//		}

//	 cand2Tmp[0] = 0;
// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
// test last lines
// printf("--- numLines= %d\n",numLines);
// fflush(stdout);

	 firstLineIdx = 0;
	 i = 0;
	 while(i<numLines){
		  if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
       	  groupNumG = getGGroupsFromLine(resultsStr, iStart, iStop);
//printf("--- i= %d, groupNumG= %d\n",i,groupNumG);
		  if(testLineConf(lpFieldsResults, iStart, iStop) >= 0 && groupNumG > 2){firstLineIdx= i;break;}
		  lpFieldsResults->lineQuadStatus[i] = 0;
		  i++;
	 }
//printf("--- firstLineIdx= %d\n",firstLineIdx);

	 k = numLines-1;
	 while(k>firstLineIdx){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
       	  groupNumG = getGGroupsFromLine(resultsStr, iStart, iStop);
		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0){
			 lpFieldsResults->lineQuadStatus[k] = 0;
			 numLines--;
		 }
		 else
			 break;
		 k--;
	 }
// printf("2--- numLines= %d\n",numLines);

	 k = numLines-1;
	 while(k>firstLineIdx){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
         testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
 	     numGr = getNumberOfGroups(resultsStr, iStart, iStop);
//     printf("1--- k= %d, numGr= %d\n",k, numGr);
 	     numGr += addPenalityForShortGroup(resultsStr, iStart, iStop);
//    printf("2--- k= %d, numGr= %d\n",k, numGr);
 	     beginCut = 0;
         numChars = getNumChars(resultsStr, iStart, iStop);
         if(numGr == thGroups){
         	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
             if(beginCut > 0)
             	numGr++;
         }
       	groupNumG = getGGroupsFromLine(resultsStr, iStart, iStop);
//    printf("--- k= %d, numGr= %d, numChars= %d, numBGr= %d\n",k, numGr, numChars, numBGr);
 	     if(numGr > thGroups && numGr < 15 && strlen(resultsStr) > 15 && numChars < 20 && numBGr < 3 && groupNumG > 2) break;
		 lpFieldsResults->lineQuadStatus[k] = 0;
		 numLines--;
		 k--;
	 }


// printf("--- numLines= %d\n",numLines);

	 i = firstLineIdx;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
        testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
	    numBGr = getBiggerGroups(resultsStr, iStart, iStop, 3);

	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
        	beginCut = testBeginCutCA(lpFieldsResults, iStart, iStop);
            if(beginCut > 0)
            	numGr++;
        }

        if(k > 0) thGroups = 3;
    	if(ticketTypeInt == 7) // Pick 10
    		thGroups += 4;

//  printLine(resultsStr, iStart, iStop);
//  printf("--- i= %d; numGr= %d, thGroups= %d, numBGr= %d\n",i, numGr, thGroups, numBGr);
	    if(numGr > thGroups && strlen(resultsStr) > 15 && numChars < 20 && numBGr < 3){
	       copyLine(lpFieldsResults, iStart, iStop);

//    printf("in --- i= %d; beginCut= %d, result= %s\n",i, beginCut, result);

	       if(beginCut > 0){
               strT[0] = 'A'+k;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
              getFirstCharFromNumLineNY(&iStart, &iStop);

   //printf("--- getFirstCharFromNumLine i= %d; iStart= %d, iStop= %d, numGr= %d\n",i, iStart, iStop, numGr);

              if(numGr < 8 && iStart == 0 && iStop == 0 && result[1] != 32) {
                 strT[0] = 'A'+k;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
             }else{
//  	   printf("--- numGr= %d; iStart= %d, iStop= %d, result[1]= %d\n",numGr, iStart, iStop, result[1]);
                 strT[0] = 'A'+k;
                 strT[1] = 0;
                replaceChars(iStart, iStop, strT);
             }
	       }
           iStop = -1;

//     printf("2 ---  result= %s\n",result);
//	        if(ticketTypeInt == 4 || ticketTypeInt == 6)
	        {
	        	int yyD, nxt;
//	case 0: // LOTTO NY
//	case 3: // SWEET MILLION NY
//	case 4: // MEGA MILLIONS NY
//	case 6: // POWER BALL NY
	        	  nxt = 0;
                  ret = getFirstQPFromNumLineNY(&iStart, &iStop, &yyD);
                  if(strlen(result)-iStop > 3) nxt = 1;
                   if(ret >= 0) {
                     replaceCharsForSpace(iStart, iStop);
//     printf("---  result= %s\n",result);
//     printf("---  i= %d; nxt= %d, iStop= %d\n",i, nxt, iStop);
                     if(nxt > 0) {
                        ret = getNextQPFromNumLineNY(&iStart, &iStop, yyD);
//     printf("---  i= %d; ret= %d, iStart= %d, iStop= %d\n",i, ret, iStart, iStop);
                        if(ret >= 0) {
                        	replaceCharsForSpace(iStart, iStop);
                            replaceCharsForSpace(iStop+1, strlen(result)-1);
                        }
                     }
                     else{
                    	 replaceCharsForSpace(iStop+1, strlen(result)-1);
                     }
                   }

	        }
//        printf("2 ---  result= %s\n",result);
	        cleanLineEnd_NY();

//    printf("3 ---  result= %s\n",result);
	       addLineToTmp();
//   printf("3 ---  resultTmp= %s\n",resultTmp);
	       k++;
//printTmp();
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
//      printf("--- not line line line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);
	    }
//    printf("--- not line line line i= %d; lpFieldsResults->lineQuadStatus[i]= %d\n",i, lpFieldsResults->lineQuadStatus[i]);
//	      printf("---  line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);
	    i++;
	 }
	 copyResultsBack(lpFieldsResults);
}
void cleanTrashAtTheEnd(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	char *resultsStr = lpFieldsResults->result;
    int i, k1, k2, dd;

//   printLine(resultsStr, iStart, iStop);

    i = iStop;
    while(resultsStr[i]<=32 && i>0)i--;
    k2 = i;
    while(resultsStr[k2]>32 && k2>0)k2--;
    k1 = k2;
    if(k2<iStop) k2++;
//printf ("---------iStart= %d, iStop= %d, k1= %d, k2= %d,resultsStr[k2]= %c \n",iStart,iStop,k1, k2, resultsStr[k2]);
    if(resultsStr[k2]<=32) return;
    while(resultsStr[k1]<=32 && k1>0)k1--;

//    printf ("---------iStart= %d, iStop= %d, k1= %d, k1-iStop= %d, k2= %d,resultsStr[k1]= %c \n",iStart,iStop,k1, k1-iStop, k2, resultsStr[k1]);
    if(k1-iStart > 6 && resultsStr[k1]>32){
    	dd = lpFieldsResults->flQuad[k2].p1X-lpFieldsResults->flQuad[k1].p2X;
    	if(dd > 120){
    		i = k2;
    		while(resultsStr[i]>32 && i<=iStop){resultsStr[i] = 32;i++;}
    		return;
    	}
//    	printf ("1---------iStart= %d, iStop= %d, dd= %d, k1= %d, k2= %d \n",iStart,iStop,dd,k1, k2);
    }

    k2 = k1;
    while(resultsStr[k2]>32 && k2>0)k2--;
    k1 = k2;
    if(k2<iStop) k2++;
    if(resultsStr[k2]<=32) return;
    while(resultsStr[k1]<=32 && k1>0)k1--;
    if(k1-iStart > 6 && resultsStr[k1]>32){
    	dd = lpFieldsResults->flQuad[k2].p1X-lpFieldsResults->flQuad[k1].p2X;
    	if(dd > 120){
    		i = k2;
    		while(resultsStr[i]>32 && i<=iStop){resultsStr[i] = 32;i++;}
    	}
//    	printf ("2---------iStart= %d, iStop= %d, dd= %d, k1= %d, k2= %d \n",iStart,iStop,dd,k1, k2);
    }
}
int getNumberOfGroups_1(){
	   int i, k, cc, flg, len;
	   len = strlen(result);
	   cc = 0;
	   k = 0;
       while(result[k]>32 && k<len)k++;
       k++;flg = 0;
	   for(i=k;i<len;i++){
		   if(result[i] == 32) {
			   if(flg == 0) cc++;
			   flg++;
		   }
		   else flg = 0;
	   }
       return cc;
}
void cleanOneDigitGroup_1(int iStart, int iStop){
 int i, yy, yyE, cc, k;

/*
     max = 0;
	for(i=iStart;i<=iStop;i++){
	     yy = flQuad[i].p3Y - flQuad[i].p1Y;
	     if(yy > max) max = yy;
  printf("--------- result[ %d ]= %c, yy= %d\n", i, result[i], yy);
	}
	th = max/3;
	for(i=iStart;i<=iStop;i++){
	     yy = flQuad[i].p3Y - flQuad[i].p1Y;
	     if(max-yy > th) result[i] = ' ';
	}
*/
	k = 0;
	while(k<5) {
	   cc = 0;
	   for(i=iStart;i<=iStop;i++){
		   if(result[i] > 32) cc++;
	   }
	   if(cc<=2) break;
	   yy = flQuad[iStart+1].p3Y - flQuad[iStart].p1Y;
	   yyE = flQuad[iStop].p3Y - flQuad[iStop-1].p1Y;
	   if(yy<yyE){
		   result[iStart] = ' ';iStart++;
	   }
	   else{
		   result[iStop] = ' ';iStop--;
	   }
	   k++;
	}
}
void cleanTooManyDigitsInGroup_1(int thGr){
 int  k,  len;
 int ccSp, ccD, cc;
 len = strlen(result);

	ccSp = 0; ccD = 0;cc = 0;
	for(k=2;k<len;k++){
		if(result[k] == 32) {
			if(ccSp == 0 && ccD > 2) {
				cleanOneDigitGroup_1(k-ccD, k-1);
			}
			ccSp++;
			ccD = 0;
			cc++;
	    }
		else{
			 ccD++;
			 ccSp = 0;
		}
		if(cc >=thGr) break;
	}


}

void cleanTicketNumbersPick10NY(LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, i, k;
	 int iStart, iStop;
	 char strT[10];
	 int ret, len;
	 int numLines = 0;

	 int thGroups = 8;
	 int numChars, thNumChars, thLen;
	 int beginCut = 0;
	 int numBGr = 0;
	 int lastI = -1;

	 resultTmp[0] = 0;

// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
// test last lines
// printf("--- numLines= %d\n",numLines);
// fflush(stdout);

	 k = numLines-1;
	 while(k>0){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0){
			 lpFieldsResults->lineQuadStatus[k] = 0;
			 numLines--;
		 }
		 else
			 break;
		 k--;
	 }
// printf("2--- numLines= %d\n",numLines);
	 if(ticketTypeInt == 2) {thGroups = 3;thNumChars = 15;thLen = 10;}
	 if(ticketTypeInt == 7) {thGroups = 8;thNumChars = 25;thLen = 15;}

	 k = numLines-1;
	 while(k>0){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
         testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
         cleanTrashAtTheEnd(lpFieldsResults, iStart, iStop);
 	     numGr = getNumberOfGroups(resultsStr, iStart, iStop);
 	     beginCut = 0;
         numChars = getNumChars(resultsStr, iStart, iStop);
         if(numGr == thGroups){
         	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
             if(beginCut > 0)
             	numGr++;
         }
         len = iStop-iStart;

//         printLine(resultsStr, iStart, iStop);
//         printf("--- k= %d,numGr= %d,len= %d, strlen(resultsStr)= %d, numChars= %d, numBGr= %d\n",k,numGr,len,strlen(resultsStr),numChars, numBGr);
//         printf("--- k= %d,thGroups= %d,thLen= %d, thNumChars= %d\n",k,thGroups,thLen,thNumChars);

         if(numGr > thGroups && len > thLen && numChars < thNumChars && numBGr < 3) break;
		 lpFieldsResults->lineQuadStatus[k] = 0;
		 numLines--;
		 k--;
	 }


// printf("--- numLines= %d\n",numLines);

	 i = 0;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
        testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
	    numBGr = getBiggerGroups(resultsStr, iStart, iStop, 3);
	    numGr -= addPenalityForShortGroup(resultsStr, iStart, iStop);

	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
        	beginCut = testBeginCutCA(lpFieldsResults, iStart, iStop);
            if(beginCut > 0)
            	numGr++;
        }

//        if(k > 0) thGroups = 7;
   	 if(ticketTypeInt == 7) {if(k > 0) thGroups = 7;}

//   	printLine(resultsStr, iStart, iStop);
   	    if(lastI >=0 && i-lastI> 2)  lastI = 500;
//   printf("--- i= %d; numGr= %d, thGroups= %d. lastI= %d\n",i, numGr, thGroups, lastI);
	    if(numGr > thGroups && strlen(resultsStr) > thLen && numChars < thNumChars && numBGr < 3 && lastI<500){
	    	lastI = i;
//  	  printf("2--- i= %d; numGr= %d, thGroups= %d\n",i, numGr, thGroups);
	       copyLine(lpFieldsResults, iStart, iStop);
//  printf("--- i= %d; k= %d, beginCut= %d\n",i, k, beginCut);
	       if(beginCut > 0){
               strT[0] = 'A'+k;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
              getFirstCharFromNumLine(&iStart, &iStop);

//   printf("--- getFirstCharFromNumLine i= %d; iStart= %d, iStop= %d, numGr= %d, result[1]= %c\n",i, iStart, iStop, numGr, result[1]);

              if(numGr < 11 && iStart == 0 && iStop == 0 && result[1] != 32) {
                 strT[0] = 'A'+k;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
             }else{
//  	   printf("--- numGr= %d; iStart= %d, iStop= %d, result[1]= %d\n",numGr, iStart, iStop, result[1]);
                 strT[0] = 'A'+k;
                 strT[1] = 0;
                replaceChars(iStart, iStop, strT);
             }
	       }
// printf("--- result= %s\n",result);
           thGroups++;
//           numGr = getNumberOfGroups_1();
//        printf("--- numGr= %d, thGroups= %d\n",numGr, thGroups);
           cleanTooManyDigitsInGroup_1(thGroups);

           iStop = -1;
//	        if(ticketTypeInt == 4 || ticketTypeInt == 6)
	        {
	        	int yyD;
//	case 0: // LOTTO NY
//	case 3: // SWEET MILLION NY
//	case 4: // MEGA MILLIONS NY
//	case 6: // POWER BALL NY
                  ret = getFirstQPFromNumLineNYP10(&iStart, &iStop, &yyD);
//                  if(strlen(result)-iStop > 3) nxt = 1;
                   if(ret >= 0) {
                     replaceCharsForSpace(iStart, iStop);
                   	 replaceCharsForSpace(iStop+1, strlen(result)-1);
                   }

	        }

//           if(ret >= 0) {
//               replaceChars(iStart, iStop, "QP");
//           }
//           if( ret > 0 || numGr >6) {
//              if(getSecondQPFromNumLine(&iStart, &iStop) >=0){
//                 replaceChars(iStart, iStop, "QP");
//              }
//           }
	       addLineToTmp();
	       k++;
//printTmp();
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
//      printf("--- not line line line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);
	    }
//    printf("--- not line line line i= %d; lpFieldsResults->lineQuadStatus[i]= %d\n",i, lpFieldsResults->lineQuadStatus[i]);
//	      printf("---  line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);
	    i++;
	 }
	 copyResultsBack(lpFieldsResults);
}

int getGroupsFromLine(char *resultsStr, int iStart, int iStop){
	int i, len, ccG, ccSp;
	ccSp = 0;
	ccG = 0;
	len = strlen(resultsStr);
	for(i=iStart+1;i<iStop;i++){
		if(resultsStr[i] == 32) ccSp++;
		else{
			if(ccSp > 0) ccG++;
			ccSp = 0;
		}
	}
    return ccG;
}
int getGGroupsFromLine(char *resultsStr, int iStart, int iStop){
	int i, len, ccG, ccGgr;
	ccGgr = 0;
	ccG = 0;
	len = strlen(resultsStr);
	for(i=iStart+1;i<iStop;i++){
		if(resultsStr[i] > 32) ccGgr++;
		else{
			if(ccGgr == 2) ccG++;
			 ccGgr = 0;
		}
	}
	if(ccGgr == 2) ccG++;
    return ccG;
}

void repairGroups(LPFIELDSRESULTS lpFieldsResults, int line, int numLines){
	char *resultsStr = lpFieldsResults->result;
	int secondLine;
	int iStart, iStop, i, k, dd;
	int x1; //, x2;

	if(numLines < 2) return;
	if(line == 0) secondLine = line+1;
	else  secondLine = line-1;
	if(getLine(resultsStr, secondLine, &iStart, &iStop) >= 0){
		x1 = lpFieldsResults->flQuad[iStart+2].p1X;
//		x2 = lpFieldsResults->flQuad[iStart+2].p2X;
		if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
			if(x1-lpFieldsResults->flQuad[iStart+2].p1X>10){
				resultsStr[iStart+2] = ' ';
				if(resultsStr[iStart+3] > 32) resultsStr[iStart+3] = ' ';
			}
			else{
				k = -1;
				for(i=iStop;i>iStart;i--){
					if(resultsStr[i] == 32) {k=i;break;}
				}
				if(k > 4 && k<iStop-1){
					dd = lpFieldsResults->flQuad[k+1].p2X-  lpFieldsResults->flQuad[k-1].p1X;
//					printf("---line= %d, dd= %d\n",line,dd);
					if(dd > 40){
						for(i=k+1;i<iStop;i++){
						if( resultsStr[i] > 32)
							resultsStr[i] = 32;
						}
					}
				}
			}

		}
	}
}
 int calcY1(LPFIELDSRESULTS lpFieldsResults, int line){
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop, stop, i, cc, yy = 0;

 		if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
 			stop = iStart+10;if(stop>iStop) stop = iStop;
 			cc = 0;yy = 0;
 			for(i=iStart;i<stop;i++){
 				if(resultsStr[i]>32){
 				  yy += lpFieldsResults->flQuad[i].p1Y;cc++;
 				}
 			}
			if(cc>0)
				yy /= cc;
 		}
 		return yy;
 }
 int calcY2(LPFIELDSRESULTS lpFieldsResults, int line){
 	 char *resultsStr = lpFieldsResults->result;
 	 int iStart, iStop, stop, i, cc, yy = 0;

  		if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
  			stop = iStart+10;if(stop>iStop) stop = iStop;
  			cc = 0;yy = 0;
  			for(i=iStart;i<stop;i++){
  				if(resultsStr[i]>32){
  				  yy += lpFieldsResults->flQuad[i].p3Y;cc++;
  				}
  			}
			if(cc>0)
				yy /= cc;
  		}
  		return yy;
  }
  void	deleteLine(char *resultsStr, int line){
 	 int i, iStart, iStop;
 	 i = line+1;
 	 while(1){
 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
 		resultsStr[iStart] = 'A'+i-1;
 		i++;
 	 }
 	if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
 		for(i=0;i<=iStop;i++)
 			resultsStr[i] = ' ';
 	}
  }
  void cleanOneDigitGroup(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
   char *resultsStr = lpFieldsResults->result;
   int i, yy, max, th;
       max = 0;
  	for(i=iStart;i<=iStop;i++){
  	     yy = lpFieldsResults->flQuad[i].p3Y - lpFieldsResults->flQuad[i].p1Y;
  	     if(yy > max) max = yy;
//  printf("--------- resultsStr[ %d ]= %c, yy= %d\n", i, resultsStr[i], yy);
  	}
  	th = max/3;
  	for(i=iStart;i<=iStop;i++){
  	     yy = lpFieldsResults->flQuad[i].p3Y - lpFieldsResults->flQuad[i].p1Y;
  	     if(max-yy > th) resultsStr[i] = ' ';
  	}
  }
  void cleanTooManyDigitsInGroup(LPFIELDSRESULTS lpFieldsResults){
   char *resultsStr = lpFieldsResults->result;
   int i, k, iStart, iStop;
   int ccSp, ccD;

   i = 0;
   while(1){
  	if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  	ccSp = 0; ccD = 0;
  	for(k=iStart+1;k<iStop;k++){
  		if(resultsStr[k] == 32) {
  			if(ccSp == 0 && ccD > 2) {
  				cleanOneDigitGroup(lpFieldsResults, k-ccD, k-1);
  			}
  			ccSp++;
  			ccD = 0;
  	    }
  		else{
  			 ccD++;
  			ccSp = 0;
  		}
  	}
  	i++;
   }
  }
  void cleanTooLongFirstGroup(LPFIELDSRESULTS lpFieldsResults){
  // too long first group
	   char *resultsStr = lpFieldsResults->result;
	   int i, iStart, iStop;
  	 	 i = 0;
  	 	 while(1){
  	 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  	 		if(resultsStr[iStart+2] >32 && resultsStr[iStart+3] >32 && resultsStr[iStart+4] >32 ){
  	 			resultsStr[iStart+2] = ' ';
  	 		}
  	 		i++;
  	 	 }
  }

  void testFinalGroups_NY(LPFIELDSRESULTS lpFieldsResults){
	 	 char *resultsStr = lpFieldsResults->result;
	 	 int i, numLines, iStart, iStop;
	 	 int groupNum[50];
	 	 int grPerType, y1, y2, yy1, yy2;

//	 	 resultTmp[0] = 0;

// too many groups
	 	 i = 0;
	 	 while(1){
	 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	 		groupNum[i] = getGroupsFromLine(resultsStr, iStart, iStop);
//printf("--- groupNum[ %d ]= %d\n",i,groupNum[i]);
	 		i++;
	 	 }
	 	numLines = i;
	 	switch(ticketTypeInt){
           case 0: // LOTTO NY
           case 3: // SWEET MILLION NY
           case 4: // MEGA MILLIONS NY
           case 6: // POWER BALL NY
     	      grPerType = 6;break;
	 	}
	 	if(groupNum[0] < grPerType && numLines > 1){
	 		y1 = calcY2(lpFieldsResults, 0);
	 		y2 = calcY1(lpFieldsResults, 1);
	 		yy1 = y2-y1;
//	printf("--- y2-y1= %d\n",y2-y1);
		    y1 = calcY2(lpFieldsResults, 1);
		    y2 = calcY1(lpFieldsResults, 2);
		    yy2 = y2-y1;
//printf("--- yy2-yy1= %d\n",yy2-yy1);
		    if(yy1-yy2 > 10) {
		    	deleteLine(resultsStr, 0);
		    }
 	    }

        for(i=0;i<numLines;i++){
        	if(groupNum[i] > grPerType)
        		repairGroups(lpFieldsResults, i,numLines);
        }
// too long first group
        cleanTooLongFirstGroup(lpFieldsResults);
 	    cleanTooManyDigitsInGroup(lpFieldsResults);
 }

void   testQP_NY(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 int i,  n, nSp, numLines, iStart, iStop;
// 	 int  y1, y2, yy1, yy2;
 	 int cc, yy, k, iK;

	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//printLine(resultsStr,  iStart, iStop);
        n = iStop;
        while(n>8 && resultsStr[n] <= 32) n--;
        nSp = n;
//printf("--- n = %d, resultsStr[n]= %c\n",n,resultsStr[n]);
        while(nSp>8 && resultsStr[nSp] > 32) nSp--;
// printf("--- nSp = %d, resultsStr[nSp]= %c\n",nSp,resultsStr[nSp]);

        if(n-nSp < 3) break;
	    cc = 0;yy = 0;
	    k = nSp+1;
//printf("1--- k = %d, resultsStr[k]= %c\n",k,resultsStr[k]);
		if(resultsStr[k]>32){
			yy += lpFieldsResults->flQuad[k].p1Y;cc++;
		}
	    k++;
// printf("2--- k = %d, resultsStr[k]= %c\n",k,resultsStr[k]);
		if(resultsStr[k]>32){
			yy += lpFieldsResults->flQuad[k].p1Y;cc++;
		}
		if(cc>0)
			yy /= cc;
//printf("--- yy = %d, cc= %d\n",yy,cc);

	    k++;
		if(resultsStr[k]>32){
//printf("--- k = %d, lpFieldsResults->flQuad[k].p1Y= %d, lpFieldsResults->flQuad[k].p1Y-yy= %d\n",k,lpFieldsResults->flQuad[k].p1Y,lpFieldsResults->flQuad[k].p1Y-yy);
			if(lpFieldsResults->flQuad[k].p1Y-yy>10){
//	printf("--- k = %d, n= %d\n",k,n);
				for(iK=k;iK<=n;iK++){
//	printf("--- iKk = %d, resultsStr[iK]= %c\n",iK,resultsStr[iK]);
					resultsStr[iK] = 32;
				}
			}
		}

//printf("--- groupNum[ %d ]= %d\n",i,groupNum[i]);
		i++;
	 }
	numLines = i;



}
  void testFinalGroupsP10_NY(LPFIELDSRESULTS lpFieldsResults){

// too many digits in group
	 	 cleanTooManyDigitsInGroup(lpFieldsResults);
 }
  int matchDailyine(int *idxOut, int nn){
  	int cc, ccMax, idxMax;
  	int i, k, kk;
  	*idxOut = 0;

  	cc = 0;ccMax = 0;idxMax = -1;
  	k = -1;
//        	printf ("---result= %s\n",result);
  	for(i=0;i<len_DailyNY_Names;i++) {
  		cc =matchToStringResult(DailyNY_Names[i], &kk);
  	   if(ccMax < cc)
  	      if(ccMax < cc && kk >= nn && kk < nn+5){ccMax = cc;idxMax = i;k = kk;}
//       	printf ("---DailyNY_Names[i]= %s, cc= %d, ccMax= %d, kk= %d\n",DailyNY_Names[i], cc, ccMax, kk);
  	}
//    printf ("ccMax= %d, k= %d, idxMax= %d\n", ccMax, k, idxMax);
//if (k > 1)
//  printf ("---result[k]= %c, result[k-1]= %c, result[k-2]= %c\n",result[k], result[k-1], result[k-2]);

  	if(idxMax < 0) return -1;

  	if(ccMax > strlen(DailyNY_Names[idxMax]) - 2 && k >= nn-1 && (result[k] == 32 || result[k-1] == 32)) {
  		*idxOut = idxMax;
  		return k;
  	}
  	return -1;
//   printf ("result= %s\n",result);
  }
  int testLine_DailyNewNY(int nn, int *idxTab){
//   printf ("------result= %s\n",result);
  //	fflush(stdout);
  	int i = 0;
  	int k=0, cc, ccMax;
  	int kk, idxLineA;

  //	matchToStringEndOneLineIdx(char *results, char *str, int sh){
  	ccMax = 0;
  	idxLineA = -1;
  	for(i=0;i<len_DailyNY_Names;i++) {
  		cc =matchToStringResult(DailyNY_Names[i], &kk);
  	      if(ccMax < cc && cc == strlen(DailyNY_Names[i])){ccMax = cc;k = kk;idxLineA = i;}
  	      else {
  		      if(ccMax < cc && (cc >= strlen(DailyNY_Names[i])-1 || cc > 5) && kk >= nn && kk < nn+5){ccMax = cc;k = kk;idxLineA = i;}
  	      }

//       	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d, idxLineA= %d, strlen(DailyNY_Names[i])= %d\n",DailyNY_Names[i], cc, ccMax, kk, idxLineA, strlen(DailyNY_Names[i]));
  	}
     *idxTab = idxLineA;
     if(idxLineA >= 0) return k;
     return -1;

  }

  void cleanTicketNumbers_Daily_NY_OLD(LPTICKETRESULTS ticketResults, int idx, int nn) {
  	 LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[idx];
  	 char *resultsStr = lpFieldsResults->result;
  	 LPFIELDSRESULTS lpFieldsResults1;
  	 char *resultsStr1;
  	 int  i, k, kS, idx1, j;
  	 int kTab[20];
  	 int shiftTab[20];
  	 int kSumItUp[20];
  	 int kComb[20];
  	 int kCombIdx[20];
  	 int kCombStart[20];
  	 int shift;
  	 int total = 0;
  	 int iStart, iStop;
  	 int iStart1, iStop1;
  	 char strT[10];
  	 int numLines = 0;
  	 char dollarAm[10];
  	 int yPrev, yCur, ddY;
  	 int firstLineIdx, startISt;
  	 idx1 = idx;
       for(i=0;i<NUMFIELDS_RESULTS;i++){
  	    	if(ticketResults->fieldsResults[i].name[0] != 0){
  	    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS1") == 0) idx1 = i;
  	    	}
      }
      lpFieldsResults1 = &ticketResults->fieldsResults[idx1];
  	 resultsStr1 = lpFieldsResults1->result;

  	 resultTmp[0] = 0;
  // get number of lines
  	 i = 0;
  	 while(1){
  		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  		numLines++;
  		i++;
  	 }
  	 k = numLines-1;
//   printf ("----------numLines= %d\n",numLines);

       for(i=0;i<20;i++){
  	     kTab[i] = -1;
  	     kSumItUp[i] = 0;
  	     shiftTab[i] = 0;
  	     kComb[i] = 0;
  	     kCombStart[i] = 0;
  	     kCombIdx[i] = 0;
       }
  //    printf ("----------numLines= %d\n",numLines);
  // find first line
       firstLineIdx = 0;
       i = 0;
  	    while(i < numLines){
  		   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  		   copyLine(lpFieldsResults, iStart, iStop-1);
//    printf ("1------result= %s\n",result);
  		   strcpy(resultTmp, result);
  		   if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
  		   copyLine(lpFieldsResults1, iStart, iStop-1);
//    printf ("2------result= %s\n",result);
  		   if(testLine_Daily(resultTmp, nn) > 0) {firstLineIdx = i;break;}
  		   i++;
  //		    printf ("------result= %s\n",resultTmp);
  	    }
  	    if(i>0){
            for(k=0;k<i;k++)
  	        lpFieldsResults->lineQuadStatus[k] = 0;
  	    }
  	    resultTmp[0] = 0;
  	    result[0] = 0;

//     printf ("------firstLineIdx= %d\n",firstLineIdx);
//     printf ("----------numLines= %d\n",numLines);

  	     {int  kkS, flgEnd;
  	        i = firstLineIdx;
  	        flgEnd = 0;
  		    while(i < numLines){
  			   if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
		       copyLine(lpFieldsResults1, iStart, iStop-1);
			   kkS =  matchDailyine(&kCombIdx[i], nn);
  	//  printf ("----------i= %d, kkS= %d, kCombIdx[i]= %d\n",i, kkS, kCombIdx[i]);
               if(kkS >= nn){
            	       flgEnd = 1;
  	              	   kkS++;
  	            	   kComb[i] = kkS;
  	            	   kCombStart[i] = iStart+kComb[i]-nn;
  	   	 	    	   if(resultsStr1[iStart+kComb[i]-1] == 32)
  	   	 	    		  kCombStart[i]--;
  	           }
               if(flgEnd > 0) break;
  			   i++;
  		    }
  	     }


  	 i = firstLineIdx;
  	 k = 0;
  	 kS = 0;
  	 yPrev = 0;
  	 yCur = 0; ddY = 0;
  	 while(i < numLines){
  		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//  		 printf ("--------------------------------------------i= %d, iStart= %d, iStop= %d\n",i, iStart, iStop);
  		   iStart1 = iStart;

             testLeftSideForLineTX(resultsStr, &iStart, iStop);
             shift = iStart-iStart1;
  //    printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);

//      printLine(resultsStr, iStart1, iStop);
  //    printLine(resultsStr1, iStart1, iStop);
             startISt = 0;
             getFirstNumberPick3TX(lpFieldsResults, iStart, iStop, &iStart1, &iStop1, nn, kComb[i]);
             if(iStart == 0) startISt--;

 //printf ("--------------------------------------------i= %d, iStart1= %d, iStop1= %d, kComb[i]= %d\n",i,iStart1, iStop1, kComb[i]);
     	    if((iStart1 < 0 || iStart1 >= iStop1) && kComb[i] > nn){
     	    	iStart1 = iStart+kComb[i]-nn;
   	    	if(resultsStr[iStart+kComb[i]-1] == 32)
     	    		iStart1--;
     	    	iStop1 = iStart1+nn;
     	    }

//    printf ("--------------------------------------------i=%d, iStart1= %d, iStop1= %d, ddY= %d\n",i,iStart1, iStop1, ddY);
//   printLine(resultsStr, iStart1, iStop);

  	    if( ((iStart1 > startISt && iStart1 < iStop1) || kSumItUp[i] > 0) && ddY < 180){
  	    	yPrev = yCur;
//   printf ("-------i= %d, k= %d, kS= %d\n",i, k, kS);
  //   	 printf ("-------i= %d, kSumItUp[i]= %d\n",i, kSumItUp[i]);

  	           copyLine(lpFieldsResults, iStart1, iStop1-1);
  	           total = 0;
  //	           if(nn == 4) testForXX(result, lpFieldsResults1,kCombStart[i], nn);

    printf ("------result= %s\n",result);
  // printf ("------result= %s\n",&lpFieldsResults->result[iStart1]);
                 getDollarAmountCA(resultsStr, iStop1, iStop, dollarAm);


  	           for(j=0;j<strlen(result);j++){
                    if((result[j]<'0' || result[j]>'9') && result[j] != 'X') break;
                    if(result[j] != 'X')
                       total += result[j] - '0';
  	           }
  //	   printf ("1------result= %s\n",result);
  	           {int jj = strlen(result);
  				strcat(result, " ");
  				strcat(result, dollarAm);
  				   for(jj=0;jj<strlen(dollarAm)+1;jj++){
  				      flConf[jj] = 100;
  				   }
  				}
  //	 printf ("2------result= %s\n",result);

                 strT[0] = 'A'+kS;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
                 addEOL();
  	           addLineToTmpSpace();
  //     printf ("------resultTmp= %s\n",resultTmp);
  	           kTab[k] = i;
  	           shiftTab[k] = shift;
  //	           iStopTab[k] = iStop;
  //	           iStop1Tab[k] = iStop1;
  	           if(i+1 < 20 && kSumItUp[i+1] == 0) kS++;
         	       k++;
  	    }else{
  	    	lpFieldsResults->lineQuadStatus[i] = 0;
  	    }
  	    i++;
  	 }
  	 copyResultsBack(lpFieldsResults);

  // printLine(resultsStr, 0, strlen(resultsStr)-1);

  	 resultTmp[0] = 0;
       k = 0;
  	 i = firstLineIdx;
  	 while(i < numLines){
  		if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
  //printf ("-------i= %d, k= %d, kSumItUp[i]= %d\n",i, k, kSumItUp[i]);
  		if(kSumItUp[i] == 0){
  		   if(i == kTab[k]){
  //	    printf ("2--------------------------------------------iStart= %d, iStop= %d, shiftTab[k]= %d\n",iStart, iStop, shiftTab[k]);
  			iStart += shiftTab[k];
  //	    printf ("2--------------------------------------------iStart= %d, iStop= %d, shiftTab[k]= %d\n",iStart, iStop, shiftTab[k]);
  	           getFirstNumberPick3TX(lpFieldsResults1, iStart, iStop, &iStart1, &iStop1, nn, kComb[i]);
  //        printf ("2--------------------------------------------iStart1= %d, iStop1= %d\n",iStart1, iStop1);

  	      	    if((iStart1 < 0 || iStart1 >= iStop1) && kComb[i] > nn){
  	      	    	iStart1 = iStart+kComb[i];
  	      	    	iStop1 = iStart1+strlen(DailyNY_Names[kCombIdx[i]]);
  	      	    }else
  	              getSecondStringPick3TX(lpFieldsResults1, iStop1, iStop, &iStart1, &iStop1);
  //       printf ("2--------------------------------------------iStart1= %d, iStop1= %d\n",iStart1, iStop1);
                if(iStart1< iStop1) {
             	     copyLineWithSpace(lpFieldsResults1, iStart1, iStop1);
             	     matchDailyTerms();
                }
                else{
   	             for(j=0;j<4;j++){
   		            result[j] = '?';
   		            flConf[j] = 0;
   	             }
                }
                addEOL();
                addLineToTmp();
  //     printf ("------resultTmp= %s\n",resultTmp);
                k++;
                if(kTab[k] < 0) break;
  	     }
  		} else {
     	        result[0] = 13;
     	        result[1] = 0;
              addLineToTmp();
  //   printf ("empty------resultTmp= %s\n",resultTmp);
  			k++;
  		}

  // printf ("3------result= %s\n",result);
  	  i++;

     }
     copyResultsBack(lpFieldsResults1);

     mergeTwoResults(lpFieldsResults, lpFieldsResults1, nn);

  }
  void getDollarAmountNYnew(int iSt, int idx, char *dollarAm){
  int i, k, n, n1, j, iStart = iSt+strlen(DailyNY_Names[idx])-1;
  int len, cc0, cc;

  //   printf("result[iStart]= %c, result[iStart+1]= %c\n", result[iStart], result[iStart+1]);

     len = strlen(result);
     i = iStart;
     if(result[iStart-1] > 32 && result[iStart] > 32 && result[iStart+1] > 32) iStart++;
     i = iStart;
  //   while(i < len && result[i] > 32) i++;
     while(i < len && result[i] <= 32) i++;
     k = 0;
     while(i < len ){dollarAm[k] = result[i];k++; i++;}

//     dollarAm[k] = 0;
//     printf("-------------- k= %d \n", k);
     cc = 0;
     for(j=0;j<k;j++){
    	 if(dollarAm[j] >= '0' && dollarAm[j] <= '9') cc++;
    	 if(dollarAm[j] == ' ' && cc>2) {dollarAm[j] = 0;k=j;break;}
     }

     cc0 = 0;n = -1;
     for(j=0;j<k;j++){
  		 if(dollarAm[j] == '0') cc0++;
  		 else cc0= 0;
  		 if(cc0==2){n=j-1;break;}
     }
//    printf("-------------- n= %d \n", n);

     if(n>3){
  	   if(dollarAm[n-1] == 32 &&  dollarAm[n-2] > 32){
  		   n1 = n-2;
  		   if(dollarAm[n-3] > 32 &&  dollarAm[n-4] == 32)   n1--;
  		   i = 0;
  		   for(j=n1;j<k;j++){
  			   dollarAm[i] = dollarAm[j];i++;
  		   }
  		   k = i;
  	   }
     }
     if(k == 3){
    	 dollarAm[3] = dollarAm[2];
    	 dollarAm[2] = dollarAm[1];
         k++;
         n = 1;
     }else {
        n = 0;
        if(dollarAm[1] == 32) n = 1;
        else  if(dollarAm[2] == 32) n = 2;
        if(n==0){
          if(k == 4) n = 1;
          else{
      	    cc0 = 0;
      	    for(j=0;j<k;j++){
      		    if(dollarAm[j] == '0') cc0++;
      		    else cc0= 0;
      		    if(cc0==2){n=j-2;break;}
      	    }
          }
        }
     }
     if(n==0) n = 1;
     dollarAm[n] = '.';
     n1 = n+1;
  // printf("--------------k= %d, n= %d\n", k, n);
     if(k-n > 2){
    	 cc0 = 0;i=n1;
    	 for(j=n1;j<k;j++){
    		 if(dollarAm[j] == '0') cc0++;
    		 else cc0= 0;
    		 if(cc0==2){i=j-1;break;}
    	 }
  // printf("--------------i= %d, n= %d\n", i, n);
  	  if(i > n1){
  	  	 for(j=n1;j<i;j++)
  	  		 dollarAm[j] = 32;
  	  }
     }

     i = 0;
     for(j=0;j<k;j++){
  	   if(dollarAm[j] >32){
  		   dollarAm[i] = dollarAm[j];i++;
  	   }
     }
     dollarAm[n+3] = 0;
  //    printf("--------------dollarAm= %s, n= %d \n", dollarAm, n);

     dollarAm[n+2] = '0';
     if(dollarAm[n+1] != '0' && dollarAm[n+1] != '5') dollarAm[n+1] = '0';
     dollarAm[n+3] = 0;

     if(n == 2){
        if(dollarAm[0]<'0' || dollarAm[0]>'9') dollarAm[0] = 32;
        else dollarAm[0] = '?';
     }
     if(n > 2) strcpy(dollarAm,"?.00");

     for(j=0;j<strlen(dollarAm);j++){
  	   if((dollarAm[j]<'0' || dollarAm[j]>'9') && dollarAm[j] !='.' && dollarAm[j]!='?' && dollarAm[j]!=' ')
  		   dollarAm[j] = '?';
     }

     if(dollarAm[0] == '8') dollarAm[0] = '0';
  // printf("-----Dollar amount=%s\n", dollarAm);
  }

  void cleanTicketNumbers_Daily_NY(LPTICKETRESULTS ticketResults, int idx, int nn) {
  	 LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[idx];
  	 char *resultsStr = lpFieldsResults->result;
  //	 LPFIELDSRESULTS lpFieldsResults1;
  //	 char *resultsStr1;
  	 int  i, k, kS;
  	 int spaceNum;
  	 int ret;
  	 int iStart, iStop;
  	 int iStart1, iStop1;
  	 char strT[10];
  	 int numLines = 0;
  	 char dollarAm[10];
  	 int firstLineIdx, lastLineIdx, strIdx;

  	 resultTmp[0] = 0;
  // get number of lines
  	 i = 0;
  	 while(1){
  		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  		numLines++;
  		i++;
  	 }
  	 k = numLines-1;
  //    printf ("----------numLines= %d\n",numLines);

  // find first line
       firstLineIdx = 0;
       i = 0;
  	    while(i < numLines){
  		   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
  		   copyLine(lpFieldsResults, iStart, iStop-1);
  //  printf ("1------result= %s\n",result);
  		   if(testLine_DailyNewNY(nn, &strIdx) > 0) {firstLineIdx = i;break;}
  		   i++;
  //		    printf ("------result= %s\n",resultTmp);
  	    }
  	    if(i>0){
            for(k=0;k<i;k++)
  	        lpFieldsResults->lineQuadStatus[k] = 0;
  	    }
  	    resultTmp[0] = 0;
  	    result[0] = 0;

//     printf ("------firstLineIdx= %d, numLines= %d\n",firstLineIdx, numLines);


     lastLineIdx = numLines-1;
     while(lastLineIdx > firstLineIdx){
  	   if(getLine(resultsStr, lastLineIdx, &iStart, &iStop) < 0) break;
  	   copyLine(lpFieldsResults, iStart, iStop-1);
  //  printf ("1------result= %s\n",result);
  	   if(testLine_DailyNewNY(nn, &strIdx) > 0) break;
  	   lastLineIdx--;
  //		    printf ("------result= %s\n",resultTmp);
     }
//     printf ("------lastLineIdx= %d, numLines= %d\n",lastLineIdx, numLines);

     if(lastLineIdx<numLines-1){
       for(k=lastLineIdx+1;k<numLines;k++)
         lpFieldsResults->lineQuadStatus[k] = 0;
     }

//     printf ("------firstLineIdx= %d, numLines= %d\n",firstLineIdx, numLines);
  	 i = firstLineIdx;
  	 k = 0;
  	 kS = 0;
  //	 yPrev = 0;
  //	 yCur = 0; ddY = 0;
  	 while(i <= lastLineIdx){
//  printf ("--------------------------------------------i=%d, start\n",i);
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//  printf ("1--------------------------------------------i=%d, start\n",i);
  //	 printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);

  //   printLine(resultsStr, iStart, iStop);

  //           testLeftSideForLineTX(resultsStr, &iStart, iStop);
  //           shift = iStart-iStart1;
           copyLine(lpFieldsResults, iStart, iStop-1);
//  printf ("------result= %s\n",result);

  	     iStart1 = -1;
  		 iStop1 = 0;
  		 strIdx = -1;
  		 k = testLine_DailyNewNY(nn, &strIdx);
//   printf ("--------------------------------------------i=%d, k= %d, nn= %d\n",i,k, nn);
           ret = -1;
           if(k > 0)
             ret = getFirstNumberDaily_3_4_CA( &iStart1, nn, k);


//    printf ("--------------------------------------------i=%d, iStart1= %d, iStop= %d ret= %d\n",i,iStart1, iStop, ret);
  //  printLine(resultsStr, iStart1, iStop);
  //  fflush(stdout);

  	    if( iStart1 >= 0 && ret >= 0 && strIdx >=0 ){
          k -= iStart1;
  // printf ("-------i= %d, k= %d, kS= %d\n",i, k, kS);
  //   	 printf ("-------i= %d, kSumItUp[i]= %d\n",i, kSumItUp[i]);

  	           copyLine(lpFieldsResults, iStart+iStart1, iStop-1);

                   getDollarAmountNYnew(k, strIdx, dollarAm);
//         printf ("------result= %s\n",result);
                   spaceNum = getNumSpaces(k);
  //            printf ("------spaceNum= %d, nn= %d,k= %d\n",spaceNum, nn, k);
                   if(k > nn+spaceNum)
                  	result[nn+spaceNum] = 0;
                   else
                      result[k] = 0;
  //   printf ("0------result= %s, k= %d\n",result, k);
                   strcat(result, DailyNY_Names[strIdx]);
                   strcat(result, dollarAm);

//  	 printf ("2------result= %s\n",result);

                 strT[0] = 'A'+kS;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
                 addEOL();
  //	           addLineToTmpSpace();
//   printf ("2------result= %s, k+2= %d\n",result, k+2);
  	           addLineToTmpSpaceN(k+2);

  //     printf ("------resultTmp= %s\n",resultTmp);
  	           if(i+1 < 20) kS++;
         	       k++;
  	    }else{
  	    	lpFieldsResults->lineQuadStatus[i] = 0;
  	    }
  	    i++;
//  	    fflush(stdout);
  	 }
  	 copyResultsBack(lpFieldsResults);

  // printLine(resultsStr, 0, strlen(resultsStr)-1);

  }

int  postProcessingNumbers_NY(LPTICKETRESULTS ticketResults, int i){
//	int status = 0, prevPostProc = 0;
	int status = 0;
	LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[i];
//	prevPostProc = testPrevPostProc(lpFieldsResults);
//	if(prevPostProc == 1) return status;
	switch(ticketTypeInt){
	case 0: // LOTTO NY
	case 3: // SWEET MILLION NY
	case 4: // MEGA MILLIONS NY
	case 6: // POWER BALL NY
		cleanTicketNumbersNY(lpFieldsResults);
		trimSpaces(lpFieldsResults);
//	printField(&ticketResults->fieldsResults[i]);
		testFinalGroups_NY(lpFieldsResults);
        trimSpaces(lpFieldsResults);
//	printField(&ticketResults->fieldsResults[i]);
		testQP_NY(lpFieldsResults);
		trimSpaces(lpFieldsResults);

//		testNumbers_LastPartCA(ticketResults, lpFieldsResults);
//		printField(&ticketResults->fieldsResults[i]);
//		trimSpaces(lpFieldsResults);
//		testNumbersLinesAfterPostProc(ticketResults, lpFieldsResults);
//		trimSpaces(lpFieldsResults);
	   break;
	case 1: // NUMBERS NY
		cleanTicketNumbers_Daily_NY(ticketResults, i,  3);
		trimSpaces(lpFieldsResults);
	   break;
	case 5: // WIN 4 NY
		cleanTicketNumbers_Daily_NY(ticketResults, i,  4);
		trimSpaces(lpFieldsResults);
	   break;
	case 2: // TAKE 5
	case 7: // PICK 10
		cleanTicketNumbersPick10NY(lpFieldsResults);
		trimSpaces(lpFieldsResults);
//	printField(&ticketResults->fieldsResults[i]);
		testFinalGroupsP10_NY(lpFieldsResults);
		trimSpaces(lpFieldsResults);
//	printField(&ticketResults->fieldsResults[i]);
	    break;

	}
    status = getNumLowConfidentChars(lpFieldsResults, 20);
	return status;
//	printf("------------------------------------------------status= %d\n", status);
}
int matchDayNY_All(int *idxMaxOut, int *kOut){
	int cc;
	int i,  ccMax;
	int kk, k, idxMax;
	ccMax = 0;
	idxMax = -1;k= -1;
	*idxMaxOut = idxMax;
	*kOut = k;
  	for(i=0;i<len_Day_Names;i++) {
  		cc =matchToStringResult(Day_Names[i], &kk);
  	   if(ccMax < cc)
  	      if(ccMax < cc && kk < 4){ccMax = cc;idxMax = i;k = kk;}
//   	printf ("---Day_Names[i]= %s, cc= %d, ccMax= %d, kk= %d\n",Day_Names[i], cc, ccMax, kk);
  	}
	*idxMaxOut = idxMax;
	*kOut = k;
	return ccMax;
}
int matchMonthNY_All(int *idxMaxOut, int *kOut){
	int cc;
	int i, ccMax;
	int kk, k, idxMax;
	ccMax = 0;
	idxMax = -1;k= -1;
	*idxMaxOut = idxMax;
	*kOut = k;
  	for(i=0;i<len_Month_Names;i++) {
  		cc =matchToStringResult(Month_Names[i], &kk);
  	   if(ccMax < cc)
  	      if(ccMax < cc && kk > 1 && kk < 7){ccMax = cc;idxMax = i;k = kk;}
//       	printf ("---Month_Names[i]= %s, cc= %d, ccMax= %d, kk= %d\n",Month_Names[i], cc, ccMax, kk);
  	}
	*idxMaxOut = idxMax;
	*kOut = k;
	return ccMax;
}

int  postProcessingDateA_NY(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 int i, k;
	 int iStart, iStop;
	 int line;
	 int cc, ccD, ccM, ccMax;
	 int lineD, lineM, kkD, kkM;
	 int lineMax, lineMaxD, lineMaxM, kkMaxD, kkMaxM;

//	 cc = matchDayA_All(resultsStr, iStart, iStop);break;

//	 *type = 0;
//     *yT = 0;
//     *yB = 0;
	 resultTmp[0] = 0;
// test every line
	 i = 0;
	 line = -1;
	 ccMax = 0;
	 lineMax = -1;
	 lineMaxD= 0; lineMaxM = 0;kkMaxD = 0; kkMaxM = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
        copyLine(lpFieldsResults, iStart, iStop-1);
//    printf("-------result = %s\n", result);

		ccD = matchDayNY_All(&lineD, &kkD);
//printf("-------ccD = %d, lineD= %d, kkD= %d\n", ccD, lineD, kkD);
		ccM = matchMonthNY_All(&lineM, &kkM);
		cc = ccD+ccM;
//printf("-------cc = %d\n", cc);
		if(ccMax < cc && ccMax<4) {ccMax = cc;lineMax = i;lineMaxD = lineD; lineMaxM = lineM;kkMaxD = kkD; kkMaxM = kkM;}
		i++;
	 }
//printf("-------ccMax = %d, lineMax= %d\n", ccMax, lineMax);


	if(ccMax > 3 && lineMax >= 0 && getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {
                strcpy(result, Day_Names[lineMaxD]);strcat(result, " ");
                strcat(result, Month_Names[lineMaxM]);//strcat(result, " ");
                k = strlen(result);i=iStart+kkMaxM+3;
                if(i<iStop)
                   if(resultsStr[i] == 32) i++;
                result[k] = resultsStr[i];k++;i++;
                if(iStop-i > 3)
                   if(resultsStr[i] == 32) i++;
                result[k] = resultsStr[i];k++;i++;
                result[k] = 0;
                strcat(result, " ");k = strlen(result);

                if(i<iStop)
                   if(resultsStr[i] == 32) i++;
                result[k] = resultsStr[i];k++;i++;
                if(i<iStop)
                   if(resultsStr[i] == 32) i++;
                result[k] = resultsStr[i];k++;i++;
                result[k] = 0;
/*
                for(i=0;i<2;i++){
                   if(i+n<iStop)
                     result[k] = resultsStr[i+n];k++;
                }
                result[k] = 0;
                strcat(result, " ");k = strlen(result);
                n += 2;
                if(n<iStop)
                   if(resultsStr[n] == 32) n++;
                for(i=0;i<2;i++){
                   if(i+n<=iStop && resultsStr[i+n] > 32)
                     result[k] = resultsStr[i+n];k++;
                }
                result[k] = 0;
*/
//	        copyLine(lpFieldsResults, iStart, iStop);
//      printf("------result= %s\n", result);
//      printf("1-------resultTmp = %s, line= %d, yT= %d, yB= %d \n", resultTmp, line, *yT, *yB);
		}else{
			result[0] = 0;
		}
        addLineToTmp();
// printf("------resultTmp= %s\n", resultTmp);
        line = lineMax;

	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}

void  postProcessingDate_NY(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults = NULL;
	 int i, iA, line;
//	 int yT, yB;
//	 int type;

	 iA = -1;
	 line = -1;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	    			line = postProcessingDateA_NY(lpFieldsResults);
	    			iA = i;
	    			break;
	    		}

	    	}
	    }
//	printf("-------resultTmp = %s, line= %d, iA= %d, yT= %d, yB= %d \n", resultTmp, line, iA, yT, yB);
	resultTmp[12] = 0;
   if(lpFieldsResults != NULL)
		 copyResultsBack(lpFieldsResults);


//        cleanYear(ticketResults);
//     if(type == 0)
//        cleanDateField(ticketResults);
	 return;
}

void ticketPostProcessing_NY(LPTICKETDATA ticketDataPtr, LPTICKETRESULTS ticketResults){
	int i;


//	  printf ("Results before post-processing----------------------------------------\n");
//	  saveLog("Results before post-processing----------------------------------------", "\n");
//		printResults_1(ticketResults);
//	 printf ("-----------------------------------------------------------------------\n");
//	 saveLog("-----------------------------------------------------------------------", "\n");
//	fflush(stdout);


    ticketResults->type = ticketTypeInt;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
//    		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {postProcessingLogo_CA(ticketResults);setLogoSlices(ticketResults);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_NY(ticketResults, i);
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {postProcessingDate_NY(ticketResults);setDateSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN1") == 0) postProcessingDate1_TX(ticketResults);
    	}
    }

}
