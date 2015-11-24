/*
 * BarcodeI25.c
 *
 *  Created on: Feb 20, 2013
 *      Author: Wicek
 */


#include <stdio.h>
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>

#include "../headers/util.h"
#include "../headers/BarCode.h"
char bufLog[500];
int printFlg;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//---------------------------------------------------
// Code I25
//---------------------------------------------------
//void saveLog(char *str1, char *str2);
void printHist(int *hist, int lenH);

#define STRI25_LEN      30
#define CODEI25_LEN     10
#define LEN_STARTI25     4
#define LEN_STOPI25      3

#define ONEBIT       0x80
#define DSTEP        4
#define MIN_LENHIST  80
#define MAX_UNITS    30

//#define STRI25_LEN      30
//#define CODEI25_LEN     40

#define CODEMAX_LEN 160
#define STRMAX_LEN  100
#define DISTR_SIZE 30
#define LEN_TABC       200
#define DSTEP2        4
#define CODEUPC_LEN 10


int  codeBarUPC_L[CODEUPC_LEN][4];
int  codeBarI25[CODEI25_LEN][5];

int  codeBarUPCLVector[CODEUPC_LEN*4] = {
       3,2,1,1, // 0
       2,2,2,1, // 1
       2,1,2,2, // 2
       1,4,1,1, // 3
       1,1,3,2, // 4
       1,2,3,1, // 5
       1,1,1,4, // 6
       1,3,1,2, // 7
       1,2,1,3, // 8
       3,1,1,2, // 9
       };

char tabTransUPC[CODEUPC_LEN] = {
       '0','1','2','3','4','5','6','7','8','9',
    };

/*
int  codeBarI25[CODEI25_LEN][5] = {
       0,0,1,1,0, // 0
       1,0,0,0,1, // 1
       0,1,0,0,1, // 2
       1,1,0,0,0, // 3
       0,0,1,0,1, // 4
       1,0,1,0,0, // 5
       0,1,1,0,0, // 6
       0,0,0,1,1, // 7
       1,0,0,1,0, // 8
       0,1,0,1,0, // 9
       };
*/
int  codeBarI25Vector[CODEI25_LEN*5] = {
       0,0,1,1,0, // 0
       1,0,0,0,1, // 1
       0,1,0,0,1, // 2
       1,1,0,0,0, // 3
       0,0,1,0,1, // 4
       1,0,1,0,0, // 5
       0,1,1,0,0, // 6
       0,0,0,1,1, // 7
       1,0,0,1,0, // 8
       0,1,0,1,0, // 9
       };

int  codeStopI25[3] = {
       0,0,1  // *
       };
int startI25[LEN_STARTI25] = {
       0,0,0,0
       };

int stopI25[LEN_STOPI25] = {
       1,0,0
       };

char tabTransI25[CODEI25_LEN] = {
       '0','1','2','3','4','5','6','7','8','9',
    };

int nOF_W_BARSI25;
int nOF_N_BARSI25;
int nOF_N_SPACESI25;
int nOF_W_SPACESI25;

BYTE    maskInfo[STRI25_LEN][CODEI25_LEN];

int     distrB_bl[DISTR_SIZE];
int     distrB_wh[DISTR_SIZE];
char    tabPron[STRI25_LEN];
char    tabPron2[STRI25_LEN];

long srHist;
long srHistG;
long srHistBest;
long srHistBestBl;
long srHistBestWh;

int     lenCodeMax;
int     tabUnitsHistSht[MAX_UNITS];
int     lenUnitsHistSht;

int  counterBar[CODEMAX_LEN][4];
char candCode1[STRMAX_LEN + 1];
char candCode2[STRMAX_LEN + 1];

int confCode1[STRMAX_LEN];
int confCode2[STRMAX_LEN];
int confCode12[STRMAX_LEN];

char    tabPronMax[STRI25_LEN];
int     tabUnitsHist[MAX_UNITS];
int     lenUnitsHist;

int  tHBest;
int  tHBestBl;
int  tHBestWh;
int  tabBarC[LEN_TABC];
int  tabBarC1[LEN_TABC];
int  tabBarC2[LEN_TABC];
int  indHistB;
int  flgCol;
int  countBar;
int  flgEndBar;
int  indDeCode;
int     lenUnitsHistSt;
int     tabUnitsHistBl[MAX_UNITS];
int     lenUnitsHistBl;

int     tabUnitsHistSt[MAX_UNITS];
int     lenUnitsHistSt;

int     tabUnitsHistWh[MAX_UNITS];
int     lenUnitsHistWh;

//int  flgCode;

void maskInterpretI25(
  LPMASK ptrMask,
  int    nOfCh
  );

int distrBarI25(
   int   *hist,               // vertical histogram of barcode
   int      lenH,                // histogram length
   int      nOfUnits,
   long     srH,
   int      maxH,
   int      iBeg,
   int      nOfChar,
   int      totChar,
   int      iDelt,
   int      indChar
   );

int decBarPI25(
   int      nOfChar,
   int   *indChar,
   int   *indMask
   );

int deCodeOneBarI25_1(int  *tabBar, LPBYTE  maskI);
int deCodeOneBarI25_2(int  *tabBar,LPBYTE  maskI);
void moveTab(int targetTab,int sourceTab, int len);
int distrAn( int  *dist_bl, int  *dist_wh, int nOF_W_B, int nOF_N_B, int nOF_N_S, int nOF_W_S,
             int  *val, int  *ind, int  *valBl, int  *indBl, int  *difBl, int  *valWh, int  *indWh, int  *difWh);
void binCode(int *hist, int iStop, int *tabC, int iBeg, long srH, int tH, int dir);
int shiftStartI25(int *hist, int lenH,  int nOfUnits, long srH, int maxH, int iBeg, int nOfChar, int totChar,
                  int iDelt, int indChar);
void histTrim( int  *hist,  int  *lenH);

int codeTestI25(int *tabC, int lenStr);
void setCodeBarI25(void);


//*****************************************************************
// return 15 start and stop problem
//                         (this is only information, code can be ok)
// return 10 start problem (this is only information, code can be ok)
// return  5 stop problem  (this is only information, code can be ok)
// return  0 standard output
// if output string is from opposite direction return = return + 1;
// return -1 too short histogram (no output string)
//*****************************************************************
int barDeCodeI25 (
    int   *hist,               // vertical histogram of barcode
    int      lenH,                // histogram length
    LPMASK   maskAlph,            // pointer to alphabet mask
    int      lenStr,              // number of characters for recognition
    int      nOfCharInBar,        // total number of characters
    int      invDec,              // 0 - no opposite direction decoding
                                  // 1 - opposite direction decoding if
                                  //     can not decode at least one character
                                  // 2 - opposite direction decoding if
                                  //     the same as 1 or start and/or stop problem
    LPSTR    strResult,           // pointer to output string
    int   *confResult,         // results confidences
    LPSTR    cand2                // second candidates
    )
{
 int  i, j, k, ii, iInv;
 int  maxHist;
 int  minValue, minValueBl, minValueWh;
 int len1;
 int vaLue, ind, vaLueBl, indBl, vaLueWh, indWh;
 int difBl, difWh;
 int minDifBl, minDifWh;
 LPMASK ptrMask;
 int iBeg, iDelt, ret;
 int lenCode, flgCut, starFlg;
 int iMask, countUnit, countUnitTot;
 int nOfNq, lenHH;
 int sTart = 0;
 int sTop  = 0;
 int startValue;
 int nOfIter = 0;
 int flgIter = 0;
 int ccBar, ccBarOk, conf1;

  printFlg = 0;

  strResult[0] = 0;
  if(lenH < MIN_LENHIST)
              return(-1);
//printHist(hist, lenH);

   lenHH = lenH;
   histTrim(hist, (int *)&lenHH);
   lenH = lenHH;
   if(lenH < MIN_LENHIST)
              return(-1);

// saveLog( "--------------", "\n\n" );
//printHist(hist, lenH);

   setCodeBarI25();

   lenCode    = 0;
   lenCodeMax = 0;
   starFlg    = 1;
   nOfNq      = 0;

//????????????????? index 0 ???
// interpret mask info
   for(i = 0; i < STRI25_LEN; i++)
           for(j = 0; j < CODEI25_LEN; j++)
                    maskInfo[i][j] = j;

   ptrMask = maskAlph;
   if(ptrMask != NULL)
       maskInterpretI25(ptrMask,nOfCharInBar);


//-----------------------
//--------TEMPORARY !!!!
//if(lenH < 1000)
//  for(i=0;i<lenH;i++)
//   histBarC[i] = hist[i];
//--------TEMPORARY !!!!
//-----------------------


   i      = 0;
   len1 = lenH -1;
   srHist = 0L;
   maxHist = 0;
   for(i=1;i<len1;i++) {
        srHist += hist[i];
        if(hist[i] > maxHist)
                   maxHist = hist[i];

        }

   srHist /= len1;
   srHist = srHist/2;
   if(srHist < 4) return -1;
   srHistG = srHist;

//   flgCode = whichCodeI25(hist, lenH, srHist, maxHist);

//   nOF_W_BARSI25   =   nOfCharInBar * 2;???
//   nOF_N_BARSI25   =   nOfCharInBar * 3;???
//   nOF_N_SPACES39  =   nOfCharInBar * 3;
//   nOF_W_SPACES39  =   nOfCharInBar * 2;

AGNBAR:
   srHist = srHistG;
   sTart = 0;
   sTop  = 0;
   nOfIter++;
   for(ii=0;ii<MAX_UNITS;ii++)
       tabUnitsHistSht[ii] = 0;
   candCode1[0] = 0;
   candCode2[0] = 0;


//--------------------------------
  if(shiftStartI25(hist, lenH, /* units */ 23,
                  srHist, maxHist, lenH - 1, /* char */ 2,
                  nOfCharInBar, -1,0) == 0) sTop = 5;

//   printf("tabUnitsHistSt, lenUnitsHistSt= %d, sTop= %d\n",lenUnitsHistSt, sTop);
//  sprintf(bufLog,"tabUnitsHistSt, lenUnitsHistSt= %d, sTop= %d\n",lenUnitsHistSt, sTop);
//  saveLog( bufLog, "\n" );
//  printHist(tabUnitsHistSt, lenUnitsHistSt);

//   printHist(hist, lenH);

  startValue = shiftStartI25(hist, lenH, /* units */ 24,
                             srHist, maxHist, 0, /* char */ 2,
                             nOfCharInBar, 1,0);
  tabUnitsHistSht[0] = startValue;
  if(startValue == 0) sTart = 10;

// printf("tabUnitsHistSt, lenUnitsHistSt= %d, sTart= %d, startValue= %d\n",lenUnitsHistSt, sTart, startValue);

//  sprintf(bufLog,"tabUnitsHistSt, lenUnitsHistSt= %d, sTart= %d, startValue= %d\n",lenUnitsHistSt, sTart, startValue);
//  saveLog( bufLog, "\n" );
//  printHist(tabUnitsHistSt, lenUnitsHistSt);

//goto TEMP;

   for(i=0;i<10;i++) {
      j = tabUnitsHistSht[i];
      k = 0;
      iMask = 0;
      while(j < lenH)  {
        j = distrBarI25(hist, lenH, /* units */ 20,
                     srHist, maxHist, j, /* char */ 2,
                     nOfCharInBar, 1,k);
        if((i == 0) && (k == 0)) {
           if(lenUnitsHist + 1 >= MAX_UNITS)
                lenUnitsHist = MAX_UNITS - 2;
           for(ii=0;ii<lenUnitsHist;ii++)
               tabUnitsHistSht[ii + 1] = tabUnitsHist[ii];
           }
        ret =  decBarPI25(2, &k, &iMask);

        lenCode = k;
        if(lenCode >= STRI25_LEN) break;

        if((j < 0) || (ret < 0)) break;
        if(k >= lenStr) {
           ind = 0;
           for(ii=0;ii<lenCode;ii++)
               if(tabPron[ii] >= 0) ind++;
           if(ind > nOfNq) {
             lenCodeMax = lenCode;
             nOfNq = ind;
             flgIter = nOfIter;
             for(ii=0;ii<lenCode;ii++)
                  tabPronMax[ii] = tabPron[ii];
             }
           if(ind >= lenCode) {
//      	     sprintf(bufLog,"goto ENDC;, ind= %d, lenCode= %d\n",ind, lenCode);
//      	     saveLog( bufLog, "\n" );

                goto ENDC;
           }
           }
        }
//printf("lenCode= %d\n",lenCode);
      if(lenCode > 0) {
           ind = 0;
           for(ii=0;ii<lenCode;ii++)
               if(tabPron[ii] >= 0) ind++;
           if((ind > nOfNq) || ((ind == nOfNq) && (lenCode < lenCodeMax))) {
//         if(lenCode > lenCodeMax) {
             lenCodeMax = lenCode;
             nOfNq = ind;
             flgIter = nOfIter;
             for(ii=0;ii<lenCode;ii++)
                tabPronMax[ii] = tabPron[ii];
//             if(lenH - j < 20) goto ENDC;
             if((ind >= lenCode) && (lenH - j < 20)) {
//printf("goto ENDC;, ind= %d, lenCode= %d, lenH - j=%d\n",ind, lenCode, lenH - j);


//for(ii=0;ii<lenCode;ii++) {
	//printf(" tabPronMax[%d]= %c\n",ii, tabPronMax[ii]);
//}


//          	     sprintf(bufLog,"goto ENDC;, ind= %d, lenCode= %d, lenH - j=%d\n",ind, lenCode, lenH - j);
//          	     saveLog( bufLog, "\n" );

                goto ENDC;
             }
             }
         }
      }

//     sprintf(bufLog,"OK;, ind= %d, lenCode= %d, lenH - j=%d\n",ind, lenCode, lenH - j);
//     saveLog( bufLog, "\n" );

//--------------------------------
// distribution of bars
//TEMP:
   minValue     = 1000;
   tHBest       = 4;
   minValueBl   = 1000;
   tHBestBl     = 4;
   minValueWh   = 1000;
   tHBestWh     = 4;
   minDifBl     = 1000;
   minDifWh     = 1000;
   srHistBest   = (srHist*3)/2;
   srHistBestBl = (srHist*3)/2;
   srHistBestWh = (srHist*3)/2;
   ccBar        = 0;
   ccBarOk      = 0;
   while(1) {
//     sprintf(bufLog,"break1; srHist= %ld, maxHist -2= %d\n",srHist, maxHist -2);
//     saveLog( bufLog, "\n" );

      if(srHist >  maxHist -2) break;
      for(i = 0;i < DISTR_SIZE; i++) {
              distrB_bl[i] = 0;
              distrB_wh[i] = 0;
              }
      for(i = startValue;i < len1; i++) {
          if(hist[i] > srHist){
//        	     sprintf(bufLog,"break2; i=% d, hist[i]= %d, srHist= %ld\n",i, hist[i], srHist);
//        	     saveLog( bufLog, "\n" );

                         break;
              }
          }
      indHistB  = i;
      flgCol    = 1;
      countBar  = 0;
      flgEndBar = 0;
      countUnit = 0;
      conf1     = 0;
      for(i=indHistB;i<len1;i++) {
        if(flgCol) {
           if(hist[i] <= srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        else {
           if(hist[i] > srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        if(flgEndBar) {
               if((countBar > 0) && (countBar < DISTR_SIZE)) {
                  if(flgCol)
                      distrB_bl[countBar]++;
                  else
                      distrB_wh[countBar]++;
                  }
               flgCol = 1 - flgCol;
               countBar  = 1;
               flgEndBar = 0;
               countUnit++;
               }
        } // for i

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     if(flgEndBar == 0) {
        if((countBar > 0) && (countBar < DISTR_SIZE)) {
                  if(flgCol)
                      distrB_bl[countBar]++;
                  else
                      distrB_wh[countBar]++;
                  }
        countUnit++;
        }
      if(sTop == 0)
            countUnit -= 3;
      countUnitTot = countUnit/10;
      if(countUnit%10 >= 5) countUnitTot++;
      nOF_W_BARSI25   =   countUnitTot * 2;
      nOF_N_BARSI25   =   countUnitTot * 3;
      nOF_N_SPACESI25 =   countUnitTot * 3;
      nOF_W_SPACESI25 =   countUnitTot * 2;
      if(sTop == 0) {
         nOF_W_BARSI25   ++;
         nOF_N_BARSI25   ++;
         nOF_N_SPACESI25 ++;
         }

      ret = distrAn((int *)distrB_bl,(int *)distrB_wh,
                 nOF_W_BARSI25, nOF_N_BARSI25,
                 nOF_N_SPACESI25, nOF_W_SPACESI25,
                 (int  *)&vaLue,(int *)&ind,
                 (int  *)&vaLueBl,(int *)&indBl,(int *)&difBl,
                 (int  *)&vaLueWh,(int *)&indWh,(int *)&difWh);
/*
      sprintf(bufLog,"ret= %d, vaLue= %d, ind= %d\n",ret, vaLue, ind);
      saveLog( bufLog, "\n" );
      sprintf(bufLog,"vaLueBl= %d, indBl= %d, difBl= %d\n",vaLueBl, indBl, difBl);
      saveLog( bufLog, "\n" );
      sprintf(bufLog,"vaLueWh= %d, indWh= %d, difWh= %d\n",vaLueWh, indWh, difWh);
      saveLog( bufLog, "\n" );
*/
      if(ret && (ind >= 0)) {
           binCode(hist, lenH, (int *)tabBarC, indHistB,
                   srHist, ind, 1);

           if(codeTestI25((int *)tabBarC, lenStr))
                                   ccBarOk += 2;
           conf1 += 2;
           if(vaLue < minValue) {
             minValue   = vaLue;
             tHBest     = ind;
             srHistBest = srHist;
             }
           }
      if(indBl >= 0) {
//          sprintf(bufLog,"-----------indBl= %d\n",indBl);
//          saveLog( bufLog, "\n" );


           binCode(hist, lenH, (int *)tabBarC, indHistB,
                   srHist, indBl, 1);

//            printHist(tabBarC, indDeCode);

     if(indBl == 6) printFlg = 1;

           if(codeTestI25((int *)tabBarC, lenStr))
                ccBarOk++;
     printFlg = 0;
           conf1++;
           if(difBl < minDifBl) {
             minValueBl   = vaLueBl;
             tHBestBl     = indBl;
             srHistBestBl = srHist;
             minDifBl     = difBl;
             }
           else {
             if((difBl == minDifBl) && (vaLueBl < minValueBl)) {
               minValueBl   = vaLueBl;
               tHBestBl     = indBl;
               srHistBestBl = srHist;
               }
             }
           }
      if(indWh >= 0) {
           binCode(hist, lenH, (int *)tabBarC, indHistB,
                   srHist, indWh, 1);

//           sprintf(bufLog,"indWh= %d\n",indWh);
//           saveLog( bufLog, "\n" );
//           printHist(tabBarC, indDeCode);

           if(codeTestI25((int *)tabBarC, lenStr))
                                   ccBarOk++;
           conf1++;
           if(difWh < minDifWh) {
             minValueWh   = vaLueWh;
             tHBestWh     = indWh;
             srHistBestWh = srHist;
             minDifWh     = difWh;
             }
           else {
             if((difWh == minDifWh) && (vaLueWh < minValueWh)) {
               minValueWh   = vaLueWh;
               tHBestWh     = indWh;
               srHistBestWh = srHist;
               }
             }
           }
      if(conf1 <=0) conf1 = 1;
      ccBar += conf1;
      srHist += DSTEP;
      } // while

//-------------------------------------------------------
// binary code
   flgCut = 0;
   srHist = srHistBestBl;
   if(srHist > srHistBestWh)
          srHist = srHistBestWh;
   if(minValue < 1000) {
       srHist = srHistBest;
       flgCut = 1;
       }
    iBeg  = startValue;
    iDelt = 1;

//AGNBAR:
//    indDeCode = 0;
    i     = iBeg;
    while((i<lenH)&&(i>=0)) {
        if(hist[i] > srHist)
                         break;
        i += iDelt;
        }

 if(flgCut)    {

   binCode(hist, lenH, (int *)tabBarC, i,
             srHistBest, tHBest, iDelt);

    } // flgCut
  else {

   binCode(hist, lenH, (int *)tabBarC1, i,
             srHistBestBl, tHBestBl, iDelt);


   binCode(hist, lenH, (int  *)tabBarC2, i,
             srHistBestWh, tHBestWh, iDelt);

     for(i=0;i<200;i++) {
      if((i%2) == 0) tabBarC[i] = tabBarC1[i];
      else           tabBarC[i] = tabBarC2[i];
      }

    } // else flgCut

       if(indDeCode < 10) goto ENDC;

        for(ind=0;ind<10;ind++) {
           i = 0;
           indHistB = ind;
           while(1) {
              flgCol = deCodeOneBarI25_1((int  *)&tabBarC[indHistB],(LPBYTE)maskInfo[i]);
              if(flgCol < 0)
                 tabPron[i] = -1;
              else
                 tabPron[i] = tabTransI25[flgCol];
              i++;
              flgCol = deCodeOneBarI25_2((int  *)&tabBarC[indHistB],(LPBYTE)maskInfo[i]);
              if(flgCol < 0)
                 tabPron[i] = -1;
              else
                 tabPron[i] = tabTransI25[flgCol];


              if(i == lenStr - 1)
                       break;

              indHistB += 10;
              if(indHistB + 8 > indDeCode)
                                   break;
              i++;
              }
           i++;
           k = 0;
           for(j=0;j<i;j++)
             if(tabPron[j] >= 0) k++;
           if(k > nOfNq)  {
             flgIter = nOfIter;
             for(j=0;j<i;j++) {
                  tabPronMax[j] = tabPron[j];
               }
             nOfNq = k;
             lenCodeMax = i;
             if(k == i) break;
             }
           } // for




    if((lenCodeMax > nOfNq) || sTop || sTart) {
       if(invDec) {
           for(iInv=0;iInv<lenH/2;iInv++) {
                i = hist[iInv];
                j = lenH - iInv - 1;
                hist[iInv] = hist[j];
                hist[j]    = i;
                }
         invDec = 0;
         lenCode    = 0;
         lenCodeMax = 0;
         starFlg    = 1;
         nOfNq      = 0;
         goto AGNBAR;
         }
      }

  conf1 = 0;
  if(ccBar > 0) conf1 = (ccBarOk*100)/ccBar;
  if(lenCodeMax > 0) {
       for(i=0; i<lenCodeMax; i++)  {
          confCode1[i] = conf1;
          candCode1[i]  =  tabPronMax[i];
          candCode2[i]  =  tabPronMax[i];
          }
       }


ENDC:

     strResult[0]  = 0;
     if(lenCodeMax > 0) {
       for(i=0; i<lenCodeMax; i++)  {
           if(tabPronMax[i] < 0)
              strResult[i]  = '?';
           else
              strResult[i]  = tabPronMax[i];
           }
       strResult[lenCodeMax]  = 0;
       for(i=0; i<strlen(candCode1); i++)  {
          if(candCode1[i] < 0) {candCode1[i] = '?';confCode1[i] = 0;}
          if(candCode2[i] < 0) candCode2[i] = candCode1[i];
          confResult[i] = confCode1[i];
          cand2[i]      = candCode2[i];
          }
       candCode1[lenCodeMax]  = 0;
       candCode2[lenCodeMax]  = 0;
       for(i=0; i<lenCodeMax; i++)  {
           if(strResult[i]  != candCode1[i])
              if(confCode12[i] < 10)
                      strResult[i]  = candCode1[i];
           }
       }


//-------------------------------------------------------
    if(flgIter > 1) flgIter = 1;
    else            flgIter = 0;
   return(sTop + sTart + flgIter);
//    return(0);

} /* barDeCodeI25() */

int codeTestI25(int  *tabC, int lenStr)
{
 int i, ind, indB, flgCol, flg;


// sprintf(bufLog,"indDeCode= %d, lenStr= %d\n",indDeCode, lenStr);
// saveLog( bufLog, "\n" );

     if(indDeCode < 10) return 0;
        for(ind=0;ind<10;ind++) {
           flg = 1;
           i = 0;
           indB = ind;
           while(1) {
              flgCol = deCodeOneBarI25_1((int  *)&tabC[indB],(LPBYTE)maskInfo[i]);
//     if(printFlg == 1){
//        sprintf(bufLog,"_1; ind= %d, flgCol= %d, i= %d\n",ind, flgCol, i);
//        saveLog( bufLog, "\n" );
//     }

              if(flgCol < 0)  {
                 flg = 0;
                 break;
                 }
              i++;
              flgCol = deCodeOneBarI25_2((int  *)&tabC[indB],(LPBYTE)maskInfo[i]);
//   if(printFlg == 1){
//            sprintf(bufLog,"_2; ind= %d, flgCol= %d, i= %d\n",ind, flgCol, i);
//            saveLog( bufLog, "\n" );
//   }

              if(flgCol < 0) {
                 flg = 0;
                 break;
                 }
              if(i == lenStr - 1)
                       break;
              indB += 10;
              if(indB + 8 > indDeCode)
                                   break;
              i++;
              }
//   if(printFlg == 1){
//           sprintf(bufLog," ind= %d, flg= %d\n",ind, flg);
//           saveLog( bufLog, "\n" );
//   }
           if(flg) return 1;
           }

      return 0;

} /* codeTest() */


void maskInterpretI25(
  LPMASK ptrMask,
  int    nOfCh
  )
{
 int  i, j, k, iBit;
 BYTE infoByte;
 BYTE  maskTmp;

  if(ptrMask == NULL) return;

  for(i=0;i<nOfCh;i++) {
     infoByte = ptrMask[i].info;

// full alphabet
     if(infoByte >= 128) continue;
// coded alphabet
     if(infoByte >= 64) {
       memset((LPBYTE)maskInfo[i], 100,  CODEI25_LEN);
       k = 0;
       for(j = 0; j < 6; j++) {
           maskTmp = ONEBIT;
           for(iBit = 0; iBit < 8; iBit++) {
               if(maskTmp & ptrMask[i].realMask[j]) {
                  maskInfo[i][k] = j*8+iBit;
                  k++;
                  }
               }
           }
       continue;
       }
     }
} /* of maskInterpretI25() */

int distrBarI25(
   int   *hist,               // vertical histogram of barcode
   int      lenH,                // histogram length
   int      nOfUnits,
   long     srH,
   int      maxH,
   int      iBeg,
   int      nOfChar,
   int      totChar,
   int      iDelt,
   int      indChar
   )
{
 int i, ret, flgCut;
 int len1, count;
 int nOF_W_B, nOF_N_B;
 int nOF_N_S, nOF_W_S;
 int  minValue, minValueBl, minValueWh;
 int vaLue, ind, vaLueBl, indBl, vaLueWh, indWh;
 int difBl, difWh;
 int minDifBl, minDifWh;
 int indH, indHBl, indHWh;
 long srHist;
 int cntSt, cntWh, cntBl, curInd;
 int  kk, conf1, conf2, confCount;
 int  iFst, chLst, flgAll, flgAllT, nOfChT;

   srHist       = srH;
   minValue     = 1000;
   tHBest       = 4;
   minValueBl   = 1000;
   tHBestBl     = 4;
   minValueWh   = 1000;
   tHBestWh     = 4;
   minDifBl     = 1000;
   minDifWh     = 1000;
   srHistBest   = (srH*3)/2;
   srHistBestBl = (srH*3)/2;
   srHistBestWh = (srH*3)/2;

   chLst = indChar+nOfChar;
   for(i=indChar;i<2*chLst;i++) {
     candCode1[i] = -1;
     confCode1[i] =  0;
     candCode2[i] = -1;
     confCode2[i] =  0;
     for(kk=0;kk<CODEI25_LEN;kk++)
       counterBar[kk][i-indChar] = 0;
     }


  len1   = lenH;
  indH   = iBeg+10;
  indHBl = iBeg+10;
  indHWh = iBeg+10;
  cntSt  = 0;
  cntBl  = 0;
  cntWh  = 0;
  confCount = 0;
  flgAll    = 0;
  flgAllT   = 0;
  while(1) {
      if(srHist >  maxH -2) break;
      nOF_W_B  =   nOfChar * 2;
      nOF_N_B  =   nOfChar * 3;
      nOF_N_S  =   nOfChar * 3;
      nOF_W_S  =   nOfChar * 2;

      for(i = 0;i < DISTR_SIZE; i++) {
              distrB_bl[i] = 0;
              distrB_wh[i] = 0;
              }
      for(i = iBeg;i < len1; i++) {
          if(hist[i] > srHist)
                         break;
          }
      indHistB  = i;
      flgCol    = 1;
      countBar  = 0;
      flgEndBar = 0;
      count     = 0;
      curInd    = 0;
      for(i=indHistB;i<len1;i++) {
        if(flgCol) {
           if(hist[i] <= srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        else {
           if(hist[i] > srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        if(flgEndBar) {
               if((countBar > 0) && (countBar < DISTR_SIZE)) {
                  if(flgCol)
                      distrB_bl[countBar]++;
                  else
                      distrB_wh[countBar]++;
                  }
               if(curInd < MAX_UNITS)   {
                     tabUnitsHist[curInd] = i;
                     curInd++;
                     }
               flgCol = 1 - flgCol;
               countBar  = 1;
               flgEndBar = 0;
               count++;
               }
        if(count >= nOfUnits) break;
        } // for i
      nOfChT = nOfChar;
      if(nOfUnits - 5 > count) {
        flgAllT = 0;
        if(i == len1) { // STOP ???????????????????
          nOF_W_B -= 1;
          nOF_N_B -= 2;
          nOF_N_S -= 2;
          nOF_W_S -= 2;
          }
        else {
          nOF_W_B -= 2;
          nOF_N_B -= 3;
          nOF_N_S -= 3;
          nOF_W_S -= 2;
          }
         }
      else flgAllT = 1;
      if(flgAllT) flgAll = flgAllT;
      if(i == len1) {
         if((countBar > 0) && (countBar < DISTR_SIZE)) {
                distrB_bl[countBar]++;
            }
          count++;
          }
      conf1 = 0;
      ret = distrAn((int  *)distrB_bl,(int  *)distrB_wh,
                 nOF_W_B, nOF_N_B,
                 nOF_N_S, nOF_W_S,
                 (int  *)&vaLue,(int  *)&ind,
                 (int  *)&vaLueBl,(int  *)&indBl,(int  *)&difBl,
                 (int  *)&vaLueWh,(int  *)&indWh,(int  *)&difWh);
      if(ret && (ind >= 0)) {
           binCode(hist, i+1, (int  *)tabBarC, indHistB,
                   srHist, ind, 1);
           for(kk=0;kk<nOfChT;kk++) {
                  iFst = deCodeOneBarI25_1((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk] += 2;
                     conf1 += 2;
                     }
                  iFst = deCodeOneBarI25_2((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk+1]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk+1] += 2;
                     conf1 += 2;
                     }
                  }
           if(vaLue < minValue) {
             minValue   = vaLue;
             tHBest     = ind;
             srHistBest = srHist;
             indH       = i;
             cntSt      = count;
             moveTab(1,0,curInd);
//             if(vaLue == 0) break;
             }
           }
      if(indBl >= 0) {
           binCode(hist, i+1, (int  *)tabBarC, indHistB,
                   srHist, indBl, 1);
           for(kk=0;kk<nOfChT;kk++) {
                  iFst = deCodeOneBarI25_1((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk]++;
                     conf1++;
                     }
                  iFst = deCodeOneBarI25_2((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk+1]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk+1]++;
                     conf1++;
                     }
                  }
           if(difBl < minDifBl) {
             minValueBl   = vaLueBl;
             tHBestBl     = indBl;
             srHistBestBl = srHist;
             minDifBl     = difBl;
             indHBl       = i;
             cntBl        = count;
             moveTab(2,0,curInd);
             }
           else {
             if((difBl == minDifBl) && (vaLueBl < minValueBl)) {
               minValueBl   = vaLueBl;
               tHBestBl     = indBl;
               srHistBestBl = srHist;
               indHBl       = i;
               cntBl        = count;
               moveTab(2,0,curInd);
               }
             }
           }
      if(indWh >= 0) {
           binCode(hist, i+1, (int  *)tabBarC, indHistB,
                   srHist, indWh, 1);
           for(kk=0;kk<nOfChT;kk++) {
                  iFst = deCodeOneBarI25_1((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk]++;
                     conf1++;
                     }
                  iFst = deCodeOneBarI25_2((int  *)&tabBarC[kk*10],(LPBYTE)maskInfo[indChar+2*kk+1]);
                  if(iFst>=0) {
                     counterBar[iFst][2*kk+1]++;
                     conf1++;
                     }
                  }
           if(difWh < minDifWh) {
             minValueWh   = vaLueWh;
             tHBestWh     = indWh;
             srHistBestWh = srHist;
             minDifWh     = difWh;
             indHWh       = i;
             cntWh        = count;
             moveTab(3,0,curInd);
             }
           else {
             if((difWh == minDifWh) && (vaLueWh < minValueWh)) {
               minValueWh   = vaLueWh;
               tHBestWh     = indWh;
               srHistBestWh = srHist;
               indHWh       = i;
               cntWh        = count;
               moveTab(3,0,curInd);
               }
//--------------------------------------------------------------- ???
             else {
               if((difWh == 0) && (vaLueWh - minValueWh < 2) &&
                  ((srHistBestBl - srHistBestWh)/DSTEP2 > 4) &&
                  (indWh > tHBestWh)) {
                  minValueWh   = vaLueWh;
                  tHBestWh     = indWh;
                  srHistBestWh = srHist;
                  indHWh       = i;
                  cntWh        = count;
                  moveTab(3,0,curInd);
                 }
               }
//--------------------------------------------------------------- ???
             }
           }
      if(conf1 <= 0)  conf1 = 1;
      confCount += conf1;
      srHist += DSTEP2;
      } // while

//-------------------------------------------------

// binary code
  nOfChT = nOfChar;
  if(flgAll == 0) nOfChT--;
  for(iFst=0;iFst<2*nOfChT;iFst++) {
     conf1 = 0;
     kk = -1;
     for(i=0;i<CODEI25_LEN;i++) {
        if(counterBar[i][iFst] > conf1) {conf1 = counterBar[i][iFst];kk = i;}
        }
     if(kk >=0) {
       candCode1[indChar+iFst]     = tabTransI25[kk];
       candCode1[indChar+iFst + 1] = 0;
       confCode1[indChar+iFst]     = conf1;
       counterBar[kk][iFst]        = 0;
       }
     else {
       candCode1[indChar+iFst]     = -1;
       candCode1[indChar+iFst + 1] = 0;
       confCode1[indChar+iFst]     = 0;
       }
     conf2 = 0;
     kk = -1;
     for(i=0;i<CODEI25_LEN;i++) {
       if(counterBar[i][iFst] > conf2) {conf2 =counterBar[i][iFst];kk = i;}
       }

     if(kk >=0) {
       candCode2[indChar+iFst]     = tabTransI25[kk];
       candCode2[indChar+iFst + 1] = 0;
       confCode2[indChar+iFst]     = conf2;
//       counterBar[kk][iFst]        = 0;
       }
     else {
       candCode2[indChar+iFst]     = -1;
       candCode2[indChar+iFst + 1] = 0;
       confCode2[indChar+iFst]     = 0;
       }

     conf1 = confCode1[indChar+iFst];
     conf2 = confCode2[indChar+iFst];
     if(conf1 > 0)
         confCode12[indChar+iFst] = (conf2*100)/conf1;

     if(candCode1[indChar] > 0) {
        if(confCount > 0)
            confCode1[indChar+iFst] = ((conf1 - conf2 - conf2/2)*100)/confCount;
        if( confCode1[indChar+iFst] < 0) confCode1[indChar+iFst] = 0;
        }

     if(candCode2[indChar+iFst] > 0) {
         if(conf1 > 0)
           confCode2[indChar+iFst] = (confCode1[indChar+iFst]*conf2)/conf1;
         }
     }


   flgCut = 0;
   srHist = srHistBestBl;
   if(srHist > srHistBestWh)
          srHist = srHistBestWh;
   if(minValue < 1000) {
       srHist = srHistBest;
       flgCut = 1;
       }
   i     = iBeg;
   while((i<lenH)&&(i>=0)) {
        if(hist[i] > srHist)
                         break;
        i += iDelt;
        }

 if(flgCut)    {
   count = cntSt;
   binCode(hist, indH+1, (int  *)tabBarC, i,
             srHistBest, tHBest, iDelt);
   moveTab(0,1,lenUnitsHistSt);

    } // flgCut
  else {
   count = cntBl;
   if(count > cntWh)
      count = cntWh;
   binCode(hist, indHBl+1, (int  *)tabBarC1, i,
             srHistBestBl, tHBestBl, iDelt);


   binCode(hist, indHWh+1, (int  *)tabBarC2, i,
             srHistBestWh, tHBestWh, iDelt);

   lenUnitsHist = count;
   for(i=0;i<nOfUnits;i++) {
     if((i%2) == 0) {tabBarC[i] = tabBarC1[i];
                     tabUnitsHist[i] = tabUnitsHistBl[i];
                     }
     else           {tabBarC[i] = tabBarC2[i];
                     tabUnitsHist[i] = tabUnitsHistWh[i];
                     }
     }
   indH = indHWh;
   if((len1 - indH < 10) && (indHBl*2 < indHWh))
     indH = indHBl;
   if(indH <= iBeg) indH = iBeg + 10;
   } // else flgCut

  if(count < LEN_TABC)
    tabBarC[count++] = 5;
  if(count < LEN_TABC)
    tabBarC[count] = 5;

  return(indH);

} /* of distrBarI25() */

int decBarPI25(
   int      nOfChar,
   int   *indChar,
   int   *indMask
   )
{
 int i, j, flg;
 int ind, indM;
 int indCode1, indCode2;

  ind  = *indChar;
  indM = *indMask;
  indHistB = 0;
  for(i=0;i<nOfChar;i++) {
    indCode1 = deCodeOneBarI25_1((int  *)&tabBarC[indHistB],(LPBYTE)maskInfo[indM]);
    indM++;
    indCode2 = deCodeOneBarI25_2((int  *)&tabBarC[indHistB],(LPBYTE)maskInfo[indM]);
    indM++;
    if(indCode1 < 0) {
      flg = 0;
      for(j=0;j<10;j++)
        if(tabBarC[indHistB + j] == 5) flg = 1;
      if(flg) {
        tabPron[ind] = -5;
        *indMask = indM;
        *indChar = ind;
        return -1;
        }
      tabPron[ind] = -1;
      }
    else
      tabPron[ind] = tabTransI25[indCode1];
    ind++;

    if(indCode2 < 0) {
      flg = 0;
      for(j=0;j<10;j++)
        if(tabBarC[indHistB + j] == 5) flg = 1;
      if(flg) {
        tabPron[ind] = -5;
        *indMask = indM;
        *indChar = ind;
        return -1;
        }
      tabPron[ind] = -1;
      }
    else
      tabPron[ind] = tabTransI25[indCode2];
    ind++;

    indHistB += 10;
    }
  *indMask = indM;
  *indChar = ind;
  return 0;

} /* of decBarPI25() */

int deCodeOneBarI25_1(int  *tabBar, LPBYTE  maskI)
{
  int i, k, ifOk, ind, ii;

    for(i=0;i<CODEI25_LEN;i++) {
          ind = maskI[i];

//if(printFlg == 1){
//       sprintf(bufLog,"deCodeOneBarI25_1; ind= %d\n",ind);
//       saveLog( bufLog, "\n" );
//}

          if(ind == 100)
                  break;
          ifOk = 1;
          ii   = 0;
          for(k=0;k<=8;k+=2) {
//        	  if(printFlg == 1){
//                 sprintf(bufLog,"tabBar[k]= %d, codeBarI25[ind][ii]= %d, ind= %d, ii= %d\n",tabBar[k], codeBarI25[ind][ii], ind, ii);
//                saveLog( bufLog, "\n" );
//       	  }
             if(tabBar[k] != codeBarI25[ind][ii]) {
                           ifOk = 0;
                           break;
                           }
             ii++;
             }
          if(ifOk)
               return(i);
          }

    return(-1);

} /* of deCodeOneBarI25_1() */

int deCodeOneBarI25_2(int  *tabBar,LPBYTE  maskI)
{
  int i, k, ifOk, ind, ii;

    for(i=0;i<CODEI25_LEN;i++) {
          ind = maskI[i];
          if(ind == 100)
                  break;
          ifOk = 1;
          ii   = 0;
          for(k=1;k<=10;k+=2) {
             if(tabBar[k] != codeBarI25[ind][ii]) {
                           ifOk = 0;
                           break;
                           }
             ii++;
             }
          if(ifOk)
               return(i);
          }

    return(-1);

} /* of deCodeOneBarI25_2() */

int getFirstWideBar(int *tabBar, int nOfUnits){
	int i;
	if(nOfUnits < 4) return -1;
	for(i=0;i<4;i++){
		if(tabBar[i] == 1) return i-1;
	}
	return -1;
}

int shiftStartI25( int *hist, int lenH, int nOfUnits, long  srH, int maxH,
                   int iBeg, int nOfChar, int totChar, int iDelt, int indChar)
//if(shiftStartI25(hist, lenH, /* units */ 23,
//                srHist, maxHist, lenH - 1, /* char */ 2,
//                nOfCharInBar, -1,0) == 0) sTop = 5;

{
 int i, ret, flgCut;
 int len1, count;
 int nOF_W_B, nOF_N_B;
 int nOF_N_S, nOF_W_S;
 int  minValue, minValueBl, minValueWh;
 int vaLue, ind, vaLueBl, indBl, vaLueWh, indWh;
 int difBl, difWh;
 int minDifBl, minDifWh;
 int indH, indHBl, indHWh;
 long srHist;
 int cntSt, cntWh, cntBl, curInd;


   srHist       = srH;
   minValue     = 1000;
   tHBest       = 4;
   minValueBl   = 1000;
   tHBestBl     = 4;
   minValueWh   = 1000;
   tHBestWh     = 4;
   minDifBl     = 1000;
   minDifWh     = 1000;
   srHistBest   = (srH*3)/2;
   srHistBestBl = (srH*3)/2;
   srHistBestWh = (srH*3)/2;


  len1   = lenH;
  indH   = iBeg+10;
  indHBl = iBeg+10;
  indHWh = iBeg+10;
  cntSt  = 0;
  cntBl  = 0;
  cntWh  = 0;
  while(1) {
      if(srHist >  maxH -2) break;
      nOF_W_S  =   nOfChar * 2;
      if(iDelt > 0) {
        nOF_W_B  =   nOfChar * 2;
        nOF_N_B  =   nOfChar * 3 + 2;
        nOF_N_S  =   nOfChar * 3 + 2;
        }
      else {
        nOF_W_B  =   nOfChar * 2 + 1;
        nOF_N_B  =   nOfChar * 3 + 1;
        nOF_N_S  =   nOfChar * 3 + 1;
        }

      for(i = 0;i < DISTR_SIZE; i++) {
              distrB_bl[i] = 0;
              distrB_wh[i] = 0;
              }

      for(i = iBeg;(i < len1) && (i >= 0); i += iDelt) {
          if(hist[i] > srHist)
                         break;
          }
      indHistB  = i;
      flgCol    = 1;
      countBar  = 0;
      flgEndBar = 0;
      count     = 0;
      curInd    = 0;


//   sprintf(bufLog,"srHist= %ld, iBeg= %d, indHistB= %d, nOfUnits= %d, len1= %d \n",srHist, iBeg, indHistB, nOfUnits, len1);
//   saveLog( bufLog, "\n" );

      for(i=indHistB;(i<len1) && (i>=0);i += iDelt) {
        if(flgCol) {
           if(hist[i] <= srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        else {
           if(hist[i] > srHist)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        if(flgEndBar) {
               if((countBar > 0) && (countBar < DISTR_SIZE)) {
                  if(flgCol)
                      distrB_bl[countBar]++;
                  else
                      distrB_wh[countBar]++;
                  }
               if(curInd < MAX_UNITS)   {
                     tabUnitsHist[curInd] = i;
                     curInd++;
                     }
               flgCol = 1 - flgCol;
               countBar  = 1;
               flgEndBar = 0;
               count++;
               }
        if(count >= nOfUnits) break;
        } // for i
/*
      sprintf(bufLog,"count= %d \n",count);
      saveLog( bufLog, "\n" );

      sprintf(bufLog,"distrB_bl, count= %d\n",count);
      saveLog( bufLog, "\n" );
      printHist(distrB_bl, count);

      sprintf(bufLog,"distrB_wh, count= %d\n",count);
      saveLog( bufLog, "\n" );
      printHist(distrB_wh, count);

      sprintf(bufLog,"nOfUnits - 5= %d, count= %d, i= %d, len1= %d\n",nOfUnits - 5, count, i, len1);
      saveLog( bufLog, "\n" );
*/
      if(nOfUnits - 5 > count) {
        if(i == len1) { // STOP
          nOF_W_B -= 1;
          nOF_N_B -= 2;
          nOF_N_S -= 2;
          nOF_W_S -= 2;
          }
        else {
          if(i == 0) {  // START
             nOF_W_B -= 2;
             nOF_N_B -= 1;
             nOF_N_S -= 1;
             nOF_W_S -= 2;
             }
          else {
             nOF_W_B -= 2;
             nOF_N_B -= 3;
             nOF_N_S -= 3;
             nOF_W_S -= 2;
             }
          }
         }
      if((i == len1) || (i == 0)) {
         if((countBar > 0) && (countBar < DISTR_SIZE)) {
                distrB_bl[countBar]++;
            }
          count++;
          }

      ret = distrAn((int  *)distrB_bl,(int  *)distrB_wh,
                 nOF_W_B, nOF_N_B,
                 nOF_N_S, nOF_W_S,
                 (int  *)&vaLue,(int  *)&ind,
                 (int  *)&vaLueBl,(int  *)&indBl,(int  *)&difBl,
                 (int  *)&vaLueWh,(int  *)&indWh,(int  *)&difWh);
/*
      sprintf(bufLog,"ret= %d, vaLue= %d, ind= %d\n",ret, vaLue, ind);
      saveLog( bufLog, "\n" );
      sprintf(bufLog,"vaLueBl= %d, indBl= %d, difBl= %d\n",vaLueBl, indBl, difBl);
      saveLog( bufLog, "\n" );
      sprintf(bufLog,"vaLueWh= %d, indWh= %d, difWh= %d\n",vaLueWh, indWh, difWh);
      saveLog( bufLog, "\n" );
*/

      if(ret && (ind >= 0)) {
           if(vaLue < minValue) {
             minValue   = vaLue;
             tHBest     = ind;
             srHistBest = srHist;
             indH       = i;
             cntSt      = count;
             moveTab(1,0,curInd);
             if(vaLue == 0) break;
             }
           }
      if(indBl >= 0) {
           if(difBl < minDifBl) {
             minValueBl   = vaLueBl;
             tHBestBl     = indBl;
             srHistBestBl = srHist;
             minDifBl     = difBl;
             indHBl       = i;
             cntBl        = count;
             moveTab(2,0,curInd);
             }
           else {
             if((difBl == minDifBl) && (vaLueBl < minValueBl)) {
               minValueBl   = vaLueBl;
               tHBestBl     = indBl;
               srHistBestBl = srHist;
               indHBl       = i;
               cntBl        = count;
               moveTab(2,0,curInd);
               }
             }
           }
      if(indWh >= 0) {
           if(difWh < minDifWh) {
             minValueWh   = vaLueWh;
             tHBestWh     = indWh;
             srHistBestWh = srHist;
             minDifWh     = difWh;
             indHWh       = i;
             cntWh        = count;
             moveTab(3,0,curInd);
             }
           else {
             if((difWh == minDifWh) && (vaLueWh < minValueWh)) {
               minValueWh   = vaLueWh;
               tHBestWh     = indWh;
               srHistBestWh = srHist;
               indHWh       = i;
               cntWh        = count;
               moveTab(3,0,curInd);
               }
//--------------------------------------------------------------- ???
             else {
               if((difWh == 0) && (vaLueWh - minValueWh < 2) &&
                  ((srHistBestBl - srHistBestWh)/DSTEP2 > 4) &&
                  (indWh > tHBestWh)) {
                  minValueWh   = vaLueWh;
                  tHBestWh     = indWh;
                  srHistBestWh = srHist;
                  indHWh       = i;
                  cntWh        = count;
                  moveTab(3,0,curInd);
                 }
               }
//--------------------------------------------------------------- ???
             }
           }
      srHist += DSTEP2;
      } // while

//-------------------------------------------------

// binary code
   flgCut = 0;
   srHist = srHistBestBl;
   if(srHist > srHistBestWh)
          srHist = srHistBestWh;
   if(minValue < 1000) {
       srHist = srHistBest;
       flgCut = 1;
       }
   i     = iBeg;
   while((i<lenH)&&(i>=0)) {
        if(hist[i] > srHist)
                         break;
        i += iDelt;
        }

//  sprintf(bufLog,"srHist= %ld, flgCut= %d, i= %d\n",srHist, flgCut, i);
//   saveLog( bufLog, "\n" );


 if(flgCut)    {
   count = cntSt;

//   sprintf(bufLog,"cntSt= %ld, indH+1= %d, tHBest= %d\n",cntSt, indH+1, tHBest);
//    saveLog( bufLog, "\n" );

   binCode(hist, indH+1, (int  *)tabBarC, i,
             srHistBest, tHBest, iDelt);


//   printHist(tabBarC, indDeCode);
   moveTab(0,1,lenUnitsHistSt);

    } // flgCut
  else {
   count = cntBl;
   if(count > cntWh)
      count = cntWh;
   binCode(hist, indHBl+1, (int  *)tabBarC1, i,
             srHistBestBl, tHBestBl, iDelt);


   binCode(hist, indHWh+1, (int  *)tabBarC2, i,
             srHistBestWh, tHBestWh, iDelt);

   lenUnitsHist = count;
   for(i=0;i<nOfUnits;i++) {
     if((i%2) == 0) {tabBarC[i] = tabBarC1[i];
                     tabUnitsHist[i] = tabUnitsHistBl[i];
                     }
     else           {tabBarC[i] = tabBarC2[i];
                     tabUnitsHist[i] = tabUnitsHistWh[i];
                     }
     }
   indH = indHWh;
   if((len1 - indH < 10) && (indHBl*2 < indHWh))
     indH = indHBl;
   if(indH <= iBeg) indH = iBeg + 10;
   } // else flgCut

 if(iDelt > 0) {

//printf("tabUnitsHist[0]= %d, tabUnitsHist[1]= %d, tabUnitsHist[2]= %d, , tabUnitsHist[3]= %d\n",tabUnitsHist[0], tabUnitsHist[1], tabUnitsHist[2], tabUnitsHist[3]);
//for(i = 0;i < nOfUnits; i++) {
//printf("tabBarC[ %d ]= %d\n",i, tabBarC[i]);
//        }

   if(lenUnitsHist <= 3) return 0;
   for(i=0;i<LEN_STARTI25;i++)
      if(tabBarC[i] != startI25[i]) {
    	  int idxTmp = getFirstWideBar(tabBarC, nOfUnits);
    	  if(idxTmp>=0)
    	     return(tabUnitsHist[idxTmp]);
    	  else
    	  return 0;
      }
   return(tabUnitsHist[3]);
   }
 else {
   if(lenUnitsHist <= 2) return 0;
   for(i=0;i<LEN_STOPI25;i++)
      if(tabBarC[i] != stopI25[LEN_STOPI25 - i - 1]) return 0;
   return(tabUnitsHist[2]);
   }

} /* of shiftStartI25() */
void histTrim(
  int  *hist,
  int  *lenH
  )
{
 int k, i ,len;

  len = *lenH;
  k = 0;
  for(i=0;i<len;i++)
      if(hist[i] > 10) {
         k = i;
         break;
         }

  if(k > 0)
         k--;
  if(k > 200)
         k = 0;
  if(k > 0) {
    len -= k;
    for(i=1;i<=len;i++)
              hist[i] = hist[i + k];
    }
  for(i=len;i>100;i--)
     if(hist[i] > 10) {
        k = i;
        break;
        }
  if(k < len)
     k++;
  len  = k;
  *lenH = len;

} /* of histTrim() */
int distrAn( int  *dist_bl, int  *dist_wh, int nOF_W_B, int nOF_N_B, int nOF_N_S, int nOF_W_S,
             int  *val, int  *ind, int  *valBl, int  *indBl, int  *difBl, int  *valWh, int  *indWh, int  *difWh)
{
  int i; //, sum;
  int difMinBl, difMinWh;
  int iF, iS, iFWh, iSWh;
  int j, sumL, sumR, difL, difR;
  int end1, end2;

  end1 = 10;
  end2 = 12;
  sumL = 0;
  for(i=end2;i<DISTR_SIZE;i++) {
     if(dist_bl[i] > 0) sumL = i;
     if(dist_wh[i] > 0) sumL = i;
     }
  if(sumL > 0) end2 = sumL+1;

//  printf("sumL= %d, end2= %d\n",sumL, end2);

   *val   = 2000;
   *valBl = 2000;
   *valWh = 2000;
   *ind   = -1;
   *indBl = -1;
   *indWh = -1;

   iF    = -1;
   iFWh  = -1;
   iS    = -1;
//   sum   = 0;
   i     = 0;
//   flgB = 1;

// bars
   difMinBl = 1000;
   for(j=2;j<end1;j++) {  // DPI ???
      sumL = 0;
      for(i=0;i<=j;i++)
          sumL += dist_bl[i];
      sumR = 0;
      for(i=j+1;i<end2;i++)  // DPI ???
          sumR += dist_bl[i];
      difL = sumL - nOF_N_B;
      difR = sumR - nOF_W_B;
      if(difL < 0) difL = -difL;
      if(difR < 0) difR = -difR;
      difL += difR;
      if(difMinBl > difL) {
         difMinBl = difL;
         iF  = j;
         if(difMinBl == 0)  break;
         }
      }
    if(iF < 0) return(0);
    iS = iF;
    if(difMinBl == 0) {
       i = iF+1;
       while(1) {
         if((dist_bl[i] == 0) && (i < end2))  // DPI ???
            iS = i;
         else
            break;
         i++;
         }
       }
   *indBl = iF;
   *valBl = dist_bl[iF];
   if(dist_bl[iF] > dist_bl[iF+1])
        *valBl = dist_bl[iF+1];
   *difBl = difMinBl;

// spaces
   difMinWh = 1000;
   for(j=2;j<end1;j++) {  // DPI ???
      sumL = 0;
      for(i=0;i<=j;i++)
          sumL += dist_wh[i];
      sumR = 0;
//      for(i=j+1;i<15;i++)
      for(i=j+1;i<end2;i++)          // DPI ???
          sumR += dist_wh[i];
      difL = sumL - nOF_N_S;
      difR = sumR - nOF_W_S;
      if(difL < 0) difL = -difL;
      if(difR < 0) difR = -difR;
      difL += difR;
      if(difMinWh > difL) {
         difMinWh = difL;
         iFWh  = j;
         if(difMinWh == 0)  break;
         }
      }
    if(iFWh < 0) return(0);
    iSWh = iFWh;
    if(difMinWh == 0) {
       i = iFWh+1;
       while(1) {
         if((dist_wh[i] == 0) && (i < end2))
            iSWh = i;
         else
            break;
         i++;
         }
       }
   *indWh = iFWh;
   *valWh = dist_wh[iFWh];
   if(dist_wh[iFWh] > dist_wh[iFWh+1])
        *valWh = dist_wh[iFWh+1];
   *difWh = difMinWh;

   if((difMinBl == 0) && (difMinWh == 0)) {
     if((iF <= iSWh) && (iFWh <= iS)) {
        j = iF;
        if(j < iFWh) j = iFWh;
        *ind = j;
        *val = dist_bl[j] + dist_wh[j];
        if(dist_bl[j] + dist_wh[j] > dist_bl[j+1] + dist_wh[j+1])
           *val = dist_bl[j+1] + dist_wh[j+1];
        return 1;
        }
     }
   return 0;

}  /* of  distrAn() */
void binCode(int *hist, int iStop, int *tabC, int iBeg, long srH, int tH, int dir)
{

 int i;
    i         = iBeg;
    indDeCode = 0;
    indHistB  = iBeg;
    flgCol    = 1;
    countBar  = 0;
    flgEndBar = 0;
//    while((i<lenH)&&(i>=0)) {
//    sprintf(bufLog,"srH= %ld, tH= %d\n",srH, tH);
//    saveLog( bufLog, "\n" );

    while(1) {
        if(dir > 0) {
            if(i >= iStop ) break;
           }
        else {
           if(i <= iStop) break;
           }
        if(flgCol) {
           if(hist[i] <= srH)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        else {
           if(hist[i] > srH)
                        flgEndBar = 1;
           else
                        countBar++;
           }
        if(flgEndBar) {
               if(countBar > tH)
                            tabC[indDeCode] = 1;
               else
                            tabC[indDeCode] = 0;
               indDeCode++;
               if(indDeCode >= LEN_TABC)
                             break;
               flgCol = 1 - flgCol;
               countBar  = 1;
               flgEndBar = 0;
               }
        i += dir;
        }
     if((indDeCode < LEN_TABC - 1) && (countBar > 0)) {
       if(countBar > tH)
           tabC[indDeCode] = 1;
       else
           tabC[indDeCode] = 0;
       indDeCode++;
       }

//    if(indDeCode <180) {
//      tabC[indDeCode++] = 0;
//      tabC[indDeCode++] = 0;
//      }

 } /* of binCode() */
void moveTab(int targetTab,int sourceTab, int len)
{
 int  *ptrT = NULL;
 int  *ptrS= NULL;
 int i;

  switch(targetTab) {
   case 0: ptrT = (int  *)tabUnitsHist;
           lenUnitsHist = len;
           break;
   case 1: ptrT = (int  *)tabUnitsHistSt;
           lenUnitsHistSt = len;
           break;
   case 2: ptrT = (int  *)tabUnitsHistBl;
           lenUnitsHistBl = len;
           break;
   case 3: ptrT = (int  *)tabUnitsHistWh;
           lenUnitsHistWh = len;
           break;
   }
  switch(sourceTab) {
   case 0: ptrS = (int  *)tabUnitsHist;
           break;
   case 1: ptrS = (int  *)tabUnitsHistSt;
           break;
   case 2: ptrS = (int  *)tabUnitsHistBl;
           break;
   case 3: ptrS = (int  *)tabUnitsHistWh;
           break;
   }

 for(i=0;i<len;i++)
    ptrT[i] = ptrS[i];

} /* of moveTab() */

void setCodeBarI25(void){
	int i, j;
/*
	int  codeBarI25[CODEI25_LEN][5] = {
	       0,0,1,1,0, // 0
	       1,0,0,0,1, // 1
	       0,1,0,0,1, // 2
	       1,1,0,0,0, // 3
	       0,0,1,0,1, // 4
	       1,0,1,0,0, // 5
	       0,1,1,0,0, // 6
	       0,0,0,1,1, // 7
	       1,0,0,1,0, // 8
	       0,1,0,1,0, // 9
	       };
	*/
	for(i=0;i<CODEI25_LEN;i++)
		for(j=0;j<5;j++)
			codeBarI25[i][j] = codeBarI25Vector[i*5+j];

	}

void setCodeBarUPC(void){
	int i, j;
	for(i=0;i<CODEUPC_LEN;i++)
		for(j=0;j<4;j++)
			codeBarUPC_L[i][j] = codeBarUPCLVector[i*4+j];
	}

void printHist(int *hist, int lenH){
	int i;
	char bufLog[500];
	for(i=0;i<lenH;i++){
	    sprintf(bufLog,"hist[ %d ]= %d\n",i, hist[i]);
//	    saveLog( "", bufLog );
	}
}

void StartUPC(int *hist, int lenH, int srHist, int *sTart, int *sTop, int *withB, int *withW){
	int i, wB, wW, ccB, k1, k2;

	*sTart = 0; *sTop = 0;
	wB = 0;ccB = 0;wW = 0;
	i = 0;
	while(i<lenH && hist[i]< srHist) i++;
	k1 = i;
	while(i<lenH && hist[i] >= srHist) i++;
	k2 = i;
	wB += (k2-k1);
	ccB++;
	k1 = k2;
	while(i<lenH && hist[i] < srHist) i++;
	k2 = i;
	wW += (k2-k1);
	k1 = k2;
	while(i<lenH && hist[i] >= srHist) i++;
	k2 = i;
	wB += (k2-k1);
	ccB++;
    wB /= ccB;

    *sTart = k2;
    *withB = wB;*withW = wW;
	i = lenH-1;
	while(i>=0 && hist[i]< srHist) i--;
    *sTop = i;

//    printf("  wB= %d, wW= %d, k2= %d\n", wB, wW, k2);
}

int setNewBarsUPC_L(int *hist, int lenH, int srHist, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2;
	int iB, iW, v;
	float wB, wW;

//printf("-------------left------\n");
	iB = *idxB; iW = *idxW;
	k1 = *sTart;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//   printf("-------------wB= %e, v= %d\n", wB, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;

	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;

	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//   printf("-------------wB= %e, v= %d\n", wB, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;

	return next;
}
int setNewBarsUPC_M(int *hist, int lenH, int srHist, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2, v;
//	int iB, iW;
	float wB, wW;

//	printf("-------------M------\n");
	k1 = *sTart;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;

	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);

//	   tBarCW[iW] = (int)(wW+0.5);iW++;
//	   *idxB = iB; *idxW = iW;*sTart = i;
	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;

	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//  printf("-------------wB= %e, v= %d\n", wB, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//  printf("-------------wB= %e, v= %d\n", wB, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);
	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	return next;
}
int setNewBarsUPC_R(int *hist, int lenH, int srHist, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2;
	int iB, iW, v;
	float wB, wW;

	iB = *idxB; iW = *idxW;
	k1 = *sTart;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
	   v = (int)(wB+0.4);
	   if(v<1) v = 1;
// printf("-------------wB= %e, v= %d\n", wB, v);

	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.4);
	   if(v<1) v = 1;

//   printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
	   v = (int)(wB+0.4);
	   if(v<1) v = 1;

//  printf("-------------wB= %e, v= %d\n", wB, v);
  	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.4);
	   if(v<1) v = 1;

//   printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	return next;
}

int getNewCodeUPC(int *tBar){
    int i, k, flag;

    for(i=0;i<CODEUPC_LEN;i++) {
    	flag = 1;
        for(k=0;k<4;k++) {
          if(tBar[k] != codeBarUPC_L[i][k]) {
                 flag = 0;
                 break;
          }
      }
      if(flag == 1) return i;
    }
   return(-1);
}

int decodeUPC(int *tBarB, int *tBarW, int idxB, int idxW){
	int i, k, idx;
	int tabBar[4];
	int len = 0;
	i = 0;k = 0;

	while(i<idxB && i< idxW){
		if(tBarW[i]> 0 && tBarB[i]> 0){
		   tabBar[0] = tBarW[i];tabBar[1] = tBarB[i];i++;
		   if(i>=idxB || i>= idxW) break;
		   tabBar[2] = tBarW[i];tabBar[3] = tBarB[i];i++;
		   idx = getNewCodeUPC(tabBar);

//    printf("  tabBar[0]= %d, tabBar[1]= %d, tabBar[2]= %d, tabBar[3]= %d, idx= %d\n", tabBar[0], tabBar[1], tabBar[2], tabBar[3], idx);
          if(k == 1 || k == 13) k++;
		   if(idx >= 0){
			   if(candCode1[k] == '?')
				    candCode1[k] = tabTransUPC[idx];
			   else{
				   if(candCode1[k] != tabTransUPC[idx]) {
			           confCode1[k]++;
			           candCode1[k] = tabTransUPC[idx];
//           printf("  tabTransUPC[idx]= %c, candCode1[k]= %c, k= %d\n", tabTransUPC[idx], candCode1[k], k);
				   }
			   }
		   }
//		   if(idx < 0)
//		      candCode1[k] = '?';
//		   else
//		      candCode1[k] = tabTransUPC[idx];
		}else{
			i++; //candCode1[k] = 'M';
		}
		k++;
	}
	len = k;
//	candCode1[k] = 0;
	return len;
}
int testFinalCodeUPC(char *CodeStr, int lenC){
//	int len = strlen(CodeStr);
	int i;
	for(i=0;i<lenC;i++)
		if(CodeStr[i] == '?') return -1;
    if(lenC < 12) return -1;
	return 1;
}

int barDeCodeUPC (
    int   *hist,               // vertical histogram of barcode
    int      lenH,                // histogram length
    int      lenStr,              // number of characters for recognition
    LPSTR    strResult,           // pointer to output string
    int      *confResult         // results confidences
  ) {
 int  i, kk;
 int  maxHist;
 int len1, lenC= 0;
// int difBl, difWh;
 //int minDifBl, minDifWh;
// int iBeg, iDelt, ret;
// int lenCode, flgCut, starFlg;
// int iMask, countUnit, countUnitTot;
// int nOfNq, lenHH;
 int sTart = 0;
 int sTop  = 0;
// int nOfIter = 0;
// int flgIter = 0;
// int ccBar, ccBarOk, conf1;
 int srHistUPC, srHistGUPC;
 int withOneB, withOneW, mid;



//  printFlg = 0;

  strResult[0] = 0;
  if(lenH < MIN_LENHIST)
              return(-1);
   setCodeBarUPC();

   len1 = lenH -1;
   srHistUPC = 0;
   maxHist = 0;
   for(i=1;i<len1;i++) {
        srHistUPC += hist[i];
        if(hist[i] > maxHist)
            maxHist = hist[i];
  }


   srHistUPC /= len1;
   srHistUPC = srHistUPC/2;
//   printf("  srHistUPC= %d, maxHist= %d\n", srHistUPC, maxHist);
   if(srHistUPC < 4) return -1;
   srHistGUPC = srHistUPC;
   for(i=0;i<15;i++) {
	      candCode1[i] = '?';
	      confCode1[i] = 0;
   }
   candCode1[1] = ' ';
   candCode1[13] = ' ';
   candCode1[7] = ' ';
   candCode1[15] = 0;

//  printf("  srHistUPC= %d, maxHist= %d\n", srHistUPC, maxHist);
//   AGNBAR:
   for(kk=0;kk<20;kk++){
      srHistUPC = srHistGUPC;
      if(srHistUPC > (maxHist -maxHist/4)) break;
      sTart = 0;
      sTop  = 0;
//      nOfIter++;
//      for(ii=0;ii<MAX_UNITS;ii++)
//          tabUnitsHistSht[ii] = 0;

//      candCode2[0] = 0;
//     printf("  srHistUPC= %d\n", srHistUPC);

     StartUPC(hist, lenH, srHistUPC, &sTart, &sTop, &withOneB, &withOneW);
     {int idxB=0, idxW=0, idxC;
        if(withOneB< 2) withOneB = 2;
        if(withOneW< 2) withOneW = 2;
        if(withOneB> 4) withOneB = 4;
        if(withOneW> 4) withOneW = 4;

//        printf("-------------withOneB= %d, withOneW= %d\n", withOneB, withOneW);
        idxC = sTart;
        for(i=0;i<6;i++){
           if(setNewBarsUPC_L(hist, lenH, srHistUPC, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
        }
        mid = setNewBarsUPC_M(hist, lenH, srHistUPC, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW);
//        printf("  mid= %d\n", mid);
//        if(mid>= 0){

        	tabBarC[idxB] = -1;idxB++;
        	tabBarC1[idxW] = -1;idxW++;
            for(i=0;i<6;i++){
               if(setNewBarsUPC_R(hist, lenH, srHistUPC, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
            }
//        }
        lenC =decodeUPC(tabBarC, tabBarC1, idxB, idxW);
//        printf("-------------candCode1= %s, lenC= %d\n", candCode1, lenC );
        if(testFinalCodeUPC(candCode1, lenC)>=0 || lenC < 4) break;
     }
     srHistGUPC += 4;
   }
   if(strlen(candCode1) > lenC && lenC > 11) candCode1[lenC] = 0;
   {
	  int flgCnf = 0;
      for(i=0;i<strlen(candCode1);i++) {
    	  if(confCode1[i] > 0) flgCnf++;
      }
//      printf("last-------------candCode1= %s, flgCnf= %d\n", candCode1, flgCnf );
//      if(flgCnf > 0) {
//         for(i=0;i<strlen(candCode1);i++) {
//             printf("-------------candCode1[ %d ]= %c, cnf[ %d ]= %d\n", i, candCode1[i], i, confCode1[i]);
//         }
//      }
   }
   if(strlen(candCode1)< lenStr) candCode1[lenStr] = 0;
   strcpy(strResult, candCode1);
   for(i=0;i<strlen(candCode1);i++) {
	  confCode1[i] = 100 - confCode1[i]*50;
	  if(confCode1[i] < 0) confCode1[i] = 0;
 	  confResult[i] = confCode1[i];
   }
   return 0;
}


void StartUPC2(int *hist, int lenH, int *sTart, int *sTop, int *withB, int *withW){
	int i, wB, wW, ccB, k1, k2;
	int srHist = 1;

	*sTart = 0; *sTop = 0;
	wB = 0;ccB = 0;wW = 0;
	i = 0;
	while(i<lenH && hist[i]< srHist) i++;
	k1 = i;
	while(i<lenH && hist[i] >= srHist) i++;
	k2 = i;
	wB += (k2-k1);
	ccB++;
	k1 = k2;
	while(i<lenH && hist[i] < srHist) i++;
	k2 = i;
	wW += (k2-k1);
	k1 = k2;
	while(i<lenH && hist[i] >= srHist) i++;
	k2 = i;
	wB += (k2-k1);
	ccB++;
    wB = (int)(((float)wB/(float)ccB)+0.501);

//    if(wB < 2) wB = 2;
    *sTart = k2;
    *withB = wB;*withW = wW;
	i = lenH-1;
	while(i>=0 && hist[i]< srHist) i--;
    *sTop = i;

//    printf("  wB= %d, wW= %d, k2= %d\n", wB, wW, k2);
}
int setNewBarsUPC_L2(int *hist, int lenH, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2;
	int iB, iW, v;
	int iBst, iWst, starTst;
	float wB, wW;
	int srHist = 1;

//printf("-------------left------, *sTart= %d\n", *sTart);
//printf("-------------withOneB=%d withOneW= %d\n", withOneB, withOneW);
   iBst = *idxB; iWst = *idxW;
   starTst = *sTart;

	iB = iBst; iW = iWst;
	k1 = starTst;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
//	   v = (int)(wW+0.5);
	   v = (int)wW;
	   if(wW > v) v++;
	   if(v<1) v = 1;
// printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCW[iW] = v;iW++;

	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
//	   v = (int)(wB+0.5);
	   v = (int)wB;
	   if(wB > v) v++;

	   if(v<1) v = 1;
//   printf("-------------wB= %e, v= %d\n", wB, v);
	   tBarCB[iB] = v;iB++;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
//	   v = (int)(wW+0.5);
	   v = (int)wW;
	   if(wW > v) v++;
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCW[iW] = v;iW++;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
//	   v = (int)(wB+0.5);
	   v = (int)wB;
	   if(wB > v) v++;
	   if(v<1) v = 1;
//   printf("-------------wB= %e, v= %d\n", wB, v);
	   tBarCB[iB] = v;iB++;

	} else next = -1;

    *idxB = iB; *idxW = iW;*sTart = i;
   	return next;
}
int setNewBarsUPC_M2(int *hist, int lenH, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2, v;
//	int iB, iW;
	float wB, wW;
	int srHist = 1;

//	printf("-------------M------\n");
	k1 = *sTart;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;

	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);

//	   tBarCW[iW] = (int)(wW+0.5);iW++;
//	   *idxB = iB; *idxW = iW;*sTart = i;
	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;

	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//  printf("-------------wB= %e, v= %d\n", wB, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
	   v = (int)(wB+0.5);
	   if(v<1) v = 1;
//  printf("-------------wB= %e, v= %d\n", wB, v);

	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
	   v = (int)(wW+0.5);
	   if(v<1) v = 1;
//  printf("-------------wW= %e, v= %d\n", wW, v);
	   *sTart = i;
	   if(v != 1) next = -1;
	} else next = -1;

	return next;
}
int setNewBarsUPC_R2(int *hist, int lenH, int *sTart, int sTop, int withOneB, int withOneW, int *tBarCB, int *tBarCW, int *idxB, int *idxW){
	int next = 1;
	int i, k1, k2;
	int iB, iW, v;
	float wB, wW;
	int srHist = 1;

	iB = *idxB; iW = *idxW;
	k1 = *sTart;
	i = k1;
	if(i<sTop) {
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
//	   v = (int)(wB+0.4);
	   v = (int)wB;
	   if(wB > v) v++;

	   if(v<1) v = 1;
// printf("-------------wB= %e, v= %d\n", wB, v);

	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
//	   v = (int)(wW+0.4);
	   v = (int)wW;
	   if(wW > v) v++;

	   if(v<1) v = 1;

//   printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
// -----
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i] >= srHist) i++;
	   k2 = i;
	   wB = (float)(k2-k1)/(float)withOneB;
//	   v = (int)(wB+0.4);

	   v = (int)wB;
	   if(wB > v) v++;

	   if(v<1) v = 1;

//  printf("-------------wB= %e, v= %d\n", wB, v);
  	   tBarCW[iW] = v;iW++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	if(i<sTop) {
	   k1 = k2;
	   while(i<lenH && hist[i]< srHist) i++;
	   k2 = i;
	   wW = (float)(k2-k1)/(float)withOneW;
//	   v = (int)(wW+0.4);
	   v = (int)wW;
	   if(wW > v) v++;

	   if(v<1) v = 1;

//   printf("-------------wW= %e, v= %d\n", wW, v);
	   tBarCB[iB] = v;iB++;
	   *idxB = iB; *idxW = iW;*sTart = i;
	} else next = -1;
	return next;
}
void recalculateBarWithUPC(int *hist, int lenH, int sTart, int sTop, int *withB, int *withW){
	int i, wB, wW, k1, k2;
	int srHist = 1;

	wB = *withB;wW = *withW;
	i = sTop;
	k1 = i;
	while(i>=0 && hist[i] >= srHist) i--;
	k2 = i;
	wB += (k1-k2);
	k1 = k2;
	while(i>=0 && hist[i] < srHist) i--;
	k2 = i;
	wW += (k1-k2);
	k1 = k2;
	while(i>=0 && hist[i] >= srHist) i--;
	k2 = i;
	wB += (k1-k2);
	wB = (int)(((float)wB/(float)3)+0.501);
	wW = (int)(((float)wW/(float)2)+0.501);
	if(wB < 2) wB = 2;
	if(wW < 2) wW = 2;
    *withB = wB;*withW = wW;
}
int decodeUPC2(int *tBarB, int *tBarW, int idxB, int idxW){
	int i, k, idx;
	int tabBar[4];
	int len = 0;
	i = 0;k = 0;

	while(i<idxB && i< idxW){
		if(tBarW[i]> 0 && tBarB[i]> 0){
		   tabBar[0] = tBarW[i];tabBar[1] = tBarB[i];i++;
		   if(i>=idxB || i>= idxW) break;
		   tabBar[2] = tBarW[i];tabBar[3] = tBarB[i];i++;
		   idx = getNewCodeUPC(tabBar);

//    printf("  i= %d, k= %d, tabBar[0]= %d, tabBar[1]= %d, tabBar[2]= %d, tabBar[3]= %d, idx= %d\n", i, k, tabBar[0], tabBar[1], tabBar[2], tabBar[3], idx);
          if(k == 1 || k == 13) k++;
		   if(idx >= 0){
			   if(candCode1[k] == '?')
				    candCode1[k] = tabTransUPC[idx];
			   else{
				   if(candCode1[k] != tabTransUPC[idx]) {
			           confCode1[k]++;
//			           candCode1[k] = tabTransUPC[idx];
//           printf("  tabTransUPC[idx]= %c, candCode1[k]= %c, k= %d\n", tabTransUPC[idx], candCode1[k], k);
				   }
			   }
		   }
//		   if(idx < 0)
//		      candCode1[k] = '?';
//		   else
//		      candCode1[k] = tabTransUPC[idx];
		}else{
			i++; //candCode1[k] = 'M';
		}
		k++;
	}
	len = k;
//	candCode1[k] = 0;
	return len;
}

/*
int  codeBarUPCLVector[CODEUPC_LEN*4] = {
       3,2,1,1, // 0
       2,2,2,1, // 1
       2,1,2,2, // 2
       1,4,1,1, // 3
       1,1,3,2, // 4
       1,2,3,1, // 5
       1,1,1,4, // 6
       1,3,1,2, // 7
       1,2,1,3, // 8
       3,1,1,2, // 9
       };
*/
int barDeCodeUPC2 (
    int   *hist,               // vertical histogram of barcode
    int      lenH,                // histogram length
    int      lenStr,              // number of characters for recognition
    LPSTR    strResult,           // pointer to output string
    int      *confResult         // results confidences
  ) {
 int  i;
 int sTart = 0;
 int sTop  = 0;
 int withOneB, withOneW, mid, lenC;

/*
	   {int nnH;
	     char bufLog[500];
	    sprintf(bufLog,"--------------------------------------------\n\n");
	    saveLog("", bufLog);
	      for(nnH=0;nnH<=lenH;nnH++){
	   	   sprintf(bufLog,"vHist[ %d ]= %d\n",nnH, hist[nnH]);
	   	   saveLog("", bufLog);
	      }
	   }
*/
//	   strResult[0] = 0;
	   if(lenH < MIN_LENHIST)
	               return(-1);

   setCodeBarUPC();

   if(strlen(strResult) > 0) {
       strcpy(candCode1, strResult);
       for(i=0;i<strlen(strResult);i++) {
  	      confCode1[i] = 0;
       }
   }
   else{
     for(i=0;i<15;i++) {
	      candCode1[i] = '?';
	      confCode1[i] = 0;
     }
     candCode1[1] = ' ';
     candCode1[13] = ' ';
     candCode1[7] = ' ';
     candCode1[15] = 0;
   }

// printf("start-------------candCode1= %s\n", candCode1);

   StartUPC2(hist, lenH, &sTart, &sTop, &withOneB, &withOneW);
   {int idxB=0, idxW=0, idxC;
      idxC = sTart;
      for(i=0;i<6;i++){
         if(setNewBarsUPC_L2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
      }
      mid = setNewBarsUPC_M2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW);
//        printf("  mid= %d\n", mid);
//        if(mid>= 0){

   	 tabBarC[idxB] = -1;idxB++;
     tabBarC1[idxW] = -1;idxW++;
     for(i=0;i<6;i++){
         if(setNewBarsUPC_R2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
     }
//     printf("1  idxB= %d, idxW= %d\n", idxB, idxW);
      lenC =decodeUPC2(tabBarC, tabBarC1, idxB, idxW);
//      printf("1-------------candCode1= %s, lenC= %d\n", candCode1, lenC );


      if(testFinalCodeUPC(candCode1, lenC) < 0 || lenC < 4) {
    	   idxB=0; idxW=0;
    	   recalculateBarWithUPC(hist, lenH, sTart, sTop, &withOneB, &withOneW);
    	      idxC = sTart;
    	      for(i=0;i<6;i++){
    	         if(setNewBarsUPC_L2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
    	      }
    	      mid = setNewBarsUPC_M2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW);
    	   	 tabBarC[idxB] = -1;idxB++;
    	     tabBarC1[idxW] = -1;idxW++;
    	     for(i=0;i<6;i++){
    	         if(setNewBarsUPC_R2(hist, lenH, &idxC, sTop, withOneB, withOneW, tabBarC, tabBarC1, &idxB, &idxW) < 0) break;
    	     }
//    	     printf("2  idxB= %d, idxW= %d\n", idxB, idxW);
    	     lenC = decodeUPC2(tabBarC, tabBarC1, idxB, idxW);
//    	      printf("2-------------candCode1= %s, lenC= %d\n", candCode1, lenC );
      }
   }

   strcpy(strResult, candCode1);
   for(i=0;i<strlen(candCode1);i++) {
//	  confCode1[i] = 100 - confCode1[i]*50;
//	  if(confCode1[i] < 0) confCode1[i] = 0;
 	  confResult[i] += confCode1[i];
   }



 return 0;
}
