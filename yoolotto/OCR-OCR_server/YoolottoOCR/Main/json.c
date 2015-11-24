/*
 * json.c
 *
 *  Created on: Dec 23, 2014
 *      Author: Wicek
 */

#include <stdio.h>
//#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/ocr.h"
#include "../headers/BarCode.h"
#include  "../headers/Prototype.h"

char *Line_Types[] = { " SUM ", " STR ", " BOX ", " STRBOX ", " FRONT ", " MID ", " BACK ", " ANY ", " COMBO", " EXT ", " EXTANY "};
int len_Line_Types = 11;
char *M_Names[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
int len_M_Names = 12;
int multiDraw;

int getLine(char* results, int n, int* iStart, int* iStop);
int matchToStringLine(char *lineStr, char *str, int *kM);
int getNumLinesFromStr(char *results);
int matchChars(char *m, char *monthName);



void printMultiDraw(LPTICKETRESULTS p, char *s){
          if(multiDraw > 0)
		      sprintf(s, "%s \"multiDraw\":%d,\n",s,1);
          else
		      sprintf(s, "%s \"multiDraw\":%d,\n",s,0);
}
  int getDrawTime(LPTICKETRESULTS  p){
  int i;
	for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];

		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DRAW") == 0) {
				  if(strcmp(result.result, "DAY DRAW") == 0) return 0;
				  if(strcmp(result.result, "NIGHT DRAW") == 0) return 1;
				  if(strcmp(result.result, "MORNING DRAW") == 0) return 2;
				  if(strcmp(result.result, "EVENING DRAW") == 0) return 3;
				  if(strcmp(result.result, "CONSECUTIVE DRAWS") == 0) return 4;
				  return -1;
			  }
		}
     }
     return -1;
 }
  int getDrawTimeTo(LPTICKETRESULTS  p){
  int i;
	for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];
		if( result.name != NULL && strlen(result.name) > 0 ){
//printf("result.name= %s\n",result.name);
			  if(strcmp(result.name, "DRAW TO") == 0) {
				  if(strcmp(result.result, "DAY") == 0) return 0;
				  if(strcmp(result.result, "NIGHT") == 0) return 1;
				  if(strcmp(result.result, "MORNING") == 0) return 2;
				  if(strcmp(result.result, "EVENING") == 0) return 3;
//				  if(strcmp(result.result, "CONSECUTIVE DRAWS") == 0) return 4;
				  return -1;
			  }
		}
     }
     return -1;
 }
 int getDrawTimeFrom(LPTICKETRESULTS  p){
  int i;
	for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];

		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DRAW FROM") == 0) {
				  if(strcmp(result.result, "DAY") == 0) return 0;
				  if(strcmp(result.result, "NIGHT") == 0) return 1;
				  if(strcmp(result.result, "MORNING") == 0) return 2;
				  if(strcmp(result.result, "EVENING") == 0) return 3;
//				  if(strcmp(result.result, "CONSECUTIVE DRAWS") == 0) return 4;
				  return -1;
			  }
		}
     }
     return -1;
 }

void printDrawTimeTX(LPTICKETRESULTS p, char *s){
	int dT = 0;
//    if(strlen(s)+strlen("\"drawTime\":")+2>= resultslength) return;
	switch(US_State){
	case 0:  // TX
	case 1:  // TX
		dT = getDrawTime(p);
		if(dT >= 0)
		  sprintf(s, "%s \"drawTime\":%d,\n",s,dT);
		else
		  sprintf(s, "%s \"drawTime\":null,\n",s);
//printf("-------------multiDraw= %d\n",multiDraw);
		if(multiDraw > 0){
			dT = getDrawTimeFrom(p);
			if(dT >= 0)
			  sprintf(s, "%s \"drawTimeFrom\":%d,\n",s,dT);
			else
			  sprintf(s, "%s \"drawTimeFrom\":null,\n",s);
			dT = getDrawTimeTo(p);
//			printf("-------------dTTO= %d\n",dT);
			if(dT >= 0)
			  sprintf(s, "%s \"drawTimeTo\":%d,\n",s,dT);
			else
			  sprintf(s, "%s \"drawTimeTo\":null,\n",s);
		}else{
			  sprintf(s, "%s \"drawTimeFrom\":null,\n",s);
			  sprintf(s, "%s \"drawTimeTo\":null,\n",s);
		}

		break;
	}
}
void printDrawTimeEmpty( char *s){
	  sprintf(s, "%s \"drawTime\":null,\n",s);
	  sprintf(s, "%s \"drawTimeFrom\":null,\n",s);
	  sprintf(s, "%s \"drawTimeTo\":null,\n",s);
}
void printMultiDrawEmpty(char *s){
      sprintf(s, "%s \"multiDraw\":null,\n",s);
}

void printState(char *s){
	sprintf(s, "%s \"state\": %d,\n", s, US_State);
}
void startLines(char *s){
	sprintf(s, "%s \"lines\": [\n", s);
}

void stopLines(char *s){
	sprintf(s, "%s\n],\n", s);
}

int getNextNum(char *results, int n, int *iStart, int *iStop){
	   int i, j, len, cc;
	   len = strlen(results);
	   cc = 0;
	   *iStart = 0;
	   *iStop = -1;
//printf("n= %d, %s",n, results);
	   i=0;
	   while(i<len) {if(results[i] != 32) break;i++;}
	   if(results[i] < '0' || results[i] > '9'){
		   while(i<len) {if(results[i] == 32) break;i++;}
		   while(i<len) {if(results[i] != 32) break;i++;}
		   if(results[i] < '0' || results[i] > '9')  return -1;
		   *iStart = i;
	   }
//   printf("i= %d, %s",i,&results[i]);
//	   k = 0;
//	   i = k;
	   while(i<len ){
		   if(results[i] == 32){
			 cc++;
		     if(cc > n){
			   *iStop = i-1;
//	   printf("*iStart= %d",*iStart);
//	   printf("*iStop= %d",*iStop);
			   return 0;
		     }
		     j = i;
		     while(j<len) {if(results[j] != 32) break;j++;}
		     i = j;
		     if(results[i] < '0' || results[i] > '9')  return -1;
		     if(i >= len) return -1;
		     *iStart = i;
//	   printf("*iStart= %d",*iStart);
		   }else
			   i++;
	   }
	   if(cc == n) {*iStop = len-1;
//	   printf("*iStop= %d",*iStop);
	   return 0;}
	   return -1;
}
void copyNum(char *lineStr, char *numL, int iStart, int iStop){
	int i, k;
	   k = 0;
	   if(iStop - iStart+1 >= 20) iStop = iStart+2;
	   for(i=iStart;i<=iStop;i++){
		   numL[k++] = lineStr[i];
	   }
	   numL[k] = 0;
}
void printOneLineTX(char *lineStr, char *s){
	int dig;
	int iStart, iStop;
	char numL[20];
	sprintf(s, "%s{\n",s);
	switch(ticketTypeInt){
	case 1:
	case 2:
	case 3:
	case 4:
	case 7:
	case 8:
		sprintf(s, "%s \"amount\":null,\n",s);
		sprintf(s, "%s \"numbers\": [\n",s);
		dig = 0;iStart = -1; iStop = -1;
		while(1){
			if(getNextNum(lineStr, dig, &iStart, &iStop)<0) break;
			copyNum(lineStr, numL, iStart, iStop);
			if(dig>0)
				sprintf(s, "%s,\n",s);
			sprintf(s, "%s %s",s,numL);
			dig++;
		}
        if(dig==0) sprintf(s, "%s null",s);
		sprintf(s, "%s\n ],\n",s);
		sprintf(s, "%s \"lineType\":null,\n",s);
		sprintf(s, "%s \"sumitUp\":null,\n",s);
		sprintf(s, "%s \"sumAmount\":null\n",s);
		break;
	}
	sprintf(s, "%s}",s);
}


void getAmmountFromLineTX(char *lineStr, char *amount){
	int i;
	amount[0] = 0;
	for(i=0;i<strlen(lineStr);i++)
		if(lineStr[i] == '$') {strcpy(amount, &lineStr[i+1]);break;}
	for(i=strlen(amount)-1;i>0;i--)
		if(amount[i]<=32) amount[i] = 0;
		else break;
}
int getLineTypeTX(char *lineStr){
	int type = 0;
//	char *Line_Types[] = { " SUM ", " STR ", " BOX ", " STRBOX ", " FRONT ", " MID ", " BACK ", " ANY ", " COMBO", " EXT ", " EXTANY "};
	int cc, ccMax, idxMax;
	int i, kkS ;

	cc = 0;ccMax = 0;idxMax = -1;

	for(i=0;i<len_Line_Types;i++) {
		 cc = matchToStringLine(lineStr, Line_Types[i], &kkS);
	      if(ccMax < cc)  {ccMax = cc;idxMax = i;}
	}
    type = idxMax+1;

	return type;
}
int getSumitUpTX(char *sumitUpLine, char *amount){
	int kkS, len;
	char sum[10];
	int n = 0;
	getAmmountFromLineTX(sumitUpLine, amount);
    matchToStringLine(sumitUpLine, "SUM IT UP", &kkS);
    len = strlen(sumitUpLine);
    if(kkS >=0 && kkS < len){
    	int i, k;
    	k = 0;
    	for(i=kkS+10;i<len;i++){
    		if(sumitUpLine[i] >= '0' || sumitUpLine[i] <='9'){
        		sum[k] = sumitUpLine[i];k++;
    		}
    		else {
    			sum[k] = 0;break;
    		}
    	}
    }
    sscanf(sum,"%d",&n);
	return n;
}
void printOneLineSumitUpTX(char *lineStr, char *s, char *sumitUpLine, int sumitUpFlg){
	int dig;
	int iStart, iStop;
	char numL[20];

	sprintf(s, "%s{\n",s);
	switch(ticketTypeInt){
	   case 5:
	   case 6:
		{ char amount[10];
		   int lineType, sumitUp;
		    getAmmountFromLineTX(lineStr, amount);
			sprintf(s, "%s \"amount\": \"%s\",\n",s, amount);
			sprintf(s, "%s \"numbers\": [\n",s);
			dig = 0;iStart = -1; iStop = -1;
			while(1){
				if(getNextNum(lineStr, dig, &iStart, &iStop)<0) break;
				copyNum(lineStr, numL, iStart, iStop);
				if(dig>0)
					sprintf(s, "%s,\n",s);
				sprintf(s, "%s %s",s,numL);
				dig++;
			}
	        if(dig==0) sprintf(s, "%s null",s);
			sprintf(s, "%s\n ],\n",s);
			lineType = getLineTypeTX(lineStr);
			sprintf(s, "%s \"lineType\": %d,\n",s, lineType);
			if(sumitUpFlg == 0){
			   sprintf(s, "%s \"sumitUp\":null,\n",s);
			   sprintf(s, "%s \"sumAmount\":null\n",s);
			}else{
				sumitUp = getSumitUpTX(sumitUpLine, amount);
				   sprintf(s, "%s \"sumitUp\": %d,\n",s, sumitUp);
				   sprintf(s, "%s \"sumAmount\": \"%s\"\n",s, amount);

			}
		}

		   break;
	}
	sprintf(s, "%s}",s);
}
void trimZeros(char *lineStr){
	char lineTmp[1000];
	int i, k, len, ok;

	strcpy(lineTmp, lineStr);
	len = strlen(lineTmp);
	i = 0;k = 0;
	while(i<len) {
		 ok = 1;
         if(lineTmp[i] ==  '0' && i < len-1){
        	 if(i == 0 && lineTmp[i+1] > 32)
        			 ok = 0;
        	 if(i > 0 && lineTmp[i-1] == ' ' && lineTmp[i+1] > 32)
        		 ok = 0;
         }
         if(ok){
        	 lineStr[k] = lineTmp[i];k++;
         }
         i++;
	}
	lineStr[k] = 0;
}
int getOneLine(char *results, int line, char *lineStr) {
 int iStart, iStop;
 int i;

   lineStr[0] = 0;
   if(getLine(results, line, &iStart, &iStop) < 0) return 0;
   for(i=iStart;i<=iStop;i++){
	   lineStr[i-iStart] = results[i];
   }
   lineStr[iStop-iStart+1] = 0;

// printf("line1 = %s\n", lineStr);
// fflush(stdout);
trimZeros(lineStr);
//printf("line2 = %s\n", lineStr);
// fflush(stdout);

  return 1;
}
int matchToStringLine(char *lineStr, char *str, int *kM){
	   int i, k, cc, ccMax, kMax;
	   int lenR = strlen(lineStr);
	   int len = strlen(str);

	   if(len > lenR) len = lenR;
	   ccMax = 0; kMax = -1;
	   for(k=0;k<lenR-len;k++){
		   cc = 0;
	      for(i=0;i<len;i++){
		     if(lineStr[i+k] == str[i]) cc++;
	      }
	      if(ccMax < cc) {ccMax = cc;kMax = k;}
	   }
	   *kM = kMax;
       return ccMax;
}

int isItSumitUp(char *sumitUp){
	int ccS, kkS;
	if(strlen(sumitUp) < 5) return 0;
	   ccS = matchToStringLine(sumitUp, "SUM IT UP", &kkS);
    if(ccS < 9) return 0;
	return 1;
}
void printEmptyLine(char *s){
	sprintf(s, "%s{\n",s);
    sprintf(s, "%s \"amount\":null,\n",s);
	sprintf(s, "%s \"numbers\": [null],\n",s);
	sprintf(s, "%s \"lineType\":null,\n",s);
	sprintf(s, "%s \"sumitUp\":null,\n",s);
	sprintf(s, "%s \"sumAmount\":null\n",s);
	sprintf(s, "%s}",s);
}
int printLinesTX(LPTICKETRESULTS p, char *s, int resultslength ){
	int i, flg, smUpflg;
	int line, numLines;
	char lineStr[1000];
	int emptyJson= 0;
	flg = 0;
	if(ticketTypeInt == 5 || ticketTypeInt == 6 ){
		char sumitUp[1000];
		int line1;
		  for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
			FIELDSRESULTS result = p->fieldsResults[i];
			if( result.name != NULL && strlen(result.name) > 0 ){
				  if(strcmp(result.name, "NUMBERS") == 0) {
					  numLines = getNumLinesFromStr(result.result);
					  if(numLines == 0) break;
					  flg = 1;
					  for( line = 0; line < numLines; line++ ){
						  smUpflg = 0;
						  if(getOneLine(result.result, line, lineStr) <= 0) {emptyJson = 1;break;}
						  if(strlen(s)+strlen(lineStr)+100>= resultslength) {emptyJson = 1;break;}
						  if(line>0)
							sprintf(s, "%s,\n",s);
						  line1 = line+1;
						  sumitUp[0] = 0;
						  if(line1 < numLines) {
							  getOneLine(result.result, line1, sumitUp);
							  smUpflg = isItSumitUp(sumitUp);
						  }
						  printOneLineSumitUpTX(lineStr, s, sumitUp, smUpflg );
						  if(smUpflg) line = line1;
					  }
					  sprintf(s, "%s\n",s);
				  }
		   }
		  }
	} else {
	  for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];
		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "NUMBERS") == 0) {
				  numLines = getNumLinesFromStr(result.result);
				  if(numLines == 0) break;
				  flg = 1;
				  for( line = 0; line < numLines; line++ ){
					  getOneLine(result.result, line, lineStr);
					  if(strlen(s)+strlen(lineStr)+100>= resultslength) {emptyJson = 1;break;}
					  if(line>0)
						sprintf(s, "%s,\n",s);
					  printOneLineTX(lineStr, s);
				  }
				  sprintf(s, "%s\n",s);
			  }


	   }
	  }
	}
	if(flg == 0) printEmptyLine(s);
	return emptyJson;

}
int printJsonLines(LPTICKETRESULTS p, char *s, int resultslength ){
	switch(US_State){
    case 0:
	case 1:
		 return printLinesTX(p, s, resultslength);
		break;
	}
	return 1;
}
int getNumLinesFromStr(char *results){
	int numLines = 0, i;
	int iStart, iStop;
	 i = 0;
	 while(1){
		if(getLine(results, i, &iStart, &iStop) < 0) break;
		numLines++;
		i++;
	 }
	return numLines;
}
void getJsonYearTX(char *result, char *str){
	int i, len;
	len = strlen(result);
	str[0] = 0;
	i=len-1;
	while(i>0){if(result[i]>32) break;i--;}
	i -= 3;
	if(i < 0) return;
	strcpy(str, &result[i]);
	str[4] = 0;
}
void getJsonMonthTX(char *result, char *str){
	int i, k, len;
	len = strlen(result);
	str[0] = 0;
	i=0;
	while(i<len){if(result[i]>32) break;i++;}
	while(i<len){if(result[i]==32) break;i++;}
	while(i<len){if(result[i]>32) break;i++;}
	k = 0;
	while(i<len){if(result[i]==32) break;str[k] = result[i];k++;i++;}
	str[k] = 0;
}

int testYear(char *tmpStr){
	int y;
	  sscanf(tmpStr,"%d",&y);
	  if(y<2014) return 0;
	  if(y>2114) return 0;
	return 1;
}
int testMonth(char *tmpStr){
	int cc, ccMax, idxMax, cc2;
	int i;
	cc = 0;ccMax = 0;idxMax = -1;
	cc2 = 0;
	for(i=0;i<12;i++) {
	   cc = matchChars(tmpStr, M_Names[i]);
	   if(ccMax == cc) cc2++;
	   else {
	      if(ccMax < cc){ccMax = cc;idxMax = i;cc2 = 0;}
	   }
	}
	idxMax++;
	tmpStr[0] = 0;
	if(cc2 == 0 && ccMax > 1) {
		if(idxMax<9) {strcpy(tmpStr,"0");tmpStr[1] = idxMax+'0';}
		else sprintf(tmpStr,"%d",idxMax);
		tmpStr[2]=0;
		return 1;
	}
	else return 0;
}
void getJsonDayTX(char *result, char *str){
	int i, len;
	len = strlen(result);
	str[0] = 0;
	i=len-1;
	while(i>0){if(result[i]>32) break;i--;}
	i -= 4;
	if(i < 0) return;
	while(i>0){if(result[i]>32) break;i--;}
	i -= 1;
	strcpy(str, &result[i]);
	str[2] = 0;
}
int testDay(char *tmpStr){
	int d;
	  sscanf(tmpStr,"%d",&d);
	  if(d<0) return 0;
	  if(d>31) return 0;
	return 1;
}
void printDrawDateTX(LPTICKETRESULTS p, char *s){
	char dateStr[20];
	char tmpStr[10];
	int i;

	  for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];
		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DATE") == 0) {
	             getJsonYearTX(result.result, tmpStr);
	             if(testYear(tmpStr)) strcpy(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdate\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonMonthTX(result.result, tmpStr);
	             if(testMonth(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdate\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonDayTX(result.result, tmpStr);
	             if(testDay(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdate\":null,\n",s);return;}
	             sprintf(s, "%s \"drawdate\": \"%s\",\n",s,dateStr);
	             break;
			  }
		}
     }
}
void printDrawDateToTX(LPTICKETRESULTS p, char *s){
	char dateStr[20];
	char tmpStr[10];
	int i;

	  for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];
		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DATE TO") == 0) {
	             getJsonYearTX(result.result, tmpStr);
	             if(testYear(tmpStr)) strcpy(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateTo\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonMonthTX(result.result, tmpStr);
	             if(testMonth(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateTo\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonDayTX(result.result, tmpStr);
	             if(testDay(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateTo\":null,\n",s);return;}
	             sprintf(s, "%s \"drawdateTo\": \"%s\",\n",s,dateStr);
	             break;
			  }
		}
     }
}
void printDrawDateFromTX(LPTICKETRESULTS p, char *s){
	char dateStr[20];
	char tmpStr[10];
	int i;

	  for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		FIELDSRESULTS result = p->fieldsResults[i];
		if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DATE") == 0) {
	             getJsonYearTX(result.result, tmpStr);
	             if(testYear(tmpStr)) strcpy(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateFrom\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonMonthTX(result.result, tmpStr);
	             if(testMonth(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateFrom\":null,\n",s);return;}
	             strcat(dateStr, "-");
	             getJsonDayTX(result.result, tmpStr);
	             if(testDay(tmpStr)) strcat(dateStr, tmpStr);
	             else { sprintf(s, "%s \"drawdateFrom\":null,\n",s);return;}
	             sprintf(s, "%s \"drawdateFrom\": \"%s\",\n",s,dateStr);
	             break;
			  }
		}
     }
}

void printDrawDate(LPTICKETRESULTS p, char *s){
  switch(US_State){
     case 0:
     case 1:
	    printDrawDateTX(p, s);
	    if(multiDraw > 0) {
	       printDrawDateFromTX(p, s);
	       printDrawDateToTX(p, s);
	    }else {
	       sprintf(s, "%s \"drawdateFrom\":null,\n",s);
	       sprintf(s, "%s \"drawdateTo\":null,\n",s);
	    }

	 break;
  }
}
void printDrawDateEmpty(char *s){
	sprintf(s, "%s \"drawdate\":null,\n",s);
	sprintf(s, "%s \"drawdateTo\":null,\n",s);
	sprintf(s, "%s \"drawdateFrom\":null,\n",s);
}
void printGameTypeIDTX(char *s){
	sprintf(s, "%s \"gametypeid\": %d,\n",s, ticketTypeInt);
}
void printGameTypeID(char *s){
	  switch(US_State){
	     case 0:
	     case 1:
	    	 printGameTypeIDTX(s);
		 break;
	  }
}
void printMultiYesNo(char *result, char *s){
	int cc, ccMax, idxMax;
	cc = 0;ccMax = 0;idxMax = -1;


    cc = matchChars(result, "YES");
    if(ccMax < cc){ccMax = cc;idxMax = 0;}
    cc = matchChars(result, "NO");
    if(ccMax < cc){ccMax = cc;idxMax = 1;}
    if(idxMax == 0)
    	  sprintf(s, "%s \"ismultiplier\": true\n",s);
    else
    	  sprintf(s, "%s \"ismultiplier\": false\n",s);
}
void printMultiplierTX(LPTICKETRESULTS p, char *s){
	int i;
	char fieldName[100];
	switch(ticketTypeInt){
	   case 1:
	   case 2:
		   strcpy(fieldName, "PLAYER");
		   break;
	   case 3:
		   strcpy(fieldName, "EXTRA");
		   break;
	}
	switch(ticketTypeInt){
	   case 1:
	   case 2:
	   case 3:
	     for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
		    FIELDSRESULTS result = p->fieldsResults[i];
		    if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, fieldName) == 0) {
                  printMultiYesNo(result.result, s);
			    }
		   }
	     }
	     break;
       case 4:
       case 5:
       case 6:
       case 7:
       case 8:
           printMultiYesNo("NO", s);
		 break;

	}
}
void printMultiplier(LPTICKETRESULTS p, char *s){
	  switch(US_State){
	     case 0:
	     case 1:
	    	 printMultiplierTX(p, s);
		 break;
	  }
}
int isItMultiDrawTX(LPTICKETRESULTS p){
	int i, n = 0;
    for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
	    FIELDSRESULTS result = p->fieldsResults[i];
	    if( result.name != NULL && strlen(result.name) > 0 ){
			  if(strcmp(result.name, "DATE TO") == 0){
                 n = 1;break;
			  }
	   }
    }
return n;
}
int isItMultiDraw(LPTICKETRESULTS p){
	int n = 0;
	  switch(US_State){
	     case 0:
	     case 1:
	    	 n = isItMultiDrawTX(p);
		 break;
	  }
	return n;
}
void printRetNumEmpty( char *s){
	  sprintf(s, "%s \"retnum\":null,\n",s);
}

void printRetNumTX(LPTICKETRESULTS p, char *s){
	  int i;
		for( i = 0; i < NUMFIELDS_RESULTS; i++ ){
			FIELDSRESULTS result = p->fieldsResults[i];
			if( result.name != NULL && strlen(result.name) > 0 ){
				  if(strcmp(result.name, "RET_NUM") == 0) {
					sprintf(s, "%s \"retnum\": \"%s\",\n",s, result.result);
					  return;
				  }
			}
	     }
		printRetNumEmpty(s);
}
void printRetNum(LPTICKETRESULTS p, char *s){
	  switch(US_State){
	     case 0:
	     case 1:
	    	 printRetNumTX(p, s);
		 break;
	  }
}

void getTicketJsonEmpty( char *s, int resultslength){

	sprintf(s, "{\n");
	printMultiDrawEmpty(s);
	printDrawTimeEmpty(s);
	printState(s);
	startLines(s);
    printEmptyLine(s);
    printDrawDateEmpty(s);
    stopLines(s);
    printGameTypeID(s);
    printRetNumEmpty(s);
    printMultiYesNo("NO", s);
	strcat(s, "}");
}

void getTicketJson( LPTICKETRESULTS p, char *s, int resultslength ){
//	int i;
//	int line, numLines;
	int emptyJson = 0;
//    if(resultslength < 1000) {printEmptyJsonTX();return;}
	multiDraw = isItMultiDraw(p);
	sprintf(s, "{\n");
	printMultiDraw(p, s);
	printDrawTimeTX(p, s);
	printState(s);
	startLines(s);
	emptyJson = printJsonLines(p, s, resultslength);
	stopLines(s);
    if(emptyJson) printEmptyLine(s);
	printDrawDate(p, s);
    printGameTypeID(s);
    printRetNum(p, s);
    printMultiplier(p, s);

//	    "gametypeid": 5,
//	    "ismultiplier": false
	strcat(s, "}\n");

}

