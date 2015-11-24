/*
 * PostprocessingTX.c
 *
 *  Created on: Apr 1, 2013
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

//void printfQuad(void);
void printLine(char *resultsStr, int iStart, int iStop);
void setLogoSlices(LPTICKETRESULTS ticketResults);
void setDateSlices(LPTICKETRESULTS ticketResults);
int getLine(char* results, int n, int* iStart, int* iStop);
int testLineConf(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void testLeftSideForLine(char *resultsStr, int *iStart, int iStop);
int getNumberOfGroups(char* results, int iStart, int iStop);
int getNumChars(char *resultsStr, int iStart, int iStop);
int testBeginCut(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void copyLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void addEOL(void);
void copyResultsBack(LPFIELDSRESULTS lpFieldsResults);
void replaceChars(int iStart, int iStop, char* strT);
//void getFirstCharFromNumLine(int* iStart, int* iStop);
void addLineToTmp(void);
int getNumLowConfidentChars(LPFIELDSRESULTS lpFieldsResults, int tH);
void cleanMonth(char *strInp);
int matchToString_1(char *results, int iStart, int iStop, char *str, int *shiftIdx);
int matchToString(char *results, int iStart, int iStop, char *str);
int matchToStringResult(char *str, int *kM);
void cutResults(int kMF);
void shrinkResults(int iStart, int iStop);
int adjustLine(char *resultsStr, int line);
int adjustLineA(char *resultsStr, int line);
int matchToStringEndOneLineIdx(char *results, char *str, int sh);
//int matchToStringEnd(char *results, int iStart, int iStop, char *str);
int  postProcessingDateD(LPFIELDSRESULTS lpFieldsResults);
void replaceFirstCharsTmp(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int n);
void cleanYear(LPTICKETRESULTS ticketResults, char *title);
void cleanDateField(LPTICKETRESULTS ticketResults, char *title);
int testLineConfC5_TX(LPFIELDSRESULTS lpFieldsResults, int line);
int testLineConfAllorN2_TX(LPFIELDSRESULTS lpFieldsResults, int line);

//void createPlierField(LPTICKETRESULTS ticketResults, int n);
//void cleanTicketNumbers(LPFIELDSRESULTS lpFieldsResults);
//int getSecondQPFromNumLine(int* iStart, int* iStop);
//void  postProcessingLogo(LPTICKETRESULTS ticketResults);
//void  postProcessingDateNew(LPTICKETRESULTS ticketResults);
//void  postProcessingDate1New(LPTICKETRESULTS ticketResults);
void stretchResults(int idx, int lenStr);
void insResult(int idx, char *strT);
void postProcessingEXTRA_TX(LPTICKETRESULTS ticketResults);
void postProcessingDraw2_TX(LPTICKETRESULTS ticketResults);
void postProcessingDraw4_TX(LPTICKETRESULTS ticketResults);
int matchToString_Idx(char *results, int iStart, int iStop, char *str, int *idx);
int matchToStringEndSh(char *results, int iStart, int iStop, char *str, int sh);
void testLeftSideForLineTXC5(LPFIELDSRESULTS lpFieldsResults, int *iStart, int iStop);
//int  postProcessingDateA_TX(LPFIELDSRESULTS lpFieldsResults);
void testTrashBegEnd(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
void getFirstCharFromNumLineC5(int* iStart, int* iStop, int nGrp);
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
void getFirstCharFromNumLineM(int* iStart, int* iStop );
void testTrashBegEndM(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
void getFirstCharFromNumLineTS(int* iStart, int* iStop);

//void printResults_1(LPTICKETRESULTS ticketResults);
extern char *Month_Names[];
extern char *Day_Names[];
extern char        resultTmp[MAXCHARS+2];
extern WORD        flConfTmp[MAXCHARS+2];
extern QUADRANGLE  flQuadTmp[MAXCHARS+2];
extern char result[200];
extern WORD flConf[200];
extern QUADRANGLE flQuad[200];
extern int thConf;
extern int vHistPreProc[H_SIZE_PREPROC];
extern int linesPostCnf;

char *Pick3_Names[] = { " STR ", " BOX ", " STRBOX ", " FRONT ", " MID ", " BACK ", " ANY ", " COMBO ", " EXT ", " EXTANY ", " COMO "};
int len_Pick3_Names = 11;
void addLineToTmpSpace(void);
void getDollarAmount(char *resultsStr, int start, int stop, char *dollarAm, int *cnfIn, int *cnfOut);
void getDollarAmountSumItUp(char *rStr, int start, int stop, char *dollarAm, char *totalAm, int *cnfIn, int *cnfOut, int *cnfTotalOut);
void getDollarSign(char *result, int *iDollar);
void shiftQuadUp(LPQUADRANGLE qO, int idx, int lenQ, int lenSh);
void copyLineWithSpace(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
void trimSpaces(LPFIELDSRESULTS lpFieldsResults);
int getGroupsFromLine(char *resultsStr, int iStart, int iStop);
void cleanAgainQP(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop);
int getNumberFromStop(char *resultsStr, int iStart, int iStop, int n, int *nStart, int *nStop);
int getGGroupsFromLine(char *resultsStr, int iStart, int iStop);
int getCnfGroups_2FromLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop);
int cmpStringsExact(char *str1, char *str2);
void cpyTmpToResult(void);
int matchToStringStr(char *str, char *strKey, int *kM);
int getGroups_2FromLineResult(void);
int getGroups_2FromLine(char *resultsStr, int iStart, int iStop);
int getGroups_1FromLine(char *resultsStr, int iStart, int iStop);
void lastCheckDateField(LPTICKETRESULTS ticketResults, char *title);
void findDayMonthResult();
int getDigDateStart(void);
int getDigDateStop(int k1);
void addFieldDateTo_AllOrN(LPTICKETRESULTS ticketResults, int lineB, int iA);
int testForMultiDraw_1(LPFIELDSRESULTS lpFieldsResults, int lineMax, int *lineTop, int *lineBottom);
void postProcessingRetNum_TX(LPTICKETRESULTS ticketResults);

void createPlierFieldTX(LPTICKETRESULTS ticketResults, int n){
	 FIELDSRESULTS fieldsResults = ticketResults->fieldsResults[n];
	 char *resultsStr = fieldsResults.result;
	 int cM, cP;
	 int cM1, cP1, cP2;
	 int cY, cN;

	 int maxc[2], linec[2];
	 int i, kk;
	 int iStart, iStop;
//	 int ccYes, ccNo, maxYes, maxNo;
	 resultTmp[0] = 0;
	 for(i = 0;i<2;i++){
		 maxc[i] = 0; linec[i] = -1;
	 }
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(&fieldsResults, iStart, iStop);
		cM =matchToStringResult("MEGAPLIER ", &kk);
		if(kk > 4) cM = 0;
		cM1 =matchToStringResult("EGAPLIER ", &kk);
		if(kk > 4) cM1 = 0;
		if(cM1 > cM) cM = cM1;
		cM1 =matchToStringResult("MEGA PLIER ", &kk);
		if(kk > 4) cM1 = 0;
		if(cM1 > cM) cM = cM1;
	    if(cM > maxc[0]) {maxc[0] = cM;linec[0] = i;}
		cP =matchToStringResult("POWERPLAY ", &kk);
		if(kk > 4) cP = 0;
		cP1 =matchToStringResult("POWER PLAY ", &kk);
		if(kk > 4) cP1 = 0;
		cP2 =matchToStringResult("POWERPMY ", &kk);
		if(kk > 4) cP2 = 0;
		if(cP1 > cP) cP = cP1;
		if(cP2 > cP) cP = cP2;
	    if(cP > maxc[1]) {maxc[1] = cP;linec[1] = i;}
//printf("---result= %s\n", result);
//printf("---i= %d, cM= %d, cP= %d\n", i, cM, cP);
	    i++;
	 }
//printf("--- maxc[0]= %d, linec[0]= %d, maxc[1]= %d, linec[1]= %d\n", maxc[0], linec[0], maxc[1],  linec[1]);


   {int nIdx = NUMFIELDS_RESULTS-1;
    int drawIdx = 7;
        strcpy(ticketResults->fieldsResults[nIdx].name, "PLAYER");
        strcpy(ticketResults->fieldsResults[nIdx].result, "???");
        drawIdx = 0;
        if(maxc[0]<maxc[1])
        	drawIdx = 1;
        if(maxc[0] == maxc[1] && linec[0] != linec[1]) drawIdx = 7;
//        printf("---drawIdx= %d\n", drawIdx);
        if(maxc[drawIdx] <= 4) drawIdx = 7;
        else{
    		if(getLine(resultsStr, linec[drawIdx], &iStart, &iStop) >= 0){
    			copyLine(&fieldsResults, iStart, iStop);
    			cY =matchToStringResult(" YES", &kk);
    			cN =matchToStringResult(" NO", &kk);
    	         if(cY > cN) {
    		         strcpy(ticketResults->fieldsResults[nIdx].result, "YES");
    	         }
    	         if(cY < cN) {
    		         strcpy(ticketResults->fieldsResults[nIdx].result, "NO");
    	         }
    	         for(i=0;i<strlen(ticketResults->fieldsResults[nIdx].result);i++){
    	        	 ticketResults->fieldsResults[nIdx].flConf[i] = 100;
    	         }

    		}
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

void  postProcessingLogo2_TX(LPTICKETRESULTS ticketResults){
int i;
  for(i=0;i<NUMFIELDS_RESULTS;i++){
	if(ticketResults->fieldsResults[i].name[0] != 0){
		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {
			createPlierFieldTX(ticketResults, i);
		}
	}
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
void  postProcessingLogo_TX(LPTICKETRESULTS ticketResults){
	switch(ticketTypeInt){

	case 1: // POWER BALL TX
	case 2: // MEGA MILLIONS TX
		postProcessingLogo2_TX(ticketResults);  break;

	case 3:  // LOTTO TEXAS
		postProcessingEXTRA_TX(ticketResults);  break;
	case 5: // PICK 3 TX
	case 6: // DAILY 4 TX
	case 8: // ALL OR NOTHING TX
		postProcessingDraw4_TX(ticketResults);  break;
	default:
		postProcessingLogo2_TX(ticketResults);break;
	}
}

/*
void  postProcessingLogo_TX(LPTICKETRESULTS ticketResults){
	switch(ticketTypeInt){
	case 0:  // LOTTO TEXAS
		postProcessingEXTRA_TX(ticketResults);  break;
//	case 1: // PICK 3 TX
//	case 4: // DAILY 4 TX
//		postProcessingDraw2_TX(ticketResults);  break;

	case 5: // POWER BALL TX
	case 6: // MEGA MILLIONS TX
		postProcessingLogo2_TX(ticketResults);  break;
	case 1: // PICK 3 TX
	case 4: // DAILY 4 TX
	case 7: // ALL OR NOTHING TX
		postProcessingDraw4_TX(ticketResults);  break;
	default:
		postProcessingLogo2_TX(ticketResults);break;
	}
}
*/
int getFirstQPFromNumLineTwoStep(void){
	int i, k, len;
	int st;
	int avX, ccX, avY, ccY;
	int xx, yy;

//   printf("---------------------------- result= %s", result);
	len = strlen(result);
	k = -1;
	avX = 0;avY = 0;
	ccX = 0;ccY = 0;

//X
	st = len/4-1;
	if(st+9>=len) return -1;
	for(i=st;i<st+9;i++){
       if(result[i] == 32) {
    	   avX += (flQuad[i+1].p1X - flQuad[i-1].p2X);
    	   ccX++;
           if(ccX > 1) {k = i;break;}
       }else{
    	   avY += (flQuad[i].p3Y - flQuad[i].p1Y);
    	   ccY++;
       }

	}
	if(ccX>0)  avX /= ccX;
	if(ccY>0)  avY /= ccY;

//	printf("  avX= %d, avY= %d \n",avX, avY);
    if(k < 0) return -1;
    xx = 0;
	for(i=k;i<len-2;i++){
		   if(result[i] == 32) {
	    	   xx = (flQuad[i+1].p1X - flQuad[i-1].p2X);
// 	   printf("  xx= %d, 4*avX= %d, result[i-1] = %c, result[i+1]= %c \n",xx, 4*avX, result[i-1], result[i+1]);
//	    	   if(xx > 4*avX) {
//	    		   result[i+1] = ' ';
//	    		   if(i+2 < len && result[i+2] > 32)
//		    		   result[i+2] = ' ';
//	    		   break;
//	    	   }
	       }else{
	    	   yy = (flQuad[i].p3Y - flQuad[i].p1Y);
//  	   printf("  yy= %d, avY-yy= %d, avY/5= %d, xx= %d, 2*avX= %d, result[i] = %c\n",yy, avY-yy, avY/5, xx, 2*avX, result[i] );
	    	   if(xx > 2*avX && avY-yy > avY/5) {
	    		   result[i] = ' ';
	    		   if(result[i+1] > 32)
		    		   result[i+1] = ' ';
	    		   break;
	    	   }
//	   	   	printf("  result[i]= %c, yy=%d \n",result[i], yy);
	       }

	}

    return 0;
}


int getLastQPFromNumLine(int* iStart, int* iStop, int nn){
		int  i, len;
		int avSp = 0, cc = 0, lSp, dd, lNSp;
		int ret = -1;

		len = strlen(result);

		 for(i=4;i<12;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			 avSp /= cc;
		      lSp = 0;lNSp = 0;
			  for(i=len-1;i>0;i--){
				  if(result[i] == 32) {lNSp= i;break;}
		      }
			  if(lNSp > 0) {
			     for(i=lNSp;i>0;i--){
				     if(result[i] == 32) {lSp= i;break;}
		         }
			  }
		       if(lSp > 0){
		    	   dd = flQuad[lSp+1].p1X -flQuad[lSp-1].p2X;
	//	    	   printf(" dd= %d, avSp= %d\n",dd, avSp);
			     if(dd > 2*avSp) {*iStart = lSp;*iStop = lNSp; ret = 1;}
			   }
			}
		return ret;
	}

int getSecondQPFromNumLineLastQP(int* iStart, int* iStop, int nn){
	int  k, i, cc, len;
	int iStop_QP1 = *iStop;
	len = strlen(result);
	k = len-1;
	while(result[k] <= 32) k--;
	*iStop = k;

// number of groups
	cc = 0;
	for(i = k;i>=0;i--) {
		if(result[i] <= 32) cc++;
	}
	if(cc > nn+1 && result[k-1] <= 32) {
		k--;
		while(result[k] <= 32) k--;
		cc = 0;
		for(i = k;i>=0;i--) {
			if(result[i] <= 32) cc++;
		}
		if(cc > nn+1 && result[k-1] <= 32) {
			k--;
			while(result[k] <= 32) k--;
		}
	}

	cc = 0;
    if(iStop_QP1 > 0){
    	for(i = iStop_QP1;i<k;i++)
    		if(result[i] <= 32) cc++;
//printf("--- iStop_QP1= %d, k= %d, cc= %d\n",iStop_QP1, k, cc);
    	if (cc < 2)
    	   return -1;
    }
	while(result[k] > 32) k--;
    if(k-2 > 0)
    	if(result[k-2] == 32)
    		   k -= 2;
    k++;
	*iStart = k;
	return 0;
//	printf("--- *iStart= %d, *iStop= %d\n",*iStart, *iStop);
//	printf("--- result[*iStart]= %c, result[*iStop]= %c\n",result[*iStart], result[*iStop]);
}
void testGroupsTX(char *resultsStr, int iStart, int iStop, int *cD, int *cS){
	int i, k, len, iSt;
	int cc, ccD, ccS;
	*cD = 0; *cS = 0;

	len = iStop-iStart-1;
	if(len < 5) return;
	iSt = iStart;
	ccD = 0;ccS = 0;
	if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt += 2;
	i = iSt;
	while(i<iStop){
		cc = 0;
		k=i;
		while(k<=iStop){
			if(resultsStr[k] <= 32) break;
			cc++;
			k++;
		}
		if(cc == 2) ccD++;
		i = k;
		cc = 0;
		while(k<iStop){
			if(resultsStr[k] > 32) break;
			cc++;
			k++;
		}
		i = k;
		if(cc == 1) ccS++;
	}
	*cD = ccD; *cS = ccS;
//    if(ccD == nn+1 && ccS == nn) corr = 2;

//  printf ("--------ccD= %d, ccS= %d, corr= %d\n", ccD, ccS, corr);

//	return corr;
}
int getSpaceForTheChars(int *hist, int x1, int xL1, int *xL, int *xR, int xR2){
	int dd = 0, xH, i;
	int ccL, ccR, stop;
	xH = *xR-x1+1;
	if(xH < 0 || xH > hist[0]) return dd;


//printf("-------------xH= %d\n", xH);
	if(hist[xH] > 0){
	  for(i=xH+1;i<=xH+3;i++){
		if(i>hist[0]) break;
		if(hist[i] == 0) {xH = i;break;}
		if(hist[i] > 6) {;break;}
	  }
	}
//   if(hist[xH] > 0){
//	   if(hist[xH+1] == 0) xH += 1;
//   }

//   printf("-------------xH= %d\n", xH);

	ccL = 0; ccR = 0;
	stop = xR2-x1+1;
	for(i=xH;i<=stop;i++){
		if(hist[i] == 0) ccR++;
		else break;
	}
	*xR = *xR+ccR;
	stop = xL1-x1+1;
	for(i=xH;i>stop;i--){
		if(hist[i] == 0) ccL++;
		else break;
	}
	if(ccL>0)
	  *xL = *xR-ccL;
	dd = ccL+ccR;
//printf("-------------xH= %d, ccR= %d, ccL= %d, dd= %d\n", xH, ccR, ccL, dd);
	return dd;
}
void addSpaceToLine(int *hist, int x1, int th, int line){
	int i, len,  dd;
	int k, dd1;
//	 char buffLog[500];

//printf("result= %s", result);

	len = strlen(result);
	k = 0;
	for(i=2;i<len;i++){
		if(result[i-2]>32 && result[i-1]>32 && result[i]>32){
			if(k==0) dd1 = getSpaceForTheChars(hist, x1, flQuad[i-2].p1X, &flQuad[i-2].p2X, &flQuad[i-1].p1X, flQuad[i].p2X);
			k++;
            dd = getSpaceForTheChars(hist, x1, flQuad[i-1].p1X, &flQuad[i-1].p2X, &flQuad[i].p1X, flQuad[i].p2X);

//printf("-------------dd= %d, result[i-1]= %c, result[i]= %c, x1= %d, flQuad[i].p1X= %d, flQuad[i].p1X-x1+1= %d\n", dd, result[i-1], result[i], x1, flQuad[i].p1X, flQuad[i].p1X-x1+1);
//printf("-------------dd= %d, dd1= %d, result[i-1]= %c, result[i]= %c\n", dd, dd1, result[i-1], result[i]);

			if(dd > th) {
			   if(flQuad[i].p2X - flQuad[i].p1X < 4){
				   result[i] = ' ';
			   }else {
		 	      stretchResults(i, 1);
		 	      insResult(i, " ");
		 	      i++;
		 	      len++;
		 	      k = 0;
			   }
			}
			else{
				if(dd1 < 10 && dd > 12 && dd > 2*dd1) {
			 	      stretchResults(i, 1);
			 	      insResult(i, " ");
			 	      i++;
			 	      len++;
			 	      k = 0;
				}
			}
			dd1 = dd;
		}else
		  k = 0;
	}
//	printf("result= %s", result);
}
void getXY_ForLine(int *x1, int *x2, int *y1, int *y2){
	int i, len;
	len = strlen(result);
	for(i=0;i<len;i++){
		if(result[i] > 32) {
			*x1 = flQuad[i].p1X;
			*y1 = flQuad[i].p1Y;
			*y2 = flQuad[i].p3Y;
			break;
		}
	}
	for(i=len-1;i>0;i--){
		if(result[i] > 32) {
			*x2 = flQuad[i].p2X;
			break;
		}
	}
}
void addSpacesToNumbersTX(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults) {
   char *resultsStr = lpFieldsResults->result;
   int iStart, iStop;
   int i;
   int byteWidthHist = ((img -> bi.biWidth + 31)/32)*4;
   int x1, x2, y1, y2, size;

//   int k;
//   char buffLog[500];

  i = 0;
  while(1){
	if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
    copyLine(lpFieldsResults, iStart, iStop);
    getXY_ForLine(&x1, &x2, &y1, &y2);

//    if(i==18){
//printf("1------------- i= %d, result= %s\n", i, result);
//fflush(stdout);
//    }
//sprintf(buffLog,"1------------- result= %s\n", result);
//saveLog("", buffLog);


    size = x2-x1+1;
    if(size >=H_SIZE_PREPROC) size =H_SIZE_PREPROC-1;
     y1 += 3;
//   printf("------byteWidthHist= %d, img -> bi.biHeight= %d, size= %d, x1= %d, x2= %d, y1= %d, y2= %d\n", byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2);
//   fflush(stdout);
   if(size > 50) {
      histVPtrPix(img ->pixPtr, byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2, vHistPreProc);
//    printf("------------- vHistPreProc[0]= %d\n", vHistPreProc[0]);

/*
if(i==13){

	for(k=0;k<vHistPreProc[0];k++) {
       sprintf(buffLog," vHistPreProc[ %d ]= %d\n", k, vHistPreProc[k]);
       saveLog("", buffLog);
	}

//    sprintf(buffLog," x1= %d, x2= %d, y1= %d, y2= %d\n", x1, x2, y1, y2);
//    saveLog("", buffLog);

	for(k=0;k<strlen(result);k++) {
       sprintf(buffLog," k= %d, result[k] = %c, flQuad[k].p1X= %d, flQuad[k].p2X= %d\n", k, result[k], flQuad[k].p1X, flQuad[k].p2X);
       saveLog("", buffLog);
	}

}
*/
//   printf("1.5------------- result= %s\n", result);
//   fflush(stdout);

    addSpaceToLine(vHistPreProc, x1, 18, i);

//printf("2------------- result= %s\n", result);
//fflush(stdout);
//    sprintf(buffLog,"2------------- result= %s\n", result);
//    sprintf(buffLog,"---------------------------\n");
//    saveLog("", buffLog);

    addLineToTmp();
   }
	i++;

//	break; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
  copyResultsBack(lpFieldsResults);
}
 int recalcNumGroups(void){
   int stopTmp = strlen(result);
   int ccSp, ccSpC, i;

    for(i=stopTmp;i>0;i--){
	  if(result[i] > 32) { stopTmp = i;break;}
	}
    ccSp = 0;ccSpC = 0;
    for(i=0;i<stopTmp;i++){
	   if(result[i] == 32) {
		   if(ccSpC == 0) ccSp++;
		   ccSpC++;
	   }
	   else ccSpC = 0;
   }
    return ccSp;
 }

 void getXY_ForLine_2(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop, int *x1, int *x2, int *y1, int *y2){
 	int i;
 	char *resultsStr = lpFieldsResults->result;
 	for(i=*iStart;i<*iStop;i++){
 		if(resultsStr[i] > 32) {
 			*x1 = lpFieldsResults->flQuad[i].p1X;
 			*y1 = lpFieldsResults->flQuad[i].p1Y;
 			*y2 = lpFieldsResults->flQuad[i].p3Y;
 			break;
 		}
 	}
 	for(i=*iStop-1;i>0;i--){
 		if(resultsStr[i] > 32) {
 			*x2 = lpFieldsResults->flQuad[i].p2X;
 			break;
 		}
 	}
 }

 void shiftSpaces(int *hist, int x1, int xL1, int *xL, int *xR, int xR2){
	 	int i;
	 	int ccL, ccR, stop, cc;
	 	ccL = *xL-x1+1;
	 	ccR = *xR-x1+1;
	 	stop = xR2-x1+1;
	 	cc = 0;
	 	for(i=ccR+1;i<=stop;i++){
	 		if(hist[i] == 0) cc++;
	 		else break;
	 	}
	 	*xR = *xR+cc;
	 	stop = xL1-x1+1;
	 	cc = 0;
	 	for(i=ccL-1;i>stop;i--){
	 		if(hist[i] == 0) cc++;
	 		else break;
	 	}
	 	*xL = *xL-cc;
 }

void recalcSpaces(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop) {
	   char *resultsStr = lpFieldsResults->result;

	   int byteWidthHist = ((img -> bi.biWidth + 31)/32)*4;
	   int x1, x2, y1, y2, size, i;
	    x1 = 0; x2 = 100; y1 = 0; y2 = 100;
	    getXY_ForLine_2(lpFieldsResults, iStart, iStop, &x1, &x2, &y1, &y2);

	    size = x2-x1+1;
	    if(size >=H_SIZE_PREPROC) size =H_SIZE_PREPROC-1;
	     y1 += 3;
	//   printf("------byteWidthHist= %d, img -> bi.biHeight= %d, size= %d, x1= %d, x2= %d, y1= %d, y2= %d\n", byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2);
	//   fflush(stdout);
	   if(size > 50) {
	      histVPtrPix(img ->pixPtr, byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2, vHistPreProc);
	//    printf("------------- vHistPreProc[0]= %d\n", vHistPreProc[0]);
	   	for(i=*iStart;i<*iStop-1;i++){
	  		if(resultsStr[i]==32)
              shiftSpaces(vHistPreProc, x1, lpFieldsResults->flQuad[i-1].p1X, &lpFieldsResults->flQuad[i-1].p2X, &lpFieldsResults->flQuad[i+1].p1X, lpFieldsResults->flQuad[i+1].p2X);

	   	}
	   }
}
int recalcNumGroups2(char *resultsStr, int iStart, int iStop){
  int stopTmp = iStop;
  int ccSp, ccSpC, i, iSt;

   for(i=stopTmp;i>iStart;i--){
	  if(resultsStr[i] > 32) { stopTmp = i;break;}
	}
   iSt = iStart;
   for(i=iStart;i<iStop;i++){
	  if(resultsStr[i] == 32) { iSt = i+1;break;}
	}

//   if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt +=2;
   ccSp = 0;ccSpC = 0;
   for(i=iSt;i<stopTmp;i++){
	   if(resultsStr[i] == 32) {
		   if(ccSpC == 0) ccSp++;
		   ccSpC++;
	   }
	   else ccSpC = 0;
  }
   return ccSp;
}

void cleanOneDigit_TwoStep(char *resultsStr, int iStart, int iStop){
	int i, numGr, thGroups = 100;
	numGr = recalcNumGroups2(resultsStr, iStart, iStop);
//	printf("cleanOneDigit; numGr= %d\n",numGr);
 	switch(ticketTypeInt){
   	                   case 4: // TWO STEP
   	                   case 7: // CASH FIVE
    	                	   thGroups = 4;break;
                       case 1: // POWER BALL
                       case 2: // MEGA MILLIONS
             		   case 3: // LOTTO TEXAS
                       case 8: // All Or Nothing
                    	      thGroups = 5;break;
 	}
    if(numGr > thGroups){
    	int  cc=0, iSt = iStart;
//    	   if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt +=2;
    	   for(i=iStart;i<iStop;i++){
    		  if(resultsStr[i] == 32) { iSt = i+1;break;}
    		}

    		for(i=iSt;i<iStop;i++){
    			if(resultsStr[i] > 32){
    			    cc++;
    			}else{
    			  if(cc==1) resultsStr[i-1] = ' ';
                  cc=0;
    			}
    		}
    }

    numGr = recalcNumGroups2(resultsStr, iStart, iStop);
//    printf("numGr= %d, thGroups=%d\n",numGr,thGroups);
    if(numGr > thGroups){
    	int   iSt = iStop;
		for(i=iStop;i>iStart;i--){
			iSt = i;
			if(resultsStr[iSt] > 32) break;
		}

		if(iSt > iStart+1 &&resultsStr[iSt] > 32 && resultsStr[iSt-1] == 32)
			resultsStr[iSt] = ' ';
    }
}

void cleanOneDigit_AllOrN(void){
	int i, numGr, thGroups = 100;
	int len = strlen(result);
	numGr = recalcNumGroups();
//	printf("cleanOneDigit; numGr= %d\n",numGr);
    thGroups = 6;
    if(numGr > thGroups){
    	int  cc=0, iSt = 0;
    	   if(result[iSt] > 32 && result[iSt+1] == 32) iSt +=2;
    		for(i=iSt;i<len;i++){
    			if(result[i] > 32){
    			    cc++;
    			}else{
    			  if(cc==1) result[i-1] = ' ';
                  cc=0;
    			}
    		}
    }

	numGr = recalcNumGroups();
//    printf("numGr= %d, thGroups=%d\n",numGr,thGroups);
    if(numGr > thGroups){
    	int   iSt = len-1;
		for(i=len-1;i>0;i--){
			iSt = i;
			if(result[iSt] > 32) break;
		}

		if(iSt > 1 && result[iSt] > 32 && result[iSt-1] == 32)
			result[iSt] = ' ';
    }
}

void testFirstQPGroup_TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
  char *resultsStr = lpFieldsResults->result;
  int  numGr, thGroups = 100;

   numGr = recalcNumGroups2(resultsStr, iStart, iStop);
	switch(ticketTypeInt){
  	                   case 4: // TWO STEP
   	                	   thGroups = 4;break;
                      case 1: // POWER BALL
                      case 2: // MEGA MILLIONS
                   	      thGroups = 5;break;
                      case 8: // All Or Nothing
                   	      thGroups = 6;break;
	}
   if(numGr > thGroups){
      int n, iN1, iN2;
      int k, iK1, iK2;
      int m = 0, iM1=-1, iM2=-1;
      int idx, ok;

	   n = getNumberFromStop(resultsStr, iStart, iStop, 1, &iN1, &iN2);
	   idx = 2;
	   iK1 = iN1;
		while(iK1>iStart) {
		   k = getNumberFromStop(resultsStr, iStart, iStop, idx, &iK1, &iK2);
//   printf("iK1= %d, iK2= %d, k= %d, idx= %d\n",iK1,iK2,k, idx);
		   if(k<=0) break;
		   m = k; iM1=iK1; iM2=iK2;
		   idx++;
		}
//printf("iN1= %d, iN2= %d, n= %d, iM1= %d, iM2= %d, m= %d\n",iN1,iN2,n, iM1, iM2, m);
		ok = 1;
    if(iN1 >= 0 && iN2 >= 0){
	   for(k=iN1;k<=iN2;k++)
		   if(lpFieldsResults->flConf[k] > 25) {ok=0;break;}
//		  printf("resultsStr[k]= %c, cnf[k]= %d\n",resultsStr[k], lpFieldsResults->flConf[k]);
    }
    else ok = 0;

    if(iM1 >= 0 && iM2 >= 0){
	   for(k=iM1;k<=iM2;k++)
		   if(lpFieldsResults->flConf[k] < 25) {ok=0;break;}
//		  printf("resultsStr[k]= %c, cnf[k]= %d\n",resultsStr[k], lpFieldsResults->flConf[k]);
    }else ok = 0;
    n = m;
    m = n; // skip compiler warnings
    if(ok == 1){
 	   for(k=iN1;k<=iN2;k++)
 		  resultsStr[k] = ' ';
    }

  }

}

void setSpaceAtStart(char *resultsStr, int iStart1, int iStart2){
	int i;
	   for(i=iStart1;i<iStart2;i++){
		   if(resultsStr[i] > 32) resultsStr[i] = ' ';
	  }
}
void trimSpacesResult(void){
	int n, len, cc;
    len = strlen(result);
// left side
    for(n=0;n<len;n++){
 	   if(result[n] == 32) {shrinkResults(n, n);n--;len = strlen(result);}
 	   else break;
    }
// inside
    cc = 0;
    for(n=0;n<len;n++){
 	   if(result[n] == 32) {
 		   cc++;
 		   if(cc>1) {shrinkResults(n, n);n--;len = strlen(result);}
 	   }
 	   else
 		   cc = 0;
   }
// right side
   len = strlen(result);n = len-2;
//	      printf ("--result[len-1]= %d\n", result[n]);
   if(result[n] == 32) {
 	  shrinkResults(n, n);
   }
}
int testOnesInsideLine(char *resultsStr, int iStart, int iStop){
	int i, cc, cc1;
	float dd;
	  cc = 0; cc1 = 0;
	  for(i=iStart;i<iStop;i++){
		  if(resultsStr[i] > 32){
			  cc++;
			  if(resultsStr[i] == '1') cc1++;
		  }
	  }
	  dd = (float)cc1/(float)cc;
	  if(dd > 0.7) return 1;
	  return 0;
}
int testLineConf_Modified(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	int i, cc, ccG, ccT;
	i = iStart;
	ccG = 0;ccT = 0;
	cc = 0;
	while(i < iStop) {
	      if(lpFieldsResults->result[i] > 32) cc++;
	      else cc = 0;
	      if(cc> 2) ccG++;
	      i++;
     }
//	printf(" ccG= %d\n",ccG);
	i = iStart;
	cc = 0;
	while(i < iStop) {
		  if((lpFieldsResults->result[i] > 32) && (lpFieldsResults->result[i] != '1')) ccT++;

		  if((lpFieldsResults->result[i] > 32) && (lpFieldsResults->result[i] != '1') &&
				  lpFieldsResults->flConf[i] < 20){ cc++;
// printf(" lpFieldsResults->result[i]= %c, lpFieldsResults->flConf[i]= %d\n",lpFieldsResults->result[i], lpFieldsResults->flConf[i]);
		  }
	      i++;
     }
//printf(" cc= %d, ccT= %d, ccG= %d\n",cc, ccT,ccG);
	if(cc > ccT/2 && ccG > 2) {
		if(cc < (2*ccT)/3){
			int gr_2;
			gr_2 = getCnfGroups_2FromLine(lpFieldsResults, iStart, iStop);
			if(gr_2 > 4) return 1;
		}

		return -1;
	}
	return 1;

}

int maxDigitGroup(char *resultsStr, int iStart, int iStop){
	int i, len, iSt;
	int cc, ccD, ccS;
	int grNum = 0, grIdx = 0;


	len = iStop-iStart-1;
	if(len < 5) return 0;
	iSt = iStart;
	if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt += 2;
	i = iSt;
	ccD = 0;ccS = 0;ccD= 0;
	cc = 0;
	while(i<iStop){
		if(resultsStr[i] > 32) {
			if(cc == 0) grNum++;
			cc++;
			ccS = 0;
		}else{
			ccS++;
			cc = 0;
		}
		i++;
	}

//	printf("grNum= %d\n",grNum);

	ccD = 0;ccS = 0;ccD= 0;
	cc = 0;
	i = iSt;
	while(i<iStop){
		if(resultsStr[i] > 32) {
			if(cc == 0) grIdx++;
			cc++;
			ccS = 0;
		}else{
			if(ccS == 0){
				if(ccD < cc && grIdx > 2 && grIdx < grNum-1) ccD = cc;
			}
			ccS++;
			cc = 0;
		}
		i++;
	}
	return ccD;
}

int maxDigitGroup1(char *resultsStr, int iStart, int iStop){
	int i, len, iSt;
	int cc, ccD, ccS;


	len = iStop-iStart-1;
	if(len < 5) return 0;
	iSt = iStart;
	if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt += 2;

	ccD = 0;ccS = 0;ccD= 0;
	cc = 0;
	i = iSt;
	while(i<iStop){
		if(resultsStr[i] > 32) {
			cc++;
			ccS = 0;
		}else{
			if(ccS == 0){
				if(ccD < cc) ccD = cc;
			}
			ccS++;
			cc = 0;
		}
		i++;
	}
	return ccD;
}

void cleanTicketNumbers_TwoStep(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, numGrCorr, i, k;
	 int iStart, iStop, iStartTmp;
	 char strT[10];
//	 int ret;
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
//		 if(ticketTypeInt == 3){
			 if(ticketTypeInt == 4){
		    if(testLineConfC5_TX(lpFieldsResults, i) >= 0){
			    firstLineIdx = i;break;
		    }
		 }else{
			    if(testLineConfAllorN2_TX(lpFieldsResults, i) >= 0){
				    firstLineIdx = i;break;
			    }

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

//		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0){
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
//	    if(numGr > thGroups && strlen(resultsStr) > 15 && numChars < 20){
	    	switch(ticketTypeInt){
                   case 4: // TWO STEP
            	         testTrashBegEndM(lpFieldsResults, &iStart, &iStop);
       	               cleanAgainQP(lpFieldsResults, &iStart, &iStop);break;
                   case 1: // POWER BALL
                   case 2: // MEGA MILLIONS
         	               testTrashBegEndM(lpFieldsResults, &iStart, &iStop);
//       	              printLine(resultsStr, iStart, iStop);
         	               cleanAgainQP(lpFieldsResults, &iStart, &iStop);
         	               break;
	    	}
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
	switch(ticketTypeInt){
                      case 4: // TWO STEP
                              getFirstCharFromNumLineTS(&iStart, &iStop);break;
                      case 1: // POWER BALL
                      case 2: // MEGA MILLIONS
 	                          getFirstCharFromNumLineM(&iStart, &iStop);break;

/*
   	                   case 3: // TWO STEP
   	                       getFirstCharFromNumLineTS(&iStart, &iStop);break;
                      case 5: // POWER BALL
                      case 6: // MEGA MILLIONS
                    	  getFirstCharFromNumLineM(&iStart, &iStop);break;
*/
	}
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
void cleanEndOfLine(){
	int len = strlen(result);
	int n , n1 = len-1;
	if( len < 4) return;
	n = n1;
	if(result[n] < 32)
		n--;
//  printf ("---------n= %d, result[n]= %c, result[n-1]= %c\n",n, result[n], result[n-1]);
    if(result[n] == '1' && result[n-1] == 32){
    	if(n != n1) {
    	   result[n-1] = result[n1];
    	   result[n] = 0;
    	}
    	else
    	   result[n-1] = 0;
//  printf ("---------result= %s\n",result);
     }
}
int getNumberOfGroupsFromOut(char* results ){
	   int i, cc, len;
	   len = strlen(results);
	   cc = 0;
	   for(i=0;i<len;i++){
		   if(results[i] == 32) cc++;
	   }
       return cc;
}
void checkAllSpacesLineTX(int *numG){
    int len = strlen(result);
    int i, ccD, nG;

    nG = *numG;
    if(len < 4) return;
    if(result[0] != 32 && result[1] != 32 && result[2] != 32 && result[3] == 32){
 	   stretchResults(1, 1);
 	   insResult(1, " ");
 	   nG++;
    }
    i=0;
    while(i<len){ if(result[i] == 32) break; i++;}
    ccD = 0;
    while(i<len){
      if(result[i] != 32) ccD++;
      else {
    	  if(ccD == 4){
    	 	   stretchResults(i-2, 1);
    	 	   insResult(i-2, " ");
    	 	   nG++;
    	 	   i++;
    	  }
    	  ccD = 0;
      }
      i++;
    }
    *numG = nG;
}
void checkAllSpacesLine_1TX(int *numG){
    int len = strlen(result);
    int i, ccD, nG;

    nG = *numG;
    if(len < 4) return;
    i=0;
    while(i<len){ if(result[i] == 32) break; i++;}
    ccD = 0;
    while(i<len){
      if(result[i] != 32) ccD++;
      else {
    	  if(ccD == 4){
    	 	   stretchResults(i-2, 1);
    	 	   insResult(i-2, " ");
    	 	   nG++;
    	 	   i++;
    	  }
    	  ccD = 0;
      }
      i++;
    }
    *numG = nG;
}
int testLineConfAllorN_TX(LPFIELDSRESULTS lpFieldsResults, int line){
	 int cnf = 0, k;
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 int ccD, ccS, ccD1, ccS1, ccD2, ccS2;

	 k = line;
	 if(k>0){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) return cnf;
	        testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
	        if(ccS < 4) cnf=-1;
	        if(cnf==0 && k>2){
	        	if(getLine(resultsStr, k-1, &iStart, &iStop) < 0) return cnf;
	        	testGroupsTX(resultsStr, iStart, iStop, &ccD1, &ccS1);
	        	if(getLine(resultsStr, k-2, &iStart, &iStop) < 0) return cnf;
	        	testGroupsTX(resultsStr, iStart, iStop, &ccD2, &ccS2);
	        	if(ccS1<4 && ccS2<4 ) cnf=-1;
	        }
	 }

	return cnf;
}
void modify_ccS(char *resultsStr, int iStart, int iStop, int *ccS){
int i, k, len, iSt;
int cc, ccMod;

// printLine(resultsStr, iStart, iStop);
   len = iStop-iStart-1;
   if(len < 5) return;
   i = 0; cc = 0;iSt = iStart;
   if(resultsStr[iSt] > 32 && resultsStr[iSt+1] == 32) iSt += 2;
   i = iSt;ccMod = 0;
   while(i<iStop){
		  cc = 0;
		  k=i;
//  printf(" k= %d, cc= %d, ccMod= %d\n", k, cc, ccMod);
		  while(k<iStop){
			if(resultsStr[k] <= 32) break;
			cc++;
			k++;
		  }
//  printf(" k= %d, cc= %d, ccMod= %d\n", k, cc, ccMod);
		  if(cc > 1) break;
		  ccMod++;
		  i = k+1;
//  printf(" i= %d, cc= %d, ccMod= %d\n", i, cc, ccMod);
   }
//  printf("1 ccMod= %d\n", ccMod);
   i = iStop;
   while(i>iStart){
	   if(resultsStr[i] > 32) break;
	   i--;
   }
   while(i>iStart){
		  cc = 0;
		  k=i;
		  while(k>iStart){
			if(resultsStr[k] <= 32) break;
			cc++;
			k--;
		  }
		  if(cc > 1) break;
		  ccMod++;
		  i = k-1;
   }
//   printf("2 ccMod= %d\n", ccMod);
   if(ccMod>0 && ccMod<5) *ccS = ((*ccS) - ccMod);

}
int getCnfGroups_2FromLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
    char *resultsStr = lpFieldsResults->result;
	int i, ccG, ccGgr;
	int cnf = 0,  thCnf = 30;
	ccGgr = 0;
	ccG = 0;
//	len = strlen(resultsStr);
	for(i=iStart+1;i<iStop;i++){
		if(resultsStr[i] > 32) {
			if(lpFieldsResults->flConf[i] > thCnf)  cnf++;
			ccGgr++;}
		else{
			if(ccGgr == 2 && cnf == 2) ccG++;
			 ccGgr = 0;
			 cnf = 0;
		}
	}
	if(ccGgr == 2 && cnf == 2) ccG++;
    return ccG;
}

int testLineConfAllorN2_TX(LPFIELDSRESULTS lpFieldsResults, int line){
	 int cnf = 0;
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 int ccD, ccS, gr_2;
	 int thD = 9, thS = 9;


		 if(getLine(resultsStr, line, &iStart, &iStop) < 0) return cnf;
	     testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);


//      printf("1 line= %d, ccD = %d, ccS= %d\n", line, ccD, ccS);

	     modify_ccS(resultsStr, iStart, iStop, &ccS);

	     gr_2 = getCnfGroups_2FromLine(lpFieldsResults, iStart, iStop);


//      printf("2 line= %d, ccD = %d, ccS= %d, ----gr_2= %d\n", line, ccD, ccS, gr_2);
	     if(gr_2 > 4) {
            thD = 10; thS = 10;
	     }

	     if(ccD < 4 || ccS < 4) cnf = -1;
	     if(ccD > thD || ccS > thS) cnf = -1;


	return cnf;
}
int testLineConfC5_TX(LPFIELDSRESULTS lpFieldsResults, int line){
	 int cnf = 0;
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 int ccD, ccS ;


		 if(getLine(resultsStr, line, &iStart, &iStop) < 0) return cnf;
	     testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
	     if(ccD < 3 || ccS < 3) cnf = -1;
	     if(ccD > 8 || ccS > 8) cnf = -1;
//            printf(" line= %d, ccD = %d, ccS= %d\n", line, ccD, ccS);

	return cnf;
}
int testLineConfC5_L_TX(LPFIELDSRESULTS lpFieldsResults, int line, int sTh){
	 int cnf = 0;
	 char *resultsStr = lpFieldsResults->result;
	 int iStart, iStop;
	 int ccD, ccS ;


		 if(getLine(resultsStr, line, &iStart, &iStop) < 0) return cnf;
	     testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
	     if(ccD < 3 || ccS < 3) cnf = -1;
	     if(ccD > 8 || ccS > sTh) cnf = -1;
//            printf(" line= %d, ccD = %d, ccS= %d\n", line, ccD, ccS);

	return cnf;
}


void getFirstCharFromNumLineAllOrN(int* iStart, int* iStop, int nGrp){
	int i, k, st = 1;
	int len = strlen(result);
	*iStart = 0;
	 k = -1;

	 if(nGrp > 5 && len > 12) {
		 int avSp = 0, cc = 0, lSp, dd;
		 for(i=4;i<12;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			avSp /= cc;
		    lSp = 0;
		    for(i=len-1;i>0;i--){
			   if(result[i] == 32) {lSp= i;break;}
		    }
	       if(lSp > 0){
	    	   dd = flQuad[lSp+1].p1X -flQuad[lSp-1].p2X;
//	    	   printf(" dd= %d, avSp= %d\n",dd, avSp);
		     if(dd > 2*avSp) nGrp--;
		     if(nGrp > 4){
		    	lSp = 0;
			    for(i=0;i<5;i++){
				   if(result[i] == 32) {lSp= i;break;}
			    }
			    if(lSp> 0) lSp--;
			    *iStop = lSp;return;
		     }
		   }
		}
	 }


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
	*iStop = k;
}
int getNumberOfBGroups(char* results, int iStart, int iStop){
	   int i, cc, ccBig;
	   cc = 0;ccBig = 0;
	   for(i=iStart;i<iStop;i++){
		   if(results[i] > 32) {
			   cc++;
		   }
		   if(results[i] == 32) {
			   if(cc>2) ccBig++;
			   cc = 0;
		   }
	   }
       return ccBig;
}
int cutLastChar_LTX(int nGrp){
	int i,  ccBig;
	int len = strlen(result);
	ccBig = getNumberOfBGroups(result, 2, len);
//	printf("nGrp = %d, ccBig= %d, len= %d\n",nGrp,ccBig,len);
	nGrp += ccBig;

    if(nGrp > 7 && len > 12) {
		 int avSp = 0, cc = 0, lSp, dd, lNSp = 0;
		 int avSp1 = 0;
		 for(i=4;i<12;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			avSp /= cc;
			avSp1 = (int)(1.75*(float)avSp);
		    for(i=len-1;i>0;i--){
			   if(result[i] > 32) {lNSp= i;break;}
		    }
		    if(lNSp < 12) return nGrp;
		    cc = 0;
		    for(i=lNSp;i>0;i--){
			   if(result[i] == 32) {
				   lSp= i;
				   if(result[lSp-1] > 32){
		    	       dd = flQuad[lSp+1].p1X -flQuad[lSp-1].p2X;
		    	       if(result[lSp-2] > 32){
		    	           if(dd > 2*avSp) {result[lSp] = '\r';result[lSp+1] = 0;nGrp--;}
		    	       }
	    	           else{
	    	        	   if(dd > avSp1) {result[lSp] = '\r';result[lSp+1] = 0;nGrp--;}
	    	           }
				   }
				   cc++;
			   }
			   if(cc > 2) break;
		    }
        }
    }
    return nGrp;

}
void getFirstCharFromNumLineC5_LTX(int* iStart, int* iStop, int nGrp){
	int i, fNSp, cc1, cc2, ccSp;
	int idx1, idx2;
	int len = strlen(result);

	*iStart = 0;

//printf("nGrp = %d, len= %d\n",nGrp,len);
//    for(i=0;i<5;i++){
//    	printf("result[i]= %c, flConf[i] = %d\n",result[i], flConf[i]);
//    }
  if(nGrp>6){
	    fNSp = 0;
	    for(i=0;i<10;i++){
		   if(result[i] > 32) {fNSp= i;break;}
	    }
	    cc1 = 0;cc2 = 0;ccSp = 0;
	    idx1 = 0, idx2 = 0;
	    for(i=fNSp;i<len;i++){
	    	if(result[i] > 32) {
	    		if(ccSp==0) cc1++;
	    		if(ccSp==1) cc2++;
	    	}
		   if(result[i] == 32) {
			   if(ccSp == 0) idx1 = i;
			   if(ccSp == 1) idx2 = i;
               ccSp++;
               if(ccSp>1) break;
		   }
	    }
//    printf("cc1 = %d, idx1= %d, cc2= %d, idx2= %d\n",cc1,idx1,cc2,idx2);
    if(cc2 == 1) {
    	 if(idx2 == 0) idx2++;
    	*iStop = idx2-1;
    }
    else{
 	   if(idx1 == 0) idx1++;
   	   *iStop = idx1-1;
    }
  }
//	 if(result[1] != 32) st = 0;
//	for(i=st;i<5;i++){
//  printf("--- char: i= %d; result[i]= %c, flConf[i]= %d\n",i, result[i], flConf[i]);
//       if(result[i] != 32 && flConf[i]> thConf-15) {
//    	   k = i-st;
//    	   break;
//       }
//	}
//	if(k> 0 && result[k] == 32)
//		k--;
//	*iStop = k;
}

void getFirstCharFromNumLineC5(int* iStart, int* iStop, int nGrp){
	int i, k, st = 1;
	int len = strlen(result);

	*iStart = 0;
	 k = -1;


	 if(nGrp > 4 && len > 12) {
		 int avSp = 0, cc = 0, lSp, dd, lNSp;
		 for(i=4;i<12;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			avSp /= cc;
		    lSp = 0;
		    lNSp = 0;
		    for(i=len-1;i>0;i--){
			   if(result[i] > 32) {lNSp= i;break;}
		    }
		    for(i=lNSp;i>0;i--){
			   if(result[i] == 32) {lSp= i;break;}
		    }
//printf(" len= %d, lSp= %d, result= %s\n",len, lSp, result);

	       if(lSp > 0){
//  	   printf(" result[lSp+1]= %c, result[lSp-1]= %c, flQuad[lSp+1].p1X= %d, flQuad[lSp-1].p2X= %d\n",result[lSp+1], result[lSp-1], flQuad[lSp+1].p1X, flQuad[lSp-1].p2X);
	    	   dd = flQuad[lSp+1].p1X -flQuad[lSp-1].p2X;
//   	   printf(" dd= %d, avSp= %d, nGrp= %d\n",dd, avSp, nGrp);
		     if(dd > 2*avSp) nGrp--;
		     if(nGrp > 3){
		    	lSp = 0;
			    for(i=0;i<5;i++){
				   if(result[i] == 32) {lSp= i;break;}
			    }
			    if(lSp> 0) lSp--;
			    *iStop = lSp;return;
		     }
		   }
		}
	 }

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
	*iStop = k;
}
void getFirstCharFromNumLineTS(int* iStart, int* iStop){
	int i, k, st = 1;
	int len = strlen(result);
	int nGrp;
    int ccSp = 0, ccSpC = 0;
    int kk, stopTmp;

	*iStart = 0;
	 k = -1;
        stopTmp = strlen(result);
        for(kk=strlen(result);kk>0;kk--){
     	   if(result[kk] > 32) { stopTmp = kk;break;}
    	}
        for(kk=0;kk<stopTmp;kk++){
    	   if(result[kk] == 32) {
    		if(ccSpC == 0) ccSp++;
    		ccSpC++;
    	   }
    	   else ccSpC = 0;
       }
//            printf(" ccSp= %d\n",ccSp);
        nGrp = ccSp;

//   printf(" result= %s\n",result);
//   printf(" nGrp= %d\n",nGrp);

	 if(nGrp > 4 && len > 12) {
		 int avSp = 0, cc = 0, lSp, dd, lNSp;
		 for(i=4;i<12;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			avSp /= cc;
		    lSp = 0;
		    lNSp = 0;
		    for(i=len-1;i>0;i--){
			   if(result[i] > 32) {lNSp= i;break;}
		    }
		    for(i=lNSp;i>0;i--){
			   if(result[i] == 32) {lSp= i;break;}
		    }
//printf(" len= %d, lSp= %d, result= %s\n",len, lSp, result);

	       if(lSp > 0){
//  	   printf(" result[lSp+1]= %c, result[lSp+1]= %c, flQuad[lSp+1].p1X= %d, flQuad[lSp-1].p2X= %d\n",result[lSp+1], result[lSp-1], flQuad[lSp+1].p1X, flQuad[lSp-1].p2X);
	    	   dd = flQuad[lSp+1].p1X -flQuad[lSp-1].p2X;
//   	   printf(" dd= %d, avSp= %d, nGrp= %d\n",dd, avSp, nGrp);
		     if(dd > 2*avSp) nGrp--;
		     if(nGrp > 3){
		    	lSp = 0;
			    for(i=0;i<5;i++){
				   if(result[i] == 32) {lSp= i;break;}
			    }
			    if(lSp> 0) lSp--;
			    *iStop = lSp;return;
		     }
		   }
		}
	 }

//  printf(" result= %s\n",result);

	 if(result[1] != 32) st = 0;
	for(i=st;i<5;i++){
//  printf("--- char: i= %d; result[i]= %c, flConf[i]= %d, thConf= %d\n",i, result[i], flConf[i], thConf);
       if(result[i] != 32 && flConf[i]> thConf-15) {
    	   k = i-st;
    	   break;
       }
	}
//    printf("------------------ k= %d\n",k);
	{
	  int gr_2 = getGroups_2FromLineResult();
//	  printf("------------------ gr_2= %d\n",gr_2);
	  if(gr_2 == 5 && k == 1 && result[0]>32) k = 0;
	}

//  printf("------------------ k= %d\n",k);
	if(k> 0 && result[k] == 32)
		k--;
//  printf("------------------ k= %d\n",k);
	*iStop = k;
}

void getFirstCharFromNumLineM(int* iStart, int* iStop ){
	int i, k;
	int len = strlen(result);
	int nGrp, stt, dd;

	*iStart = 0;
	 k = 0;
	 stt = 0;
	 for(i=0;i<len;i++){
		   if(result[i] == 32) {stt = i;break;}
	 }
	 stt++;
	 if(len > stt+10) {
		 int avSp = 0, cc = 0;
		 for(i=stt;i<stt+10;i++){
			   if(result[i] == 32) {
				   avSp += flQuad[i+1].p1X -flQuad[i-1].p2X;
				   cc++;
		       }
		 }
		 if(cc>0){
			avSp /= cc;
			nGrp = 0;
		    for(i=stt;i<len;i++){
		    	  if(result[i] == 32) {
			    	  dd = flQuad[i+1].p1X -flQuad[i-1].p2X;
//   	  printf(" i = %d, result[i-1]= %c, result[i+1]= %c, dd= %d, 2*avSp= %d\n", i, result[i-1], result[i+1], dd, 2*avSp);
				      if(dd > 2*avSp) break;
		    		  nGrp++;
		    	  }
			}
//printf("nGrp = %d, stt= %d, k= %d\n", nGrp, stt, k);
			if(nGrp == 4) {
				k = stt-1;
				if(k > 0) k--;
			}
		if(nGrp > 4) {
			 for(i=stt;i<len;i++){
			   if(result[i] == 32) {stt = i;break;}
			 }
			 k = stt-1;
		}
	  }
	 }
// printf(" k= %d\n",  k);
	 *iStop = k;
}

void cleanTicketNumbers_AllorN(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, i, k, kk, len;
	 int iStart, iStop;
	 char strT[10];
	 int numLines = 0;

	 int thGroups = 5;
	 int numChars;
	 int beginCut = 0;
	 int firstLineIdx = 0;
	 resultTmp[0] = 0;

// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
	 // add spaces !!!
	 	 addSpacesToNumbersTX(img, lpFieldsResults);
	 	 resultTmp[0] = 0;

	 i = 0;
	 firstLineIdx = 0;
	 while(i<numLines){
		 if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		 if(testLineConfAllorN2_TX(lpFieldsResults, i) >= 0){
			 firstLineIdx = i;break;
		 }
		 i++;
	 }
     if(firstLineIdx>0){
        for(k=0;k<i;k++)
          lpFieldsResults->lineQuadStatus[k] = 0;
     }


	 k = numLines-1;
	 while(k>firstLineIdx){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0 || testLineConfAllorN_TX(lpFieldsResults, k) < 0){
			 lpFieldsResults->lineQuadStatus[k] = 0;
			 numLines--;
		 }
		 else
			 break;
		 k--;
	 }

	 i = firstLineIdx;
	 k = 0;
	 kk = 0;

//printf ("--------------------------------------------numLines= %d, \n",numLines);
	 while(i < numLines){
//printf ("---------i=% d, k= %d\n",i,k);
		 if(k >= 20) break;
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//	printLine(resultsStr, iStart, iStop);
         testLeftSideForLine(resultsStr, &iStart, iStop);
//	printLine(resultsStr, iStart, iStop);
	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
//printf ("---------i=% d, numGr= %d\n",i,numGr);

	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
        	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
            if(beginCut > 0)
            	numGr++;
        }
        len = strlen(resultsStr);
//    thGroups = 5;
    thGroups = 4;
//    if( k%2 == 1) thGroups = 4;
    { int gr_2 = getGroups_2FromLine(resultsStr, iStart, iStop);
//      int gr_1 = getGroups_1FromLine(resultsStr, iStart, iStop);
//      printf ("---------gr_1=% d, gr_2= %d\n",gr_1, gr_2);
      if(gr_2 < 3)
    	 numGr = 0;

    }
//   	printLine(resultsStr, iStart, iStop);
//   printf ("---------i=% d, numGr= %d, thGroups= %d, len = %d, numChars= %d\n",i,numGr,thGroups, len, numChars);
    if(numGr > thGroups && len > 11 && numChars < 25){
//  	 printLine(resultsStr, iStart, iStop);

	    	testTrashBegEnd(lpFieldsResults, &iStart, &iStop);
// printLine(resultsStr, iStart, iStop);
           copyLine(lpFieldsResults, iStart, iStop);

// printf ("0---------i = %d, result= %s\n",i,  result);
           if((k%2==0 && numGr > 7) || (k%2==1 && numGr > 5)) {
       	       cleanEndOfLine();
           }
           if((k%2==0 && numGr > 4)) {
       	       checkAllSpacesLineTX(&numGr);
           }
           if((k%2==1 && numGr > 4)) {
       	       checkAllSpacesLine_1TX(&numGr);
           }
//        printf ("---------result= %s\n",result);
//           printf(" beginCut= %d, numGr= %d\n",beginCut, numGr);

	       if(beginCut > 0){
               strT[0] = 'A'+kk;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
//              getFirstCharFromNumLineAllOrN(&iStart, &iStop, numGr);
        {
         int ccSp = 0, ccSpC = 0;
         int kk, stopTmp;
            stopTmp = strlen(result);
            for(kk=strlen(result);kk>0;kk--){
         	   if(result[kk] > 32) { stopTmp = kk;break;}
        	}
            for(kk=0;kk<stopTmp;kk++){
        	   if(result[kk] == 32) {
        		if(ccSpC == 0) ccSp++;
        		ccSpC++;
        	   }
        	   else ccSpC = 0;
           }
//            printf(" ccSp= %d\n",ccSp);
            numGr = ccSp;
        }
        getFirstCharFromNumLineAllOrN(&iStart, &iStop, numGr);
//        printf ("---------result= %s\n",result);
//      printf(" iStart= %d, iStop= %d\n",iStart, iStop);

              if((numGr < 8 && iStart == 0 && iStop == 0 && result[1] != 32) ||
            		  k%2==1) {
                 strT[0] = 'A'+kk;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
             }else{
                 strT[0] = 'A'+kk;
                 strT[1] = 0;
                replaceChars(iStart, iStop, strT);
             }
	       }
//        printf ("---------result= %s\n",result);
           iStop = -1;
//           if( k%2 == 0 && numGr > 5) {
//        	   if(getNumberOfGroupsFromOut(result) > 6){
//                 if(getSecondQPFromNumLine(&iStart, &iStop) >=0){
//                    replaceChars(iStart, iStop, "QP");
//                 }
//              }
//           }

           cleanOneDigit_AllOrN();
	       addLineToTmp();
       	   k++;
           if( k%2 == 0) kk++;
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
	    }
	    i++;
	 }
	 copyResultsBack(lpFieldsResults);
}
void testLeftSideForLineTXC5(LPFIELDSRESULTS lpFieldsResults, int *iStart, int iStop){
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
		if(dd > 120) {*iStart = cc+1;return;}
//		printf ("---------dd= %d\n",dd);
	}

/*
	if(resultsStr[isT] == '1' && resultsStr[isT+1] == 32) {
		if(resultsStr[isT+2] != 32 && resultsStr[isT+3] == 32 && resultsStr[isT+5] == 32) {
			cc = 0;
			for(i=isT+6;i<iStop;i++){
				if(resultsStr[i] == 32) cc++;
			}
			if(cc < 5){
//	printf ("&---------isT+3= %d\n",isT+3);
				resultsStr[isT+3] = '&';
			}
		}
		isT += 2;
	}
*/
	*iStart = isT;

}
void filtration(char ch){
	int i, len;

	   len = strlen(result);
	   for(i=0;i<len;i++){
		   if(result[i] == ch) {
			   shrinkResults(i, i);break;
		   }
	   }

}
void testTrashBeg_3Chars(LPIMGDATA img){
//printf("--- result= %s\n", result);
//printf("--- result[0]= %c, result[1]= %c, result[2]= %c\n",  result[0], result[1], result[2]);
//	char buffLog[500];

	 if(result[0] > 32 && result[1] > 32 && result[2] > 32){
		 shrinkResults(1,1 );
	 }
	 if(result[0] > 32 && result[1] == 32 && result[2] > 32){
		 int x1, x2, y1, y2, yy, size;
		 int byteWidthHist = ((img -> bi.biWidth + 31)/32)*4;
		 int i, massT, massV;

        x1 = flQuad[0].p1X;x2 = flQuad[0].p2X;
        y1 = flQuad[0].p1Y;y2 = flQuad[0].p3Y;
	    size = x2-x1+1;
	    if(size >=H_SIZE_PREPROC) return;
	    if(x1 < 0 || x2 < 0) return;
	    if(x1 >= img -> bi.biWidth || x2 >= img -> bi.biWidth) return;
	    if(y1 < 0 || y2 < 0) return;
	    if(y1 >= img -> bi.biHeight || y2 >= img -> bi.biHeight) return;
//   printf("------byteWidthHist= %d, img -> bi.biHeight= %d, size= %d, x1= %d, x2= %d, y1= %d, y2= %d\n", byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2);
		//   fflush(stdout);
	     if(size > 5) {
		      histVPtrPix(img ->pixPtr, byteWidthHist, img -> bi.biHeight, size, x1, x2, y1, y2, vHistPreProc);

	     }
	     massT = 0; massV = 0;
	     for(i=1;i<vHistPreProc[0];i++){
	    	 if(vHistPreProc[i] > 0) {massV++;massT += vHistPreProc[i];}
	     }
	     yy = y2-y1;
//	 	for(i=0;i<vHistPreProc[0];i++) {
//	        sprintf(buffLog," vHistPreProc[ %d ]= %d\n", i, vHistPreProc[i]);
//	        saveLog("", buffLog);
//	 	}
//	     printf("------massV= %d, massT= %d\n", massV, massT);
	     if(massV < size/4 && massT < 2*yy)
	    	 shrinkResults(0,1);
	 }
}
void testfirst2Groups(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int avY){
// check order at the beginning
	char *resultsStr = lpFieldsResults->result;
	char str1[10], str2[10];
	int k1, k2, firstS, firstE;
	int n1, n2, i, n, yy;

//	printLine(resultsStr, iStart, iStop);
		  k1 = iStart;
	      if(resultsStr[k1] > 32 && resultsStr[k1+1] == 32)
	    	  k1++;
	      k2 = k1;
	      for(i=k2;i<iStop;i++){
		      if(resultsStr[i] > 32) {k1=i;break;}
	      }
	      k2 = k1;
	      for(i=k1;i<iStop;i++){
		      if(resultsStr[i] == 32) {k2=i;break;}
	      }
//     printf("k2-k1= %d, k1= %d, k2= %d\n",k2-k1, k1, k2);
//    printf("k1= %d, resultsStr[k1] = %c, k2= %d, resultsStr[k2] = %c\n",k1-iStart, resultsStr[k1], k2-iStart, resultsStr[k2]);
          if(k2-k1 > 2){
        	  int maxY = 0, iMaxY = -1;
        	  for(i=k1;i<k2;i++){
        	     yy = lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y;
//    	     printf("resultsStr[i]= %c, maxY= %d, yy= %d, avY= %d, avY-yy= %d\n",resultsStr[i], maxY, yy, avY, avY-yy);
        	     if(maxY < abs(avY-yy)) {maxY = abs(avY-yy); iMaxY = i;}
        	  }
        	  if(iMaxY >= 0) resultsStr[iMaxY] = ' ';
          }

// printLine(resultsStr, iStart, iStop);
	      n = 0;
	      for(i=k1;i<k2;i++){
	    	  if(resultsStr[i] > 32)
	    	    str1[n++] = resultsStr[i];
	      }
	      str1[n] = 0;
	      firstS = k1; firstE = k2-1;

		  k1 = k2;
	      for(i=k2;i<iStop;i++){
		      if(resultsStr[i] > 32) {k1=i;break;}
	      }
	      k2 = k1;
	      for(i=k1;i<iStop;i++){
		      if(resultsStr[i] == 32) {k2=i;break;}
	      }
	      n = 0;
	      for(i=k1;i<k2;i++){
	    	  str2[n++] = resultsStr[i];
	      }
	      str2[n] = 0;
	      sscanf(str1,"%d",&n1);sscanf(str2,"%d",&n2);
//	printf("str1= %s, n1= %d, str2= %s, n2= %d\n", str1, n1, str2, n2);
	      if(n1 > n2 || n1 == 0){
	// delete first group
		      for(i=firstS;i<=firstE;i++){
			     if( resultsStr[i] > 32)
			        resultsStr[i] = 32;
			     }
		      return;
	      }

}
void cleanAgainQP(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop){
	 char *resultsStr = lpFieldsResults->result;
	 int i, cc, ccN, cc0, k, st;
	 int max1, max2;

//  printLine(resultsStr, *iStart, *iStop);
	 st = *iStop;
	 if(resultsStr[*iStop]<32) st--;
	   cc = 0; ccN = 0; cc0 = 0;
	   for(i=st;i>*iStart;i--){
// printf("resultsStr[ %d ] = %c\n",i, resultsStr[i]);
	    	if(resultsStr[i] == 32) {
	    		if(ccN>0){
	    		   if(ccN == cc0 && cc0 == 2){
	    			   resultsStr[i+1] = ' ';
	    			   resultsStr[i+2] = ' ';
	    		   }
//printf("ccN= %d, cc0= %d, cc= %d\n",ccN, cc0, cc);
	    		   if(ccN > cc0 && cc0 == 2){
	    			  max1 = 0;
	    			  for(k=i+1;k<=i+ccN; k++)
	    				if(max1 < lpFieldsResults->flConf[k]) max1 = lpFieldsResults->flConf[k];
	    			  max2 = 0;
	    			  for(k=i+1;k<=i+ccN; k++)
	    				if(max2 < lpFieldsResults->flConf[k] && lpFieldsResults->flConf[k] < max1) max2 = lpFieldsResults->flConf[k];

// 			  printf("max1= %d, max2= %d\n",max1, max2);
		    		   if(max2 < 30){
			    			  for(k=i+1;k<=i+ccN; k++){
		    			         resultsStr[k] = ' ';
			    			  }
		    		   }

	    		   }
// 	 printf ("2. iStop= %d, resultsStr[iStop]= %d\n", iStop, resultsStr[iStop]);
//printf("ccN= %d, cc0= %d, cc= %d\n",ccN, cc0, cc);
//if(cc0 == 2){
//printf("resultsStr[i+1]= %c, resultsStr[i+2]= %c, resultsStr[i+3]= %c, resultsStr[i+4]= %c\n",resultsStr[i+1], resultsStr[i+2], resultsStr[i+3], resultsStr[i+4]);
//printf("cnf[i+1]= %d, cnf[i+2]= %d, cnf[i+3]= %d, cnf[i+4]= %d\n",lpFieldsResults->flConf[i+1], lpFieldsResults->flConf[i+2], lpFieldsResults->flConf[i+3], lpFieldsResults->flConf[i+4]);
//}
    		     cc++;
	    		}
	    		ccN = 0;
	    		cc0 = 0;
	    		if(cc > 3) break;
	    	}
	    	else{
	    		ccN++;
	    		if(resultsStr[i] == '0') cc0++;
	    	}
	   }


}
void testTrashBegEndM(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop){
	 char *resultsStr = lpFieldsResults->result;
	 int i, k, th, sizeT;
     int xxT[10], xxIT[10];
     int yyT[10], yyIT[10];
	 int xx1, xx2, ccSp, ccSpT, thGroups;
	 int yy1, yy2;
	 int kStartX, kStartY;
	 int xx, yy, avX, avY;
     int ccSpC, stopTmp, cc;
     int fSp, fSp2, ccfSp;

       fSp = *iStart+1;
//	  printLine(resultsStr, *iStart, *iStop);
	    ccSpT = 0;
	    for(i=*iStart;i<*iStop;i++){
	    	if(resultsStr[i] == 32) ccSpT++;
	    }
	    sizeT = ccSpT-3;

//	    printf("--- ccSpT= %d\n",ccSpT);

	    for(i=*iStart+1;i<*iStop;i++){
	    	if(resultsStr[i] == 32) {fSp = i;break;}
	    }
// 	printf("---fSp= %d\n",fSp);
 	    ccfSp = 1;
	    fSp2 = fSp;
	    if(ccSpT > 9){
		    for(i=fSp+1;i<*iStop;i++){
		    	if(resultsStr[i] == 32) {fSp2 = i;break;}
		    }
		    if(fSp2 > fSp){
		    	xx1 = lpFieldsResults->flQuad[fSp+1].p1X-lpFieldsResults->flQuad[fSp-1].p2X;
		    	xx2 = lpFieldsResults->flQuad[fSp2+1].p1X-lpFieldsResults->flQuad[fSp2-1].p2X;
//  	printf("---xx1= %d, xx2= %d\n",xx1, xx2);
  	             if(xx2-xx1 > 20) {fSp = fSp2;ccfSp= 2;}
		    }
	    }
	    fSp++;
//  	printf("---fSp= %d\n",fSp);
	    ccfSp = 0;
	    for(i=fSp;i<*iStop;i++){
	    	if(resultsStr[i] == 32) break;
	    	ccfSp++;
	    }
	    if(ccfSp > 3) {
	    	fSp += ccfSp;
		    for(i=fSp;i<*iStop;i++){
		    	if(resultsStr[i] > 32) {fSp = i;break;}
		    }
	    }
	    if(*iStop - fSp < 12) fSp = *iStart+1;
//printf("---fSp= %d, *iStart= %d\n",fSp, *iStart);

	    if(sizeT >= 10) sizeT = 9;
	       avX = 0;
	       avY = 0;
			k = 0;
    	    for(i=fSp;i<*iStop;i++){
    	       if(resultsStr[i] == 32){
    	    	    xxT[k]= lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
//	    printf("--- xxT[ %d ]= %d, resultsStr[i-1]= %c, resultsStr[i+1]= %c\n",k,xxT[k], resultsStr[i-1], resultsStr[i+1]);
		    	    xxIT[k] = i;
		    	    k++;
		    	    if(k > sizeT) break;
		        }
			}
//		    for(i=0;i<k;i++){
//	             printf("--- xxT[ %d ]= %d, xxIT[i] = %d\n",i,xxT[i], xxIT[i]);
//		    }

		    kStartX = 1;
		    for(i=1;i<k-1;i++){
		    	xx1 = xxT[i] - xxT[i-1];
		    	xx2 = xxT[i+1] - xxT[i];
//  	printf("---xx1= %d, xx2= %d, xx1-xx2= %d\n",xx1, xx2, xx1-xx2);
		    	if(abs(xx1) < 10 && abs(xx2) < 10) {
		    		kStartX = xxIT[i];avX = xxT[i];break;
		    	}
		    }
	       if(avX == 0) {
	    	   cc = 0;
			    for(i=2;i<k-2;i++){
			    	if(xxT[i] > 10 && xxT[i] < 50){
					    avX += xxT[i];
					    cc++;
			    	}
			    }
			    if(cc>0)
			    	avX /= cc;
		    	kStartX = 2;
		    }
	       if(avX == 0) {kStartX = 2; avX = 25;}
//   printf("---avX= %d, kStartX= %d\n",avX, kStartX);
			k = 0;
//   printf("startSp= %d, *iStop= %d\n", startSp, *iStop);

    	    for(i=fSp;i<*iStop;i++){

//  	printf("---i= %d, resultsStr[i]= %c\n",i, resultsStr[i]);

     	       if(resultsStr[i] > 32){
		    	   yyT[k] = lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y;
		    	   yyIT[k] = i;
		    	   k++;
		    	   if(k > sizeT) break;
		       }
			}

//    printf("---k= %d, sizeT= %d\n",k, sizeT);
//	 for(i=0;i<k;i++){
//	 printf("--- yyT[ %d ]= %d,yyIT[i]= %d\n",i,yyT[i], yyIT[i]);
//		    }

		    kStartY = 1;
		    for(i=1;i<k-1;i++){
		    	yy1 = yyT[i] - yyT[i-1];
		    	yy2 = yyT[i+1] - yyT[i];
		    	if(abs(yy1) < 10 && abs(yy2) < 10) {
		    		kStartY = yyIT[i];avY = yyT[i];break;
		    	}
		    }
//  printf("---avY= %d, kStartY= %d, fSp= %d\n",avY, kStartY, fSp-*iStart);
            if(avY == 0) {
            	int minYv, maxYv, ccMin, ccMax, i_ccMin, i_ccMax, cc_av;
            	minYv = 1000; maxYv = 0;
            	i_ccMin = -1; i_ccMax = -1;
    		    for(i=0;i<k;i++){
    		    	if(minYv > yyT[i]) {minYv = yyT[i];i_ccMin = i;}
    		    	if(maxYv < yyT[i]) {maxYv = yyT[i];i_ccMax = i;}
    		    }
    		    if(i_ccMin >= 0 && i_ccMax >= 0) {
    		    	ccMin = 0; ccMax = 0;
    		       for(i=0;i<k;i++){
    		    	  ccMin += (yyT[i]-minYv);
    		    	  ccMax += (maxYv - yyT[i]);
    		       }

//    printf("---ccMin= %d, ccMax= %d\n",ccMin, ccMax);

    		       if(ccMin - ccMax > 10){
    		    	  yyT[i_ccMin] = -1;
    		    	  avY = 0;cc_av = 0;
     		          for(i=0;i<k;i++){
     		        	if(yyT[i] > 0){
     		        	   avY += yyT[i];cc_av++;
     		        	}
     		          }
     		          if(cc_av > 0) avY /= cc_av;
    		       }
    		    }
            }
//    printf("---avY= %d, kStartY= %d\n",avY, kStartY);
		    if(avY == 0) {
			   	i = k/2;
			   	kStartY = yyIT[i];avY = yyT[i];
			}


//   printf("---avY= %d, kStartY= %d\n",avY, kStartY);

//printf("---avX= %d, avY= %d\n", avX, avY);

	 th = avX;
	 cc = 0;
	 for(i=*iStart;i<*iStop;i++){
	   if(resultsStr[i] == 32) {
		    cc++;
	        xx = lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
//   printf("---xx= %d, xx - avX= %d, th= %d, resultsStr[i-1]= %c, resultsStr[i+1]= %c, ccfSp= %d\n", xx, xx - avX, th, resultsStr[i-1], resultsStr[i+1], ccfSp);
	        if(xx - avX > th) *iStart = i+1;
	        else {if(cc >= ccfSp) break;}
	   }
	 }
	    ccSp = 0;ccSpC = 0;
	    for(i=*iStart;i<*iStop;i++){
	    	if(resultsStr[i] == 32) {
	    		if(ccSpC == 0) ccSp++;
	    		ccSpC++;
	    	}
	    	else ccSpC = 0;
	    }

// end of string-------------------
//	    printLine(resultsStr, *iStart, *iStop);
    { int stEnd = *iStop;
      int ccN = 0, ccCount = 0, iLastDone = -1;
      for(i=*iStop;i>*iStart;i--){
    	  if(resultsStr[i] > 32) {stEnd = i;break;}
      }
	 for(i=stEnd;i>*iStart;i--){
		 if(ccSp < 7) break;
		 if(resultsStr[i] > 32) ccN++;


	   if(resultsStr[i] == 32) {
	        xx = lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
	        yy = lpFieldsResults->flQuad[i+1].p3Y-lpFieldsResults->flQuad[i+1].p1Y;
//  printf("---xx - avX= %d, th= %d, resultsStr[i-1]= %c, resultsStr[i+1]= %c, ccSp= %d, avY- yy=%d, avY= %d, yy= %d\n", xx - avX, th, resultsStr[i-1], resultsStr[i+1], ccSp, avY- yy,avY,yy);
	        ccCount++;
	        if(xx - avX > 8*th || (xx - avX > 3*th && abs(avY-yy) > 5)) {
	        	resultsStr[i+1] = ' ';
	        	if(i+2 <=*iStop && resultsStr[i+2] > 32)
	        		resultsStr[i+2] = ' ';
	        	iLastDone = i+1;
	        	ccSp--;
	        }
	        else {
	        	if(ccN < 2 && ccCount < 5){
	        		ccN = 0;
	        		continue;
	        	}
	        	break;
	        }
	        ccN = 0;
	   }
	 }
	 if(iLastDone > 0){
		 ccN = 0;
		 for(i=stEnd;i>iLastDone;i--){
			 if(resultsStr[i] > 32) ccN++;
			 else{
				 if(ccN == 1){
					 resultsStr[i+1] = ' ';
				 }
				 ccN = 0;
			 }
		 }
	 }
    }
// end of string-------------------
// printLine(resultsStr, *iStart, *iStop);

    thGroups = 4;
 	switch(ticketTypeInt){
   	                   case 4: // TWO STEP
    	                	   thGroups = 3;break;
                       case 1: // POWER BALL
                       case 2: // MEGA MILLIONS
                    	      thGroups = 4;break;
                       case 8: // All Or Nothing
                    	      thGroups = 5;break;
/*
	                   case 3: // TWO STEP
	                	   thGroups = 3;break;
                   case 5: // POWER BALL
                   case 6: // MEGA MILLIONS
                	      thGroups = 4;break;
                   case 7: // All Or Nothing
                	      thGroups = 5;break;
*/
 	}
 	k = *iStop;
    ccSp = 0;
    for(i=kStartY;i<*iStop;i++){
    	if(resultsStr[i] == 32) ccSp++;
    	if(ccSp == thGroups) {k=i;break;}
    }
//    printf("---k= %d, kStartY= %d, *iStop= %d\n", k, kStartY, *iStop);

    xx = 0;
    for(i=k;i<*iStop;i++){
  	   if(resultsStr[i] == 32) {
  		 xx = lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
  	   }
 	   if(resultsStr[i] > 32) {
	       yy = lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y;
//printf("1--- result[ %d ]= %c, yy1= %d, yy2= %d\n",i, result[i],yy1, yy2);
//  printf("---xx - avX= %d, th= %d, avY- yy= %d, resultsStr[i]= %c\n", xx - avX, 2*avX, avY- yy, resultsStr[i]);
    	  if(xx-avX>2*avX &&  avY- yy > 5) {
//   printf("---xx - avX= %d, avY- yy= %d\n", xx - avX,avY- yy);
    		     resultsStr[i] = ' ';
    		     if(i+1 <*iStop && resultsStr[i+1] > 32) {
    		    	 yy = lpFieldsResults->flQuad[i+1].p3Y-lpFieldsResults->flQuad[i+1].p1Y;
//  printf("---xx - avX= %d, th= %d, avY- yy= %d, resultsStr[+1i]= %c\n", xx - avX, 2*avX, avY- yy, resultsStr[i+1]);
    		    	 if(avY- yy > 8) resultsStr[i+1] = ' ';
    		     }
	    	}
        }
    }
// printLine(resultsStr, *iStart, *iStop);

 	switch(ticketTypeInt){
   	                   case 4: // TWO STEP
    	                	   thGroups = 5;break;
                       case 1: // POWER BALL
                       case 2: // MEGA MILLIONS
                    	      thGroups = 6;break;
                       case 8: // All Or Nothing
                    	      thGroups = 7;break;
/*
	                   case 3: // TWO STEP
	                	   thGroups = 5;break;
                   case 5: // POWER BALL
                   case 6: // MEGA MILLIONS
                	      thGroups = 6;break;
                   case 7: // All Or Nothing
                	      thGroups = 7;break;
*/
 	}

 	{


//  printLine(resultsStr, *iStart, *iStop);

    stopTmp = *iStop;
    for(i=*iStop;i>*iStart;i--){
    	if(resultsStr[i] > 32) { stopTmp = i;break;}
   	}

    ccSp = 0;ccSpC = 0;
    for(i=*iStart;i<stopTmp;i++){
    	if(resultsStr[i] == 32) {
    		if(ccSpC == 0) ccSp++;
    		ccSpC++;
    	}
    	else ccSpC = 0;
    }

//  printf("---ccSp= %d, thGroups= %d\n", ccSp, thGroups);

    if(ccSp >thGroups) {
// check QP
     	k = *iStop;
        ccSp = 0;ccSpC = 0;
        for(i=*iStart;i<*iStop;i++){
        	if(resultsStr[i] == 32) {
        		if(ccSpC == 0) ccSp++;
        		ccSpC++;
        	}
        	else ccSpC = 0;
        	if(ccSp == 3) {k=i;break;}
        }

   	    th = avX/2;
        for(i=k;i<*iStop;i++){
      	   if(resultsStr[i] == 32) {
      		 xx = lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
      	   }
     	   if(resultsStr[i] > 32) {
    	       yy = lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y;
//printf("---i= %d, xx= %d, avX= %d, xx-avX= %d, th= %d, avY= %d, yy= %d, avY- yy= %d, resultsStr[i]= %c\n",i-*iStart,xx, avX, xx-avX, th, avY, yy, avY- yy, resultsStr[i]);
        	  if(xx-avX>th &&  avY- yy > 5) {
// printf("2---i= %d, xx= %d, avX= %d, xx-avX= %d, th= %d, avY= %d, yy= %d, avY- yy= %d, resultsStr[i]= %c\n",i-*iStart,xx, avX, xx-avX, th, avY, yy, avY- yy, resultsStr[i]);
        		     resultsStr[i] = ' ';
        		     if(i+1 <*iStop && resultsStr[i+1] > 32) {
        		    	 yy = lpFieldsResults->flQuad[i+1].p3Y-lpFieldsResults->flQuad[i+1].p1Y;
        		    	 if(avY- yy > 5) resultsStr[i+1] = ' ';
        		     }
    	    	}
            }
        }
    }
// printLine(resultsStr, *iStart, *iStop);

    ccSp = 0;ccSpC = 0;
    for(i=*iStart;i<stopTmp;i++){
    	if(resultsStr[i] == 32) {
    		if(ccSpC == 0) ccSp++;
    		ccSpC++;
    	}
    	else ccSpC = 0;
    }

//printf("---*iStart= %d\n", *iStart);
//  printf("---ccSp= %d\n", ccSp);
     if(ccSp >thGroups) {
 // check start
		k = 0;
 	    for(i=*iStart+1;i<*iStop;i++){
 	       if(resultsStr[i] == 32){
 	    	    xxT[k]= lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
//	    printf("--- xxT[ %d ]= %d, resultsStr[i-1]= %c, resultsStr[i+1]= %c\n",k,xxT[k], resultsStr[i-1], resultsStr[i+1]);
		    	    xxIT[k] = i;
		    	    k++;
		    	    if(k > 2) break;
		        }
			}

    	 if(xxT[0]-xxT[1] > 15)
    		  *iStart = xxIT[0]+1;
//printf("---*iStart= %d, xxT[0]= %d, xxT[1]= %d, xxT[0]-xxT[1]= %d\n", *iStart, xxT[0], xxT[1], xxT[0]-xxT[1]);

      }
 	}

// printLine(resultsStr, *iStart, *iStop);
 	testfirst2Groups(lpFieldsResults, *iStart, *iStop, avY);
// 	 printLine(resultsStr, *iStart, *iStop);

 	kStartY = kStartX;// avoiding warnings
// 	 printLine(resultsStr, *iStart, *iStop);
}

void testTrashBegEnd(LPFIELDSRESULTS lpFieldsResults, int *iStart, int *iStop){
	 char *resultsStr = lpFieldsResults->result;
	 int i, k, xx, av, th;
/*
	 k = 0;
	 for(i=*iStart;i<*iStop;i++){
		 if(resultsStr[i] == 32){
		    xx= lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
//printf("---xx[ %d ]= %d\n", k, xx);k++;
		 }
	 }
*/
	 k = 0;av = 0;
	 for(i=*iStart+3;i<*iStart+12;i++){
		 if(resultsStr[i] == 32){
		    xx= lpFieldsResults->flQuad[i+1].p1X-lpFieldsResults->flQuad[i-1].p2X;
//printf("---xx[ %d ]= %d\n", k, xx);
           av += xx; k++;
		 }
	 }
	 if(k>0) av /= k;
//printf("---av= %d\n", av);
//	 th = av/2+av/3;
	 th = av-5;
	 k = -1;
	 for(i=*iStart;i<*iStop;i++){
	   if(resultsStr[i] == 32) {k = i;break;}
	 }
	 if(k > 0 && k-*iStart < 4){
		 xx= lpFieldsResults->flQuad[k+1].p1X-lpFieldsResults->flQuad[k-1].p2X;
// printf("1---xx= %d, av= %d, th= %d, xx - av= %d\n", xx, av, th, xx - av);
// printf("1---k= %d, resultsStr[k+1]= %c, resultsStr[k-1]= %c, lpFieldsResults->flQuad[k+1].p1X= %d, lpFieldsResults->flQuad[k-1].p2X= %d\n", k, resultsStr[k+1], resultsStr[k-1], lpFieldsResults->flQuad[k+1].p1X, lpFieldsResults->flQuad[k-1].p2X);
		 if(xx - av > th) *iStart = k+1;
	 }
     k += 2;
     if(resultsStr[k] == 32) {
		 xx= lpFieldsResults->flQuad[k+1].p1X-lpFieldsResults->flQuad[k-1].p2X;
		 if(xx - av > th) *iStart = k+1;
// printf("1---xx= %d, av= %d, th= %d, xx - av= %d\n", xx, av, th, xx - av);
//printf("1---k= %d, resultsStr[k+1]= %c, resultsStr[k-1]= %c, lpFieldsResults->flQuad[k+1].p1X= %d, lpFieldsResults->flQuad[k-1].p2X= %d\n", k, resultsStr[k+1], resultsStr[k-1], lpFieldsResults->flQuad[k+1].p1X, lpFieldsResults->flQuad[k-1].p2X);
   	 }



// printLine(resultsStr, *iStart, *iStop);
	 k = -1;
	 for(i=*iStop;i>*iStart;i--){
	   if(resultsStr[i] == 32) {k = i;break;}
	 }
// printf("1---k= %d, resultsStr[k+1]= %c, resultsStr[k-1]= %c, lpFieldsResults->flQuad[k+1].p1X= %d, lpFieldsResults->flQuad[k-1].p2X= %d\n", k, resultsStr[k+1], resultsStr[k-1], lpFieldsResults->flQuad[k+1].p1X, lpFieldsResults->flQuad[k-1].p2X);
	 if(k > 0 && *iStop-k < 5){
		 xx= lpFieldsResults->flQuad[k+1].p1X-lpFieldsResults->flQuad[k-1].p2X;
//	 printf("2---xx= %d, av= %d, xx - av= %d, th= %d\n", xx, av, xx - av, th);
		 if(xx - av > th) {
// printf("2---xx= %d, av= %d, xx - av= %d, th= %d\n", xx, av, xx - av, th);
			 for(i=k;i<*iStop;i++){
			    if(resultsStr[i] >= 32) resultsStr[i] = ' ';
			 }
		 }
	 }
// printLine(resultsStr, *iStart, *iStop);
// av Y
	 {int yy = 0;
	   k = 0;av = 0;
	   for(i=*iStart+3;i<*iStart+10;i++){
		 if(resultsStr[i] > 32){
		    yy= lpFieldsResults->flQuad[i].p3Y-lpFieldsResults->flQuad[i].p1Y;
//printf("---xx[ %d ]= %d\n", k, xx);
           av += yy; k++;
		 }
	   }
	   if(k>0) av /= k;
//printf("------------avY= %d\n", av);
		 for(i=*iStop;i>*iStart;i--){
		   if(resultsStr[i] > 32) {k = i;break;}
		 }
		 th = 12;
		 if(k > 0 && *iStop-k < 5){
			 yy= lpFieldsResults->flQuad[k].p3Y-lpFieldsResults->flQuad[k].p1Y;
//	 printf("1---yy= %d, av= %d, av- yy  = %d, th= %d\n", yy, av, av -yy , th);
			 if(av-yy > th) {
// printf("1---yy= %d, av= %d, av-yy= %d, th= %d\n", yy, av, av-yy, th);
				 resultsStr[k] = ' ';
				 k--;
				 if(resultsStr[k] > 32) {
					 yy= lpFieldsResults->flQuad[k].p3Y-lpFieldsResults->flQuad[k].p1Y;
//	 printf("2---yy= %d, av= %d, yy - av= %d, th= %d\n", yy, av, yy - av, th);
					 if(av-yy > th) {
// printf("2---yy= %d, av= %d,  av-yy= %d, th= %d\n", yy, av, av-yy, th);
						 resultsStr[k] = ' ';
				 }
			 }
		    }
	     }



	 }

}
void cleanTicketNumbers_LottoTXC5(LPIMGDATA img, LPFIELDSRESULTS lpFieldsResults, int nn) {
	 char *resultsStr = lpFieldsResults->result;
	 int numGr, numGrCorr, i, k;
	 int iStart, iStop;
	 char strT[10];
	 int numLines = 0;

	 int thGroups = 5;
	 int thMaxGroup;
	 int numChars;
	 int beginCut = 0;
	 int ccD, ccS;
	 int firstLineIdx = 0;
	 int countLines = 0, ccCount, ccSth;
	 int lineStarted, lineSkipped, againLine;

	 resultTmp[0] = 0;
// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
 ccCount = 0;
 ccSth = 7;
 while(ccCount < 2){
	 lineStarted = 0; lineSkipped= 0; againLine = 0;
	 ccSth++;
	 i = 0;
	 firstLineIdx = 0;
	 while(i<numLines){
		 if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		 if(testLineConfC5_L_TX(lpFieldsResults, i, ccSth) >= 0){
			 firstLineIdx = i;break;
		 }
		 i++;
	 }
     if(firstLineIdx>0){
        for(k=0;k<i;k++)
          lpFieldsResults->lineQuadStatus[k] = 0;
     }

//   	printf("------------------------------firstLineIdx= %d\n", firstLineIdx);


	 k = numLines-1;
	 while(k>firstLineIdx){
		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0){
			 lpFieldsResults->lineQuadStatus[k] = 0;
			 numLines--;
		 }
		 else
			 break;
		 k--;
	 }
//   	printf("------------------------------numLines= %d\n", numLines);
	 i = firstLineIdx;
	 k = 0;
	 while(i < numLines){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//           testLeftSideForLine(resultsStr, &iStart, iStop);

//printLine(resultsStr, iStart, iStop);
           testLeftSideForLineTXC5(lpFieldsResults, &iStart, iStop);
// printLine(resultsStr, iStart, iStop);

	    numGr = getNumberOfGroups(resultsStr, iStart, iStop);
	    beginCut = 0;
        numChars = getNumChars(resultsStr, iStart, iStop);
        if(numGr == thGroups){
        	beginCut = testBeginCut(lpFieldsResults, iStart, iStop);
            if(beginCut > 0)
            	numGr++;
        }
        testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
// printLine(resultsStr, iStart, iStop);
//  printf ("--------i= %d, numGr= %d, nn= %d, ccD= %d, ccS= %d\n", i, numGr, nn, ccD, ccS);
        numGrCorr = 0;
        if(ccD == nn+1 && ccS == nn) numGrCorr = 2;
        if(ccD < nn || ccS < nn-1) numGrCorr = -3;

        thMaxGroup = nn+6;
        thGroups = nn;

//   printf ("--------i= %d, numGr+numGrCorr= %d, thMaxGroup= %d, thGroups= %d, numChars= %d\n", i, numGr+numGrCorr, thMaxGroup, thGroups, numChars);

	    if(numGr+numGrCorr < thMaxGroup && numGr+numGrCorr > thGroups && strlen(resultsStr) > 15 && numChars < 20+nn){
//	    if(numGr > thGroups && strlen(resultsStr) > 15 && numChars < 20){
//  	printf("------------------------------line= %d\n", i);
	    	if(lineStarted > 0 && lineSkipped > 0) againLine++;
	    	if(lineStarted == 0) lineStarted++;


	    	testTrashBegEnd(lpFieldsResults, &iStart, &iStop);
	       copyLine(lpFieldsResults, iStart, iStop);
	       testTrashBeg_3Chars(img);
//   printf("1--- beginCut= %d, line i= %d; %s\n",beginCut, i, result);
	       if(beginCut > 0){
               strT[0] = 'A'+k;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
	       }else{
//              getFirstCharFromNumLine(&iStart, &iStop);
//  printf("1--- i= %d, numGr= %d; result= %s\n",i, numGr, result);
              numGr = cutLastChar_LTX(numGr);
              getFirstCharFromNumLineC5_LTX(&iStart, &iStop, numGr);

              if(numGr < 8 && iStart == 0 && iStop == 0 && result[1] != 32) {
                 strT[0] = 'A'+k;
                 strT[1] = 32;
                 strT[2] = 0;
                 replaceChars(0, -1, strT);
             }else{
                 strT[0] = 'A'+k;
                 strT[1] = 0;
                replaceChars(iStart, iStop, strT);
//       printf("replaceChars, iStart= %d; iStop= %d, result= %s\n",iStart, iStop, result);
             }
	       }
	       filtration('&');
// printf("2--- line i= %d; %s\n",i, result);
           iStop = -1;
//           if( numGr > nn) {
// printf("--- line i= %d; ccS= %d,nn= %d \n",i, ccS, nn);

//               if(getLastQPFromNumLine(&iStart, &iStop, nn+1) >0)
//                   replaceChars(iStart, iStop, "QP");
//           if( ccS > nn) {
//              if(getSecondQPFromNumLineLastQP(&iStart, &iStop, nn+1) >=0){
//                 replaceChars(iStart, iStop, "QP");
//              }
//           }
	       addLineToTmp();
	       countLines++;
       	   k++;
	    }else{
	    	lpFieldsResults->lineQuadStatus[i] = 0;
	    	if(lineStarted > 0) lineSkipped++;
	    }
	    i++;
	 }
	 if(countLines > 0) break;
	 ccCount++;
	 }
// printf("1-------------------lineStarted= %d, lineSkipped= %d, againLine= %d,linesPostCnf= %d\n", lineStarted,lineSkipped,againLine,linesPostCnf);
	 if(lineStarted > 0 && lineSkipped > 0 && againLine > 0) {linesPostCnf += 4;
//printf("2-------------------lineStarted= %d, lineSkipped= %d, againLine= %d,linesPostCnf= %d\n", lineStarted,lineSkipped,againLine,linesPostCnf);
	 }
	 copyResultsBack(lpFieldsResults);
}

void getFirstNumberPick3TX_New(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1, int nn, int kCombi, int iStart_X){
	char *resultsStr = lpFieldsResults->result;
	int isT = iStart, dd = 0, len, i, n;

//printf ("---------------------------------------isT= %d, resultsStr[isT]= %s, \n",isT, &resultsStr[isT]);

	*iStart1 = 0; *iStop1 = 0;
	len = iStop-iStart+1;
	if(len< nn+4 || len > 30) return;
    if(iStart_X >= 0){
    	*iStart1 = iStart_X;
    	*iStop1 = iStart_X+nn; return;
    }
//		printLine(resultsStr, iStart, iStop);
		n = -1;
		for(i=iStart;i<iStop;i++){
			if(resultsStr[i] == 32) {n=i-iStart;break;}
		}
//	  printf ("--------------------n= %d, nn= %d, kCombi= %d\n", n, nn, kCombi-1);
		if(n == nn && kCombi-1 == nn){
			*iStart1 = iStart;
			*iStop1 = nn+1;
			return;
		}



	if(resultsStr[isT] == 32 || resultsStr[isT+1] != 32) return;
	for(i=0;i<nn;i++){
		if(resultsStr[isT+2+i] == 32) return;
	}
	if(resultsStr[isT+2+nn] != 32) return;
	n = isT+2+nn+1;
	if(resultsStr[n] == 32) return;

	dd = lpFieldsResults->flQuad[isT+2].p1X-lpFieldsResults->flQuad[isT].p2X;

// printf ("--------------------------------------------iStart= %d, dd1= %d\n",iStart, dd);
	if(dd > 70) return;
    dd = lpFieldsResults->flQuad[n].p1X-lpFieldsResults->flQuad[n-2].p2X;
//    printf ("--------------------resultsStr[n]= %c, resultsStr[n-2]= %c\n",resultsStr[n], resultsStr[n-2]);

//   printf ("--------------------------------------------iStart= %d, dd2= %d\n",iStart, dd);
	if(dd < 30 || dd > 300) return;
	*iStart1 = isT+2;
	*iStop1 = n;

//	 printf ("--------------------------------------------iStart= %d, dd2= %d\n",iStart, dd);
}

void getFirstNumberPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1, int nn, int kCombi){
	char *resultsStr = lpFieldsResults->result;
	int isT = iStart, dd = 0, len, i, n;

//printf ("---------------------------------------isT= %d, resultsStr[isT]= %s, \n",isT, &resultsStr[isT]);

	*iStart1 = 0; *iStop1 = 0;
	len = iStop-iStart+1;
	if(len< nn+4) return;

//		printLine(resultsStr, iStart, iStop);
		n = -1;
		for(i=iStart;i<iStop;i++){
			if(resultsStr[i] == 32) {n=i-iStart;break;}
		}
//	  printf ("--------------------n= %d, nn= %d, kCombi= %d\n", n, nn, kCombi-1);
		if(n == nn && kCombi-1 == nn){
			*iStart1 = iStart;
			*iStop1 = nn+1;
			return;
		}



	if(resultsStr[isT] == 32 || resultsStr[isT+1] != 32) return;
	for(i=0;i<nn;i++){
		if(resultsStr[isT+2+i] == 32) return;
	}
	if(resultsStr[isT+2+nn] != 32) return;
	n = isT+2+nn+1;
	if(resultsStr[n] == 32) return;

	dd = lpFieldsResults->flQuad[isT+2].p1X-lpFieldsResults->flQuad[isT].p2X;

// printf ("--------------------------------------------iStart= %d, dd1= %d\n",iStart, dd);
	if(dd > 70) return;
    dd = lpFieldsResults->flQuad[n].p1X-lpFieldsResults->flQuad[n-2].p2X;
//    printf ("--------------------resultsStr[n]= %c, resultsStr[n-2]= %c\n",resultsStr[n], resultsStr[n-2]);

//   printf ("--------------------------------------------iStart= %d, dd2= %d\n",iStart, dd);
	if(dd < 30 || dd > 300) return;
	*iStart1 = isT+2;
	*iStop1 = n;

//	 printf ("--------------------------------------------iStart= %d, dd2= %d\n",iStart, dd);
}
void getSecondStringPick3TX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop, int* iStart1, int* iStop1){
	char *resultsStr = lpFieldsResults->result;
	int isT = iStart, isT1 = 0, len, i;
	*iStart1 = isT; *iStop1 = isT1;
	len = iStop-iStart+1;
// printf ("--------------------------------------------len= %d, isT= %d\n",len, isT);
	if(len< 4) return;
//printf (&resultsStr[isT]);
	for(i=0;i<len;i++){
//printf ("------------------------------------------isT+i= %d, resultsStr[isT+i]= %c\n",isT+i,resultsStr[isT+i]);
		if(resultsStr[isT+i] == 32) {isT1 = isT+i; break;}
	}
	*iStop1 = isT1;

}
void testLeftSideForLineTX(char *resultsStr, int *iStart, int iStop){
	int isT = *iStart;
	if(iStop - isT < 4) return;
	if(resultsStr[isT] == '1' && resultsStr[isT+1] == 32 && resultsStr[isT+2] != 32 && resultsStr[isT+3] == 32)
		isT += 2;
	*iStart = isT;
}

void cleanDollarAmount(char *dollarAm, int nn, char *strT){
	int i, n, len, flag;
	char dA[20];
	float dAf;
	float DollarMax = 5.0;

	if(strcmp(strT, "COMBO") == 0) {
		if(nn == 3)
		  DollarMax = 30.0;
		else
		  DollarMax = 120.0;
	}

	len = strlen(dollarAm);
	flag = 0;
	n = 0;
	if(dollarAm[1] == '.') {dA[n] = '0';n++;flag = 1;}
	for(i=1;i<len;i++) {
		if((dollarAm[i] >= '0' && dollarAm[i] <= '9') || dollarAm[i] == '.') {
			dA[n] = dollarAm[i];n++;
		}
	}
	dA[n] = 0;
	sscanf(dA,"%f",&dAf);

//	printf("-----dA= %s\n", dA);
//	   printf("-----dAf= %f\n", dAf);
//	   printf("-----dollarAm= %s\n", dollarAm);

	if(dAf > DollarMax) strcpy(dollarAm, "$1.00");
	else {
		len = strlen(dA);
		if(dA[len-1] == '.') {dA[len] = '0';dA[len+1] = '0';dA[len+2] = 0; len += 2; flag = 1;}
		if(dA[len-2] == '.') {dA[len] = '0';dA[len+1] = 0; len++; flag = 1;}
		if(flag == 1)
		   strcpy(&dollarAm[1], dA);
	}
	len = strlen(dollarAm);
	for(i=1;i<len;i++) {
		if(dollarAm[i] == '.'){
		   if(i+1<len && dollarAm[i+1] != '0' && dollarAm[i+1] != '5') dollarAm[i+1] = '0';
		   break;
		}
	}

//	   printf("-----dollarAm= %s\n", dollarAm);
//	   printf("-------\n");
}

void mergeTwoResults(LPFIELDSRESULTS lpFieldsResults, LPFIELDSRESULTS lpFieldsResults1, int nn){
	 char *resultsStr = lpFieldsResults->result;
	 char *resultsStr1 = lpFieldsResults1->result;
	 int iStart, iStop, len, i;
	 int iStart1, iStop1, len1, i1, iSp, lenT;
	 char strT[20];
	 int  cnfT[20];
	 int iDollar, lenDamt1, lenDamt2;
     char dollarAm[20];
     int  dollarCnfAm[20];
     int lenD, kkJ;

	 i = 0;
	 resultTmp[0] = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//printf ("---iStart= %d, iStop= %d\n",iStart, iStop);
		if(getLine(resultsStr1, i, &iStart1, &iStop1) < 0) break;
        copyLine(lpFieldsResults, iStart, iStop);
//printf ("1---result= %s, strlen(result)= %d\n",result,strlen(result));
        len = iStop-iStart;
        len1 = iStop1-iStart1;
//printf ("---i= %d, len1= %d\n",i, len1);

//  printLine(resultsStr, iStart, iStop);
//  printLine(resultsStr1, iStart1, iStop1);

        if(len1 < 20) {
       	  if(len1 > 0) {

       		getDollarSign(result, &iDollar);
 //    printf ("---iDollar= %d\n",iDollar);

// 	printf ("---&result[iDollar]= %s\n",&result[iDollar]);
//       	    printf ("------result= %s\n",result);
//        	  	           {
//        	  	        	   int iiK;
//        	  	        	   for(iiK=0;iiK<strlen(result);iiK++){
//        	  	        		    printf ("------iiK= %d, result[iiK]= %c, flConf[iiK]= %d\n",iiK, result[iiK], flConf[iiK]);
//        	  	        	   }
//        	  	           }

       		strcpy(dollarAm, &result[iDollar]);
       		result[iDollar] = 13;
       		result[iDollar+1] = 0;
       		lenD = strlen(dollarAm);
       		if(dollarAm[lenD-1] == 13) dollarAm[lenD-1] = 0;
       		lenDamt1 = strlen(dollarAm);
       		for(kkJ=0;kkJ<lenDamt1;kkJ++){
       		     dollarCnfAm[kkJ] = flConf[kkJ+iDollar];
//       		 	 printf("result[kkJ+iDollar] = %c, dollarCnfAm[kkJ+iDollar]= %d\n",result[kkJ+iDollar], dollarCnfAm[kkJ]);
       		}


 //        {int jj;
 //            for(jj=0;jj<strlen(result);jj++){
 //           	 printf("%d,",result[jj]);
 //            }
 //          	printf("\n");
 //          }



//  	printf ("---result= %s, dollarAm= %s\n",result,dollarAm);
            iSp = 0;
            if(result[strlen(result)-2] != ' '){
                strT[0] = ' ';
                iSp = 1;
            }


//printf ("---result[strlen(result)-2]= %d, iSp= %d\n",result[strlen(result)-2], iSp);
            for(kkJ=0;kkJ<20;kkJ++)
               cnfT[kkJ] = 100;
            for(i1=iStart1;i1<iStop1;i1++) {
                strT[i1-iStart1+iSp] = resultsStr1[i1];
                cnfT[i1-iStart1+iSp] = lpFieldsResults1->flConf[i1];
            }
            strT[iStop1-iStart1+iSp] = 0;
            lenT = strlen(strT);
// printf ("---strT= %s, dollarAm= %s\n",strT, dollarAm);
// printf ("1---strlen(dollarAm)= %d\n",strlen(dollarAm));
 cleanDollarAmount(dollarAm, nn, strT);
 lenDamt2 = strlen(dollarAm);
// printf ("2---strlen(dollarAm)= %d\n",strlen(dollarAm));
// printf ("---strT= %s, dollarAm= %s\n",strT, dollarAm);
            strcat(strT, " ");
            strcat(strT, dollarAm);


 //          stretchResults(len, len1+iSp+strlen(dollarAm)+1);
//     printf ("---len= %d\n",strlen(result));
           stretchResults(iDollar-1, strlen(strT));
//       printf ("---len= %d\n",strlen(result));

 //      printf ("---strT=%s\n",strT);
           insResult(iDollar-1, strT);

           {int iCnf = 0;
              kkJ = iDollar-1;
       	      for(iCnf=0;iCnf<lenT;iCnf++){
// 	    	printf ("---kkJ=%d, iCnf= %d, cnfT[iCnf]= %d\n",kkJ, iCnf, cnfT[iCnf]);
// 	    	fflush(stdout);
       	        flConf[kkJ] = cnfT[iCnf];kkJ++;
       	      }
       	      flConf[kkJ] = 100;kkJ++;
       	      for(iCnf=0;iCnf<lenDamt1;iCnf++){

//   	printf ("---kkJ=%d, iCnf= %d, dollarCnfAm[iCnf]= %d\n",kkJ, iCnf, dollarCnfAm[iCnf]);
//   	fflush(stdout);

       	        flConf[kkJ] = dollarCnfAm[iCnf];kkJ++;
       	      }
       	      for(iCnf=lenDamt1;iCnf<lenDamt2;iCnf++){
       	        flConf[kkJ] = 0;kkJ++;
       	      }


           }

 //         	printf ("---result= %s\n",result);
      	  }
          addLineToTmp();
        }

		i++;
	 }
	 len1 = len;// avoiding warnings

//	           	    printf ("------result= %s\n",resultTmp);
//	            	  	           {
//	            	  	        	   int iiK;
//	            	  	        	   for(iiK=0;iiK<strlen(resultTmp);iiK++){
//	            	  	        		    printf ("------iiK= %d, result[iiK]= %c, flConf[iiK]= %d\n",iiK, resultTmp[iiK], flConfTmp[iiK]);
//	            	  	        	   }
//	            	  	           }

	 copyResultsBack(lpFieldsResults);
}
int matchPickLine(int *idxOut, int nn, int *max){
	int cc, ccMax, idxMax;
	int i, k, kk, j, start, stop, len;
	int ccDraw, kkDraw;
	*idxOut = 0;
	*max = 0;
	cc = 0;ccMax = 0;idxMax = -1;
	k = -1;
//      	printf ("---result= %s\n",result);

	for(i=0;i<len_Pick3_Names;i++) {
		cc =matchToStringResult(Pick3_Names[i], &kk);
		if(ccMax < cc) {
			ccDraw =matchToStringResult("DRAW ", &kkDraw);
			if(ccDraw > 4) cc = 0;
//		printf("cc= %d, ccMax= %d, ccDraw= %d\n",cc, ccMax,ccDraw);
		}
	   if(ccMax < cc) {
		    start = kk+1;len = strlen(Pick3_Names[i]);
		    stop = start+len-2;
		    if(stop >= strlen(result)) stop = strlen(result);
//		    printf ("--- cc= %d\n", cc);
			for(j=start;j<stop;j++) {
				if(result[j] == 32) cc--;
			}
//	      if(ccMax < cc && kk >= nn && kk < nn+6){ccMax = cc;idxMax = i;k = kk;}
	      if(ccMax < cc && kk >= nn && kk < nn+10){ccMax = cc;idxMax = i;k = kk;}
	   }
//   	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d, nn+6= %d\n",Pick3_Names[i], cc, ccMax, kk,nn+6);
	}
//  printf ("ccMax= %d, k= %d, Pick3_Names[idxMax]= %s\n", ccMax, k,  Pick3_Names[idxMax]);
	*max = ccMax;
	if(ccMax > 2 && k >= 0) {
		*idxOut = idxMax;
		return k;
	}
	return -1;
// printf ("result= %s\n",result);
}

void matchPickTerms(void){
	int cc, ccMax, idxMax;
	int i, k, kk;
	cc = 0;ccMax = 0;idxMax = -1;
	k = -1;
//printf ("---result= %s\n",result);
	for(i=0;i<len_Pick3_Names;i++) {
		cc =matchToStringResult(Pick3_Names[i], &kk);
	   if(ccMax < cc)
	      if(ccMax < cc){ccMax = cc;idxMax = i;k = kk;}
//   	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d\n",Pick3_Names[i], cc, ccMax, kk);
	}
//	 printf ("ccMax= %d, k= %d, idxMax= %d\n", ccMax, k, idxMax);
	if(ccMax > 0 && k >= 0) {
	   if(k > 0) shrinkResults(0, k-1);
//  printf ("result= %s, Pick3_Names[idxMax]= %s\n",result, Pick3_Names[idxMax]);
	   if(idxMax == 10) idxMax= 7;
	   { char strTmp[20];
	      strcpy(strTmp,  &Pick3_Names[idxMax][1]);
	      strTmp[strlen(strTmp)-1] = 0;
	      strcpy(result,  strTmp);
//	      strcpy(result,  Pick3_Names[idxMax]);
	   }
	}
// printf ("result= %s\n",result);
}
void testForXX(char *result, LPFIELDSRESULTS lpFieldsResults, int iStart, int nn){
	char *resultsStr = lpFieldsResults->result;
	int iStop = iStart+nn-1;
//	printf ("iStart= %d, iStop= %d\n",iStart, iStop);
//	printLine(resultsStr, iStart, iStop+1);
//	printf ("result= %s\n",result);

//	printf ("resultsStr[iStart]= %c, lpFieldsResults->flConf[iStart]= %d\n",resultsStr[iStart], lpFieldsResults->flConf[iStart]);
	if(resultsStr[iStart]=='X' && lpFieldsResults->flConf[iStart] > 20) {
//		printf ("resultsStr[iStart+1]= %c, lpFieldsResults->flConf[iStart+1]= %d\n",resultsStr[iStart+1], lpFieldsResults->flConf[iStart+1]);
		if(resultsStr[iStart+1]=='X' && lpFieldsResults->flConf[iStart+1] > 20) {
			result[0] = 'X';result[1] = 'X';
			return;
		}
//		printf ("resultsStr[iStop]= %c, lpFieldsResults->flConf[iStop]= %d\n",resultsStr[iStop], lpFieldsResults->flConf[iStop]);
		if(resultsStr[iStop]=='X' && lpFieldsResults->flConf[iStop] > 20) {
			result[0] = 'X';result[3] = 'X';
			return;
		}

	}
//	printf ("resultsStr[iStop]= %c, lpFieldsResults->flConf[iStop]= %d\n",resultsStr[iStop], lpFieldsResults->flConf[iStop]);
	if(resultsStr[iStop]=='X' && lpFieldsResults->flConf[iStop] > 20) {
//		printf ("resultsStr[iStop-1]= %c, lpFieldsResults->flConf[iStop-1]= %d\n",resultsStr[iStop-1], lpFieldsResults->flConf[iStop-1]);
		if(resultsStr[iStop-1]=='X' && lpFieldsResults->flConf[iStop-1] > 20) {
			result[2] = 'X';result[3] = 'X';
			return;
		}
	}


}

int testLine_1(char *resultN, int nn){
//	int mode = 0;
// printf ("------result= %s\n",result);
//	fflush(stdout);
//	int len = strlen(result);
	int i = 0, ok = 1;
	int k, cc, ccMax;
	int kk, idxLineA;

//	matchToStringEndOneLineIdx(char *results, char *str, int sh){
	ccMax = 0;
	idxLineA = -1;
	for(i=0;i<len_Pick3_Names;i++) {
		cc =matchToStringResult(Pick3_Names[i], &kk);
//	   if(ccMax < cc)
//	      if(ccMax < cc && cc == strlen(Pick3_Names[i])){ccMax = cc;k = kk;idxLineA = i;}
		    if(ccMax < cc){ccMax = cc;k = kk;idxLineA = i;}
//     	printf ("---Pick3_Names[i]= %s, cc= %d, ccMax= %d, kk= %d, idxLineA= %d\n",Pick3_Names[i], cc, ccMax, kk, idxLineA);
	}

     if(idxLineA < 0) return 0;
     if(strlen(Pick3_Names[idxLineA])-ccMax > 3 ) return 0;
//          printf ("---Pick3_Names[i]= %s, ccMax= %d, k= %d\n",Pick3_Names[idxLineA], ccMax, k);

     kk = k;// avoiding warnings
   if(ccMax > 2) return ok;
   return 0;
 /*
   if(resultN[nn] > 32) return 0;

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

//	printf ("------cc1= %d \n",cc1);
//	while(result[i] != 32 && i<len) {
//		printf ("------i= %d, resultr[i]= %c, flConf[i]= %d\n",i, result[i], flConf[i]);
//		i++;
//	}
	return ok;
*/
}
int getY1(){
 int stop, i, cc, yy = 0;
		stop = strlen(result);
		cc = 0;yy = 0;
		for(i=0;i<stop;i++){
		   if(result[i]>32){
		     yy += flQuad[i].p1Y;cc++;
//  printf ("y1----------result[ %d ]= %c, flQuad[i].p1Y= %d\n",i, result[i], flQuad[i].p1Y);
		   }
		}
	   if(cc>0)
		  yy /= cc;

		return yy;
}
int getY2(){
 int stop, i, cc, yy = 0;

	    stop = strlen(result);
 		cc = 0;yy = 0;
 		for(i=0;i<stop;i++){
		   if(result[i]>32){
			  yy += flQuad[i].p3Y;cc++;
// printf ("y2----------result[ %d ]= %c, flQuad[i].p1Y= %d\n",i, result[i], flQuad[i].p3Y);
		   }
	  }
      if(cc>0)
		  yy /= cc;

	  return yy;
 }
int getX_Pick3(LPFIELDSRESULTS lpFieldsResults, int line, int nn, int xSt){
	int iStart, iStop;
	int x = 0;
	int n;
	char *resultsStr = lpFieldsResults->result;

    if(getLine(resultsStr, line, &iStart, &iStop) < 0) return x;
    n = xSt-nn +iStart;
    if(n >=0 && resultsStr[n] != 32)
    	x = lpFieldsResults->flQuad[n].p1X;
//  printf("  resultsStr[n]= %c, resultsStr[n+1]= %c, resultsStr[n+2]= %c, x= %d\n", resultsStr[n], resultsStr[n+1], resultsStr[n+2], x);
//	   copyLine(lpFieldsResults1, iStart, iStop-1);
//    printf("  resultsStr[iStart+xSt]= %c, resultsStr[iStart+xSt+1]= %c, resultsStr[iStart+xSt+2]= %c\n", resultsStr[iStart+xSt], resultsStr[iStart+xSt+1], resultsStr[iStart+xSt+2]);

	return x;
}

int getX_Pick3N(LPFIELDSRESULTS lpFieldsResults, int line, int nn, int xX){
	int iStart, iStop;
	int n = -1, i, minD, iMinD;
	char *resultsStr = lpFieldsResults->result;
    if(getLine(resultsStr, line, &iStart, &iStop) < 0) return n;
    minD = 1000;iMinD= -1;
    for(i=iStart;i<iStop;i++){
    	if(resultsStr[i] > 0) {
    	  if(abs(lpFieldsResults->flQuad[i].p1X -xX) < minD){
    		  minD = abs(lpFieldsResults->flQuad[i].p1X -xX);iMinD = i;
    	  }
    	}
    }
    if(minD < 30) n = iMinD;
//    printf("  line= %d, minD= %d, n= %d\n", line, minD, n-iStart);
//    if(n >=0)
//       printf("  resultsStr[n]= %c, resultsStr[n+1]= %c\n", resultsStr[n], resultsStr[n+1]);
	return n;
}

void cleanTicketNumbers_Pick3TX(LPTICKETRESULTS ticketResults, int idx, int nn) {
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
	 char strNum[10];
//	 int iStopTab[20];
//	 int iStop1Tab[20];
	 int iStart, iStop;
	 int iStart1, iStop1;
	 char strT[10];
	 int numLines = 0;
	 char dollarAm[10];
	 int  dollarAmCnf[10], dollarAmTotCnf[10];
	 char totalStr[10];
	 int yPrev, yCur, ddY;
	 int firstLineIdx;
	 int xX, iStart_X;

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

//	 while(k>0){
//		 if(getLine(resultsStr, k, &iStart, &iStop) < 0) break;
//		 if(testLineConf(lpFieldsResults, iStart, iStop) < 0){
//			 lpFieldsResults->lineQuadStatus[k] = 0;
//			 numLines--;
//		 }
//		 else
//			 break;
//		 k--;
//	 }
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
		   strcpy(resultTmp, result);
		   if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
		   copyLine(lpFieldsResults1, iStart, iStop-1);

//   printf ("------i= %d, result= %s\n",i, result);
           if(strlen(result)< 30)
		      if(testLine_1(resultTmp, nn) > 0) {firstLineIdx = i;break;}
		   i++;
//		    printf ("------result= %s\n",result);
	    }
	    if(i>0){
          for(k=0;k<i;k++)
	        lpFieldsResults->lineQuadStatus[k] = 0;
	    }
	    resultTmp[0] = 0;
	    result[0] = 0;
//   printf ("------firstLineIdx= %d\n",firstLineIdx);
//   printf ("----------numLines= %d\n",numLines);

// SUM IT UP test
     {int ccS = 0, kkS; //, yyDist1 = 0, yyDist2, thY = 100;
      int kkSTab[numLines], kMax[numLines];
      for(i=0;i<numLines;i++) {kkSTab[i] = -1;kMax[i] = 0;}
        i = firstLineIdx;
	    while(i < numLines){
		   if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
		   copyLine(lpFieldsResults1, iStart, iStop-1);
		   ccS =matchToStringResult("SUM IT UP", &kkS);

//  printf ("----------i= %d, result= %s, ccS= %d, kkS= %d\n",i, result, ccS, kkS);
//		   printLine(resultsStr1, iStart, iStop);
		   if(ccS > 5)
			   kSumItUp[i] = 1;
		   else{
/*
			   if(i == firstLineIdx){
			      yyDist1 = getY2();
			   }
			   else{
			      yyDist2 = getY1();
			      if(yyDist2 - yyDist1 > thY) {numLines = i+1;break;}
			      yyDist1 = getY2();
			   }
*/

			   kkSTab[i] =  matchPickLine(&kCombIdx[i], nn, &kMax[i]);

//  printf ("-------------i= %d, kCombIdx[i]= %d, Pick3_Names[idxMax]= %s, kMax[i]= %d\n", i, kCombIdx[i],  Pick3_Names[kCombIdx[i]], kMax[i]);

//  printf ("----------i= %d, kkSTab[i]= %d, kCombIdx[i]= %d, kMax[i]= %d\n",i, kkSTab[i], kCombIdx[i], kMax[i]);

//                 if(kkS >= nn){
//               	   kkS++;
//            	   kComb[i] = kkS;
//            	   kCombStart[i] = iStart+kComb[i]-nn;
//   	 	    	   if(resultsStr1[iStart+kComb[i]-1] == 32)
//   	 	    		  kCombStart[i]--;
//               }
		   }

		   i++;
   	    }
	    {int kkSMax, iKksMax, iK1;
	      kkSMax = 0; iKksMax = 0;
	      for(i=0;i<numLines;i++) if(kkSMax < kMax[i] && kMax[i]>=nn){  kkSMax=kMax[i];iKksMax = i;}
	      if(kkSMax>3){
	    	  if(iKksMax > 0) {
	    		 iK1 = iKksMax-1;
	    	     for(i=iK1;i>=0;i--){
	    	    	 if((kMax[i]> 3 && kkSTab[i]>=nn) || kSumItUp[i] > 0) iKksMax = i;
	    	    	 else break;
	    	     }
	    	  }
	    	  for(i=iKksMax;i<numLines;i++){
	   		     if(getLine(resultsStr1, i, &iStart, &iStop) < 0) break;
	   		     if((kMax[i]<=3 || kkSTab[i]<nn) &&  kSumItUp[i] == 0) break;
	   		     kComb[i] = kkSTab[i];
	   		     kCombStart[i] = iStart+kComb[i]-nn;
	   		     if(resultsStr1[iStart+kComb[i]-1] == 32)
	   		  	    kCombStart[i]--;
	    	  }
		  }

	    }

     }

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
//    fflush(stdout);
//    printLine(resultsStr1, iStart1, iStop);
           xX = getX_Pick3(lpFieldsResults1, i, nn, kComb[i]);
           iStart_X = -1;
           if(xX > 0)
              iStart_X = getX_Pick3N(lpFieldsResults, i, nn, xX);
//printf ("-------------------------------i= %d\n",i);
           getFirstNumberPick3TX_New(lpFieldsResults, iStart, iStop, &iStart1, &iStop1, nn, kComb[i], iStart_X);

// printf ("--------------------------------------------i= %d, iStart1= %d, iStop1= %d, kComb[i]= %d\n",i,iStart1, iStop1, kComb[i]);
   	    if((iStart1 < 0 || iStart1 >= iStop1) && kComb[i] > nn){
   	    	iStart1 = iStart+kComb[i]-nn;
 	    	if(resultsStr[iStart+kComb[i]-1] == 32)
   	    		iStart1--;
   	    	iStop1 = iStart1+nn;
   	    }

//  printf ("--------------------------------------------i=%d, iStart1= %d, iStop1= %d\n",i,iStart1, iStop1);
//   	    if(kSumItUp[i] == 0){
   	    	yCur = lpFieldsResults->flQuad[iStart1].p3Y;
   	    	if(yPrev > 0) {
   	    		ddY =  yCur - yPrev;
//   	    		printf ("------yPrev=%d, yCur= %d, ddY= %d\n",yPrev,yCur, ddY);
   	    	}
//   	    	yPrev = yCur;
//   	    }
//  printLine(resultsStr, iStart1, iStop1);
//  fflush(stdout);
//  	 printf ("-------------------------------i= %d, iStart1= %d, iStop1= %d, kComb[i]= %d, ddY= %d\n",i,iStart1, iStop1, kComb[i], ddY);
	    if( ((iStart1 > 0 && iStart1 < iStop1 && kComb[i] > 0) || kSumItUp[i] > 0) && ddY < 180){
	    	yPrev = yCur;

// printLine(resultsStr, iStart1, iStop1);
// printf ("-------i= %d, kComb[i]= %d\n",i, kComb[i]);
//   	 printf ("-------i= %d, kSumItUp[i]= %d\n",i, kSumItUp[i]);

	    	if(kSumItUp[i] > 0){
	    		int kkJ = 0;
				strcpy(result,  "SUM IT UP ");
				for(j=0;j<10;j++){
				   flConf[j] = 100;
				}

				getDollarAmountSumItUp(resultsStr, iStart, iStop, dollarAm, totalStr, (int *)lpFieldsResults->flConf, dollarAmCnf, dollarAmTotCnf);
				if(totalStr[0] == 0){
				   sprintf(strNum,"%d",total);
				}
				else{
					strcpy(strNum, totalStr);
				}
			    strcat(result,  strNum);
			    kkJ = 10;
				for(j=0;j<strlen(strNum);j++){
				   flConf[kkJ] = dollarAmTotCnf[j];
				   kkJ++;
				}

				strcat(result, " ");flConf[kkJ] = 100;kkJ++;
				strcat(result, dollarAm);
				{int jj;
				   for(jj=0;jj<strlen(dollarAm)+1;jj++){
				      flConf[kkJ] = dollarAmCnf[jj];kkJ++;
				   }
				}

	               strT[0] = 'A'+kS;
	               strT[1] = 32;
	               strT[2] = 0;
	               replaceChars(0, -1, strT);
	               addEOL();
//      printf ("S------result= %s\n",result);
		           addLineToTmp();
//      printf ("S------resultTmp= %s\n",resultTmp);
		           kTab[k] = i;
		           shiftTab[k] = shift;
		           k++;
		           kS++;

	    	}
	    	else {
//	    		if(xX > 0) adjust_Numbers(lpFieldsResults, i, nn, &iStart1, &iStop1);
	           copyLine(lpFieldsResults, iStart1, iStop1-1);
	           total = 0;
	           if(nn == 4) testForXX(result, lpFieldsResults1,kCombStart[i], nn);


// printf ("------result= %s\n",&lpFieldsResults->result[iStart1]);
               getDollarAmount(resultsStr, iStop1, iStop, dollarAm, (int *)lpFieldsResults->flConf, dollarAmCnf);


	           for(j=0;j<strlen(result);j++){
                  if((result[j]<'0' || result[j]>'9') && result[j] != 'X') break;
                  if(result[j] != 'X')
                     total += result[j] - '0';
	           }
//	   printf ("1------result= %s\n",result);
	           {int jjStart = strlen(result), jj, kkJ;
				strcat(result, " ");flConf[jjStart] = 100;jjStart++;
				strcat(result, dollarAm);
				   kkJ = 0;
				   for(jj=jjStart;jj<strlen(dollarAm);jj++){
				      flConf[jj] = dollarAmCnf[kkJ++];
				   }
				}
//	 printf ("2------result= %s\n",result);

               strT[0] = 'A'+kS;
               strT[1] = 32;
               strT[2] = 0;
               replaceChars(0, -1, strT);
               addEOL();
	           addLineToTmpSpace();
	           kTab[k] = i;
	           shiftTab[k] = shift;
//	           iStopTab[k] = iStop;
//	           iStop1Tab[k] = iStop1;
	           if(i+1 < 20 && kSumItUp[i+1] == 0) kS++;
       	       k++;
	       }
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
//			   printLine(resultsStr1, iStart, iStop);
//	    printf ("2--------------------------------------------iStart= %d, iStop= %d, shiftTab[k]= %d\n",iStart, iStop, shiftTab[k]);
//			iStart += shiftTab[k];
//	    printf ("2--------------------------------------------iStart= %d, iStop= %d, shiftTab[k]= %d\n",iStart, iStop, shiftTab[k]);
	//printf ("-------------------------------i= %d\n",i);

	           getFirstNumberPick3TX(lpFieldsResults1, iStart+shiftTab[k], iStop, &iStart1, &iStop1, nn, kComb[i]);
//        printf ("2--------------------------------------------iStart1= %d, iStop1= %d\n",iStart1, iStop1);

	      	    if(kComb[i] > nn){
	      	    	iStart1 = iStart+kComb[i];
	      	    	iStop1 = iStart1+strlen(Pick3_Names[kCombIdx[i]]);
//printf ("-------i= %d, kComb[i]= %d, iStart+kComb[i]= %d, kCombIdx[i]= %d, Pick3_Names[kCombIdx[i]]= %s\n",i, kComb[i], iStart+kComb[i], kCombIdx[i], Pick3_Names[kCombIdx[i]]);
	      	    }else
	              getSecondStringPick3TX(lpFieldsResults1, iStop1, iStop, &iStart1, &iStop1);

//  printLine(resultsStr1, iStart, iStop);
//  printLine(resultsStr1, iStart1, iStop1);

/*
	      	    if((iStart1 < 0 || iStart1 >= iStop1) && kComb[i] > nn){
	      	    	iStart1 = iStart+kComb[i];
	      	    	iStop1 = iStart1+strlen(Pick3_Names[kCombIdx[i]]);
	      	    }else
	              getSecondStringPick3TX(lpFieldsResults1, iStop1, iStop, &iStart1, &iStop1);
*/

//       printf ("2--------------------------------------------iStart1= %d, iStop1= %d\n",iStart1, iStop1);

              if(iStart1< iStop1) {
           	     copyLineWithSpace(lpFieldsResults1, iStart1, iStop1);
//           	    printf ("------result= %s\n",result);
//           	  	           {
//           	  	        	   int iiK;
//           	  	        	   for(iiK=0;iiK<strlen(result);iiK++){
//           	  	        		    printf ("------iiK= %d, result[iiK]= %c, flConf[iiK]= %d\n",iiK, result[iiK], flConf[iiK]);
//           	  	        	   }
//           	  	           }
	             matchPickTerms();
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

/*
     typeOfLotteryTXT[0] = "LOTTO TEXAS"; //8
    typeOfLotteryTXT[1] = "PICK 3";  //9
    typeOfLotteryTXT[2] = "CASH FIVE"; //10
    typeOfLotteryTXT[3] = "TWO STEP"; //12
    typeOfLotteryTXT[4] = "DAILY 4"; //13
    typeOfLotteryTXT[5] = "POWER BALL"; //14
    typeOfLotteryTXT[6] = "MEGA MILLIONS"; //15
    typeOfLotteryTXT[7] = "ALL OR NOTHING"; //16

 */
int findWorst(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	 char *resultsStr = lpFieldsResults->result;
	 int  yy, th = 7, thMax = 20;
	 int min, iMin, max, iMax;
	 int i, ccMin, ccMax;
	 int iCnf, minCnf;

	  min = 500; iMin = -1; max = 0; iMax = -1;
	  minCnf = 101;iCnf=-1;
	  for(i=iStart;i<=iStop;i++){
// find min, max for y
		  if(resultsStr[i]>32){
			  yy = lpFieldsResults->flQuad[i].p3Y - lpFieldsResults->flQuad[i].p1Y;
			  if(min> yy) {min = yy; iMin = i;}
			  if(max < yy) {max = yy; iMax = i;}
			  if(minCnf > lpFieldsResults->flConf[i]) {minCnf=lpFieldsResults->flConf[i];iCnf=i;}

//  printf("yy = %d, resultsStr[i]= %c, cnf[i]= %d\n",yy, resultsStr[i],lpFieldsResults->flConf[i]);
		  }
	  }
//printf("min = %d, max= %d\n",min, max);

// which one is wrong
	  ccMin = 0; ccMax = 0;
	  for(i=iStart;i<=iStop;i++){
			  if(resultsStr[i]>32){
				  yy = lpFieldsResults->flQuad[i].p3Y - lpFieldsResults->flQuad[i].p1Y;
				  if(max-yy< thMax) ccMax++;
				  if(yy-min< th) ccMin++;
			  }
	  }
//  printf("ccMax = %d, ccMin= %d\n",ccMax, ccMin);
//  printf("resultsStr[iMax]= %c, lpFieldsResults->flConf[iMax] = %d\n",resultsStr[iMax],lpFieldsResults->flConf[iMax]);
//  printf("resultsStr[iMin]= %c, lpFieldsResults->flConf[iMin] = %d\n",resultsStr[iMin],lpFieldsResults->flConf[iMin]);

	  if(ccMax < ccMin && lpFieldsResults->flConf[iMax] < 25)  {/*printf("lpFieldsResults->flConf[iMax] = %d\n",lpFieldsResults->flConf[iMax]);*/return(iMax);}
	  if(ccMax >= ccMin && lpFieldsResults->flConf[iMin] < 25)  {/*printf("lpFieldsResults->flConf[iMin] = %d\n",lpFieldsResults->flConf[iMin]);*/return(iMin);}
	  if(minCnf < 25) return(iCnf);
      return -1;
}
void cleanOneDigitGroupTX(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
 char *resultsStr = lpFieldsResults->result;
 int i, iW, k, cc, k1, k2;
 int nn;
	  for(k=0;k<4;k++){
	     for(i=iStart;i<=iStop;i++){
			if(resultsStr[i]>32) {iStart = i;break;}
	     }
	     for(i=iStop;i>=iStart;i--){
		    if(resultsStr[i]>32) {iStop = i;break;}
  	     }
//  printLine(resultsStr, iStart, iStop);


//  for(i=iStart;i<=iStop;i++){
//	   printf("resultsStr[i]= %c, lpFieldsResults->flConf[i] = %d\n",resultsStr[i],lpFieldsResults->flConf[i]);
//  }
//printf("iStart= %d, iStop= %d\n",iStart, iStop);
	     if(iStop - iStart+1 > 2){
		    iW = findWorst(lpFieldsResults, iStart, iStop);
 //printf("iW= %d\n",iW);
		    if(iW<0) {
		    	nn = iStop- iStart+1;
//		    	printf("nn= %d\n",nn);
		    	if(nn == 3){
		    		iW = -1;
//  	   printf("iStart= %d, resultsStr[iStart]= %c, iStop = %d, resultsStr[iStop]= %c, iW= %d\n",iStart, resultsStr[iStart], iStop, resultsStr[iStop], iW);
		    		if(lpFieldsResults->flConf[iStop]-lpFieldsResults->flConf[iStart] > 20) iW = iStart;
//	   printf("lpFieldsResults->flConf[iStop]-lpFieldsResults->flConf[iStart]= %d, iW = %d\n",lpFieldsResults->flConf[iStop]-lpFieldsResults->flConf[iStart], iW);
		    		if(lpFieldsResults->flConf[iStart]-lpFieldsResults->flConf[iStop] > 20) iW = iStop;
//	   printf("lpFieldsResults->flConf[iStart]-lpFieldsResults->flConf[iStop]= %d, iW = %d\n",lpFieldsResults->flConf[iStart]-lpFieldsResults->flConf[iStop], iW);
		    		if(iW >=0 ) {
//   printf("iW= %d, resultsStr[iW]= %c\n",iW, resultsStr[iW]);
		    			resultsStr[iW] = ' ';
		    		}
		    	}
	    	    return;
		    }
		    resultsStr[iW] = ' ';
// printf("resultsStr= %s\n",resultsStr);
         }

	     else break;
	     cc = 0;k1 = iStart; k2 = iStop;
	     for(i=k1;i<=k2;i++){
		     if(resultsStr[i]>32) {
		    	 if(cc == 0) iStart = i;
		    	 cc++;
		     }
		     else {
		    	 if(cc >2) {iStop = i;break;}
   			     cc = 0;
		     }
         }
//    printf("iStart= %d, iStop= %d, cc= %d\n",iStart, iStop, cc);
	     if(cc <= 2) break;
      }
}

void cleanTooManyDigitsInGroupTX(LPFIELDSRESULTS lpFieldsResults){
 char *resultsStr = lpFieldsResults->result;
 int i, k, iStart, iStop;
 int ccSp, ccD;

 i = 0;
 while(1){
	if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
// printf("--------- line= %d\n", i);
// fflush(stdout);

	ccSp = 0; ccD = 0;
	for(k=iStart+1;k<iStop;k++){
		if(resultsStr[k] == 32) {
			if(ccSp == 0 && ccD > 2) {
//printLine(resultsStr, iStart, iStop);
//printf(" ccD= %d, k= %d\n", ccD, k);
				cleanOneDigitGroupTX(lpFieldsResults, k-ccD, k-1);
//				printf("--------cleanTooManyDigitsInGroupTX;--ccD= %d\n",ccD);
				linesPostCnf++;
			}
			ccSp++;
			ccD = 0;
	    }
		else{
			 ccD++;
			ccSp = 0;
		}
//printf(" ccD= %d, k= %d\n", ccD, k);
		if(ccSp == 0 && ccD > 2) {
//printLine(resultsStr, iStart, iStop);
//printf(" ccD= %d, k= %d\n", ccD, k);
			cleanOneDigitGroupTX(lpFieldsResults, k-ccD, k);
//				printf("--------cleanTooManyDigitsInGroupTX;--ccD= %d\n",ccD);
			linesPostCnf++;
		}


	}
	i++;
 }
}
int getNumberFromStop(char *resultsStr, int iStart, int iStop, int n, int *nStart, int *nStop){
	int i, k, k1, k2, kStart, cc;
	int numb;
	char strN[10];

	*nStart = -1; *nStop = -1;
	k2 = k1 = iStop; cc = 0;
    while(k1>=iStart) {
       k2 = k1;
       for(i=k1;i>=iStart;i--){
	      if(resultsStr[i] > 32) {k2=i;break;}
       }
       k1 = k2;
       for(i=k2;i>=iStart;i--){
	      if(resultsStr[i] == 32 || i == iStart) {k1=i;break;}
       }
//printf("k1= %d, k2= %d, cc= %d, n= %d\n", k1, k2, cc, n);
       if(cc >= n) break;
       cc++;
    }
    numb = 0;
    if(k1<k2 && cc==n){
    	kStart = k1;
    	if(k1 > iStart)
    		kStart++;
       k = 0;
       for(i=kStart;i<=k2;i++)
  	      strN[k++] = resultsStr[i];
       strN[k] = 0;
       sscanf(strN,"%d",&numb);
       *nStart = kStart; *nStop = k2;
//  printf("strN= %s, numb= %d\n", strN, numb);
    }
    return numb;
}
void cleanLastNumber(char *resultsStr, int iStart, int iStop){
	int i, k;
	k = iStop;
    for(i=iStop;i>iStart;i--){
        if(resultsStr[i] > 32) {k=i;break;}
    }
//printf("iStop= %d, resultsStr[ %d ]= %c\n",iStop,k,resultsStr[k]);
    for(i=k;i>iStart;i--){
        if(resultsStr[i] == 32) break;
        resultsStr[i] = ' ';
    }
}
void cleanFirstNumber(char *resultsStr, int iStart, int iStop){
	int i, k, iSt;
	iSt = iStart;
//printf("iStart= %d, resultsStr[ %d ]= %c, resultsStr[ %d ]= %c, resultsStr[ %d ]= %c\n",iStart,iSt,resultsStr[iSt],iSt+1,resultsStr[iSt+1],iSt+2,resultsStr[iSt+2]);
	if(resultsStr[iSt] != 32 && resultsStr[iSt+1] == 32 && resultsStr[iSt+2] != 32){
	   k = iSt+2;
       for(i=k;i<iStop;i++){
           if(resultsStr[i] == 32) break;
           resultsStr[i] = ' ';
       }
	}
}

int getCnfFirstGroup(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	char *resultsStr = lpFieldsResults->result;
	int  iSt, cnf1 = 0, cnf2;
	if(iStop - iStart < 6) return 0;
	iSt = iStart;
	if(resultsStr[iSt+1] == 32 && resultsStr[iSt+2] != 32 && resultsStr[iSt+3] != 32 && resultsStr[iSt+4] == 32){
		cnf1 = lpFieldsResults->flConf[iSt+2];
		cnf2 = lpFieldsResults->flConf[iSt+3];
//		printf("first cnf1= %d, cnf2= %d\n",cnf1, cnf2);
		if(cnf1 > cnf2) cnf1 = cnf2;
	}
	return cnf1;
}
int getCnfLastGroup(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){
	char *resultsStr = lpFieldsResults->result;
	int  iSt, cnf1 = 0, cnf2;
	if(iStop - iStart < 6) return 0;
	iSt = iStop;
	if(resultsStr[iSt] <= 32) iSt--;
	if(resultsStr[iSt] != 32 && resultsStr[iSt-1] != 32 && resultsStr[iSt-2] == 32){
		cnf1 = lpFieldsResults->flConf[iSt];
		cnf2 = lpFieldsResults->flConf[iSt-1];
//		printf(" last cnf1= %d, cnf2= %d\n",cnf1, cnf2);
		if(cnf1 > cnf2) cnf1 = cnf2;
	}
	return cnf1;
}

void repairGroupsTX(LPFIELDSRESULTS lpFieldsResults, int line, int numLines, int grPerType){
	char *resultsStr = lpFieldsResults->result;
	int iStart, iStop, i, k1, k2;
	int firstS, firstE;
	int n, n1, n2;
	char str1[4], str2[4];

   if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
// check order at the beginning
	  k1 = iStart;
      if(resultsStr[k1] > 32 && resultsStr[k1+1] == 32)
    	  k1++;

      k2 = k1;
      for(i=k2;i<iStop;i++){
	      if(resultsStr[i] > 32) {k1=i;break;}
      }
      k2 = k1;
      for(i=k1;i<iStop;i++){
	      if(resultsStr[i] == 32) {k2=i;break;}
      }
      n = 0;
      for(i=k1;i<k2;i++){
    	  str1[n++] = resultsStr[i];
      }
      str1[n] = 0;
      firstS = k1; firstE = k2-1;

	  k1 = k2;
      for(i=k2;i<iStop;i++){
	      if(resultsStr[i] > 32) {k1=i;break;}
      }
      k2 = k1;
      for(i=k1;i<iStop;i++){
	      if(resultsStr[i] == 32) {k2=i;break;}
      }
      n = 0;
      for(i=k1;i<k2;i++){
    	  str2[n++] = resultsStr[i];
      }
      str2[n] = 0;
      sscanf(str1,"%d",&n1);sscanf(str2,"%d",&n2);
//printf("str1= %s, n1= %d, str2= %s, n2= %d\n", str1, n1, str2, n2);
      if(n1 > n2 || n1 == 0){
// delete first group
	      for(i=firstS;i<=firstE;i++){
		     if( resultsStr[i] > 32)
		        resultsStr[i] = 32;
		     }
	      return;
      }


// last group
      k1 = iStop;
      for(i=iStop;i>iStart;i--){
	      if(resultsStr[i] > 32) {k1=i;break;}
      }
      k2 = iStop;
      for(i=k1;i>iStart;i--){
	      if(resultsStr[i] == 32) {k2=i;break;}
      }
      if(k2> 8 && k2 < iStop-1){
	 	switch(ticketTypeInt){
	      case 4: // TWO STEP
	      case 1: // POWER BALL
		  case 2: // MEGA MILLIONS
		  { int cc = 0;
		      for(i=k2;i<=iStop;i++){
			     if( resultsStr[i] > 32) cc++;
			     }
//     printf("--- cc= %d\n",cc);
		      if(cc > 1){
		    	  if(resultsStr[k2-1] > 32 && resultsStr[k2-2] == 32){
//     printf("--- resultsStr[k2-1]= %c\n",resultsStr[k2-1]);
				        resultsStr[k2-1] = 32;
		    	  }
		    	  break;
		      }
		      for(i=k2;i<=iStop;i++){
			     if( resultsStr[i] > 32)
			        resultsStr[i] = 32;
			     }
		  }
			  break;
		  default:
		      for(i=k2;i<=iStop;i++){
			     if( resultsStr[i] > 32)
			        resultsStr[i] = 32;
			     }
	 	}
      }
      {
        int  groupNum = getGroupsFromLine(resultsStr, iStart, iStop);
        int n1,n2, iN1, iN2;
        if(groupNum >grPerType){

// printLine(resultsStr, iStart, iStop);
        	n1= getNumberFromStop(resultsStr, iStart, iStop, 1, &iN1, &iN2);
        	n2= getNumberFromStop(resultsStr, iStart, iStop, 2, &iN1, &iN2);

//printf("n1 = %d, n2 = %d\n", n1, n2);
        	if(n2>n1) cleanLastNumber(resultsStr, iStart, iStop);
        }

        groupNum = getGroupsFromLine(resultsStr, iStart, iStop);
        if(groupNum >grPerType){
        	n1 = getCnfFirstGroup(lpFieldsResults, iStart, iStop);
        	n2 = getCnfLastGroup(lpFieldsResults, iStart, iStop);
//printf("n1= %d, n2= %d, n1-n2= %d\n",n1,n2,n1-n2);
//printLine(resultsStr, iStart, iStop);
            if( n1 - n2 > 30) cleanLastNumber(resultsStr, iStart, iStop);
            if( n2 - n1 > 30) cleanFirstNumber(resultsStr, iStart, iStop);
        }
      }

   }
}
void testFirstGroupTX_C5(LPFIELDSRESULTS lpFieldsResults, int grPerType, int iStart, int iStop){
	 char *resultsStr = lpFieldsResults->result;
	int groupNum, i1, i2, min1, min2;
	groupNum = getGroupsFromLine(resultsStr, iStart, iStop);
	if(groupNum < grPerType || iStop-iStart < 5) return;
	i1 = iStart;
	i2 = iStop; if(resultsStr[i2] <=32) i2--;
	if(resultsStr[i1]>32 && resultsStr[i1+1]>32 && resultsStr[i1+2]>32 && resultsStr[i1+3]==32){
	   min1 = lpFieldsResults->flConf[i1+1]; if(min1 > lpFieldsResults->flConf[i1+2]) min1 = lpFieldsResults->flConf[i1+2];
	   min2 = lpFieldsResults->flConf[i2-1];if(min2 > lpFieldsResults->flConf[i2]) min2 = lpFieldsResults->flConf[i2];
	   if(min1 < min2){
		  resultsStr[i1+1] = ' ';
		  resultsStr[i1+2] = ' ';
	  }
	}
//	if(resultsStr[i1]>32 && resultsStr[i1+1]>32 && resultsStr[i1+2]>32 && resultsStr[i1+3]==32){
//		printf("cnf1= %d, cnf2= %d\n",lpFieldsResults->flConf[i1+1], lpFieldsResults->flConf[i1+2]);
//		printf("cnfN-1= %d, cnfN= %d\n",lpFieldsResults->flConf[i2-1], lpFieldsResults->flConf[i2]);
//	}
}
void stretchResultsWoleField(LPFIELDSRESULTS lpFieldsResults, int st, int shift, char *txt){
	 char *resultsStr = lpFieldsResults->result;
 	 int i, numLines, iStart, iStop;
 	 i = 0;
 	 while(1){
 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
 		i++;
 	 }
 	resultTmp[0] = 0;
 	numLines = i;
    for(i=0;i<numLines;i++){
	    if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	      copyLine(lpFieldsResults, iStart, iStop);
	      if(st > iStart && st < iStop){
	    	    stretchResults(st, shift);
	    	    insResult(st, txt);
	      }
	      addLineToTmp();
    }
    copyResultsBack(lpFieldsResults);
}

void testFirstGroupNoSpaceTX(LPFIELDSRESULTS lpFieldsResults, int grPerType, int iStart, int iStop){
	 char *resultsStr = lpFieldsResults->result;
	 int groupNum, i1;
		groupNum = getGroupsFromLine(resultsStr, iStart, iStop);
		i1 = iStart;
//		printf("groupNum= %d, grPerType= %d, iStop-iStart= %d\n",groupNum,grPerType,iStop-iStart);
		if(groupNum >= grPerType || iStop-iStart < 5) return;
		if(resultsStr[i1]>32 && resultsStr[i1+1]>32 && resultsStr[i1+2]>32 && resultsStr[i1+3]==32){
			  stretchResultsWoleField(lpFieldsResults, i1+1, 1, " ");
		}

}
void testFinalGroups_TX(LPFIELDSRESULTS lpFieldsResults){
	 	 char *resultsStr = lpFieldsResults->result;
	 	 int i, numLines, iStart, iStop;
	 	 int groupNum[50];
	 	 int grPerType = 0;
	 	 int k;

//	 	 resultTmp[0] = 0;

// too many groups
	 	 i = 0;
	 	 while(1){
	 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//printf("--- groupNum[ %d ]= %d\n",i,groupNum[i]);
	 		i++;
	 	 }
	 	numLines = i;

	 	switch(ticketTypeInt){
		  case 3: // LOTTO TEXAS
	      case 1: // POWER BALL
		  case 2: // MEGA MILLIONS
		  case 8: // ALL OR NOTHING
			grPerType = 6;break;
          case 7: // CASH FIVE
      	  case 4: // TWO STEP
   	        grPerType = 5;break;

/*
		  case 0: // LOTTO TEXAS
	      case 5: // POWER BALL
		  case 6: // MEGA MILLIONS
		  case 7: // ALL OR NOTHING
			grPerType = 6;break;
          case 2: // CASH FIVE
      	  case 3: // TWO STEP
   	        grPerType = 5;break;
*/
	 	}
      for(i=0;i<numLines;i++){
	    if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
//printLine(resultsStr, iStart, iStop);
	 	switch(ticketTypeInt){
	 	  case 7: // CASH FIVE
	 		  testFirstGroupTX_C5(lpFieldsResults, grPerType, iStart, iStop);
	 		  break;
	 	}
	 	testFirstGroupNoSpaceTX(lpFieldsResults, grPerType, iStart, iStop);
//printLine(resultsStr, iStart, iStop);
   	    for(k=0;k<4;k++) {
   	      if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
     	  groupNum[i] = getGroupsFromLine(resultsStr, iStart, iStop);
//printf("i= %d, k= %d, groupNum[i]= %d, grPerType= %d\n",i,k, groupNum[i],grPerType);
//printLine(resultsStr, iStart, iStop);
      	  if(groupNum[i] > grPerType){
      		  repairGroupsTX(lpFieldsResults, i, numLines, grPerType);
      	      trimSpaces(lpFieldsResults);
      	  }
      	  else{
      		if(groupNum[i] == grPerType && resultsStr[iStart+1] != 32){
      			resultsStr[iStart+1] = ' ';
      		}
   		    break;
      	  }
    	}
      }

      for(i=0;i<numLines;i++){
	    if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
     	  groupNum[i] = getGroupsFromLine(resultsStr, iStart, iStop);
//printf("i= %d, groupNum[i]= %d, grPerType= %d\n",i,groupNum[i],grPerType);
//printLine(resultsStr, iStart, iStop);
      	  if(groupNum[i] > grPerType){
           	 cleanOneDigit_TwoStep(resultsStr, iStart, iStop);
    	     trimSpaces(lpFieldsResults);
      	  }
      }

}

void testGroupsAgain(LPFIELDSRESULTS lpFieldsResults, int gr2){
	 	 char *resultsStr = lpFieldsResults->result;
	 	 int i, k, numLines, iStart, iStop;
         int ccD, ccS;


	 	 i = 0;
	 	 while(1){
	 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
	 		i++;
	 	 }
	 	numLines = i;

// first line
 		if(getLine(resultsStr, 0, &iStart, &iStop) >= 0) {
           testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);
           if(ccD < gr2 || testOnesInsideLine(resultsStr, iStart, iStop)>0){
// delete line
//	printf("--------delete line;--ccD= %d, ticketTypeInt= %d\n",ccD, ticketTypeInt);
       	    linesPostCnf++;
       	    if(ticketTypeInt != 8) {
        	   i = 1;
       	       while(1){
       		      if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
       		      resultsStr[iStart] = 'A'+i-1;
       		      i++;
       	       }
  		       if(getLine(resultsStr, 0, &iStart, &iStop) >= 0) {
    		       for(i=iStart;i<=iStop;i++)
    			       resultsStr[i] = ' ';
                }
       	    }
       	    else{
         	   i = 1; k = 1;
       	       while(1){
        		      if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
        		      resultsStr[iStart] = 'A'+k-1;
//printf("i= %d, k= %d, resultsStr[iStart]= %c\n",i,k,resultsStr[iStart]);
        		      if(i%2 == 0) k++;
        		      i++;
//printf("i= %d, k= %d\n",i,k);
       	       }
   		       if(getLine(resultsStr, 0, &iStart, &iStop) >= 0) {
     		       for(i=iStart;i<=iStop;i++)
     			       resultsStr[i] = ' ';
                 }

       	    }
  		   numLines--;
  		   }
 		}

// last line
 		if(getLine(resultsStr, numLines-1, &iStart, &iStop) >= 0) {
           testGroupsTX(resultsStr, iStart, iStop, &ccD, &ccS);


//printLine(resultsStr, iStart, iStop);
//printf ("--------i= %d, ccD= %d, ccS= %d\n", i,  ccD, ccS);
// delete line
           if(ccD < gr2 || testOnesInsideLine(resultsStr, iStart, iStop)>0){
//			printf("2--------delete line;--ccD= %d\n",ccD);
        	  linesPostCnf++;
    		  for(i=iStart;i<=iStop;i++)
    			  resultsStr[i] = ' ';
            }
 		}
 		ccD=ccS; // to avoid compilation warning
}
void testVNumber(LPFIELDSRESULTS lpFieldsResults, int line, int n, int iN1,  int maxNum){
	 char *resultsStr = lpFieldsResults->result;
     if(n > maxNum){
    	 int i, iStart, iStop;
    	  if(getLine(resultsStr, line, &iStart, &iStop) < 0) return;
		  for(i=iN1;i<iN1+2;i++){
			  lpFieldsResults->flConf[i] = 0;
		  }
		  if(maxNum < 20) {
// printf("maxNum= %d, iN1= %d, resultsStr[iN1]= %c\n",maxNum, iN1,resultsStr[iN1]);
//	printf("--------maxNum= %d\n",maxNum);
			 linesPostCnf++;
		     if(resultsStr[iN1] == '9') resultsStr[iN1] = '0';
		     if(resultsStr[iN1] == '8') resultsStr[iN1] = '0';
		     if(resultsStr[iN1] == '7') resultsStr[iN1] = '1';
		     if(resultsStr[iN1] == '6') resultsStr[iN1] = '0';
		     if(resultsStr[iN1+1] == '6') resultsStr[iN1] = '5';
		     if(resultsStr[iN1+1] == '7') resultsStr[iN1] = '1';
		  }else{
//		     if(resultsStr[iN1] == '9') resultsStr[iN1] = '0';
//		     if(resultsStr[iN1] == '8') resultsStr[iN1] = '6';
		  }
     }

}
void testMaxValueNumbers_TX(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
	 int i, k, n, iN1, iN2, iStart, iStop;
	 int ok = 0;
	 int lastMaxNum = 0, insideMaxNum = 0;

 	switch(ticketTypeInt){
	  case 2: // MEGA MILLIONS
		  ok = 1;
		  lastMaxNum = 15; insideMaxNum = 75;
		  break;
 	}
 	if(ok == 0) return;

	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		k = 0;
		while(1) {
    	   n = getNumberFromStop(resultsStr, iStart, iStop, k, &iN1, &iN2);
//printf("iN1= %d, iN2= %d, n= %d\n",iN1,iN2,n);
    	   if(n<=0) break;
    	   if(iN1+1 == iN2) {
    	      if(k==0) {testVNumber(lpFieldsResults, i, n, iN1, lastMaxNum);}
    	      else {testVNumber(lpFieldsResults, i, n, iN1, insideMaxNum);}
    	   }
   		   k++;
		}

		i++;
	 }
}
int getGroups_1FromLine(char *resultsStr, int iStart, int iStop){
	int i, ccG, ccGgr, iSt = iStart;
	ccGgr = 0;
	ccG = 0;
	for(i=iStart;i<iStop;i++){
		if(resultsStr[i] == 32) {iSt = i;break;}
	}
	for(i=iSt;i<iStop;i++){
		if(resultsStr[i] > 32) {
			ccGgr++;}
		else{
			if(ccGgr == 1) ccG++;
			 ccGgr = 0;
		}
	}
	if(ccGgr == 1) ccG++;
    return ccG;
}

int getGroups_2FromLine(char *resultsStr, int iStart, int iStop){
	int i, ccG, ccGgr, iSt = iStart;
	int cc0;
	ccGgr = 0;
	ccG = 0;
	for(i=iStart;i<iStop;i++){
		if(resultsStr[i] == 32) {iSt = i;break;}
	}

	cc0 = 0;
	for(i=iSt+1;i<iStop;i++){
		if(resultsStr[i] > 32) {
			ccGgr++;
			if(resultsStr[i] == '0') cc0++;
		}
		else{
			if(ccGgr == 2 && cc0 != 2) ccG++;
			 ccGgr = 0;
			 cc0 = 0;
		}
	}
	if(ccGgr == 2) ccG++;
    return ccG;
}
int getGroups_2FromLineResult(void){
	int i, ccG, ccGgr;
	int len = strlen(result);
	ccGgr = 0;
	ccG = 0;
	for(i=0;i<len;i++){
		if(result[i] > 32) {
			ccGgr++;}
		else{
			if(ccGgr == 2) ccG++;
			 ccGgr = 0;
		}
	}
	if(ccGgr == 2) ccG++;
    return ccG;
}
int getGroups_OtherThan2FromLine(char *resultsStr, int iStart, int iStop){
	int i, ccG, ccGgr;
	ccGgr = 0;
	ccG = 0;
	for(i=iStart+1;i<=iStop;i++){
		if(resultsStr[i] > 32) {
			ccGgr++;
		}
		else{
			if(ccGgr > 0 && ccGgr != 2) ccG++;
			 ccGgr = 0;
		}
	}
	if(ccGgr > 0 && ccGgr == 2) ccG++;
    return ccG;
}

void testNumbersConfidence(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
 	 int iStart, iStop;
	int i, thGr, gr_2, gr_OtherThan2;
	  switch(ticketTypeInt){
       case 1: // POWER BALL
       case 2: // MEGA MILLIONS
	   case 3: // LOTTO TEXAS
	    case 8:	// ALL OR NOTHING
	    	thGr =  6;break;
	    case 4: // TWO STEP
        case 7: // CASH FIVE
        	thGr = 5;break;
	    case 5: // PICK 3
	    case 6: // DAILY 4
	    	return;
	    default:
	    	linesPostCnf = 10;
	    	return;
	 }
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		  gr_2 = getGroups_2FromLine(resultsStr, iStart, iStop);
		  gr_OtherThan2 = getGroups_OtherThan2FromLine(resultsStr, iStart, iStop);
		  if(gr_2 != thGr) linesPostCnf++;
		  if(gr_OtherThan2 != 0) linesPostCnf++;
          if( testOnesInsideLine(resultsStr, iStart, iStop)>0) linesPostCnf++;
		  i++;
	 }
}
int calculateTopYfromLine(LPFIELDSRESULTS lpFieldsResults, int iStart, int iStop){

	 char *resultsStr = lpFieldsResults->result;
	 int i, cc, yy;

	 cc = 0; yy = 0;
	 for(i=iStart;i<iStop;i++){
		 if(resultsStr[i]>32) {yy += lpFieldsResults->flQuad[i].p1Y;cc++;}
	 }
	 if(cc>0) yy /= cc;
	 return yy;
}

void testLastLine(LPFIELDSRESULTS lpFieldsResults){
	 char *resultsStr = lpFieldsResults->result;
 	 int i, numLines, iStart, iStop;
	 int gr_2, thGr;
	 int y1, y2;

 	 i = 0;
 	 while(1){
 		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
 		i++;
 	 }
 	numLines = i;
	switch(ticketTypeInt){
       case 1: // POWER BALL
       case 2: // MEGA MILLIONS
	   case 3: // LOTTO TEXAS
	    	thGr =  6;break;
	    case 4: // TWO STEP
        case 7: // CASH FIVE
        	thGr = 5;break;
	    default:
	    	return;
	}

	y1 = -1;
	if(numLines > 1) {
		if(getLine(resultsStr, numLines-2, &iStart, &iStop) >= 0)
		   y1 = calculateTopYfromLine(lpFieldsResults, iStart, iStop);
	}

	 if(getLine(resultsStr, numLines-1, &iStart, &iStop) >= 0) {
// printLine(resultsStr, iStart, iStop);
 			gr_2 = getGroups_2FromLine(resultsStr, iStart, iStop);
//	 printf("-------------gr_2= %d\n",gr_2);
			if(gr_2 < 3) {resultsStr[iStart] = 0;linesPostCnf++;}
			else{
				if(gr_2 < thGr && numLines > 1 && y1 > 0) {
		            y2 = calculateTopYfromLine(lpFieldsResults, iStart, iStop);
//  	   printf("-------------y2-y1= %d\n",y2-y1);
		            if(y2-y1 > 200) {
		               resultsStr[iStart] = 0;linesPostCnf++;
		            }
				}
			}
	 }

}
void testNumbers_InsideLineTX(LPFIELDSRESULTS lpFieldsResults, int gr2){
//printf("0. lpFieldsResults->result= %s\n", lpFieldsResults->result);
	 testLastLine(lpFieldsResults);
//printf("0. lpFieldsResults->result= %s", lpFieldsResults->result);

//printf("1. lpFieldsResults->result= %s", lpFieldsResults->result);
	 cleanTooManyDigitsInGroupTX(lpFieldsResults);
	 trimSpaces(lpFieldsResults);
//printf("2. lpFieldsResults->result= %s\n", lpFieldsResults->result);

	 testFinalGroups_TX(lpFieldsResults);
	 trimSpaces(lpFieldsResults);
//printf("3. lpFieldsResults->result= %s", lpFieldsResults->result);

    testGroupsAgain(lpFieldsResults, gr2);
    trimSpaces(lpFieldsResults);
//printf("4. lpFieldsResults->result= %s", lpFieldsResults->result);

    testMaxValueNumbers_TX(lpFieldsResults);
//printf("5. lpFieldsResults->result= %s", lpFieldsResults->result);
    testNumbersConfidence(lpFieldsResults);
}
void testCnfForSegmentation(LPFIELDSRESULTS lpFieldsResults, int gr2){
	 char *resultsStr = lpFieldsResults->result;
//	 int i, cc = 0, av = 0;
//	 int min, cnf;
//	 min = 100;
// printf ("strlen(resultsStr)=  %d\n", strlen(resultsStr));
// printf ("resultsStr=  %s\n", resultsStr);
 fflush(stdout);
	 if(strlen(resultsStr) < 4){
		 segmentationCnf = 0;
		 if(segmentationSwitchFlag == 1) segmentationSwitchFlag = 0;
		 else segmentationSwitchFlag = 1;
// printf ("segmentationSwitchFlag= %d\n",  segmentationSwitchFlag);
		 return;
	 }
	 segmentationCnf = 1;
	 if(segmentationSwitchFlag == 1) segmentationSwitchFlag = 0;

/*
	 for(i=0;i<strlen(resultsStr);i++){
		 if(resultsStr[i]>32){
			 cnf = lpFieldsResults->flConf[i];
 printf ("resultsStr[i]= %c, cnf= %d\n", resultsStr[i], cnf);
			 av += cnf;cc++;
			 if(min > cnf) min = cnf;
		 }
	 }
	 if(cc>0) av /= cc;
	 printf ("min= %d, av=%d \n", min, av);
*/
}

int  postProcessingNumbers_TX(LPIMGDATA img, LPTICKETRESULTS ticketResults, int i){
	int status = 0;
	LPFIELDSRESULTS lpFieldsResults = &ticketResults->fieldsResults[i];
	/*
	1 - Powerball
	2- Mega Millions
	3- Lotto Texas
	4- Texas Two Steps
	5- Pick 3
	6- Daily 4
	8 - All Or Nothing
	*/

	switch(ticketTypeInt){

	case 3: // LOTTO TEXAS
//	   cleanTicketNumbers(lpFieldsResults);
	   cleanTicketNumbers_LottoTXC5(img, lpFieldsResults, 5);
	   testNumbers_InsideLineTX(lpFieldsResults, 4);
	   break;
	case 5: // PICK 3
	   cleanTicketNumbers_Pick3TX(ticketResults, i,  3);
	   testCnfForSegmentation(lpFieldsResults, 3);
	   break;
	case 7: // CASH FIVE
	   cleanTicketNumbers_LottoTXC5(img, lpFieldsResults, 4);
	   testNumbers_InsideLineTX(lpFieldsResults, 3);
	   break;
	case 4: // TWO STEP
    case 1: // POWER BALL
    case 2: // MEGA MILLIONS
	   cleanTicketNumbers_TwoStep(img, lpFieldsResults);
	   testNumbers_InsideLineTX(lpFieldsResults, 4);
	   break;
	case 6: // DAILY 4
	   cleanTicketNumbers_Pick3TX(ticketResults, i,  4);
	   testCnfForSegmentation(lpFieldsResults, 4);
//	   cleanTicketNumbers(lpFieldsResults);
	   break;
//	case 5:
	//case 6:
//	   cleanTicketNumbers(lpFieldsResults);
//	   break;
	case 8:
	   cleanTicketNumbers_AllorN(img, lpFieldsResults);
	   testNumbers_InsideLineTX(lpFieldsResults, 4);
	   break;

	}
    status = getNumLowConfidentChars(lpFieldsResults, 20);
	return status;
//	printf("------------------------------------------------status= %d\n", status);
}
int matchDayA(char *resultsStr, int iStart, int iStop, char* day1, char* day2){
	int ccF = 0;
	int ccT = 0;
	int shiftIdx;
	ccF = matchToString_1(resultsStr, iStart, iStop, day1, &shiftIdx);
	ccT = matchToString_1(resultsStr, iStart, iStop, day2, &shiftIdx);

	if(ccF<ccT) ccF = ccT;
	if(resultsStr[iStart+3+shiftIdx] != ' ') ccF--;
	return ccF;
}

int matchDayA_All(char *resultsStr, int iStart, int iStop){
	int cc[6];
	int i, max;
	int shiftIdx;
//	extern char *Day_Names[];
	for(i=0;i<6;i++){
	   cc[i] = matchToString_1(resultsStr, iStart, iStop, Day_Names[i+1], &shiftIdx);
	}
	max = 0;
	for(i=0;i<6;i++){
		if(max < cc[i]) max = cc[i];
	}
	if(max >= 0)
	   if(resultsStr[iStart+3+shiftIdx] != ' ') max--;
	return max;
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

int matchDayA_TX(char *resultsStr, int iStart, int iStop){
	int cc = 0, cc1 = 0;
	switch(ticketTypeInt){
	case 5:  // PICK 3
	case 6: // DAILY 4
		cc = matchDayA(resultsStr, iStart, iStop, "DAY", "NIGHT");break;
	case 7: // CASH FIVE
		cc = matchDayA_All(resultsStr, iStart, iStop);break;
	case 4: // TWO STEP
		cc = matchDayA(resultsStr, iStart, iStop, "THU", "MON");break;
	case 3: // LOTTO TEXAS
	case 1: // POWER BALL
		cc = matchDayA(resultsStr, iStart, iStop, "SAT", "WED");break;
	case 2: // MEGA MILLIONS
		cc = matchDayA(resultsStr, iStart, iStop, "FRI", "TUE");break;
	case 8: // ALL OR NOTHING
		cc = matchDayA_All(resultsStr, iStart, iStop);break;
//		cc = matchDayA(resultsStr, iStart, iStop, "DAY", "NIGHT");break;
//		cc1 = matchDayA(resultsStr, iStart, iStop, "MORNING", "EVENING");break;
		if(cc1 > cc) cc = cc1;
	   break;
	}
	return cc;
}
void cleanDate(char* day1, char* day2){
	int kM1, cc1;
	int kM2, cc2;
// day
	cc1 = matchToStringResult(day1, &kM1);
	cc2 = matchToStringResult(day2, &kM2);
	if(cc1 > cc2) {
	   if(kM1 > 0) {shrinkResults(0, kM1-1);cutResults(kM1-1);}
	   result[0] = day1[0];result[1] = day1[1];result[2] = day1[2];
	}
	if(cc1 < cc2) {
	   if(kM2 > 0) {shrinkResults(0, kM2-1);cutResults(kM2-1);}
	   result[0] = day2[0];result[1] = day2[1];result[2] = day2[2];
	}
	cleanMonth(&result[4]);
}
void cleanDate_All(){
	int kM[6], cc[6];
	int i, max, iMax;

// day
	for(i=0;i<6;i++){
	  cc[i] = matchToStringResult(Day_Names[i+1], &kM[i]);
	}

	max = 0; iMax = -1;
	for(i=0;i<6;i++){
		if(max < cc[i] ) {max = cc[i];iMax = i;}
	}
	if(iMax >= 0) {
	   if(kM[iMax] > 0) {shrinkResults(0, kM[iMax]-1);cutResults(kM[iMax]-1);}
	   result[0] = Day_Names[iMax+1][0];result[1] = Day_Names[iMax+1][1];result[2] = Day_Names[iMax+1][2];
	}
	cleanMonth(&result[4]);
}

int matchToFlexString_Idx(char *strLine, int strLineSh, char *str, int strSh,  int *idx1, int *idx2){
	   int i, kLineSh, kStrSh, n, cc, ccMax;
	   int len1 = strlen(strLine), len2 = strlen(str);

//printf(" str= %s\n",str);

	   if(strLineSh >= len1) strLineSh = len1-1;
	   if(strSh >= len2) strSh = len2-1;

	   ccMax = 0;*idx1 = 0;*idx2 = 0;
	   for(kLineSh=0;kLineSh<strLineSh;kLineSh++) {
//  printf("-----------kLineSh= %d\n", kLineSh);
		   for(kStrSh=0;kStrSh<strSh;kStrSh++) {
//  printf("---------kStrSh= %d\n", kStrSh);
	          cc = 0;
	          for(i=kStrSh;i<len2;i++){
	    	     n = i-kStrSh+kLineSh;
	    	     if(n >= len1) break;
		         if(strLine[n] == str[i]) cc++;
//    printf("n= %d, strLine[n]= %c, i= %d, str[i]= %c, cc= %d\n",n,strLine[n], i, str[i], cc);
	          }
	          if(ccMax < cc) {ccMax = cc;*idx1 = kLineSh;*idx2 = kStrSh;}
		   }
	   }
    return ccMax;
}

int matchDayAndMonth_Left(){
   char strD_M[40];
   int kM1, kM2, cc;
   int  max, iMax, kMx1, kMx2;
   int i, j ;
   char bestStr[100];

// DayMonth
   max = 0; iMax = -1; kMx1 = 0; kMx2 = 0;
//    k = 0;
	for(i=0;i<6;i++){
		for(j=0;j<12;j++){
//			if(k >= 100) break;
		    strcpy(strD_M,Day_Names[i+1]);
			strcat(strD_M, Month_Names[j]);
			cc = matchToFlexString_Idx(result, 5, strD_M, 2, &kM1, &kM2);
			if(max < cc ) {max = cc;kMx1 = kM1; kMx2 = kM2;strcpy(bestStr, strD_M);}
//	printf(" k= %d, cc[k]= %d, kM1[k]= %d, kM2[k]= %d, strD_M= %s\n",k, cc[k], kM1[k], kM2[k], strD_M);
//			k++;
		}
	}
//	max = 0; iMax = -1; kMx1 = 0; kMx2 = 0;
//	ver = 0;
//	for(i=0;i<k;i++){
//		if(max < cc[i] ) {max = cc[i];iMax = i;kMx1 = kM1[i]; kMx2 = kM2[i];}
//	}
//printf("1 max= %d, iMax= %d, kMx1= %d, kMx2= %d\n",max, iMax, kMx1, kMx2);
//    printf("1 bestStr= %s\n",bestStr);

// Day Month
//    k = 0;
	for(i=0;i<6;i++){
		for(j=0;j<12;j++){
//			if(k >= 100) break;
		    strcpy(strD_M,Day_Names[i+1]);
			strcat(strD_M, " ");
			strcat(strD_M, Month_Names[j]);
			cc = matchToFlexString_Idx(result, 5, strD_M, 2, &kM1, &kM2);
			if(max < cc ) {max = cc;kMx1 = kM1; kMx2 = kM2;strcpy(bestStr, strD_M);}

//printf(" k= %d, cc[k]= %d, kM1[k]= %d, kM2[k]= %d, strD_M= %s\n",k, cc[k], kM1[k], kM2[k], strD_M);
//			k++;
		}
	}
//	for(i=0;i<k;i++){
//		if(max < cc[i] ) {max = cc[i];iMax = i;kMx1 = kM1[i]; kMx2 = kM2[i];ver=1;}
//	}
//	printf("2 max= %d, iMax= %d, kMx1= %d, kMx2= %d\n",max, iMax, kMx1, kMx2);
	kM1 = kMx1;kM2 = kMx2; i = iMax;
//    printf("2 bestStr= %s\n",bestStr);
    strcpy(result, bestStr);


	return 0;

}
int cleanDate_AllTX(){
	int kM[6], cc[6];
	int i, max, iMax, kMx;

	for(i=0;i<6;i++){
//	  cc[i] = matchToStringResult(Day_Names[i+1], &kM[i]);
	  cc[i] = matchToString_Idx(result, 0, strlen(result), Day_Names[i+1], &kM[i]);
// printf("cc[ %d ]= %d, Day_Names= %s, kM= %d\n",i,cc[i],Day_Names[i+1], kM[i]);
	}

	max = 0; iMax = -1;kMx = 0;
	for(i=0;i<6;i++){
		if(max < cc[i] ) {max = cc[i];iMax = i;kMx = kM[i];}
		if(max == cc[i] && kMx>kM[i]) {max = cc[i];iMax = i;kMx = kM[i];}
	}
// printf("max= %d, iMax= %d\n",max, iMax);
	if(iMax >= 0) {
//	printf( "  kM[iMax]= %d\n ", kM[iMax]);
	   if(kM[iMax] > 0) {shrinkResults(0, kM[iMax]-1);cutResults(kM[iMax]-1);}
	   result[0] = Day_Names[iMax+1][0];result[1] = Day_Names[iMax+1][1];result[2] = Day_Names[iMax+1][2];
	}
	cleanMonth(&result[3]);
	if(iMax < 0) iMax = 0;
	return kM[iMax];
}


int cleanDate_TX(){
	 return matchDayAndMonth_Left();

//	return cleanDate_AllTX();
}



int  postProcessingDateAPlay_TX(LPTICKETRESULTS ticketResults, int iField, int *iShift){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, k, kk;
	 int iStart, iStop;
	 int line;
	 int cc, cc1, ccMax;
	 int lineMax;
	 int ccMaxTab[50];
	 int matchCnf, lineTop, lineBottom;

	 lpFieldsResults = &ticketResults->fieldsResults[iField];
	 resultsStr = lpFieldsResults->result;
	 *iShift = 0;
	 resultTmp[0] = 0;
	 i = 0;
	 line = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		ccMax = 0;
		cc =matchToStringResult("MEGAPLIER ", &kk);
		if(kk > 4) cc = 0;
		cc1 =matchToStringResult("MEGA PLIER ", &kk);
		if(kk > 4) cc1 = 0;
		if(cc1>cc) cc = cc1;
		if(cc > ccMax) ccMax = cc;
		cc =matchToStringResult("POWERPLAY ", &kk);
		if(kk > 4) cc = 0;
		cc1 =matchToStringResult("POWER PLAY ", &kk);
		if(kk > 4) cc1 = 0;
		if(cc1>cc) cc = cc1;
		if(cc > ccMax) ccMax = cc;
		cc =matchToStringResult("POWERPMY ", &kk);
		if(kk > 4) cc = 0;
		if(cc > ccMax) ccMax = cc;
	    if(i< 50)
	       ccMaxTab[i] = ccMax;
		i++;
	 }
	 k = i;
	 ccMax = 0;lineMax = 0;
	 for(i=0;i<k;i++){
//printf("-------ccMaxTab[ %d ]= %d\n",i, ccMaxTab[i]);
		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;}
	 }
 //printf("-------lineMax= %d\n",lineMax);
    lineMax++;

    matchCnf = 1;
    if(lineMax>= k || ccMax < 4) matchCnf = 0;
    multiDraw = 0;
    if(matchCnf == 1){
// test for Multi Draw
       multiDraw = testForMultiDraw_1(lpFieldsResults, lineMax, &lineTop, &lineBottom);
    }
// printf("matchCnf= %d, multiDraw= %d\n",matchCnf,multiDraw);
    if(multiDraw == 1) {
       lineMax = lineTop;
       addFieldDateTo_AllOrN(ticketResults, lineBottom, iField);
    }
    resultTmp[0] = 0;


	if(getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {
	         copyLine(lpFieldsResults, iStart, iStop);
	         *iShift = cleanDate_TX();
	         if(result[3] != 32) {
	   	       stretchResults(3, 1);
	   	       insResult(3, " ");
	         }
	         addLineToTmp();
	         line = lineMax;
		}
//printf("-------resultTmp= %s\n",resultTmp);
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}

int  postProcessingDateAEXTRA_TX(LPTICKETRESULTS ticketResults, int iField, int *iShift){
 	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, k, kk;
	 int iStart, iStop;
	 int line;
	 int cc, cc1, ccMax;
	 int lineMax;
	 int ccMaxTab[50];
	 int matchCnf, lineTop, lineBottom;

	 lpFieldsResults = &ticketResults->fieldsResults[iField];
	 resultsStr = lpFieldsResults->result;
	 *iShift = 0;
	 resultTmp[0] = 0;
	 i = 0;
	 line = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		ccMax = 0;
		cc =matchToStringResult("EXTRA NO", &kk);
		cc1 =matchToStringResult("EXTRA YES", &kk);
		if(cc1>cc) cc = cc1;
	    if(cc > ccMax) ccMax = cc;
	    if(i< 50)
	       ccMaxTab[i] = ccMax;
		i++;
	 }
	 k = i;
	 ccMax = 0;lineMax = 0;
	 for(i=0;i<k;i++){
//printf("-------ccMaxTab[ %d ]= %d\n",i, ccMaxTab[i]);
		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;}
	 }
// printf("-------lineMax= %d\n",lineMax);
    lineMax++;
//printf("-------lineMax= %d, k= %d, ccMax= %d\n",lineMax,k,ccMax);
    if(lineMax>= k || ccMax < 4) {
      	 i = 0;
      	 line = -1;
      	 while(1){
      		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
      		copyLine(lpFieldsResults, iStart, iStop);
      		ccMax = 0;
      		cc =matchToStringResult(" DRAW", &kk);
      	    if(cc > ccMax) ccMax = cc;
      	    if(i< 50)
      	       ccMaxTab[i] = ccMax;
      		i++;
      	 }
      	 k = i;
   	 ccMax = 0;lineMax = 0;
   	 for(i=0;i<k;i++){
   //printf("-------ccMaxTab[ %d ]= %d\n",i, ccMaxTab[i]);
   		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;}
   	 }
//    printf("-------lineMax= %d\n",lineMax);
    }
    if(lineMax>= k || ccMax < 4) {
   	 i = 0;
   	 line = -1;
   	 while(1){
   		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
   		copyLine(lpFieldsResults, iStart, iStop);
   		ccMax = 0;
   		cc =matchToStringResult(" NO ", &kk);
   		cc1 =matchToStringResult(" YES ", &kk);
   		if(cc1>cc) cc = cc1;
   	    if(cc > ccMax) ccMax = cc;
   	    if(i< 50)
   	       ccMaxTab[i] = ccMax;
   		i++;
   	 }
   	 k = i;
	 ccMax = 0;lineMax = 0;
	 for(i=0;i<k;i++){
//printf("-------ccMaxTab[ %d ]= %d\n",i, ccMaxTab[i]);
		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;}
	 }
// printf("-------lineMax= %d\n",lineMax);
    lineMax++;
    }

// printf("-------lineMax= %d\n",lineMax);
    matchCnf = 1;
    if(lineMax>= k || ccMax < 4) matchCnf = 0;
    multiDraw = 0;
    if(matchCnf == 1){
// test for Multi Draw
       multiDraw = testForMultiDraw_1(lpFieldsResults, lineMax, &lineTop, &lineBottom);
    }
// printf("matchCnf= %d, multiDraw= %d\n",matchCnf,multiDraw);
    if(multiDraw == 1) {
       lineMax = lineTop;
       addFieldDateTo_AllOrN(ticketResults, lineBottom, iField);
    }
    resultTmp[0] = 0;


// printf("2------lineMax= %d, k= %d, ccMax= %d\n",lineMax,k,ccMax);
    if(lineMax>= k || ccMax < 3) {
       strcpy(resultTmp, "???");
       line = -1;
//    printf("-------line= %d\n",line);
    }
    else {
	if(getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {
	         copyLine(lpFieldsResults, iStart, iStop);
// printf("-------result= %s\n",result);
	         *iShift = cleanDate_TX();
//    printf("-------result= %s\n",result);
	         if(result[3] != 32) {
	   	       stretchResults(3, 1);
	   	       insResult(3, " ");
	         }
	         addLineToTmp();
	         line = lineMax;
		}
//printf("-------resultTmp= %s\n",resultTmp);
    }
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}
int matchBestDayMonth(){
	int kM[6], cc[6];
	int i, k, ccM, kMmonth;
	int ccMmax, ccMi, count;
	int max, iMax;

//	printf("------------------result= %s\n",result);

	// day
	for(i=0;i<6;i++){
		  cc[i] = matchToStringResult( Day_Names[i+1], &kM[i]);
		  if(kM[i] >2 ) cc[i] = 0;
//		  printf("day cc[ %d]= %d, kM[i]= %d\n", i, cc[i], kM[i]);
	}
	ccMmax = 0;ccMi = -1;
	for(i=0;i<6;i++){
		if(cc[i] > 1 && kM[i] < 3) {
			for(k=0;k<12;k++) {
				ccM = matchToStringStr(&result[kM[i]], Month_Names[k], &kMmonth);
				if(ccMmax < ccM  && kMmonth > 2 && kMmonth < 5) {ccMmax = ccM; ccMi = k;}
//				printf("k= %d, ccM= %d, kMmonth= %d\n", k, ccM, kMmonth);
			}
			if(ccMi >=0)
				cc[i] += ccMmax;
//			  printf("cc[ %d]= %d, ccMi= %d, ccMmax= %d\n", i, cc[i], ccMi, ccMmax);
		}
	}

	max = 0; iMax = -1;
	for(i=0;i<6;i++){
//  printf("day cc[ %d]= %d\n", i, cc[i]);
		if(max < cc[i]) {max = cc[i]; iMax = i;}
	}
	count = 0;
	for(i=0;i<6;i++){
		if(max == cc[i]) count++;
	}
//	printf("ccMi= %d, count= %d, max= %d, iMax= %d\n", ccMi, count, max, iMax);
//	printf("resultStr= %s\n",resultStr);

	if(iMax >= 0 && count == 1 && ccMi >= 0 && max > 4) {
		return max;
	}
	return 0;
}
int matchBestDayMonth2(){
	   char strD_M[40];
	   int kM1, kM2, cc, kMmax1, kMmax2;
	   int  max;
	   int i, j ;

	// DayMonth
	   max = 0;
	   kMmax1 = 0; kMmax2 = 0;
	//    k = 0;
		for(i=0;i<6;i++){
			for(j=0;j<12;j++){
	//			if(k >= 100) break;
			    strcpy(strD_M,Day_Names[i+1]);
				strcat(strD_M, Month_Names[j]);
				cc = matchToFlexString_Idx(result, 5, strD_M, 2, &kM1, &kM2);
				if(max < cc ) {max = cc; kMmax1 = kM1; kMmax2 = kM2;}
	//	printf(" k= %d, cc[k]= %d, kM1[k]= %d, kM2[k]= %d, strD_M= %s\n",k, cc[k], kM1[k], kM2[k], strD_M);
	//			k++;
			}
		}
//printf("1 max= %d, kMmax1[k]= %d, kMmax2[k]= %d\n",max, kMmax1, kMmax2);
	// Day Month
	//    k = 0;
		for(i=0;i<6;i++){
			for(j=0;j<12;j++){
	//			if(k >= 100) break;
			    strcpy(strD_M,Day_Names[i+1]);
				strcat(strD_M, " ");
				strcat(strD_M, Month_Names[j]);
				cc = matchToFlexString_Idx(result, 5, strD_M, 2, &kM1, &kM2);
				if(max < cc ) {max = cc; kMmax1 = kM1; kMmax2 = kM2;}

	//printf(" k= %d, cc[k]= %d, kM1[k]= %d, kM2[k]= %d, strD_M= %s\n",k, cc[k], kM1[k], kM2[k], strD_M);
	//			k++;
			}
		}
		if(kMmax1 > 4 || kMmax2 > 4) max--;
//		i = kM1;i = kM2;
//	printf("2 max= %d, kMmax1[k]= %d, kMmax2[k]= %d\n",max, kMmax1, kMmax2);

		return max;

}

int testForMultiDraw_1(LPFIELDSRESULTS lpFieldsResults, int lineMax, int *lineTop, int *lineBottom){
	 char *resultsStr = lpFieldsResults->result;
	 int i, iStart, iStop;
	 int line, ccD, kk, ccT, cc1, cc2;
	 int numLines = 0;

// get number of lines
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }

	  line = lineMax;
	  cc1 = 0;
// printf("line= %d, numLines= %d\n",line, numLines);
	  if(line < numLines-2) {
	     line++;
	     if(getLine(resultsStr, line, &iStart, &iStop) < 0) return 0;
	     copyLine(lpFieldsResults, iStart, iStop);
//printf("result= %s\n",result);
	     ccT =matchToStringResult("THRU", &kk);
	     if(kk > 4) ccT = 0;
	     if(strlen(result) > 10) ccT -= 2;
	     if(strlen(result) > 20) ccT -= 2;
//printf("ccT= %d, kk= %d,strlen(result)= %d\n",ccT,kk,strlen(result));
	     line++;
	     if(getLine(resultsStr, line, &iStart, &iStop) < 0) return 0;
	     copyLine(lpFieldsResults, iStart, iStop);
//  printf("result= %s\n",result);
	     ccD = matchBestDayMonth2();
// printf("ccD= %d\n",ccD);
	     cc1 = ccT+ccD;
	  }
	  if(cc1 > 6) {
		  *lineTop = lineMax;
		  *lineBottom = line;
		  return 1;
	  }

	  line = lineMax;
// printf("line= %d, numLines= %d\n",line, numLines);
	  cc2 = 0;
	  if(line > 1) {
	     line--;
	     if(getLine(resultsStr, line, &iStart, &iStop) < 0) return 0;
	     copyLine(lpFieldsResults, iStart, iStop);
//  printf("result= %s\n",result);
	     ccT =matchToStringResult("THRU", &kk);
	     if(kk > 4) ccT = 0;
	     if(strlen(result) > 10) ccT -= 2;
	     if(strlen(result) > 20) ccT -= 2;
// printf("ccT= %d, kk= %d\n",ccT,kk);
	     line--;
	     if(getLine(resultsStr, line, &iStart, &iStop) < 0) return 0;
	     copyLine(lpFieldsResults, iStart, iStop);
// printf("result= %s\n",result);
	     ccD =matchBestDayMonth2();
// printf("ccD= %d\n",ccD);
	     cc2 = ccT+ccD;
	  }
	  if(cc2 > 6) {
		  *lineTop = line;
		  *lineBottom = lineMax;
		  return 1;
	  }
// printf("cc1= %d, cc2= %d\n",cc1,cc2);
	  if(cc1 > cc2){
		  if(cc1 > 4){
			  *lineTop = lineMax;
			  *lineBottom = line;
			  return 1;
		  }
	  }
	  if(cc2 > cc1){
		  if(cc2 > 4){
			  *lineTop = line;
			  *lineBottom = lineMax;
			  return 1;
		  }
	  }

	  line = lineMax;
	  cc1 = 0;
// printf("line= %d, numLines= %d\n",line, numLines);
	  if(line < numLines-1) {
	     line++;
	     if(getLine(resultsStr, line, &iStart, &iStop) < 0) return 0;
	     copyLine(lpFieldsResults, iStart, iStop);
//printf("result= %s\n",result);
         cc1 = matchBestDayMonth2();
// printf("cc1= %d\n",cc1);
	  }
	  if(cc1 > 5) {
		  *lineTop = lineMax;
		  *lineBottom = line;
		  return 1;
	  }


      return 0;
}
int  postProcessingDateAOnlyDraw_TX(LPTICKETRESULTS ticketResults, int iField, int *iShift){
  	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, k, kk;
	 int iStart, iStop;
	 int line;
	 int cc,  ccMax;
	 int lineMax;
	 int ccMaxTab[50];
	 int dayMonth_cc, dayMonth_line;
	 int matchCnf, lineTop, lineBottom;

	 lpFieldsResults = &ticketResults->fieldsResults[iField];
	 resultsStr = lpFieldsResults->result;
	 *iShift = 0;
	 resultTmp[0] = 0;
	 i = 0;
	 line = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		ccMax = 0;
		cc =matchToStringResult(" DRAW", &kk);
        if(kk< 10) cc = 0;
	    if(cc > ccMax) ccMax = cc;
	    if(i< 50)
	       ccMaxTab[i] = ccMax;
		i++;
	 }
	 k = i;
	 ccMax = 0;lineMax = 0;
	 for(i=0;i<k;i++){
//printf("-------ccMaxTab[ %d ]= %d\n",i, ccMaxTab[i]);
		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;}
	 }
// printf("-------lineMax= %d, ccMax= %d\n",lineMax, ccMax);

 if(ccMax < 4){
	 i = 0;
	 dayMonth_cc = 0;
	 dayMonth_line = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		cc = matchBestDayMonth();
	    if(cc > dayMonth_cc) {dayMonth_cc = cc; dayMonth_line = i;}
		i++;
	 }
	 if(dayMonth_cc > 4) {lineMax = dayMonth_line;matchCnf = 1;
//	    printf("-------dayMonth_line= %d, dayMonth_ccx= %d\n",dayMonth_line, dayMonth_cc);
	 }
 }else matchCnf = 1;

    multiDraw = 0;
    if(matchCnf == 1){
// test for Multi Draw
     multiDraw = testForMultiDraw_1(lpFieldsResults, lineMax, &lineTop, &lineBottom);
    }
//printf("matchCnf= %d, multiDraw= %d\n",matchCnf,multiDraw);
	if(multiDraw == 1) {
	    lineMax = lineTop;
	    addFieldDateTo_AllOrN(ticketResults, lineBottom, iField);
	}
	resultTmp[0] = 0;


	if(getLine(resultsStr, lineMax, &iStart, &iStop) >= 0) {
	         copyLine(lpFieldsResults, iStart, iStop);
//printf("-------result= %s\n",result);

	         *iShift = cleanDate_TX();
	         if(result[3] != 32) {
	   	       stretchResults(3, 1);
	   	       insResult(3, " ");
	         }
	         addLineToTmp();
	         line = lineMax;
		}
//printf("-------resultTmp= %s\n",resultTmp);
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}
void createFieldDRAW_FROM_TO(LPTICKETRESULTS ticketResults, int nDraw, char *title, int n){
 int nIdx = NUMFIELDS_RESULTS-1;
 int i;
// find first Empty slot for field
 for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] == 0){
    		nIdx = i;break;
    	}
 }
// int drawIdx = 7;
     strcpy(ticketResults->fieldsResults[nIdx].name, title);
     strcpy(ticketResults->fieldsResults[nIdx].result, "???");
     switch(nDraw){
       case 1:strcpy(ticketResults->fieldsResults[nIdx].result, "MORNING");break;
       case 2:strcpy(ticketResults->fieldsResults[nIdx].result, "DAY");break;
       case 3:strcpy(ticketResults->fieldsResults[nIdx].result, "EVENING");break;
       case 4:strcpy(ticketResults->fieldsResults[nIdx].result, "NIGHT");break;
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
void createNewFieldDate(LPTICKETRESULTS ticketResults, char *title, int n) { // from resultTmp
	 int nIdx = NUMFIELDS_RESULTS-1;
	 int i;
	// find first Empty slot for field
	 for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] == 0){
	    		nIdx = i;break;
	    	}
	 }
	     strcpy(ticketResults->fieldsResults[nIdx].name, title);
	     strcpy(ticketResults->fieldsResults[nIdx].result, resultTmp);

	     QUADRANGLE  flQuadTmp[MAXCHARS+2];

	     for(i=0;i<strlen(ticketResults->fieldsResults[nIdx].result);i++){
	    	 ticketResults->fieldsResults[nIdx].flConf[i] = flConfTmp[i];
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p1X = flQuadTmp[i].p1X;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p2X = flQuadTmp[i].p2X;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p3X = flQuadTmp[i].p3X;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p4X = flQuadTmp[i].p4X;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p1Y = flQuadTmp[i].p1Y;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p2Y = flQuadTmp[i].p2Y;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p3Y = flQuadTmp[i].p3Y;
	    	 ticketResults->fieldsResults[nIdx].flQuad[i].p4Y = flQuadTmp[i].p4Y;
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

void addFieldDrawFromTo_AllOrN(LPTICKETRESULTS ticketResults, int line, char *titleOut, int mode){
	 int iStart, iStop;
	 int cc, cc1, ccMax;
	 int i, kk, kk1;
	 int nDraw = 0;
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;

	 if(mode>0) line +=2;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
//	printf("-------------field name= %s\n",ticketResults->fieldsResults[i].name);
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	    			resultsStr = lpFieldsResults->result;
	                if(getLine(resultsStr, line, &iStart, &iStop) >= 0){
	                   copyLine(lpFieldsResults, iStart, iStop);

	           		   ccMax = 0;
	           		   cc =matchToStringResult("MORNING DRAW", &kk);
	           		   cc1 =matchToStringResult("MORNING ", &kk1);
	           		   if(cc1>cc) {cc = cc1; kk = kk1;}
	           	       if(cc > ccMax) {ccMax = cc; nDraw = 1;}
	           	       cc =matchToStringResult("DAY DRAW", &kk);
	           	       cc1 =matchToStringResult("DAY ", &kk1);
	           		   if(cc1>cc) {cc = cc1; kk = kk1;}
	           	       if(cc > ccMax) {ccMax = cc; nDraw = 2;}
	           	       cc =matchToStringResult("EVENING DRAW", &kk);
	           	       cc1 =matchToStringResult("EVENING ", &kk1);
	           		   if(cc1>cc) {cc = cc1; kk = kk1;}
	           	       if(cc > ccMax) {ccMax = cc; nDraw = 3;}
	           	       cc =matchToStringResult("NIGHT DRAW", &kk);
	           	       cc1 =matchToStringResult("NIGHT ", &kk1);
	           		   if(cc1>cc) {cc = cc1; kk = kk1;}
	           	       if(cc > ccMax) {ccMax = cc; nDraw = 4;}
//printf("createFieldDRAW_FROM_TO");
	           	       createFieldDRAW_FROM_TO(ticketResults, nDraw, titleOut, i);

	                }else{
	                	createFieldDRAW_FROM_TO(ticketResults, -1, titleOut, i);
	                }
	                break;
	    		}
	    	}
     }
}

void addFieldDateTo_AllOrN(LPTICKETRESULTS ticketResults, int lineB, int iA){
	 int i, lineTo = lineB;
	 int iStart, iStop, idxY, iShift, len;
	 int k1, k2;

	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
		lpFieldsResults = &ticketResults->fieldsResults[iA];
		resultsStr = lpFieldsResults->result;


		if(getLine(resultsStr, lineTo, &iStart, &iStop) >= 0) {
		         copyLine(lpFieldsResults, iStart, iStop);
	//   printf("-------result= %s\n",result);
		         iShift = cleanDate_TX();k1 = iShift;
		         if(result[3] != 32) {
		   	       stretchResults(3, 1);
		   	       insResult(3, " ");
		         }
		         addLineToTmp();

		         for(i=0;i<NUMFIELDS_RESULTS;i++){
		    	    	if(ticketResults->fieldsResults[i].name[0] != 0){
		    	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN") == 0) {
		    	    			 if(lineTo >=0 && iA >= 0) {
		    	 	    			lpFieldsResults = &ticketResults->fieldsResults[i];
		    	 	    			 resultsStr = lpFieldsResults->result;
		    // 			printf("1-------line=%d \n", line);
//		    	 	    			 line = adjustLineTX(resultsStr, line);
		    //   			printf("2-------line=%d\n", line);

		    	 	    			 cpyTmpToResult();
		        				     findDayMonthResult();
		        				     resultTmp[0] = 0;
		        				     addLineToTmp();

		    	    				 getLine(resultsStr, lineTo, &iStart, &iStop);

		    	    				 resultTmp[7] = 0;
		    //	    				 cand2Tmp[7] = 0;
		    //	    		printf("-------result = %s \n", result);
		    //	    		printf("-------resultTmp = %s \n", resultTmp);
		    	    				 copyLine(&ticketResults->fieldsResults[i], iStart, iStop);
		    	    				 len = strlen(result);
		    	    				 if(result[len-1] < 32) result[len-1] = 0;
		    	    				 len = strlen(result);
		    //  				 printf("-------result = %s \n", result);
		    	    				 k1 = getDigDateStart();
		    //	    				 k1 += iShift;
		    	    				 k2 = getDigDateStop(k1);

		    //printf("-------k1=%d, k2= %d \n", k1,k2);

		        			        copyLine(&ticketResults->fieldsResults[i], iStart+k1, iStart+k2);
		        		   	        stretchResults(0, 1);
		        			   	    insResult(0, " ");
		                            if(result[3] != ' '){
		        		   	           stretchResults(3, 1);
		        			   	       insResult(3, " ");
		                            }

		    		    			len = strlen(result);
		    //	    		printf("-------result = %s \n", result);
		        			        addLineToTmp();
		    //          		printf("-------resultTmp = %s \n", resultTmp);
		        			         createNewFieldDate(ticketResults, "DATE TO", i);  // from resultTmp
//		    	    				 copyResultsBack(&ticketResults->fieldsResults[iA]);
		    	    			 } else{
		    //	    				 line = postProcessingDateD(&ticketResults->fieldsResults[i]);
		    //	    			 		printf("-------line = %d \n", line);
		    	    				 lineTo = -1;
		    		    			 if(lineTo >=0 && iA >= 0) {
		    			 	    			lpFieldsResults = &ticketResults->fieldsResults[iA];
		    			 	    			resultsStr = lpFieldsResults->result;
		    			    				lineTo = adjustLineA(resultsStr, lineTo);
		    			    				getLine(resultsStr, lineTo, &iStart, &iStop);
		    				    			idxY = matchToStringEndOneLineIdx(resultTmp, "201", 3);
		    				    			idxY -= 3;
		    				    			if(idxY <= 0 || idxY >= strlen(resultTmp)) idxY = 7;
		    //         		printf("-------idxY = %d \n", idxY);
		    			    				replaceFirstCharsTmp(lpFieldsResults, iStart, iStop, idxY);
		    			    				createNewFieldDate(ticketResults, "DATE TO", i);  // from resultTmp
//		    			    				copyResultsBack(&ticketResults->fieldsResults[iA]);
		    		    			 }else
		    		    				 if(iA >= 0)
		    	    				       ticketResults->fieldsResults[iA].result[0] = 0;
		    	    			 }
		    	    			break;
		    	    		}
		    	    	}
		    	    }
		            cleanYear(ticketResults, "DATE TO");
		            cleanDateField(ticketResults, "DATE TO");
		            lastCheckDateField(ticketResults, "DATE TO");

		}else{
			resultTmp[0] = 0;
			createNewFieldDate(ticketResults, "DATE TO", iA);  // from resultTmp
		}


}

//int  postProcessingDateADraw_TX(LPFIELDSRESULTS lpFieldsResults, int *iShift){
int  postProcessingDateADraw_TX(LPTICKETRESULTS ticketResults, int iField, int *iShift){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, k, kk, kk1;
	 int iStart, iStop;
	 int line;
	 int cc, cc1, ccMax, kkMax, mDraw;
	 int lineMax, line1;
	 int ccMaxTab[50];
	 int kkMaxTab[50];
	 int mDrawTab[50];
	 int lineTop, lineBottom;

	 lpFieldsResults = &ticketResults->fieldsResults[iField];
	 resultsStr = lpFieldsResults->result;
	 *iShift = 0;
	 resultTmp[0] = 0;
	 i = 0;
	 line = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(lpFieldsResults, iStart, iStop);
		ccMax = 0;kkMax = 0; mDraw = 0;
		cc =matchToStringResult("MORNING DRAW ", &kk);
		cc1 =matchToStringResult("MORNINGDRAW ", &kk1);
		if(cc1>cc) {cc = cc1; kk = kk1;}
	    if(cc > ccMax) {ccMax = cc; kkMax = kk;}
	    cc =matchToStringResult("DAY DRAW ", &kk);
	    cc1 =matchToStringResult("DAYDRAW ", &kk1);
		if(cc1>cc) {cc = cc1; kk = kk1;}
	    if(cc > ccMax) {ccMax = cc; kkMax = kk;}
	    cc =matchToStringResult("EVENING DRAW ", &kk);
	    cc1 =matchToStringResult("EVENINGDRAW ", &kk1);
		if(cc1>cc) {cc = cc1; kk = kk1;}
	    if(cc > ccMax) {ccMax = cc; kkMax = kk;}
	    cc =matchToStringResult("NIGHT DRAW ", &kk);
	    cc1 =matchToStringResult("NIGHTDRAW ", &kk1);
		if(cc1>cc) {cc = cc1; kk = kk1;}
	    if(cc > ccMax) {ccMax = cc; kkMax = kk;}
	    cc =matchToStringResult("CONSECUTIVE DRAWS ", &kk);
	    cc1 =matchToStringResult("CONSECUTIVEDRAWS ", &kk1);
		if(cc1>cc) {cc = cc1; kk = kk1;}
	    if(cc > ccMax) {ccMax = cc; kkMax = kk; mDraw = 1;}

	    if(i< 50) {
	       ccMaxTab[i] = ccMax;
	       kkMaxTab[i] = kkMax;
		   mDrawTab[i] = mDraw;
	    }
//  printf("-------result= %s\n",result);
// printf("-------ccMaxTab[ %d ]= %d, kkMaxTab[i]= %d, ccMax= %d\n",i, ccMaxTab[i], kkMaxTab[i], ccMax);
		i++;
	 }
	 k = i;
	 ccMax = 0;lineMax = 0;
	 for(i=0;i<k;i++){
//printf("-------ccMaxTab[ %d ]= %d, kkMaxTab[ %d ]= %d\n",i, ccMaxTab[i],i,kkMaxTab[i]);
		 if(ccMax < ccMaxTab[i]) {ccMax = ccMaxTab[i];lineMax = i;multiDraw = mDrawTab[i];}
	 }
// printf("-------lineMax= %d, multiDraw= %d\n",lineMax, multiDraw);
     if(lineMax == 0){
    	 line1 = lineMax;
     } else{
         if(lineMax == k-1){
        	 if(ccMaxTab[lineMax-1] > 5) line1 = lineMax-1;
        	 else line1 = lineMax;
         }else{
        	 if(ccMaxTab[lineMax-1] > ccMaxTab[lineMax+1]) line1 = lineMax-1;
        	 else line1 = lineMax;
         }
     }

// test---------------------
//     multiDraw = 0;
// test---------------------

//printf("-------line1= %d\n",line1);
     if(kkMaxTab[line1] < 10)
        line1++;
     if(multiDraw == 0){
 // test for Multi Draw
        multiDraw = testForMultiDraw_1(lpFieldsResults, line1, &lineTop, &lineBottom);
     }else{
    	 lineTop = line1; lineBottom = lineTop+2;
     }


	if(multiDraw == 1) {
		line1 = lineTop;
		addFieldDrawFromTo_AllOrN(ticketResults, lineTop, "DRAW FROM", 0);
	    addFieldDateTo_AllOrN(ticketResults, lineBottom, iField);
	    addFieldDrawFromTo_AllOrN(ticketResults, lineTop, "DRAW TO", 1);
	}
	resultTmp[0] = 0;
//printf("-------line1= %d\n",line1);
	if(getLine(resultsStr, line1, &iStart, &iStop) >= 0) {
	         copyLine(lpFieldsResults, iStart, iStop);
//   printf("-------result= %s\n",result);
	         *iShift = cleanDate_TX();
	         if(result[3] != 32) {
	   	       stretchResults(3, 1);
	   	       insResult(3, " ");
	         }
	         addLineToTmp();
	         line = line1;
		}
//printf("-------resultTmp= %s\n",resultTmp);
	 lpFieldsResults->name[strlen(lpFieldsResults->name)-1] = 0;
	 return line;
}
int getDigDateStart(void){
	int i, len;
	int k, k1, idx, sh;
	k = -1;idx = 0;
	len = strlen(result);
	if(len < 12) return idx;
	for(i=0;i<3;i++){
		if(result[i] == 32) {k = i;break;}
	}
	if(result[k+4] == 32)
		idx = k+8;
	else
		idx = k+7;
//printf("idx= %d, result[idx+1]= %c, result[idx+2]= %c, result[idx+3]= %c\n", idx, result[idx+1], result[idx+2], result[idx+3]);
// test for '20'
	if(result[idx+2]== '2' && result[idx+3]== '0')
		return idx;
	if(result[idx+3]== '2' && result[idx+4]== '0'){
	  if(result[idx+2] == ' ') return idx;
		return idx+1;
	}
//printf("idx= %d, result[idx+1]= %c, result[idx+2]= %c, result[idx+3]= %c\n", idx, result[idx+1], result[idx+2], result[idx+3]);
	if(result[idx+1]== '2' && result[idx+2]== '0' && result[idx+3]== '1')
		return idx-1;

	k1 = idx;
	for(i=idx;i<len;i++){
		if(result[i] == 32) {k1 = i;break;}
	}
	sh = k1-idx-1;
//printf("k1= %d, idx= %d, sh= %d", k1, idx, sh);
	if(sh >5) idx += (sh-5);
	return idx;
}
int getDigDateStop(int k1){
	int len;
	int idx;
	idx = 0;
	len = strlen(result);
	if(len < 12) return idx;
	if(result[k1+2] == 32)
		idx = k1+6;
	else
		idx = k1+5;
	return idx;

}

int matchToStringResultDateTX(char *str, int stop){
	   int i, k, cc, ccMax, kMax;
	   int lenR = strlen(result);
	   int len = strlen(str);

	   if(len > lenR) len = lenR;
	   ccMax = 0; kMax = -1;
	   for(k=0;k<=lenR-len;k++){
		   if(k > stop) break;
		   cc = 0;
	      for(i=0;i<len;i++){
	    	 if(i+k >= lenR) break;
		     if(result[i+k] == str[i]) cc++;
	      }
	      if(ccMax < cc) {ccMax = cc;kMax = k;}
	   }
	   k = kMax; // to avoid compiler warning
       return ccMax;
}
void copyLineOnlyStr(char *str, int iStart, int iStop){
	   int i, k;
	   if(iStop - iStart >= 200)
		   iStart = iStop - 200;
	   k = 0;
	   for(i=iStart;i<=iStop;i++){
		   result[k] = str[i];
		   k++;
	   }
	   result[k] = 0;
}

int adjustLineTX(char *resultsStr, int line){
	int iStart, iStop;
	 int ccY, ccYmax;
	 int lineYmax;
//	 int sh, dd;

    ccYmax= 0; lineYmax = line;

    getLine(resultsStr, line, &iStart, &iStop);
    copyLineOnlyStr(resultsStr, iStart, iStop);
    ccY = matchToStringResultDateTX("201", 20);
	if(ccYmax < ccY) {ccYmax = ccY;lineYmax = line;}

//	   printf("line= %d, ccY= %d, lineYmax= %d, result= %s\n", line-1, ccY, lineYmax, result);

	if(line > 0){
         getLine(resultsStr, line-1, &iStart, &iStop);

         copyLineOnlyStr(resultsStr, iStart, iStop);
         ccY = matchToStringResultDateTX("201", 20);
	    if(ccYmax < ccY) {ccYmax = ccY;lineYmax = line-1;}

//   printf("line= %d, ccY= %d, lineYmax= %d, result= %s\n", line-1, ccY, lineYmax, result);

	}

    if(getLine(resultsStr, line+1, &iStart, &iStop) >= 0){
        copyLineOnlyStr(resultsStr, iStart, iStop);
        ccY = matchToStringResultDateTX("201", 20);

 		 if(ccYmax < ccY) {ccYmax = ccY;lineYmax = line+1;}

// printf("line= %d, ccY= %d, lineYmax= %d, result= %s\n", line-1, ccY, lineYmax, result);

    }
    return lineYmax;
}
int matchToStringStr(char *str, char *strKey, int *kM){
	   int i, k, cc, ccMax, kMax;
	   int lenR = strlen(str);
	   int len = strlen(strKey);

	   if(len > lenR) len = lenR;
	   ccMax = 0; kMax = -1;
	   for(k=0;k<=lenR-len+1;k++){
		   cc = 0;
	      for(i=0;i<len;i++){
	    	 if(i+k >= lenR) break;
		     if(str[i+k] == strKey[i]) cc++;
	      }
	      if(ccMax < cc) {ccMax = cc;kMax = k;}
	   }
	   *kM = kMax;
       return ccMax;
}

void findDayMonthResult(){
	int kM[6], cc[6];
	int i, k, ccM, kMmonth;
	int ccMmax, ccMi, count;
	int max, iMax;

//	printf("------------------result= %s\n",result);

	// day
	for(i=0;i<6;i++){
		  cc[i] = matchToStringResult( Day_Names[i+1], &kM[i]);
	}
	ccMmax = 0;ccMi = -1;
	for(i=0;i<6;i++){
		if(cc[i] > 1) {
			for(k=0;k<12;k++) {
				ccM = matchToStringStr(&result[kM[i]], Month_Names[k], &kMmonth);
				if(ccMmax < ccM  && kMmonth > 2 && kMmonth < 5) {ccMmax = ccM; ccMi = k;}
//				printf("k= %d, ccM= %d, kMmonth= %d\n", k, ccM, kMmonth);
			}
			if(ccMi >=0 && ccMmax == 3)
				cc[i]++;
//			  printf("cc[ %d]= %d, ccMi= %d, ccMmax= %d\n", i, cc[i], ccMi, ccMmax);
		}
	}

	max = 0; iMax = -1;
	for(i=0;i<6;i++){
		if(max < cc[i]) {max = cc[i]; iMax = i;}
	}
	count = 0;
	for(i=0;i<6;i++){
		if(max == cc[i]) count++;
	}
//	printf("ccMi= %d, count= %d, kM[ccMi]= %d\n", ccMi, count, kM[ccMi]);
//	printf("resultStr= %s\n",resultStr);

	if(iMax >= 0 && count == 1 && kM[iMax]>0) {
		int dd, sh;
	   	 shrinkResults(0, kM[iMax]-1);
	   	dd = ccMmax-3; sh = 1+dd;
	   	if(ccMi >=0 && ccMmax == 3)
	   	 if(result[3] > 32) {
	        stretchResults(3, sh);
	        for(i=0;i<sh;i++)
   	           insResult(3+i, " ");
	     }
	}

//	printf("2-----------------result= %s\n",result);
}
int getFirstDigit(char *str){
	int i, len = strlen(str);
	for(i=0;i<len;i++){
		if(str[i] >='0' && str[i] <='9') return i;
	}
	return -1;
}
void lastCheckDateField(LPTICKETRESULTS ticketResults, char *title){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, j;
  for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, title) == 0) {

	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	    			resultsStr = lpFieldsResults->result;
                    j= getFirstDigit(resultsStr);
//                    printf("------j= %d, resultsStr[j+2]= %c\n",j, resultsStr[j+2]);
	    			if(j > 0 && j+2 < strlen(resultsStr) && resultsStr[j+2] > 32){
	    			   copyLine(lpFieldsResults, 0, strlen(resultsStr));
 		   	           stretchResults(j+2, 1);
 			   	       insResult(j+2, " ");
 			   	       resultTmp[0] = 0;
 			   	       addLineToTmp();
 			   	       copyResultsBack(lpFieldsResults);
	    			}
	    			break;
	    		}

	    	}
	    }
}

void  postProcessingDate_TX(LPTICKETRESULTS ticketResults){
	 LPFIELDSRESULTS lpFieldsResults;
	 char *resultsStr;
	 int i, iA, line, len;
	 int iStart, iStop, idxY;
	 int k1, k2;
	 int iShift = 0;
	 iA = -1;
	 line = -1;
     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {


	    			/*
	    			1 - Powerball
	    			2- Mega Millions
	    			3- Lotto Texas
	    			4- Texas Two Steps
	    			5- Pick 3
	    			6- Daily 4
	    			8 - All Or Nothing
	    			*/

    			   switch(ticketTypeInt){
    			    case 3: // LOTTO TEXAS
//	    					line = postProcessingDateAEXTRA_TX(&ticketResults->fieldsResults[i], &iShift);
	    					line = postProcessingDateAEXTRA_TX(ticketResults, i, &iShift);
	    					break;
	    			case 5: // PICK 3
	    			case 6: // DAILY 4
    	    		case 8: // ALL OR NOTHING TX
		    			//line = postProcessingDateADraw_TX(&ticketResults->fieldsResults[i], &iShift);
		    			line = postProcessingDateADraw_TX(ticketResults, i, &iShift);
		    			break;
	    			  case 7: // CASH FIVE
	    			  case 4: // TWO STEP
//			    			line = postProcessingDateAOnlyDraw_TX(&ticketResults->fieldsResults[i], &iShift);
			    			line = postProcessingDateAOnlyDraw_TX(ticketResults, i, &iShift);
			    			break;
	    		      case 1: // POWER BALL
	    			  case 2: // MEGA MILLIONS
//	    					line = postProcessingDateAPlay_TX(&ticketResults->fieldsResults[i], &iShift);
	    					line = postProcessingDateAPlay_TX(ticketResults, i, &iShift);
	    					break;

	    		    }

	    			iA = i;
	    			break;
	    		}

	    	}
	    }
//	printf("-------resultTmp = %s, line= %d, iA= %d \n", resultTmp, line, iA);

     for(i=0;i<NUMFIELDS_RESULTS;i++){
	    	if(ticketResults->fieldsResults[i].name[0] != 0){
	    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN") == 0) {
	    			 if(line >=0 && iA >= 0) {
	 	    			lpFieldsResults = &ticketResults->fieldsResults[i];
	 	    			 resultsStr = lpFieldsResults->result;
// 			printf("1-------line=%d \n", line);
	 	    			 line = adjustLineTX(resultsStr, line);
//   			printf("2-------line=%d\n", line);

	 	    			 cpyTmpToResult();
    				     findDayMonthResult();
    				     resultTmp[0] = 0;
    				     addLineToTmp();

	    				 getLine(resultsStr, line, &iStart, &iStop);

	    				 resultTmp[7] = 0;
//	    				 cand2Tmp[7] = 0;
//	    		printf("-------result = %s \n", result);
//	    		printf("-------resultTmp = %s \n", resultTmp);
	    				 copyLine(&ticketResults->fieldsResults[i], iStart, iStop);
	    				 len = strlen(result);
	    				 if(result[len-1] < 32) result[len-1] = 0;
	    				 len = strlen(result);
//  				 printf("-------result = %s \n", result);

	    				 k1 = getDigDateStart();
//	    				 k1 += iShift;
	    				 k2 = getDigDateStop(k1);

//printf("-------k1=%d, k2= %d \n", k1,k2);

    			        copyLine(&ticketResults->fieldsResults[i], iStart+k1, iStart+k2);
    		   	        stretchResults(0, 1);
    			   	    insResult(0, " ");
                        if(result[3] != ' '){
    		   	           stretchResults(3, 1);
    			   	       insResult(3, " ");
                        }

		    			len = strlen(result);
//	    		printf("-------result = %s \n", result);
    			        addLineToTmp();
//          		printf("-------resultTmp = %s \n", resultTmp);

	    				 copyResultsBack(&ticketResults->fieldsResults[iA]);
	    			 } else{
//	    				 line = postProcessingDateD(&ticketResults->fieldsResults[i]);
//	    			 		printf("-------line = %d \n", line);
	    				 line = -1;
		    			 if(line >=0 && iA >= 0) {
			 	    			lpFieldsResults = &ticketResults->fieldsResults[iA];
			 	    			resultsStr = lpFieldsResults->result;
			    				line = adjustLineA(resultsStr, line);
			    				getLine(resultsStr, line, &iStart, &iStop);
				    			idxY = matchToStringEndOneLineIdx(resultTmp, "201", 3);
				    			idxY -= 3;
				    			if(idxY <= 0 || idxY >= strlen(resultTmp)) idxY = 7;
//         		printf("-------idxY = %d \n", idxY);
			    				replaceFirstCharsTmp(lpFieldsResults, iStart, iStop, idxY);
			    				copyResultsBack(&ticketResults->fieldsResults[iA]);
		    			 }else
		    				 if(iA >= 0)
	    				       ticketResults->fieldsResults[iA].result[0] = 0;
	    			 }
	    			break;
	    		}
	    	}
	    }
//        cleanYear(ticketResults);
        cleanYear(ticketResults, "DATE");
        cleanDateField(ticketResults, "DATE");
        lastCheckDateField(ticketResults, "DATE");
	 return;
}
/*
void  postProcessingDate1_TX(LPTICKETRESULTS ticketResults){
	 resultTmp[0] = 0;
	 postProcessingDate1New(ticketResults);
}
*/
int testdateFormat(char *date){
  int i, len, pen = 0, stop;
  len = strlen(date);

  if(len != 14) pen--;
  stop = 3; if(stop > len) stop = len;
  for(i=0;i<stop;i++){
	if(date[i] < 'A' || date[i] > 'Z') pen--;
  }
  if(len < 4) return (pen--);
  if(date[3] !=' ') pen--;
  if(len < 5) return (pen--);
  stop = 7; if(stop > len) stop = len;
  for(i=4;i<stop;i++){
	if(date[i] < 'A' || date[i] > 'Z') pen--;
  }
  if(len < 8) return (pen--);
  stop = 9; if(stop > len) stop = len;
  for(i=7;i<stop;i++){
	if(date[i] < '0' || date[i] > '9') pen--;
  }
  if(len < 10) return (pen--);
  if(date[9] !=' ') pen--;
  if(len < 14) return (pen--);
  stop = 14;
  for(i=10;i<stop;i++){
	if(date[i] < '0' || date[i] > '9') pen--;
  }
  return pen;
}
int cmpStringsExact(char *str1, char *str2){
	   int i,  pen = 0;
	   int len1 = strlen(str1), len2 = strlen(str2);
	   int len = len1;
	   if(len1 != len2) {
		   if(len1 > len2) {len = len2; pen = len1-len2;}
		   else
		      {pen = len2-len1;}
	   }
       for(i=0;i<len;i++){
	      if(str1[i] != str2[i]) pen++;
      }
       pen = -pen;
      return pen;
}

int testdateDay(char *date){
	  int i, len, pen = 0;
	  int cc[10];
      char dStr[10];
	  len = strlen(date);
	  if(len < 3) return -3;
	  for(i=0;i<3;i++)
		  dStr[i] = date[i];
	  dStr[3] = 0;
	  for(i=0;i<7;i++){
		  cc[i] = cmpStringsExact(dStr, Day_Names[i]);
//printf("Day-------cc[i]= %d, dStr= %s, Day_Names[i]= %s \n", cc[i], dStr, Day_Names[i]);
		}
	  pen = -3;
	  for(i=0;i<7;i++){
		if(pen < cc[i]) pen = cc[i];
      }
	  return pen;
}

int testdateMonth(char *date){
	  int i, len, pen = 0;
	  int cc[12];
      char dStr[10];
	  len = strlen(date);
	  if(len < 7) return -3;
	  for(i=0;i<3;i++)
		  dStr[i] = date[i+4];
	  dStr[3] = 0;
	  for(i=0;i<12;i++){
		  cc[i] = cmpStringsExact(dStr, Month_Names[i]);
// printf("Month-------cc[i]= %d, dStr= %s, Month_Names[i]= %s \n", cc[i], dStr, Month_Names[i]);
		}
	  pen = -3;
	  for(i=0;i<12;i++){
		if(pen < cc[i]) pen = cc[i];
      }
	  return pen;
}
int isLeapYear(int aYear) {
        int result = 0;
        if (aYear % 100 == 0) {
          if (aYear % 400 == 0) {
            result = 1;
          }
        }
        else if (aYear % 4 == 0) {
          result = 1;
        }
        return result;
      }

int getNumDaysInMonth(int aYear, int aMonth) {
        int result = 0;
        switch(aMonth){
        case 2:result = isLeapYear(aYear) ? 29 : 28;break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
        	result = 31;break;
        case 4:
        case 6:
        case 9:
        case 11:
        	result = 30;break;
        }
        return result;
      }

int testdateDayD(char *date){
	  int i, n, len, pen = 0;
      char dStr[10];
	  len = strlen(date);
	  if(len < 8) return -3;
	  for(i=0;i<2;i++)
		  dStr[i] = date[i+7];
	  dStr[2] = 0;
	  sscanf(dStr,"%d",&n);
//	  printf("day-------n= %d\n", n);
      if(n<1 || n >31) pen--;
//printf("day D-------pen= %d \n", pen);
	  return pen;
}
int testdateYearD(char *date){
	  int i, n, len, pen = 0;
      char dStr[10];
	  len = strlen(date);
	  if(len < 14) return -3;
	  for(i=0;i<4;i++)
		  dStr[i] = date[i+10];
	  dStr[4] = 0;
	  sscanf(dStr,"%d",&n);
//	  printf("year-------n= %d\n", n);
      if(n<2012 || n >2016) pen--;
//printf("year D-------pen= %d \n", pen);
	  return pen;
}

int checkDates(char *dateT, char *dateB){
	int ret = 0;
	int cnfT = 0, cnfB = 0;

	cnfT += testdateFormat(dateT);
//printf("Format-------cnfT= %d\n", cnfT);
	cnfB += testdateFormat(dateB);
//printf("Format-------cnfT= %d, cnfB= %d \n", cnfT, cnfB);
//printf("Day Top \n");
	cnfT += testdateDay(dateT);
//printf("Day Bottom \n");
	cnfB += testdateDay(dateB);
//printf("Day-------cnfT= %d, cnfB= %d \n", cnfT, cnfB);

//printf("Month-------dateT= %s\n", dateT);
	cnfT += testdateMonth(dateT);
//printf("Month-------dateB= %s\n", dateB);
	cnfB += testdateMonth(dateB);
//printf("Month-------cnfT= %d, cnfB= %d \n", cnfT, cnfB);
	cnfT += testdateDayD(dateT);
	cnfB += testdateDayD(dateB);
	cnfT += testdateYearD(dateT);
	cnfB += testdateYearD(dateB);
	if(cnfT > cnfB) ret = 1;
//printf("------cnfT= %d, cnfB= %d \n", cnfT, cnfB);
	return ret;
}
void compareWithTopDate(LPTICKETRESULTS ticketResults){
	char dateTop[100], dateB[100];
	int i, iT=-1, iB=-1;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
      	if(ticketResults->fieldsResults[i].name[0] != 0){
      		if(strcmp(ticketResults->fieldsResults[i].name, "DATETOP") == 0)   {strcpy(dateTop, ticketResults->fieldsResults[i].result);iT = i;}
      		if(strcmp(ticketResults->fieldsResults[i].name, "DATE") == 0)   {strcpy(dateB, ticketResults->fieldsResults[i].result);iB = i;}
    	}
    }
//printf("------------checkDates; dateTop= %s, dateB= %s, iT= %d, iB= %d \n", dateTop,dateB, iT, iB);
    if(checkDates(dateTop, dateB)>0){
    	strcpy(ticketResults->fieldsResults[iB].result, ticketResults->fieldsResults[iT].result);
    }

}
void createExtraField(LPTICKETRESULTS ticketResults, int n){
	 FIELDSRESULTS fieldsResults = ticketResults->fieldsResults[n];
	 char *resultsStr = fieldsResults.result;
	 int cM;
//	 int cM1, cP1, cP2;
	 int cY, cN, cc;
	 int th = 5;

	 int maxc[2], linec[2];
	 int i, kk;
	 int iStart, iStop;

//	 int ccYes, ccNo, maxYes, maxNo;
	 resultTmp[0] = 0;
	 for(i = 0;i<2;i++){
		 maxc[i] = 0; linec[i] = -1;
	 }
	 i = 0;cc = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(&fieldsResults, iStart, iStop);
		cM =matchToStringResult("EXTRA ", &kk);
		if(kk > 4) cM = 0;
		if(cM == 4) cc++;
	    if(cM > maxc[0]) {maxc[0] = cM;linec[0] = i;}
//printf("---result= %s\n", result);
//printf("---i= %d, cM= %d\n", i, cM);
	    i++;
	 }
//printf("--- maxc[0]= %d, linec[0]= %d\n", maxc[0], linec[0]);
	 if(maxc[0] == 4 && cc == 1) th--;

   {int nIdx = NUMFIELDS_RESULTS-1;
    int drawIdx = 7;
        strcpy(ticketResults->fieldsResults[nIdx].name, "EXTRA");
        strcpy(ticketResults->fieldsResults[nIdx].result, "???");
        drawIdx = 0;
//        printf("---drawIdx= %d\n", drawIdx);
        if(maxc[drawIdx] < th) {
         	 i = 0;
         	 maxc[0] = 0;linec[0] = -1;
         	 while(1){
         		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
         		copyLine(&fieldsResults, iStart, iStop);
         		cM =matchToStringResult(" DRAW", &kk);
         		if(cM > maxc[0]) {maxc[0] = cM;linec[0] = i;}
         		i++;
         	 }
        }
     	if(maxc[drawIdx] < 4)
    	    drawIdx = 7;
        else{
    		if(getLine(resultsStr, linec[drawIdx], &iStart, &iStop) >= 0){
    			copyLine(&fieldsResults, iStart, iStop);
    			cY =matchToStringResult("YES ", &kk);
    			cN =matchToStringResult("NO ", &kk);
// 			printf("---result= %s, cY= %d, cN= %d\n", result, cY, cN);
    			if(cY==cN){
        			cY =matchToStringResult(" YES ", &kk);
        			cN =matchToStringResult(" NO ", &kk);
        			if(cY==cN){
            			cY =matchToStringResult("YES", &kk);
            			cN =matchToStringResult("NO", &kk);
        			}
    			}
//   			printf("---result= %s, cY= %d, cN= %d\n", result, cY, cN);
    	         if(cY > cN) {
    		         strcpy(ticketResults->fieldsResults[nIdx].result, "YES");
    	         }
    	         if(cY < cN) {
    		         strcpy(ticketResults->fieldsResults[nIdx].result, "NO");
    	         }
    	         for(i=0;i<strlen(ticketResults->fieldsResults[nIdx].result);i++){
    	        	 ticketResults->fieldsResults[nIdx].flConf[i] = 100;
    	         }

    		}
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


void postProcessingEXTRA_TX(LPTICKETRESULTS ticketResults){
	int i;
	  for(i=0;i<NUMFIELDS_RESULTS;i++){
		if(ticketResults->fieldsResults[i].name[0] != 0){
			if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {
				createExtraField(ticketResults, i);
			}
		}
	  }
}

void createDraw2Field(LPTICKETRESULTS ticketResults, int n){
	 FIELDSRESULTS fieldsResults = ticketResults->fieldsResults[n];
	 char *resultsStr = fieldsResults.result;
	 int cD, cN, maxcD, maxcN, lineD, lineN;
	 int i;
	 int iStart, iStop;
	 resultTmp[0] = 0;
	 i = 0;
	 maxcD = 0;
	 lineD = -1;
	 maxcN = 0;
	 lineN = -1;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;

	    cD = matchToString(resultsStr, iStart, iStop, "DAY DRAW");
	    if(cD > maxcD) {maxcD = cD;lineD = i;}
	    cN = matchToString(resultsStr, iStart, iStop, "NIGHT DRAW");
	    if(cN > maxcN) {maxcN = cN;lineN = i;}
	    i++;
	 }
     if(maxcD < 4 && maxcN < 4 ) {
// printf("---maxcD= %d, maxcN= %d\n", maxcD, maxcN);

    	 maxcD = 0;
    	 lineD = -1;
    	 i = 0;
    	 while(1){
    		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
            if(iStop-iStart < 6) {
    	       cD = matchToString(resultsStr, iStart, iStop, "XYAB");
    	       if(cD > maxcD) {maxcD = cD;lineD = i;}
    	       cD = matchToString(resultsStr, iStart, iStop, "BYAB");
    	       if(cD > maxcD) {maxcD = cD;lineD = i;}
    	       cD = matchToString(resultsStr, iStart, iStop, "DYAB");
    	       if(cD > maxcD) {maxcD = cD;lineD = i;}
            }
    	    i++;
    	 }
    	 if(maxcD == 4) maxcD++;
     }
//     printf("2---maxcD= %d, maxcN= %d\n", maxcD, maxcN);

    {int nIdx = NUMFIELDS_RESULTS-1;
         strcpy(ticketResults->fieldsResults[nIdx].name, "DRAW");
         strcpy(ticketResults->fieldsResults[nIdx].result, "???");
         if(maxcD > 4 && maxcD > maxcN ) {
	         strcpy(ticketResults->fieldsResults[nIdx].result, "DAY DRAW");
         }
         if(maxcN > 4 && maxcN > maxcD ) {
	         strcpy(ticketResults->fieldsResults[nIdx].result, "NIGHT DRAW");
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
    lineD=lineN;// avoiding warnings
    lineN=lineD;// avoiding warnings
}

void postProcessingDraw2_TX(LPTICKETRESULTS ticketResults){
	int i;
	  for(i=0;i<NUMFIELDS_RESULTS;i++){
		if(ticketResults->fieldsResults[i].name[0] != 0){
			if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {
				createDraw2Field(ticketResults, i);
			}
		}
	  }
}

int getMaxIdx(int *maxc){
	int max, iMax;
	int i;
	  max = maxc[0]; iMax = 0;
	  for(i=1;i<5;i++){
		  if(max < maxc[i]){
			  max = maxc[i]; iMax = i;
		  }
	  }
	  return iMax;
}

void createDraw4Field(LPTICKETRESULTS ticketResults, int n){
	 FIELDSRESULTS fieldsResults = ticketResults->fieldsResults[n];
	 char *resultsStr = fieldsResults.result;
	 int cM, cD, cE, cN;
	 int cM1, cD1, cE1, cN1, cCon, cCon1;
	 int cE2, cE3;
	 int maxc[5], linec[5];
	 int i, kk;
	 int iStart, iStop;
//	 int ccYes, ccNo, maxYes, maxNo;
	 resultTmp[0] = 0;
	 for(i = 0;i<5;i++){
		 maxc[i] = 0; linec[i] = -1;
	 }
	 i = 0;
	 while(1){
		if(getLine(resultsStr, i, &iStart, &iStop) < 0) break;
		copyLine(&fieldsResults, iStart, iStop);
		cM =matchToStringResult("MORNING DRAW", &kk);
		cM1 =matchToStringResult("MORNINGDRAW", &kk);
		if(cM1 > cM) cM = cM1;
	    if(cM > maxc[0]) {maxc[0] = cM;linec[0] = i;}
		cD =matchToStringResult("DAY DRAW", &kk);
		cD1 =matchToStringResult("DAYDRAW", &kk);
		if(cD1 > cD) cD = cD1;
	    if(cD > maxc[1]) {maxc[1] = cD;linec[1] = i;}
		cE =matchToStringResult("EVENING DRAW", &kk);
		cE1 =matchToStringResult("EVENINGDRAW", &kk);
		cE2 =matchToStringResult("EWENING DRAW", &kk);
		cE3 =matchToStringResult("EWNING DRAW", &kk);
		if(cE1 > cE) cE = cE1;
		if(cE2 > cE) cE = cE2;
		if(cE3 > cE) cE = cE3;
	    if(cE > maxc[2]) {maxc[2] = cE;linec[2] = i;}
		cN =matchToStringResult("NIGHT DRAW", &kk);
		cN1 =matchToStringResult("NIGHTDRAW", &kk);
		if(cN1 > cN) cN = cN1;
	    if(cN > maxc[3]) {maxc[3] = cN;linec[3] = i;}

	    cCon =matchToStringResult("CONSECUTIVE DRAWS ", &kk);
	    cCon1 =matchToStringResult("CONSECUTIVEDRAWS ", &kk);
		if(cCon1 > cCon) cCon = cCon1;
		if(cCon > maxc[4]) {maxc[4] = cCon;linec[4] = i;}

//printf("---i= %d, cM= %d, cD= %d, cE= %d, cN= %d, cCon= %d\n", i,cM, cD, cE, cN, cCon);
	    i++;
	 }
//printf("--- maxc[0]= %d, maxc[1]= %d, maxc[2]= %d, maxc[3]= %d, maxc[4] = %d\n", maxc[0], maxc[1], maxc[2], maxc[3],maxc[4]);


    {int nIdx = NUMFIELDS_RESULTS-1;
     int drawIdx = 7;
         strcpy(ticketResults->fieldsResults[nIdx].name, "DRAW");
         strcpy(ticketResults->fieldsResults[nIdx].result, "???");
         drawIdx = getMaxIdx(maxc);
//       printf("---drawIdx= %d, maxc[drawIdx]= %d\n", drawIdx, maxc[drawIdx]);
         if(maxc[drawIdx] <= 4) drawIdx = 7;
         switch(drawIdx){
           case 0:strcpy(ticketResults->fieldsResults[nIdx].result, "MORNING DRAW");break;
           case 1:strcpy(ticketResults->fieldsResults[nIdx].result, "DAY DRAW");break;
           case 2:strcpy(ticketResults->fieldsResults[nIdx].result, "EVENING DRAW");break;
           case 3:strcpy(ticketResults->fieldsResults[nIdx].result, "NIGHT DRAW");break;
           case 4:strcpy(ticketResults->fieldsResults[nIdx].result, "CONSECUTIVE DRAWS");break;
         }

         for(i=0;i<strlen(ticketResults->fieldsResults[nIdx].result);i++){
        	 ticketResults->fieldsResults[nIdx].flConf[i] = 100;
         }


//         printf("---ticketResults->fieldsResults[nIdx].name= %s\n", ticketResults->fieldsResults[nIdx].name);
//         printf("---ticketResults->fieldsResults[nIdx].result= %s\n", ticketResults->fieldsResults[nIdx].result);
//         printf("---NUM_LINESTATUS= %d\n", NUM_LINESTATUS);



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
    kk = linec[0];// avoiding warnings
}

void postProcessingDraw4_TX(LPTICKETRESULTS ticketResults){
	int i;
	  for(i=0;i<NUMFIELDS_RESULTS;i++){
		if(ticketResults->fieldsResults[i].name[0] != 0){
			if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {
				createDraw4Field(ticketResults, i);
			}
		}
	  }
}

void ticketPostProcessing_TX(LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPTICKETRESULTS ticketResults){
	int i;
//	  printf ("Results before post-processing----------------------------------------\n");
//	  saveLog("Results before post-processing----------------------------------------", "\n");
//		printResults_1(ticketResults);
//	 printf ("-----------------------------------------------------------------------\n");
//	 saveLog("-----------------------------------------------------------------------", "\n");

//	int nIdx = NUMFIELDS_RESULTS-1;
	linesPostCnf = 0;
    ticketResults->type = ticketTypeInt;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
//	 printf ("name= %s\n", ticketResults->fieldsResults[i].name);

    		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {postProcessingLogo_TX(ticketResults);setLogoSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_TX(&ticketResults->fieldsResults[i], i);
    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_TX(img, ticketResults, i);
    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {
    			      postProcessingRetNum_TX(ticketResults);
//    			      setRetNumSlices(ticketResults);
    			      postProcessingDate_TX(ticketResults);setDateSlices(ticketResults);
    		}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN1") == 0) postProcessingDate1_TX(ticketResults);

//	printf("---ticketResults->fieldsResults[nIdx].name= %s\n", ticketResults->fieldsResults[nIdx].name);
    		//         printf("---ticketResults->fieldsResults[nIdx].result= %s\n", ticketResults->fieldsResults[nIdx].result);
    		//         printf("---NUM_LINESTATUS= %d\n", NUM_LINESTATUS);

    	}
    }
/*
	switch(ticketTypeInt){

	case 0: // LOTTO TEXAS
	case 1: // PICK 3
	case 2: // CASH FIVE
	case 4: // DAILY 4
	case 7: // ALL OR NOTHING
		compareWithTopDate(ticketResults);break;
	}
*/
}


int searchForDollarPattern(char *rStr, int i, int stop){
int k = i, n;
   while(k < stop){
	   if( rStr[k] == '5'){
	       n = k+2;
		   if( n+3 < stop && rStr[n] == 32 && rStr[n+3] == 32)
			   return k;
	       n = k+3;
		   if( n+3 < stop && rStr[n] == 32 && rStr[n+3] == 32)
			   return k;
	   }
	   k++;
   }
   return i;
}
 void getDollarAmount(char *rStr, int start, int stop, char *dollarAm, int *cnfIn, int *cnfOut){
 int i, k;
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
 dollarAm[k] = '$';cnfOut[k] = 100;i++;k++;
 while(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];cnfOut[k] = cnfIn[i];k++; i++;}
 while(i < stop && rStr[i] == 32) i++;
 dollarAm[k] = '.';cnfOut[k] = 100;k++;
 if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];cnfOut[k] = cnfIn[i];} else {dollarAm[k] = '0';cnfOut[k] = 0;}
 k++; i++;
// if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];} else {dollarAm[k] = '0';}
 if(i < stop && rStr[i] != 32){dollarAm[k] = '0';cnfOut[k] = cnfIn[i];} else {dollarAm[k] = '0';cnfOut[k] = 0;}
 k++; i++;
 dollarAm[k] = 0;

// printf("-----Dollar amount= %s\n", dollarAm);
 }
 void getDollarAmountSumItUp(char *rStr, int start, int stop, char *dollarAm, char *totalAm, int *cnfIn, int *cnfOut, int *cnfTotalOut){
 int i, k;

// for(i=start;i<stop;i++)
  //   printf("%c", rStr[i]);

// printf("\n");
 totalAm[0] = 0;
 dollarAm[0] = 0;

 i = start;
// printf("i= %d", i);
// printf("rStr= %c", rStr[i+1]);
 if(rStr[i+1] == 32) i += 11;
 else i += 9;
// printf("i= %d", i);


 k = 0;
 while(i < stop && rStr[i] == 32) i++;
 while(i < stop && rStr[i] != 32){totalAm[k] = rStr[i];cnfTotalOut[k] = cnfIn[i];k++; i++;}
 totalAm[k] = 0;
 k = 0;
 while(i < stop && rStr[i] == 32) i++;
 k = 0;
 dollarAm[k] = '$';cnfOut[k] = 100;i++;k++;
 while(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];cnfOut[k] = cnfIn[i];k++; i++;}
 while(i < stop && rStr[i] == 32) i++;
 dollarAm[k] = '.';cnfOut[k] = 100;k++;
 if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];cnfOut[k] = cnfIn[i];} else {dollarAm[k] = '0';cnfOut[k] = 0;}
 k++; i++;
// if(i < stop && rStr[i] != 32){dollarAm[k] = rStr[i];} else {dollarAm[k] = '0';}
 if(i < stop && rStr[i] != 32){dollarAm[k] = 0;cnfOut[k] = cnfIn[i];} else {dollarAm[k] = '0';cnfOut[k] = 0;}
 k++; i++;
 dollarAm[k] = 0;

 cleanDollarAmount(dollarAm, 3, "SUM IT UP");
// printf("-----Total amount= %s\n", totalAm);
// printf("-----Dollar amount= %s\n", dollarAm);
 }

 void getDollarSign(char *result, int *iDollar){
	 int i, len;
	 *iDollar = 0;
	 len = strlen(result);
     for(i = len-1;i>4;i--){
    	 if(result[i] == '$') {*iDollar = i;return;}
     }
 }
 void stretchResults2(int idx, int lenStr){
 	int i, k, len;
 	   len = strlen(result);
//  printf ("---idx= %d, lenStr= %d\n",idx, lenStr);
 	   for(i=len;i>=idx;i--){
 		   k = i+lenStr;
 		   result[k] = result[i];

//  printf ("---k= %d, i= %d, r[k]= %d,%c, r[i]= %d,%c\n",k, i, result[k], result[k], result[i], result[i]);
 //		   cand1[k]  = cand1[i];
 //		   cand2[k]  = cand2[i];
 		   flConf[k] = flConf[i];
 	   }
 	   shiftQuadUp(flQuad, idx, len, lenStr);
 }

