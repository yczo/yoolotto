/*
 * PostprocessingCA.c
 *
 *  Created on: Aug 30, 2013
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

int getNumLowConfidentChars(LPFIELDSRESULTS lpFieldsResults, int tH);
int getLine(char* results, int n, int* iStart, int* iStop);
int testLineConf(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
//void testLeftSideForLine(char *resultsStr, int *iStart, int iStop);
int getNumberOfGroups(char* results, int iStart, int iStop);
int getNumChars(char *resultsStr, int iStart, int iStop);
int testBeginCut(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void copyLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void copyResultsBack(LPFIELDSRESULTS lpFieldsResults);
void replaceChars(int iStart, int iStop, char* strT);
void getFirstCharFromNumLine(int* iStart, int* iStop);
void addLineToTmp(void);
int getBiggerGroups(char* results, int iStart, int iStop, int count);
//void cleanTrashInsideLine(int iStart, int iStop, int *numGr);
void shrinkResults(int iStart, int iStop);
void trimSpaces(LPFIELDSRESULTS lpFieldsResults);
void addLineToResult(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void setDateSlices(LPTICKETRESULTS ticketResults);
//void cleanYear(LPTICKETRESULTS ticketResults);
void cleanDateField(LPTICKETRESULTS ticketResults, char *title);
//int matchDayA_TX(char *resultsStr, int iStart, int iStop);
//void cleanDate_TX();
int matchDayA(char *resultsStr, int iStart, int iStop, char* day1, char* day2);
int cmpStringsExact(char *str1, char *str2);
void cleanDate(char* day1, char* day2);

char *Daily3_Names[] = { " STR ", " BOX ", " STR/BOX "};
int len_Daily3_Names = 3;
void addLineToTmpSpace(void);
void getSecondStringPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1);
void copyLineWithSpace(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
int matchToStringResult(char *str, int *kM);
void testLeftSideForLineTX(char *resultsStr, int *iStart, int iStop);
void getFirstNumberPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1, int nn, int kCombi);
void getDollarAmount(char *resultsStr, int start, int stop, char *dollarAm);
void addEOL(void);
void mergeTwoResults(LPFIELDSRESULTS lpFieldsResults, LPFIELDSRESULTS lpFieldsResults1, int nn);
int matchToString(char *results, int iStart, int iStop, char *str);
void setLogoSlices(LPTICKETRESULTS ticketResults);
int matchDayA_All(char *resultsStr, int iStart, int iStop);
int searchForDollarPattern(char *rStr, int i, int stop);
void cleanDate_All();
void addLineToTmpSpaceN(int N);

char *DailyDerby_Names[] = { "GOLD RUSH","LUCKY STAR","HOT SHOT","BIG BEN","CALIFORNIA CLASSIC","WHIRL WIN","EUREKA","GORGEOUS GEORGE","WINNING SPIRIT","SOLID GOLD","MONEY BAGS","LUCKY CHARMS"};
int len_DailyDerby_Names = 12;

void printResults_1(LPTICKETRESULTS ticketResults);

extern char *Month_Names[];
extern char        resultTmp[MAXCHARS+2];
extern char result[200];
extern WORD flConf[200];
extern QUADRANGLE flQuad[200];
extern QUADRANGLE  flQuadTmp[MAXCHARS+2];

int getBestLineN_CA(LPFIELDSRESULTS lpFieldsResults, int yT, int yB){
	char *resultsStr = lpFieldsResults->result;
    int  i, n;
    int iStart, iStop;
    int cc, yTN, yBN;
    int lineMin, dd, ddMin;

	  ddMin = 10000;lineMin = -1;
    if(yT>0 && yB>0) {
	   i = 0;
	   while(1){
		  if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		      cc = 0;yTN = 0;yBN = 0;
		  for(n=iStart;n<iStop;n++) {
			      if(resultsStr[n] > 32) {
		            yTN += lpFieldsResults->flQuad[n].p1Y;
		            yBN += lpFieldsResults->flQuad[n].p3Y;
		            cc++;
			      }
		      }
		      if(cc>0){
			    yTN /= cc;
			    yBN /= cc;
		      }
//	printf ("--cleanGroupInsideCA; ii= %d, yTR= %d, yBR= %d\n", ii, yTR, yBR);
//	printf ("--cleanGroupInsideCA; ii= %d, yTR -yT= %d, yBR -yB= %d\n", ii, yTR -yT, yBR -yB);
		     dd =  min(abs(yTN -yT),abs(yBN -yB));
		     if(ddMin > dd) {ddMin = dd;lineMin=i;}
		  i++;
	   }
	   if(ddMin > 25) lineMin = -1;
    }
    return lineMin;
}

int matchDayA_CA(char *resultsStr, int iStart, int iStop){
	int cc = 0;
	switch(ticketTypeInt){
	case 0:  // SUPER LOTTO CA
	case 1:  // POWER BALL CA
		cc = matchDayA(resultsStr, iStart, iStop, "WED ", "SAT ");break;
	case 2: // MEGA MILLIONS CA
		cc = matchDayA(resultsStr, iStart, iStop, "TUE ", "FRI ");break;
	case 3: // Daily 3
	case 4: // Daily 4
	case 5: // Fantasy 5
	case 6: // Hot Spot
	case 7: // Daily Derby
		cc = matchDayA_All(resultsStr, iStart, iStop);break;
	}
	return cc;
}
int testMonthinLine(char *term){
	  int i;
	  int cc, pen, ccMax;
	  ccMax = 0;

	  for(i=0;i<12;i++){
		  pen = cmpStringsExact(term, Month_Names[i]);
		  cc = 3+pen;
		  if(ccMax <cc ) ccMax = cc;
//printf("Month-------cc= %d, term= %s, Month_Names[i]= %s \n", cc, term, Month_Names[i]);
		}
	  return ccMax;
}

int matchFromDayA_CA(char *resultsStr, int iStart, int iStop, int i1){
	int i;
    char term[10];
    int cnf = 0;
    if(iStop-iStart <= 3) return cnf;
    for(i=iStart+i1;i<iStart+3;i++)
		  term[i-iStart] = resultsStr[i];
    term[3] = 0;
    cnf += testMonthinLine(term);

    return cnf;
}

void cleanDate_CA(){
	switch(ticketTypeInt){

	case 0:  // SUPER LOTTO CA
	case 1:  // POWER BALL CA
		cleanDate("WED", "SAT");break;
	case 2: // MEGA MILLIONS CA
		cleanDate("TUE", "FRI");break;
	case 3: // Daily 3
	case 4: // Daily 4
	case 5: // Fantasy 5
	case 6: // Hot Spot
		cleanDate_All();break;
	}

}


int  postProcessingDateA_CA(LPFIELDSRESULTS lpFieldsResults, int *yT, int *yB, int *type){
	 char *resultsStr = lpFieldsResults->result;
	 int i, k;
	 int iStart, iStop;
	 int line, dd;
	 int cc, ccMax;
	 int lineMax;
	 int y1, y2;
	 int ccMax2, pen;
	 int lineMax2;


	 *type = 0;
     *yT = 0;
     *yB = 0;
	 resultTmp[0] = 0;
// test every line
	 i = 0;
	 line = -1;
	 ccMax = 0;
	 lineMax = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		cc = matchDayA_CA(resultsStr, iStart, iStop);
//printLine(resultsStr, iStart, iStop);
//printf("-------cc = %d\n", cc);
		if(ccMax < cc) {ccMax = cc;lineMax = i;}
		i++;
	 }

//printf("-------ccMax = %d, lineMax= %d\n", ccMax, lineMax);

if(ccMax < 5){
	 i = 0;
	 ccMax2 = 0;
	 lineMax2 = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		dd = iStop-iStart;
		cc = matchFromDayA_CA(resultsStr, iStart, iStop, 0);
		if(dd > 11)
		   cc += matchFromDayA_CA(resultsStr, iStart, iStop, 9);
		else
			cc--;
//printf("-------cc = %d\n", cc);
		cc *= 2;
		pen = abs(17-dd);
// printf("1-------i = %d, pen= %d, dd= %d\n", i, pen, dd);
		cc -= pen;
		pen = 3;
		if(dd > 5 && resultsStr[iStart+5] == ' ') pen--;
		if(dd > 8 && resultsStr[iStart+8] == ' ') pen--;
		if(dd > 14 && resultsStr[iStart+14] == ' ') pen--;
//	 printf("2-------i = %d, pen= %d\n", i, pen);
		cc -= pen;
		if(ccMax2 < cc) {ccMax2 = cc;lineMax2 = i;}
//printf("-------cc = %d, ccMax2= %d, lineMax2= %d\n", cc, ccMax2, lineMax2);
		i++;
	 }

// printf("-------ccMax2 = %d, lineMax2= %d\n", ccMax2, lineMax2);
	 if(ccMax2 > 8){
		 *type = 1;
		 ccMax = ccMax2;
		 lineMax = lineMax2;
	 }
}
//printf("-------ccMax = %d, lineMax= %d\n", ccMax, lineMax);

	if(getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {

		       cc = 0; y1 = 0; y2 = 0;
		      for(k=iStart;k<iStop;k++){
			      if(resultsStr[k] > 32){
		     		y1 += lpFieldsResults->flQuad[k].p1Y;
		     		y2 += lpFieldsResults->flQuad[k].p3Y;
		     		cc++;
		         }
              }
		      if(cc>0){
                 y1 /= cc;
                 y2 /= cc;
		      }
		      *yT = y1;
		      *yB = y2;
	         copyLine(lpFieldsResults, iStart, iStop);
	         if(*type == 0)
	            cleanDate_CA();
             if(ccMax < 3)
            	 result[0] = 0;
	         addLineToTmp();
	         line = lineMax;
//      printf("1-------resultTmp = %s, line= %d, yT= %d, yB= %d \n", resultTmp, line, *yT, *yB);
		}
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}
int  postProcessingDateA_DD_CA(LPFIELDSRESULTS lpFieldsResults, int *yT, int *yB, int *type){
	 char *resultsStr = lpFieldsResults->result;
	 int i;
	 int iStart, iStop;
	 int line, dd;
	 int cc, ccMax;
	 int lineMax;
//	 int y1, y2;
	 int ccMax2, pen;
	 int lineMax2;


	 *type = 0;
     *yT = 0;
     *yB = 0;
	 resultTmp[0] = 0;
// test every line
	 i = 0;
	 line = -1;
	 ccMax = 0;
	 lineMax = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		cc = matchDayA_CA(resultsStr, iStart, iStop);
//printLine(resultsStr, iStart, iStop);
//printf("-------cc = %d\n", cc);
		if(ccMax < cc) {ccMax = cc;lineMax = i;}
		i++;
	 }

//printf("-------ccMax = %d, lineMax= %d\n", ccMax, lineMax);

if(ccMax < 5){
	 i = 0;
	 ccMax2 = 0;
	 lineMax2 = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		dd = iStop-iStart;
		cc = matchFromDayA_CA(resultsStr, iStart, iStop, 0);
		if(dd > 11)
		   cc += matchFromDayA_CA(resultsStr, iStart, iStop, 9);
		else
			cc--;
//printf("-------cc = %d\n", cc);
		cc *= 2;
		pen = abs(17-dd);
// printf("1-------i = %d, pen= %d, dd= %d\n", i, pen, dd);
		cc -= pen;
		pen = 3;
		if(dd > 5 && resultsStr[iStart+5] == ' ') pen--;
		if(dd > 8 && resultsStr[iStart+8] == ' ') pen--;
		if(dd > 14 && resultsStr[iStart+14] == ' ') pen--;
//	 printf("2-------i = %d, pen= %d\n", i, pen);
		cc -= pen;
		if(ccMax2 < cc) {ccMax2 = cc;lineMax2 = i;}
//printf("-------cc = %d, ccMax2= %d, lineMax2= %d\n", cc, ccMax2, lineMax2);
		i++;
	 }

// printf("-------ccMax2 = %d, lineMax2= %d\n", ccMax2, lineMax2);
	 if(ccMax2 > 8){
		 *type = 1;
		 ccMax = ccMax2;
		 lineMax = lineMax2;
	 }
}
//printf("-------ccMax = %d, lineMax= %d\n", ccMax, lineMax);

	if(getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {
/*
		       cc = 0; y1 = 0; y2 = 0;
		      for(k=iStart;k<iStop;k++){
			      if(resultsStr[k] > 32){
		     		y1 += lpFieldsResults->flQuad[k].p1Y;
		     		y2 += lpFieldsResults->flQuad[k].p3Y;
		     		cc++;
		         }
              }
		      if(cc>0){
                 y1 /= cc;
                 y2 /= cc;
		      }
		      *yT = y1;
		      *yB = y2;
*/
	         copyLine(lpFieldsResults, iStart, iStop);
	         if(*type == 0)
	            cleanDate_CA();
             if(ccMax < 3)
            	 result[0] = 0;
	         addLineToTmp();
	         line = lineMax;
//      printf("1-------resultTmp = %s, line= %d, yT= %d, yB= %d \n", resultTmp, line, *yT, *yB);
		}
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}

int adjustDateN_X(){
	int i, x, n, len, lenTmp;
	int d, dMin, kMin;
    lenTmp = strlen(resultTmp);
    n = 0;
    for(i = lenTmp-1;i>0;i--){
    	if(resultTmp[i] >32){
    		n = i;break;
    	}
    }
	x = flQuadTmp[n].p2X;
//printf("-------x= %d, n= %d, lenTmp= %d \n", x, n, lenTmp);
	kMin = 0;dMin = 1000;
	len = strlen(result);
	for(i = 0;i<len;i++){
		if(result[i] >32){
		  d = abs(flQuad[i].p1X - x);
//printf("-------d= %d, i= %d, result[i]= %c, result[i]= %d \n", d, i, result[i], result[i]);
		   if(dMin > d) {dMin = d;kMin = i;}
		   else break;
		}
	}
	return kMin;
}
void filterLastSpace(){
	int len = strlen(result);
	if(len < 3) return;
	if(result[len-2] == 32 && result[len-3] != 32 && result[len-1] != 32){
		result[len-2] = result[len-1];result[len-1] = 0;
		flConf[len-2] = flConf[len-1];
		copyQuad(&flQuad[len-2], &flQuad[len-1], 0, 0);
	}
}
void testDayCAN(){
	if(result[0] > '3')
		result[0] = '?';
	if(result[0] == '3' && result[1] > '1')
		result[0] = '?';
}

void  postProcessingDateOther_CA(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, k, iA, line, len, sh;
	 int iStart, iStop, idxY, flgCut;
	 int ddL;
	 int yT, yB;
	 int type;

	 iA = -1;
	 line = -1;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
	    			line = postProcessingDateA_CA(&ticketResults->fieldsResults[i], &yT, &yB, &type);
	    			iA = i;
	    			break;
	    		}

	    	}
	    }
//	printf("-------resultTmp = %s, line= %d, iA= %d, yT= %d, yB= %d \n", resultTmp, line, iA, yT, yB);

     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN") == 0) {
//	    			 if(line >=0 && iA >= 0) {
	 	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	 	    			 resultsStr = lpFieldsResults->result;
	 	    			 line = getBestLineN_CA(lpFieldsResults, yT, yB);
	 	    			 if(line >= 0) {
	    				    getLine(resultsStr, line, &iStart, &iStop);
    				        if(type == 0){

	    				    resultTmp[7] = 0;
//	    		printf("-------resultTmp = %s \n", resultTmp);
	    				    copyLine(&ticketResults->fieldsResults[i], iStart, iStop);
//			    printLine(resultsStr, iStart, iStop);
	    				    len = strlen(result);
	    				    if(result[len-1] < 32) result[len-1] = 0;
	    				    len = strlen(result);
//printLine(resultsStr, iStart, iStop);
	    				    flgCut = 0;
	    				    idxY = 7;
	    				    k = iStart+idxY;
	    				    ddL = 0;
// printf("-------resultsStr[k] = %s \n", &resultsStr[k]);
	    				    if(k<iStop) {
	    			          copyLine(&ticketResults->fieldsResults[i], k, iStop-ddL);
//	    printLine(resultsStr, k, iStop-ddL);
		    				   len = strlen(result);
//printf("-------len= %d \n", len);
		    				   if(result[len-1] < 32) result[len-1] = 0;
		    				   len = strlen(result);
//  printf("-------len= %d \n", len);
                               sh = 0;
		    				   if(len > 5) {
		    					   sh = adjustDateN_X();
  //printf("-------len= %d, sh= %d \n", len, sh);
		    					   if(sh> 0){
		    						   copyLine(&ticketResults->fieldsResults[i], k+sh, iStop-ddL);
		    						   len = strlen(result);
				    				   if(result[len-1] < 32) result[len-1] = 0;
		    					   }
		    				   }
	    				      if(len > 5){
		    				    	 filterLastSpace();len = strlen(result);
		    				    	 if(len > 5)
		    				    	    strcpy(result, "?? ??");
		    				     }
		    				     testDayCAN();
	   			                 addLineToTmp();
	    				    }
    				        }
	    				    else{
		    				    copyLine(&ticketResults->fieldsResults[i], iStart, iStop);
		    				    len = strlen(result);
		    				    sh = 2;
		    				    for(k=2;k<len;k++){
		    				    	if(result[k] == ' ') {sh = k;break;}
		    				    }
		    				    if(strlen(resultTmp) > 7 && len > sh+2){
			    				    resultTmp[3] = result[sh-2];
			    				    resultTmp[4] = result[sh-1];
			    				    resultTmp[6] = result[sh+1];
			    				    resultTmp[7] = result[sh+2];
		    				    }
		    				    sh++;
		    				    ddL = sh;
		    				    for(k=len-2;k>sh;k--){
		    				    	if(result[k] == ' ') {sh = k;break;}
		    				    }
		    				    if(strlen(resultTmp) > 16 && sh-2 >= 0 && len > sh+2){
			    				    resultTmp[12] = result[sh-2];
			    				    resultTmp[13] = result[sh-1];
			    				    resultTmp[15] = result[sh+1];
			    				    resultTmp[16] = result[sh+2];
		    				    }
	    				    }
//	    				    }
//          		printf("-------resultTmp = %s \n", resultTmp);
	    				 }
	    				 copyResultsBack(&ticketResults->fieldsResults[iA]);
	    				 break;
	    			 }
	    		}
	    	}
//        cleanYear(ticketResults);
      if(type == 0)
         cleanDateField(ticketResults, "DATE");
	 return;
}
void  postProcessingDateDD_CA(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults = NULL;
	 int i, iA, line;
	 int yT, yB;
	 int type;

	 iA = -1;
	 line = -1;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	    			line = postProcessingDateA_CA(lpFieldsResults, &yT, &yB, &type);
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
      if(type == 0)
         cleanDateField(ticketResults, "DATE");
	 return;
}
void  postProcessingDate_CA(LPTICKETRESULTS ticketResults){

	postProcessingDateDD_CA(ticketResults);
	/*
	switch(ticketTypeInt){
	   case 2: // MEGA MILLIONS CA
	   case 4: // Daily 4 CA
	   case 7: // Daily Derby CA
		   postProcessingDateDD_CA(ticketResults);
	   break;
	   default:
		   postProcessingDateOther_CA(ticketResults);
		   break;
	}
	*/
}

void testLeftSideForLineCA(LPFIELDSRESULTS lpFieldsResults, int *iStart, int iStop){
	int isT = *iStart;
	int cc, i, dd;
	char *resultsStr = lpFieldsResults->result;
	if(iStop - isT < 6) return;

	cc = 0;
	for(i=isT;i<isT+5;i++){
		if(resultsStr[i] == 32) {cc = i;break;}
	}
//printf ("---------cc= %d, isT= %d\n",cc,isT);

	if(cc > isT ){
		dd = lpFieldsResults->flQuad[cc+1].p1X-lpFieldsResults->flQuad[cc-1].p2X;
//		printf ("---------dd= %d\n",dd);
		if(dd > 120) {*iStart = cc+1;return;}

	}

	if(resultsStr[isT] == '1' && resultsStr[isT+1] == 32 && resultsStr[isT+2] != 32 && resultsStr[isT+3] == 32) {
		*iStart = isT+4;
	}
}
int testConfidenceCA(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	int i, cc = 0;
	char *resultsStr = lpFieldsResults->result;
	for(i=iStart;i<iStop;i++){
		if(resultsStr[i] != 32)
			if(lpFieldsResults->flConf[i] > 20) cc++;
	}
	return cc;
}

int testBeginCutCA(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	   int cc;
	   cc = 0;
	   if(lpFieldsResults->result[iStart] > 32) cc++;
	   if(lpFieldsResults->result[iStart+1] > 32) cc++;
	   if(lpFieldsResults->result[iStart+2] > 32) cc--;
//  printf("--- lpFieldsResults->result[iStart]= %c, lpFieldsResults->result[iStart+1]= %c, cc= %d\n",lpFieldsResults->result[iStart], lpFieldsResults->result[iStart+1], cc);
	   if(cc > 1){
		   cc = 0;
		   if(lpFieldsResults->flConf[iStart] > 20) cc++;
           if(lpFieldsResults->flConf[iStart+1] > 20) cc++;
//printf("--- lpFieldsResults->flConf[iStart]= %d, lpFieldsResults->flConf[iStart+1]= %d, cc= %d\n",lpFieldsResults->flConf[iStart], lpFieldsResults->flConf[iStart+1], cc);
         if(cc > 0)
	        return 1;
	   }
    return 0;

}
int getBiggerGroupsPenality(char* results, int iStart, int iStop, int count){
	   int i, cc;
	   int bigGr = 0;
	   cc = 0;
	   for(i=iStart;i<=iStop;i++){
	      if(results[i] < 32) break;
		   if(results[i] != 32) cc++;
		   else {if(cc > count) bigGr += (cc-count);cc = 0;}
 	   }
 	   if(cc > count) bigGr += (cc-count);
	   return bigGr;
}

int getSmallerGroupsPenality(char* results, int iStart, int iStop, int count){
	   int i, cc, k;
	   int smallGr = 0;
	   cc = 0;
	   k = iStart;
	   for(i=iStart;i<iStop;i++){
		   k = i;
		   if(results[i] == 32) break;
	   }
       if(k == iStop-1) return smallGr;
	   for(i=k;i<=iStop;i++){
	    if(results[i] < 32) break;
		   if(results[i] != 32) cc++;
		   else {if(cc > 0 && cc < count) smallGr++;cc = 0;}
	   }
	   if(cc > 0 && cc < count) smallGr++;
	   return smallGr;
}

void cleanTrashAtRight(){
 int gr = 0;
 int i, k, k1 = 0, k2;
 int cc, ccGr, len;
 len = strlen(result);

   if(ticketTypeInt == 5) {
	  gr = 5;
	  k1 = 0;k2 = -1;
	  if(result[0] > 32 && result[1] == 32) k1 = 2;
	  k = len-1; while(result[k]< 32 && k>0) k--;
	  if(result[k] > 32 && result[k-1] == 32) k2 = k-1;

//	  printf(" result[k]= %d, result[k-1]= %d, k= %d\n", result[k], result[k-1], k);
//	  printf(" k1= %d, k2= %d, len= %d\n", k1, k2, len);

	  if(k1 == 0 || k2 < 0) return;
	  cc = 0;ccGr = 0;
      for(i=k1;i<k2;i++){
		  if(result[i] > 32) {
		       if(cc == 1) ccGr++;
		       cc++;
		  }
		  else
			  cc = 0;
      }
//      printf(" ccGr= %d\n", ccGr);
      if(ccGr >= 5)   result[k2] = 0;
  }


}


void cleanTicketNumbersCA(LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, i, k;
	 int iStart, iStop;
	 char strT[10];
//	 int ret;
	 int numLines = 0;

	 int thGroups = 4;
	 int numChars;
	 int beginCut = 0;
	 int numBGr = 0;
	 int numCnf = 0;
	 int numSGr = 0;
	 int lastLine = 0;

	 resultTmp[0] = 0;
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

	 k = numLines-1;
	 while(k>0){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
         testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
 	     numGr = getNumberOfGroups(resultsStr, iStart, iStop);
 	     beginCut = 0;
         numChars = getNumChars(resultsStr, iStart, iStop);
         if(numGr == thGroups){
         	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
             if(beginCut > 0)
             	numGr++;
         }
 	     if(numGr > thGroups && strlen(resultsStr) > 15 && numChars < 20 && numBGr < 3) break;
		 lpFieldsResults->lineQuadStatus[k] = 0;
		 numLines--;
		 k--;
	 }


 //printf("--- numLines= %d\n",numLines);

	 i = 0;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//		 printf("--- i= %d, iStart= %d\n",i,iStart);
//	 printLine(resultsStr, iStart, iStop);
        testLeftSideForLineCA(lpFieldsResults, &iStart, iStop);
	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
//	    numBGr = getBiggerGroups(resultsStr, iStart, iStop, 3);
	    numBGr = getBiggerGroupsPenality(resultsStr, iStart, iStop, 2);

	    numSGr = getSmallerGroupsPenality(resultsStr, iStart, iStop, 2);
//      printf("---------------- numBGr= %d\n",numBGr);

	    numCnf = testConfidenceCA(lpFieldsResults, iStart, iStop);

//	        printf("--- numGr= %d, numCnf= %d, numBGr= %d\n",numGr, numCnf, numBGr);

	    beginCut = 0;
// printf("--- line i= %d; numGr= %d, iStart= %d, iStop= %d\n",i, numGr, iStart, iStop);
        numChars = getNumChars(resultsStr, iStart, iStop);
//printf("--- line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);
        if(numGr == thGroups){
        	beginCut = testBeginCutCA(lpFieldsResults, iStart, iStop);
//printf("--- line i= %d; beginCut= %d\n",i, beginCut);
            if(beginCut > 0)
            	numGr++;
        }

// printf("--- line i= %d; numGr= %d, numChars= %d, beginCut= %d\n",i, numGr, numChars, beginCut);
//   printLine(resultsStr, iStart, iStop);
//   fflush(stdout);


//   fflush(stdout);
        if(k > 0) thGroups = 3;
//     printf("--- line i= %d; k= %d, numGr= %d, numChars= %d, len = %d, numBGr= %d, thGroups= %d\n",i, k, numGr, numChars, strlen(resultsStr), numBGr, thGroups);
        if(numBGr > 0 && numCnf < 5) numBGr += 10;
        if(numGr > 9 || numSGr > 2 ) numChars = 100;
        if(lastLine -i > 2 && numBGr > 1) numChars = 100;

	    if(numGr > thGroups && strlen(resultsStr) > 15 && numChars < 20 && numBGr < 3){
//  	     printf("--- line i= %d; k= %d, numGr= %d, numChars= %d, len = %d, numBGr= %d, thGroups= %d, numSGr= %d\n",i, k, numGr, numChars, strlen(resultsStr), numBGr, thGroups, numSGr);
	       copyLine(lpFieldsResults, iStart, iStop);
//    printf("--- inside line line i= %d; numGr= %d, numChars= %d\n",i, numGr, numChars);

    lastLine = i;

//    printf("--- result= %s\n",result);
//       printf("--- numGr= %d, numCnf= %d, numBGr= %d\n",numGr, numCnf, numBGr);

//  printLine(resultsStr, iStart, iStop);

//    printfQuad();
//  printf("--- line i= %d; %s\n",i, result);
//  printf("--- beginCut= %d\n",beginCut);

	       if(beginCut > 0){


//  printf("--- line i= %d; beginCut= %d\n",i, beginCut);

               strT[0] = 'A'+k;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
              getFirstCharFromNumLine(&iStart, &iStop);

//  printfQuad();
//   printf("--- getFirstCharFromNumLine i= %d; iStart= %d, iStop= %d\n",i, iStart, iStop);

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
//  printf("1--- line i= %d; %s\n",i, result);
  	         cleanTrashAtRight();

//	       cleanTrashInsideLine(iStart, iStop, &numGr);
//	       cleanTrashInsideLine2(iStart, iStop, &numGr);
// printf("2--- line i= %d; %s\n",i, result);
           iStop = -1;
//           ret = getFirstQPFromNumLine(&iStart, &iStop);

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
	    i++;
	 }
	 copyResultsBack(lpFieldsResults);
}
void replaceLastNums(char *resultsStr, int nSp, int iStop, char *resultsStrR, int nSpR, int iStopR){
	int i;
	resultsStr[nSp+1] = resultsStrR[nSpR+1];
	resultsStr[nSp+2] = resultsStrR[nSpR+2];
    for(i=nSp+3;i<=iStop;i++){
    	if(resultsStr[i] > 32)
    		resultsStr[i] = 32;
    }
}
void replaceInsideNumsNoCnf(char *resultsStr, int nSp, int iStop, char *resultsStrR, int nSpR, int iStopR){
	int i;
	resultsStr[nSp] = resultsStrR[nSpR];
	resultsStr[nSp+1] = resultsStrR[nSpR+1];
    for(i=nSp+2;i<=iStop;i++){
    	if(resultsStr[i] > 32)
    		resultsStr[i] = 32;
    	else
    		break;
    }
}
void replaceInsideNums(LPFIELDSRESULTS lpFieldsResults, int nSp, int iStop, LPFIELDSRESULTS lpFieldsResultsR, int nSpR, int iStopR){
	char *resultsStr = lpFieldsResults->result;
	char *resultsStrR = lpFieldsResultsR->result;
	int i;
	resultsStr[nSp] = resultsStrR[nSpR];lpFieldsResults->flConf[nSp] = lpFieldsResultsR->flConf[nSpR];
	resultsStr[nSp+1] = resultsStrR[nSpR+1];lpFieldsResults->flConf[nSp+1] = lpFieldsResultsR->flConf[nSpR+1];

    for(i=nSp+2;i<=iStop;i++){
    	if(resultsStr[i] > 32)
    		resultsStr[i] = 32;
    	else
    		break;
    }
}

int getCharInLineR(LPFIELDSRESULTS lpFieldsResultsR, int iStartR, int iStopR, int x1, int x2){
	int nCharR = 0, x1R, x2R;
	char *resultsStrR = lpFieldsResultsR->result;
	int minXd, nCharMin, dd;
	int n;
	x1R = 0; x2R = 0;
	minXd = 1000; nCharMin = -1;
    for(n=iStartR;n<iStopR;n++) {
		if(resultsStrR[n] > 32) {
	       x1R = lpFieldsResultsR->flQuad[n].p1X;
	       x2R = lpFieldsResultsR->flQuad[n].p2X;
//printf ("--cleanGroupInsideCA; n= %d, x1 -x1R= %d, x2 -x2R= %d\n", n, x1 -x1R, x2 -x2R);
           dd = min(abs(x1 -x1R),abs(x2 -x2R));
           if(minXd> dd) {minXd = dd;nCharMin = n;}
// 		   if(abs(x1 -x1R) < 15 || abs(x2 -x2R) < 15){
// 			   nCharR = n; break;
// 		   }
		}
	  }
    if(minXd < 42) nCharR = nCharMin;
//   printf ("-- nCharR= %d, iStartR= %d\n", nCharR, iStartR);
	return nCharR;
}

void replaceInsideSpace(LPFIELDSRESULTS lpFieldsResults, int nSp, int iStop){
	char *resultsStr = lpFieldsResults->result;
//	int i;
	resultsStr[nSp] = resultsStr[nSp+1];lpFieldsResults->flConf[nSp] = lpFieldsResults->flConf[nSp+1];
	resultsStr[nSp+1] = 32;
}


void cleanGroupInsideCA(LPFIELDSRESULTS lpFieldsResults, int line, int iStart, int iStop, int  nChar, int ccSp, LPTICKETRESULTS ticketResults){
	int i, k;
	char *resultsStr = lpFieldsResults->result;
    return;

    k = -1;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(ccSp>3){
    		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSR") == 0) {k = i;break;}
    		}
   		    else{
     		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSL") == 0) {k = i;break;}
   		    }
    	}
    }
    if(k < 0) return;
    {
   	 LPFIELDSRESULTS lpFieldsResultsR = &ticketResults->fieldsResults[k];
   	 char *resultsStrR = lpFieldsResultsR->result;
	 int start, stop, n, cc, ii;
	 int iStartR, iStopR, startR; //, stopR;
	 int yT, yB;
	 int yTR, yBR; //, conf;
	 int nCharR; //, nSpR, confR, nCharR;

		start = nChar-3;if(start < iStart) start = iStart;
		stop = nChar+3;if(stop >= iStop) stop = iStop-1;
		cc = 0;yT = 0;yB = 0;
		for(n=start;n<stop;n++) {
			if(resultsStr[n] > 32) {
		      yT += lpFieldsResults->flQuad[n].p1Y;
		      yB += lpFieldsResults->flQuad[n].p3Y;
		      cc++;
			}
		}
		if(cc>0){
			yT /= cc;
			yB /= cc;
		}
//printf ("--cleanGroupInsideCA; line= %d, yT= %d, yB= %d\n", line, yT, yB);
        if(yT>0 && yB>0) {
// find line in the R field
     	   ii = 0;
       	   while(1){
       		  if(getLine(resultsStrR, ii, &iStartR, &iStopR) < 0) break;
       		  startR = iStopR-5;if(startR < iStartR) startR = iStartR;
      		  cc = 0;yTR = 0;yBR = 0;
   			  for(n=startR;n<iStopR;n++) {
      			if(resultsStrR[n] > 32) {
      		       yTR += lpFieldsResultsR->flQuad[n].p1Y;
      		       yBR += lpFieldsResultsR->flQuad[n].p3Y;
      		       cc++;
      			}
      		  }
      		  if(cc>0){
      			yTR /= cc;
      			yBR /= cc;
      		  }
//	printf ("--cleanGroupInsideCA; ii= %d, yTR= %d, yBR= %d\n", ii, yTR, yBR);
//	printf ("--cleanGroupInsideCA; ii= %d, yTR -yT= %d, yBR -yB= %d\n", ii, yTR -yT, yBR -yB);
//    		  if(abs(yTR -yT) < 20 && abs(yBR -yB) < 20){
       		  if(abs(yTR -yT) < 20 || abs(yBR -yB) < 20){
    			  n = nChar;
    			  if(resultsStr[n] == 32) n--;
    			  else{
    				  while(n>iStart && resultsStr[n] != 32) n--;
    				  n++;
    			  }
// printf ("--n= %d, resultsStr[n]= %c, iStart= %d\n", n, resultsStr[n], iStart);
    			  if(resultsStr[n] <= 32 || n <= iStart) return;
    			  nCharR = getCharInLineR(lpFieldsResultsR, iStartR, iStopR, lpFieldsResults->flQuad[n].p1X, lpFieldsResults->flQuad[n].p2X);
    			  if(nCharR < iStartR|| nCharR >= iStopR) return;
//printf ("--n= %d, resultsStr[n]= %c, nCharR= %d, resultsStrR[nCharR]= %c\n", n, resultsStr[n], nCharR, resultsStrR[nCharR]);
//    			  replaceInsideNums(resultsStr, n, iStop, resultsStrR, nCharR, iStopR);
    			  replaceInsideNums(lpFieldsResults, n, iStop, lpFieldsResultsR, nCharR, iStopR);
//printf ("--resultsStr[n]= %c \n",  resultsStr[n]);
//printf ("--resultsStr= %s \n",  resultsStr);
break;
    		  }
    		  ii++;
       	   }
        } //if(yT>0 && yB>0) {

    }

}
void addSpacesAtTheEndCA(char *resultsStr, int iStop, int n){
	int i;
    for(i=n;i<=iStop;i++){
    	if(resultsStr[i] > 32)
    		resultsStr[i] = 32;
    }
}

void testNumbers_InsideLineCA(LPTICKETRESULTS ticketResults, LPFIELDSRESULTS lpFieldsResults){
	int i;
	char *resultsStr = lpFieldsResults->result;
	int iStart, iStop, n;
    int nSp, ccSp, nccSp;

//	 resultTmp[0] = 0;
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		nSp = 0;ccSp = 0;nccSp = 0;
		for(n=iStart;n<=iStop;n++) {
			if(resultsStr[n] < 32) break;
			if(resultsStr[n] > 32) {
				nSp++;
			}else {
				if(nSp==1 && ccSp>0){
					if((n+2<=iStop && resultsStr[n+2]<=32) || (n+2>iStop) ) {
					    replaceInsideSpace(lpFieldsResults, n, iStop);
					}
				}
				nSp = 0;
				ccSp++;
			}

		}
		i++;
    }


	 if(trapezoidVer == 1)  return;

	 trimSpaces(lpFieldsResults);
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		nSp = 0;ccSp = 0;nccSp = 0;
		for(n=iStart;n<=iStop;n++) {
			if(resultsStr[n] < 32) break;
//printf ("--i= %d, n= %d resultsStr[n]= %c, ccSp= %d, nSp= %d\n", i, n, resultsStr[n], ccSp, nSp);
			if(resultsStr[n] > 32) {
				if(ccSp >=7) {
//printf ("--addSpacesAtTheEndCA; i= %d, n= %d resultsStr[n]= %c, ccSp= %d, nSp= %d\n", i, n, resultsStr[n], ccSp, nSp);
					addSpacesAtTheEndCA(resultsStr, iStop, n);
					break;
				}
				nccSp = 0;
				nSp++;
				if(nSp>2) cleanGroupInsideCA(lpFieldsResults, i, iStart, iStop, n, ccSp, ticketResults);
			}else {
				nSp = 0;
				nccSp++;
				if(nccSp == 1)
				   ccSp++;
			}

		}
		i++;
    }

}

void testNumbers_LastPartCA(LPTICKETRESULTS ticketResults, LPFIELDSRESULTS lpFieldsResults){
	int i, k;
	char *resultsStr = lpFieldsResults->result;
    k = -1;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSR") == 0) {k = i;break;}
    	}
    }
    if(k < 0) return;
    {
	 int iStart, iStop, start, n, cc, ii, len;
	 int iStartR, iStopR, startR;
	 int yT, yB;
	 int yTR, yBR, conf;
	 int nSp, nSpR, confR;
	 LPFIELDSRESULTS lpFieldsResultsR = &ticketResults->fieldsResults[k];
	 char *resultsStrR = lpFieldsResultsR->result;
     len = strlen(resultsStr);
	 resultTmp[0] = 0;
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		start = iStop-5;if(start < iStart) start = iStart;
		cc = 0;yT = 0;yB = 0;
		for(n=start;n<iStop;n++) {
			if(resultsStr[n] > 32) {
		      yT += lpFieldsResults->flQuad[n].p1Y;
		      yB += lpFieldsResults->flQuad[n].p3Y;
		      cc++;
			}
		}
		if(cc>0){
			yT /= cc;
			yB /= cc;
		}
        if(yT>0 && yB>0) {
// 		printf ("--------------------------i= %d,\n", i);
//        printLine(resultsStr, iStart, iStop);
// find line in the R field
       	   ii = 0;
       	   while(1){
       		  if(getLine(resultsStrR, ii, &iStartR, &iStopR) < 0) break;
       		  startR = iStopR-5;if(startR < iStartR) startR = iStartR;
      		  cc = 0;yTR = 0;yBR = 0;
   			  for(n=startR;n<iStopR;n++) {
      			if(resultsStrR[n] > 32) {
      		       yTR += lpFieldsResultsR->flQuad[n].p1Y;
      		       yBR += lpFieldsResultsR->flQuad[n].p3Y;
      		       cc++;
      			}
      		  }
      		  if(cc>0){
      			yTR /= cc;
      			yBR /= cc;
      		  }
//printf ("--yTR= %d, yTR -yT= %d, yBR= %d, yBR -yB= %d\n", yTR,  yTR -yT, yBR, yBR -yB);
       		  if(abs(yTR -yT) < 10 && abs(yBR -yB) < 10){
//test last chars
//       			printLine(resultsStr, iStart, iStop);
//       			printLine(resultsStrR, iStartR, iStopR);
       			  nSp = 0; nSpR = 0; cc = 0;
       			  for(n=iStop;n>0;n--) {
       				if(resultsStr[n] >='0' && resultsStr[n] <='9')
       					cc++;
       				if(resultsStr[n] == 32 && cc == 2) {
       					nSp = n;
       					break;
       				}
       			  }
       			  cc = 0;
       			  for(n=iStopR;n>0;n--) {
       				  if(resultsStrR[n] >='0' && resultsStrR[n] <='9')
         				 cc++;
       				 if(resultsStrR[n] == 32 && cc == 2) {nSpR = n;break;}
       			  }

       			  if(nSp>1 && nSpR>1){

//  				   if(iStop-nSp > 3 && iStopR-nSpR == 3) {
//		printf ("--iStop-nSp= %d, iStopR-nSpR= %d\n", iStop-nSp, iStopR-nSpR);
//printf ("--resultsStr[nSp]= %d, resultsStr[nSp+1]= %d, resultsStr[nSp+2]= %d, resultsStr[nSp+3]= %d,\n", resultsStr[nSp], resultsStr[nSp+1], resultsStr[nSp+2], resultsStr[nSp+3]);
//printf ("--resultsStr[nSp]= %c, resultsStr[nSp+1]= %c, resultsStr[nSp+2]= %c, resultsStr[nSp+3]= %c,\n", resultsStr[nSp], resultsStr[nSp+1], resultsStr[nSp+2], resultsStr[nSp+3]);
//  					   replaceLastNums(resultsStr, nSp, iStop, resultsStrR, nSpR, iStopR);
//  				   }

 				   if(iStop-nSp == 3 && iStopR-nSpR == 3){
 					  conf = (lpFieldsResults->flConf[nSp+1]+lpFieldsResults->flConf[nSp+2])/2;
 					  confR = (lpFieldsResultsR->flConf[nSpR+1]+lpFieldsResultsR->flConf[nSpR+2])/2;
 					  if(confR > conf+10){
//	printf ("--confR= %d, conf= %d\n", confR, conf);
 	  					   replaceLastNums(resultsStr, nSp, iStop, resultsStrR, nSpR, iStopR);
 					  }
 				   }
       		     }
       		  }
       		  ii++;
       	   }
        }
		i++;
	 }
    }
}
int getLineConfidence(char *resultsStr,  int iStart, int iStop){
	int cnf = 0;
	int pen = 0;
	int  n;
    int nSp, ccSp;

//     printLine(resultsStr, iStart, iStop);
     nSp=0; ccSp = 0;
	for(n=iStart;n<=iStop;n++) {
//printf ("-- n= %d, resultsStr[n]= %d, resultsStr[n]= %c\n", n, resultsStr[n], resultsStr[n]);
		if(resultsStr[n] < 32) break;

		if(resultsStr[n] > 32) {
			if(ccSp >=7) {
				pen++;
				break;
			}
			nSp++;
			if(nSp>2)	pen++;
		}else {
			if(nSp==1 && ccSp>0)
				pen++;
			if(nSp==2 && ccSp>0)
				cnf++;
			nSp = 0;
			ccSp++;
		}
//	printf ("-- n= %d, cnf= %d, pen= %d\n", n, cnf, pen);
    }
	return cnf-pen;
}
int getLineFrom_L_R(LPTICKETRESULTS ticketResults,  LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int leftRight ){
     int lineOut = -1;
     int i, k;
 	 char *resultsStr = lpFieldsResults->result;

     k = -1;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
     	if(ticketResults->fieldsResults[i].name[0] != 0){
     		if(leftRight == 0){
     		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSL") == 0) {k = i;break;}
     		}
    		    else{
      		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSR") == 0) {k = i;break;}
    		    }
     	}
     }
     if(k < 0) return lineOut;
     {
   	 LPFIELDSRESULTS lpFieldsResultsR = &ticketResults->fieldsResults[k];
   	 char *resultsStrR = lpFieldsResultsR->result;

 	 int start, stop, n, cc, ii;
 	 int iStartR, iStopR, startR, stopR;
 	 int yT, yB;
 	 int yTR, yBR;

 	    startR = 0;
		if(leftRight == 0){
 		     start = iStart+2;
 		     stop = start+4;if(stop >= iStop) stop = iStop-1;
		}
		else{
			 stop = iStop-1;
		     start = stop-4;if(start < iStart) stop = iStart;
		}
	     cc = 0;yT = 0;yB = 0;
 		for(n=start;n<stop;n++) {
 			if(resultsStr[n] > 32) {
 		      yT += lpFieldsResults->flQuad[n].p1Y;
 		      yB += lpFieldsResults->flQuad[n].p3Y;
 		      cc++;
 			}
 		}
 		if(cc>0){
 			yT /= cc;
 			yB /= cc;
 		}
 //printf ("--cleanGroupInsideCA; line= %d, yT= %d, yB= %d\n", line, yT, yB);
         if(yT>0 && yB>0) {
 // find line in the R field
      	   ii = 0;
        	   while(1){
        		  if(getLine(resultsStrR, ii, &iStartR, &iStopR) < 0) break;
        			if(leftRight == 0){
        	 		     startR = iStartR+2;
        	 		     stopR = startR+4;if(stopR >= iStopR) stopR = iStopR-1;
        			}
       		      cc = 0;yTR = 0;yBR = 0;
    			  for(n=startR;n<iStopR;n++) {
       			      if(resultsStrR[n] > 32) {
       		            yTR += lpFieldsResultsR->flQuad[n].p1Y;
       		            yBR += lpFieldsResultsR->flQuad[n].p3Y;
       		            cc++;
       			      }
       		      }
       		      if(cc>0){
       			    yTR /= cc;
       			    yBR /= cc;
       		      }
 //	printf ("--cleanGroupInsideCA; ii= %d, yTR= %d, yBR= %d\n", ii, yTR, yBR);
 //	printf ("--cleanGroupInsideCA; ii= %d, yTR -yT= %d, yBR -yB= %d\n", ii, yTR -yT, yBR -yB);
        		  if(abs(yTR -yT) < 20 || abs(yBR -yB) < 20){lineOut = ii;break;}
        		  ii++;
        	   }
         }
     }

    return lineOut;
}
void changeLineFrom_L_R(LPTICKETRESULTS ticketResults,  int lineL, int lineR){
    int i, k;
    int iStart, iStop;

    k = -1;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
    		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSL") == 0) {k = i;break;}
    	}
    }
    if(k < 0) return;
    {
     char ch = result[0];
  	 LPFIELDSRESULTS lpFieldsResultsL = &ticketResults->fieldsResults[k];
  	 char *resultsStrL = lpFieldsResultsL->result;
	 if(getLine(resultsStrL, lineL, &iStart, &iStop) < 0) return;
	 copyLine(lpFieldsResultsL, iStart, iStop);
	 result[0] = ch;
	 result[strlen(result)-1] = ' ';
     k = -1;
	 for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		   if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERSR") == 0) {k = i;break;}
	    	}
	    }
	    if(k < 0) return;
	    {
     	 LPFIELDSRESULTS lpFieldsResultsR = &ticketResults->fieldsResults[k];
	     char *resultsStrR = lpFieldsResultsR->result;
	   	 if(getLine(resultsStrR, lineR, &iStart, &iStop) < 0) return;
	   	 addLineToResult(lpFieldsResultsR, iStart, iStop);
	    }
    }
  printf ("-------------------- result= %s\n", result);
}

void replaceOneLineCA(LPTICKETRESULTS ticketResults,  LPFIELDSRESULTS lpFieldsResults, int line, int lineL, int lineR){
	int i;
	char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 i = 0;
	 resultTmp[0] = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	       copyLine(lpFieldsResults, iStart, iStop);
	       if(i == line)
	    	   changeLineFrom_L_R(ticketResults, lineL, lineR);
	       addLineToTmp();
	       i++;
	 }
	 copyResultsBack(lpFieldsResults);
}
void testNumbersLinesAfterPostProc(LPTICKETRESULTS ticketResults,  LPFIELDSRESULTS lpFieldsResults){
	int i;
	char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop, cnf = 0;
	 int lineL, lineR;
//     len = strlen(resultsStr);
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//printf ("-------------------- i= %d, cnf= %d\n", i, cnf);
		cnf = getLineConfidence(resultsStr,  iStart, iStop);
		if(cnf < 5) {
			lineL = getLineFrom_L_R(ticketResults, lpFieldsResults, iStart, iStop, 0);
			lineR = getLineFrom_L_R(ticketResults, lpFieldsResults, iStart, iStop, 0);
printf ("-------------------- i= %d, cnf= %d, lineL= %d, lineR= %d\n", i, cnf, lineL, lineR);
fflush(stdout);
			if(lineL >= 0 && lineR >= 0){
                replaceOneLineCA(ticketResults, lpFieldsResults, i, lineL, lineR);
			}
		}

		i++;
	 }

}

void trimSpaces(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop, i, n, len, cc;
	 resultTmp[0] = 0;
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	       copyLine(lpFieldsResults, iStart, iStop);
	       len = strlen(result);
// left side
	       for(n=0;n<len;n++){
	    	   if(result[n] == 32) {shrinkResults(n, n);n--;len = strlen(result);}
	    	   else break;
	       }
	       cc = 0;
	       for(n=0;n<len;n++){
	    	   if(result[n] == 32) {
	    		   cc++;
	    		   if(cc>1) {shrinkResults(n, n);n--;len = strlen(result);}
	    	   }
	    	   else
	    		   cc = 0;
	      }
	      len = strlen(result);n = len-2;
//	      printf ("--result[len-1]= %d\n", result[n]);
	      if(result[n] == 32) {
	    	  shrinkResults(n, n);
	      }
	      addLineToTmp();
	      i++;
	 }
	 copyResultsBack(lpFieldsResults);
}

//    typeOfLotteryCAT[0] = "SUPER LOTTO CA"; //8
//    typeOfLotteryCAT[1] = "POWER BALL CA"; //12
//    typeOfLotteryCAT[2] = "MEGA MILLIONS CA"; //15
int testPrevPostProc(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 if(resultsStr[0] < '0' || resultsStr[0] > '9')
		 return 1;
	 return 0;
}
void matchDailyTerms(void){
	int cc, ccMax, idxMax;
	int i, k, kk;
	cc = 0;ccMax = 0;idxMax = -1;
	k = -1;
//printf ("---result= %s\n",result);
	for(i=0;i<len_Daily3_Names;i++) {
		cc =matchToStringResult(Daily3_Names[i], &kk);
	   if(ccMax < cc)
	      if(ccMax < cc){ccMax = cc;idxMax = i;k = kk;}
//   	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d\n",Pick3_Names[i], cc, ccMax, kk);
	}
//	 printf ("ccMax= %d, k= %d, idxMax= %d\n", ccMax, k, idxMax);
	if(ccMax > 0 && k >= 0) {
	   if(k > 0) shrinkResults(0, k-1);
//  printf ("result= %s, Pick3_Names[idxMax]= %s\n",result, Pick3_Names[idxMax]);
	   { char strTmp[20];
	      strcpy(strTmp,  &Daily3_Names[idxMax][1]);
	      strTmp[strlen(strTmp)-1] = 0;
	      strcpy(result,  strTmp);
//	      strcpy(result,  Pick3_Names[idxMax]);
	   }
	}
// printf ("result= %s\n",result);
}
int testLine_Daily(char *resultN, int nn){
//	int mode = 0;
// printf ("------result= %s\n",result);
//	fflush(stdout);
	int len = strlen(result);
	int i = 0, ok = 1;
	int k, cc, ccMax;
//	int k, n, cc, ccMax;
	int kk, idxLineA;

//	matchToStringEndOneLineIdx(char *results, char *str, int sh){
	ccMax = 0;
	idxLineA = -1;
	if(len < 5) return 0;

	for(i=0;i<len_Daily3_Names;i++) {
		cc =matchToStringResult(Daily3_Names[i], &kk);

//	   if(ccMax < cc)
	      if(ccMax < cc && cc == strlen(Daily3_Names[i])){ccMax = cc;k = kk;idxLineA = i;}
	      else {
		      if(ccMax < cc && cc >= strlen(Daily3_Names[i])-1 && kk > nn && kk < nn+3){ccMax = cc;k = kk;idxLineA = i;}
	      }

//     	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d, idxLineA= %d\n",Daily3_Names[i], cc, ccMax, kk, idxLineA);
	}

//	printf ("---idxLineA= %d\n", idxLineA);

   if(idxLineA >= 0) return ok;
    return -1;
//   if(resultN[nn] > 32) return 0;
/*
   i = 0;
// first word
	while(resultN[i] <= 32 && i<len) i++;
	cc = i;
	while(resultN[i] > 32 && i<len) i++;

	cc = i-cc;
	if(cc < 2) mode = 1;

// skipp next word
//	while(result[i] == 32 && i<len) i++;
//	while(result[i] > 32 && i<len) i++;
	k = i;ccMax = 0;n=1;
	while( k<len) {
		while(resultN[k] <= 32 && k<len) k++;
		cc = k;
		while(resultN[k] > 32 && k<len) k++;
		cc = k-cc;
		n++;
		if(cc > 6 && n == 2 && mode == 0) {ok = 0; break;}
		if(cc > ccMax && n>2) ccMax = cc;
		if(ccMax > 4 && mode == 0) {ok = 0; break;}
		if(cc > 6 && mode == 1) {ok = 0; break;}
	}
*/
//	printf ("------cc1= %d \n",cc1);
//	while(result[i] != 32 && i<len) {
//		printf ("------i= %d, resultr[i]= %c, flConf[i]= %d\n",i, result[i], flConf[i]);
//		i++;
//	}
	return ok;
}
int testLine_DailyNew(int nn, int *idxTab){
// printf ("------result= %s\n",result);
//	fflush(stdout);
	int i = 0;
	int k=0, cc, ccMax;
	int kk, idxLineA;

//	matchToStringEndOneLineIdx(char *results, char *str, int sh){
	ccMax = 0;
	idxLineA = -1;
	for(i=0;i<len_Daily3_Names;i++) {
		cc =matchToStringResult(Daily3_Names[i], &kk);

//	   if(ccMax < cc)
	      if(ccMax < cc && cc == strlen(Daily3_Names[i])){ccMax = cc;k = kk;idxLineA = i;}
	      else {
		      if(ccMax < cc && (cc >= strlen(Daily3_Names[i])-1 || cc > 5) && kk > nn && kk < nn+5){ccMax = cc;k = kk;idxLineA = i;}
	      }

//     	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d, idxLineA= %d, strlen(Daily3_Names[i])= %d\n",Daily3_Names[i], cc, ccMax, kk, idxLineA, strlen(Daily3_Names[i]));
	}
   *idxTab = idxLineA;
   if(idxLineA >= 0) return k;
   return -1;
//   if(result[nn] > 32) return -1;

/*
   i = 0;
// first word
	while(result[i] <= 32 && i<len) i++;
	cc = i;
	while(result[i] > 32 && i<len) i++;

	cc = i-cc;
	if(cc < 2) mode = 1;

// skipp next word
//	while(result[i] == 32 && i<len) i++;
//	while(result[i] > 32 && i<len) i++;
	k = i;ccMax = 0;n=1;
	while( k<len) {
		while(result[k] <= 32 && k<len) k++;
		cc = k;
		while(result[k] > 32 && k<len) k++;
		cc = k-cc;
		n++;
		if(cc > 6 && n == 2 && mode == 0) {ok = 0; break;}
		if(cc > ccMax && n>2) ccMax = cc;
		if(ccMax > 4 && mode == 0) {ok = 0; break;}
		if(cc > 6 && mode == 1) {ok = 0; break;}
	}
*/
//	printf ("------cc1= %d \n",cc1);
//	while(result[i] != 32 && i<len) {
//		printf ("------i= %d, resultr[i]= %c, flConf[i]= %d\n",i, result[i], flConf[i]);
//		i++;
//	}
//	return ok;
}

void getDollarAmountCA(char *rStr, int start, int stop, char *dollarAm){
int i, k, n;
int newI;
//for(i=start;i<stop;i++)
 //   printf("%c", rStr[i]);

// printf("\n");

i = start;
while(i < stop && rStr[i] == 32) i++;
while(i < stop && rStr[i] != 32) i++;
while(i < stop && rStr[i] == 32) i++;
if(stop - i > 12){
	 newI = searchForDollarPattern(rStr, i, stop);
//	 printf("-----i= %d, newI= %d\n", i, newI);
	 if(newI > i) i = newI;
}

k = 0;
dollarAm[k] = '$';k++;
while(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];k++; i++;}
while(i < stop && rStr[i] == 32) i++;
dollarAm[k] = '.';n = k;k++;

if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];} else {dollarAm[k] = '0';}
k++; i++;
// if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];} else {dollarAm[k] = '0';}
if(i < stop && rStr[i] != 32){dollarAm[k] = '0';} else {dollarAm[k] = '0';}
k++; i++;
dollarAm[k] = 0;


dollarAm[n+2] = '0';
if(dollarAm[n+1] != '0' && dollarAm[n+1] != '5') dollarAm[n+1] = '0';
dollarAm[n+3] = 0;

// printf("-----Dollar amount= %s\n", dollarAm);
}
void getDollarAmountCAnew(int iSt, int idx, char *dollarAm){
int i, k, n, n1, j, iStart = iSt+strlen(Daily3_Names[idx])-1;
int len, cc0;

//   printf("result[iStart]= %c, result[iStart+1]= %c\n", result[iStart], result[iStart+1]);

   len = strlen(result);
   i = iStart;
   if(result[iStart-1] > 32 && result[iStart] > 32 && result[iStart+1] > 32) iStart++;
   i = iStart;
//   while(i < len && result[i] > 32) i++;
   while(i < len && result[i] <= 32) i++;
   k = 0;
   while(i < len ){dollarAm[k] = result[i];k++; i++;}

   dollarAm[k] = 0;
//   printf("--------------dollarAm= %s, n= %d \n", dollarAm, n);

   cc0 = 0;n = -1;
   for(j=0;j<k;j++){
		 if(dollarAm[j] == '0') cc0++;
		 else cc0= 0;
		 if(cc0==2){n=j-1;break;}
   }
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

// printf("-----Dollar amount=%s\n", dollarAm);
}

void cleanTicketNumbers_Daily3_4CAOld(LPTICKETRESULTS ticketResults, int idx, int nn) {
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
//	 char strNum[10];
//	 int iStopTab[20];
//	 int iStop1Tab[20];
	 int iStart, iStop;
	 int iStart1, iStop1;
	 char strT[10];
	 int numLines = 0;
	 char dollarAm[10];
//	 char totalStr[10];
	 int yPrev, yCur, ddY;
	 int firstLineIdx;
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
//    printf ("----------numLines= %d\n",numLines);

     for(i=0;i<20;i++){
	     kTab[i] = -1;
	     kSumItUp[i] = 0;
	     shiftTab[i] = 0;
	     kComb[i] = 0;
	     kCombStart[i] = 0;
	     kCombIdx[i] = 0;
//	     iStopTab[i] = -1;
//	     iStop1Tab[i] = -1;
     }
//    printf ("----------numLines= %d\n",numLines);
// find first line
     firstLineIdx = 0;
     i = 0;
	    while(i < numLines){
		   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		   copyLine(lpFieldsResults, iStart, iStop-1);
//  printf ("1------result= %s\n",result);
		   strcpy(resultTmp, result);
		   if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
		   copyLine(lpFieldsResults1, iStart, iStop-1);
//  printf ("2------result= %s\n",result);

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
   //printf ("------firstLineIdx= %d, numLines= %d\n",firstLineIdx, numLines);

	 i = firstLineIdx;
	 k = 0;
	 kS = 0;
	 yPrev = 0;
	 yCur = 0; ddY = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//		 printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);
		   iStart1 = iStart;

           testLeftSideForLineTX(resultsStr, &iStart, iStop);
           shift = iStart-iStart1;
//    printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);

//    printLine(resultsStr, iStart1, iStop);
//    printLine(resultsStr1, iStart1, iStop);

           getFirstNumberPick3TX(lpFieldsResults, iStart, iStop, &iStart1, &iStop1, nn, kComb[i]);

// printf ("--------------------------------------------i= %d, iStart1= %d, iStop1= %d, kComb[i]= %d\n",i,iStart1, iStop1, kComb[i]);
   	    if((iStart1 < 0 || iStart1 >= iStop1) && kComb[i] > nn){
   	    	iStart1 = iStart+kComb[i]-nn;
 	    	if(resultsStr[iStart+kComb[i]-1] == 32)
   	    		iStart1--;
   	    	iStop1 = iStart1+nn;
   	    }

//  printf ("--------------------------------------------i=%d, iStart1= %d, iStop1= %d\n",i,iStart1, iStop1);
//  printLine(resultsStr, iStart1, iStop);
	    if( ((iStart1 > 0 && iStart1 < iStop1) || kSumItUp[i] > 0) && ddY < 180){
	    	yPrev = yCur;
// printf ("-------i= %d, k= %d, kS= %d\n",i, k, kS);
//   	 printf ("-------i= %d, kSumItUp[i]= %d\n",i, kSumItUp[i]);

	           copyLine(lpFieldsResults, iStart1, iStop1-1);
	           total = 0;
//	           if(nn == 4) testForXX(result, lpFieldsResults1,kCombStart[i], nn);

//  printf ("------result= %s\n",result);
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
	      	    	iStop1 = iStart1+strlen(Daily3_Names[kCombIdx[i]]);
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


int getFirstNumberDaily_3_4_CA(int* iStart1, int nn, int iBox){
//	char *resultsStr = lpFieldsResults->result;
	int  len, n, i1, i2, dd, flg;
	int ret = -1;

//printf ("---------------------------------------iBox= %d, results= %s, \n",iBox, result);

//	*iStart1 = iStart+2; *iStop1 = iStop;
	len = strlen(result);
//	printf ("---------------------------------------len= %d \n",len);
	if(len< nn+4) return ret;
	n = iBox;

//printf ("-----------------------------n= %d, results[n]=%c,results[n-1]=%c \n",n, result[n], result[n-1]);

//    while(result[n] > 32 && n> 0) n--;
	if(result[n] > 32) n--;
//printf ("1---------------------------------------n= %d \n",n);
    while(result[n] <= 32 && n> 0) n--;
    i2 = n;
//   printf ("---------------------------------------i2= %d \n",i2);
    while(result[n] > 32 && n>= 0) n--;
    i1 = n;
    dd = i2-i1;
//  printf ("---------------------------------------dd= %d, nn= %d \n",dd, nn);
    flg = 1;
    if(dd < nn){
        while(result[n] <= 32 && n> 0) n--;
        while(result[n] > 32 && n> 0) n--;
        i1 = n;
        dd = i2-i1;
        if(n>0)
           dd--;
        else
          flg = 0;
//       printf ("1---------------------------------------dd= %d, nn= %d, i1= %d \n",dd, nn, i1);
    }else{
// 	printf ("2---------------------------------------dd= %d, n= %d \n",dd,n);

    	if(dd > nn && n < 4){
//	printf ("---------------------------------------result[i1+1]= %c, flConf[i1+1]= %d, result[i2]= %c, flConf[i2]= %d \n",result[i1+1], flConf[i1+1],result[i2],flConf[i2]);
	        if(flConf[i2] > flConf[i1+1])
    		   i1 = i2-nn;
    		dd = nn;
    	}
    }
//   printf ("---------------------------------------i1= %d, dd= %d, nn= %d \n",i1, dd, nn);
    if(dd != nn) return ret;
    if(flg == 1)
	  *iStart1 = i1+1;
    else
      *iStart1 = i1;
    return 1;
}
int getNumSpaces(int k){
	int numSpc = 0, i;
	for(i=0;i<k;i++){
		if(result[i] == 32) numSpc++;
	}
	return numSpc;

}
void cleanTicketNumbers_Daily3_4CA(LPTICKETRESULTS ticketResults, int idx, int nn) {
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
//	 char totalStr[10];
//	 int yPrev, yCur, ddY;
	 int firstLineIdx, lastLineIdx, strIdx;
//	 idx1 = idx;
//     for(i=0;i<NUMFIELDS_RESULTS;i++){
//	    	if(ticketResults->fieldsResults[i].name[0] != 0){
//	    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS1") == 0) idx1 = i;
//	    	}
//    }
//    lpFieldsResults1 = &ticketResults->fieldsResults[idx1];
//	 resultsStr1 = lpFieldsResults1->result;

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

//     for(i=0;i<20;i++){
//	     kTab[i] = -1;
//	     kSumItUp[i] = 0;
//	     shiftTab[i] = 0;
//	     kComb[i] = 0;
//	     kCombStart[i] = 0;
//	     kCombIdx[i] = 0;
//     }
//    printf ("----------numLines= %d\n",numLines);
// find first line
     firstLineIdx = 0;
     i = 0;
	    while(i < numLines){
		   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		   copyLine(lpFieldsResults, iStart, iStop-1);
//  printf ("1------result= %s\n",result);
		   if(testLine_DailyNew(nn, &strIdx) > 0) {firstLineIdx = i;break;}
		   i++;
//		    printf ("------result= %s\n",resultTmp);
	    }
	    if(i>0){
          for(k=0;k<i;k++)
	        lpFieldsResults->lineQuadStatus[k] = 0;
	    }
	    resultTmp[0] = 0;
	    result[0] = 0;
//   printf ("------firstLineIdx= %d, numLines= %d\n",firstLineIdx, numLines);

//   fflush(stdout);
   lastLineIdx = numLines-1;
   while(lastLineIdx > firstLineIdx){
	   if(getLine(resultsStr, lastLineIdx, &iStart, &iStop) < 0) break;
	   copyLine(lpFieldsResults, iStart, iStop-1);
//  printf ("1------result= %s\n",result);
	   if(testLine_DailyNew(nn, &strIdx) > 0) break;
	   lastLineIdx--;
//		    printf ("------result= %s\n",resultTmp);
   }
//   printf ("------lastLineIdx= %d, numLines= %d\n",lastLineIdx, numLines);

   if(lastLineIdx<numLines-1){
     for(k=lastLineIdx+1;k<numLines;k++)
       lpFieldsResults->lineQuadStatus[k] = 0;
   }


	 i = firstLineIdx;
	 k = 0;
	 kS = 0;
//	 yPrev = 0;
//	 yCur = 0; ddY = 0;
	 while(i <= lastLineIdx){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//	 printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);

//   printLine(resultsStr, iStart, iStop);

//           testLeftSideForLineTX(resultsStr, &iStart, iStop);
//           shift = iStart-iStart1;
         copyLine(lpFieldsResults, iStart, iStop-1);
	     iStart1 = -1;
		 iStop1 = 0;
		 strIdx = -1;
		 k = testLine_DailyNew(nn, &strIdx);
// printf ("--------------------------------------------i=%d, k= %d, nn= %d\n",i,k, nn);
         ret = 0;
         if(k > 0)
           ret = getFirstNumberDaily_3_4_CA( &iStart1, nn, k);
//    printf ("--------------------------------------------iStart= %d, iStop= %d\n",iStart, iStop);


//  printf ("--------------------------------------------i=%d, iStart1= %d, iStop= %d ret= %d\n",i,iStart1, iStop, ret);
//  printLine(resultsStr, iStart1, iStop);
//  fflush(stdout);

	    if( iStart1 >= 0 && ret >= 0 && strIdx >=0 ){
        k -= iStart1;
// printf ("-------i= %d, k= %d, kS= %d\n",i, k, kS);
//   	 printf ("-------i= %d, kSumItUp[i]= %d\n",i, kSumItUp[i]);

	           copyLine(lpFieldsResults, iStart+iStart1, iStop-1);

//	           if(nn == 4) testForXX(result, lpFieldsResults1,kCombStart[i], nn);

//  printf ("------result= %s\n",result);
// printf ("------result= %s\n",&lpFieldsResults->result[iStart1]);
//               getDollarAmountCA(resultsStr, iStop1, iStop, dollarAm);
                 getDollarAmountCAnew(k, strIdx, dollarAm);
//       printf ("------result= %s\n",result);
                 spaceNum = getNumSpaces(k);
//            printf ("------spaceNum= %d, nn= %d,k= %d\n",spaceNum, nn, k);
                 if(k > nn+spaceNum)
                	result[nn+spaceNum] = 0;
                 else
                    result[k] = 0;
//   printf ("0------result= %s, k= %d\n",result, k);
                 strcat(result, Daily3_Names[strIdx]);
                 strcat(result, dollarAm);


//	   printf ("1------result= %s, k= %d\n",result, k);

//	           {int jj = strlen(result);
//				strcat(result, " ");
//				strcat(result, dollarAm);
//				   for(jj=0;jj<strlen(dollarAm)+1;jj++){
//				      flConf[jj] = 100;
//				   }
//				}
//	 printf ("2------result= %s\n",result);

               strT[0] = 'A'+kS;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
               addEOL();
//	           addLineToTmpSpace();
// printf ("2------result= %s, k+2= %d\n",result, k+2);
	           addLineToTmpSpaceN(k+2);

//     printf ("------resultTmp= %s\n",resultTmp);
	           if(i+1 < 20) kS++;
       	       k++;
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
	    }
	    i++;
	    fflush(stdout);
	 }
	 copyResultsBack(lpFieldsResults);

// printLine(resultsStr, 0, strlen(resultsStr)-1);


//   mergeTwoResults(lpFieldsResults, lpFieldsResults1, nn);

}

int getNumSpots(char *res){
	int i, cc, ccD;
	int len = strlen(res);

	ccD = 0; cc = 0;
	for(i=0;i<len;i++){
		if(res[i] > 32 && ccD == 0) {cc++;ccD++;}
		else ccD = 0;
	}

	return cc;
}

void cleanTicketNumbers_HotSpotCA(LPTICKETRESULTS ticketResults, int idx) {
	 LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[idx];
	 char *resultsStr = lpFieldsResults->result;
	 LPFIELDSRESULTS lpFieldsResults1;
	 char *resultsStr1;
	 int  i, k, kT, n, nC, idx1;
	 int iStart, iStop;
	 int iStart1, iStop1;
	 int numLines = 0;
	 int y1, y2, cc;
	 int yN1, yN2;
	 int firstLineIdx, firstLineNIdx, lastLineIdx, score, scoreMax, lineMax;
	 char strT[10];

	 idx1 = idx;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS1") == 0) idx1 = i;
	    	}
    }
    lpFieldsResults1 = &ticketResults->fieldsResults[idx1];
	 resultsStr1 = lpFieldsResults1->result;

// get number of lines and spot line
	 scoreMax = 0; lineMax = -1;
	 i = 0;
	 while(1){
		if(getLine(resultsStr1, i, &iStart1, &iStop1) < 0) break;
		score = matchToString(resultsStr1, iStart1, iStop1, " SPOTI");
// printf ("----------i= %d, score= %d\n",i, score);
		if(score > scoreMax) {scoreMax = score; lineMax = i;}
		numLines++;
		i++;
	 }
//    printf ("----------numLines= %d\n",numLines);
// find first line
     firstLineIdx = -1;
     if(scoreMax > 3)
    	 firstLineIdx = lineMax;

	  resultTmp[0] = 0;
	  result[0] = 0;

//	 printf ("----------firstLineIdx= %d, lineMax= %d, scoreMax= %d\n",firstLineIdx, lineMax, scoreMax);
      if(firstLineIdx< 0){
    	     for(i=0;i<numLines;i++){
	    	   lpFieldsResults->lineQuadStatus[i] = 0;
    	     }
    	     copyResultsBack(lpFieldsResults);
    	     return;
      }
//     get the same line for NUMBERS

//  printf ("----------firstLineIdx= %d\n",firstLineIdx);

      firstLineNIdx = -1;
      getLine(resultsStr1, firstLineIdx, &iStart1, &iStop1);
      cc = 0; y1 = 0; y2 = 0;
      for(i=iStart1;i<iStop1;i++){
	      if(resultsStr1[i] > 32){
    		y1 += lpFieldsResults1->flQuad[i].p1Y;
    		y2 += lpFieldsResults1->flQuad[i].p3Y;
    		cc++;
        }
     }
     if(cc>0){y1 /= cc; y2 /= cc;}
//   printf ("----------y1= %d, y2= %d\n",y1, y2);

     for(i=0;i<numLines;i++){
         getLine(resultsStr, i, &iStart, &iStop);
         cc = 0; yN1 = 0; yN2 = 0;
         for(k=iStart;k<iStop;k++){
   	      if(resultsStr[k] > 32){
       		yN1 += lpFieldsResults->flQuad[k].p1Y;
       		yN2 += lpFieldsResults->flQuad[k].p3Y;
       		cc++;
           }
        }
        if(cc>0){yN1 /= cc; yN2 /= cc;}
//  printf ("----------i= %d, yN1= %d, yN2= %d\n",i,yN1, yN2);
//  printf ("----------i= %d, y1-yN1= %d, y2-yN2= %d\n",i,y1-yN1, y2-yN2);

    	if(abs(y1-yN1) < 20 || abs(y2-yN2)<20){
    	   firstLineNIdx = i;break;
    	 }
     }
     if(firstLineNIdx< 0){
   	     for(i=0;i<numLines;i++){
	    	   lpFieldsResults->lineQuadStatus[i] = 0;
   	     }
   	     copyResultsBack(lpFieldsResults);
   	     return;
     }
     for(i=0;i<firstLineNIdx;i++){
    	   lpFieldsResults->lineQuadStatus[i] = 0;
     }
     getLine(resultsStr1, firstLineIdx, &iStart1, &iStop1);
	 copyLine(lpFieldsResults1, iStart1, iStop1);
     addLineToTmp();
// printf ("1----------resultTmp= %s\n",resultTmp);

	 getLine(resultsStr, firstLineNIdx, &iStart, &iStop);
	 copyLine(lpFieldsResults, iStart, iStop);
//     addEOL();
//printf ("----------firstLineNIdx= %d, result= %s\n",firstLineNIdx, result);

	 resultTmp[0] = result[0]; kT = 2;
	 strT[0] = result[0];
//printf ("2----------resultTmp= %s\n",resultTmp);
	 strT[1] = 0;
	 if(result[1] > 32) {resultTmp[1] = result[1];strT[1] = result[1];strT[2] = 0;kT++;}
	 k = 8;
     for(i=4;i<strlen(result);i++){
    	 if(result[i] == 32) {k = i+1;break;}
     }


//printf ("3----------resultTmp= %s\n",resultTmp);
	 strcpy(&resultTmp[kT], "SPOT: ");
//	 resultTmp[kT+6] = ' ';
	 strcat(resultTmp, &result[k]);
//printf ("4----------resultTmp= %s\n",resultTmp);
     if(strT[1] != 0) {
        n =  strT[1] - '0'; n += (10*(strT[0] - '0'));
     }
     else
    	 n =  strT[0] - '0';

     nC = getNumSpots(&result[k]);
     lastLineIdx = firstLineIdx+1;
//     printf ("----------n= %d, nC= %d\n",n, nC);
     if(n > nC){  // more spots
    	 i = firstLineNIdx+1;
    	 while(i < numLines && n > nC) {
    	    getLine(resultsStr, i, &iStart, &iStop);
    	    copyLine(lpFieldsResults, iStart, iStop);
//    	    addEOL();
    	    addLineToTmp();
    	     nC += getNumSpots(result);
    	    i++;
    	    lastLineIdx++;
    	 }
     }
     if(getLine(resultsStr1, lastLineIdx, &iStart1, &iStop1)>=0){
//    	 printLine(resultsStr1, iStart1, iStop1);
//	 printf ("----------lastLineIdx= %d\n",lastLineIdx);

 		score = matchToString(resultsStr1, iStart1, iStop1, "BULLS EYE PLAYED");
//printf ("----------score= %d\n",score);


 		if(score > 5){
 			 strcpy(result, "BULLS EYE PLAYED");
 			 addEOL();
     	    addLineToTmp();
 		}
     }
	 copyLine(lpFieldsResults1, iStart1, iStop1);



     copyResultsBack(lpFieldsResults);

     for(i=firstLineNIdx+1;i<numLines;i++){
    	   lpFieldsResults->lineQuadStatus[i] = 0;
     }
}
//char *DailyDerby_Names[] = { "GOLD RUSH","LUCKY STAR","HOT SHOT","BIG BEN","CALIFORNIA CLASSIC","WHIRL WIN","EUREKA","GORGEOUS GEORGE","WINNING SPIRIT","SOLID GOLD","MONEY BAGS","LUCKY CHARMS"};
//int len_DailyDerby_Names = 12;

int matchToStringResult_DDerby(char *str, int *kM){
	   int i, k, cc, ccMax, kMax;
	   int lenR = strlen(result);
	   int len = strlen(str);

	   if(len > lenR) len = lenR;
	   ccMax = 0; kMax = -1;
	   for(k=0;k<lenR-len+2;k++){
		   cc = 0;
	      for(i=0;i<len;i++){
		     if(result[i+k] == str[i]) cc++;
	      }
	      if(ccMax < cc) {ccMax = cc;kMax = k;}
	   }
	   *kM = kMax;
       return ccMax;
}
int findHorseNameIDX(int idx){
	int idxOut = idx, i;
	if(result[idxOut] <=32) idxOut++;
	for(i=0;i<4;i++){
		idxOut++;
		if(result[idxOut] <=32) break;
	}
	return idxOut;
}
int getTheHorseNumberFromLine(int idx){
	int i, num = -1;
	char strT[4];

//printf ("-------------------- result= %s, idx= %d\n", result, idx);
	while(result[idx]<=32 && idx < strlen(result)) idx++;
//	while(result[idx]>='0' && result[idx]<='9' && idx > 2) idx--;

//printf ("-------------------- idx= %d\n", idx);
	for(i=0;i<3;i++){
		strT[i] = 0;
		if(result[idx+i]>='0' && result[idx+i]<='9') strT[i] = result[idx+i];
	}

//   printf ("-------------------- strT= %s, num= %d\n", strT, num);

   if(strlen(strT) > 0) {
	  if(strlen(strT) > 1) {
         num = 10 + strT[1]-'0';
	  }else{
		  num = strT[0]-'0';
	  }
	  num--;
   }
//printf ("-------------------- strT= %s, num= %d\n", strT, num);

	if(num >= len_DailyDerby_Names) num = len_DailyDerby_Names-1;
//	if(num < 0) num = 0;
	return num;
}
int matchHorseName(int idx){
	int cc, ccMax, idxMax, idxMaxN;
	int i, k, kk, th, th1;
	cc = 0;ccMax = 0;idxMax = -1;
	k = -1;
	th = 4;

//printf ("---result= %s\n",result);

	for(i=0;i<len_DailyDerby_Names;i++) {
		cc =matchToStringResult_DDerby(DailyDerby_Names[i], &kk);
	   if(ccMax < cc)
	      if(ccMax < cc){ccMax = cc;idxMax = i;k = kk;}
	}

//if(idxMax >=0)
//	 printf ("ccMax= %d, k= %d, idxMax= %d, DailyDerby_Names[idxMax]= %s\n", ccMax, k, idxMax, DailyDerby_Names[idxMax]);
	if(ccMax > 0 && k >= 0) {
		th1 = strlen(DailyDerby_Names[idxMax])/2;
		if(th1 < th) th = th1;
//  printf (" ccMax= %d, th= %d\n", ccMax, th);
		if(ccMax<=th) {
	          idxMaxN = getTheHorseNumberFromLine(idx);
//         printf (" idxMaxN= %d \n", idxMaxN);
	          if(idxMaxN >= 0) idxMax = idxMaxN;
	          else{
	             if(ccMax<3) idxMax = idxMaxN;
	          }
		}

//if(idxMax >=0)
//   printf (" idxMax= %d, DailyDerby_Names[idxMax]= %s, idx= %d\n", idxMax, DailyDerby_Names[idxMax], idx);
//	      shrinkResults(idx+1, strlen(result)-1);
	      shrinkResults(5, strlen(result)-1);
//printf ("---result= %s\n",result);
	      if(idxMax>=0) {
    	    char strTmp[20];
 	        sprintf(strTmp,"%d ",idxMax+1);
	        strcat(result,  strTmp);
	        strcat(result,  DailyDerby_Names[idxMax]);
	      }
	      else{
		    strcat(result,  "? ???");
	      }
/*
		}else{
printf ("2 ccMax= %d, k= %d, idxMax= %d\n", ccMax, k, idxMax);
          idxMax = getTheHorseNumberFromLine(idx);
	      if(k > 0) shrinkResults(k, strlen(result)-1);
	        strcat(result,  DailyDerby_Names[idxMax]);
		}
*/
//  printf ("result= %s, DailyDerby_Names[idxMax]= %s\n",result, DailyDerby_Names[idxMax]);
      return 0;

	}

// printf ("result= %s\n",result);
   return -1;
}
int getNextRaceTimeLine(LPFIELDSRESULTS lpFieldsResults, int startLine, int numLines){
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 int stop;
	 int i, cc, kk;
	 int raceTime = -1;

	   i = startLine;
	   stop = i+5;
	   if(stop>=numLines) stop = numLines;
	   while(i< numLines){
		   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			copyLine(lpFieldsResults, iStart, iStop);
//	printLine(resultsStr, iStart, iStop);
			cc = matchToStringResult("RACETIME: ", &kk);
//     printf("RACETIME--- i= %d, cc= %d, kk= %d\n",i, cc, kk);
			if(cc > 4 && kk < 3){
				raceTime = i;
				break;
			}
			i++;
	   }
//	   printf(" returnRACETIME-------------------------------- raceTime= %d\n",raceTime);
	   return raceTime;
}
int getNextPlayLine(LPFIELDSRESULTS lpFieldsResults, int startLine, int numLines, int currPlay){
 char *resultsStr = lpFieldsResults->result;
 int iStart, iStop;
 int i, cc, kk;
 int firstPlay = -1;
 char strToMatch[20];
 int len;

   strcpy(strToMatch,"PLAY ");
   len = strlen(strToMatch);
   strToMatch[len] = 'A'+currPlay;  strToMatch[len+1] = 0;
   strcat(strToMatch," H");

   i = startLine;
   while(i< numLines){
	   if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
//printLine(resultsStr, iStart, iStop);
		cc = matchToStringResult(strToMatch, &kk);
//printf("PLAY--- i= %d, cc= %d, kk= %d\n",i, cc, kk);
		if(cc > 6 && kk < 3){
			firstPlay = i;
			break;
		}
		if(cc > 4 && kk < 5){
			if(getLine(resultsStr, i+1, &iStart, &iStop) >= 0){
				copyLine(lpFieldsResults, iStart, iStop);
				cc =matchToStringResult("1ST: ", &kk);
//printf("1ST--- i= %d, cc= %d, kk= %d\n",i, cc, kk);
				if(cc > 3 && kk < 5) {
					firstPlay = i;
					break;
				}
			}
		}
		if(firstPlay < 0){
		   cc = matchToStringResult("1ST: ", &kk);
			if(cc > 3 && kk < 5) {
				if(getLine(resultsStr, i+1, &iStart, &iStop) >= 0){
					copyLine(lpFieldsResults, iStart, iStop);
					cc =matchToStringResult("2ND: ", &kk);
	//printf("1ST--- i= %d, cc= %d, kk= %d\n",i, cc, kk);
					if(cc > 3 && kk < 5) {
						firstPlay = i-1;
						if(firstPlay < 0) firstPlay = 0;
						break;
					}
				}
			}
		}
		if(firstPlay >= 0) break;
	   i++;
   }
   return firstPlay;
}
int findPlaceToreplaceDderby(int startKK, int stopKK, int dd){
	int retKK = startKK+dd;
	int i;

    for(i=startKK;i<stopKK-1;i++){
    	if(result[i] == 32 && result[i+1] >= '0' && result[i+1] <= '9')
    		return i;
    }
	return retKK;
}
void replaceSpacesDDerby(int start, int stop){
	int i, k;
	char strT[stop-start+1];
	k = 0;
    for(i=start;i<stop;i++){
    	if(result[i] > 32){
    		strT[k]=result[i];k++;
    	}
    }
    for(i=0;i<k;i++){
   		result[i+start] = strT[i];
    }
    result[k+start] = 0;
    addEOL();
}
int changeTo_0(char ch){
	if(ch == 'C' || ch == 'O' || ch == 'Q' || ch == 'D' || ch == 'H' || ch == 'E' || ch == 'U')
		return 1;
	return 0;
}
int changeTo_1(char ch){
	if(ch == 'I' || ch == 'L')
		return 1;
	return 0;
}
int changeTo_8(char ch){
	if(ch == 'B')
		return 1;
	return 0;
}

void replaceToDigitDDerby(int start, int stop){
	int i;
    for(i=start;i<stop;i++){
    	if(result[i] > '9'){
    		if(changeTo_0(result[i])>0)
    			result[i] = '0';
    		if(changeTo_1(result[i])>0)
    			result[i] = '1';
    		if(changeTo_8(result[i])>0)
    			result[i] = '8';

    	}
    }
}
void repearTimeDDerby(int start, int stop){
	int i, k;
//	int dd;
	k = -1;
	for(i=stop-1;i>start;i--){
		if(result[i] == '.'){k=i;break; }
	}
// printf("--- result= %s\n",result);
// printf("--- result[start]= %c, result[start]= %d\n",result[start], result[start]);
// dd = k-start;
// printf("--- k= %d, start= %d, k-start= %d\n",k, start, dd);
	if(k>0){
		if(k-start == 2) {replaceChars(start, start-1, "1:");}
		if(k-start == 3 && result[start+1] != ':') {replaceChars(start+1, start, ":");}
	}else{
		if(stop-start > 4)
			replaceChars(stop-3, stop-4, ".");
	}
}

void cleanTicketNumbers_DailyDerbyCA(LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int  i;
	 int iStart, iStop;
	 char strT[10];

	 int numLines = 0;
	 int currLine = 0;
	 int cc, kk, rKK;
	 int firstPlay = -1, currPlay = -1;
	 int LastPlay = -1, raceTimeLine;

	 resultTmp[0] = 0;
     result[0] = 0;


// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }

// printf("--- numLines= %d\n",numLines);
 fflush(stdout);

 firstPlay = -1;
// find first PLAY
 firstPlay = getNextPlayLine(lpFieldsResults, currLine, numLines, currPlay);

// printf("--- firstPlay= %d\n",firstPlay);

  if(firstPlay < 0) {
	 i = 0;
	 while(i<numLines){
		 if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		 lpFieldsResults->lineQuadStatus[i] = 0;
		 i++;
	 }
     copyResultsBack(lpFieldsResults);
     return;
  }
  i = 0;
  while(i<firstPlay){
	 lpFieldsResults->lineQuadStatus[i] = 0;
	 i++;
  }

// get All Plays
  currPlay = 0;
  currLine = firstPlay;
  while(1) {
     strcpy(result, "PLAY ");
     strT[0] = 'A'+currPlay;  strT[1] = 0;
     strcat(result, strT);
     strcat(result, " HORSES SELECTED");
	 addEOL();
	 addLineToTmp();

 	raceTimeLine = getNextRaceTimeLine(lpFieldsResults, currLine+1, numLines);
//printf ("----currLine= %d, raceTimeine= %d\n", currLine, raceTimeLine);

 	if(raceTimeLine>0 && raceTimeLine-currLine< 4){
//empty PLAY
 	     strcpy(result, "1ST: ? ???");
 		 addEOL();
 		 addLineToTmp();
 	     strcpy(result, "2ND: ? ???");
 		 addEOL();
 		 addLineToTmp();
 	     strcpy(result, "3RD: ? ???");
 		 addEOL();
 		 addLineToTmp();
 		currLine = raceTimeLine-1;

 	} else{
// 1ST
     currLine++;
	 if(getLine(resultsStr, currLine, &iStart, &iStop) >= 0){
		copyLine(lpFieldsResults, iStart, iStop);
		cc =matchToStringResult("1ST: ", &kk);
//printf ("1----cc= %d, kk= %d\n", cc, kk);
		kk +=4;
		replaceChars(0, kk, "1ST: ");
//	printf ("----result= %s\n", result);
//	printf ("----resultTmp= %s\n", resultTmp);
//		idx = findHorseNameIDX(kk);
		if(matchHorseName(kk) >=0 ) addEOL();
		addLineToTmp();

	 }
// 2ND
     currLine++;
	 if(getLine(resultsStr, currLine, &iStart, &iStop) >= 0){
		copyLine(lpFieldsResults, iStart, iStop);
		cc =matchToStringResult("2ND: ", &kk);
//printf ("2----cc= %d, kk= %d\n", cc, kk);
		kk +=4;
		replaceChars(0, kk, "2ND: ");
//		idx = findHorseNameIDX(kk);
		if(matchHorseName(kk) >=0 ) addEOL();
		addLineToTmp();
	 }
// 3RD
     currLine++;
 	 if(getLine(resultsStr, currLine, &iStart, &iStop) >= 0){

//printLine(resultsStr, iStart, iStop);
 		copyLine(lpFieldsResults, iStart, iStop);
 		cc =matchToStringResult("3RD: ", &kk);
//printf ("3----cc= %d, kk= %d\n", cc, kk);

 		kk +=4;
 		replaceChars(0, kk, "3RD: ");
//printf("1        result= %s\n",result);
//		idx = findHorseNameIDX(kk);
 		if(matchHorseName(kk) >=0 ) addEOL();
//printf("2        result= %s\n",result);
 		addLineToTmp();
 	 }
 	}
// RACETIME
     currLine++;
 	 if(getLine(resultsStr, currLine, &iStart, &iStop) >= 0){
 		copyLine(lpFieldsResults, iStart, iStop);
 		cc =matchToStringResult("RACETIME: ", &kk);
 		rKK = findPlaceToreplaceDderby(kk, strlen(result)-4, 9);
//printf("        result= %s, cc= %d, kk= %d, rKK= %d\n",result, cc, kk, rKK);
	    replaceSpacesDDerby(rKK+1, strlen(result));
	    replaceToDigitDDerby(rKK+1, strlen(result));
	    repearTimeDDerby(rKK+1, strlen(result));
 		replaceChars(0, rKK, "RACETIME: ");


 		addLineToTmp();
 	 }
// find next PLAY
//printf("find next PLAY--- numLines= %d, currLine= %d\n",numLines, currLine);
 	 if(numLines - currLine < 6) break;
 	currPlay++;
 	LastPlay = currLine;
 	currLine++;
//printf("find next PLAY--- numLines= %d, currLine= %d\n",numLines, currLine);
 	currLine = getNextPlayLine(lpFieldsResults, currLine, numLines, currPlay);
//printf("after find next PLAY--- numLines= %d, currLine= %d\n",numLines, currLine);
    if(currLine < 0) break;
  }
  LastPlay++;
  if(LastPlay >= 0) {
 	   i = LastPlay;
 	   while(i<numLines){
 		  lpFieldsResults->lineQuadStatus[i] = 0;
 		  i++;
 	   }
  }

  copyResultsBack(lpFieldsResults);

}


int  postProcessingNumbers_CA(LPTICKETRESULTS ticketResults, int i){
	int status = 0; //, prevPostProc = 0;
	LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[i];
//	prevPostProc = testPrevPostProc(lpFieldsResults);
//	printf("------------------------------------------------ticketTypeInt= %d, prevPostProc= %d\n", ticketTypeInt, prevPostProc);
//	if(ticketTypeInt < 7)
//	   if(prevPostProc == 1) return status;
	switch(ticketTypeInt){
	case 0: // SUPER LOTTO CA
	case 1: // POWER BALL CA
	case 2: // MEGA MILLIONS CA
		cleanTicketNumbersCA(lpFieldsResults);
//		printField(&ticketResults->fieldsResults[i]);
		testNumbers_InsideLineCA(ticketResults, lpFieldsResults);
		trimSpaces(lpFieldsResults);
//		printField(&ticketResults->fieldsResults[i]);
		testNumbers_LastPartCA(ticketResults, lpFieldsResults);
//		printField(&ticketResults->fieldsResults[i]);
		trimSpaces(lpFieldsResults);
//		testNumbersLinesAfterPostProc(ticketResults, lpFieldsResults);
//		trimSpaces(lpFieldsResults);
	   break;
	case 3: // DAILY 3
	   cleanTicketNumbers_Daily3_4CA(ticketResults, i,  3);
	   trimSpaces(lpFieldsResults);
	   break;
	case 4: // DAILY 4
	   cleanTicketNumbers_Daily3_4CA(ticketResults, i,  4);
	   trimSpaces(lpFieldsResults);
	   break;
	case 5: // FANTASY 5
	   cleanTicketNumbersCA(lpFieldsResults);
		testNumbers_InsideLineCA(ticketResults, lpFieldsResults);
		trimSpaces(lpFieldsResults);
	   break;
	case 6: // HOT SPOT CA
	   cleanTicketNumbers_HotSpotCA(ticketResults, i);
	   break;
	case 7: // Daily Derby CA
	   cleanTicketNumbers_DailyDerbyCA(lpFieldsResults);

	   break;

	}
    status = getNumLowConfidentChars(lpFieldsResults, 20);
	return status;
//	printf("------------------------------------------------status= %d\n", status);
}
void trimAllSpaces(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop, i, n, len;
	 resultTmp[0] = 0;
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	       copyLine(lpFieldsResults, iStart, iStop);
	       len = strlen(result);
	       for(n=0;n<len;n++){
	    	   if(result[n] == 32) {
	    		   shrinkResults(n, n);n--;len = strlen(result);
	    	   }
	      }
	      len = strlen(result);n = len-2;
//	      printf ("--result[len-1]= %d\n", result[n]);
	      if(result[n] == 32) {
	    	  shrinkResults(n, n);
	      }
	      addLineToTmp();
	      i++;
	 }
	 copyResultsBack(lpFieldsResults);
}

void createDrawD3Field(LPTICKETRESULTS ticketResults, int n){
	 FIELDSRESULTS fieldsResults = ticketResults->fieldsResults[n];
	 char *resultsStr = fieldsResults.result;
	 int cD, cN, maxcD, maxcN, lineD, lineN;
	 int i;
	 int iStart, iStop;
	 resultTmp[0] = 0;

	 trimAllSpaces(&fieldsResults);
// printf("---resultsStr= %s\n", resultsStr);

	 i = 0;
	 maxcD = 0;
	 lineD = -1;
	 maxcN = 0;
	 lineN = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	    cD = matchToString(resultsStr, iStart, iStop, "EVENING");
	    if(cD > maxcD) {maxcD = cD;lineD = i;}
	    cN = matchToString(resultsStr, iStart, iStop, "MIDDAY");
	    if(cN > maxcN) {maxcN = cN;lineN = i;}
	    i++;
	 }
//     printf("2---maxcD= %d, maxcN= %d\n", maxcD, maxcN);

    {int nIdx = NUMFIELDS_RESULTS-1;
         strcpy(ticketResults->fieldsResults[nIdx].name, "DRAW");
         strcpy(ticketResults->fieldsResults[nIdx].result, "???");
         if(maxcD > 4 && maxcD > maxcN ) {
	         strcpy(ticketResults->fieldsResults[nIdx].result, "EVENING");
         }
         if(maxcN > 4 && maxcN > maxcD ) {
	         strcpy(ticketResults->fieldsResults[nIdx].result, "MIDDAY");
         }
         for(i=0;i<strlen(ticketResults->fieldsResults[nIdx].result);i++){
        	 ticketResults->fieldsResults[nIdx].flConf[i] = 100;
         }
   		for(i=0;i<NUM_LINESTATUS;i++){
   			ticketResults->fieldsResults[nIdx].lineQuadStatus[i] = 0;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p1X = ticketResults->fieldsResults[n].lineQuad[i].p1X;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p2X = ticketResults->fieldsResults[n].lineQuad[i].p2X;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p3X = ticketResults->fieldsResults[n].lineQuad[i].p3X;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p4X = ticketResults->fieldsResults[n].lineQuad[i].p4X;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p1Y = ticketResults->fieldsResults[n].lineQuad[i].p1Y;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p2Y = ticketResults->fieldsResults[n].lineQuad[i].p2Y;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p3Y = ticketResults->fieldsResults[n].lineQuad[i].p3Y;
   			ticketResults->fieldsResults[nIdx].lineQuad[i].p4Y = ticketResults->fieldsResults[n].lineQuad[i].p4Y;
   		}
   		ticketResults->fieldsResults[nIdx].lineQuadStatus[0] = 1;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p1X = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p1X;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p2X = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p2X;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p3X = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p3X;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p4X = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p4X;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p1Y = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p1Y;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p2Y = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p2Y;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p3Y = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p3Y;
 		ticketResults->fieldsResults[nIdx].lineQuad[0].p4Y = ticketResults->fieldsResults[n].lineQuad[NUM_LINESTATUS-1].p4Y;

    }
}

void postProcessingDrawD3_CA(LPTICKETRESULTS ticketResults){
	int i;
	  for(i=0;i<NUMFIELDS_RESULTS;i++){
		if(ticketResults->fieldsResults[i].name[0] != 0){
			if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {
				createDrawD3Field(ticketResults, i);
			}
		}
	  }
}


void  postProcessingLogo_CA(LPTICKETRESULTS ticketResults){
	switch(ticketTypeInt){
	case 3: // DAILY 3
		postProcessingDrawD3_CA(ticketResults);  break;
	}
}



void ticketPostProcessing_CA(LPTICKETDATA ticketDataPtr, LPTICKETRESULTS ticketResults){
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
    		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {postProcessingLogo_CA(ticketResults);setLogoSlices(ticketResults);}
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_CA(ticketResults, i);
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {postProcessingDate_CA(ticketResults);setDateSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN1") == 0) postProcessingDate1_TX(ticketResults);
    	}
    }

}

int getNumbersCnf_MegaCA(LPFIELDSRESULTS lpFieldsResults){
 	 int pen = 0;
	 char *resultsStr = lpFieldsResults->result;
	 int numLines = 0, i, k;
	 int iStart, iStop, len, lenTot;

	// get number of lines
		 i = 0;
		 while(1){
			if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			numLines++;
			i++;
		 }
		 if(numLines <1) pen = 10;

		 lenTot = strlen(resultsStr);
		 i = 0;
		 while(i < numLines){
			if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			 len = iStop-iStart;
			 if(iStop == lenTot-1 && resultsStr[iStop] > 32)
				 len++;
			 pen += abs(19-len);
//printLine(resultsStr, iStart, iStop);
//printf ("----i= %d, pen= %d, len= %d\n", i, pen, len);
			 k = iStart+1;
			 while(k<iStop-2){
				 if(resultsStr[k] != ' ') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
				 if(resultsStr[k] < '0' || resultsStr[k] > '9') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
				 if(resultsStr[k] < '0' || resultsStr[k] > '9') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
			 }
			i++;
		 }
//	 printf ("----pen= %d, len= %d\n", pen, len);
   return pen;
}
int getNumbersCnf_Fantasy5CA(LPFIELDSRESULTS lpFieldsResults){
 	 int pen = 0;
	 char *resultsStr = lpFieldsResults->result;
	 int numLines = 0, i, k;
	 int iStart, iStop, len, lenTot;

	// get number of lines
		 i = 0;
		 while(1){
			if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			numLines++;
			i++;
		 }
		 if(numLines <1) pen = 10;

		 lenTot = strlen(resultsStr);
		 i = 0;
		 while(i < numLines){
			if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
			 len = iStop-iStart;
			 if(iStop == lenTot-1 && resultsStr[iStop] > 32)
				 len++;
			 pen += abs(16-len);
//printLine(resultsStr, iStart, iStop);
//printf ("----i= %d, pen= %d, len= %d\n", i, pen, len);
			 k = iStart+1;
			 while(k<iStop-2){
				 if(resultsStr[k] != ' ') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
				 if(resultsStr[k] < '0' || resultsStr[k] > '9') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
				 if(resultsStr[k] < '0' || resultsStr[k] > '9') pen++;
//				 printf ("----pen= %d, resultsStr[ %d ]= %c\n", pen, k, resultsStr[k]);
				 k++;
			 }
			i++;
		 }
//	 printf ("----pen= %d, len= %d\n", pen, len);
   return pen;
}

int getNumbersCnf_CA(LPFIELDSRESULTS lpFieldsResults){
  int pen = 0;

	switch(ticketTypeInt){
	case 0: // SUPER LOTTO CA
	case 1: // POWER BALL CA
	case 2: // MEGA MILLIONS CA
		pen = getNumbersCnf_MegaCA(lpFieldsResults);
//		printf ("----pen= %d\n", pen);
	   break;
	case 5: // FANTASY 5 CA
		pen = getNumbersCnf_Fantasy5CA(lpFieldsResults);
//		printf ("----pen= %d\n", pen);
	   break;

	}



  return pen;

}
