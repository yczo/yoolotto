/*
 * UtilRecognition3.c
 *
 *  Created on: Sep 1, 2012
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include  "../headers/UtilRecognition.h"

extern BYTE      mask_R[];
extern BYTE      mask_L[];
extern     int       gr_ar[];
/***************************************************************************
 *									   *
 *  PURPOSE      :fill array arrExc                                        *
 ***************************************************************************/
void arrExcF(int nB)
{
   int i;
     for(i=1;i<255;i++) {
        arrExc[i]=1;
        }
     arrExc[0]  =0;
     arrExc[255]=0;
     switch(nB) {
        case 1:arrExc[3]  =0;
               arrExc[6]  =0;
               arrExc[7]  =0;
               arrExc[12] =0;
               arrExc[14] =0;
               arrExc[15] =0;
               arrExc[24] =0;
               arrExc[27] =0;
               arrExc[28] =0;
               arrExc[30] =0;
               arrExc[31] =0;
               arrExc[48] =0;
               arrExc[51] =0;
               arrExc[54] =0;
               arrExc[55] =0;
               arrExc[56] =0;
               arrExc[59] =0;
               arrExc[60] =0;
               arrExc[62] =0;
               arrExc[63] =0;
               arrExc[96] =0;
               arrExc[99] =0;
               arrExc[102]=0;
               arrExc[103]=0;
               arrExc[108]=0;
               arrExc[110]=0;
               arrExc[111]=0;
               arrExc[112]=0;
               arrExc[115]=0;
               arrExc[118]=0;
               arrExc[119]=0;
               arrExc[120]=0;
               arrExc[124]=0;
               arrExc[126]=0;
               arrExc[127]=0;
               arrExc[192]=0;
               arrExc[195]=0;
               arrExc[198]=0;
               arrExc[199]=0;
               arrExc[204]=0;
               arrExc[206]=0;
               arrExc[207]=0;
               arrExc[216]=0;
               arrExc[219]=0;
               arrExc[220]=0;
               arrExc[222]=0;
               arrExc[223]=0;
               arrExc[224]=0;
               arrExc[227]=0;
               arrExc[230]=0;
               arrExc[231]=0;
               arrExc[236]=0;
               arrExc[238]=0;
               arrExc[239]=0;
               arrExc[240]=0;
               arrExc[243]=0;
               arrExc[246]=0;
               arrExc[247]=0;
               arrExc[248]=0;
               arrExc[251]=0;
               arrExc[252]=0;
               arrExc[254]=0;
               break;
        case 2:
               arrExc[7]  =0;
               arrExc[14] =0;
               arrExc[15] =0;
               arrExc[28] =0;
               arrExc[30] =0;
               arrExc[31] =0;
               arrExc[56] =0;
               arrExc[60] =0;
               arrExc[62] =0;
               arrExc[63] =0;
               arrExc[112]=0;
               arrExc[119]=0;
               arrExc[120]=0;
               arrExc[124]=0;
               arrExc[126]=0;
               arrExc[127]=0;
               arrExc[224]=0;
               arrExc[231]=0;
               arrExc[238]=0;
               arrExc[239]=0;
               arrExc[240]=0;
               arrExc[247]=0;
               arrExc[248]=0;
               arrExc[252]=0;
               arrExc[254]=0;
               break;
        case 3:
               arrExc[15] =0;
               arrExc[30] =0;
               arrExc[31] =0;
               arrExc[60] =0;
               arrExc[62] =0;
               arrExc[63] =0;
               arrExc[120]=0;
               arrExc[124]=0;
               arrExc[126]=0;
               arrExc[127]=0;
               arrExc[240]=0;
               arrExc[248]=0;
               arrExc[252]=0;
               arrExc[254]=0;
               break;
        case 4:
               arrExc[31] =0;
               arrExc[62] =0;
               arrExc[63] =0;
               arrExc[124]=0;
               arrExc[126]=0;
               arrExc[127]=0;
               arrExc[248]=0;
               arrExc[252]=0;
               arrExc[254]=0;
               break;
        case 5:
               arrExc[63] =0;
               arrExc[126]=0;
               arrExc[127]=0;
               arrExc[252]=0;
               arrExc[254]=0;
               break;
        case 6:
               arrExc[127]=0;
               arrExc[254]=0;
               break;
        }
}/* arrExcF() */
void cleanSpeck(
   SREMOVAL  *lpData,    /* structure for removal */
   int        wXBegin,    /* X offset in bitmap (of rectangle area) in pixels */
   int        wYBegin     /* Y offset in bitmap (of rectangle area) in pixels */
)
{
  int x, y, wImg, mask, i;
  LONG lY;
  BYTE  *hptr;
    wImg  =(((lpData->iLineWidth)+31)/32)*4;
                for(i=0;i<indxTr;i++) {
                     x=arrTrace[i][0];
                     y=arrTrace[i][1];
                     y=(lpData->iImgHeight)-1-y;
                     lY=(LONG)y*(LONG)wImg;
                     hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                     mask=LEFT_BIT;
                     mask >>=(x%8);
                     hptr[0]=hptr[0]|mask;
                     }

} /* of cleanSpeck() */
int sizeSpot(SREMOVAL *lpData, int wXBegin, int wYBegin){
  int x, y, xx, yy, wImg, flgB, yUpA = 0, yUpP = 0, yBtA = 0, yBtP = 0, y1 = 0, y2 = 0;
  int sizeX, cc, xmin, xmax, i;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
             y=wYBegin;
             x=wXBegin;
             yy=y-1;
             if(yy>=0) {
                if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),x,yy)) return(0);
                xx=x-1;
                if(xx>=0) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) return(0);
                        }
                xx=x+1;
                if(xx<(lpData->iLineWidth)) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) return(0);
                        }
                }
             indxTr=0;
             sizeX=0;
             xx=x;
             yUpP=yy;
             yBtP=yy;
             while((xx>=0)) {
               yy=y;
               cc=0;
               flgB=0;
               while((yy<(lpData->iImgHeight))) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        if(flgB==0) {
                            if((yy<yUpP-1)||(yy>yBtP+1)) break;
                            flgB=1;
                            yUpA=yy;
                            }
                        cc++;
                        arrTrace[indxTr][0]=xx;
                        arrTrace[indxTr][1]=yy;
                        indxTr++;
                        if(indxTr>=MAXARR) return(0);
                        }
                  else {
                        if(flgB)  {
                            yBtA=yy-1;
                            break;
                            }
                        }
                  if(cc>(lpData->iSpeckSizeV)) return(0);
                  yy++;
                  }
                if(cc>0) {
                        yUpP=yUpA;
                        yBtP=yBtA;
                        if(xx==x) {
                                y1=yUpA;
                                y2=yBtA;
                                }
                        sizeX++;
                        }
                else {
                        break;
                        }
                if(sizeX>(lpData->iSpeckSizeH)) return(0);
                xx--;
                if(y-1>=0) {
                  if(xx-1>=0) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx-1,y-1)) return(0);
                        }
                  }

                }
             xx=x+1;
             yUpP=y1;
             yBtP=y2;
             while(xx<(lpData->iLineWidth)) {
               yy=y;
               cc=0;
               flgB=0;
               while(yy<(lpData->iImgHeight)) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        if(flgB==0) {
                            if((yy<yUpP-1)||(yy>yBtP+1)) break;
                            flgB=1;
                            yUpA=yy;
                            }
                        cc++;
                        arrTrace[indxTr][0]=xx;
                        arrTrace[indxTr][1]=yy;
                        indxTr++;
                        if(indxTr>=MAXARR) return(0);
                        }
                  else {
                        if(flgB)  {
                            yBtA=yy-1;
                            break;
                            }
                        }
                  if(cc>(lpData->iSpeckSizeV)) return(0);
                  yy++;
                  }
                if(cc>0) {
                        yUpP=yUpA;
                        yBtP=yBtA;
                        sizeX++;
                        }
                else {
                        break;
                        }
                if(sizeX>(lpData->iSpeckSizeH)) return(0);
                xx++;
                if(y-1>=0) {
                  if(xx+1<(lpData->iLineWidth)) {
                        if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx+1,y-1)) return(0);
                        }
                  }

                }
        xmax=x;
        xmin=x;
        for(i=0;i<indxTr;i++) {
                if(arrTrace[i][0]<xmin) xmin=arrTrace[i][0];
                if(arrTrace[i][0]>xmax) xmax=arrTrace[i][0];
                }


         yy=y;
         while((yy<=y+(lpData->iSpeckSizeV)) && (yy<(lpData->iImgHeight))) {
               xx=x;
               cc=0;
               flgB=0;
               y1=0;
               while(xx>0) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        flgB=1;
                        cc++;
                        if(cc>(lpData->iSpeckSizeH)) return(0);
                        }
                  else {
                        if(xx<xmin) break;
                        if(flgB==1) break;
                        }
                  xx--;
                  }
               if(cc==0) y1=1;
               xx=x;
               cc=0;
               flgB=0;
               while(xx<(lpData->iLineWidth)) {
                  if(whatIsThisPreP((lpData->lpPointer),wImg,(lpData->iImgHeight),xx,yy)) {
                        flgB=1;
                        cc++;
                        if(cc>(lpData->iSpeckSizeH)) return(0);
                        }
                  else {
                        if(xx>xmax) break;
                        if(flgB==1) break;
                        }
                  xx++;
                  }
               if((y1==1)&&(cc==0)) break;
                yy++;
                }
           return(1);
} /* sizeSpot() */

void setUpL(STRUCTWB *lpWorkBuffer, int iTop, int iBot){
 int y1, indFr, y2, h;

    y1 = (int)((lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH));
    y2 = (int)((lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH) +
               lpWorkBuffer->Height);

    indFr=0;
    h = iTop + y1;
    if(h > y2) h = y2;
    (lpWorkBuffer->line[indFr]).y1[0]      = h;
    h = iBot + y1;
    if(h > y2) h = y2;
    (lpWorkBuffer->line[indFr]).y2[0]      = h;
    (lpWorkBuffer->infFrame[indFr]).nLines = 1;
 } /* setUpL() */

void addLine(STRUCTWB *lpWorkBuffer, int ind){
 int i, indFr, y1;

  indFr=0;
  y1=(int)((lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH));
  if((lpWorkBuffer->infFrame[indFr]).nLines != 1)    {
            tmpLines[ind].lIns = 0;
            return;
            }

  if(maxIndxLines-1+1<MAXLINES) {
    for(i=maxIndxLines-1;i>ind;i--) {
        tmpLines[i+1].indTop = tmpLines[i].indTop;
        tmpLines[i+1].indBot = tmpLines[i].indBot;
        tmpLines[i+1].lIns   = tmpLines[i].lIns;
        }
    tmpLines[ind+1].indTop = (lpWorkBuffer->line[indFr]).y1[0] - y1;
    tmpLines[ind+1].indBot = (lpWorkBuffer->line[indFr]).y2[0] - y1;
    tmpLines[ind+1].lIns   = 1;
    maxIndxLines++;
    (lpWorkBuffer->infFrame[indFr]).nLines  = 0;
    return;
    }
  tmpLines[ind].lIns  =0;
} /* addLine() */
/***************************************************************************
 *  PURPOSE      :searching for posibility of text line existience         *
 *                                                                         *
 ***************************************************************************/
int srchFstEptPl(int *lpInd, int tH){
  int i;
    for(i=0;i<maxIndxLines-1;i++) {
        if(tmpLines[i].lIns) {
                if(tmpLines[i+1].indTop-tmpLines[i].indBot>tH) {
                        *lpInd=i;
                        return(1);
                        }
                }
        }
   return(-1);
} /* srchFstEptPl() */
/***************************************************************************
 *  PURPOSE      :prepacking lines from local to final structure           *
 ***************************************************************************/
void prePackFinL(STRUCTWB *lpWorkBuffer){
 int i, y1, indFr;
    y1=(int)((lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH));

    indFr=0;
    for(i=1;i<maxIndxLines-1;i++) {
        (lpWorkBuffer->line[indFr]).y1[i-1]=tmpLines[i].indTop+y1;
        (lpWorkBuffer->line[indFr]).y2[i-1]=tmpLines[i].indBot+y1;
        }
    (lpWorkBuffer->infFrame[indFr]).nLines=maxIndxLines-2;
 } /* prePackFinL() */

void filtrSmallHLines(STRUCTWB *lpWorkBuffer, int *hist, int lenH){
 int  i, y1, indFr, lT, lB;
 int  k, ccT, ccB;
 long cc;

    y1=(int)((lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH));
    indFr=0;
    i = 0;
    while(1) {
        if(i >= (lpWorkBuffer->infFrame[indFr]).nLines) break;
        lT = (lpWorkBuffer->line[indFr]).y1[i] - y1;
        lB = (lpWorkBuffer->line[indFr]).y2[i] - y1;
        if(lT <= 0)    lT = 1;
        if(lB >= lenH) lB = lenH - 1;
        ccT = lB;
        ccB = lT;
        for(k=lT;k<=lB;k++)
            if(hist[k] > 1) {ccT = k;break;}
        for(k=lB;k>=lT;k--)
            if(hist[k] > 1) {ccB = k;break;}
        cc = 0;
        for(k=lT;k<=lB;k++)
            cc += hist[k];
        k = ccB - ccT;
        if(k < 0) k = 0;
        if((k*5 < lB - lT) || (cc < 10L)) {   //filtration
            for(k=i;k<(lpWorkBuffer->infFrame[indFr]).nLines;k++) {
                   (lpWorkBuffer->line[indFr]).y1[k]  = (lpWorkBuffer->line[indFr]).y1[k+1];
                   (lpWorkBuffer->line[indFr]).y2[k]  = (lpWorkBuffer->line[indFr]).y2[k+1];
                   }
            (lpWorkBuffer->infFrame[indFr]).nLines--;
            }
        else
           i++;
        }
      if((lpWorkBuffer->infFrame[indFr]).nLines == 0)  maxIndxLines = 0;
 } /* filtrSmallHLines() */

int singleLineWh(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight, int startH, int endH){
 int  iTop, iBot, flg, level = 0, penLTY = 0, den, step;
 int  iTopL, iBotL, dd, maxGlb, con34;
 int  i, k, ik, cc, ccGlb, indGlb = 0, diff, endH1;
 long ccL;
 int  p10, minSt, minEnd,  h, k1, k2;
 int  ratio, ratio1;
 int  maxGlbL, indT, indTGlb = 0, iHmax, iH, iHmaxGlb;
 int  lineH1;
 int  den2, five;

   if(lineHeight < TH_C2) return(-1);
   if(lenH < TH_C2) return(-1);
   if(startH < 0) startH = 1;
   if(endH < 0)   endH   = lenH;
   if(endH > lenH)   endH   = lenH;
   den = 5;
   maxGlb = 0;
   con34 = (lineHeight*3)/4;
   step  = lineHeight/8;
   if(step == 0) step = 1;
   if(step > 8)  step = 8;
   for(i=startH;i<endH;i++)
      if(maxGlb < hist[i]) maxGlb = hist[i];

   if(maxGlb < 10) den = 7;
   diff  = lineHeight/10 + 1;
   endH1 = endH;
   iTop = startH;
   for(i=startH;i<endH;i++)
      if(hist[i] > 0) break;
   iTop = i - 3;
   if(iTop < startH) iTop = startH;
   if(iTop > endH - TH_C) return(0);

   iBot = endH1;
   for(i=endH1;i>=startH;i--)
      if(hist[i] > 0) break;

   iBot = i + 3;
   if(iBot > endH1) iBot = endH1;
   if(iBot < startH + TH_C) return(0);

   dd = iBot - iTop;
   if(dd <= lineHeight) {
      iTopL = iTop - (lineHeight - dd)/2;
//      if(iTopL < 0) iTopL = 0;
      if(iTopL < startH ) iTopL = startH;
      iBotL = iBot + (lineHeight - dd)/2;
      if(iBotL > endH1) iBotL = endH1;
      setUpL(lpWorkBuffer,iTopL,iBotL);
      return(1);
      }
maxGlb /= 2;

   ccGlb = -10000;
   i = iTop;
   den2 = den;
   five = den;
   if(lpWorkBuffer -> txtLineVer == 65)  {den2 = 1;five = den + 3;den = 4;}        // 08-07-95
   while(i < iBot) {
       ccL   = 0L;
       flg    = 0;
       if(i + con34 > iBot) break;
       for(k=0;k<lineHeight;k++) {
             ik = i + k;
             if(ik > iBot) {ccL -= (den*maxGlb*(lineHeight - k - 1));break;}
             if(flg) {
               ratio1 = (hist[ik]+hist[ik+1])/2;
               if(ratio1 < level) {
                      ccL -= (penLTY*maxGlb);
                      if(hist[ik] == 0) ccL--;
                      }
               else  {
                 ccL += (long)hist[ik];
                 flg  = 0;
                 }
               }
             else {
                 if(ik > 2) {
                   ratio = (hist[ik-1]+hist[ik-2])/2;
                   level = ratio/den;
                   if(level == 0) level = 1;
                   ratio1 = (hist[ik]+hist[ik+1])/2;
                   if(ratio1 < level) {
                         if(ratio1 == 0) {penLTY = 1;ccL--;}
                         else              penLTY = ratio/(ratio1*den2);
                         if(penLTY > five)    penLTY = five;
                         ccL -= (penLTY*maxGlb);
                         flg  = 1;
                         }
                    else
                         ccL += (long)hist[ik];
                    }
                  }
             }
       cc = (int)((4L*ccL)/lineHeight);
       if(cc > ccGlb) {
             ccGlb = cc;
             indGlb = i;
             }
        i += step;
        }

   if(ccGlb <= -10000) return(0);
   iTopL  = indGlb - lineHeight/3;
   if(iTopL < startH ) iTopL = startH;
   iBotL  = indGlb + lineHeight - 1;
   iBotL += lineHeight/3;
   if(iBotL > endH1) iBotL = endH1;

{
int endFOR;
  iHmaxGlb = 20000;
  lineH1   = lineHeight-2;
  if(lineH1 <= 0) lineH1 = 1;
  endFOR = lineHeight+2;
if(lpWorkBuffer -> txtLineVer == 65)  {
  lineH1   = lineHeight;
  endFOR   = lineHeight;
  }

for(iH=lineH1;iH<=endFOR;iH++) {
   ccGlb = -10000;
   i = iTopL;
   while(i < iBotL) {
       ccL   = 0L;
       flg   = 0;
       if(i + con34 > iBotL) break;
       maxGlbL = maxGlb;
       for(k=0;k<iH;k++) {
             maxGlbL -= 3;
             if(maxGlbL < 10) maxGlbL = 10;
             ik = i + k;
             if(ik > iBotL) {ccL -= (den*maxGlbL*(iH - k - 1));break;}
             if(flg) {
               ratio1 = (hist[ik]+hist[ik+1])/2;
               if(ratio1 < level) {
                      ccL -= (penLTY*maxGlbL);
                      if(hist[ik] == 0) ccL--;
                      }
               else  {
                 ccL += (long)hist[ik];
                 flg  = 0;
                 }
               }
             else {
                 if(ik > 2) {
                   ratio = (hist[ik-1]+hist[ik-2])/2;
                   level = ratio/den;
                   if(level == 0) level = 1;
                   ratio1 = (hist[ik]+hist[ik+1])/2;
                   if(ratio1 < level) {
                         if(ratio1 == 0) {penLTY = 1;ccL--;}
                         else              penLTY = ratio/(ratio1*den2);
                         if(penLTY > five)    penLTY = five;
                         ccL -= (penLTY*maxGlbL);
//                         ccL -= maxGlb;
                         flg  = 1;
                         }
                    else
                         ccL += (long)hist[ik];
                    }
                  }
             }
       cc = (int)((4L*ccL)/iH);
       if(cc > ccGlb) {
             ccGlb = cc;
             indGlb = i;
             }
        i++;
        }
   indT  = indGlb;
   iHmax = hist[indT];
   if(indT > iTopL) indT--;
   iHmax +=hist[indT];
   indT++;
   if(indT < iBotL) indT++;
   iHmax +=hist[indT];

   indT   = indGlb + lineHeight - 1;
   if(indT > iBotL) indT = iBotL;
   iHmax += hist[indT];
   if(indT > 1) indT--;
   iHmax +=hist[indT];
   indT++;
   if(indT < iBotL) indT++;
   iHmax +=hist[indT];
   if(iHmax < iHmaxGlb) {
       indTGlb  = indGlb;
       iHmaxGlb = iHmax;
       }

   }
   indGlb = indTGlb;
   if(ccGlb <= -10000) return(0);
   iTopL  = indGlb;
   iBotL  = indGlb + lineHeight - 1;
}
   p10   = lineHeight/10;
   if(p10 < 3) p10 = 3;
if(lpWorkBuffer -> txtLineVer == 65)  {
   i = iTopL - lineHeight/10;
   if(i<iTop) i = iTop;
   ik = iTopL + lineHeight/10;
   if(i > iBot) i = iBot;
   }
else   {
   i = iTopL - lineHeight/4;
   if(i<iTop) i = iTop;
   ik = iTopL + lineHeight/4;
   if(i > iBot) i = iBot;
   }

   while(i < ik) {
       if(i + con34 > endH1) break;
       minSt = 10000;
       k1    = i - p10;
       k2    = i + p10;
       if(k1 < 0) k1 = 0;
       if(k2 >= iBot) k2 = iBot - 1;
       if(k2 - k1 > 1) {
         for(k=k1;k<k2;k++) {
             h = hist[k] + hist[k+1];
             if(minSt > h) minSt = h;
             }
         minSt *= 10;
         minSt /= 2;
         }

       minEnd = 10000;
       k1     = i + lineHeight - p10;
       k2     = i + lineHeight + p10;
       if(k1 < 0) k1 = 0;
       if(k2 >= iBot) k2 = iBot - 1;
       if(k2 - k1 > 1) {
         for(k=k1;k<k2;k++) {
             h = hist[k] + hist[k+1];
             if(minEnd > h) minEnd = h;
             }
         minEnd *= 10;
         minEnd /= 2;
         }
       else   minEnd = minSt;
       ind[i] = minSt + minEnd;
       i++;
       }
   ccGlb  = 10000;
   indGlb = iTopL;
if(lpWorkBuffer -> txtLineVer == 65)
   i = iTopL - lineHeight/10;
else
   i = iTopL - lineHeight/4;
   if(i<iTop) i = iTop;
   k = 0;
   cc = 0;
   while(i < ik) {
       if(i + con34 > endH1) break;
       if(ind[i] < ccGlb) {ccGlb = ind[i]; indGlb = i;k = 1;cc = 1;}
       else {if(cc && (ind[i] == ccGlb)) k++;else cc = 0;}
       i++;
       }
   ik = iTopL;
   indGlb += (k/2);
   if(ind[indGlb] < ind[iTopL]) ik = indGlb;

   iBotL  = ik + lineHeight;
if(lpWorkBuffer -> txtLineVer == 65)  {
                   p10 /= 2;
                   if(p10 == 0) p10 = 1;
                   }
   minSt  = 10000;
   k1     = ik - p10;
   k2     = ik + p10;
   if(k1 < 0) k1 = 0;
   if(k2 >= iBot) k2 = iBot - 1;
   if(k2 - k1 > 1) {
      for(k=k1;k<k2;k++) {
          h = hist[k] + hist[k+1];
          if(minSt >= h) {minSt = h;ik = k;}
          }
      }
    if(ik+1 < iBot)
      if(hist[ik] == hist[ik+1])  ik++;
    if(ik+1 < iBot)
      if(hist[ik] == hist[ik+1])  ik++;

    iTopL = ik;
    if(iTopL < iTop) iTopL = iTop;
    iTopL--;
    ik     = iBotL;
    minEnd = 10000;
    k1     = ik - p10;
    k2     = ik + p10;
    if(k1 < 0) k1 = 0;
    if(k2 >= iBot) k2 = iBot - 1;
    if(k2 - k1 > 1) {
       for(k=k1;k<k2;k++) {
           h = hist[k] + hist[k+1];
           if(minEnd > h) {minEnd = h;ik = k+1;}
           }
       }
    iBotL = ik;
    if(iBotL > iBot) iBotL = iBot;
    iBotL--;
    setUpL(lpWorkBuffer,iTopL,iBotL);
    return(1);
} /* singleLineWh() */

int  multiLineWh(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight){
   int  i, lTop, lBot, indL, ret, tH1;

     tH1 = lineHeight - lineHeight/3;
     lTop=1;
     for(i=1;i<lenH;i++) {
          if(hist[i]>0) {lTop=i-3;break;}
         }

     lTop=max(1,lTop);
     lBot=lenH-1;
     for(i=lenH-1;i>1;i--) {
          if(hist[i]>0) {lBot=i+3;break;}
         }
     lBot=min(lenH-1,lBot);

     indL=0;
     maxIndxLines=2;

     tmpLines[0].indTop=1;
     tmpLines[0].indBot=1;
     tmpLines[0].lIns  =1;

     tmpLines[1].indTop=lenH;
     tmpLines[1].indBot=lenH;
     tmpLines[1].lIns  =1;

     do  {

        singleLineWh(hist,lenH,lpWorkBuffer,lineHeight, lTop, lBot);
        addLine(lpWorkBuffer, indL);

SRCH:   ret=srchFstEptPl((int *)&indL,tH1);
        if(ret<0) {
           prePackFinL(lpWorkBuffer);
           filtrSmallHLines(lpWorkBuffer, hist, lenH);
           return(0);
           }
        lTop=tmpLines[indL].indBot;
        lBot=tmpLines[indL+1].indTop;
        if(lBot - lTop < tH1) {tmpLines[indL].lIns=0;goto SRCH;}
        } while(1);
    return(0);

  } /* multiLineWh() */

/***************************************************************************
 *  PURPOSE      : search end of line                                      *
 *                                                                         *
 ***************************************************************************/
int endLine(int ind, int len)
{
 int i, min, indmin = 0, j;
       min=30000;
       for(i=ind;i<len;i++) {
            if(hntHist[i]==0) return(i-ind);
            if(min>hntHist[i]) {
                min=hntHist[i];
                indmin=i;
                }
            if(hntHist[i]>10)   {
                for(j=i;j<min(i+15,len);j++)  {
                      if(hntHist[j]==0) return(j-ind);
                      }
                return(indmin-ind);
                }
            }
       return(len-ind-1);
} /* endLine*/



void maxLineH(STRUCTWB *lpWorkBuffer, int nOfFr, int *hist, int lenH ){
  int  j, h, k, lT, lB, y1, dd;
  int  srMax, sr = 0, hMax, iMax;
  long av;

   (lpWorkBuffer-> maxLH) = 0;
   iMax = 0;
   for(j=0;j<(lpWorkBuffer->infFrame[nOfFr]).nLines;j++) {
      h = (lpWorkBuffer->line[nOfFr]).y2[j]-(lpWorkBuffer->line[nOfFr]).y1[j];
      if((lpWorkBuffer-> maxLH) < h) {
                               (lpWorkBuffer-> maxLH) = h;
                               iMax = j;
                               }

      }
   if((lpWorkBuffer-> maxLH) > 40) {
          y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
          srMax = 0;
          hMax  = 0;
          for(j=0;j<(lpWorkBuffer->infFrame[nOfFr]).nLines;j++) {
             lT=(lpWorkBuffer->line[nOfFr]).y1[j]-y1;
             lB=(lpWorkBuffer->line[nOfFr]).y2[j]-y1;
             dd = lB-lT;
             av = 0L;
             for(k=lT;k<lB;k++) {
                  av+=(LONG)hist[k];
                  }
             if(dd>0) {sr=(int)(av/(LONG)dd);}
             if(srMax < sr) {
                  srMax = sr;
                  hMax  = (lpWorkBuffer->line[nOfFr]).y2[j]-(lpWorkBuffer->line[nOfFr]).y1[j];
                  }
             }
          lT=(lpWorkBuffer->line[nOfFr]).y1[iMax]-y1;
          lB=(lpWorkBuffer->line[nOfFr]).y2[iMax]-y1;
          dd = lB-lT;
          av = 0L;
          for(k=lT;k<lB;k++) {
                  av+=(LONG)hist[k];
                  }
          if(dd>0) {sr=(int)(av/(LONG)dd);}
          if(sr*10 < srMax)
                  (lpWorkBuffer-> maxLH) = hMax;
          }

} /* maxLineH() */

void subHist(int *hist, int lenH, int dd, int iBeg, int iEnd){
  int i, iB, iE;

     iB = max(1,iBeg);
     iE = min(lenH,iEnd);
     for(i=iB; i<iE;i++)  {
         hist[i] -= dd;
         }

} /* of subHist() */

/***************************************************************************
 *  PURPOSE      :filtration narrow lines from histogram                   *
 *                                                                         *
 ***************************************************************************/
int filtrHistVert(STRUCTWB *lpWorkBuffer, int *hist, int lenH, int xLI, int ver, int *histH, int lenHH, int action){
  int   i, j, k, x1, x2, y1, y2, indFr;
  int   lL, lR, max, maxS, vv1, lenVl, maxLl, tT, tTh;
  int   gap, flg_hLM, eps, epsTmp;
  float flC2;
  int   dif, maxLlB, dInt, gap2, ret, gap3, hLn, kk;

   flg_hLM = lpWorkBuffer -> horLineMM;
     gap = 3;
     if(xLI<0) {
        xLI++;
        xLI = -xLI;
        gap = 1;
        }
     if(flg_hLM == 1) {
        gap = 10;
        eps = 1;
        }
     if(flg_hLM >= 10) {
        gap = flg_hLM - 10;
        }

/* line enhancment */

 dInt   = 5;
 indFr  = ver;
 maxLlB = 0;
 gap2   = 2*gap + 1;
 gap3   = 2*gap2 - 1;
 ret    = 0;

    if( ((lpWorkBuffer->infFrame[indFr]).nLines == 0) && ((lpWorkBuffer -> heightForVert) == 0) ) {
        return(ret);
        }

 drawHist[0] = *hist;
 for(i=1; i<lenH; i++) drawHist[i] = *(hist+(long)i) - *(hist+(long)(i-1));
 drawHist[lenH] = -(*(hist+(long)(lenH-1)));

 k = dInt;
 for(i=0; i<=k-1; i++)
    {max = -1;
     for(j=1; j<=k; j++)
	{if(i-j>=0)
	    {dif = drawHist[i-j] - drawHist[i+j];
	     if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j;}
	    }
	 if(i-j+1>=0)
	    {dif = drawHist[i-j+1] - drawHist[i+j];
	     if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j-1;}
	    }
	}
    }

 for(i=k; i<=lenH-k; i++)
    {max = -1;
     for(j=1; j<=k; j++)
	{dif = drawHist[i-j] - drawHist[i+j];
	 if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j;}
	 dif = drawHist[i-j+1] - drawHist[i+j];
	 if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j - 1;}
	}
    }
 for(i=lenH-k+1; i<=lenH; i++)
    {max = -1;
     for(j=1; j<=k; j++)
	{if(i+j<=lenH)
	    {dif = drawHist[i-j] - drawHist[i+j];
	     if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j;}
	     dif = drawHist[i-j+1] - drawHist[i+j];
	     if(dif>max) {max = dif; histVTmpT[i] = dif; dd[i] = 2*j - 1;}
	    }
	}
    }

 if(((lpWorkBuffer->infFrame[indFr]).nLines > 0) && ((lpWorkBuffer -> fieldType) >= CHECKBOX)
     && (action == 0)) {
      hLn = (lpWorkBuffer->infFrame[indFr]).nLines*(lpWorkBuffer -> heightForVert);
      }
 else {
      hLn = lpWorkBuffer -> heightForVert;
      }


/* line enhancment */
     tTh=(int)((float)10*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));
     tT=6;
     maxLl=0;
     if(gap == 3) {
       for(i=0;i<(lpWorkBuffer->infFrame[indFr]).nLines;i++) {
          k=(lpWorkBuffer->line[indFr]).y2[i]-(lpWorkBuffer->line[indFr]).y1[i]+1;
          maxLl+=k;
          }
       }
     else {
       for(i=0;i<(lpWorkBuffer->infFrame[indFr]).nLines;i++) {
          k=(lpWorkBuffer->line[indFr]).y2[i]-(lpWorkBuffer->line[indFr]).y1[i]+1;
          if(k>maxLl) {maxLl=k;}

          }
       }
     k = maxLl- hLn;
     if(k < 0) {k = -k;}
     if( hLn && (k > 7) ) {
        maxLl = hLn;
        }
     else {
        maxLl += tT;
        }
     maxLlB = maxLl/3;
     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
     y2=y1+(lpWorkBuffer -> Height)-1;

     x1=(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW);
     x2=x1+(lpWorkBuffer -> Width)-1;

     max=0;
     i = 1;

     while(i < lenH - 1) {
        kk = 1;
        if((histVTmpT[i+1]<=histVTmpT[i])&&(histVTmpT[i-1]<histVTmpT[i])&&(histVTmpT[i]!=0)) {
                max=histVTmpT[i];

                if(hist[i] <= maxLlB) {
                        i++;
                        continue;
                        }

                for(j=i+1;j<=i+dInt;j++) {
                        if(histVTmpT[j] > max) {
                                max=histVTmpT[j];
                                i = j;
                                }
                        }

                dif = dd[i]/2;
                if(hist[i-1] > hist[i]) {i--; kk = 2;}
                lL = i - dif;
                lR = i + dif;

                   maxS=0;
                   vv1=0;
                   k=lL-3;
                   if(k>=0) {
                        vv1=hist[k];
                        }
                   k=lL-2;
                   if(k>=0) {
                        vv1+=hist[k];
                        if(hist[k]!=vv1)  {vv1/=2;}
                        }
                   k=lR+2;
                   if(maxS<vv1) {maxS=vv1;}
                   vv1=0;
                   if(k<lenH) {
                        vv1=hist[k];
                        }
                   k=lR+3;
                   if(k<lenH) {
                        vv1+=hist[k];
                        if(hist[k]!=vv1)   {vv1/=2;}
                        }

                   if(maxS<vv1) {maxS=vv1;}
                   max=hist[i];

                   flC2=(float)0.0;
                   if(maxS==0) {maxS=1;}
                   if(maxS>0) {flC2=(float)max/(float)maxS;}
                   k=0;
                   if(flC2>(float)2.0) {

                        lenVl=isItVertL(lpWorkBuffer,gap,xLI+lL,xLI+lR,y1,y2);
                        if(lenVl>maxLl) {
                             begVertLine=y1;
                             endVertLine=y2;
                             if((lL > 0) && (hist[lL] > 0) && (hist[lL-1] > 0)) {
                                 if(max > 3*hist[lL]) {lL++;}
                                 }
                             if((lR < lenH) && (hist[lR] > 0) && (hist[lR+1] > 0)) {
                                 if(max > 3*hist[lR]) {lR--;}
                                 }
                             if(flg_hLM == 1) {
                               epsTmp = eps;
                               if(lR - lL < 2) eps = 2;
                               lL = max(lL - eps,1);
                               lR = min(lR + eps,lenH - 1);
                               eps = epsTmp;
                               }
                             if(lL <= lR) {
                               if(action == 0)
                                     putWhiteV(lpWorkBuffer,xLI+lL,xLI+lR,begVertLine,endVertLine);
                               if(lenHH > 1)
                                    subHist(histH,lenHH,lR -lL,begVertLine - y1 + 1,endVertLine - y1 + 1);
                               ret = 1;
                               }
                             }
                        else {
                          if(gap<3) {
                             i += kk;
                             continue;
                             }
                          lenVl=outOfL(lpWorkBuffer,xLI+lL,xLI+lR,y1,y2,ver);
                          if(lenVl>tT) {
                               if((lpWorkBuffer->infFrame[indFr]).nLines>1) {
                                   if(action == 0)
                                        putWhiteVBt(lpWorkBuffer,ver,xLI+lL,xLI+lR,y1,y2);
                                   if(lenHH > 1)
                                        subHist(histH,lenHH,lR -lL,1,lenHH);
                                   ret = 1;
                                   }
                               else {
                                    isItVertL(lpWorkBuffer,6,xLI+lL,xLI+lR,y1,y2);
                                    if((begVertLine+1 >= (lpWorkBuffer->line[indFr]).y1[0]) && (endVertLine - 1 <= (lpWorkBuffer->line[indFr]).y2[0]) ) {
                                        putWhiteVBt(lpWorkBuffer,ver,xLI+lL,xLI+lR,y1,y2);
                                        }
                                    else {
                                       maxS = max/3;
                                       if(max - hist[lL] > maxS) {lL++;}
                                       if(max - hist[lR] > maxS) {lR--;}
                                       if(lL <= lR) {
                                         if(action == 0)
                                                 putWhiteV(lpWorkBuffer,xLI+lL,xLI+lR,begVertLine,endVertLine);
                                         if(lenHH > 1)
                                                 subHist(histH,lenHH,lR -lL,begVertLine - y1 + 1,endVertLine - y1 + 1);
                                         ret = 1;
                                         }
                                       }
                                    }
                               }
                          else {

                               if((lenVl>2) && (2*max > maxLl)) {
                                  vv1=1;
                                  k=lL-2;
                                  if(k>=0) {
                                    if(hist[k]>1) {vv1=0;}
                                    }
                                  k=lL-1;
                                  if(k>=0) {
                                    if(hist[k]>1) {vv1=0;}
                                    }
                                  else            {vv1=0;}
                                  k=lR+1;
                                  if(k<lenH) {
                                    if(hist[k]>1) {vv1=0;}
                                    }
                                  else            {vv1=0;}
                                  k=lR+2;
                                  if(k<lenH) {
                                    if(hist[k]>1) {vv1=0;}
                                    }
                                  if(vv1) {
                                    isItVertL(lpWorkBuffer,8,xLI+lL,xLI+lR,y1,y2);
                                    if(action == 0)
                                          putWhiteV(lpWorkBuffer,xLI+lL,xLI+lR,begVertLine,endVertLine);
                                    if(lenHH > 1)
                                          subHist(histH,lenHH,lR -lL,begVertLine - y1 + 1,endVertLine - y1 + 1);
                                    ret = 1;
                                    }
                                  }
                               }
                          }
                        }

                max=0;
                }
        i += kk;

        }
     return(ret);
} /* of filtrHistVert() */
void cpyHist(int *lpHOtp,  int *lpHInp, int lenH)
{
 int i;
    for(i=0;i<lenH;i++)
    lpHOtp[i] = lpHInp[i];
}
void  cleanHist(int *lpH, int len)
{
 int i;
    for(i=1;i<len;i++)
    if(lpH[i] < 3) lpH[i] = 0;
   for(i=4;i<len - 4;i++)
    if(lpH[i] == 3) {
      if((lpH[i-1] == 0) && (lpH[i-2] == 0)  &&
         (lpH[i+1] == 0) && (lpH[i+2] == 0) )  lpH[i] = 0;
      }
}
/***************************************************************************
 *  PURPOSE      :recognize lines from histogram                           *
 ***************************************************************************/
int linesUnp(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight){
   int  i, lTop, lBot, kk, aV = 0, baV, indL, ret, tH, aVT = 0, dif, res, aVLn;
   int  maxCC, tHDpi, countF, mm;
   LONG aVl;
     tH=(int)((float)15*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));
     tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0))-2;
     lTop=1;
     for(i=1;i<lenH;i++) {
          if(hist[i]>0) {lTop=i-1;break;}
         }
     lTop=max(1,lTop);
     lBot=lenH-1;
     for(i=lenH-1;i>1;i--) {
          if(hist[i]>0) {lBot=i+1;break;}
         }
     lBot=min(lenH-1,lBot);

     indL=0;
     maxIndxLines=2;

     tmpLines[0].indTop=1;
     tmpLines[0].indBot=1;
     tmpLines[0].lIns  =1;

     tmpLines[1].indTop=lenH;
     tmpLines[1].indBot=lenH;
     tmpLines[1].lIns  =1;
     if((lineHeight<tHDpi)&&(lineHeight>0)) {return(0);}
     aVLn=0;
     countF=0;
     do  {
        maxIndxLines1=0;
        kk=lBot-lTop;
        aVl=0L;
        mm = 0;
        for(i=lTop;i<lBot;i++) {
           aVl+=(LONG)hist[i];
           if(mm < hist[i])
                    mm = hist[i];
           }
        if(kk>0) {
               aV=(int)(aVl/(LONG)kk);
               if((mm < 14) && (aV >= 4))
                                mm = 1;
               else
                                mm = 0;
               }
        aVl=0L;
        maxCC=0;
        kk=0;
        for(i=lTop;i<lBot;i++) {
           if(hist[i]>aV) {
                aVl+=(LONG)hist[i];
                kk++;
                }
           else {
                if(kk>0) {
                        if(maxCC<kk) {maxCC=kk;aVT=(int)(aVl/(LONG)kk);}
                        kk=0;aVl=0L;
                        }
                }
           }
         if(kk>0) {
                if(maxCC<kk) {maxCC=kk;aVT=(int)(aVl/(LONG)kk);}
                }
        if(maxCC>tHDpi) {aV=aVT-aVT/10;if((aVT%10)>4) {aV--;}}
        baV=aV/5;
        if(aV<15) {aV*=4;}
        else      {aV=2*aV+aV/2;}
        kk=0;
        aVl=0L;
        for(i=lTop;i<lBot;i++) {
           if((hist[i]<aV)&&(hist[i]>baV)) {aVl+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {aV=(int)(aVl/(LONG)kk);}
        baV=aV/5;
        if(aV<15) {aV*=4;}
        else      {aV=2*aV+aV/2;}
        aVT=aV;
        kk=0;
        aVl=0L;
        for(i=lTop;i<lBot;i++) {
           if((hist[i]<aV)&&(hist[i]>baV)) {aVl+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {aV=(int)(aVl/(LONG)kk);}
          baV=aV-aV/10;
          if((aV%10)>4) {baV--;}
          if(aV<20)     {baV--;}
          if(aV<10)     {baV--;}
          kk=0;
          dif=0;
          maxCC=lTop;
          if(aVT<10) {aVT=20;}
          for(i=lBot-1;i>lTop;i--) {
           if((hist[i]<aVT)&&(hist[i]>baV)) {
                maxCC=i;
                break;
                }
           }

          for(i=lTop;i<lBot;i++) {
           if((hist[i]<aVT)&&(hist[i]>baV)) {
                kk++;
                }
           if(kk&&(i<maxCC)) {dif+=abs(hist[i]-hist[i+1]);}
           }
          if(kk<5) {goto OMT;}
          aVT=aV;
          res=aV/10;
          if((aV%10)>4) res++;

          if(dif>kk/4) {
                aVT-=res;
                }
          if(dif>kk/2) {
                aVT-=res;
                }
          if(dif>kk) {
                aVT-=res;
                if(aVT<14) {
                    aVT=aVT/2-1;
                    if(aVT<8) {aVT--;}
                    }
                else {
                   if(dif>3*kk) {
                        aVT-=(aVT/3);
                        }
                   if(dif>5*kk) {
                        aVT-=(aVT/4);
                        }

                    }
                }
          aVT=max(0,aVT);

          aV=aVT;

          if(mm && (aV > 2)) {
             if(lBot - lTop <30)
                     aV /= 2;
             else
                     aV--;
                     }

         if(lBot - lTop > 200)
                      if(aV > 7)
                           aV -= 2;

        linLoc(lpWorkBuffer,hist,lTop,lBot,lineHeight,aV);

OMT:    countF++;
        if(maxIndxLines1==0) {
           if(countF>1) {tmpLines[indL].lIns=0;}
           }
        else {
           shiftBord(lpWorkBuffer,hist,lTop,lBot,lineHeight,aV);
           for(i=0;i<maxIndxLines1; i++) {
              kk=0;
              if((lineHeight<=0)||(lineHeight>=1000)) {
                ret=srchB(hist,lenH,lTop,lBot,tmpLines1[i].indTop,1);
                if(ret>0) {
                     tmpLines1[i].indTop=ret;
                  }
                else {kk=1;}
                }
              if(tmpLines1[i].indBot-tmpLines1[i].indTop>=tHDpi) {
                 if((lineHeight<=0)&&(lineHeight>=1000)) {
                    ret=srchB(hist,lenH,lTop,lBot,tmpLines1[i].indBot,-1);
                    if(ret>0) {
                            tmpLines1[i].indBot=ret;
                        if(tmpLines1[i].indBot-tmpLines1[i].indTop<tHDpi) {kk=1;}
                        }
                    else {kk=1;}
                    }
                 }
               else {kk=1;}
               if(kk) {
                       for(kk=i;kk<maxIndxLines1-1; kk++) {
                                tmpLines1[kk].indBot=tmpLines1[kk+1].indBot;
                                tmpLines1[kk].indTop=tmpLines1[kk+1].indTop;
                                }
                        maxIndxLines1--;
                        if(maxIndxLines1==0) {tmpLines[indL].lIns=0;}
                        i--;
                        }
               }

           if(aVLn==0) {
             aVT=0;kk=0;
             for(i=0;i<maxIndxLines1; i++) {
                aVT+=tmpLines1[i].indBot-tmpLines1[i].indTop;
                kk++;
                }
             if(kk>0) {aVLn=aVT/kk;}
             }
           if(aVLn>0) {
             for(i=0;i<maxIndxLines1; i++) {
                if(((float)(tmpLines1[i].indBot-tmpLines1[i].indTop)/(float)aVLn)<(float)0.6) {
                        for(kk=i;kk<maxIndxLines1-1; kk++) {
                                tmpLines1[kk].indBot=tmpLines1[kk+1].indBot;
                                tmpLines1[kk].indTop=tmpLines1[kk+1].indTop;
                                }
                        maxIndxLines1--;
                        if(maxIndxLines1==0) {tmpLines[indL].lIns=0;}
                        }
                }
              }
           prePackL(indL);
           }

        ret=srchFstEptPl((int *)&indL,tH);
        if(ret<0) {
           prePackFinL(lpWorkBuffer);
           return(0);
           }
        lTop=tmpLines[indL].indBot;
        lBot=tmpLines[indL+1].indTop;
        lTop++;
        lBot--;
        ret=srchB(hist,lenH,lTop,lBot,lTop,1);
        if(ret>0) {lTop=ret;}
        ret=srchB(hist,lenH,lTop,lBot,lBot,-1);
        if(ret>0) {lBot=ret;}
        if(lBot-lTop<tH) {
                 maxIndxLines1=0;
                 goto OMT;
                 }

        } while(1);
    return(0);

  } /* linesUnp() */
/***************************************************************************
 *  PURPOSE      : vertical histogram,                                     *
 *                                                                         *
 ***************************************************************************/
void histVert(STRUCTWB *lpWorkBuffer ){
  int  j, kk, curInd, yy;
  int x1, y1, x2, y2, yD, ch;
  int lByte, rByte, rL, rR, w_bt;
  LONG nY;
  BYTE  *hptr;
  BYTE mask;

    w_bt=(lpWorkBuffer -> widthImg);

    x1=(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW);
    x2=x1+(lpWorkBuffer -> Width)-1;
    y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
    y2=y1+(lpWorkBuffer -> Height)-1;

    memset((int *)ind,0x00,(unsigned int)(2*MAXPROFILE));
    ind[0]=x2-x1+1;
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
                          ind[kk]++;
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
                          ind[curInd]++;
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
                          ind[curInd]++;
                          }
                     curInd++;
                     mask >>=1;
                     }
                 }
             }
        }
   }/* of histVert()*/
int outOfL(STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB, int ver){
    int   yn, k, cc, flgIns;
    int   nn, indFr, yBeg, yEnd;
    int   x1, x2, flgOut;
  int   ynD, w_bt;
  LONG nY;
  BYTE  *hptr;
  BYTE  *hptrL;
  BYTE  *hptrR;
  BYTE maskL, maskR, maskM;
  BYTE maskLL, maskRL;
  BYTE maskLR, maskRR;
  int  nL, nR, bL, bR, lB, i, rI, rI1;
  int  nLL, nRL, bLL, bRL, lBL, xLL, xRL;
  int  nLR, nRR, bLR, bRR, lBR, xLR, xRR;

        w_bt=(lpWorkBuffer -> widthImg);
        x1=(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW);
        x2=x1+(lpWorkBuffer -> Width)-1;
        indFr=ver;
        nn=(lpWorkBuffer->infFrame[indFr]).nLines;
        if(nn<=0) return(0);

        cc=0;
        yEnd=(lpWorkBuffer->line[indFr]).y1[0];
        ynD=yT;
          ynD=(lpWorkBuffer -> hightImg)-1-ynD;
          nY = (LONG)ynD*(LONG)w_bt;

       bL = xL/8;
       bR = xR/8;
       nL = xL%8;
       nR = xR%8;
       nR = 7 - nR;
       maskL = mask_L[nL];
       maskM = 255;
       maskR = mask_R[nR];
       if(bL == bR) {
          maskL &=mask_R[nR];
          }
       lB = bR - bL + 1;
       rI  = lB - 2;
       rI1 = rI + 1;
       hptr = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bL;
       if(lB>2) {lB = 3;}

       xLL = xL - 6;
       xRL = xL - 2;
       bLL = xLL/8;
       bRL = xRL/8;
       nLL = xLL%8;
       nRL = xRL%8;
       nRL = 7 - nRL;
       maskLL = mask_L[nLL];
       maskRL = mask_R[nRL];
       if(bLL == bRL) {
          maskLL &=maskRL;
          }
       lBL   = bRL - bLL + 1;
       hptrL = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLL;
       if(lBL>2) {lBL = 3;}

       xLR = xR + 2;
       xRR = xR + 6;
       bLR = xLR/8;
       bRR = xRR/8;
       nLR = xLR%8;
       nRR = xRR%8;
       nRR = 7 - nRR;

       maskLR = mask_L[nLL];
       maskRR = mask_R[nRL];
       if(bLR == bRR) {
          maskLR &=maskRR;
          }

       lBR   = bRR - bLR + 1;
       hptrR = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLR;
       if(lBR>2) {lBR = 3;}
       switch(lB) {
         case 1:
                for(yn=yT;yn<yEnd-1;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }

                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 2:
                for(yn=yT;yn<yEnd-1;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                     if( (~(*(hptr + 1L))) & maskR) {flgIns=1;}
                        }
                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 3:
                for(yn=yT;yn<yEnd-1;yn++) {
                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                          for(i=1;i<=rI;i++) {
                             if( (~(*(hptr + (long)i))) & maskM) {flgIns=1;break;}
                             }
                          }
                     if(!flgIns) {
                          if( (~(*(hptr + (long)rI1))) & maskR) {flgIns=1;}
                          }

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }

                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         }

        for(k=0;k<nn-1;k++) {
          yBeg=(lpWorkBuffer->line[indFr]).y2[k];
          yEnd=(lpWorkBuffer->line[indFr]).y1[k+1];
         ynD=yBeg+1;
          ynD=(lpWorkBuffer -> hightImg)-1-ynD;
          nY = (LONG)ynD*(LONG)w_bt;
       hptr  = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bL;
       hptrL = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLL;
       hptrR = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLR;

       switch(lB) {
         case 1:
                for(yn=yBeg+1;yn<yEnd-1;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }

                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 2:
                for(yn=yBeg+1;yn<yEnd-1;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                     if( (~(*(hptr + 1L))) & maskR) {flgIns=1;}
                        }
                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }

                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 3:
                for(yn=yBeg+1;yn<yEnd-1;yn++) {
                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                          for(i=1;i<=rI;i++) {
                             if( (~(*(hptr + (long)i))) & maskM) {flgIns=1;break;}
                             }
                          }
                     if(!flgIns) {
                          if( (~(*(hptr + (long)rI1))) & maskR) {flgIns=1;}
                          }

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         }
       }
        yBeg=(lpWorkBuffer->line[indFr]).y2[nn-1];
         ynD=yBeg+1;
          ynD=(lpWorkBuffer -> hightImg)-1-ynD;
          nY = (LONG)ynD*(LONG)w_bt;
       hptr  = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bL;
       hptrL = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLL;
       hptrR = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bLR;

       switch(lB) {
         case 1:
                for(yn=yBeg+1;yn<yB;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 2:
                for(yn=yBeg+1;yn<yB;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                     if( (~(*(hptr + 1L))) & maskR) {flgIns=1;}
                        }
                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         case 3:
                for(yn=yBeg+1;yn<yB;yn++) {
                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                          for(i=1;i<=rI;i++) {
                             if( (~(*(hptr + (long)i))) & maskM) {flgIns=1;break;}
                             }
                          }
                     if(!flgIns) {
                          if( (~(*(hptr + (long)rI1))) & maskR) {flgIns=1;}
                          }

                     if(flgIns) {
                       flgOut=0;
                       if(xL-6>=x1) {
                         switch(lBL) {
                           case 1:
                                  flgOut = gr_ar[(~(*hptrL)) & maskLL];
                                  break;
                           case 2:
                                  flgOut  = gr_ar[(~(*hptrL)) & maskLL];
                                  flgOut += gr_ar[(~(*(hptrL + 1L))) & maskRL];
                           }
                         }
                       if(xR+6<x2) {
                         switch(lBR) {
                           case 1:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  break;
                           case 2:
                                  flgOut += gr_ar[(~(*hptrR)) & maskLR];
                                  flgOut += gr_ar[(~(*(hptrR + 1L))) & maskRR];
                           }
                         }
                       if(flgOut<2) {cc++;}
                       }
                     hptr  -= (LONG)w_bt;
                     hptrL -= (LONG)w_bt;
                     hptrR -= (LONG)w_bt;
                     }
                break;
         }
         return(cc);
  } /* outOfL() */

/***************************************************************************
 *  PURPOSE      :put white pixels                                         *
 *                                                                         *
 ***************************************************************************/
void putWhiteV( STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB){
    short w_bt;
    LONG nY;
    BYTE *hptr;
    int  yn, ynD;
    BYTE maskL, maskR, maskM;
    int  nL, nR, bL, bR, lB, i, rI, rI1;

        w_bt=(lpWorkBuffer -> widthImg);
         ynD=yT;
          ynD=(lpWorkBuffer -> hightImg)-1-ynD;
          nY = (LONG)ynD*(LONG)w_bt;

       bL = xL/8;
       bR = xR/8;
       nL = xL%8;
       nR = xR%8;
       nR = 7 - nR;

       maskL = mask_L[nL];
       maskM = 255;
       maskR = mask_R[nR];
       if(bL == bR) {
          maskL &=mask_R[nR];
          }

       lB   = bR - bL + 1;
       rI   = lB - 2;
       rI1  = rI + 1;
       hptr = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bL;
       if(lB>2) {lB = 3;}
       switch(lB) {
         case 1:
                for(yn=yT;yn<=yB;yn++) {
                     (*hptr) |= maskL;
                     hptr -= (LONG)w_bt;
                     }
                break;
         case 2:
                for(yn=yT;yn<=yB;yn++) {
                     (*hptr)       |= maskL;
                     (*(hptr + 1L)) |= maskR;
                     hptr -= (LONG)w_bt;
                     }
                break;
         case 3:
                for(yn=yT;yn<=yB;yn++) {
                     (*hptr) |= maskL;
                     for(i=1;i<=rI;i++) {
                          (*(hptr + (long)i)) = maskM;
                          }
                     (*(hptr + (long)rI1)) |= maskR;
                     hptr -= (LONG)w_bt;
                     }
                break;
        }

  } /* putWhiteV() */

/***************************************************************************
 *  PURPOSE      :put white pixels between lines                           *
 ***************************************************************************/
void putWhiteVBt(STRUCTWB *lpWorkBuffer, int ver, int xL, int xR, int yT, int yB){
    int  k,yEnd, yBeg, nn, indFr;

        indFr=ver;
        nn=(lpWorkBuffer->infFrame[indFr]).nLines;
        if(nn<=0) return;

        yEnd=(lpWorkBuffer->line[indFr]).y1[0];
        putWhiteV(lpWorkBuffer,xL,xR,yT,yEnd);

        for(k=0;k<nn-1;k++) {
          yBeg=(lpWorkBuffer->line[indFr]).y2[k];
          yEnd=(lpWorkBuffer->line[indFr]).y1[k+1];
          putWhiteV(lpWorkBuffer,xL,xR,yBeg,yEnd);

          }
        yBeg=(lpWorkBuffer->line[indFr]).y2[nn-1];
        putWhiteV(lpWorkBuffer,xL,xR,yBeg,yB);
  } /* putWhiteVBt() */
int isItVertL(STRUCTWB *lpWorkBuffer, int gap, int xL, int xR, int yT, int yB){
  int   yn, flgW, flgB, flgIns, begL, endL, maxLen;
  int   ynD, w_bt;
  LONG nY;
  BYTE *hptr;
  BYTE maskL, maskR, maskM;
  int  nL, nR, bL, bR, lB, i, rI, rI1;
        w_bt=(lpWorkBuffer -> widthImg);

         ynD=yT;
          ynD=(lpWorkBuffer -> hightImg)-1-ynD;
          nY = (LONG)ynD*(LONG)w_bt;
       bL = xL/8;
       bR = xR/8;
       nL = xL%8;
       nR = xR%8;
       nR = 7 - nR;

       maskL = mask_L[nL];
       maskM = 255;
       maskR = mask_R[nR];
       if(bL == bR) {
          maskL &=mask_R[nR];
          }

       lB  = bR - bL + 1;
       rI  = lB - 2;
       rI1 = rI + 1;
       hptr = (lpWorkBuffer->lpRBuffer)+nY + (LONG)bL;
       if(lB>2) {lB = 3;}

        flgB=0;
        flgW=0;
        begL=0;
        endL=0;
        maxLen=0;

       switch(lB) {
         case 1:
                for(yn=yT;yn<=yB;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}

                     if(flgIns) {
                       if(!flgB) {
                          flgB=1;
                          begL=yn;
                          }
                       flgW=0;
                       }
                    else {
                       if(flgB) {flgW++;}
                       if(flgW>gap) {
                         endL=yn-flgW;
                         if(maxLen<endL-begL+1) {
                             maxLen=endL-begL+1;
                             begVertLine=begL;
                             endVertLine=endL;
                             }
                         flgB=0;
                         flgW=0;
                         begL=0;
                         endL=0;
                         }
                      }
                      hptr -= (LONG)w_bt;
                     }
                break;
         case 2:
                for(yn=yT;yn<=yB;yn++) {

                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                     if( (~(*(hptr + 1L))) & maskR) {flgIns=1;}
                        }

                     if(flgIns) {
                       if(!flgB) {
                          flgB=1;
                          begL=yn;
                          }
                       flgW=0;
                       }
                    else {
                       if(flgB) {flgW++;}
                       if(flgW>gap) {
                         endL=yn-flgW;
                         if(maxLen<endL-begL+1) {
                             maxLen=endL-begL+1;
                             begVertLine=begL;
                             endVertLine=endL;
                             }
                         flgB=0;
                         flgW=0;
                         begL=0;
                         endL=0;
                         }
                      }
                     hptr -= (LONG)w_bt;
                     }
                break;
         case 3:
                for(yn=yT;yn<=yB;yn++) {
                     flgIns=0;
                     if( (~(*hptr)) & maskL) {flgIns=1;}
                     else {
                          for(i=1;i<=rI;i++) {
                             if( (~(*(hptr + (long)i))) & maskM) {flgIns=1;break;}
                             }
                          }
                     if(!flgIns) {
                          if( (~(*(hptr + (long)rI1))) & maskR) {flgIns=1;}
                          }

                     if(flgIns) {
                       if(!flgB) {
                          flgB=1;
                          begL=yn;
                          }
                       flgW=0;
                       }
                    else {
                       if(flgB) {flgW++;}
                       if(flgW>gap) {
                         endL=yn-flgW;
                         if(maxLen<endL-begL+1) {
                             maxLen=endL-begL+1;
                             begVertLine=begL;
                             endVertLine=endL;
                             }
                         flgB=0;
                         flgW=0;
                         begL=0;
                         endL=0;
                         }
                      }
                     hptr -= (LONG)w_bt;
                     }
                break;
        }

        if((endL==0)&&(begL>0)) {
                endL=yB-flgW;
                if(maxLen<endL-begL+1) {
                        maxLen=endL-begL+1;
                        begVertLine=begL;
                        endVertLine=endL;
                        }
                }
        return(maxLen);
  } /* isItVertL() */
/***************************************************************************
 *  PURPOSE      :shift lines structure for indM frame                      *
 *                                                                         *
 ***************************************************************************/
void shiftBack(STRUCTWB *lpWorkBuffer, int indM, int indF){
  int i;
     for(i=indF;i<(lpWorkBuffer->infFrame[indM]).nLines;i++)  {
        (lpWorkBuffer->line[indM]).y1[i-1]=(lpWorkBuffer->line[indM]).y1[i];
        (lpWorkBuffer->line[indM]).y2[i-1]=(lpWorkBuffer->line[indM]).y2[i];
        }
    ((lpWorkBuffer->infFrame[indM]).nLines)--;
} /* of shiftBack() */

/***************************************************************************
 *  PURPOSE      :max y coef. in the next line                             *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                 <0  - there are not more lines                          *
 *                 >=0 - index of frame where was searched max             *
 *                                                                         *
 ***************************************************************************/
int maxYstruct(STRUCTWB *lpWorkBuffer){
  int i, maxY, indMax;

     maxY=2*MAXSIZE;
     indMax=-1;
     for(i=0;i<(lpWorkBuffer->frNumber);i++)  {
        (lpWorkBuffer->curLine[i]).y2=0;
        if( ((lpWorkBuffer->infFrame[i]).nLines)>0 )  {
                if( ((lpWorkBuffer->line[i]).y1[0])<maxY ) {
                        maxY=(lpWorkBuffer->line[i]).y1[0];
                        indMax=i;
                        }
                }
        }

     if(indMax>=0) {
       (lpWorkBuffer->curLine[indMax]).x1=(lpWorkBuffer->infFrame[indMax]).x1;
       (lpWorkBuffer->curLine[indMax]).x2=(lpWorkBuffer->infFrame[indMax]).x2;
       (lpWorkBuffer->curLine[indMax]).y1=(lpWorkBuffer->line[indMax]).y1[0];
       (lpWorkBuffer->curLine[indMax]).y2=(lpWorkBuffer->line[indMax]).y2[0];
       (lpWorkBuffer->indTopCurLine)=indMax;
       shiftBack(lpWorkBuffer,indMax,1);
       }
       if(indMax>0) {
         while( (0<((lpWorkBuffer->infFrame[indMax]).nLines)) && (((lpWorkBuffer->line[indMax-1]).y2[0])>(lpWorkBuffer->line[indMax]).y2[0]) )  {
              (lpWorkBuffer->curLine[indMax]).y2=(lpWorkBuffer->line[indMax]).y2[0];
              shiftBack(lpWorkBuffer,indMax,1);
              }
        }
     return(indMax);
} /* of maxYstruct() */
/***************************************************************************
 *  PURPOSE      :search for line from current frame to first one          *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                first frame                                              *
 *                                                                         *
 ***************************************************************************/
int searchBeg(STRUCTWB *lpWorkBuffer, int nFr){
  int i, y1, y2, Yd, y, indCur, j;

     if(nFr==0) return(nFr);
     y1=(lpWorkBuffer->curLine[nFr]).y1;
     y2=(lpWorkBuffer->curLine[nFr]).y2;
     Yd=5;
     if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) Yd=0;
     indCur=nFr;
     for(i=nFr-1;i>=0;i--)  {
        (lpWorkBuffer->curLine[i]).x1=(lpWorkBuffer->infFrame[i]).x1;
        (lpWorkBuffer->curLine[i]).x2=(lpWorkBuffer->infFrame[i]).x2;

        if( ((lpWorkBuffer->infFrame[i]).nLines)>0 )  {
            j=1;
            while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y2[j])<(y2+2)) )  {
              (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
              shiftBack(lpWorkBuffer,i,j+1);
              }

            j=1;
            while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y1[j])<((2*y2+y1)/3)) )  {
              (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
              shiftBack(lpWorkBuffer,i,j+1);
              }
            if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) {
              while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y1[j])<((5*y2+y1)/6))&& (((lpWorkBuffer->line[i]).y2[j])<(y2+4)) )  {
                (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
                shiftBack(lpWorkBuffer,i,j+1);
                }
              }

            y=(lpWorkBuffer->line[i]).y1[0];
            if( (y<y2+Yd) ) {
              (lpWorkBuffer->curLine[i]).y1=(lpWorkBuffer->line[i]).y1[0];
              (lpWorkBuffer->curLine[i]).y2=(lpWorkBuffer->line[i]).y2[0];
              shiftBack(lpWorkBuffer,i,1);
              y2=(lpWorkBuffer->curLine[i]).y2;
              indCur=i;
              }
            }
        }
     return(indCur);

} /* of searchBeg() */

/***************************************************************************
 *  PURPOSE      :search for line from current frame to end one            *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                end frame                                                *
 *                                                                         *
 ***************************************************************************/
int searchEnd(STRUCTWB * lpWorkBuffer, int nFr){
  int i, y1, y2, Yd, y, indCur, frN, j;

     frN=(lpWorkBuffer->frNumber);
     if( nFr>=(frN-1) ) return(nFr);
     y1=(lpWorkBuffer->curLine[nFr]).y1;
     y2=(lpWorkBuffer->curLine[nFr]).y2;
     Yd=5;
     if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) Yd=-1;
     indCur=nFr;
     for(i=nFr+1;i<frN;i++)  {
        (lpWorkBuffer->curLine[i]).x1=(lpWorkBuffer->infFrame[i]).x1;
        (lpWorkBuffer->curLine[i]).x2=(lpWorkBuffer->infFrame[i]).x2;

        if( ((lpWorkBuffer->infFrame[i]).nLines)>0 )  {
            j=1;
            while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y2[j])<(y2+2)) )  {
              (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
              shiftBack(lpWorkBuffer,i,j+1);
              }

            j=1;
            while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y1[j])<((2*y2+y1)/3)) )  {
              (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
              shiftBack(lpWorkBuffer,i,j+1);
              }
            if(((lpWorkBuffer -> fieldType)==MULTILINEBOXD)||((lpWorkBuffer -> fieldType)==MULTILINEBOX)||((lpWorkBuffer -> fieldType)==MULTIBOX)) {
              while( (j<((lpWorkBuffer->infFrame[i]).nLines)) && (((lpWorkBuffer->line[i]).y1[j])<((5*y2+y1)/6))&& (((lpWorkBuffer->line[i]).y2[j])<(y2+4)) )  {
                (lpWorkBuffer->line[i]).y2[0]=(lpWorkBuffer->line[i]).y2[j];
                shiftBack(lpWorkBuffer,i,j+1);
                }
              }

            y=(lpWorkBuffer->line[i]).y1[0];
            if( (y<y2+Yd) ) {
              (lpWorkBuffer->curLine[i]).y1=(lpWorkBuffer->line[i]).y1[0];
              (lpWorkBuffer->curLine[i]).y2=(lpWorkBuffer->line[i]).y2[0];
              shiftBack(lpWorkBuffer,i,1);
              y2=(lpWorkBuffer->curLine[i]).y2;
              indCur=i;
              }
            }
        }

     return(indCur);
} /* of searchEnd() */
/***************************************************************************
 *									   *
 *  FUNCTION     :void linLoc()                                            *
 *                                                                         *
 *  PURPOSE      :recognize local lines from histogram                     *
 *                                                                         *
 *  RETURN VALUE :none                                                     *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void linLoc(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int    iBot, int lineHeight, int  levelC  ){
   int  i, flg_g, count_0, indL, tH, tH1, flgE, tH10;
   int  tHlp1, tHlp2, flg_gB, gap, thEE, tHDpi;

   int  tH101;

       tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0))-2;

       if(lineHeight<=0) lineHeight=1000;

       tH=levelC;
       tH10=tH/10;
       if((tH%10)>4) {tH10++;}
       if(tH10==0)   {tH10=-1;}
       if(levelC==0) {tH10=0;}

       tH101 = tH10;
       if(tH10 > 0 ) {
        tH10 = tH/4;
        }

       flg_g=0;
       flg_gB=0;
       count_0=0;
       indL=maxIndxLines1;

       tH1=0;
       tHlp1=tH1;
       tHlp2=1;
       thEE=TH_END;
       flgE=0;
       if(lineHeight<1000)  thEE=lineHeight-tHDpi+2;
       gap=1;
       i=iTop;
       while(i<iBot) {

             if(flg_g>lineHeight) {
                i+=(-count_0);
                if(count_0) i++;

                tmpLines1[indL].indBot=i;
                indL++;
                flg_g=0;
                flg_gB=0;
                tH1=tHlp1;
                count_0=0;
                if(indL>=MAXLINES) {
                        maxIndxLines1 =indL;
                        return;
                        }
                }
             else {
                if(hist[i]>tH)  {
                     if(flg_g==0) {
                          tmpLines1[indL].indTop=max(i-1,1);
                          }
                     flgE=0;
                     flg_g++;
                     flg_gB++;
                     count_0=0;
                     if(gap) {
                       if(flg_g>tHDpi-2) tH1=tHlp2;
                       if(flg_g>thEE)    {
                          tH1=tHlp1;
                          tH10 = tH101;
                          }
                       }
                        }
                 else{
                    if((flg_g>0)&&(flgE<2)&&(tH-hist[i]<=tH10)) {
                        flgE++;
                        flg_g++;
                        flg_gB++;
                        count_0=0;
                        goto NXTI;
                        }
                    flgE=0;
                    if(flg_g>lineHeight-4) tH1=tHlp1;
                    if(((flg_gB>tHDpi)&&(count_0>tH1-1)) ){
                           i-=count_0;
                           tmpLines1[indL].indBot=i;
                           indL++;
                           if(indL>=MAXLINES) {
                                maxIndxLines1 =indL;
                                return;
                                }
                        }
                    if((flg_g>0)&&(count_0<tH1)) flg_g++;
                    count_0++;
                    if(count_0>tH1) {
                        flg_g=0;
                        flg_gB=0;
                        tH1=tHlp1;
                        }

                    }
                }
NXTI:           i++;

               } /* while */

            if(flg_g>tHDpi){
                  i-=count_0;
                  tmpLines1[indL].indBot=i;
                  indL++;
                  }
            maxIndxLines1 =indL;
  } /* of linLoc() */
/***************************************************************************
 *  PURPOSE      :test for shift one border                                *
 *                                                                         *
 ***************************************************************************/
int srchB(
int   *hist,         /* pointer to histogram  */
int    lenH,          /* lenght of histogram  */
int    iTop,                /* top    histogram index */
int    iBot,                /* bottom histogram index */
int    ind,                 /* border index */
int    drc                  /* shift direction        */
)
{
   int  aV, aVp, i, k;
   LONG laV;

      k=0;
      laV=0L;
      if(drc>0) {
         if((ind==1)&&(hist[ind]>8)) {return(ind);}
         for(i=ind;i<iBot-2;i++) {
           k++;
           laV+=(LONG)hist[i];
           aV=(int)(laV/(LONG)k);
           aVp=aV/3;
           if((aV%3)>1) {aVp++;}
           if(aVp<3)    {aVp++;}
           if(hist[i+1]-aV>aVp) {return(i);}
           }
         }
      else {
         if((ind>=lenH-1)&&(hist[ind]>8)) {return(ind);}
         for(i=ind;i>iTop+2;i--) {
           k++;
           laV+=(LONG)hist[i];
           aV=(int)(laV/(LONG)k);
           aVp=aV/3;
           if((aV%3)>1) {aVp++;}
           if(aVp<3)    {aVp++;}
           if(hist[i-1]-aV>aVp) {return(i);}
           }

         }

     return(-1);

} /* srchB() */
int sBLoc(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int iBot, int ind, int levelC, int lenH, int drc){
   int  flgUp, aV, aVp, i, iB, k, tH, ind2;
   LONG laV;
   float fH, fA, fB, fC, fY;

     tH=(int)((float)6*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0)+(float)0.5);
     fH=(float)0.18;
     fA=(float)(-0.02);
     fB=(float)2.1;
      ind2=ind;
      iB=ind+drc;
      if((iB<=iBot)&&(iB>=iTop)) {
        if((hist[iB]==0)&&(hist[ind]>0)) {return(iB);}
        if((drc > 0) && (levelC > 50)) {
                if((hist[ind] < levelC/3)) {return(ind);}
                if((hist[iB] < levelC/3)) {return(iB);}
                }
        }
      if((levelC>15)&&(hist[ind]>2)) {
        for(i=ind;(i<=iBot)&&(i>=iTop);i+=drc) {
            if(hist[i]<=2) {
                iB=i+drc;
                if((iB<=iBot)&&(iB>=iTop)) {
                      if((hist[iB]==0)&&(hist[i]>0)) {return(iB);}
                      }
                return(i);
                }
            iB=i+drc;
            if((iB<=iBot)&&(iB>=iTop)) {
                if(hist[i]<60 ) {fY=fH*((float)hist[i]*fA+fB);}
                else            {fY=(float)0.138;}
                fC=(float)(hist[i]-hist[iB])/(float)hist[i];
                if(fC<fY) {
                        if((hist[iB]==0)&&(hist[i]>0)) {return(iB);}
                        ind=i;
                        break;
                        }
                }
          }
        }
      flgUp=0;
      iB=ind+drc;
      if((iB<=iBot)&&(iB>=iTop)) {
        if(hist[iB]>levelC) {flgUp=1;}
        }
      iB+=drc;
      if((iB<=iBot)&&(iB>=iTop)) {
        if(hist[iB]>levelC) {flgUp=1;}
        }
      if(flgUp) {
        k=0;
        for(i=iB;(i<=iBot)&&(i>=iTop);i+=drc) {
                k++;
                if(k>TH_C) {break;}
                if(hist[i]<levelC) {
                       k=i+drc;
                       if((k<=iBot)&&(k>=iTop)) {
                           if((hist[k]==0)&&(hist[i]>0)) {return(k);}
                           }
                       k+=drc;
                       if((k<=iBot)&&(k>=iTop)) {
                           if((hist[k]==0)&&(hist[i]>0)) {return(k);}
                           }

                        return(i);
                        }
                }
        }
      iB=min(lenH/3,tH);
      k=0;
      laV=0L;
      for(i=ind;(i<iBot)&&(i>iTop);i+=drc) {
        k++;
        laV+=(LONG)hist[i];
        aV=(int)(laV/(LONG)k);
        aVp=aV/3;
        if((aV%3)>1) {aVp++;}
        if(aVp<3)    {aVp++;}
        if(aV-hist[i+drc]>aVp) {return(i+drc);}
        if(k>iB) {break;}
        }
     if(ind==ind2) {return(-1);}
     return(ind);
} /* sBLoc() */

int shBrd(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int iBot, int ind, int levelC, int drc){
   int  i;
    for(i=ind;(i<=iBot)&&(i>=iTop);i+=drc) {
            if(hist[i] < levelC) {
                return(i);
			}
	}
    return (ind);
} /* shBrd() */

/***************************************************************************
 *  PURPOSE      :shift borders                                             *
 *                                                                         *
 ***************************************************************************/
void shiftBord(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int iBot, int lineHeight, int levelC ){
   int  i, newInd, lenH, tmpI;
   long aV;
   int  sr, j, wh;


     if(lineHeight<=0) lineHeight=1000;

     wh = 0;
     for(i=0;i<maxIndxLines1; i++) {
        aV = 0L;
        lenH=tmpLines1[i].indBot-tmpLines1[i].indTop;
        if(lenH > 0) {
             for(j=tmpLines1[i].indTop;j<tmpLines1[i].indBot;j++) {
                   aV += (long)hist[j];
                   }
             sr = (int)(aV/lenH);
             if(sr == 0)
                      sr = 1;
             if((lenH < 18) && (maxIndxLines1 > 2) && (levelC > 25)) {
                          if((levelC*100)/sr > 60)  {
                                   wh = 1;
                                   }
                          }
             }

        tmpI=tmpLines1[i].indTop;
        if(!wh)
           newInd=sBLoc(lpWorkBuffer,hist,iTop,iBot,tmpLines1[i].indTop,levelC,lenH,-1);
        else
           newInd=shBrd(lpWorkBuffer,hist,iTop,iBot,tmpLines1[i].indTop,levelC - (levelC*3)/10,-1);
        if(newInd>=iTop) {
                if(tmpI>newInd) {
                  if(tmpLines1[i].indBot-newInd>lineHeight) {
                     newInd=min(tmpI,tmpLines1[i].indBot-lineHeight);
                     }
                  }

                if(i>0) {
                        if(newInd>=tmpLines1[i-1].indBot) {tmpLines1[i].indTop=newInd;}
                        }
                else    {tmpLines1[i].indTop=newInd;}
                }

        tmpI=tmpLines1[i].indBot;
        if(!wh)
           newInd=sBLoc(lpWorkBuffer,hist,iTop,iBot,tmpLines1[i].indBot,levelC,lenH,1);
        else
           newInd=shBrd(lpWorkBuffer,hist,iTop,iBot,tmpLines1[i].indBot,levelC - (levelC*3)/10,1);

        if((newInd<=iBot)&&(newInd>0)) {
             if(tmpI<newInd) {
              if(newInd-tmpLines1[i].indTop>lineHeight) {
                 newInd=max(tmpI,tmpLines1[i].indTop+lineHeight);
                 }
              }
                if(i<maxIndxLines1-1) {
                        if(newInd<=tmpLines1[i+1].indTop) {tmpLines1[i].indBot=newInd;}
                        }
                else    {tmpLines1[i].indBot=newInd;}
                }
        }

} /* shiftBord() */
/***************************************************************************
 *  PURPOSE      :prepacking lines from local1 to local structure          *
 *                                                                         *
 ***************************************************************************/
void prePackL(
int    ind           /* index from local structure */
)
{
 int i;

  if(maxIndxLines-1+maxIndxLines1<MAXLINES) {
    for(i=maxIndxLines-1;i>ind;i--) {
        tmpLines[i+maxIndxLines1].indTop=tmpLines[i].indTop;
        tmpLines[i+maxIndxLines1].indBot=tmpLines[i].indBot;
        tmpLines[i+maxIndxLines1].lIns  =tmpLines[i].lIns;
        }
    for(i=0;i<maxIndxLines1;i++) {
        tmpLines[i+ind+1].indTop=tmpLines1[i].indTop;
        tmpLines[i+ind+1].indBot=tmpLines1[i].indBot;
        tmpLines[i+ind+1].lIns  =1;
        }
    maxIndxLines+=maxIndxLines1;
    return;
    }
  tmpLines[ind].lIns  =0;

} /* prePackL() */
/***************************************************************************
 *  PURPOSE      :recognize lines from histogram                           *
 ***************************************************************************/
void linesUnp2(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int tHBottom){
   int i, flg_g, f_time, count_0, y1, indL, tH, delt, tH1;
   int tHlp1, tHlp2, flg_gB, thEE, tHDpi;

     tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));
       tH=tHBottom;
       flg_g=0;
       flg_gB=0;
       count_0=0;
       f_time=0;
       indL=0;

       tH1=0;
       tHlp1=tH1;
       tHlp2=2;
       thEE=TH_END;
       y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
       i=1;
       while(i<lenH) {

                if(hist[i]>tH)  {
                     if(flg_g==0) {
                          tmpLines[indL].indTop=i+y1;
                          }
                     flg_g++;
                     flg_gB++;
                     count_0=0;
                     if(flg_g>tHDpi-2) tH1=tHlp2;
                     if(flg_g>thEE)    tH1=tHlp1;
                     }
                 else{
                    if(((flg_gB>tHDpi)&&(count_0>tH1-1)) ){
                           delt=0;
                           i+=(delt-count_0);
                           tmpLines[indL].indBot=min(i+y1,(lpWorkBuffer->begRect.y)+(lpWorkBuffer -> Height)-1);
                           indL++;
                           if(indL>=MAXLINES) {
                                maxIndxLines=indL;
                                return;
                                }
                        }
                    if((flg_g>0)&&(count_0<tH1)) flg_g++;
                    count_0++;
                    if(count_0>tH1) {
                        flg_g=0;
                        flg_gB=0;
                        tH1=tHlp1;
                        }

                    }

               i++;

               } /* while */

            if(flg_g>tHDpi){
                  i-=count_0;
                  i+=(y1);
                  i=min(i,(lpWorkBuffer -> hightImg)-2);
                  tmpLines[indL].indBot=min(i,(lpWorkBuffer->begRect.y)+(lpWorkBuffer -> Height)-1);
                  indL++;
                  }
            maxIndxLines=indL;

  } /* of linesUnp2() */
int  leftBordB(STRUCTWB *lpWorkBuffer, int xLI, int xRI, int yT, int yB)
{
  BYTE maskByte;
  BYTE ch;
  int yD, j, k, cB, lByte, rByte, xOutp, flgB;
  short mask, w_bt;
  LONG nY;
  BYTE  *hptr;
  BYTE  *hptrW;

        w_bt=(lpWorkBuffer -> widthImg);

        yD = yT;
        yD=(lpWorkBuffer -> hightImg)-1-yD;
        nY=(LONG)yD*(LONG)w_bt;

        lByte = xLI/8;
        rByte = xRI/8;
        hptr  = (lpWorkBuffer->lpRBuffer)+nY+(LONG)lByte;
        hptrW = hptr;

        xOutp = xLI;
        for(j=0;j<rByte-lByte+1;j++){
           maskByte = 0;
           hptr = hptrW + (long)j;
           for(k=0;k<yB-yT+1;k++){
               ch = *hptr;
               if(ch != 255) {
                  maskByte |= (~ch);
                  }
               hptr -= (long)w_bt;
               }

           mask=LEFT_BIT;
           flgB = 0;
           for(k=0;k<8;k++) {
               cB=maskByte & mask;
               if(cB)  {
                  flgB++;
                  }
               else {
                  flgB = 0;
                  }
               if(flgB > 1) {
                  xOutp = (lByte + j)*8 + (k -1);
                  return(xOutp);
                  }
               mask >>= 1;
               }
           }
      return(xOutp);

 } /* leftBordB() */
int  rightBordB(STRUCTWB *lpWorkBuffer, int xLI, int xRI, int yT, int yB){
  BYTE maskByte;
  BYTE ch;
  int yD, j, k, cB, lByte, rByte, xOutp, flgB;
  short mask, w_bt;
  LONG nY;
  BYTE *hptr;
  BYTE *hptrW;

        w_bt=(lpWorkBuffer -> widthImg);

        yD = yT;
        yD=(lpWorkBuffer -> hightImg)-1-yD;
        nY=(LONG)yD*(LONG)w_bt;

        lByte = xLI/8;
        rByte = xRI/8;
        hptr  = (lpWorkBuffer->lpRBuffer)+nY+(LONG)rByte;
        hptrW = hptr;

        xOutp = xRI;
        for(j=0;j<rByte-lByte+1;j++){
           maskByte = 0;
           hptr = hptrW - (long)j;
           for(k=0;k<yB-yT+1;k++){
               ch = *hptr;
               if(ch != 255) {
                  maskByte |= (~ch);
                  }
               hptr -= (long)w_bt;
               }

           mask=RIGHT_BIT;
           flgB = 0;
           for(k=0;k<8;k++) {
               cB=maskByte & mask;
               if(cB)  {
                  flgB++;
                  }
               else {
                  flgB = 0;
                  }
               if(flgB > 1) {
                  xOutp = (rByte - j+ 1)*8 - (k -1);
                  return(xOutp);
                  }
               mask <<= 1;
               }
           }
      return(xOutp);

 } /* rightBordB() */

/***************************************************************************
 *                                                                         *
 *  PURPOSE      :filtration narrow lines from histogram                   *
 *                                                                         *
 ***************************************************************************/
int filtrHistInsL(STRUCTWB *lpWorkBuffer, int *hist, int lenH, int xLI, int xRI, TMPLINES *lpTmpL, int maxIL, long  alpha,
                             int  scaleAngle, int wCharType){
  int   i, j, k, indT, indB, impT, impB, ii, count;
  int   y1, y2, yT, yB, x1,x2, xL, xR, indFr, xRemL, xRemR;
  int   tHDpi, tHDpi1, begS, endS;
  int   lT, lB, max, maxS, maxP, ret, aVG, min, tHTl, tHTh, tT;
  int   frQ, dd, lTs, lBs;
  LONG  aV;
  float flC1, flC2, flCT;
  int   leftB, rightB, maxLenghtLine, lenH1;

     lenH1 = lenH - 1;
     ret=-1;
     indFr=0;
     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
     y2=y1+(lpWorkBuffer -> Height)-1;

     x1=(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW);
     x2=x1+(lpWorkBuffer -> Width)-1;
     tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)300.0));
     tHTl=(int)((float)25*((float)(lpWorkBuffer -> Dpi.y)/(float)300.0));
     tHTh=(int)((float)45*((float)(lpWorkBuffer -> Dpi.y)/(float)300.0));
     if(lenH<MAXPROFILE-1) {hist[lenH]=0;}
     maxLenghtLine = 35;
     tHDpi1 = tHDpi +1;

     for(i=0;i<maxIL;i++) {
        yT  =lpTmpL[i].indTop;
        yB  =lpTmpL[i].indBot;
        indT=yT-y1;
        indB=yB-y1;

        j = indB;
        if((j < lenH1) && (hist[j] > 0)) {
             j++;
             if((j < lenH1) && (hist[j] > 0)) {
                  j++;
                  if((j < lenH1) && (hist[j] > 0)) {
                        if(wCharType < 4)
                              indB = min(lenH1,indB+30);
                        else
                              indB = min(lenH1,indB+10);
                        if(i+1 < maxIL) {
                           dd = lpTmpL[i+1].indTop - y1;
                           if(indB >= dd) {
                              indB = dd - 1;
                              }
                           }
                        }
                  }
             }

/* left border */
        xL = leftBordB(lpWorkBuffer,xLI,xRI,yT,yB);
        if(xL<0) {k=0;goto NXTE;}

/* right border */

        xR = rightBordB(lpWorkBuffer,xLI,xRI,yT,yB);
        if(xR<0) {k=0;goto NXTE;}

        tT=tHTh;
        if((xR-xL)<tT) {k=0;goto NXTE;}
       aVG=0;
       aV=0L;k=0;
       for(j=indT;j<=indB;j++) {
          k++;
          aV+=hist[j];
          }
       if(k>0) {aVG=(int)(aV/(LONG)k);}
       aVG=(int)((float)1.5*aVG);
     begS=max(2,indT);
     endS=min(lenH1,indB);
     if(endS>=MAXPROFILE) {endS=MAXPROFILE-1;}

     for(ii=begS;ii<=endS;ii++) {
        if((hist[ii+1]<=hist[ii])&&(hist[ii-1]<hist[ii])&&(hist[ii]!=0)) {
            max=hist[ii];

         if(max<aVG) {continue;}

         flC1=(float)(-1.0);
         if(max>0)  flC1=(float)(xR-xL)/(float)max;
         if(flC1 >= (float)2.0) {continue;}

         maxP=max-max/10;
         k=ii;
         lT=k;
         for(j=1;j<=tHDpi1;j++) {
          if(k-j>0) {
            if(maxP>hist[k-j]) {break;}
            lT=k-j;
            }
          }
         lB=k;
         for(j=1;j<=tHDpi1;j++) {
          if(k+j<lenH) {
            if(maxP>hist[k+j]) {break;}
            lB=k+j;
            }
          }

       if(lB-lT>=tHDpi) {k=0;goto NXT;}

       maxS=0;
       aV=0;k=0;

       leftB  = max(indT,ii - maxLenghtLine);
       rightB = min(indB,ii + maxLenghtLine);
       for(j=leftB;j<=rightB;j++) {
          if(((j<lT-2)||(j>lB+2))&&(hist[j]<maxP)) {
                k++;
                aV+=hist[j];
                }
          }

       if(k>0) {maxS=(int)(aV/(LONG)k);}
       impT=leftB;
       for(j=leftB;j<rightB;j++) {
          if(hist[j]>=maxS) {impT=j;break;}
          }

       impB=rightB;
       for(j=rightB;j>leftB;j--) {
          if(hist[j]>=maxS) {impB=j;break;}
          }
       aV=0;k=0;
       for(j=impT;j<=impB;j++) {
          if(((j<lT-2)||(j>lB+2))&&(hist[j]<maxP)) {
                k++;
                aV+=hist[j];
                }
          }
       if(k>0) {maxS=(int)(aV/(LONG)k);}
       maxP=maxS+(3*maxS)/4;
       k=lT;
       for(j=1;j<=tHDpi1;j++) {
          if(k-j>0) {
            if(hist[k-j]<maxP) {break;}
            lT=k-j;
            }
          }
       k=lB;
       for(j=1;j<=tHDpi1;j++) {
          if(k+j<lenH) {
            if(hist[k+j]<maxP) {break;}
            lB=k+j;
            }
          }
        if(lB-lT>=tHDpi) {k=0;goto NXT;}

       if(maxS>0) {
        count=0;
AGNI:   count++;
        if(lT-2>0) {
                min=hist[lT-2];
                if(lT-3>0) {
                        if(min>hist[lT-3]) {min=hist[lT-3];}
                        }
                if(min==0) {min=1;}
                flCT=(float)hist[lT-1]/(float)min;
                if(flCT>(float)1.7) {lT--;}
                else {
                    if((hist[lT-1]>maxP)&&(flCT>(float)1.3)) {lT--;}
                    }

            }
         else {
            if(lT-1>0) {
               if(hist[lT-1]>maxP) {lT--;}
               }
            }
         if(lB+2<lenH) {
                min=hist[lB+2];
                if(lB+3<lenH) {
                        if(min>hist[lB+3]) {min=hist[lB+3];}
                        }
                if(min==0) {min=1;}
                flCT=(float)hist[lB+1]/(float)min;
                if(flCT>(float)1.7) {lB++;}
                else {
                    if((hist[lB+1]>maxP)&&(flCT>(float)1.3)) {lB++;}
                    }
            }
         else {
            if(lB+1<lenH) {
               if(hist[lB+1]>maxP) {lB++;}
               }
            }
         if(count<2) {goto AGNI;}
         }

       flC2=(float)(-1.0);
       if(maxS>0) flC2=(float)max/(float)maxS;

       k=0;
       if(wCharType>3) {
          if((max>300)&&(flC1<(float)1.01)&&(flC2>(float)2.2)) {k=1;goto NXT;}

          if((flC1<(float)1.09)&&(flC2>(float)2.5)) {k=1;goto NXT;}
          if((flC1<(float)1.3)&&(flC2>(float)3.3))  {k=1;goto NXT;}
          if((flC1<(float)1.5)&&(flC2>(float)3.6))  {k=1;goto NXT;}
          if((flC1<(float)1.6)&&(flC2>(float)4.0))  {k=1;goto NXT;}
          if((flC1<(float)1.8)&&(flC2>(float)5.0))  {k=1;goto NXT;}
          if((flC1<(float)2.0)&&(flC2>(float)5.5))  {k=1;goto NXT;}

          if((max>800)&&(flC1<(float)1.3)&&(flC2>(float)2.9))  {k=1;goto NXT;}


          }
       else {
          if((lpWorkBuffer->horLineMM) == 1){
                if((flC1<(float)2.0)&&(flC2>(float)2.8)) {k=1;goto NXT;}
          }
          else{
                if((flC1<(float)1.09)&&(flC2>(float)3.5)) {k=1;goto NXT;}
          }
          if((flC1<(float)1.3)&&(flC2>(float)4.6))  {k=1;goto NXT;}

          }

NXT:    if(k) {
          xRemL = xL;
          xRemR = xR;
          if(flC1 < (float)1) {
              xRemL = xLI;
              xRemR = xRI;
              }
          histVertR(lpWorkBuffer,(int *)ind,xRemL,xRemR,max(lT+y1-1-8,y1),min(lB+y1-1,y2));
          dd=lB-lT;
          lTs=lT;
          lBs=lB;
          memset(xxx,0x00,(unsigned int)(40));
          for(j=1;j<ind[0];j++) {
                if(ind[j]<=dd) {xxx[ind[j]]++;}
                }
          frQ=0;k=dd;
          for(j=1;j<=dd;j++) {
                if(frQ<xxx[j]) {frQ=xxx[j];k=j;}
                }
          if((k<dd)&&(frQ>max/3)) {
                j=1;
                while(lBs-lTs>k) {
                        switch(j) {
                                case 0:lTs++;
                                       break;
                                case 1:lTs++;
                                       j=2;
                                       break;
                                case 2:lBs--;
                                       j=1;
                                       break;
                                }
                        }
                }

          j = 1;
          k = max/100;
          if((lT > 4)&&((hist[lT-1] <= k) || (hist[lT-2] <= k)))
                                       j = 0;
          if(lpWorkBuffer -> horLineMM == 2) {
                 if(lB-lT > 1)   lT++;
                 if(lBs-lTs > 1) lTs++;
                 if(lB-lT > 1)   lT++;
                 if(lBs-lTs > 1) lTs++;
                 }

          if(lpWorkBuffer -> horLineMM == 3) {
                 if(lB-lT > 1)   lB--;
                 if(lBs-lTs > 1) lBs--;
                 if(lB-lT > 1)   lB--;
                 if(lBs-lTs > 1) lBs--;
                 }

          putWhiteH(lpWorkBuffer,xRemL,xRemR,lT+y1-1,lB+y1-1,lTs+y1-1,lBs+y1-1,alpha,scaleAngle,j);
          ret=0;
          }
        else {

          }
         max=0;
         }
        }
NXTE:;
     }
     return(ret);
} /* of filtrHistInsL() */
/***************************************************************************
 *  PURPOSE      : vertical histogram,                                     *
 ***************************************************************************/
void histVertR(STRUCTWB *lpWorkBuffer, int *hist, int x1, int x2, int y1, int y2){
  int  j, kk, curInd, yy;
  int  yD, ch;
  int  lByte, rByte, rL, rR, w_bt;
  LONG nY;
  BYTE *hptr;
  BYTE mask;

    w_bt=(lpWorkBuffer -> widthImg);
    memset(hist,0x00,(unsigned int)(2*MAXPROFILE));

    hist[0]=x2-x1+1;


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
                          hist[kk]++;
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
                          hist[curInd]++;
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
                          hist[curInd]++;
                          }
                     curInd++;
                     mask >>=1;
                     }
                 }
             }
        }
   }/* of histVertR()*/

int linesUnpRet_Num(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight){
   int  i, lTop, lBot, kk, aV = 0, baV, indL, ret, tH, aVT = 0, dif, res, aVLn;
   int  maxCC, tHDpi, countF, mm;
   LONG aVl;
     tH=(int)((float)15*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));
     tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0))-2;
     lTop=1;
     for(i=1;i<lenH;i++) {
          if(hist[i]>0) {lTop=i-1;break;}
         }
     lTop=max(1,lTop);
     lBot=lenH-1;
     for(i=lenH-1;i>1;i--) {
          if(hist[i]>0) {lBot=i+1;break;}
         }
     lBot=min(lenH-1,lBot);

     indL=0;
     maxIndxLines=2;

     tmpLines[0].indTop=1;
     tmpLines[0].indBot=1;
     tmpLines[0].lIns  =1;

     tmpLines[1].indTop=lenH;
     tmpLines[1].indBot=lenH;
     tmpLines[1].lIns  =1;
     if((lineHeight<tHDpi)&&(lineHeight>0)) {return(0);}
     aVLn=0;
     countF=0;
     do  {
        maxIndxLines1=0;
        kk=lBot-lTop;
        aVl=0L;
        mm = 0;
        for(i=lTop;i<lBot;i++) {
           aVl+=(LONG)hist[i];
           if(mm < hist[i])
                    mm = hist[i];
           }
        if(kk>0) {
               aV=(int)(aVl/(LONG)kk);
               if((mm < 14) && (aV >= 4))
                                mm = 1;
               else
                                mm = 0;
               }
        aVl=0L;
        maxCC=0;
        kk=0;
        for(i=lTop;i<lBot;i++) {
           if(hist[i]>aV) {
                aVl+=(LONG)hist[i];
                kk++;
                }
           else {
                if(kk>0) {
                        if(maxCC<kk) {maxCC=kk;aVT=(int)(aVl/(LONG)kk);}
                        kk=0;aVl=0L;
                        }
                }
           }
         if(kk>0) {
                if(maxCC<kk) {maxCC=kk;aVT=(int)(aVl/(LONG)kk);}
                }
        if(maxCC>tHDpi) {aV=aVT-aVT/10;if((aVT%10)>4) {aV--;}}
        baV=aV/5;
        if(aV<15) {aV*=4;}
        else      {aV=2*aV+aV/2;}
        kk=0;
        aVl=0L;
        for(i=lTop;i<lBot;i++) {
           if((hist[i]<aV)&&(hist[i]>baV)) {aVl+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {aV=(int)(aVl/(LONG)kk);}
        baV=aV/5;
        if(aV<15) {aV*=4;}
        else      {aV=2*aV+aV/2;}
        aVT=aV;
        kk=0;
        aVl=0L;
        for(i=lTop;i<lBot;i++) {
           if((hist[i]<aV)&&(hist[i]>baV)) {aVl+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {aV=(int)(aVl/(LONG)kk);}
          baV=aV-aV/10;
          if((aV%10)>4) {baV--;}
          if(aV<20)     {baV--;}
          if(aV<10)     {baV--;}
          kk=0;
          dif=0;
          maxCC=lTop;
          if(aVT<10) {aVT=20;}
          for(i=lBot-1;i>lTop;i--) {
           if((hist[i]<aVT)&&(hist[i]>baV)) {
                maxCC=i;
                break;
                }
           }

          for(i=lTop;i<lBot;i++) {
           if((hist[i]<aVT)&&(hist[i]>baV)) {
                kk++;
                }
           if(kk&&(i<maxCC)) {dif+=abs(hist[i]-hist[i+1]);}
           }
          if(kk<5) {goto OMT;}
          aVT=aV;
          res=aV/10;
          if((aV%10)>4) res++;

          if(dif>kk/4) {
                aVT-=res;
                }
          if(dif>kk/2) {
                aVT-=res;
                }
          if(dif>kk) {
                aVT-=res;
                if(aVT<14) {
                    aVT=aVT/2-1;
                    if(aVT<8) {aVT--;}
                    }
                else {
                   if(dif>3*kk) {
                        aVT-=(aVT/3);
                        }
                   if(dif>5*kk) {
                        aVT-=(aVT/4);
                        }

                    }
                }
          aVT=max(0,aVT);

          aV=aVT;

          if(mm && (aV > 2)) {
             if(lBot - lTop <30)
                     aV /= 2;
             else
                     aV--;
                     }

         if(lBot - lTop > 200)
                      if(aV > 7)
                           aV -= 2;

        linLoc(lpWorkBuffer,hist,lTop,lBot,lineHeight,aV);

OMT:    countF++;
        if(maxIndxLines1==0) {
           if(countF>1) {tmpLines[indL].lIns=0;}
           }
        else {
           shiftBord(lpWorkBuffer,hist,lTop,lBot,lineHeight,aV);
           for(i=0;i<maxIndxLines1; i++) {
              kk=0;
              if((lineHeight<=0)||(lineHeight>=1000)) {
                ret=srchB(hist,lenH,lTop,lBot,tmpLines1[i].indTop,1);
                if(ret>0) {
                     tmpLines1[i].indTop=ret;
                  }
                else {kk=1;}
                }
              if(tmpLines1[i].indBot-tmpLines1[i].indTop>=tHDpi) {
                 if((lineHeight<=0)&&(lineHeight>=1000)) {
                    ret=srchB(hist,lenH,lTop,lBot,tmpLines1[i].indBot,-1);
                    if(ret>0) {
                            tmpLines1[i].indBot=ret;
                        if(tmpLines1[i].indBot-tmpLines1[i].indTop<tHDpi) {kk=1;}
                        }
                    else {kk=1;}
                    }
                 }
               else {kk=1;}
               if(kk) {
                       for(kk=i;kk<maxIndxLines1-1; kk++) {
                                tmpLines1[kk].indBot=tmpLines1[kk+1].indBot;
                                tmpLines1[kk].indTop=tmpLines1[kk+1].indTop;
                                }
                        maxIndxLines1--;
                        if(maxIndxLines1==0) {tmpLines[indL].lIns=0;}
                        i--;
                        }
               }

           if(aVLn==0) {
             aVT=0;kk=0;
             for(i=0;i<maxIndxLines1; i++) {
                aVT+=tmpLines1[i].indBot-tmpLines1[i].indTop;
                kk++;
                }
             if(kk>0) {aVLn=aVT/kk;}
             }
           if(aVLn>0) {
             for(i=0;i<maxIndxLines1; i++) {
//                if(((float)(tmpLines1[i].indBot-tmpLines1[i].indTop)/(float)aVLn)<(float)0.6) {
                if(((float)(tmpLines1[i].indBot-tmpLines1[i].indTop)/(float)aVLn)<(float)0.46) {
//                	{float f = (float)(tmpLines1[i].indBot-tmpLines1[i].indTop)/(float)aVLn;
//                	printf("tmpLines1[i].indTop= %d, tmpLines1[i].indBot= %d, f= %f\n",tmpLines1[i].indTop, tmpLines1[i].indBot, f);
//                	}
                        for(kk=i;kk<maxIndxLines1-1; kk++) {
                                tmpLines1[kk].indBot=tmpLines1[kk+1].indBot;
                                tmpLines1[kk].indTop=tmpLines1[kk+1].indTop;
                                }
                        maxIndxLines1--;
                        if(maxIndxLines1==0) {tmpLines[indL].lIns=0;}
                        }
                }
              }
           prePackL(indL);
           }

        ret=srchFstEptPl((int *)&indL,tH);
        if(ret<0) {
           prePackFinL(lpWorkBuffer);
           return(0);
           }
        lTop=tmpLines[indL].indBot;
        lBot=tmpLines[indL+1].indTop;
        lTop++;
        lBot--;
        ret=srchB(hist,lenH,lTop,lBot,lTop,1);
        if(ret>0) {lTop=ret;}
        ret=srchB(hist,lenH,lTop,lBot,lBot,-1);
        if(ret>0) {lBot=ret;}
        if(lBot-lTop<tH) {
                 maxIndxLines1=0;
                 goto OMT;
                 }

        } while(1);
    return(0);

  } /* linesUnp() */

int  linesForFontsRet_Num(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
                   int wCharType, int lRemH, int lRemV, int horLineM){
  int i, difL, shFt, kk, nn;
  int max,  y1, indFr, j, iAv;
  int tH1, tH2;
  int newHSwitch;
  float tGt, alP;
  int flgVertRemov = 0;
  LONG av;

     if(((lpWorkBuffer -> Dpi.x)>70)&&((lpWorkBuffer -> Dpi.x)<420) ) {
        tH1=(int)((float)12*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
        tH2=(int)((float)8*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
        }
     else {
        tH1=12;
        tH2=8;
        }

      enhanceHistForTextLineRec(hist, lenH);
//printf("--------------------tH1= %d, tH2= %d \n",tH1, tH2);

     indFr=0;
     shFt=2;
     kk=1;
     difL=min(lenH,MAXPROFILE);
     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);

        av=0L;
        kk=0;
        iAv=0;
        for(i=shFt;i<difL;i++) {
           if(hist[i]>0) {av+=(LONG)hist[i];kk++;}
//           if(i < 600)
//  printf("--------------------i= %d, hist[i]= %d \n",i, hist[i]);
           }
        if(kk>0) {iAv=(int)(av/(LONG)kk);}
//    printf("--------------------iAv= %d \n",iAv);

        j=iAv*3;
        av=0L;
        kk=0;
        iAv=0;
        for(i=shFt;i<difL;i++) {
           if((hist[i]>0)&&(hist[i]<j)) {av+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {iAv=(int)(av/(LONG)kk);}

//        printf("--------------------iAv= %d \n",iAv);
//            iAv = 100;

     memset(arrInd,0x00,(unsigned int)(sizeof(int)*MAXNUMBMIN));
     for(i=shFt;i<difL;i++) {
        if((hist[i]>0)&&(hist[i]<MAXNUMBMIN)) {
                arrInd[hist[i]]++;
                }
        }
     max=0;
     nn=0;
     for(i=1;i<MAXNUMBMIN/2+2;i++) {
        if(arrInd[i]>max) {
                max=arrInd[i];
                nn=i;
                }
        }
     if(arrInd[nn]<3) {nn=0;}
     nn++;
     if(iAv<tH1) {nn/=2;}
     if(iAv<tH2) {nn=1;}

     lpHistH = (int *)&histHSk[0][0];
     smoothH((int *)hist,lenH,lpHistH,30,8);

     if(horLineM >= 8) horLineM = 0;
// printf("--------------------horLineM= %d, lRemH= %d \n",horLineM, lRemH);

     if(lRemH) {
          if(horLineM) {
            linesHCellB(lpWorkBuffer,hist,lenH,20,40,alpha,scaleAngle);
            callRemLineH(lpWorkBuffer,alpha,scaleAngle);
            if(horLineM >= 8)
                    lRemH = 0;
            }
          else{
             filtrHist(lpWorkBuffer,hist,lenH,lpHistH,nn,(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),
                (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW)+(lpWorkBuffer -> Width)-1,alpha,scaleAngle);
          }
      }
     kk=0;
     for(i=shFt;i<difL;i++) {
//  printf("--------------------i= %d, hist[i]= %d \n",i, hist[i]);
        if(hist[i]>nn+4) kk++;
        }
     if(kk<10)  {nn=1;}

// printf("--------------------nn= %d, \n",nn);

     if((lpWorkBuffer->fieldType == DROPBOX) && (lineH > 0) && (lineH < 1000)){
        singleLineWh(lpHistH,lenH,lpWorkBuffer,lineH,-1,-1);
     }
     else {
        if((lpWorkBuffer->fieldType == MULTILINEBOXD) && (lineH > 0) && (lineH < 1000)){
             multiLineWh(lpHistH,lenH,lpWorkBuffer,lineH);
        }
        else {
//         	 printf("---------------linesUnp---lenH= %d, \n",lenH);
             linesUnpRet_Num(lpHistH,lenH,lpWorkBuffer,lineH);
             }
        }

     newHSwitch = 0;
       if(lRemV) {
        histVert(lpWorkBuffer);
        flgVertRemov = 0;
        if((lpWorkBuffer->fieldType != DROPBOX) && (lpWorkBuffer -> heightForVert > 0)) {
           lpHistH2 = (int *)&histHSk[1][0];
           cpyHist(lpHistH2,  lpHistH, lenH);
           if( filtrHistVert(lpWorkBuffer,(int *)&ind[1],ind[0],(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),0,
                          lpHistH,lenH, 1) ) {
               cleanHist(lpHistH,lenH);
               if((lpWorkBuffer->fieldType == MULTILINEBOXD) && (lineH > 0) && (lineH < 1000))
                   {multiLineWh(lpHistH, lenH, lpWorkBuffer, lineH);flgVertRemov = 1;}
               else
                   {linesUnpRet_Num(lpHistH,lenH,lpWorkBuffer,lineH);flgVertRemov = 1;}
               cpyHist(lpHistH,  lpHistH2, lenH);
               histVert(lpWorkBuffer);
               }
           }
        if( filtrHistVert(lpWorkBuffer,(int *)&ind[1],ind[0],(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),0,
                          lpHistH,lenH, 0) ) {
               newHSwitch = 1;
               }
        else {
               if(flgVertRemov == 1) {
                    if((lpWorkBuffer->fieldType == MULTILINEBOXD) && (lineH > 0) && (lineH < 1000))
                       multiLineWh(lpHistH, lenH, lpWorkBuffer, lineH);
                    else
                       linesUnpRet_Num(lpHistH,lenH,lpWorkBuffer,lineH);
                  }
               }
        }
     maxIndxLines=0;
     linesUnp2(lpHistH,lenH,lpWorkBuffer,nn);
     if(lRemH) {
                filtrHistInsL(lpWorkBuffer, hist, lenH, (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),
                             (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW)+(lpWorkBuffer -> Width)-1,
                             (TMPLINES *)tmpLines, maxIndxLines, alpha, scaleAngle, wCharType);
                simpleFfiltrH(lpWorkBuffer,lpHistH,lenH,
                              alpha, scaleAngle);
                }

     if(newHSwitch) {
               alP  = (float)(alpha*PI)/(float)180.0;
               alP /= (float)scaleAngle;
               tGt  = (float)tan((double)alP);
               histH_alpha(lpWorkBuffer,lpWorkBuffer->begRect.x,lpWorkBuffer->begRect.y,
                           (lpWorkBuffer->begRect.x)+(lpWorkBuffer->Width)-1,(lpWorkBuffer->begRect.y)+(lpWorkBuffer->Height)-1,
                           alpha,tGt,(int *)&hist[1],(int *)&hist[0]);
               hist[0]--;
               lenH = hist[0];
               lpHistH = (int *)&histHSk[0][0];
               smoothH((int *)hist,lenH,lpHistH,30,12);
               if((lpWorkBuffer->fieldType == DROPBOX) && (lineH > 0) && (lineH < 1000))
                   singleLineWh(lpHistH,lenH,lpWorkBuffer,lineH,-1,-1);
               else {
                   if((lpWorkBuffer->fieldType == MULTILINEBOXD) && (lineH > 0) && (lineH < 1000))
                          multiLineWh(lpHistH,lenH,lpWorkBuffer,lineH);
                   else
                          linesUnpRet_Num(lpHistH,lenH,lpWorkBuffer,lineH);
                   }
               }

     if(((lpWorkBuffer->infFrame[indFr]).nLines==0)/*&&((lpWorkBuffer->flgUNP)>0)*/) {
        if(maxIndxLines>2) {
                for(i=1;i<maxIndxLines - 1;i++) {
                     (lpWorkBuffer->line[indFr]).y1[i-1]=tmpLines[i].indTop;
                     (lpWorkBuffer->line[indFr]).y2[i-1]=tmpLines[i].indBot;
                     }
                (lpWorkBuffer->infFrame[indFr]).nLines=maxIndxLines-2;

                }
        }

//   printf("------------ linesForFonts, lpWorkBuffer->infFrame[indFr]).nLines= %d indFr= %d \n", (lpWorkBuffer->infFrame[indFr]).nLines, indFr);

//   for(j=0;j<(lpWorkBuffer->infFrame[indFr]).nLines;j++) {
//       printf("------------ line j= %d, (lpWorkBuffer->line[indFr]).y1[j]= %d (lpWorkBuffer->line[indFr]).y2[j]= %d \n", j, (lpWorkBuffer->line[indFr]).y1[j], (lpWorkBuffer->line[indFr]).y2[j]);
//   }

     return(0);

} /* linesForFontsRet_Num */


