/*
 * PostprocessingOtherSt.c
 *
 *  Created on: Apr 15, 2015
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

extern int US_State;
void printLine(char *resultsStr, int iStart, int iStop);
int getLine(char* results, int n, int* iStart, int* iStop);

void copyLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
//void addEOL(void);
void copyResultsBack(LPFIELDSRESULTS lpFieldsResults);
void replaceChars(int iStart, int iStop, char* strT);
int getNumberOfGroups(char* results, int iStart, int iStop);
int getNumChars(char *resultsStr, int iStart, int iStop);
int testBeginCut(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void addLineToTmp(void);
int getNumLowConfidentChars(LPFIELDSRESULTS lpFieldsResults, int tH);


void addSpacesToNumbersTX(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults);
int testLineConfAllorN2_TX(LPFIELDSRESULTS lpFieldsResults, int line);
int testLineConf_Modified(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
int testOnesInsideLine(char *resultsStr, int iStart, int iStop);
void testLeftSideForLineTXC5(LPFIELDSRESULTS lpFieldsResults, int *iStart, int iStop);
void testGroupsTX(char *resultsStr, int iStart, int iStop, int *cD, int *cS);
int maxDigitGroup(char *resultsStr, int iStart, int iStop);
int maxDigitGroup1(char *resultsStr, int iStart, int iStop);
void recalcSpaces(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
void testTrashBegEndM(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
void cleanAgainQP(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
void cleanOneDigit_TwoStep(char *resultsStr, int iStart, int iStop);
void testFirstQPGroup_TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void trimSpacesResult(void);
void setSpaceAtStart(char *resultsStr, int iStart1, int iStart2);
void getFirstCharFromNumLineM(int* iStart, int* iStop );
void testTrashBegEndM(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
int getFirstQPFromNumLineTwoStep(void);
int recalcNumGroups(void);
void testNumbers_InsideLineTX(LPFIELDSRESULTS lpFieldsResults, int gr2);
void shrinkResults(int iStart, int iStop);

//void getFirstCharFromNumLineTS(int* iStart, int* iStop);

//extern char *Month_Names[];
//extern char *Day_Names[];
extern char        resultTmp[MAXCHARS+2];
//extern WORD        flConfTmp[MAXCHARS+2];
//extern QUADRANGLE  flQuadTmp[MAXCHARS+2];
extern char result[200];
extern WORD flConf[200];
//extern QUADRANGLE flQuad[200];
extern int linesPostCnf;

void cleanTicketNumbers_OtherSt(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, numGrCorr, i, k;
	 int iStart, iStop, iStartTmp;
	 char strT[10];
	 int numLines = 0;

	 int thGroups = 5;
	 int numChars;
	 int beginCut = 0;
	 int ccD, ccS;
	 int firstLineIdx = 0;
	 int linesTab[100], idxLines = 0;
	 int skipLine = 0;

	 resultTmp[0] = 0;
// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
//	  fflush(stdout);
// add spaces !!!
	 addSpacesToNumbersTX(img, lpFieldsResults);
	 resultTmp[0] = 0;

// printf("resultsStr= %s\n",resultsStr);

	 i = 0;
	 firstLineIdx = 0;
	 while(i<numLines){
		 if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			    if(testLineConfAllorN2_TX(lpFieldsResults, i) >= 0){
				    firstLineIdx = i;break;
			    }
		 i++;
	 }
// printf("resultsStr= %s\n",resultsStr);
// printf("-------------------firstLineIdx= %d\n", firstLineIdx);

     if(firstLineIdx>0){
        for(k=0;k<i;k++)
          lpFieldsResults->lineQuadStatus[k] = 0;
     }

//      printf ("--------firstLineIdx= %d\n", firstLineIdx);

	 k = numLines-1;
	 while(k>firstLineIdx){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
//   printf ("--------k= %d\n", k);
//   printLine(resultsStr, iStart, iStop);

		 if(testLineConf_Modified(lpFieldsResults, iStart, iStop) < 0){
			 lpFieldsResults->lineQuadStatus[k] = 0;
			 numLines--;
		 }
		 else
			 break;
		 k--;
	 }
//	  printf ("--------numLines= %d\n", numLines);
//	  fflush(stdout);

// second line test---------------
	 i = firstLineIdx;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
        if(testOnesInsideLine(resultsStr, iStart, iStop) > 0) {i++;continue;}
		   testLeftSideForLineTXC5(lpFieldsResults, &iStart, iStop);

//printLine(resultsStr, iStart, iStop);
//fflush(stdout);

	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
     	  beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
           if(beginCut > 0)
         	numGr++;
        }
        testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
        numGrCorr = 0;
        if(ccD == 5 && ccS == 4) numGrCorr = 2;
        if(ccD < 4 || ccS < 3) numGrCorr = -3;

//printLine(resultsStr, iStart, iStop);
//printf ("--------i= %d, numGr= %d, numGrCorr= %d, thGroups= %d, strlen(resultsStr)= %d, numChars= %d\n", i, numGr, numGrCorr, thGroups, strlen(resultsStr), numChars);

	    if(numGr+numGrCorr > thGroups && strlen(resultsStr) > 15 && numChars < 35){
	    	linesTab[idxLines] = i;idxLines++;
	    }
	    if(idxLines >= 100) break;
        i++;
	 }
//printf("-------------------idxLines= %d\n", idxLines);
//for(i=0;i<idxLines;i++){
//    printf("linesTab[ %d]= %d\n", i, linesTab[i]);
//}

// has to be tested !!!!!!!!!!!!!!!!!
if(idxLines>0)
	   firstLineIdx = linesTab[0];

{int gap = -1;
   if(idxLines > 4) {
      for(i=0;i<3;i++){
	      if(linesTab[i+1] - linesTab[i] > 3) {gap = i+1;break;}
      }
   }
//  printf("gap= %d\n", gap);
   if(gap >=0 && idxLines > gap+2) {
	   if(linesTab[gap+1] - linesTab[gap] == 1) {
		   firstLineIdx = linesTab[gap];
	   }
   }

}
//printf("-------------------firstLineIdx= %d\n", firstLineIdx);
// second line test---------------

	 i = firstLineIdx;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//           testLeftSideForLine(resultsStr, &iStart, iStop);
		   testLeftSideForLineTXC5(lpFieldsResults, &iStart, iStop);

//printLine(resultsStr, iStart, iStop);

	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
        	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
            if(beginCut > 0)
            	numGr++;
        }
        testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
        numGrCorr = 0;
        if(ccD == 5 && ccS == 4) numGrCorr = 2;
        if(ccD < 4 || ccS < 3) numGrCorr = -3;

//        if(numGr > 3 && numGr <= thGroups) numGrCorr= testGroupsTwoStep(resultsStr, iStart, iStop, 4);

//   printf ("--------i= %d, ccD= %d, ccS= %d, numGr= %d, numGrCorr= %d\n", i, ccD, ccS, numGr, numGrCorr);
//       	printLine(resultsStr, iStart, iStop);
        {
        	int maxD_G = maxDigitGroup(resultsStr, iStart, iStop);
        	int maxD_G1 = maxDigitGroup1(resultsStr, iStart, iStop);
        	if(numGr > 7 && maxD_G > 6) numGr = 0;
        	if(maxD_G1 > 4 && skipLine == 1) numGr = 0;
//       	  printf ("--------i= %d, maxD_G= %d\n", i, maxD_G);
        }
//    printf ("--------i= %d, numGr= %d, numGrCorr= %d, thGroups= %d, strlen(resultsStr)= %d, numChars= %d\n", i, numGr, numGrCorr, thGroups, strlen(resultsStr), numChars);
        skipLine = 0;
	    if(numGr+numGrCorr > thGroups && strlen(resultsStr) > 15 && numChars < 35){
//  	printLine(resultsStr, iStart, iStop);
            iStartTmp = iStart;
	    	recalcSpaces(img, lpFieldsResults, &iStart, &iStop);

            testTrashBegEndM(lpFieldsResults, &iStart, &iStop);
//       	              printLine(resultsStr, iStart, iStop);
            cleanAgainQP(lpFieldsResults, &iStart, &iStop);

//   	printLine(resultsStr, iStart, iStop);
	    cleanOneDigit_TwoStep(resultsStr, iStart, iStop);
//printLine(resultsStr, iStart, iStop);
        if(iStartTmp < iStart) setSpaceAtStart(resultsStr, iStartTmp, iStart);
        testFirstQPGroup_TX(lpFieldsResults, iStart, iStop);

//      	if(getLine(resultsStr, i, &iStart, &iStop) < 0) continue;


	       copyLine(lpFieldsResults, iStart, iStop);
      	   trimSpacesResult();
//	       printf("beginCut= %d\n", beginCut);
// printf ("1--------result= %s\n", result);
	       if(beginCut > 0){
               strT[0] = 'A'+k;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
//              getFirstCharFromNumLine(&iStart, &iStop);
//              getFirstCharFromNumLineC5(&iStart, &iStop, numGr);
	           getFirstQPFromNumLineTwoStep();
//       printf("2------------- result= %s\n", result);

               getFirstCharFromNumLineM(&iStart, &iStop);break;

//   printf("3------------- result= %s\n", result);
	           numGr = recalcNumGroups();
//   printf("------------- numGr= %d, iStart= %d, iStop= %d, result[1]= %c\n", numGr, iStart, iStop, result[1]);
              if(numGr < 10 && iStart == 0 && iStop == 0 && result[1] != 32) {
                 strT[0] = 'A'+k;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
             }else{
                 strT[0] = 'A'+k;
                 strT[1] = 0;
                replaceChars(iStart, iStop, strT);
             }
	       }
//       printf("4------------- result= %s\n", result);
           iStop = -1;
//           ret = getFirstQPFromNumLineTwoStep(&iStart, &iStop);
//        printf("3------------- result= %s\n", result);
//           getFirstQPFromNumLineTwoStep();

//           if(ret >= 0) {
//               replaceChars(iStart, iStop, "QP");
//           }
//           if( ret > 0 || numGr > 5) {
//              if(getSecondQPFromNumLine(&iStart, &iStop) >=0){
//                 replaceChars(iStart, iStop, "QP");
//              }
//           }
//          printf ("1--------result= %s\n", result);
	       addLineToTmp();
	       k++;
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
	        skipLine = 1;
	    }
//  printf("-- resultTmp= %s", resultTmp);
	    i++;
	 }
	 copyResultsBack(lpFieldsResults);

//	 printf("end; lpFieldsResults->result= %s", lpFieldsResults->result);
}

 void emptyTicketNumbers(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults) {
   char *resultsStr = lpFieldsResults->result;
   int i;
   int iStart, iStop;

	 resultTmp[0] = 0;
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		lpFieldsResults->lineQuadStatus[i] = 0;
		i++;
	 }
	 copyResultsBack(lpFieldsResults);

 }

int  postProcessingNumbers_OtherSt(LPIMGDATA img, LPTICKETRESULTS ticketResults, int i){
	int status = 0;
	LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[i];
	/*
	1 - Powerball
	2- Mega Millions
	*/

	switch(ticketTypeInt){

    case 1: // POWER BALL
    case 2: // MEGA MILLIONS
	   cleanTicketNumbers_OtherSt(img, lpFieldsResults);
	   testNumbers_InsideLineTX(lpFieldsResults, 4);
	   break;
    default:
 	   emptyTicketNumbers(img, lpFieldsResults);
 	   break;
	}
    status = getNumLowConfidentChars(lpFieldsResults, 20);
	return status;
//	printf("------------------------------------------------status= %d\n", status);
}

void ticketPostProcessing_OtherStates(LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPTICKETRESULTS ticketResults){
	int i;
//	  printf ("Results before post-processing----------------------------------------\n");
//	  saveLog("Results before post-processing----------------------------------------", "\n");
//		printResults_1(ticketResults);
//	 printf ("-----------------------------------------------------------------------\n");
//	 saveLog("-----------------------------------------------------------------------", "\n");

	linesPostCnf = 0;
    ticketResults->type = ticketTypeInt;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
//	 printf ("name= %s\n", ticketResults->fieldsResults[i].name);

//    		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {postProcessingLogo_TX(ticketResults);setLogoSlices(ticketResults);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_OtherSt(img, ticketResults, i);
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {postProcessingDate_TX(ticketResults);setDateSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN1") == 0) postProcessingDate1_TX(ticketResults);


    	}
    }
}
int getFirstSpace_Result(void){
   int i, k = -1, len;
      len = strlen(result);
	  for(i=0;i<len;i++){
		  if(result[i]<=32) {k=i;break;}
	  }
	return k;
}
void trimTX_Ret_Num(void ){
 int i, k, kk, n, len;
 len = strlen(result);

   if(result[len-1]<32) {result[len-1] = 0;len = strlen(result);}
   k = getFirstSpace_Result();
//   printf("----------k= %d, len= %d\n",k,len);
//   printf("----------result= %s\n",result);

   if(k < 6 && len > 9) shrinkResults(k,k);

   k = getFirstSpace_Result();len = strlen(result);

//   printf("2----------k= %d, len= %d\n",k,len);
//   printf("2----------result= %s\n",result);


//   if(k==6 && len > 9) result[9] = 0;
   if(k > 6) {
	 n = k-6;kk = -1;
	for(i=0;i<n;i++){
			if(result[i] <= 32 || flConf[i] < 20) kk=i;
//        			printf("result[ %d ]= %c, cnf= %d\n",i,result[i],flConf[i]);
		}
//       			printf("n= %d, kk= %d\n",n,kk);
			if(kk >=0)
			   shrinkResults(0,kk);
   }
   k = getFirstSpace_Result();
   len = strlen(result);
   if(k==6 && len > 9) result[9] = 0;
   len = strlen(result);
   if(k==6 && len == 9) {
	   if(result[7] != '0' && result[7] != '9') result[7] = '0';
//	   if(result[8] != '0' && result[8] != '8' && result[8] != '9') result[8] = '0';
   }
//   k = getFirstSpace_Result();
   len = strlen(result);
   if(len < 8 || len > 10) result[0] = 0;


}
void trimRet_Num_byState(void){
    switch(US_State) {
    case 0:
    case 1: // TX

    	trimTX_Ret_Num();
         break;
    }

}
