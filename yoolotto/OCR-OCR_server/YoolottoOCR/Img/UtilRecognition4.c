/*
 * UtilRecognition4.c
 *
 *  Created on: Sep 6, 2012
 *      Author: Wicek
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include  "../headers/UtilRecognition.h"
//void saveLog(char *str1, char *str2);
//void printTab(int *pMH, int n, char *str);

int getFirstHistEnd(int *hntHist);
int getNextHistEnd(int *hntHist, int beg);
void cleanCuts(IMAGEL *pImage, RECCHARSTRUCT * pRecCharStruct, int  *hist, int lenH, int sizeW);
//void cleanHistogram(int *hntHist, int thX, int thY);
int checkConfidence(STRUCTWB * lpWorkBuffer, IMAGEL *pImage, RECCHARSTRUCT * pRecCharStruct, int begHOtp,
		            int *hist, int hLen, int *markC, int indxM,  int *countG, int *countAll);
void replaceMarks(void);
extern int US_State;

void  smHist2(int tH)
{
 int i;

    for(i=2;i<hntHist[0];i++) {
         if((hntHist[i]< hntHist[i-1]) || (hntHist[i]< hntHist[i-1]))
                hntHist[i] = (hntHist[i-1] + 2*hntHist[i] + hntHist[i+1])/4;
         }

} /* of smHist2() */

int isLineValid(STRUCTWB *lpWorkBuffer, int wCharType){
 int  i,tH;
       tH  = 8;
       if(wCharType < 4) {
              tH = 15;
              }
       tH = (int)((float)tH*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0)); // ?????????????????????????????????????
       tH = max(tH,(lpWorkBuffer-> maxLH));
       for(i=0;i<(lpWorkBuffer->frNumber);i++)  {
          if( (lpWorkBuffer->curLine[i]).y2 - (lpWorkBuffer->curLine[i]).y1 > tH ) {
                return(1);
                }
          }
       return(0);

 }

/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :get next un-proportional font                    *
 *                                                                         *
 ***************************************************************************/

int sendNextFont( STRUCTWB * lpWorkBuffer, char *fieldName){
  int x1, x2, yT, yB, i, y, x, tHm, rm, nSp;
  int lenT;
  POINT pP1, pP2;
  int   dX = 0;
  float alpH;
  long scAngleHH;

//  char buff[500];

         scAngleHH = SCALEAN;
         nSp=0;
         if((lpWorkBuffer ->indUnpSp)) {(lpWorkBuffer ->indUnpSp)=0;return(1);}
AGN:;
         if((lpWorkBuffer ->indUnpB)>=(lpWorkBuffer ->indUnpE)) {
                if(nSp>0) {return(1000+nSp);}
                return(3);
                }

         yT=min(lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].lT.y,lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].rT.y);
         yB=max(lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].lB.y,lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].rB.y);

         x1=lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].lT.x;
         x2=lpWorkBuffer->arrCellP[(lpWorkBuffer ->indUnpB)].rT.x;
         (lpWorkBuffer ->traceL[0])=yT;

         if(lpWorkBuffer->flgItalic) {
           alpH = (float)(lpWorkBuffer->angH)*(float)PI/((float)180.0*(float)scAngleHH);
           if(alpH < 0) {alpH = -alpH;}
           if((scAngleHH >0) && (alpH < (float)0.785)) {
              dX =(int)((float)(yB -yT)*tan(alpH)+(float)0.5);
              }
           pP1.y = yT;
           pP1.x = x1 + dX;
           if(pP1.x >= (lpWorkBuffer -> begRect.x) + (lpWorkBuffer -> Width)) {
                pP1.x = (lpWorkBuffer -> begRect.x) + (lpWorkBuffer -> Width) - 1;
                }

           pP2.y = yB;
           pP2.x = x1;
           lenT = skewTraces((int *)&(lpWorkBuffer ->traceL[1]), pP1, pP2);

           (lpWorkBuffer ->lengthTraceL)=lenT;
           (lpWorkBuffer ->traceC[0])=yT;

           pP1.x = x2 + dX;
           if(pP1.x >= (lpWorkBuffer -> begRect.x) + (lpWorkBuffer -> Width)) {
                pP1.x = (lpWorkBuffer -> begRect.x) + (lpWorkBuffer -> Width) - 1;
                }

           pP2.x = x2;
           lenT = skewTraces((int *)&(lpWorkBuffer ->traceC[1]),pP1,pP2);

           (lpWorkBuffer ->lengthTraceC)=lenT;

           for(i=1;i<=lenT;i++) {
                (lpWorkBuffer ->traceL[i])++;
//                (lpWorkBuffer ->traceC[i])=x2;
                }
           }
         else {
           lenT = min(yB-yT+1,MAXLTRACE-1);
           (lpWorkBuffer ->lengthTraceL)=lenT;
           (lpWorkBuffer ->traceC[0])=yT;
           (lpWorkBuffer ->lengthTraceC)=lenT;

//         i=1;
           for(i=1;i<=lenT;i++) {
                (lpWorkBuffer ->traceL[i])=x1;
                (lpWorkBuffer ->traceC[i])=x2;
//                i++;
                }
           }
         (lpWorkBuffer ->indUnpB)++;
         tHm=12;

         tHm=(int)((float)tHm*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));


         rm=0;
         i=1;
         while((i<=(lpWorkBuffer ->lengthTraceL))&&(rm<tHm)) {
            y=(lpWorkBuffer->traceL[0])+i-1;
            for(x=(lpWorkBuffer->traceL[i]);x<=(lpWorkBuffer->traceC[i]);x++)  {
                if(whatIsThis(lpWorkBuffer,x,y)) {
                   rm++;
                   }
                }
            i++;
            }


//          if(strcmp(fieldName, "NUMBERS") == 0) {
//      	        printf("------------ lpWorkBuffer->traceL[1]= %d, lpWorkBuffer->traceC[1]= %d \n", lpWorkBuffer->traceL[1], lpWorkBuffer->traceC[1]);
//        	    printf("------------ tHm= %d, rm= %d, nSp = %d, x2-x1= %d \n", tHm, rm, nSp, lpWorkBuffer->traceC[1]-lpWorkBuffer->traceL[1]);
//         }

        if(rm<tHm) { nSp++;
//        if(strcmp(fieldName, "NUMBERS") == 0) {
//      	    printf("------------ nSp = %d \n", nSp);
//       }
        goto AGN;}
        if(nSp>0) {
                (lpWorkBuffer ->indUnpSp)=1;
                return(1000+nSp);
                }
        return(1);
  } /* of sendNextFont */

int skewTraces(int *Trace, POINT pP1, POINT pP2){
 int  xx, yy, indT;
 int  dif = 0, difX, difY, abX, abY, kindL, isX;
 int  aIncr = 0, bIncr = 0, yIncr, xIncr;
 int  start;

/* BbresenLine */

    difX=pP2.x-pP1.x;
    difY=pP2.y-pP1.y;

    kindL = 0;
    indT  = 0;

      if(difX==0)  {
        if(difY==0) return(LINE_ERR1);
        kindL=1;
        }
      else {
        if(difY==0) kindL=2;
        }

      if(difX<0) {
            abX=-difX;
            }
       else {
            abX=difX;
            }
       if(difY<0) {
            abY=-difY;
            }
       else {
            abY=difY;
            }
      if((abX>32000)||(abY>32000)) return(LINE_ERR2);

      isX=0;
      if(abX>abY) isX=1;
      if(!kindL) {
        switch(isX)  {
          case 0:

                if(difY<0)  {
                        yIncr = -1;
                        if(difX<0) {
                                aIncr = 2*(difX-difY);
                                bIncr = 2*difX;
                                dif = 2*difX-difY;
                                xIncr = -1;
                                }
                        else    {
                                aIncr = 2*(-difX-difY);
                                bIncr = -2*difX;
                                dif = -2*difX+difY;
                                xIncr = 1;
                                }
                        }
                else {
                        yIncr = 1;
                        if(difX<0) {
                                aIncr = 2*(-difX-difY);
                                bIncr = -2*difX;
                                dif = -2*difX-difY;
                                xIncr = -1;
                                }
                        else    {
                                aIncr = 2*(difX-difY);
                                bIncr = 2*difX;
                                dif = 2*difX-difY;
                                xIncr = 1;
                                }
                        }

                break;
          case 1:
                if(difX<0)  {
                        xIncr = -1;
                        if(difY<0) {
                                aIncr = 2*(difY-difX);
                                bIncr = 2*difY;
                                dif = 2*difY-difX;
                                yIncr = -1;
                                }
                        else    {
                                aIncr = 2*(-difY-difX);
                                bIncr = -2*difY;
                                dif = -2*difY+difX;
                                yIncr = 1;
                                }
                        }
                else {
                        xIncr = 1;
                        if(difY<0) {
                                aIncr = 2*(-difY-difX);
                                bIncr = -2*difY;
                                dif = -2*difY-difX;
                                yIncr = -1;
                                }
                        else    {
                                aIncr = 2*(difY-difX);
                                bIncr = 2*difY;
                                dif = 2*difY-difX;
                                yIncr = 1;
                                }
                        }

                break;
          }
        }
       else {
          yIncr = 1;
          xIncr = 1;
          if(difY<0)  {
            yIncr = -1;
            }
          if(difX<0)  {
            xIncr = -1;
            }
        }

       xx=pP1.x;
       yy=pP1.y;

        Trace[indT] = xx;
        indT++;

      switch(kindL) {
        case 0:
                switch(isX) {
                        case 0:
                                start=pP1.y+yIncr;
                                if(yIncr>0) {
                                   for(yy=start;yy<=pP2.y;yy++) {
                                        if(dif>=0) {
                                            xx+=xIncr;
                                            dif+=aIncr;
                                            }
                                        else {
                                            dif+=bIncr;
                                            }
                                          Trace[indT] = xx;
                                          indT++;

                                        }
                                   }
                               else {
                                   for(yy=start;yy>=pP2.y;yy--) {
                                        if(dif<=0) {
                                            xx+=xIncr;
                                            dif+=aIncr;
                                            }
                                        else {
                                            dif+=bIncr;
                                            }
                                          Trace[indT] = xx;
                                          indT++;


                                        }
                                   }
                                break;
                        case 1:
                                start=pP1.x+xIncr;
                                if(xIncr>0) {
                                   for(xx=start;xx<=pP2.x;xx++) {
                                        if(dif>=0) {
                                            yy+=yIncr;
                                            dif+=aIncr;
                                            }
                                        else {
                                            dif+=bIncr;
                                            }
                                          Trace[indT] = xx;
                                          indT++;

                                        }
                                    }
                                else {
                                   for(xx=start;xx>=pP2.x;xx--) {
                                        if(dif<=0) {
                                            yy+=yIncr;
                                            dif+=aIncr;
                                            }
                                         else {
                                            dif+=bIncr;
                                            }

                                          Trace[indT] = xx;
                                        indT++;
                                        }

                                    }

                                break;
                        }

                break;
        case 1:
                start=pP1.y+yIncr;
                if(yIncr>0) {
                  for(yy=start;yy<=pP2.y;yy++) {
                      Trace[indT] = xx;
                      indT++;
                     }
                  }
                else {
                  for(yy=start;yy>=pP2.y;yy--) {
                      Trace[indT] = xx;
                      indT++;
                     }
                  }
                break;
        case 2:
                start=pP1.x+xIncr;
                if(xIncr>0) {
                   for(xx=start;xx<=pP2.x;xx++) {
                      Trace[indT] = xx;
                      indT++;
                     }
                   }
                else {
                   for(xx=start;xx>=pP2.x;xx--) {
                      Trace[indT] = xx;
                      indT++;
                     }
                   }
                break;
        }
   return(indT);
 } /* skewTraces() */

void setEmptyBuff(LPBYTE lpExtractCh, int width, int height){
int width_bt;
      width_bt  =((width+31)/32)*4;
     memset(lpExtractCh,0xFF,width_bt*height);

} /* of setEmptyBuff() */
int strongMassTest(STRUCTWB *lpWorkBuffer, int tHm)
{
  int i, x, y, rm;

   rm=0;
   i=1;
   while(i<=(lpWorkBuffer ->lengthTraceL)) {
       y=(lpWorkBuffer->traceL[0])+i-1;
       for(x=(lpWorkBuffer->traceL[i]);x<=(lpWorkBuffer->traceC[i]);x++)  {
           if(whatIsThis(lpWorkBuffer,x,y))
             rm++;
           }
       i++;
       if(rm > tHm) return(0);
       }

   return(1);

} /* of strongMassTest() */


/***************************************************************************
 *                                                                         *
 *  PURPOSE      :send extracted character to the buffer                   *
 *                                                                         *
 *  RETURN VALUE : 1 - if whole character sent                             *
 *                 0 - if character was cut                                *
 *                                                                         *
 ***************************************************************************/
int sendToBuffer(STRUCTWB *lpWorkBuffer, LPBYTE lpExtractCh, int width, int height, int charIndex)
   {
     int   x_start, x_end, length, i, j, i_y, width_bt, x_sides, y_sides;
     int   top_offset, left_offset, right_b, left_b, min, max, minR;
     int   yIn, xIn, xMin, xMax, yMin, yMax, cTop, cMid, cBot, y1, y2;
     int   flgBlack, tHm;
     float flHeight, flShape, flWidth;
     int   tHm1, tHm2, tHm3;
     int   yy1, yy2;
     int   smallSignYesNo;
     int nof_seg, mass, ret, kk;
     int ratioA, curArea;
     int avArea;
     BYTE mask;
        ret = 0;
        mass = 0;
        kk=0;

       width_bt  =((width+31)/32)*4;
        min = min_left(lpWorkBuffer);
        max = max_current(lpWorkBuffer);
        length=max-min+1;
        nof_seg = whichFrame(lpWorkBuffer,(max+min)/2);

if(((lpWorkBuffer->txtLineVer == 1) || (lpWorkBuffer->txtLineVer == 2) ||
    (lpWorkBuffer->txtLineVer == 11)) && (lpWorkBuffer -> segmType != 55)
    && (lpWorkBuffer -> txtLineVer != 65)) {

    yMin = lpWorkBuffer->traceL[0];
    yMax = yMin + lpWorkBuffer->lengthTraceL;
    y1=(lpWorkBuffer->curLine[nof_seg]).y1;
    y2=(lpWorkBuffer->curLine[nof_seg]).y2;
    yy1 = (lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
    yy2 = (lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH)+
          (lpWorkBuffer -> Height)-1;
    tHm  = (max - min)/5;
    tHm3 = (y2 - y1)/4 + 2;
    if(tHm3 < 2) tHm3 = 2;

//top
    if(yMin - tHm3 <= yy1) tHm3 = yMin - 1 - yy1;
    if(yMax + tHm3 >= yy2) tHm3 = yy2 - yMax - 1;
    cMid = ((tHm3 - 2)*4)/5;
    x_start = yMin - tHm3;
    x_end = yMax + tHm3;
    hist_h(lpWorkBuffer, min, x_start, max, x_end);

    yIn  = tHm3 + 1;
    xIn  = hntHist[0] - tHm3;
    ret  = hntHist[0]/2;
    mass = hntHist[0] - tHm3 - 1;
    if(mass < ret) mass = ret;

//  hntHist
    if(hntHist[yIn] > 0) {
       cTop     = 0;
       cBot     = yIn + tHm3;
       for(i=yIn;i<ret;i++) {
           if(hntHist[i] > tHm) cTop++;
           if(hntHist[i] == 0) {cBot = i-1;break;}
           }
       if(cBot < yIn + tHm3) { // shift to the bottom
          kk = 0;
          cBot++;
          for(i=cBot;i<mass;i++) {
              if(hntHist[i] > 0) break;
              kk++;
              }
            if((kk > cMid) || ((cTop > 0) && (kk > 1))) {
                 cBot -= (yIn - 1);
                 kk += (cBot - 2);
                 if(kk < 0) kk = 0;
                 lpWorkBuffer->lengthTraceL -= kk;
                 lpWorkBuffer->traceL[0]    += kk;
                 lpWorkBuffer->lengthTraceC -= kk;
                 lpWorkBuffer->traceC[0]    += kk;
                 }
             else {
                 x_end = yIn - tHm3/2;
                 if(x_end < 0) x_end = 0;
                 shiftTopBordA(lpWorkBuffer,yIn,x_end,(int *)hntHist);
                }
           }
       else {  // shift to the top
           shiftTopBordA(lpWorkBuffer,yIn,0,(int *)hntHist);
          }
       }
    else {
       kk = 1;
       for(i=yIn;i<mass;i++)
           if(hntHist[i] > 0) {kk = i;break;}
       kk -= yIn;
//       if(kk > tHm3) {
       if(kk > 1) {
          kk--;
          if(kk > 0) {
               lpWorkBuffer->lengthTraceL -= kk;
               lpWorkBuffer->traceL[0]    += kk;
               lpWorkBuffer->lengthTraceC -= kk;
               lpWorkBuffer->traceC[0]    += kk;
               }
          }
       }
//bottom
    i_y = xIn;
    if(hntHist[i_y] > 0) {
       cTop = i_y - tHm3;
       if(cTop < 1) cTop = 1;
       kk = 1;
       tHm1 = 0;
       for(i=i_y;i>=cTop;i--)
           if(hntHist[i] == 0) {
               if(i > ret) tHm1 = 1;
               cTop = hntHist[0];
               for(j=i_y;j<=cTop;j++)
                   if(hntHist[j] == 0) {kk = 0;break;}
                 if(!kk) break;
                 kk = i_y - i;
                 lpWorkBuffer->lengthTraceL -= kk;
                 lpWorkBuffer->lengthTraceC -= kk;
                 kk = 0;
                 tHm1 = 0;
                 break;
                 }
       if(!kk && tHm1 && (j - i_y <= 4)) {
            kk = 1;
            for(i=j;i<=cTop;i++)
              if(hntHist[i] != 0) {kk = 0;break;}
           }
       if(kk) {
          cTop = hntHist[0];
          cBot = i_y;
          for(i=i_y;i<=cTop;i++)
              if(hntHist[i] == 0) {cBot = i;break;}
          kk = cBot - i_y;
          if(kk > 0) {
             cBot = lpWorkBuffer->traceL[lpWorkBuffer->lengthTraceL];
             for(i=1;i<=kk;i++) {
               i_y = lpWorkBuffer->lengthTraceL + i;
               if(i_y < MAXLTRACE)                        // 08-14-95
                      lpWorkBuffer->traceL[i_y] = cBot;
               }
             cBot = lpWorkBuffer->traceC[lpWorkBuffer->lengthTraceC];
             for(i=1;i<=kk;i++) {
               i_y = lpWorkBuffer->lengthTraceC + i;
               if(i_y < MAXLTRACE)                       // 08-14-95
                  lpWorkBuffer->traceC[i_y] = cBot;
               }
             lpWorkBuffer->lengthTraceL += kk;
             lpWorkBuffer->lengthTraceC += kk;
             }
          }
       }
     else { // cut empty bottom
       kk = i_y;
       for(i=i_y;i>0;i--)
           if(hntHist[i] > 0) {kk = i;break;}
       kk = i_y - i;
//       if(kk > tHm3) {
       if(kk > 1) {
//          kk -= 2;
          kk--;
          if(kk > 0) {
               lpWorkBuffer->lengthTraceL -= kk;
               lpWorkBuffer->lengthTraceC -= kk;
               }
          }
       }

   if(lpWorkBuffer->traceL[0] <= yy1)  lpWorkBuffer->traceL[0] = yy1 + 1;
   if(lpWorkBuffer->traceC[0] <= yy1)  lpWorkBuffer->traceC[0] = yy1 + 1;
   if(lpWorkBuffer->traceL[0] + lpWorkBuffer->lengthTraceL -1 >= yy2)
            lpWorkBuffer->lengthTraceL = yy2 -  lpWorkBuffer->traceL[0];
   if(lpWorkBuffer->traceC[0] + lpWorkBuffer->lengthTraceC -1 >= yy2)
            lpWorkBuffer->lengthTraceC = yy2 -  lpWorkBuffer->traceC[0];

  }

        ret = 0;
        kk=0;
        mass = 0;
        yMax=yMin=0;
        xMax=0;
        xMin=32000;
        if(width>length){
           x_sides = (width-length)/2;
           left_offset =0;
           }
        else{
           left_offset = (length-width)/2;
           if((length-width)%2) max--;
           x_sides=0;
           }

        left_b = min + left_offset;
        right_b = max - left_offset;


        if(height>lpWorkBuffer->lengthTraceL){
             y_sides=(height-lpWorkBuffer->lengthTraceL)/2;
             top_offset=0;
             }
        else{
             top_offset=(lpWorkBuffer->lengthTraceL- height)/2;
             if((lpWorkBuffer->lengthTraceL- height)%2) kk=1;
             y_sides=0;
             }

        memset(lpExtractCh,0xFF,width_bt*height);

        for(i=top_offset;i<lpWorkBuffer->lengthTraceL-top_offset-kk;i++) {

             i_y=(i+lpWorkBuffer->traceL[0]);

             if(lpWorkBuffer->traceL[i+1]<left_b){
                             x_start=left_b;
                             }
             else{
                  x_start = lpWorkBuffer->traceL[i+1];
                  }

             if(lpWorkBuffer->traceC[i+1]>right_b){
                             x_end=right_b;
                             }
             else{
                  x_end = lpWorkBuffer->traceC[i+1];
                  }
             min=32000;
             max=0;
             for(j=x_start;j<x_end+1;j++) {

               if(whatIsThis(lpWorkBuffer,j,i_y)) {
                   if(min==32000) {min=j;}
                   max=j;
                   mass++;

                   mask=LEFT_BIT;
                   mask >>=((x_sides+j-left_b)%8);
                   lpExtractCh[(height-(y_sides+i-top_offset)-1)* width_bt+(x_sides+j-left_b)/8] &= ~(mask);
                   }
               }
             if(xMin>min) {xMin=min;}
             if(xMax<max) {xMax=max;}

             }

       if((charIndex>3) && (lpWorkBuffer -> segmType != 55)) {
         yMin = lpWorkBuffer->traceL[0];
         yMax = yMin + lpWorkBuffer->lengthTraceL - 1;
         tHm=12;
         tHm=(int)((float)tHm*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
         if(mass<tHm) {return(1001);}
         y1=(lpWorkBuffer->curLine[nof_seg]).y1;
         y2=(lpWorkBuffer->curLine[nof_seg]).y2;
         min = 2;
         minR = 2;

/* left side */
        kk       = 0;
        for(j=xMin;j<xMax;j++) {
             flgBlack=0;
             for(i_y=yMin;i_y<=yMax;i_y++) {
               if(whatIsThis(lpWorkBuffer,j,i_y)) {flgBlack++;}
               }
             if(flgBlack > 0) kk++;
             else             kk = 0;
             if(kk > 2) {
                xMin = j - 2;
                break;
                }
             if(flgBlack > min) {
                if(kk > 0) kk--;
                xMin = j - kk;
                break;
                }

             }

/* right side */
        kk = 0;
        for(j=xMax;j>xMin;j--) {
             flgBlack=0;
             for(i_y=yMin;i_y<=yMax;i_y++) {
               if(whatIsThis(lpWorkBuffer,j,i_y)) {flgBlack++;}
               }
             if(flgBlack > 0) kk++;
             else             kk = 0;
             if(kk > 2) {
                xMax = j + 2;
                break;
                }
             if(flgBlack > min) {
                if(kk > 0) kk--;
                xMax = j + kk;
                break;
                }
             }

/* top  */
        kk = 0;
        for(i_y=yMin;i_y<yMax;i_y++) {
             flgBlack=0;
             for(j=xMin;j<=xMax;j++) {
               if(whatIsThis(lpWorkBuffer,j,i_y)) {flgBlack++;}
               }
             if(flgBlack > 0) kk++;
             else             kk = 0;
             if(kk > 2) {
                yMin = i_y - 2;
                break;
                }
             if(flgBlack > min) {
                if(kk > 0) kk--;
                yMin = i_y - kk;
                break;
                }
             }

/* bottom  */
        kk = 0;
        for(i_y=yMax;i_y>yMin;i_y--) {
             flgBlack=0;
             for(j=xMin;j<=xMax;j++) {
               if(whatIsThis(lpWorkBuffer,j,i_y)) {flgBlack++;}
               }
             if(flgBlack > 0) kk++;
             else             kk = 0;
             if(kk > 2) {
                yMax = i_y + 2;
                break;
                }
             if(flgBlack > min) {
                if(kk > 0) kk--;
                yMax = i_y + kk;
                break;
                }
             }

         if((lpWorkBuffer->smallSignPer) > 0) {
            avArea = lpWorkBuffer->avArea;
            if(avArea > 0) {
                curArea = (yMax-yMin)*(xMax-xMin);
                ratioA = (int)(((float)(curArea)/(float)avArea)*(float)100);
                if(ratioA < (lpWorkBuffer->smallSignPer) )
                         return(5);   // small sign
                }
            }

//-------------------------------------------------------------------
         yy1=(lpWorkBuffer->curLine[nof_seg]).ym1;
         yy2=(lpWorkBuffer->curLine[nof_seg]).ym2;
         flHeight= (float)0.0;
         flShape = (float)0.0;
         flWidth = (float)0.0;
//         y1=(lpWorkBuffer->curLine[nof_seg]).y1;
//         y2=(lpWorkBuffer->curLine[nof_seg]).y2;
         min=y2-y1;
         if(min>0) {
            flHeight=(float)(yMax-yMin)/(float)min;
            max=xMax-xMin;
            if(max>0) {
//              flMass=(float)(mass)/((float)min*(float)min);
              flShape=(float)(yMax-yMin)/(float)max;
              flWidth = (float)(yy2-yy1)/(float)max;
              }
            }
//?????????????????????????????????????????????????????????????
         tHm1 = (y2-y1)/10;
         if(yy1 - y1 <= tHm1) yy1 = y1;
         if(y2 - yy2 <= tHm1) yy2 = y2;
         tHm  = (yy1 -  y1)/3;
         tHm2 = (yy2 - yy1)/3;
         tHm3 = (y2  - yy2)/3;
         j = 0;
         if(yy1 - yMin > tHm) j += 1;
         if(min(yMax,yy2) - max(yMin,yy1) > tHm2) j += 2;
         if(yMax - yy2 > tHm3) j += 4;
         i = 12;
         switch(j) {
            case 1: i = 10;break;
            case 2: i = 13;break;
            case 3: i = 11;break;
            case 4: i = 15;break;
            case 6: i = 14;break;
            case 7: i = 12;break;
            }

/****************************
10 - top
11 - top, mid
12 - top, mid, bot
13 - mid
14 - mid, bot
15 - bot
*****************************/
         smallSignYesNo = 0;
         switch(lpWorkBuffer->smallSignStrat) {
           case 0:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.4)&&(flWidth>2.0))) smallSignYesNo = 1;
                  break;
           case 1:
                  if((flHeight<(float)0.3) || ((flHeight<(float)0.4)&&(flWidth>1.8))
                     || ((flHeight<(float)0.6)&&(flWidth>2.0))) smallSignYesNo = 1;
                  break;
           case 2:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.42)&&(flWidth>1.7))
                     || ((flHeight<(float)0.5)&&(flWidth>1.8))
                     || ((flHeight<(float)0.6)&&(flWidth>2.0))) smallSignYesNo = 1;
                  break;
           case 3:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.4)&&(flWidth>1.38))
                     || ((flHeight<(float)0.45)&&(flWidth>1.65))
                     || ((flHeight<(float)0.5)&&(flWidth>1.8))
                     || ((flHeight<(float)0.6)&&(flWidth>2.0))) smallSignYesNo = 1;
                  break;
           case 4:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.4)&&(flWidth>1.3))
                     || ((flHeight<(float)0.45)&&(flWidth>1.48))
                     || ((flHeight<(float)0.5)&&(flWidth>1.65))
                     || ((flHeight<(float)0.6)&&(flWidth>1.8))) smallSignYesNo = 1;
                  break;
           case 5:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.4)&&(flWidth>1.3))
                     || ((flHeight<(float)0.45)&&(flWidth>1.38))
                     || ((flHeight<(float)0.55)&&(flWidth>1.55))
                     || ((flHeight<(float)0.6)&&(flWidth>1.7))) smallSignYesNo = 1;
                  break;
           case 6:
                  if((flHeight<(float)0.3)
                     || ((flHeight<(float)0.4)&&(flWidth>1.3))
                     || ((flHeight<(float)0.55)&&(flWidth>1.35))
                     || ((flHeight<(float)0.6)&&(flWidth>1.6))) smallSignYesNo = 1;
                  break;


           }

        if(smallSignYesNo) {
// small marks
         cTop = yy1;
         cBot = yy2;
         cMid = (y2-y1)/3;
         if((yy1 == y1) && (yy2 == y2)) {
                  cTop = y1 + cMid;
                  cBot = y2 - cMid;
                  }
         cTop++;
         cBot--;
         j = 0;
// top
         cMid = (2*cBot + cTop)/3;
         min = (cBot + cTop)/2;
         if((yMin <= cTop) && (yMax < cMid))         j = 1;
         if((yMin <= (cTop + tHm1)) && (yMax < min)) j = 1;

// bottom
         if(!j) {
           cMid = (cBot + 2*cTop)/3;
           if((yMax >= cBot) && (yMin > cMid))         j = 3;
           if((yMax >= (cBot - tHm1)) && (yMin > min)) j = 3;
           }

         if(!j && flHeight>(float)0.3) {
             i = yMin - y1;if(i == 0) i = 1;
             if((y2 - yMax)/i >= 5) j = 1;
             if(!j) {
               i = y2 - yMax;if(i == 0) i = 1;
               if((yMin - y1)/i >= 5) j = 3;
               }
             }

         if(!j) j = 2;

         if((j != 2) && (flShape<(float)0.3) &&
            (yMin > (y2+3*y1)/4) && (yMax < (3*y2+y1)/4+1))
               j = 2;
          switch(j) {
                case 1: ret=40;
                        break;
                case 2: ret=60;
                        break;
                case 3: ret=80;
                        break;
                }
          i=0;
          if(flShape>(float)1.5)                                 {i=1;}
          if((!i)&&(flShape>(float)1.3)&&(flHeight>(float)0.3))  {i=1;}
          if((!i)&&(flShape<(float)0.3))                         {i=2;}
          if((!i)&&(flShape<(float)0.41)&&(j==2))                {i=2;}
          if((!i)&&(flShape<(float)1.12)&&(flShape>(float)0.88)) {i=3;}
          if((!i)&&(flShape<(float)1.26)&&(flShape>(float)0.70)&&
             (flHeight<(float)0.27))                             {i=3;}
          if((!i)&&(flShape<(float)1.35)&&(flShape>(float)0.55)&&
             (flHeight<(float)0.2))                              {i=3;}
          if((!i)&&(flShape<(float)1.3)&&(flShape>(float)0.7)&&
             (flHeight>(float)0.3)&&(j==1))                      {i=3;}

          switch(i) {
                case 0: ret+=15;
                        break;
                case 1:
                        break;
                case 2: ret+=10;
                        break;
                case 3: ret+=5;
                        break;
                }
          }
         }
        else {   // Hand printed
          if(lpWorkBuffer -> segmType != 55)
              ret = smSignHnd(lpWorkBuffer);
          }
  return (ret);

  } /* of sendToBuffer() */

int  min_left(STRUCTWB *lpWorkBuffer) {
/*-----------------------------------------------------------------*
 *         get minimal value of left trace                         *
 *-----------------------------------------------------------------*/
   int i, min;

      min = lpWorkBuffer->traceL[1];
      for(i=1; i<lpWorkBuffer->lengthTraceL; i++){
          if (lpWorkBuffer->traceL[i+1]<min){
                min = lpWorkBuffer->traceL[i+1];
                }
          }
      return(min);
      } /* of min_left() */

 int max_current(STRUCTWB *lpWorkBuffer) {
/*-----------------------------------------------------------------*
 *         get maximal value of right trace                        *
 *-----------------------------------------------------------------*/
   int i, max;
      max = lpWorkBuffer->traceC[1];
      for(i=1; i<lpWorkBuffer->lengthTraceC; i++){
          if (lpWorkBuffer->traceC[i+1]>max){
                max = lpWorkBuffer->traceC[i+1];
                }
          }
      return(max);
      } /* of max_current() */

 int smSignHnd(STRUCTWB *lpWorkBuffer){
    int i, j, iL, iR, thL, thR, lm, rm, gm, dm;
    int w, h, ratioA, perA;
    float ratio;
    int avArea;

     avArea = lpWorkBuffer->avArea;

     if((lpWorkBuffer ->markNumber) <= 0)
                                return(0);
     perA = (lpWorkBuffer->smallSignPer);
     thL = (lpWorkBuffer->traceL[1]);
     thR = (lpWorkBuffer->traceC[1]);
     iL = -1;
     for(i=0;i<(lpWorkBuffer ->markNumber);i++) {
           lm=(lpWorkBuffer-> mark[i]).l_m;
           if(lm < 0)
               continue;
           if(lm >= thL) {
                if(lm >= thR)
                         return(0);
                iL = i;
                break;
                }
            }
     if((iL <0) || (iL >= (lpWorkBuffer ->markNumber)))
                                    return(0);
     j = min(iL + 3,(lpWorkBuffer ->markNumber)-1);
     iR = -1;
     for(i=j;i>=0;i--) {
           rm=(lpWorkBuffer-> mark[i]).r_m;
           if(rm < 0)
               continue;
           if(rm <= thR) {
                if(rm <= thL)
                         return(0);
                iR = i;
                break;
                }
            }

     if((iR <0) || (iR >=(lpWorkBuffer ->markNumber)))
                                    return(0);
     gm = (lpWorkBuffer-> mark[iL]).g_m;
     dm = (lpWorkBuffer-> mark[iL]).d_m;
     if(iL < iR) {
        for(i=iL+1;i<=iR;i++) {
           if(gm > (lpWorkBuffer-> mark[i]).g_m)
                     gm = (lpWorkBuffer-> mark[i]).g_m;
           if(dm < (lpWorkBuffer-> mark[i]).d_m)
                     dm = (lpWorkBuffer-> mark[i]).d_m;
           }
        }
    if(avArea == 0)
             return(0);
    w      = rm - lm;
    h      = dm - gm;
    ratioA = (int)(((float)(w*h)/(float)avArea)*(float)100);
    if( w > 0)
        ratio = (float)h/(float)w;
    else
       return(0);
    if(ratioA < 5)
            return(5);   // trash

    if(ratio > (float)2.5)
             return(4);   // 1 or i

    if((ratioA < 50) && (ratio < 0.50))
            return(45);  // hyphen

    if(ratioA < perA)
            return(5);   // small sign


    return(0);

 }

 int whichFrame(STRUCTWB *lpWorkBuffer, int x)
 {
   int i;
      i=0;
      while( i<(lpWorkBuffer->frNumber) ) {
         if( ((lpWorkBuffer->curLine[i]).x1<=x)&&((lpWorkBuffer->curLine[i]).x2>=x)) return(i);
         i++;
         }
      return(-1);
 } /* of whichFrame() */

 void shiftTopBordA(STRUCTWB *lpWorkBuffer, int yIn, int iEnd, int *hist)
 {
  int i, j, kk, i_y, cB;

        for(i=yIn;i>iEnd;i--)
            if(hist[i] == 0) {
                 kk = yIn - i;
                 cB = lpWorkBuffer->traceL[lpWorkBuffer->lengthTraceL];
                 for(j=1;j<=kk;j++) {
                     i_y = lpWorkBuffer->lengthTraceL + j;
                     if(i_y < MAXLTRACE)
                        lpWorkBuffer->traceL[i_y] = cB;
                     }
                 cB = lpWorkBuffer->traceC[lpWorkBuffer->lengthTraceC];
                 for(j=1;j<=kk;j++) {
                     i_y = lpWorkBuffer->lengthTraceC + j;
                     if(i_y < MAXLTRACE)
                         lpWorkBuffer->traceC[i_y] = cB;
                     }
                 lpWorkBuffer->lengthTraceL += kk;
                 lpWorkBuffer->traceL[0]    -= kk;
                 lpWorkBuffer->lengthTraceC += kk;
                 lpWorkBuffer->traceC[0]    -= kk;
                 break;
                 }
 }
// extern int charSaveIndex;
// void printSegm(STRUCTWB * lpWorkBuffer, char *str);
 int SegmChars2(STRUCTWB *lpWorkBuffer, int scal, int kindD, int *lpLwidth, char *fieldName, int sizeW){
  int i, frN, k, lm;
  int kext;
  int ret;
  int begHOtp, endHOtp = 0;
  IMAGEL  imageForRec;
  int ccG1, ccAll1;
  int ccG2, ccAll2;
  float factor1=100, factor2;
  int secondSegm;
  int segmMode;
  int segmModeNumbers = 0;
  int segmModeDate = 0;
//  saveLog("", "\n");
//  saveLog("SegmChars2", "\n");
//   flgI=1;
//      printf("     lpWorkBuffer->flgUNP= %d \n", lpWorkBuffer->flgUNP);

  if(lpWorkBuffer->lengthTraceL >= MAXLTRACE) lpWorkBuffer->lengthTraceL = MAXLTRACE-1;

//  if((US_State == 2 || US_State == 3) && (strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "LOGO3") == 0)) segmModeDate = 1;
  if((strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0 || strcmp(fieldName, "LOGO3") == 0) ||
		  strcmp(fieldName, "RET_NUMA") == 0 || strcmp(fieldName, "RET_NUMN") == 0) segmModeDate = 1;
  if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) segmModeNumbers = 1;


  {int nnn = 1000;
     if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) {

       if(lpWorkBuffer->infFrame[0].nLines > 0)
		    nnn = lpWorkBuffer->hightImg/lpWorkBuffer->infFrame[0].nLines;
       if(nnn < 60) segmModeNumbers = 0;
     }
  }
  segmMode = 0;
  if(segmModeNumbers == 1 || segmModeDate == 1)
        segmMode = 1;


   secondSegm = 0;
   if( (lpWorkBuffer->newLine)==1 ) {
      (lpWorkBuffer->markNumber)=0;
      (lpWorkBuffer->indMinMark)=0;
      (lpWorkBuffer->indMaxMark)=0;
      (lpWorkBuffer->indCurMinMark)=0;
      (lpWorkBuffer->indCurMaxMark)=0;
      frN=(lpWorkBuffer->frNumber);
      k=0;
      kext=3;
      lm=0;
      (lpWorkBuffer->avLineHigh)=0;

//printf("  lpWorkBuffer->newLine= %d \n", lpWorkBuffer->newLine);

      for(i=0;i<frN;i++)  {
         if( (((lpWorkBuffer->curLine[i]).y2)!=0) || ((i>0) && (((lpWorkBuffer->curLine[i-1]).y2)!=0) && k) ) {
                lm++;
                profiles(lpWorkBuffer,i);


                k=SegmProfiles(lpWorkBuffer,i,k);

//     printf("SegmProfiles;  k= %d \n", k);

                searchBordLine(lpWorkBuffer,i);
                (lpWorkBuffer->avLineHigh)+=((lpWorkBuffer->curLine[i]).y2-(lpWorkBuffer->lowBord)-(lpWorkBuffer->curLine[i]).y1)-(lpWorkBuffer->upBord);
             if(!lpWorkBuffer->flgUNP) {
                kext=minMaxLok(lpWorkBuffer,i,kext,kindD);
                (lpWorkBuffer->indCurMaxMark)=(lpWorkBuffer->markNumber);
                }
              }
          }


 /*  un-proportional fonts */
     if(!lpWorkBuffer->flgUNP) {(lpWorkBuffer->Method)=2;}


     angleH = 0L;
     shfX   = 0L;
     ccG1 = 0;
	 ccAll1 = 0;

     if( (((lpWorkBuffer->  lettWidth)>0)&&((lpWorkBuffer->  Method)==1))||(lpWorkBuffer->flgUNP) ) {
         if(!(lpWorkBuffer->flgItalic)){

                             histV(lpWorkBuffer);
             }
         }

         if( (lpWorkBuffer->  Method)==1 ) {

 /*  one step segmentation for un-proportional fonts*/
             begHOtp=1;
             endHOtp=hntHist[0];
             memset(markConst,0x00,(unsigned int)(sizeof(int)*MAXNUMBCUTS));

       lpWorkBuffer->angH = angleH;
       lpWorkBuffer->shfH = shfX;

       imageForRec.lpPointer  = (lpWorkBuffer->lpRBuffer);
       imageForRec.iLineWidth = (lpWorkBuffer->realWidth);
       imageForRec.iImgHeight = (lpWorkBuffer->hightImg);

       imageForRec.iXBegin    = (lpWorkBuffer->infFrame[0]).x1;
       imageForRec.iYBegin    = (lpWorkBuffer->curLine[0]).y1;
       imageForRec.iWidth     = (lpWorkBuffer->infFrame[0]).x2 - (lpWorkBuffer->infFrame[0]).x1 + 1;
       imageForRec.iHeight    = (lpWorkBuffer->curLine[0]).y2 - (lpWorkBuffer->curLine[0]).y1 + 1;

       imageForRec.iDpiX      = (lpWorkBuffer->Dpi.x);
       imageForRec.iDpiY      = (lpWorkBuffer->Dpi.y);

//       *lpLwidth= (lpWorkBuffer->  lettWidth);
       if((lpWorkBuffer->flgUNP)==2) {
  // segmentation for proportional font
                  *lpLwidth= (lpWorkBuffer->  lettWidth);
                      ret=segFont((IMAGEL *)&imageForRec,
                               (int  *)&hntHist[begHOtp],endHOtp-begHOtp+1,
                               angleH,shfX,(lpWorkBuffer->recChar),
                               (int  *)lpLwidth,(int *)markConst,
                               (int  *)&indxMarkC,(long *)&Cost[0],(int *)pOutT,0,fieldName, segmMode);
                      if(ret<0) return(ret);

                  (lpWorkBuffer->  lettWidth)=*lpLwidth;
        }

       if((lpWorkBuffer->flgUNP)==1) {
         *lpLwidth=0;
           ret=segFixedPitch2((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                         (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                         (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0], 0, segmMode);
           segmMode = 0;
          if(ret<0) return(ret);
         (lpWorkBuffer->  lettWidth)=*lpLwidth;
//if(strcmp(fieldName, "NUMBERS") == 0) {
         ccG1 = 0;
         ccAll1 = 0;

    checkConfidence(lpWorkBuffer, (IMAGEL *)&imageForRec, (lpWorkBuffer->recChar), begHOtp,
    		(int *)&hntHist[begHOtp], endHOtp-begHOtp+1, markConst, indxMarkC, &ccG1, &ccAll1);
//    saveLog("checkConfidence", "\n");

            factor1 = 100;
            if(ccAll1> 0) factor1 = (float)ccG1/(float)ccAll1;
//            if(ccAll1 > 0 && ccG1 == 0) secondSegm = 1;
            if(factor1 < 0.5) secondSegm = 1;

//          sprintf(buff,"ccAll1= %d, ccG1= %d, factor1= %e, secondSegm= %d\n",ccAll1, ccG1, factor1, secondSegm);
//          saveLog(buff, "");
//       }
         }
       else {
           *lpLwidth=(lpWorkBuffer->  lettWidth);
            ret=segFixedPitch((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                         (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                         (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0],0);
            if(ret<0) return(ret);

            }
       if(segmMode == 1)
           secondSegm = 0;
       else
          secondSegm = 1;

       if(segmModeNumbers == 1 || segmModeDate == 1)
             segmMode = 1;


       if(secondSegm==1) {
  // segmentation for proportional font
                  *lpLwidth= (lpWorkBuffer->  lettWidth);
                      ret=segFont((IMAGEL *)&imageForRec,
                               (int  *)&hntHist[begHOtp],endHOtp-begHOtp+1,
                               angleH,shfX,(lpWorkBuffer->recChar),
                               (int  *)lpLwidth,(int *)markConst2,
                               (int  *)&indxMarkC2,(long *)&Cost2[0],(int *)pOutT,0,fieldName, segmMode);
                      if(ret<0) return(ret);

                  (lpWorkBuffer->  lettWidth)=*lpLwidth;
                  ccG2 = 0;
                  ccAll2 = 0;

                      checkConfidence(lpWorkBuffer, (IMAGEL *)&imageForRec, (lpWorkBuffer->recChar), begHOtp,
                      		(int *)&hntHist[begHOtp], endHOtp-begHOtp+1, markConst2, indxMarkC2, &ccG2, &ccAll2);

                  //         (lpWorkBuffer->  lettWidth)=*lpLwidth;
                      factor2 = 100;
                      if(ccAll2> 0) factor2 = (float)ccG2/(float)ccAll2;

//                  sprintf(buff,"ccAll2= %d, ccG2= %d, factor2 = %e\n",ccAll2, ccG2, factor2);
//                  saveLog(buff, "\n");
                  if(factor2 > factor1) {
//                      sprintf(buff,"replaceMarks; factor1= %e, factor2 = %e\n",factor1, factor2);
//                      saveLog(buff, "\n");
                      replaceMarks();
                  }
        }


     }
  }


      filtrMark(lpWorkBuffer);
 /*----------------------------------------------------------------*/
 /*  one step segmentation for unproportional fonts */

      if( ((lpWorkBuffer->  lettWidth)>0)&&((lpWorkBuffer->  Method)==1) ) {
        if(kindD >3) {
         avArea  =  (lpWorkBuffer->curLine[0]).y2 - (lpWorkBuffer->curLine[0]).y1;
         avArea *=  (lpWorkBuffer->  lettWidth);
         avArea  = (int)(((float)avArea*(float)75)/(float)100);
         lpWorkBuffer->avArea = avArea;
         }
         (lpWorkBuffer->newLine)=0;
         if(indxMarkC >= NUMBBOX) {indxMarkC = NUMBBOX-1;}
         for(i=0;i<indxMarkC;i++) {
               if(markConst[i] < 0) {markConst[i] = 0;}
               if(markConst[i] >= (lpWorkBuffer -> Width)) {markConst[i] = (lpWorkBuffer -> Width) -1;}
               }


//       	 if(strcmp(fieldName, "NUMBERS") == 0) {
//        	 cleanCuts((IMAGEL *)&imageForRec, (lpWorkBuffer->recChar), (int  *)&hntHist[begHOtp], endHOtp-begHOtp+1);
//         }
//       	 if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0) {
        	 cleanCuts((IMAGEL *)&imageForRec, (lpWorkBuffer->recChar), (int  *)&hntHist[begHOtp], endHOtp-begHOtp+1, sizeW);
//         }

         segmUNP(begHOtp,lpWorkBuffer);

//   if(strcmp(fieldName, "NUMBERS") == 0)
//         printSegm(lpWorkBuffer, "segm2");

         ret=sendNextFont(lpWorkBuffer, fieldName);


         return(ret);
         }

      return(0);

 } /* of Segm() */

 int SegmChars(STRUCTWB *lpWorkBuffer, int scal, int kindD, int *lpLwidth, char *fieldName, int sizeW){
  int i, frN, k, lm, rm, gm, dm, wid_s, hig_s, hig_l, hig_d, hig_W = 0, hig_E;
  int crL, crR, higL, higR, kext, flgI, Ok, minSp;
  int ret;
  int begHOtp, endHOtp = 0;
  float  cosAlph;
  IMAGEL  imageForRec;

//  lpWorkBuffer->flgUNP=2;
//   printf("       lpWorkBuffer->flgUNP= %d, fieldName= %s \n", lpWorkBuffer->flgUNP, fieldName);
//   if(strcmp(fieldName, "RET_NUMN") == 0)
//	    if(strcmp(fieldName, "RET_NUMN") == 0){
//	   	       printf("    flgI= %d, lpWorkBuffer->newSegm= %d, lpWorkBuffer->curMark1= %d, lpWorkBuffer->curMark2= %d, lpWorkBuffer->markNumber= %d\n",flgI,lpWorkBuffer->newSegm, lpWorkBuffer->curMark1,lpWorkBuffer->curMark2, lpWorkBuffer->markNumber);
//    	       printf("    SegmChars; lpWorkBuffer->newLine= %d, \n",lpWorkBuffer->newLine);
//	    }

  if(lpWorkBuffer->lengthTraceL >= MAXLTRACE) lpWorkBuffer->lengthTraceL = MAXLTRACE-1;

   flgI=1;
   if( (lpWorkBuffer->newLine)==1 ) {
      (lpWorkBuffer->markNumber)=0;
      (lpWorkBuffer->indMinMark)=0;
      (lpWorkBuffer->indMaxMark)=0;
      (lpWorkBuffer->indCurMinMark)=0;
      (lpWorkBuffer->indCurMaxMark)=0;
      frN=(lpWorkBuffer->frNumber);
      k=0;
      kext=3;
      lm=0;
      (lpWorkBuffer->avLineHigh)=0;


      for(i=0;i<frN;i++)  {
         if( (((lpWorkBuffer->curLine[i]).y2)!=0) || ((i>0) && (((lpWorkBuffer->curLine[i-1]).y2)!=0) && k) ) {
                lm++;
                profiles(lpWorkBuffer,i);


                k=SegmProfiles(lpWorkBuffer,i,k);
                searchBordLine(lpWorkBuffer,i);
                (lpWorkBuffer->avLineHigh)+=((lpWorkBuffer->curLine[i]).y2-(lpWorkBuffer->lowBord)-(lpWorkBuffer->curLine[i]).y1)-(lpWorkBuffer->upBord);
             if(!lpWorkBuffer->flgUNP) {
                kext=minMaxLok(lpWorkBuffer,i,kext,kindD);
                (lpWorkBuffer->indCurMaxMark)=(lpWorkBuffer->markNumber);
                }
              }
          }


 /* ?? Hand or OCR  ??? */
//        if(kindD <= 3)
//             averageSize(lpWorkBuffer);
//        if((lpWorkBuffer->hndOrOCR) == 1 ) {
//         return(hndOrOCR(lpWorkBuffer));

 /* ------------------
     0  - OCR
     >0 - Hand
     <0 - dont know
 ---------------------*/
//         }
 /* ?? Hand or OCR  ??? */


 /*  un-proportional fonts */

     if(!lpWorkBuffer->flgUNP) {(lpWorkBuffer->Method)=2;}

//  printf("       lpWorkBuffer->Method= %d, fieldName= %s \n", lpWorkBuffer->Method, fieldName);

     angleH = 0L;
     shfX   = 0L;

     if( (((lpWorkBuffer->  lettWidth)>0)&&((lpWorkBuffer->  Method)==1))||(lpWorkBuffer->flgUNP) ) {
         if(!(lpWorkBuffer->flgItalic)){

                             histV(lpWorkBuffer);
             }
         }
         if( (lpWorkBuffer->  Method)==1 ) {

 /*  one step segmentation for un-proportional fonts*/
             begHOtp=1;
             endHOtp=hntHist[0];
             memset(markConst,0x00,(unsigned int)(sizeof(int)*MAXNUMBCUTS));

//       if(lpWorkBuffer->flgItalic) {

 /*-----------------------------------------------------------*/
 /* ITALIC ITALIC ITALIC ITALIC ITALIC ITALIC ITALIC !!! ??? */
//            shfX = 0L;
//            histVSkewed(lpWorkBuffer,(float)0,(float)40,(float)4,(int *)hntHist,(int *)&hntHist[0],(LONG *)&angleH,(int *)&scAngleH,(long *)&shfX);
//            endHOtp=hntHist[0];

 /* ITALIC ITALIC ITALIC ITALIC ITALIC ITALIC ITALIC !!! ??? */
 /*-----------------------------------------------------------*/
//           }

       lpWorkBuffer->angH = angleH;
       lpWorkBuffer->shfH = shfX;

             if(((lpWorkBuffer->flgUNP)==4) || ((lpWorkBuffer->flgUNP)==5)) {
                 if((lpWorkBuffer->flgUNP)==4) {

                     *lpLwidth=0;
 //                *lpLwidth=(lpWorkBuffer->  lettWidth);
                   ret=segFixedPitch((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                                 (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                                 (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0],1);
                   if(ret<0) return(ret);
                   (lpWorkBuffer->  lettWidth)=*lpLwidth;
                   }
                 else {
                 *lpLwidth=(lpWorkBuffer->  lettWidth);
                   ret=segFixedPitch((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                                 (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                                 (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0],2);
                   if(ret<0) return(ret);
                  (lpWorkBuffer->  lettWidth)=*lpLwidth;
                   }


               }
             else {
               if((lpWorkBuffer->flgUNP)==2) {

 // segmentation for proportional font
                 imageForRec.lpPointer  = (lpWorkBuffer->lpRBuffer);
                 imageForRec.iLineWidth = (lpWorkBuffer->realWidth);
                 imageForRec.iImgHeight = (lpWorkBuffer->hightImg);

                 imageForRec.iXBegin    = (lpWorkBuffer->infFrame[0]).x1;
                 imageForRec.iYBegin    = (lpWorkBuffer->curLine[0]).y1;
                 imageForRec.iWidth     = (lpWorkBuffer->infFrame[0]).x2 - (lpWorkBuffer->infFrame[0]).x1 + 1;
                 imageForRec.iHeight    = (lpWorkBuffer->curLine[0]).y2 - (lpWorkBuffer->curLine[0]).y1 + 1;

                 imageForRec.iDpiX      = (lpWorkBuffer->Dpi.x);
                 imageForRec.iDpiY      = (lpWorkBuffer->Dpi.y);

                 *lpLwidth= (lpWorkBuffer->  lettWidth);
// printf("  3 segFont; *lpLwidth= %d, fieldName= %s \n", *lpLwidth, fieldName);
                 ret=segFont((IMAGEL *)&imageForRec,
                             (int  *)&hntHist[begHOtp],endHOtp-begHOtp+1,
                             angleH,shfX,(lpWorkBuffer->recChar),
                             (int  *)lpLwidth,(int *)markConst,
                             (int  *)&indxMarkC,(long *)&Cost[0],(int *)pOutT,0,fieldName, 0);

                 if(ret<0) return(ret);
                 (lpWorkBuffer->  lettWidth)=*lpLwidth;
//printf("  after segFixedPitch; *lpLwidth= %d, fieldName= %s\n", *lpLwidth, fieldName);


                 }
             else {
                 if((lpWorkBuffer->flgUNP)==1) {

                   *lpLwidth=0;
                   ret=segFixedPitch((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                                   (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                                   (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0],0);
                   if(ret<0) return(ret);
//  if(strcmp(fieldName, "NUMBERS") == 0) {
//    checkConfidence(lpWorkBuffer, (lpWorkBuffer->recChar), begHOtp);
//  }
                   (lpWorkBuffer->  lettWidth)=*lpLwidth;
//printf("  after segFixedPitch; *lpLwidth= %d, fieldName= %s \n", *lpLwidth, fieldName);
                   }
                 else {
                     *lpLwidth=(lpWorkBuffer->  lettWidth);
// printf("  5 segFixedPitch; *lpLwidth= %d, fieldName= %s \n", *lpLwidth, fieldName);
                      ret=segFixedPitch((WORD)lpWorkBuffer -> Dpi.x,(WORD)lpWorkBuffer -> Dpi.y,
                                   (int *)&hntHist[begHOtp],endHOtp-begHOtp+1,(int *)lpLwidth,
                                   (int *)markConst,(int *)&indxMarkC,(long *)&Cost[0],0);
                      if(ret<0) return(ret);
//   printf("--- after segFixedPitch; *lpLwidth= %d, fieldName= %s \n", *lpLwidth, fieldName);

                      }
                 }
             }
           }


      filtrMark(lpWorkBuffer);
//    printf("3    lpWorkBuffer->Method= %d, fieldName= %s \n", lpWorkBuffer->Method, fieldName);
 /*----------------------------------------------------------------*/
//       printf("  lpWorkBuffer->  Method= %d \n", lpWorkBuffer->  Method);
 /*  one step segmentation for unproportional fonts */

      if( ((lpWorkBuffer->  lettWidth)>0)&&((lpWorkBuffer->  Method)==1) ) {
        if(kindD >3) {
         avArea  =  (lpWorkBuffer->curLine[0]).y2 - (lpWorkBuffer->curLine[0]).y1;
         avArea *=  (lpWorkBuffer->  lettWidth);
         avArea  = (int)(((float)avArea*(float)75)/(float)100);
         lpWorkBuffer->avArea = avArea;
         }
         (lpWorkBuffer->newLine)=0;
         if(indxMarkC >= NUMBBOX) {indxMarkC = NUMBBOX-1;}
         if(lpWorkBuffer->flgItalic) {
             cosAlph = (float)cos((float)angleH*PI/(180.0*(float)scAngleH));
             if(cosAlph < (float)0) {cosAlph = -cosAlph;}
             if(cosAlph < (float)1E-10) {cosAlph = (float)1.0;}
             for(i=0;i<indxMarkC;i++) {
                 markConst[i] = (int)((float)markConst[i]/cosAlph  - (float)shfX/(float)scAngleH + (float)0.5);
                 }
            }

         for(i=0;i<indxMarkC;i++) {
               if(markConst[i] < 0) {markConst[i] = 0;}
               if(markConst[i] >= (lpWorkBuffer -> Width)) {markConst[i] = (lpWorkBuffer -> Width) -1;}
               }

//printf("  indxMarkC= %d, begHOtp= %d, (lpWorkBuffer->curLine[0]).x1+begHOtp-1=%d, fieldName= %s \n", indxMarkC, begHOtp, (lpWorkBuffer->curLine[0]).x1+begHOtp-1, fieldName);
//         if((lpWorkBuffer->flgUNP)==2) {


//       	 if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0) {
//        	 cleanCuts((IMAGEL *)&imageForRec, (lpWorkBuffer->recChar), (int  *)&hntHist[begHOtp], endHOtp-begHOtp+1, sizeW);
//         }
         segmUNP(begHOtp,lpWorkBuffer);

         ret=sendNextFont(lpWorkBuffer, fieldName);

         return(ret);
         }

 /*----------------------------------------------------------------*/
      if(lm>0) {
        (lpWorkBuffer->avLineHigh)/=lm;
        }
      (lpWorkBuffer->indCurMaxMark)=0;
      (lpWorkBuffer->newLine)=0;
      (lpWorkBuffer->curMark1)=0;
      (lpWorkBuffer->curMark2)=0;
      (lpWorkBuffer->highGlob)=(lpWorkBuffer->avLineHigh)+2;

 /* ---------- line begin for MultiLine!!! ---------- */

      if((lpWorkBuffer->lettWidth)!=0) {
          lm=(lpWorkBuffer->mark[0]).l_m;
          if(lm<0) lm=-lm;
          (lpWorkBuffer->numbOfChar)=lm;
          if((lpWorkBuffer->markNumber)>1)  {
               rm=(lpWorkBuffer->mark[1]).l_m;
               if((rm>0)&&(rm-lm<(lpWorkBuffer->lettWidth))) {
                  (lpWorkBuffer->numbOfChar)=rm-(lpWorkBuffer->lettWidth);
                  }
               }
          }
 /* ------------------------------------------------- */

      if((frN>1)&&((lpWorkBuffer->markNumber)==frN)&&(((lpWorkBuffer->indMinMark)==0)||((lpWorkBuffer->indMaxMark)==0))) {
         k=0;
         for(i=0;i<frN-1;i++) {
             if((lpWorkBuffer->mark[i]).r_m<0) k++;
                 }
         if(k==frN-1) {
            return(0);
            }
         }
      isMonoSpace1(lpWorkBuffer,kindD);
      }

// 	    if(strcmp(fieldName, "RET_NUMN") == 0)
//  	    	       printf("    lpWorkBuffer->newSegm= %d, \n",lpWorkBuffer->newSegm);


    if( (lpWorkBuffer->newSegm) ) {                /* new segment */
       crL=(lpWorkBuffer->curMark1);
       if( crL>=(lpWorkBuffer->markNumber) ) {
         if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) {
            (lpWorkBuffer->newLine)=1;
            (lpWorkBuffer->  lettWidth) = (lpWorkBuffer->  lettWidthG);
            return(3);                               /* new line==end of box  */
            }
         else {
            (lpWorkBuffer->newLine)=5;
            return(0);                               /* new line==end of box  */
            }
          }
 /*  blank testing  */
       if((crL == 0) && ((lpWorkBuffer->flgSpSegm) == 1) && ((lpWorkBuffer->blank)==0)) {
                i = firstSpaces(lpWorkBuffer);
                if(i > 0) {
                   (lpWorkBuffer->blank)=i;
                   return(1000+i);
                   }
                }
       if( (crL>0)&&((lpWorkBuffer->blank)==0) )  {
           lm=(lpWorkBuffer->mark[crL]).l_m;
           rm=(lpWorkBuffer->mark[crL-1]).r_m;
           if( (lm>0)&&(rm>0) ) {
              kext=lm-rm;
              i=kext;
              if(kindD>3) {
                   minSp=(int)((float)12*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
                   }
              else {
                   minSp=(int)((float)40*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
                   }
              if(kindD>3) {
                 k= (int)((float)lpWorkBuffer->lettWidth *(float)0.8);
                 }
              else  {
                 k= (int)((float)lpWorkBuffer->lettWidth *(float)1.1);
                 }
              if(k==0) k=(lpWorkBuffer->OCRSpace);
                 if(k<=0) k=minSp;
                 if((kindD>3)&&((lpWorkBuffer->lettWidth)==0)) {
                   k=max(4,k);
                   kext=kext/k;
                   if(((lpWorkBuffer->OCRSpace)>0)&&((lpWorkBuffer->monoSpace)>0)) i/=(lpWorkBuffer->monoSpace);
                   else i=kext;
                   }
                 else {
                   if(((lpWorkBuffer->OCRSpace)>0)&&((lpWorkBuffer->monoSpace)>0)) i/=(lpWorkBuffer->monoSpace);
                   else i/=k;
                   if(((lpWorkBuffer->lettWidth)==0)&&((lpWorkBuffer->OCRSpace)==0)) k+=k/4;
                   kext/=k;
                   }

              if(kext>0) {
                 if(i<=0) i=1;
                 (lpWorkBuffer->blank)=i;
                 return(1000+i);
                 }
              }
           }

 /* ---------- line begin for MultiLine!!! ---------- */

    if((crL>0)&&((lpWorkBuffer->blank)!=0)) {
      if((lpWorkBuffer->lettWidth)!=0) {
          lm=(lpWorkBuffer->mark[crL]).l_m;
          if(lm>0) {
            (lpWorkBuffer->numbOfChar)=lm;
            if((lpWorkBuffer->markNumber)>crL+1) {
               rm=(lpWorkBuffer->mark[crL+1]).l_m;
               if((rm>0)&&(rm-lm<(lpWorkBuffer->lettWidth))) {
                  (lpWorkBuffer->numbOfChar)=rm-(lpWorkBuffer->lettWidth);
                  }
               }
            }
          }
      }

 /* ------------------------------------------------- */

       (lpWorkBuffer->blank)=0;
       (lpWorkBuffer->newSegm)=0;
       lm=(lpWorkBuffer->mark[crL]).l_m;
       if(lm<0) lm=-lm;
       rm=(lpWorkBuffer->mark[crL]).r_m;
       if(rm<0) rm=-rm;
       crR=rightM(lpWorkBuffer,crL);

       (lpWorkBuffer->curMark2)=crR;
       if( (crL!=crR)&&((rm-lm)<MAXSIZECHAR2) ) {
          gm=min((lpWorkBuffer->mark[crL]).g_m,(lpWorkBuffer->mark[crL+1]).g_m);
          dm=max((lpWorkBuffer->mark[crL]).d_m,(lpWorkBuffer->mark[crL+1]).d_m);
          }
       else {
          gm=(lpWorkBuffer->mark[crL]).g_m;
          dm=(lpWorkBuffer->mark[crL]).d_m;
          }
       (lpWorkBuffer->bottBord)=dm;
       rm=(lpWorkBuffer->mark[crR]).r_m;
       if(rm<0) rm=-rm;

       wid_s=rm-lm+1;
       hig_s=min(dm-gm+2,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-gm);
       higL=min(hig_s,MAXLTRACE-1);
       higR=min( ((lpWorkBuffer->mark[crR]).d_m-(lpWorkBuffer->mark[crR]).g_m)+2,MAXLTRACE-1);
       higR=min(higR,(lpWorkBuffer -> begRect.y)+(lpWorkBuffer -> Height)-(lpWorkBuffer->mark[crR]).g_m);
       (lpWorkBuffer->lengthTraceL)=higL;
       (lpWorkBuffer->lengthTraceR)=higR;
       (lpWorkBuffer->traceL[0])=gm;
       (lpWorkBuffer->traceR[0])=(lpWorkBuffer->mark[crR]).g_m;
       (lpWorkBuffer->traceC[0])=(lpWorkBuffer->mark[crR]).g_m;

       for(i=1;i<=higL;i++)  {
            (lpWorkBuffer->traceL[i])=lm;
            }
       for(i=1;i<=higR;i++)  {
            (lpWorkBuffer->traceR[i])=rm;
            (lpWorkBuffer->traceC[i])=rm;
            }
      }/* of new segment */
    else {
       higL=(lpWorkBuffer->lengthTraceL);
       higR=(lpWorkBuffer->lengthTraceR);
       crL=(lpWorkBuffer->curMark1);
       crR=(lpWorkBuffer->curMark2);
       if((lpWorkBuffer->boundLeft)>0) {
           crL = srIndMark(lpWorkBuffer,crL,crR,(lpWorkBuffer->traceL[1])+(lpWorkBuffer->boundLeft));
           }

       lm=((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceL[higL]))/2;
       rm=(lpWorkBuffer->mark[crL]).r_m;
       if(rm<0) rm=-rm;
       if( (crL!=crR)&&((rm-lm)<MAXSIZECHAR2) ) {
          gm=min((lpWorkBuffer->mark[crL]).g_m,(lpWorkBuffer->mark[crL+1]).g_m);
          dm=max((lpWorkBuffer->mark[crL]).d_m,(lpWorkBuffer->mark[crL+1]).d_m);
          }
       else {
          gm=(lpWorkBuffer->mark[crL]).g_m;
          dm=(lpWorkBuffer->mark[crL]).d_m;
          }
       (lpWorkBuffer->bottBord)=dm;
       rm=((lpWorkBuffer->traceR[1])+(lpWorkBuffer->traceR[higR]))/2;
       wid_s=rm-lm+1;
       hig_s=dm-gm+2;
       }

 /* -----thresholds for segmentation----- */
       hig_l=(int)((float)hig_s*(float)0.8);
       hig_E=max((int)((float)hig_s*(float)0.7),20);
       Ok=-2;

       switch(kindD) {
          case 1:case 2: case 3:                    /* thresholds for letters */
             hig_W=min((int)((float)hig_s*(float)2.2),100);
             hig_d=(int)((float)hig_s*(float)1.8);
             break;
         case 0:
             hig_d=(int)((float)hig_s*(float)1.2);
             break;
         case 6:case 4:
             hig_d=(int)((float)hig_s*(float)1.2);
             break;
         case 5:case 7:
             Ok=whichFrame(lpWorkBuffer,(lm+rm)/2);
             if(Ok>=0) {
                hig_s=(lpWorkBuffer->curLine[Ok]).ym2-(lpWorkBuffer->curLine[Ok]).ym1+2;
                (lpWorkBuffer->bottBord)=(lpWorkBuffer->curLine[Ok]).ym2;
                }
             hig_d=hig_s-4;
             break;
         default:
             hig_W=min((int)((float)hig_s*(float)2.2),100);
             hig_d=(int)((float)hig_s*(float)1.8);
             break;

          }
       if(hig_d>MAXSIZECHAR) hig_d=MAXSIZECHAR;
       (lpWorkBuffer->thWid)=hig_d;

 /* ------------Segmentation ------------ */

    flgUNPROP=0;
    if(((lpWorkBuffer->lettWidth)>0)&&((lpWorkBuffer->Method)==2)) {
         if( wid_s>(lpWorkBuffer->lettWidth)+(lpWorkBuffer->lettWidth)/2 ) {

                (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceL);
                for(i=1;i<=(lpWorkBuffer->lengthTraceC);i++) {
                     (lpWorkBuffer->traceC[i])=(lpWorkBuffer->traceL[i])+(lpWorkBuffer->lettWidth);
                     }
                (lpWorkBuffer->traceC[0])=(lpWorkBuffer->traceL[0]);
                flgUNPROP=1;
                if( (flgI=sendCh(scal,lpWorkBuffer,kindD))==-100) {
                     return(-100);
                     }
                (lpWorkBuffer->flgCut)=1;
                goto LETT;
                }

         if( wid_s>(lpWorkBuffer->lettWidth)+(lpWorkBuffer->lettWidth)/10 ) {
             if( (flgI=divideFr(scal,lpWorkBuffer,kindD,1))==-100) {
                 return(-100);
                 }
             }
         else {
                for(i=0;i<=(lpWorkBuffer->lengthTraceR);i++) {
                      (lpWorkBuffer->traceC[i])=(lpWorkBuffer->traceR[i]);
                      }
                (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceR);
                if( (flgI=sendCh(scal,lpWorkBuffer,kindD))==-100) {
                     return(-100);
                     }
                (lpWorkBuffer->flgCut)=1;
                if((lpWorkBuffer->flgTraceR)) {
                      (lpWorkBuffer->flgCut)=1;
                      }
                else {
                    (lpWorkBuffer->curMark1)=(lpWorkBuffer->curMark2)+1;
                    (lpWorkBuffer->newSegm)=1;
                    }
               }
         goto LETT;
         }

       i=0;
       if( (kindD==2)&&(wid_s>hig_E)&&((lpWorkBuffer->boundLeft)==0) ) {
          if(testE(scal,lpWorkBuffer)!=0) return(-100);
          }
       if( (kindD==2)&&(wid_s>hig_d)&&(wid_s<hig_W) ) {
         i=testW(lpWorkBuffer);
               }
       if(i)  {     /* if W */
           if( (flgI=divideFr(scal,lpWorkBuffer,kindD,0))==-100) return(-100);
           }
       else {      /* if no W */
         if( (wid_s>hig_d) ) {
             if( (flgI=divideFr(scal,lpWorkBuffer,kindD,1))==-100) {
                 return(-100);
                 }
             }
         else {
             if( (wid_s>hig_l) ) {
                 if( (flgI=divideFr(scal,lpWorkBuffer,kindD,0))==-100) return(-100);
                }
             else {
                for(i=0;i<=(lpWorkBuffer->lengthTraceR);i++) {
                      (lpWorkBuffer->traceC[i])=(lpWorkBuffer->traceR[i]);
                      }
                (lpWorkBuffer->lengthTraceC)=(lpWorkBuffer->lengthTraceR);
                if( (flgI=sendCh(scal,lpWorkBuffer,kindD))==-100) {
                     return(-100);
                     }
                (lpWorkBuffer->flgCut)=1;
                if((lpWorkBuffer->flgTraceR)) {
                      (lpWorkBuffer->flgCut)=1;
                      }
                else {
                    (lpWorkBuffer->curMark1)=(lpWorkBuffer->curMark2)+1;
                    (lpWorkBuffer->newSegm)=1;
                    }
                }
             }
         } /* end of if no W */
  /* ------------------------------------- */
 LETT:;
// 	    if(strcmp(fieldName, "RET_NUMN") == 0)
// 	    	       printf("    flgI= %d, lpWorkBuffer->newSegm= %d, lpWorkBuffer->curMark1= %d, lpWorkBuffer->curMark2= %d\n",flgI,lpWorkBuffer->newSegm, lpWorkBuffer->curMark1,lpWorkBuffer->curMark2);

    if(flgI==-1) return(0);
    return(flgI);

 } /* of Segm() */
 /***************************************************************************
  *									   *
  *  PURPOSE      : vertical histogram, lower and higher profiles           *
  *                                                                         *
  ***************************************************************************/
 void profiles(STRUCTWB *lpWorkBuffer, int indF){
   int  i, j, widthL, yy, xx;
   int x1, y1, x2, y2;

     x1=(lpWorkBuffer->curLine[indF]).x1;
     x2=(lpWorkBuffer->curLine[indF]).x2;
     y1=(lpWorkBuffer->curLine[indF]).y1;
     y2=(lpWorkBuffer->curLine[indF]).y2;
     if(y2==0) {
         y2=(lpWorkBuffer->curLine[indF-1]).y2;
         y1=(lpWorkBuffer->curLine[indF-1]).y1;
         }
     memset(hntHist,0x00,(unsigned int)(2*MAXPROFILE));
     memset(upProf,0x00,(unsigned int)(2*MAXPROFILE));
     widthL=y2-y1+2;
     for (i=0;i<MAXPROFILE;i++){
             lowProf[i]=widthL;
             }

     if(y1<1) y1=1;
     for(j=0;j<x2-x1+1;j++){
       xx=x1+j;
       for(i=0;i<widthL;i++){
            yy=y1+i-1;
            if(whatIsThis(lpWorkBuffer,xx,yy)) {
                  hntHist[j]++;
                  lowProf[j] = widthL-i;
                  if(upProf[j]==0){
                          upProf[j]=widthL-i+1;
                          }
                  }
          }
       }
    }/* of profiles()*/


 /***************************************************************************
  *  PURPOSE      :segmentation profiles  of character string               *
  *                                                                         *
  *  RETURN VALUE :function returns the values:                             *
  *                0 -character is not on the frame border                  *
  *                1 -character is on the frame border                      *
  *                                                                         *
  ***************************************************************************/

 int  SegmProfiles(STRUCTWB *lpWorkBuffer, int indF, int k)
 {
   int i, ins_flag, zer_c, min_h = 0, max_h = 0, pMark;
   int  kkl, kku, x1, y1, x2, y2, len, flgZerLine;
    flgZerLine=0;
    if((lpWorkBuffer->curLine[indF]).y2==0)  {
         (lpWorkBuffer->curLine[indF]).y2=(lpWorkBuffer->curLine[indF-1]).y2;
         (lpWorkBuffer->curLine[indF]).y1=(lpWorkBuffer->curLine[indF-1]).y1;
         flgZerLine=1;
         }
    x1=(lpWorkBuffer->curLine[indF]).x1;
    x2=(lpWorkBuffer->curLine[indF]).x2;
    y1=(lpWorkBuffer->curLine[indF]).y1;
    y2=(lpWorkBuffer->curLine[indF]).y2;
    len=x2-x1+1;
    zer_c=(lpWorkBuffer->markNumber);
    ins_flag=0;
    if(k) {
         if(zer_c>=MAXMARKERS) goto RESC;
         ins_flag=1;
         (lpWorkBuffer->mark[zer_c]).l_m = -x1;
         pMark=0;
         if(zer_c>0) {
            min_h=(lpWorkBuffer->mark[zer_c-1]).g_m;
            max_h=(lpWorkBuffer->mark[zer_c-1]).d_m;
            }
         }

    for (i=1;i<len;i++){
         if(ins_flag==0){
            if(hntHist[i]) {
                     if(zer_c>=MAXMARKERS) goto RESC;
                     max_h=0;
                     min_h=2*MAXSIZE;
                     ins_flag=1;
                     pMark=i-1;
                     (lpWorkBuffer->mark[zer_c]).l_m = x1+pMark;
                     continue;
                     }
            }
         else{
             if(hntHist[i]==0) {
                   ins_flag=0;
                   if( ((i-pMark)>MINWIDTHMARK)|| k ) {
                      (lpWorkBuffer->mark[zer_c]).r_m = x1+i;
                      (lpWorkBuffer->mark[zer_c]).g_m = min_h;
                      (lpWorkBuffer->mark[zer_c]).d_m = max_h;
                      zer_c++;
                      k=0;
                      if(zer_c>=MAXMARKERS) goto RESC;
                      }
                    continue;
                    }
             else{
                    kkl=y2-lowProf[i];
                    kku=y2-upProf[i];
                    if (kku<min_h) min_h=kku;
                    if (kkl>max_h) max_h=kkl;
                    }
             }

       }
         if(ins_flag==1)  {
                   (lpWorkBuffer->mark[zer_c]).r_m = -(x1+len-1);
                   (lpWorkBuffer->mark[zer_c]).g_m = min_h;
                   (lpWorkBuffer->mark[zer_c]).d_m = max_h;
                   zer_c++;
              }

 RESC:
             (lpWorkBuffer->markNumber) = zer_c;
             return(ins_flag);

  } /* SegmProfiles() */
 /***************************************************************************
  *									   *
  *  PURPOSE      :search border lines for min and max                      *
  *                                                                         *
  ***************************************************************************/
 int searchBordLine(STRUCTWB  *lpWorkBuffer, int indF)
 {
   int    i, len,  hh; //, beg, end;
   int   maxH, sumH, iS, iE, iM;
   int   top, bot, maxL, step;

     (lpWorkBuffer->curLine[indF]).ym1 = (lpWorkBuffer->curLine[indF]).y1;
     (lpWorkBuffer->curLine[indF]).ym2 = (lpWorkBuffer->curLine[indF]).y2;

      hist_h(lpWorkBuffer,(lpWorkBuffer->curLine[indF]).x1,(lpWorkBuffer->curLine[indF]).y1,(lpWorkBuffer->curLine[indF]).x2,(lpWorkBuffer->curLine[indF]).y2);
      len=hntHist[0];
      if(len<4) {
         (lpWorkBuffer->upBord)=0;
         (lpWorkBuffer->lowBord)=0;
         return(1);
         }
      maxL = 500;
      iS = 1;
      for(i=1;i<len;i++)
            if(hntHist[i] > 0) {iS = i;break;}

      iE = len;
      for(i=len;i>0;i--)
            if(hntHist[i] > 0) {iE = i;break;}

      if(iE > maxL) iE = maxL;
      if(iS >= iE) {
         (lpWorkBuffer->upBord)=0;
         (lpWorkBuffer->lowBord)=0;
         return(1);
         }

      iM = (3*iE + iS)/4;

      maxH = 0;
      for(i=iS;i<iM;i++) {
            sumH = hntHist[i] + hntHist[i+1];
            if(maxH < sumH) maxH = sumH;
            }
      maxH /= 2;
      bot  = maxH/3;
      top  = (int)((7L*(long)maxH)/8);
      hh   = maxH/2;
      step = hh/20 - 1;
      if(step > 20) step = 20;
      if(step < 1)  step = 1;

      memset(pOutT,0x00,(unsigned int)(2*maxL));
      hh = bot;
      while(hh < top) {
        for(i=iS;i<iM;i++)
             if(hntHist[i] > hh) {pOutT[i]++;break;}
        hh += step;
        }

      maxH = 0;
      hh   = 1;
      for(i=iS;i<iM;i++)
           if(pOutT[i] > maxH) {maxH = pOutT[i];hh = i;}

      hh--;
      (lpWorkBuffer->upBord) = hh;
      (lpWorkBuffer->curLine[indF]).ym1 =
                              (lpWorkBuffer->curLine[indF]).y1 +
                              (lpWorkBuffer->upBord);

 //------------------------------------------------------------

      iM = (iE + 3*iS)/4;
      maxH = 0;
      for(i=iM;i<iE;i++) {
            sumH = hntHist[i] + hntHist[i+1];
            if(maxH < sumH) maxH = sumH;
            }
      maxH /= 2;

      bot  = maxH/3;
      top  = (int)((7L*(long)maxH)/8);

      hh   = maxH/2;
      step = hh/20 - 1;
      if(step > 20) step = 20;
      if(step < 1)  step = 1;

      memset(pOutT,0x00,(unsigned int)(2*maxL));
      hh = bot;
      while(hh < top) {
        for(i=iE;i>iM;i--)
             if(hntHist[i] > hh) {pOutT[i]++;break;}
        hh += step;
        }

      maxH = 0;
      hh   = 1;
      for(i=iE;i>iM;i--)
           if(pOutT[i] > maxH) {maxH = pOutT[i];hh = i;}

      hh--;
      if(hh > lpWorkBuffer->upBord) {
        (lpWorkBuffer->lowBord) = hh;
        (lpWorkBuffer->curLine[indF]).ym2 =
                              (lpWorkBuffer->curLine[indF]).y1 +
                              (lpWorkBuffer->lowBord);
        }
   return(1);
 } /* of searchBordLine() */


 int testE(int scal,STRUCTWB *lpWorkBuffer)
 {
  int halfTr, i,  minH, x, y, dL, cntL, flgB;
  int part, xOld, flgF, flgB10, flgE10, thr, c10;
      halfTr=min(((lpWorkBuffer->traceL[1])+(lpWorkBuffer->traceR[1]))/2-4,(lpWorkBuffer->traceL[1])+50);
      minH=(lpWorkBuffer->lengthTraceL)-1;
      y=(lpWorkBuffer->traceL[0]);

      cntL=0;
      dL=0;
      flgF=0;
      xOld=0;
      for(i=2;i<minH;i++)  {
         flgB=0;
         for(x=(lpWorkBuffer->traceL[i])+1;x<halfTr;x++)  {
                 if(flgB==0) {
                    if(whatIsThis(lpWorkBuffer,x,y)) {
                        if(flgF) {
                            part=xOld-x;
                            if(part<0) part=-part;
                            if(part>dL) dL=part;
                            }
                        xOld=x;
                        flgF=1;
                        flgB=1;
                        cntL++;
                        }
                    }

                 }
         y++;
         }

      if(minH>4) {
          part=(int)(((float)dL/(float)minH)*(float)1000);
          }
      else { return(0);}
      if(part>300) return(0);

      part=(int)(((float)cntL/(float)minH)*(float)1000);
      if(part<700) return(0);

      y=(lpWorkBuffer->traceL[0])+(lpWorkBuffer->lengthTraceL)-1;
      flgB10=0;
      flgE10=0;
      c10=0;
      thr=minH/5;
      for(i=minH;i>minH/2;i--)  {
         flgB=0;
         flgF=1;
         dL=0;
         for(x=(lpWorkBuffer->traceL[i])+1;x<halfTr;x++)  {
                 if(flgF) {
                    if(whatIsThis(lpWorkBuffer,x,y)) {
                        flgB=1;
                        dL++;
                        }
                    else {
                         if(flgB) flgF=0;
                         }
                     }
                 }
         if( (minH-i)<4 ) {
                 if( (flgB10==0)&&(dL>thr) ) flgB10=1;
                 }
         else {
                 if(flgB10==0) return(0);
                 }
         if( (dL>thr)&&(flgE10==0) ) {

              c10++;
              }
         else {
              if(flgB10) flgE10=1;
              }
         if(c10>6) return(0);



         y--;
         }
         if(c10<2) return(0);
      halfTr=min((lpWorkBuffer->traceL[1])+((lpWorkBuffer->traceL[1])+2*(lpWorkBuffer->traceR[1]))/3,(lpWorkBuffer->traceL[1])+80);
      y=(lpWorkBuffer->traceL[0]);
      xOld=minH/3-1;
     memset(hntHist,0x00,200);

      for(i=1;i<=xOld;i++)  {
         c10=(lpWorkBuffer->traceL[i])+1;
         for(x=c10;x<halfTr;x++)  {
              if(whatIsThis(lpWorkBuffer,x,y)) {
                   hntHist[x-c10]++;
                   }

              }
         y++;
         }
      y=(lpWorkBuffer->traceL[0]);
      xOld=min(20,halfTr-1);
      i=0;
      while( (i<xOld)&&(hntHist[i]==0) ) i++;
      if(i>=xOld) return(0);
      x=i;
      xOld=min(x+15,halfTr-1);
      i=x;
      while( (i<xOld)&&(hntHist[i]!=0) ) i++;
      if(i>=xOld) return(0);
      x=i;
      xOld=min(x+20,halfTr-1);
      i=x;
      while( (i<xOld)&&(hntHist[i]==0) ) i++;
      if(i>=xOld) return(0);
      x=i;
      (lpWorkBuffer->boundLeft)=x;
      return(1);
 } /* of testE() */
 /***************************************************************************
  *  PURPOSE      :searching for local min/max of profiles                  *
  *                                                                         *
  ***************************************************************************/
 int  minMaxLok(STRUCTWB * lpWorkBuffer, int indF, int kext, int kindD){
   int i, indMx, indMn, max_l = 0, max_i = 0, min_l = 0, min_i = 0, j;
   int x1, x2, y2, range, flg_sw, flg_ret = 0;
   int int_flag_l, int_flag_u, flg1_4, th1, lind, rind, lm, rm;


    x1=(lpWorkBuffer->curLine[indF]).x1;
    x2=(lpWorkBuffer->curLine[indF]).x2;
    y2=(lpWorkBuffer->curLine[indF]).y2;
    indMx=(lpWorkBuffer->indMaxMark);
    indMn=(lpWorkBuffer->indMinMark);

 /*--------------
    max searching
  *--------------*/

    for(j=(lpWorkBuffer->indCurMaxMark);j<(lpWorkBuffer->markNumber);j++) {
        lm=(lpWorkBuffer->mark[j]).l_m;
        if(lm<0) lm=-lm;
        rm=(lpWorkBuffer->mark[j]).r_m;
        if(rm<0) rm=-rm;
        range=(lpWorkBuffer->mark[j]).d_m-(lpWorkBuffer->lowBord)-((lpWorkBuffer->mark[j]).g_m+(lpWorkBuffer->upBord))+1;
        int_flag_l=0;
        int_flag_u=0;
        flg1_4=0;
        flg_sw=1;
        if(kindD<4) {
          lind=lm-x1+1;
          if( (lpWorkBuffer->mark[j]).l_m>0 ) lind+=5;
          rind=rm-x1;
          if( (lpWorkBuffer->mark[j]).r_m>0 ) rind-=5;
          if((rind-lind)<10) continue;
          }
        else {
          lind=lm-x1+1;
          if( (lpWorkBuffer->mark[j]).l_m>0 ) lind+=2;
          rind=rm-x1;
          if( (lpWorkBuffer->mark[j]).r_m>0 ) rind-=2;
          if((rind-lind)<6) continue;
          }
        for(i=lind;(i<rind)&&(i<(x2-x1));i++)  {
          th1=lowProf[i]-(y2-(lpWorkBuffer->mark[j]).d_m+(lpWorkBuffer->lowBord));
          if( th1>(range/5) ) {
                 if(!int_flag_l) {
                     int_flag_l=1;
                     max_l=lowProf[i];
                     max_i=i;
                     } else {
                     if( (th1<(range/4)) && flg1_4 ) {
                         flg_sw=0;
                         flg1_4=0;
                         flg_ret=1;
                         if( (j==(lpWorkBuffer->indCurMaxMark))&&((lpWorkBuffer->mark[j]).l_m<0) ) {
                           if( ((kext==0)||(kext==2))&&((lind+2)>max_i) ) {
                             flg_ret=0;
                             }
                           }
                         if(flg_ret) {
                             if(indMx>=MAXMARKERS) goto RESC;
                             (lpWorkBuffer->maxMark[indMx])=x1+max_i;
                             indMx++;
                             }
                         continue;
                         }

                     if(lowProf[i]>max_l) {
                         max_l=lowProf[i];
                         max_i=i;
                         if( th1>(range/4) ) {
                              if(!flg_sw) {
                                 int_flag_l=0;
                                 flg_sw=1;
                                 }
                              flg1_4=1;
                              }
                         }
                     }
                 } else {
                 if(int_flag_l) {
                         int_flag_l=0;
                         flg1_4=0;
                         if(flg_sw) {
                           flg_ret=1;
                           if( (j==(lpWorkBuffer->indCurMaxMark))&&((lpWorkBuffer->mark[j]).l_m<0) ) {
                             if( ((kext==0)||(kext==2))&&((lind+2)>max_i) ) {
                               flg_ret=0;
                               }
                             }
                           if(flg_ret) {
                               if(indMx>=MAXMARKERS) goto RESC;
                               (lpWorkBuffer->maxMark[indMx])=x1+max_i;
                               indMx++;
                               }
                           }
                         flg_sw=1;
                         }
                 }
 /*--------------
    min searching
  *--------------*/
             th1=upProf[i]-(y2-(lpWorkBuffer->mark[j]).d_m+(lpWorkBuffer->lowBord));
             if( th1<(4*range)/5 ) {
                 if(!int_flag_u) {
                     int_flag_u=1;
                     min_l=upProf[i];
                     min_i=i;
                     } else {
                     if(upProf[i]<min_l) {
                         min_l=upProf[i];
                         min_i=i;
                         }
                     }
                 } else {
                 if(int_flag_u) {
                         int_flag_u=0;
                         flg_ret=1;
                         if( (j==(lpWorkBuffer->indCurMaxMark))&&((lpWorkBuffer->mark[j]).l_m<0) ) {
                             if( (kext<2)&&((lind+2)>min_i) ) {
                               flg_ret=0;
                               }
                             }
                         if(flg_ret) {
                               if(indMn>=MAXMARKERS) goto RESC;
                               (lpWorkBuffer->minMark[indMn])=x1+min_i;
                               indMn++;
                               }
                         }
                 }
              }/* for i*/

              if( int_flag_l && ((rind-2)>max_i)) {
                      if(indMx>=MAXMARKERS) goto RESC;
                     (lpWorkBuffer->maxMark[indMx])=x1+max_i;
                      indMx++;
                      }
              if( int_flag_u && ((rind-2)>min_i)) {
                      if(indMn>=MAXMARKERS) goto RESC;
                      (lpWorkBuffer->minMark[indMn])=x1+min_i;
                      indMn++;
                      }
               flg_ret=0;
               if( (j==((lpWorkBuffer->markNumber)-1))&&((lpWorkBuffer->mark[j]).r_m<0) ) {
                      if( int_flag_l && ((rind-2)<=max_i)) {
                         flg_ret=5;
                         }
                      if( int_flag_u && ((rind-2)<=min_i)) {
                         flg_ret++;
                         }
                      }

       } /*for j*/

 RESC:
    (lpWorkBuffer->indMaxMark)=indMx;
    (lpWorkBuffer->indMinMark)=indMn;
    if(flg_ret==0) return(0);                     // min Ok       max Ok
    if(flg_ret==5) return(1);                     // min Ok       max canceled
    if(flg_ret<5) return(2);                      // min canceled max Ok
    if(flg_ret>5) return(3);                      // min canceled max canceled
    return(0);
  } /* of minMaxLok() */

 void histV(STRUCTWB *lpWorkBuffer)
 {
   int  i, j, kk, curInd, yy;
   int x1, y1, x2, y2, yD, ch, mD, mG;
   int lByte, rByte, rL, rR, w_bt;
   LONG nY;
   BYTE *hptr;
   BYTE mask;

     w_bt=(lpWorkBuffer -> widthImg);
     x1=(lpWorkBuffer->curLine[0]).x1;
     x2=(lpWorkBuffer->curLine[(lpWorkBuffer->frNumber)-1]).x2;
     mG=(lpWorkBuffer->curLine[0]).y1;
     mD=(lpWorkBuffer->curLine[0]).y2;
     for(i=1;i<(lpWorkBuffer->frNumber);i++) {
         if((mG>(lpWorkBuffer->curLine[i]).y1)&&((lpWorkBuffer->curLine[i]).y2>0)) mG=(lpWorkBuffer->curLine[i]).y1;
         if(mD<(lpWorkBuffer->curLine[i]).y2) mD=(lpWorkBuffer->curLine[i]).y2;
         }

     y1=mG;
     y2=mD;

     memset(hntHist,0x00,(unsigned int)(2*MAXSIZE));
     hntHist[0]=x2-x1+1;


      lByte=x1/8;
      rL   =x1%8;
      if(rL!=0) {lByte++;}

      rByte=x2/8;
      rR   =x2%8;
      if(rR!=7) {rByte--;}

      for(yy=y1;yy<=y2;yy++) {
         yD=yy;
         yD=(lpWorkBuffer -> hightImg)-1-yD;
         nY=(LONG)yD*(LONG)w_bt;
         hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(lByte-1);

         curInd=1;
         if(rL!=0) {
              curInd=9-rL;
              if((*hptr)<255) {
                  mask=LEFT_BIT;
                  mask >>=rL;
                  for(kk=1;kk<curInd;kk++) {
                      ch=(~hptr[0])&mask;
                      if(ch!=0) {
                           hntHist[kk]++;
                           }
                      mask >>=1;
                      }
                  }
               }

         for(j=lByte;j<=rByte;j++) {
              hptr+=1L;
              if((*hptr)<255) {
                  mask=LEFT_BIT;
                  for(kk=0;kk<8;kk++) {
                      ch=(~hptr[0])&mask;
                      if(ch!=0) {
                           hntHist[curInd]++;
                           }
                      curInd++;
                      mask >>=1;
                      }
                  }
              else {curInd+=8;}
              }

           if(rR!=7) {
              hptr+=1L;
              if((*hptr)<255) {
                  mask=LEFT_BIT;
                  for(kk=0;kk<=rR;kk++) {
                      ch=(~hptr[0])&mask;
                      if(ch!=0) {
                           hntHist[curInd]++;
                           }
                      curInd++;
                      mask >>=1;
                      }
                  }
              }
         }
    }/* of histV()*/
 int segFixedPitch2(
  WORD wDpiX,                /*X-resolution                  */
  WORD wDpiY,                /*Y-resolution                  */
  int * pH,              /*pointer to the line histogram */
  int n,                     /* size of histogram            */
  int * pCw,             /* pointer to cell width        */
  int * pCut,            /* pointer to the sequence of
                                x-coordinates of cuts        */
  int * pNCut,            /* pointer to number of cuts    */
  long * pCost,          /* pointer to cost of partition */
  int version,                /* version number (temporary)   */
  int segmMode
  ){
   int err;
   int cw;
   int i;
   int cwMin;
   int cwMax;
   long cwMaxL;
   long c;
   long cx;
   long x;
   long x2;
   long a;
   int cost;
   int cwM;
   int n1;
   int * pThist;


   if(n > LENLINE) return(-1);
   n1 = n-1;
   pThist = &thist[0];

   if(version == 0) {
  	thist[0] = *pH + *(pH+1L);
   	for(i=1; i<=n1-1; i++)  thist[i] = *(pH+(long)(i-1)) +
                                  	    *(pH+(long)i) +
                                   	   *(pH+(long)(i+1));
  	thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
   }
   if(version > 0) {
  	thist[0] = *pH + *(pH+1L);
  	thist[0] = *pH + *(pH+1L) + *(pH+2L);
      thist[0] = *pH + *(pH+1L) + *(pH+2L) + *(pH+3L);
   	for(i=2; i<=n1-2; i++)  thist[i] = *(pH+(long)(i-2)) +
   	                                   *(pH+(long)(i-1)) +
                                  	   *(pH+(long)i) +
                                   	   *(pH+(long)(i+1)) +
                                   	   *(pH+(long)(i+2));
     thist[n1] = *(pH+(long)(n1-3)) + *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
     thist[n1] = *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
     thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
   }

   if(*pCw == 0)
     {
//      cwMin = 4;
//      cwMax = 50;
	   if(segmMode == 1){
	         cwMin = 24;
	         cwMax = 70;
//	         cwMin = 54;
//	         cwMax = 90;

	   }else{
         cwMin = 18;
         cwMax = 60;
	   }
//printf("------------cwMin= %d, cwMax = %d\n", cwMin, cwMax);

      cwMaxL = (long)cwMax;

      for(cw=cwMin; cw<=cwMax; cw++)
         {err =
            segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
              if(err<0) return(err);
          cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
         }
      for(cw=1; cw<cwMin; cw++) cA[cw] = cA[cwMin];

  //    piCost = (int far *)(pCost + 1L);
  //    for(cw=1; cw<=cwMax; cw++)
  //       *(piCost+(long)cw) = cA[cw];
  //    *(piCost+(long)(cwMax+1)) = cwMin;

      c = 0L; cx = 0L;
      for(cw=1; cw<=cwMax; cw++)
         {c = c + (long)cA[cw];
          cx = cx + (long)cw*(long)cA[cw];
         }
      x = cwMaxL*(cwMaxL + 1L)/2;
      x2 = (cwMaxL*cwMaxL - 1L);
      a = 12*(cwMaxL*cx - c*x)/(cwMaxL*x2);

      cost = 32000; cwM = cwMin;
      for(cw=1; cw<=cwMax; cw++)
         {cA[cw] = (int)((cwMaxL*cA[cw] - a*cw)/cwMaxL);
          if((cA[cw] < cost)&&(cw>cwMin)) {cost = cA[cw]; cwM = cw;}
         }

      *pCw = cwM;
     }

   else
     {cw = *pCw;
      if(version == 2) cw = (cw*4+3)/6;
      cwMin = (int)(9L*cw/10);
      cwMax = (int)(11L*cw/10);
      cwMaxL = (long)cwMax;

      for(cw=cwMin; cw<=cwMax; cw++)
         {err =
            segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
              if(err<0) return(err);
          cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
         }

      cost = 32000; cwM = cw;
      for(cw=cwMin; cw<=cwMax; cw++)
         {if(cA[cw] < cost) {cost = cA[cw]; cwM = cw;}
         }

      *pCw = cwM;


     }
//     printf("------------cwM= %d, version = %d\n", cwM, version);

   err = segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cwM, pCut, pNCut, pCost, version);
           if(err<0) return(err);

   return(0);
  }
 int segFixedPitch3(
  WORD wDpiX,                /*X-resolution                  */
  WORD wDpiY,                /*Y-resolution                  */
  int * pH,              /*pointer to the line histogram */
  int n,                     /* size of histogram            */
  int * pCw,             /* pointer to cell width        */
  int * pCut,            /* pointer to the sequence of
                                x-coordinates of cuts        */
  int * pNCut,            /* pointer to number of cuts    */
  long * pCost,          /* pointer to cost of partition */
  int version                /* version number (temporary)   */
  ){
   int err;
   int cw;
   int i;
   int cwMin;
   int cwMax;
   long cwMaxL;
   long c;
   long cx;
   long x;
   long x2;
   long a;
   int cost;
   int cwM;
   int n1;
   int * pThist;


   if(n > LENLINE) return(-1);
   n1 = n-1;
   pThist = &thist[0];

   if(version == 0) {
  	thist[0] = *pH + *(pH+1L);
   	for(i=1; i<=n1-1; i++)  thist[i] = *(pH+(long)(i-1)) +
                                  	    *(pH+(long)i) +
                                   	   *(pH+(long)(i+1));
  	thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
   }
   if(version > 0) {
  	thist[0] = *pH + *(pH+1L);
  	thist[0] = *pH + *(pH+1L) + *(pH+2L);
      thist[0] = *pH + *(pH+1L) + *(pH+2L) + *(pH+3L);
   	for(i=2; i<=n1-2; i++)  thist[i] = *(pH+(long)(i-2)) +
   	                                   *(pH+(long)(i-1)) +
                                  	   *(pH+(long)i) +
                                   	   *(pH+(long)(i+1)) +
                                   	   *(pH+(long)(i+2));
     thist[n1] = *(pH+(long)(n1-3)) + *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
     thist[n1] = *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
     thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
   }

   if(*pCw == 0)
     {
//      cwMin = 4;
//      cwMax = 50;

      cwMin = 24;
      cwMax = 70;

      cwMaxL = (long)cwMax;

      for(cw=cwMin; cw<=cwMax; cw++)
         {err =
            segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
              if(err<0) return(err);
          cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
         }
      for(cw=1; cw<cwMin; cw++) cA[cw] = cA[cwMin];

  //    piCost = (int far *)(pCost + 1L);
  //    for(cw=1; cw<=cwMax; cw++)
  //       *(piCost+(long)cw) = cA[cw];
  //    *(piCost+(long)(cwMax+1)) = cwMin;

      c = 0L; cx = 0L;
      for(cw=1; cw<=cwMax; cw++)
         {c = c + (long)cA[cw];
          cx = cx + (long)cw*(long)cA[cw];
         }
      x = cwMaxL*(cwMaxL + 1L)/2;
      x2 = (cwMaxL*cwMaxL - 1L);
      a = 12*(cwMaxL*cx - c*x)/(cwMaxL*x2);

      cost = 32000; cwM = cwMin;
      for(cw=1; cw<=cwMax; cw++)
         {cA[cw] = (int)((cwMaxL*cA[cw] - a*cw)/cwMaxL);
          if((cA[cw] < cost)&&(cw>cwMin)) {cost = cA[cw]; cwM = cw;}
         }

      *pCw = cwM;
     }

   else
     {cw = *pCw;
      if(version == 2) cw = (cw*4+3)/6;
      cwMin = (int)(9L*cw/10);
      cwMax = (int)(11L*cw/10);
      cwMaxL = (long)cwMax;

      for(cw=cwMin; cw<=cwMax; cw++)
         {err =
            segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
              if(err<0) return(err);
          cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
         }

      cost = 32000; cwM = cw;
      for(cw=cwMin; cw<=cwMax; cw++)
         {if(cA[cw] < cost) {cost = cA[cw]; cwM = cw;}
         }

      *pCw = cwM;


     }
 //    printf("------------cwM= %d, version = %d\n", cwM, version);
   err = segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cwM, pCut, pNCut, pCost, version);
           if(err<0) return(err);

   return(0);
  }

int segFixedPitch(
 WORD wDpiX,                /*X-resolution                  */
 WORD wDpiY,                /*Y-resolution                  */
 int * pH,              /*pointer to the line histogram */
 int n,                     /* size of histogram            */
 int * pCw,             /* pointer to cell width        */
 int * pCut,            /* pointer to the sequence of
                               x-coordinates of cuts        */
 int * pNCut,            /* pointer to number of cuts    */
 long * pCost,          /* pointer to cost of partition */
 int version                /* version number (temporary)   */
 ){
  int err;
  int cw;
  int i;
  int cwMin;
  int cwMax;
  long cwMaxL;
  long c;
  long cx;
  long x;
  long x2;
  long a;
  int cost;
  int cwM;
  int n1;
  int * pThist;


  if(n > LENLINE) return(-1);
  n1 = n-1;
  pThist = &thist[0];

  if(version == 0) {
 	thist[0] = *pH + *(pH+1L);
  	for(i=1; i<=n1-1; i++)  thist[i] = *(pH+(long)(i-1)) +
                                 	    *(pH+(long)i) +
                                  	   *(pH+(long)(i+1));
 	thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
  }
  if(version > 0) {
 	thist[0] = *pH + *(pH+1L);
 	thist[0] = *pH + *(pH+1L) + *(pH+2L);
     thist[0] = *pH + *(pH+1L) + *(pH+2L) + *(pH+3L);
  	for(i=2; i<=n1-2; i++)  thist[i] = *(pH+(long)(i-2)) +
  	                                   *(pH+(long)(i-1)) +
                                 	   *(pH+(long)i) +
                                  	   *(pH+(long)(i+1)) +
                                  	   *(pH+(long)(i+2));
    thist[n1] = *(pH+(long)(n1-3)) + *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
    thist[n1] = *(pH+(long)(n1-2)) + *(pH+(long)(n1-1)) + *(pH+(long)n1);
    thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);
  }

  if(*pCw == 0)
    {
     cwMin = 4;
     cwMax = 50;
//     cwMin = 20;
//     cwMax = 60;

     cwMaxL = (long)cwMax;

     for(cw=cwMin; cw<=cwMax; cw++)
        {err =
           segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
             if(err<0) return(err);
         cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
        }
     for(cw=1; cw<cwMin; cw++) cA[cw] = cA[cwMin];

 //    piCost = (int far *)(pCost + 1L);
 //    for(cw=1; cw<=cwMax; cw++)
 //       *(piCost+(long)cw) = cA[cw];
 //    *(piCost+(long)(cwMax+1)) = cwMin;

     c = 0L; cx = 0L;
     for(cw=1; cw<=cwMax; cw++)
        {c = c + (long)cA[cw];
         cx = cx + (long)cw*(long)cA[cw];
        }
     x = cwMaxL*(cwMaxL + 1L)/2;
     x2 = (cwMaxL*cwMaxL - 1L);
     a = 12*(cwMaxL*cx - c*x)/(cwMaxL*x2);

     cost = 32000; cwM = cwMin;
     for(cw=1; cw<=cwMax; cw++)
        {cA[cw] = (int)((cwMaxL*cA[cw] - a*cw)/cwMaxL);
         if((cA[cw] < cost)&&(cw>cwMin)) {cost = cA[cw]; cwM = cw;}
        }

     *pCw = cwM;
    }

  else
    {cw = *pCw;
     if(version == 2) cw = (cw*4+3)/6;
     cwMin = (int)(9L*cw/10);
     cwMax = (int)(11L*cw/10);
     cwMaxL = (long)cwMax;

     for(cw=cwMin; cw<=cwMax; cw++)
        {err =
           segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cw, pCut, pNCut, pCost, version);
             if(err<0) return(err);
         cA[cw] = (int)(((*pCost)*10)/((*pNCut) + 5));
        }

     cost = 32000; cwM = cw;
     for(cw=cwMin; cw<=cwMax; cw++)
        {if(cA[cw] < cost) {cost = cA[cw]; cwM = cw;}
        }

     *pCw = cwM;


    }
//    printf("------------cwM= %d, version = %d\n", cwM, version);
  err = segFixedPitchPartition(wDpiX, wDpiY, pThist, n, cwM, pCut, pNCut, pCost, version);
          if(err<0) return(err);

  return(0);
 }

int segFixedPitchPartition(
WORD wDpiX,                /*X-resolution                  */
WORD wDpiY,                /*Y-resolution                  */
int * pH,              /*pointer to the line histogram */
int n,                     /* size of histogram            */
int cw,                    /* cell width                   */
int * pCut,            /* pointer to the sequence of
                              x-coordinates of cuts        */
int * pNCut,            /* pointer to number of cuts    */
long * pCost,          /* pointer to cost of partition */
int version                /* version number (temporary)   */
)
{
 int i;
 int j;
 int n1;
 long min;
 int cw2;

 if(version==0)
   {n1 = n-1;

    if(cw > n-2)
       {*(pCut+0L) = n-1;
        *(pCut+1L) = 0;
        *pNCut = 2;
        *pCost = (long)((*pH) + *(pH+(long)(n-1)));
        return(0);
       }

    c[0] = (long)(*pH); ind[0] = -1;

    for(i=1; i<=cw+1; i++) {c[i] = *(pH+(long)i)+c[0]; ind[i] = 0;}

    for(i=cw+2; i<=n-1; i++)
       {j = i-cw-1;
        min = c[j]; ind[i] = j++;
        if(c[j]<=min) {min = c[j]; ind[i] = j;} j++;
        if(c[j]<min) {min = c[j]; ind[i] = j;}
       c[i] = min + *(pH+(long)i);
       }

    j = n-cw-2;
    min = c[j]; ind[n1] = j;
    for(j=n-cw-2; j<=n-1; j++) {if(c[j]<min) {min = c[j]; ind[n1] = j;}}
    *pCost = min + *(pH+(long)n1);

    i=0;
    x[i] = n1; *pCut = n1;
    while((j = x[i]) > 0)
         {x[i+1] = ind[j]; *(pCut+(long)(i+1)) = ind[j];
          i++;
         }

    *pNCut = i+1;

    return(0);
   }

 else if(version > 0)
   {n1 = n-1;
    cw2 = cw/3;
    if(version == 2) cw2 = cw/2;

    if(cw > n1-cw2)
       {*(pCut+0L) = n-1;
        *(pCut+1L) = 0;
        *pNCut = 2;
        *pCost = (long)((*pH) + *(pH+(long)(n-1)));
        return(0);
       }

    c[0] = (long)(*pH); ind[0] = -1;

    for(i=1; i<=cw+cw2; i++) {c[i] = *(pH+(long)i) + c[0]; ind[i] = 0;}

    for(i=cw+cw2+1; i<=n1-1; i++)
       {j = i-cw-cw2;
        min = c[j]; ind[i] = j;
        for(j = i-cw-cw2+1; j<=i-cw; j++)
           {if(c[j]<=min) {min = c[j]; ind[i] = j;}}
        for(j = i-cw+1; j<=i-cw+cw2; j++)
           {if(c[j]<=min) {min = c[j]; ind[i] = j;}}
        c[i] = min + *(pH+(long)i);
       }

    j = n1-cw-cw2;
    min = c[j]; ind[n1] = j;
    for(j=n1-cw-cw2; j<=n1-1; j++) {if(c[j]<min) {min = c[j]; ind[n1] = j;}}
    *pCost = min + *(pH+(long)n1);

    i=0;
    x[i] = n1; *pCut = n1;
    while((j = x[i]) > 0)
         {x[i+1] = ind[j]; *(pCut+(long)(i+1)) = ind[j];
          i++;
         }

    *pNCut = i+1;

    return(0);
   }
 else return(-1);
 }

//void saveLog(char *str1, char *str2);
//int lineNumSegm = 0;

int  segFont(
IMAGEL * pImage,       /* pointer to the image          */
int * pH,              /* pointer to the line histogram */
int n,                     /* size of histogram             */
long angle,                /* skew angle of histogram       */
long shift,                /* shift of histogram            */
RECCHARSTRUCT * pRecCharStruct,
                           /* pointer to recognition        */
                           /* parameters structure          */
int * pCw,             /* pointer to cell width         */
int * pCut,            /* pointer to the sequence of
                              X-coordinates of cuts         */
int * pNCut,           /* pointer to number of cuts     */
long * pCost,          /* pointer to cost of partition  */
int * pOut,            /* at least 3100 integers        */
int version,                /* version number (temporary)    */
char *fieldName,
int segmMode
)
{int err;
 int cw;
 int i;
 int j;
 int n1;
 int m;
 int l;
 int cwMin;
 int cwMax, cwMax1;
 long cost;
 int * pThist;
 int thMean;
 int fthMean;
 int thMax;
 int fthMax;
 int fthFlat;
 int fcw;
 long sum;
 int factor;
 int * pMinH;
 int flag;
 int k = 0;
 int h, lh, rh;
 int count;
 long countL;
 LPBYTE pBuffer;
 int width0 = 96;
 int height0 = 96;
 int width;
 int height;
 int * pWidth;
 int * pHeight;
 char char1;
 char * pChar;
 int conf;
 int * pConf;
 long max;
 int minH = 0;
 int maxH = 0;
 int nCut;
 int sw;
 int fsw;

// char buff[500];
// int printFlg = 0;

 if(n > LENLINE) return(-1);

 n1 = n-1;
 if(version != 0) return(-1);
 if(n<5)
    {*(pCut+0L) = n1;
     *(pCut+1L) = 0;
     *pNCut = 2;
     *pCost = 0L;
     return(0);
    }

 pThist = &thist[0];

 thist[0] = *pH + *(pH+1L) + *(pH+2L);
 for(i=1; i<=n1-1; i++)  thist[i] = *(pH+(long)(i-1)) +
                                    *(pH+(long)i)     +
                                    *(pH+(long)(i+1));
 thist[n1] = *(pH+(long)(n1-1)) + *(pH+(long)n1);


 sum = 0;
 thMax = 0;
 for(i=0; i<n; i++)
     {sum = sum + thist[i];
      if(thist[i] > thMax) thMax = thist[i];
     }
 thMean = (int)(sum/n);
 thMax = thMax/3;
 factor = 133;
 fthMean = (int)(((long)thMean*factor + 50)/100L);
 factor = 3*10;
 fthFlat = (int)(((long)thMax*factor + 50)/100L);
 factor = 10;
 fthMax = (int)(((long)thMax*factor + 50)/100L);

 pMinH = &dMaxSeg[0];

 /*    1 - minimum (in nonflat area)
      2 - left and
      3 - right boundaries of flat area
      4 - maximum (in nonflat area)
      5 - inside flat areas (temporary)
      0 - others
 */

 /*  0 - initialization */
 for(i= 0; i<n; i++) {pMinH[i] = 0; ind[i] = 0;}


 /*  flat areas  */
 flag = 0;
 for(i=0; i<n; i++)
    {h = thist[i];
     if(flag == 0)
        {if(h < fthFlat) {flag = 1; k = i;}}
     else
        {if(h >=fthFlat) {flag = 0;
                          if(i-k > fthMax) {pMinH[k] = 2; pMinH[i-1] = 3;}
                         }
        }
    }
 if(flag == 1)
    {if(n-k >fthMax) {pMinH[k] = 2; pMinH[n-1] = 3;}
    }

 /*  marking areas */
 flag = 0;
 for(i= 0; i<n; i++)
    {h = pMinH[i];
     if(flag == 0) {if(h == 2) flag = 1;}
     else          {if(h == 3) flag = 0; else pMinH[i] = 5;}
    }

 /*  maximums   */
 h = thist[0];
 rh = thist[1];
 if(pMinH[0]==0)
    {if(h==rh) {flag = 1; k = 0;}
     else flag = 0;
    }
 else flag = 0;
 for(i=1; i<n1; i++)
    {lh = h; h = rh; rh = thist[i+1];
     if(pMinH[i]==0)
         {if(h>lh) {if(h>rh) pMinH[i] = 4;
                    else if(h==rh) {flag = 1; k = i;}
                   }
          else if(flag==1)
                  {if(h>rh) {flag = 0; pMinH[(k+i)/2] = 4;}
                   else if(h<rh) flag = 0;
                  }
         }
      else flag = 0;
     }
 lh = h;
 h = thist[n1];
 if(pMinH[n1]==0)
    {if(h>lh) pMinH[i] = 4;
     else if(flag==1) pMinH[(k+n1)/2] = 4;
    }

 /*  minimums */
 h = thist[0];
 rh = thist[1];
 if(pMinH[0]==0)
    {if(h==rh) {flag = 1; k = 0;}
     else flag = 0;
    }
 else  flag = 0;
 for(i=1; i<n1; i++)
    {lh = h; h = rh; rh = thist[i+1];
     if(pMinH[i]==0)
         {if(h<lh) {if(h<rh) pMinH[i] = 1;
                    else if(h==rh) {flag = 1; k = i;}
                   }
          else if(flag==1)
                  {if(h<rh) {flag = 0; pMinH[(k+i)/2] = 1;}
                   else if(h>rh) flag = 0;
                  }
         }
      else flag = 0;
     }
 lh = h;
 h = thist[n1];
 if(pMinH[n1]==0)
    {if(h<lh) pMinH[i] = 1;
     else if(flag==1) pMinH[(k+n1)/2] = 1;
    }

 /*  selection   */
 flag = 0;
 for(i=0; i<n; i++)
    {if(pMinH[i]==1)
        {if(thist[i] < fthMean)
            {h = thist[i];
             if(flag==0) {flag =1; k = i; minH = h; maxH = 0;}
             else {if((maxH-minH >= fthFlat)||(maxH-h >= fthFlat))
                       {k = i; minH = h;}
                   else if(h < minH)
                           {pMinH[k] = 0; k = i; minH = h;}
                   }
            }
         else pMinH[i]=0;
        }
      else if(pMinH[i]==4) maxH = (thist[i] > maxH ? thist[i] : maxH);
      else if(pMinH[i]==2) flag = 0;
    }


  /*  0 instead of 4 and 5 */
  for(i=0;i<n; i++)
      {if((pMinH[i]==4)||(pMinH[i]==5)) pMinH[i] = 0;};


  /* boundaries */
  if(pMinH[0]==0) pMinH[0] = 1;
  if(pMinH[n1]==0) pMinH[n1] = 1;

 /*   cw estimation */
  if(*pCw == 0)
      {count = 0;
       sum = 0;
       k = -1;
       if(pMinH[0]==1) k = 0;
       else if(pMinH[0]==3) k = 0;
       for(i=1; i<n1; i++)
          {if(pMinH[i]==1) {count++; sum = sum + i - k; k = i;}
           else if(pMinH[i]==2) {count++; sum = sum + i - k;}
           else if(pMinH[i]==3) k = i;
          }
       if(pMinH[n1]==1) {count++; sum = sum + i - k; k = i;}
       else if(pMinH[n1]==2) {count++; sum = sum + i - k;}

       if(count==0) {*(pCut+0L) = n1;
                     *(pCut+1L) = 0;
                     *pNCut = 2;
                     *pCost = 0L;
                     return(0);
                    }
       cw = 3*n1/(2*count);
       *pCw = cw;
      }
 else {cw = *pCw;}
   cwMin = cw/3;
   cwMax = cw*3; if(cwMax > n1) cwMax = n1;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//   if(strcmp(fieldName, "NUMBERS") == 0 && lineNumSegm == 2){
//	   printFlg = 1;
//	   printTab(pMinH, n1, "pMinH");
//	   sprintf(buff,"cwMin= %d, cwMax= %d\n",cwMin, cwMax);
//	   saveLog("", buff);
//   }

//   {
//   int segmMode = 0;

//   if(strcmp(fieldName, "NUMBERS") == 0 || strcmp(fieldName, "NUMBERS1") == 0
//		   || strcmp(fieldName, "NUMBERSL") == 0 || strcmp(fieldName, "NUMBERSR") == 0) segmMode = 1;
//   if(US_State == 2 && (strcmp(fieldName, "DATEA") == 0 || strcmp(fieldName, "DATEN") == 0)) segmMode = 1;


   if(segmMode == 1)
        if(cwMin < 20) cwMin = 20;

//   }
//   if(printFlg==1){
//	   sprintf(buff,"pMinH[0]= %d, cwMin= %d, cwMax= %d\n",pMinH[0], cwMin, cwMax);
//	   saveLog("", buff);
//   }

 pBuffer = &bufferSeg[0];
 pWidth = &width;
 pHeight = &height;
 pChar = &char1;
 pConf = &conf;

 for(i=0; i<n; i++) c[i] = 0L;

 c[0] = 0L; ind[0] = -1;

 if(pMinH[0]==1)
    {for(i=1; i<=cwMax; i++)
        {if((pMinH[i]==1)||(pMinH[i]==2))
           {width = width0; height = height0;
            err = extractChar(pImage, angle, shift,
                              0, i,
                              pBuffer, pWidth, pHeight);
            if(err < 0) conf = 0;
            else
               {err = recChar(pBuffer, width, height, pImage,
                              pRecCharStruct,
                              pChar, pConf);
                if(err < 0) conf = 0;
               }
            c[i] = (long)conf*i + c[0];
            ind[i] = 0; confM[i] = conf;

//            if(printFlg==1){
//         	   sprintf(buff,"1 pMinH[0]==1; i= %d, conf= %d, c[i]= %ld\n",i, conf, c[i]);
//         	   saveLog("", buff);
//            }

           }
         else if(pMinH[i]==3) break;
        }
    }
 else if(pMinH[0]==2)

//     if(printFlg==1){
//  	   sprintf(buff,"1 pMinH[0]==2;\n");
//  	   saveLog("", buff);
//     }

         for(i=1; i<n; i++)
            {if(pMinH[i]==3)
                {c[i] = 100L*i + c[0];
                 ind[i] = 0; confM[i] = 100; break;

//          if(printFlg==1){
//         	   sprintf(buff,"1 pMinH[0]==3; i= %d, conf= %d, c[i]= %ld\n",i, confM[i], c[i]);
//           	   saveLog("", buff);
//          }

                }

            }

 for(i=cwMin+1; i<=n1-1; i++)
    {if((pMinH[i]==1)||(pMinH[i]==2))
        {max = c[i];
         if(cwMax > i) cwMax1 = i; else cwMax1 = cwMax;
         for(j=1; j<=cwMax1; j++)
            {k = i-j;
             if((pMinH[k]==1)&&(j>=cwMin))
                 {width = width0; height = height0;
                  err = extractChar(pImage, angle, shift,
                                    k, i,
                                    pBuffer, pWidth, pHeight);
                  if(err < 0) conf = 0;
                  else
                     {err = recChar(pBuffer, width, height, pImage,
                                    pRecCharStruct,
                                    pChar, pConf);
                      if(err < 0) conf = 0;
                     }
                  cost = (long)conf*j + c[k];
                  if(cost > max) {max = cost;
                                  ind[i] = k; confM[i] = conf;
                                 }
//              if(printFlg==1){
//              	   sprintf(buff,"2 pMinH[k]==1; k= %d, i= %d, conf= %d, cost= %ld\n",k, i, conf, cost);
//               	   saveLog("", buff);
//                }

                 }
             else if(pMinH[k]==3)
                     {width = width0; height = height0;
                      err = extractChar(pImage, angle, shift,
                                        k, i,
                                        pBuffer, pWidth, pHeight);
                      if(err < 0) conf = 0;
                      else
                         {err = recChar(pBuffer, width, height, pImage,
                                        pRecCharStruct,
                                        pChar, pConf);
                          if(err < 0) conf = 0;
                         }
                      cost = (long)conf*j + c[k];
                      if(cost > max) {max = cost;
                                      ind[i] = k; confM[i] = conf;
                                     }
//                   if(printFlg==1){
//                  	   sprintf(buff,"2 pMinH[k]==3 || 2; k= %d, i= %d, conf= %d, cost= %ld\n",k, i, conf, cost);
//                   	   saveLog("", buff);
//                   }

                      break;
                     }
            }
         c[i] = max;
        }
     else if(pMinH[i]==3)
             {for (j=1; j<=i; j++)
                  {k = i-j;
                   if(pMinH[k]==2)
                      {c[i] = 100L*j + c[k];
                       ind[i] = k; confM[i] = 100; break;
//                if(printFlg==1){
//              	   sprintf(buff,"2 pMinH[i]==3; k= %d, i= %d, conf= %d, c[i]= %ld\n",k, i, confM[i], c[i]);
//               	   saveLog("", buff);
//                }

                      }
                  }
             }
    }

 max = c[n1];
 if(pMinH[n1]==1)
    {for(j=1; j<=cwMax; j++)
        {k = n1-j;
         if(pMinH[k]==1)
            {width = width0; height = height0;
             err = extractChar(pImage,angle, shift,
                               k, n1,
                               pBuffer, pWidth, pHeight);
             if(err < 0) conf = 0;
             else
                {err = recChar(pBuffer, width, height, pImage,
                               pRecCharStruct,
                               pChar, pConf);
                 if(err < 0) conf = 0;
                }
             cost = (long)conf*j + c[k];
             if(cost > max) {max = cost;
                             ind[n1] = k; confM[n1] = conf;
                            }
//             if(printFlg==1){
//           	   sprintf(buff,"3 pMinH[k]==1; k= %d, n1= %d, conf= %d, cost= %ld\n",k, n1, confM[n1], cost);
//            	   saveLog("", buff);
//             }

            }
         else if(pMinH[k]==3)
                 {width = width0; height = height0;
                  err = extractChar(pImage,angle, shift,
                                    k, n1,
                                    pBuffer, pWidth, pHeight);
                  if(err < 0) conf = 0;
                  else
                     {err = recChar(pBuffer, width, height, pImage,
                                    pRecCharStruct,
                                    pChar, pConf);
                      if(err < 0) conf = 0;
                     }
                  cost = (long)conf*j + c[k];
                  if(cost > max) {max = cost;
                                  ind[n1] = k; confM[i] = conf; // ??????? n1
                                 }
//                  if(printFlg==1){
//                	   sprintf(buff,"3 pMinH[k]==3; i= %d, k= %d, n1= %d,  confM[i]= %d, cost= %ld\n",i, k, n1, conf, cost);
//                 	   saveLog("", buff);
//                  }

                  break;
                 }
        }
     c[n1] = max;
    }
 else if(pMinH[n1]==3)
         {for(j=1; j<=n1; j++)
             {k = n1-j;
              if(pMinH[k]==2)
                 {c[n1] = 100L*j + c[k];
                  ind[n1] = k; confM[n1] = 100; break;
//               if(printFlg==1){
//             	   sprintf(buff,"3 pMinH[n1]==3; k= %d, n1= %d, confM[n1]= %d \n", k, n1, confM[n1]);
//              	   saveLog("", buff);
//               }

                 }
             }
         }

 *pCost = c[n1];

 i=0;
 x[i] = n1; cut[0] = n1;
 j = x[i];
 while(j > 0)
      {x[i+1] = ind[j]; cut[i+1] = ind[j];
       i++;
       j = x[i];
      }

 nCut = i+1;
// if(printFlg==1){
// printTab(cut, nCut, "cut");
// }

 sum = 0;
 countL = 0L;
 j = n1;
 for(i=1; i<nCut; i++)
     {k = cut[i];
      if((pMinH[k]==1)||(pMinH[k]==3))
          {conf = confM[j];
           sum = sum + (j - k)*(long)conf;
           countL = countL + conf;
          }
      j = k;
     }
// if(printFlg==1){
//	   sprintf(buff," countL= %ld \n",countL);
//	   saveLog("", buff);
// }

 if(countL==0) {*(pCut+0L) = n1;
                *(pCut+1L) = 0;
                *pNCut = 2;
                *pCost = 0L;
                return(0);
                }
 cw = (int)(sum/countL);
 factor = 30;
 fcw = (int)(((long)cw*factor + 50)/100L);

// if(printFlg==1){
//	   sprintf(buff," cw= %d fcw = %d\n",cw, fcw);
//	   saveLog("", buff);
// }

 // average distance between character in words
 factor = 3;
 sum = 0;
 countL = 0;
 j = n1;
 for(i=1; i<nCut-1; i++)
     {k = cut[i];
      if(pMinH[k]==1) {conf = confM[k];
                       sum = sum + (long)fthMax*(long)conf*factor*3L/4;
                       countL = countL + conf;
                      }
      else if((pMinH[k]==2)&&(i>1))
          {if(j-k < cw/2) {conf = confM[k];
                           sum = sum + (long)(j - k)*(long)conf*factor;
                           countL = countL + conf;
                          }
          }
      j = k;
     }
 if(countL==0) fsw = 0;
 else fsw = (int)(sum/countL);
 sw = (int)((2*fsw + factor)/(2*factor));

 cw = cw + sw;
 *pCw = cw;

// if(printFlg==1){
//	   sprintf(buff," cw= %d sw = %d\n",cw, sw);
//	   saveLog("", buff);
// }

 // spaces
 pCut[0] = n1;
 count = 1;
 j = n1;
 for(i=1; i<nCut-1; i++)
    {k = cut[i];
     if((pMinH[k]==1)||(pMinH[k]==3))
         {pCut[count] = k; count++; j = k;}
     else if(pMinH[k]==2)
             {if((j-k+1 <= fsw)||(j-k+1 <= fcw)
               ||(j-cut[i+1] < cw+sw))
                 {pCut[count-1] = (k+j)/2; j = (k+j)/2;}
              else if(j-k < 3*cw/2)
                 {pCut[count] = k; count++; j = k;}
              else
                  {m = (j-k)/cw;
                   for(l=m-1; l>=0; l--)
                       {pCut[count] = k + (int)((long)(j-k)*l)/m;
                        count++;
                       }
                    j = k;
                  }
             }
    }
  pCut[count] = 0;



//  if(printFlg==1){
// 	   sprintf(buff," *pNCut= %d\n",*pNCut);
// 	   saveLog("", buff);
// 	   printTab(pCut, *pNCut, "pCut");
//  }

 *pNCut = count+1;

 for(i=0; i<n; i++)
     {if(pMinH[i]==1)      pOut[i] = 350;
      else if(pMinH[i]==2) pOut[i] = 250;
      else if(pMinH[i]==3) pOut[i] = 200;
      else                 pOut[i] = thist[i];
     }

// if(strcmp(fieldName, "NUMBERS") == 0){
//     lineNumSegm++;
// }

 return(0);
}


int skipTrash(int *hntHist, int k) {
	int idx = k;
	int i;
    int  max, cc;
    int thX = 5, thY = 5;
	cc = 0;max = 0;
	for(i=k;i<hntHist[0];i++){
	    if(hntHist[i] < 1) {idx = i;break;}
		if(hntHist[i] > max) max = hntHist[i];
		  cc++;
		 if(max > thY || cc > thX) break;
	}
	return idx;
}

int getStartChar(int *hntHist, int k){
	int start = k, i, k1;
    for(i=k;i<hntHist[0];i++){
		if(hntHist[i] > 1) {
			k1 = skipTrash(hntHist, i);
			if(k1 == i){
			   start = i;
			   break;
			}
			i = k1;
		}
	   }
	return start;
}
int getStopChar(int *hntHist, int k){
	int stop = k, i;
    for(i=k;i<hntHist[0];i++){
		if(hntHist[i] < 1) {stop = i;break;}
	   }
	return stop;
}

int getFirstHistEnd(int *hntHist){
	int  k1, k2;
	  k1 = 0;
	  k2 = 1;
	  k1 = getStartChar(hntHist, k2);
 printf("  getFirstHistEnd; k1= %d \n", k1);
	  if(k1 == 0)
		 return -1;
	  k2 = getStopChar(hntHist, k1);
printf("  getFirstHistEnd; k2= %d \n", k2);
	  if(k2 <= k1)
		 return -1;
	  k1 = getStartChar(hntHist, k2);
 printf("  getFirstHistEnd; k1= %d \n", k1);
	  if(k2 >= k1)
		 return -1;
	  return k1-2;
}

int getNextHistEnd(int *hntHist, int beg){
int  k1, k2;
  k1 = 0;
  k2 = beg;

  k1 = getStartChar(hntHist, k2);
printf("  getNextHistEnd; k1= %d \n", k1);
  if(k1 == 0)
	 return -1;
  k2 = getStopChar(hntHist, k1);
printf("  getNextHistEnd; k2= %d \n", k2);
  if(k2 <= k1)
	 return -1;
  k1 = getStartChar(hntHist, k2);
printf("  getNextHistEnd; k1= %d \n", k1);
  if(k2 >= k1)
	 return -1;
  k2 = getStopChar(hntHist, k1);
 printf("  getNextHistEnd; k2= %d \n", k2);
  if(k2 <= k1)
	 return -1;
  k1 = getStartChar(hntHist, k2);
 printf("  getNextHistEnd; k1= %d \n", k1);
  if(k2 >= k1)
	 return -1;
  return k1-2;
}

void cleanHistogram(int *hntHist, int thX, int thY){
	int i, start, k;
    int j, max, cc;
	start = 0;
	for(i=1;i<hntHist[0];i++){
		if(hntHist[i] > 0 && start == 0) {
			start = i;
			cc = 0;max = 0;
			for(j=start;j<hntHist[0];j++){
				if(hntHist[j] < 1) break;
				if(hntHist[j] > max) max = hntHist[j];
				cc++;
				if(max > thY) break;
			}
			if(cc < thX && max < thY){
				for(j=0;j<cc;j++)
					hntHist[j+start] = 0;
			}
			k = start;
			for(j=start;j<hntHist[0];j++){
				if(hntHist[j] < 1) {k=j;break;}
			}
			i = k;
			start = 0;
		}
	}
}

int getMassFromTheSegment(int *hist, int hLen, int cL, int cR){
	int mass = 0, i;
	for(i=cL+1;i<cR;i++)
		mass += hist[i];
	return mass;

}
int checkConfidence(STRUCTWB * lpWorkBuffer, IMAGEL *pImage, RECCHARSTRUCT * pRecCharStruct, int begHOtp,
		            int *hist, int hLen, int *markC, int indxM,  int *countG, int *countAll){
	int i, i1, i2, err, mass;
	long angle = 0;
	long shift = 0;
	LPBYTE pBuffer;
	int width0 = 96;
	int height0 = 96;
	int width;
	int height;
	int * pWidth;
	int * pHeight;
	char char1;
	char * pChar;
	int conf, tConf, cc, ccGood;
	int * pConf;
	int  TH = 20;

    pBuffer = &bufferSeg[0];
    pWidth = &width;
    pHeight = &height;
    pChar = &char1;
    pConf = &conf;

//   printf("------------SegmChars2;       indxM= %d\n",indxM);

    for(i=0;i<indxM;i++) {
          markC[i]+=((lpWorkBuffer->curLine[0]).x1+begHOtp-1);
     }
    tConf = 0;
    cc = 0;
    ccGood = 0;

    for(i=0;i<indxM-1;i++) {
          i1=indxM-i-1;
          i2=indxM-i-2;
          mass = getMassFromTheSegment(hist, hLen, markC[i1], markC[i2]);
          if(mass > 6) {
     	     width = width0; height = height0;
             err = extractChar(pImage, angle, shift,
        		            markC[i1], markC[i2],
                            pBuffer, pWidth, pHeight);
//     printf("1-----------------------------  err = %d, markC[i1]= %d, markC[i2]= %d, pWidth= %d, pHeight= %d \n", err, markC[i1], markC[i2], *pWidth, *pHeight);
             if(err < 0){ conf = 0;
//     printf("1-----------------------------  err = %d, markC[i1]= %d, markC[i2]= %d, pWidth= %d, pHeight= %d, conf= %d \n", err, markC[i1], markC[i2], *pWidth, *pHeight, conf);
             }

             else
                {err = recChar(pBuffer, width, height, pImage,
                            pRecCharStruct,
                            pChar, pConf);
                 if(err < 0) conf = 0;
                }
             tConf += conf;
             cc++;
             if(conf > TH) ccGood++;
//printf("2-----------------------------  i1= %d, i2= %d, markConst[i1]= %d, markConst[i2]= %d, cnf= %d \n", i1, i2, markConst[i1], markConst[i2], conf);
          }

    }

//   printf("-----------------------------  ccGood= %d,  cc= %d\n", ccGood, cc);

   *countG = ccGood;
   *countAll = cc;
   return 0;
}
void replaceMarks(void){
	int i;
	for(i=0;i<indxMarkC2;i++) {
	       markConst[i] = markConst2[i];
	       indxMarkC = indxMarkC2;
	}
}
/*
void printTab(int *pMH, int n, char *str){
	char buff[500];
	int i;
   saveLog(str, "  ------------------------------------------------------------------------------------\n");
	for(i=0;i<n;i++) {
	   if(i%20 == 0) sprintf(buff,"\ni= %d; ",i);
	   sprintf(buff,"%d, ",pMH[i]);
	   saveLog("", buff);
	}
	saveLog("", "------------------------------------------------------------------------------------\n");
}
*/

