/*
 * PostprocessingTX2.c
 *
 *  Created on: Apr 23, 2015
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

extern char        resultTmp[MAXCHARS+2];
//extern WORD        flConfTmp[MAXCHARS+2];
//extern QUADRANGLE  flQuadTmp[MAXCHARS+2];
extern char result[200];
extern WORD flConf[200];
//extern QUADRANGLE flQuad[200];

int getLine(char* results, int n, int* iStart, int* iStop);
void copyLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void shrinkResults(int iStart, int iStop);
void copyResultsBack(LPFIELDSRESULTS lpFieldsResults);
//void replaceChars(int iStart, int iStop, char* strT);
void addLineToTmp(void);
int matchToStringResult(char *str, int *kM);
void trimRet_Num_byState(void);


int findLineRET_NUM(LPTICKETRESULTS ticketResults, int n, int *iShift){
  LPFIELDSRESULTS lpFieldsResults;
  char *resultsStr;
  int i, cc, kk;
  int maxCC, iMax, maxCCg, iMaxg;
  int iStart, iStop;
  int shiftTH = 6;

	 lpFieldsResults = &ticketResults->fieldsResults[n];
	 resultsStr = lpFieldsResults->result;

	 i = 0;
	 iMaxg = -1;maxCCg = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		iMax = -1;maxCC = 0;
		cc =matchToStringResult("PRINTED ON ", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("PRINTED ON", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("RINTED ON ", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("RINTED ON", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("INTED ON ", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("INTED ON", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("PRINTEDON ", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}
		cc =matchToStringResult("PRINTEDON", &kk);
		if(kk > shiftTH) cc = 0;
		if(maxCC<cc){maxCC=cc;iMax=i;}

//printf("---result= %s\n", result);
//printf("---- maxCC= %d, iMax= %d, kk= %d\n",maxCC,iMax,kk);
		if(maxCC > 4){
			if(getLine(resultsStr, i+1, &iStart, &iStop) < 0) break;
			copyLine(lpFieldsResults, iStart, iStop);
			cc =matchToStringResult("RET", &kk);
			if(kk > shiftTH) cc = 0;
			if(cc>1){maxCC +=cc;}
		}
		if(maxCCg<maxCC){maxCCg=maxCC;iMaxg=iMax+1;*iShift = kk;}
	    i++;
	 }
//     printf("---- maxCCg= %d, iMaxg= %d\n",maxCCg,iMaxg);
   if(maxCCg > 4)
       return(iMaxg);
   else{
// try again
	   maxCC = 0;
	   for(i=0;i<3;i++){
	     if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	     iMaxg = -1;maxCCg = 0;
		 copyLine(lpFieldsResults, iStart, iStop);
		 cc =matchToStringResult("RET", &kk);
		 if(kk > shiftTH) cc = 0;
// printf("---result= %s\n", result);
//  printf("---- cc= %d, kk= %d\n",cc,kk);
//		 if(maxCC < cc){maxCC =cc;}
		 if(cc==3){
			*iShift = kk;
			return (i);
		 }
       }
   }

   return -1;
}
int findLineRET_NUM2(LPTICKETRESULTS ticketResults,  int *iShift){
//	 LPFIELDSRESULTS lpFieldsResults;
//	 char *resultsStr;
	 int i,  line;

	 line = -1;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "RET_NUMA") == 0) {
	    			line = findLineRET_NUM(ticketResults, i, iShift);
	    		}
	    	}
     }

     return line;

}
void createEmptyRet_Num(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 int i;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "RET_NUMN") == 0) {
	    		    lpFieldsResults = &ticketResults->fieldsResults[i];
    			    lpFieldsResults->result[0] = 0;
    			    ticketResults->fieldsResults[i].name[strlen(ticketResults->fieldsResults[i].name)-1] = 0;
    			    return;
	    		}
	    	}
    }
}
void createFieldRet_Num(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 int i;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "RET_NUMN") == 0) {
	    		    lpFieldsResults = &ticketResults->fieldsResults[i];
    			    copyResultsBack(lpFieldsResults);
    			    ticketResults->fieldsResults[i].name[strlen(ticketResults->fieldsResults[i].name)-1] = 0;
    			    return;
	    		}
	    	}
    }
}
void testStartOfRetNum(int *iShift){
	int i, sh, k;
	sh = *iShift+1;
	if(sh >= strlen(result)-4) return;
	k = sh;
	for(i=sh;i<strlen(result);i++){
		if(result[i] > 32 && flConf[i] > 20) {k=i;break;}
//		printf("result[ %d ]= %c, cnf= %d\n",i,result[i],flConf[i]);
	}
    if(k > sh) *iShift = k-1;
}
void postProcessingRetNum_TX_Old(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i,  line;
	 int iStart, iStop;
//	 int k1, k2;
	 int iShift = 0;
	 char title[100];
//	 iA = -1;
	 line = -1;

     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
	    			  line = findLineRET_NUM(ticketResults, i, &iShift);
                      if(line >=0){
//                    	  iA = i;
                    	  strcpy(title, "DATEN");
                      }
                      else{
                    	  line = findLineRET_NUM2(ticketResults,  &iShift);
                    	  strcpy(title, "RET_NUMN");
                      }
	    		}
	    	}
    }
//   printf("---- line= %d, iShift= %d, title= %s\n",line,iShift,title);

    if(line < 0 || iShift > 4){
    	createEmptyRet_Num(ticketResults);
    	return;
    }

    for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, title) == 0) {
	    		    lpFieldsResults = &ticketResults->fieldsResults[i];
	    		    resultsStr = lpFieldsResults->result;
   				    if(getLine(resultsStr, line, &iStart, &iStop)<0) {
   				    	createEmptyRet_Num(ticketResults);
   				    	return;
   				    }
   				    resultTmp[0] = 0;
   				    copyLine(lpFieldsResults, iStart, iStop);
   				    iShift += 2;
   				    testStartOfRetNum(&iShift);
   				    shrinkResults(0,iShift);
   				    trimRet_Num_byState();
   				    addLineToTmp();
   				    createFieldRet_Num(ticketResults);
//   				    copyResultsBack(lpFieldsResults);
   				    return;
	    		}
	    	}
    }

}

void postProcessingRetNum_TX(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i,  line;
	 int iStart, iStop;
//	 int k1, k2;
	 int iShift = 0;
//	 char title[100];
//	 iA = -1;
	 line = -1;

     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "RET_NUMA") == 0) {
	    			  line = findLineRET_NUM(ticketResults, i, &iShift);
                      if(line >=0){
//                    	  iA = i;
//                    	  strcpy(title, "DATEN");
                      }
	    		}
	    	}
    }
//   printf("---- line= %d, iShift= %d\n",line,iShift);

    if(line < 0 || iShift > 6){
    	createEmptyRet_Num(ticketResults);
    	return;
    }

    for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "RET_NUMN") == 0) {
	    		    lpFieldsResults = &ticketResults->fieldsResults[i];
	    		    resultsStr = lpFieldsResults->result;
   				    if(getLine(resultsStr, line, &iStart, &iStop)<0) {
   				    	createEmptyRet_Num(ticketResults);
   				    	return;
   				    }
   				    resultTmp[0] = 0;
   				    copyLine(lpFieldsResults, iStart, iStop);
//  printf("-------------------------------- result= %s\n",result);
   				    iShift += 2;
   				    testStartOfRetNum(&iShift);
   				    shrinkResults(0,iShift);
// printf("2-------------------------------- result= %s\n",result);
   				    trimRet_Num_byState();
   				    addLineToTmp();
   				    createFieldRet_Num(ticketResults);
//   				    copyResultsBack(lpFieldsResults);
   				    return;
	    		}
	    	}
    }

}



