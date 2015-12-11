/*
 * UtilRecognition2.c
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

extern     int       gr_ar[];
    LONG arrCos1[]={
    /* 0,1,2    */   1048576L,1048416L,1047937L,
    /* 3,4      */   1047139L,1046022L,
    /* 86,87,88 */   73145L,54878L,36595L,
    /* 89,90,91 */   18300L,0L,-18300L,
    /* 92,93,94 */   -36595L,-54878L,-73145L,
    /* 176, 177 */   -1046022L,-1047139L,
    /* 178, 179 */   -1047937L,-1048416L
                        };
    LONG arrSin1[]={
    /* 0,1,2    */   0L,18300L,36595L,
    /* 3,4      */   54878L,73145L,
    /* 86,87,88 */   1046022L,1047139L,1047937L,
    /* 89,90,91 */   1048416L,1048576L,1048416L,
    /* 92,93,94 */   1047937L,1047139L,1046022L,
    /* 176, 177 */   73145L,54878L,
    /* 178, 179 */   36595L, 18300L
                        };
    double arrCos[]={
    /* 0,1,2       */   1.0,0.9998476951564,0.9993908270191,
    /* 3,4         */   0.9986295347546,0.9975640502598,
    /* 86,87,88    */   0.06975647374413,0.052335956624294,0.0348994967025,
    /* 89,90,91    */   0.01745240643728,0.0,-0.01745240643728,
    /* 92,93,94    */   -0.0348994967025,-0.052335956624294,-0.06975647374413,
    /* 176, 177    */   -0.9975640502598,-0.9986295347546,
    /* 178, 179    */   -0.9993908270191,-0.9998476951564
                        };
    double arrSin[]={
    /* 0,1,2       */   0,0.01745240643728,0.0348994967025,
    /* 3,4         */   0.05233595624294,0.06975647374413,
    /* 86,87,88    */   0.9975640502598,0.9986295347546,0.9993908270191,
    /* 89,90,91    */   0.9998476951564,1.0,0.9998476951564,
    /* 92,93,94    */   0.9993908270191, 0.9986295347546,0.9975640502598,
    /* 176, 177    */   0.06975647374413,0.05233595624294,
    /* 178, 179    */   0.0348994967025, 0.01745240643728
                        };
    int arrTet[]={0,1,2,3,4,86,87,88,89,90,91,92,93,94,176,177,178,179};

void initRemStruct(  SREMOVAL  *lpData    ){

     if(lpData->iMinLH<=2)        lpData->iMinLH=100;

     lpData->iMaxHGapH     = 2;
     lpData->iMaxVGapH     = 0;
     lpData->iMaxWidthH    = 10;
     lpData->iMinDistH     = 1;
     lpData->iMaxDifShapeH = 2;

     if(lpData->iMinLV<=2)        lpData->iMinLV=64;

     lpData->iMaxVGapV     = 2;
     lpData->iMaxHGapV     = 0;
     lpData->iMaxWidthV    = 10;
     lpData->iMinDistV     = 1;
     lpData->iMaxDifShapeV = 2;

     if(lpData->iSpeckSizeH<0)    lpData->iSpeckSizeH=2;
     if(lpData->iSpeckSizeV<0)    lpData->iSpeckSizeV=2;

     lpData->iRemoval      = 1;
     lpData->iFiltration   = 0;

     lpData->iCellWidth    = 0;


 } /* of initRemStruct() */
int cleanSpecksOut( SREMOVAL  *lpData  ){

  int x, y, x1, wImg, i, ch, ind, mask, ret, yy;
  LONG lY;
  BYTE  *hptr;
  int xSt;


    if( (lpData->iXBegin)+(lpData->iWidth)>=(lpData->iLineWidth) ) (lpData->iWidth)=(lpData->iLineWidth)-(lpData->iXBegin)-1;
    if( ((lpData->iImgHeight)<MINSIZE) || ((lpData->iLineWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( (lpData->iYBegin)+(lpData->iHeight)>=(lpData->iImgHeight) ) (lpData->iHeight)=(lpData->iImgHeight)-(lpData->iYBegin)-1;
    if( ((lpData->iHeight)<MINSIZE) || ((lpData->iWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( ((lpData->iHeight)>MAXSIZE) || ((lpData->iWidth)>MAXSIZE) ) return(SIZE_ERR2);
    if( ((lpData->iXBegin)>(lpData->iLineWidth)) || ((lpData->iYBegin)>(lpData->iImgHeight)) ) return(SIZE_ERR3);

    if((lpData->iSpeckSizeH)<0) (lpData->iSpeckSizeH)=0;
    if((lpData->iSpeckSizeV)<0) (lpData->iSpeckSizeV)=0;

//    printf("lpData->iSpeckSizeH = %d, lpData->iImgHeight= %d, lpData->iHeight= %d \n",lpData->iSpeckSizeH, lpData->iImgHeight, lpData->iHeight);
//	  fflush(stdout);
//    ySt, xSt;
    wImg  =(((lpData->iLineWidth)+31)/32)*4;
              arrExcF((lpData->iSpeckSizeH));
              x1=(lpData->iXBegin)/8;
              x =x1*8;
              y =(lpData->iYBegin);

              while(y<(lpData->iYBegin)+(lpData->iHeight))  {
                x =x1*8;
                yy=y;

                yy=(lpData->iImgHeight)-1-y;
                lY=(LONG)yy*(LONG)wImg;
                hptr=(lpData->lpPointer)+lY+(LONG)x1;

//            printf("lpData->lpPointer= %lu, y= %d, hptr= %lu\n", (unsigned long)lpData->lpPointer, y, (unsigned long)hptr);
//           	fflush(stdout);

                xSt = (lpData->iXBegin)+(lpData->iWidth)-250;
                while(x<(lpData->iXBegin)+(lpData->iWidth)) {
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
               	  if(x < 250 || x> xSt) {
                        if(arrExc[ind]) {
                            for(i=0;i<8;i++) {
                              mask=LEFT_BIT;
                              mask >>=i;
                              ch=(~hptr[0])&mask;
                              if(ch!=0) {
                                if((ret=sizeSpot(lpData,x+i,y))) {
                                        cleanSpeck(lpData,x+i,y);
                                        }
                                }
                              }

                            }
               	  }
                        hptr++;
                        x+=8;
                        }

                y++;
                }

        return(0);

} /* of cleanSpecks() */


int cleanSpecks( SREMOVAL  *lpData  ){

  int x, y, x1, wImg, i, ch, ind, mask, ret, yy;
  LONG lY;
  BYTE  *hptr;


    if( (lpData->iXBegin)+(lpData->iWidth)>=(lpData->iLineWidth) ) (lpData->iWidth)=(lpData->iLineWidth)-(lpData->iXBegin)-1;
    if( ((lpData->iImgHeight)<MINSIZE) || ((lpData->iLineWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( (lpData->iYBegin)+(lpData->iHeight)>=(lpData->iImgHeight) ) (lpData->iHeight)=(lpData->iImgHeight)-(lpData->iYBegin)-1;
    if( ((lpData->iHeight)<MINSIZE) || ((lpData->iWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( ((lpData->iHeight)>MAXSIZE) || ((lpData->iWidth)>MAXSIZE) ) return(SIZE_ERR2);
    if( ((lpData->iXBegin)>(lpData->iLineWidth)) || ((lpData->iYBegin)>(lpData->iImgHeight)) ) return(SIZE_ERR3);

    if((lpData->iSpeckSizeH)<0) (lpData->iSpeckSizeH)=0;
    if((lpData->iSpeckSizeV)<0) (lpData->iSpeckSizeV)=0;

//    printf("lpData->iSpeckSizeH = %d, lpData->iImgHeight= %d, lpData->iHeight= %d \n",lpData->iSpeckSizeH, lpData->iImgHeight, lpData->iHeight);
//	  fflush(stdout);

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
              arrExcF((lpData->iSpeckSizeH));
              x1=(lpData->iXBegin)/8;
              x =x1*8;
              y =(lpData->iYBegin);

              while(y<(lpData->iYBegin)+(lpData->iHeight))  {
                x =x1*8;
                yy=y;

                yy=(lpData->iImgHeight)-1-y;
                lY=(LONG)yy*(LONG)wImg;
                hptr=(lpData->lpPointer)+lY+(LONG)x1;

//            printf("lpData->lpPointer= %lu, y= %d, hptr= %lu\n", (unsigned long)lpData->lpPointer, y, (unsigned long)hptr);
//           	fflush(stdout);


                while(x<(lpData->iXBegin)+(lpData->iWidth)) {
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        if(arrExc[ind]) {
                            for(i=0;i<8;i++) {
                              mask=LEFT_BIT;
                              mask >>=i;
                              ch=(~hptr[0])&mask;
                              if(ch!=0) {
                                if((ret=sizeSpot(lpData,x+i,y))) {
                                        cleanSpeck(lpData,x+i,y);
                                        }
                                }
                              }

                            }
                        hptr++;
                        x+=8;
                        }
                y++;
                }

        return(0);

} /* of cleanSpecks() */

void  remBlackA(STRUCTWB *lpWorkBuffer, int x1, int y1, int x2, int y2){
  int sww, i ,j, dx, x1D8, dy, sum, ind, w_bt, dx1;
  int iy, jx, brL, brR, endL, blT;
  int ccB, yMin, yMax;
  LONG nY, nY1;
  BYTE *hptr;
  BYTE *hptrN;
  BYTE *hptrH;

              blT = 7;
              w_bt = (lpWorkBuffer -> widthImg);
              nY   = (long)w_bt;
              x1D8 = x1/8;
              dy   = y2-y1+1;
              dx   = x2/8-x1D8-1;
              dx1  = dx - 2;
              brL  = -1;
              if(x1D8>0) {brL--;}
              brR  = dx;
              if(x1D8+dx+2<w_bt) {brR++;}
              i    = 2;
              while(i<dy-3)  {
                sum=0;
                sww=y1+i;


                sww=(lpWorkBuffer -> hightImg)-1-sww;
                nY1=(LONG)sww*(LONG)w_bt;
                hptr=(lpWorkBuffer->lpRBuffer)+nY1+(LONG)x1D8;

                hptr++;
                hptr++;
                for(j=1;j<dx1;j+=2)  {
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        if(gr_ar[ind]>6) {
                            ind=(int)(*(hptr-nY));
                            if(ind<0) {ind+=256;}
                            ind=255-ind;
                            if(gr_ar[ind]<blT) {hptr+=2;continue;}
                            ind=(int)(*(hptr-nY-nY));
                            if(ind<0) {ind+=256;}
                            ind=255-ind;
                            if(gr_ar[ind]<blT) {hptr+=2;continue;}

                            ind=(int)(*(hptr+nY));
                            if(ind<0) {ind+=256;}
                            ind=255-ind;
                            if(gr_ar[ind]<blT) {hptr+=2;continue;}
                            ind=(int)(*(hptr+nY+nY));
                            if(ind<0) {ind+=256;}
                            ind=255-ind;
                            if(gr_ar[ind]<blT) {hptr+=2;continue;}

                            ccB = 1;
                            ind=(int)(*(hptr-1L));
                            if(ind<0) {ind+=256;}
                            ind=255-ind;
                            if(gr_ar[ind]<blT) {ccB = 0;}
                            if(ccB) {
                               ind=(int)(*(hptr-1L-nY));
                               if(ind<0) {ind+=256;}
                               ind=255-ind;
                               if(gr_ar[ind]<blT) {ccB = 0;}
                               }
                            if(ccB) {
                               ind=(int)(*(hptr-1L+nY));
                               if(ind<0) {ind+=256;}
                               ind=255-ind;
                               if(gr_ar[ind]<blT) {ccB = 0;}
                                }
                            if(!ccB) {
                               ccB = 1;
                               ind=(int)(*(hptr+1L));
                               if(ind<0) {ind+=256;}
                               ind=255-ind;
                               if(gr_ar[ind]<blT) {ccB = 0;}
                               if(ccB) {
                                  ind=(int)(*(hptr+1L-nY));
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {ccB = 0;}
                                  }
                               if(ccB) {
                                  ind=(int)(*(hptr+1L+nY));
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {ccB = 0;}
                                  }
                                }

                            if(!ccB) {hptr+=2;continue;}

                            jx = j;
                            hptrH = hptr;
                            yMin = yMax = 0;
                            while(jx>brL) {
                              iy=i;
                              hptrN = hptrH;
                              endL = 0;
                              while(iy>0) {
                                  ind=(int)(*hptrN);
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {
                                        *hptrN = 255;
                                        if(!yMin) {yMin = iy;}
                                        break;
                                        }
                                  *hptrN = 255;
                                  hptrN += nY;
                                  iy--;
                                }
                              if(i-iy == 0) {endL++;}
                              iy=i+1;
                              hptrN = hptrH-nY;
                              while(iy<dy) {
                                  ind=(int)(*hptrN);
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {
                                        *hptrN = 255;
                                        if(!yMax) {yMax = iy;}
                                        break;
                                        }
                                  *hptrN = 255;
                                  hptrN -= nY;
                                  iy++;
                                }
                              if(iy-i-1 == 0) {
                                   if(endL) {break;}
                                   }
                              hptrH--;
                              jx--;
                              }

                            if(jx>0) {
                              hptrN = hptrH;
                              if(yMin) {
                                for(iy=i;iy>=yMin;iy--) {
                                  *hptrN = 255;
                                  hptrN += nY;
                                  }
                                }
                              if(yMax) {
                                hptrN = hptrH;
                                for(iy=i+1;iy<=yMax;iy++) {
                                  *hptrN = 255;
                                  hptrN -= nY;
                                  }
                                }
                              }

                            jx = j+1;
                            hptrH = hptr+1;
                            yMin = yMax =0;
                            while(jx<brR) {
                              iy=i;
                              hptrN = hptrH;
                              endL = 0;
                              while(iy>0) {
                                  ind=(int)(*hptrN);
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {
                                        *hptrN = 255;
                                        if(!yMin) {yMin = iy;}
                                        break;
                                        }
                                  *hptrN = 255;
                                  hptrN += nY;
                                  iy--;
                                }
                              if(i-iy == 0) {endL++;}
                              iy=i+1;
                              hptrN = hptrH-nY;
                              while(iy<dy) {
                                  ind=(int)(*hptrN);
                                  if(ind<0) {ind+=256;}
                                  ind=255-ind;
                                  if(gr_ar[ind]<blT) {
                                        *hptrN = 255;
                                        if(!yMax) {yMax = iy;}
                                        break;
                                        }
                                  *hptrN = 255;
                                  hptrN -= nY;
                                  iy++;
                                }
                              if(iy-i-1 == 0) {
                                   if(endL) {break;}
                                   }
                              hptrH++;
                              jx++;
                              }
                            if(jx<dx-1) {
                              hptrN = hptrH;

                              if(yMin) {
                                for(iy=i;iy>=yMin;iy--) {
                                  *hptrN = 255;
                                  hptrN += nY;
                                  }
                                }

                              hptrN = hptrH;
                              if(yMax) {
                                for(iy=i+1;iy<=yMax;iy++) {
                                  *hptrN = 255;
                                  hptrN -= nY;
                                  }
                                }
                              }

                           }
                        hptr+=2;
                        }
                i++;
                }

 }/* remBlackA() */

/***************************************************************************
 *                                                                         *
 *  FUNCTION:      int searchCloseP()                                      *
 *                                                                         *
 *  PURPOSE:       search for the closest rectangle                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int searchCloseP( SREMOVAL *lpData,  POINT *lTopP, POINT *lBottomP,  POINT *rTopP, POINT *rBottomP, int typeField){
  int x, y, lB, rB, tB, bB, ind, tHV, cVL, yy;
  int mass, tH, OK, tHmass, tH2, iImg;
  BYTE  *hptr;
  LONG nY;

    iImg  =(((lpData->iLineWidth)+31)/32)*4;
       tH=8;
       if(typeField!=0) {
        tHmass=2;
        tH2=8;
        tHV=4;
        }
      else  {
        tHmass=1;
        tH2=6;
        tHV=0;
        }

       (lTopP->x)=(lpData -> iXBegin);
       (lTopP->y)=(lpData -> iYBegin);
       (lBottomP->x)=(lpData -> iXBegin);
       (lBottomP->y)=(lpData -> iYBegin)+(lpData -> iHeight)-1;
       (rTopP->x)=(lpData -> iXBegin)+(lpData -> iWidth)-1;
       (rTopP->y)=(lpData -> iYBegin);
       (rBottomP->x)=(lpData -> iXBegin)+(lpData -> iWidth)-1;
       (rBottomP->y)=(lpData -> iYBegin)+(lpData -> iHeight)-1;

/* left x */

       lB=(lTopP->x);
       rB=(rTopP->x)-tH;
       tB=(lTopP->y);
       bB=(lBottomP->y);
       x=lB/8;
       OK=1;
       while(x<rB/8) {
           y=tB;
           mass=0;
           cVL=0;
           while(y<bB) {
                yy=y;
                yy=(lpData -> iImgHeight)-1-y;
                nY=(LONG)yy*(LONG)iImg;
                hptr=(lpData->lpPointer)+nY+(LONG)x;
                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                mass+=gr_ar[ind];
                if(gr_ar[ind]>0) cVL++;
                y++;
                }
           if((mass>tHmass)&&(cVL>tHV)) {
                OK=0;
                break;
                }
           x++;
           }
        if(!OK) {
           lB=x*8;
           rB=min(lB+8,(rTopP->x)-tH);
           tB=(lTopP->y);
           bB=(lBottomP->y);
           OK=1;
           x=lB;
           while(x<rB) {
               y=tB;
               mass=0;
               while(y<bB) {
                    mass+=whatIsThisPreP((lpData->lpPointer),iImg,(lpData->iImgHeight),x,y);
                    y++;
                    }
               if(mass>tHmass) {
                  OK=0;
                  break;
                  }
               x++;
               }
           if(!OK) {
                (lTopP->x)=max(x-tH2,(lpData->iXBegin));
                (lBottomP->x)=max(x-tH2,(lpData->iXBegin));
                }
           }


/* right x */
       lB=(lTopP->x)+tH;
       rB=(rTopP->x);
       x=rB/8;
       OK=1;
       while(x>lB/8) {
           y=tB;
           mass=0;
           cVL=0;
           while(y<bB) {
                yy=y;
                yy=(lpData -> iImgHeight)-1-y;
                nY=(LONG)yy*(LONG)iImg;
                hptr=(lpData->lpPointer)+nY+(LONG)x;
                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                mass+=gr_ar[ind];
                if(gr_ar[ind]>0) cVL++;
                y++;
                }
           if((mass>tHmass)&&(cVL>tHV)) {
                OK=0;
                break;
                }
           x--;
           }
        if(!OK) {
           rB=x*8+7;
           lB=max(rB-8,(lTopP->x));
           x=rB;
           OK=1;
           while(x>lB) {
               y=tB;
               mass=0;
               while(y<bB) {
                    mass+=whatIsThisPreP((lpData->lpPointer),iImg,(lpData->iImgHeight),x,y);
                    y++;
                    }
               if(mass>tHmass) {
                  OK=0;
                  break;
                  }
               x--;
               }
           if(!OK) {
                (rTopP->x)=min(x+tH2,(lpData->iXBegin)+(lpData -> iWidth)-1);
                (rBottomP->x)=min(x+tH2,(lpData->iXBegin)+(lpData -> iWidth)-1);
                }
           }
/* Top y */

       lB=(lTopP->x)/8;
       rB=(rTopP->x)/8;
       tB=(lTopP->y);
       bB=(lBottomP->y)-tH;
       y=tB;
       OK=1;
       while(y<bB) {
           x=lB;
           mass=0;
           yy=y;
           yy=(lpData -> iImgHeight)-1-y;
           nY=(LONG)yy*(LONG)iImg;
           hptr=(lpData->lpPointer)+nY+(LONG)x;
           while(x<rB) {
                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                mass+=gr_ar[ind];
                x++;
                hptr++;
                }
           if(mass>tHmass) {
                OK=0;
                break;
                }
           y++;
           }
        if(!OK) {
                (lTopP->y)=max(y-tH2,(lpData->iYBegin));
                (rTopP->y)=max(y-tH2,(lpData->iYBegin));
                }

/* Bottom y */

       tB=(lTopP->y)+tH;
       bB=(lBottomP->y);
       y=bB;
       OK=1;
       while(y>tB) {
           x=lB;
           mass=0;
           yy=y;
           yy=(lpData -> iImgHeight)-1-y;
           nY=(LONG)yy*(LONG)iImg;
           hptr=(lpData->lpPointer)+nY+(LONG)x;
           while(x<rB) {
                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                mass+=gr_ar[ind];
                x++;
                hptr++;
                }
           if(mass>tHmass) {
                OK=0;
                break;
                }
           y--;
           }
        if(!OK) {
                (lBottomP->y)=min(y+tH2,(lpData->iYBegin)+(lpData -> iHeight)-1);
                (rBottomP->y)=min(y+tH2,(lpData->iYBegin)+(lpData -> iHeight)-1);
                }

       return(0);

} /* of searchCloseP() */
/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :clean almost horizontal and vertical lines               *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int cleanLines( SREMOVAL *lpData){
    int xSt, ySt, i, j, direct, x, y, ret, tet, tHH, jj;
    int xAct, yAct, widthAct, widthAct1, heightAct, mlLinV, mlLinH;
    int nH, nV, rH, rV, iH, iV, xx, yy = 0, lL, gap;

    if( (lpData->iXBegin)+(lpData->iWidth)>=(lpData->iLineWidth) ) (lpData->iWidth)=(lpData->iLineWidth)-(lpData->iXBegin)-1;
    if( ((lpData->iImgHeight)<MINSIZE) || ((lpData->iLineWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( (lpData->iYBegin)+(lpData->iHeight)>=(lpData->iImgHeight) ) (lpData->iHeight)=(lpData->iImgHeight)-(lpData->iYBegin)-1;
    if( ((lpData->iHeight)<MINSIZE) || ((lpData->iWidth)<MINSIZE) ) return(SIZE_ERR1);
    if( ((lpData->iHeight)>MAXSIZE) || ((lpData->iWidth)>MAXSIZE) ) return(SIZE_ERR2);
    if( ((lpData->iXBegin)>(lpData->iLineWidth)) || ((lpData->iYBegin)>(lpData->iImgHeight)) ) return(SIZE_ERR3);
    (lpData->iMethod)=0;

    if((lpData->iMinLH)<0) (lpData->iMinLH)=0;
    if((lpData->iMinLV)<0) (lpData->iMinLV)=0;
    if((lpData->iMaxHGapH)<0) (lpData->iMaxHGapH)=0;
    if((lpData->iMaxVGapH)<0) (lpData->iMaxVGapH)=0;
    if((lpData->iMaxHGapV)<0) (lpData->iMaxHGapV)=0;
    if((lpData->iMaxVGapV)<0) (lpData->iMaxVGapV)=0;
    if((lpData->iMaxWidthH)<0) (lpData->iMaxWidthH)=0;
    if((lpData->iMaxWidthV)<0) (lpData->iMaxWidthV)=0;
    if((lpData->iMinDistH)<0) (lpData->iMinDistH)=0;
    if((lpData->iMinDistV)<0) (lpData->iMinDistV)=0;
    if((lpData->iMaxDifShapeH)<0) (lpData->iMaxDifShapeH)=0;
    if((lpData->iMaxDifShapeV)<0) (lpData->iMaxDifShapeV)=0;

         initTeta();

         mlLinH=(lpData->iWidth);
         mlLinV=(lpData->iHeight);

         widthAct1=(lpData->iWidth);
         heightAct=(lpData->iHeight);
         nH=1;rH=0;
         nV=1;rV=0;
         if((lpData->iWidth)>MAXRECTSIDE) {
                nH=(lpData->iWidth)/MAXRECTSIDE + 1;
                rH=(lpData->iWidth)%nH;
                widthAct1=(lpData->iWidth)/nH;
                }
         if((lpData->iHeight)>MAXRECTSIDE) {
                nV=(lpData->iHeight)/MAXRECTSIDE + 1;
                rV=(lpData->iHeight)%nV;
                heightAct=(lpData->iHeight)/nV;
                }

         direct=1;
         iV=0;
       while(iV<nV) {
          yAct=(lpData->iYBegin)+iV*heightAct;
          if(iV==nV-1) heightAct+=rV;
          if(yAct+heightAct>(lpData->iYBegin)+(lpData->iHeight)) heightAct=(lpData->iYBegin)+(lpData->iHeight)-yAct-1;
          iH=0;widthAct=widthAct1;
          while(iH<nH) {
            xAct=(lpData->iXBegin)+iH*widthAct;
            if(iH==nH-1) widthAct+=rH;
            if(xAct+widthAct>(lpData->iXBegin)+(lpData->iWidth)) widthAct=(lpData->iXBegin)+(lpData->iWidth)-xAct-1;
            indxYST=0;
            indxXST=0;
            indxYFT=0;
            indxXFT=0;

            compAccum(lpData,xAct,yAct,widthAct,heightAct,2);

            for(i=0;i<indxOutp;i++) {
              if(arrOutp[i][2]==0) {
                continue;
                }
              if(abs(arrOutp[i][1]-90)<6) { /* horizontal line */
                for(j=0;j<=mlLinH;j++) {
                   arrLine[j][0]=-1;
                   }
                xSt=xAct;
                tet=invTet[arrOutp[i][1]];
                y=(int)((((double)arrOutp[i][0]+((double)widthAct/2.0)*arrCos[tet])/arrSin[tet])+0.5);
                ySt=(yAct+heightAct/2)-y;
                ySt=max(ySt,yAct);
                ySt=min(ySt,yAct+heightAct-1);
                if(ySt<(lpData->iYBegin)) goto END1;
                if(ySt>=(lpData->iYBegin)+(lpData->iHeight)) goto END1;

                for(j=0;j<indxYST;j++) {
                        if(abs(arrYST[j]-ySt)<=(lpData->iMinDistH)) {
                                goto END1;
                                }
                        }
                for(j=0;j<indxYFT;j++) {
                        if(arrYFT[j]==ySt) {
                                goto END1;
                                }
                        }

                ret=hLineTrack(lpData,xAct,yAct,widthAct,heightAct,xSt,ySt,direct);

                for(j=0;j<indxTr;j++) {
                   arrLine[xSt+j*direct-(lpData->iXBegin)][0]=arrTrace[j][0];
                   arrLine[xSt+j*direct-(lpData->iXBegin)][1]=arrTrace[j][1];
                   }

                  xx=0;
                  for(jj=indxTr-1;jj>indxTr-(lpData->iMaxHGapH)-2;jj--) {
                        if(arrTrace[jj][0]>0) {
                                yy=(arrTrace[jj][0]+arrTrace[jj][1])/2;
                                xx=xSt+jj*direct;
                                break;
                                }
                        }

                   if(xx>0) {
                     ret=hLineTrackGap(lpData,xx,yy,1);
                     }

                  xx=0;lL=0;gap=0;
                  tHH=min((lpData->iMinLH)/2,MAXRECTSIDE/4);

                  for(jj=0;jj<indxTr;jj++) {
                        if(arrTrace[jj][0]!=-1) {
                                lL++;
                                gap=0;
                                if((lL>tHH)&&(arrTrace[jj][0]>0)) {
                                      yy=(arrTrace[jj][0]+arrTrace[jj][1])/2;
                                      xx=xSt+jj*direct;
                                      break;
                                    }
                                }
                        else {
                                gap++;
                                if(gap>(lpData->iMaxHGapH)) lL=0;
                                }
                        }
                  if((xx==0)&&(lL>0)) {
                    for(jj=xSt+(indxTr-1)*direct-(lpData->iXBegin);jj<mlLinH;jj++) {
                        if(arrLine[jj][0]!=-1) {
                                lL++;
                                gap=0;
                                if((lL>tHH)&&(arrLine[jj][0]>0)) {
                                      yy=(arrLine[jj][0]+arrLine[jj][1])/2;
                                      xx=jj+(lpData->iXBegin);
                                      break;
                                    }
                                }
                        else {
                                gap++;
                                if(gap>(lpData->iMaxHGapH)) break;
                                }
                        }
                   }

                   if(xx>0) {
                     ret=hLineTrackGap(lpData,xx,yy,-1);
                     }


                   if(clLineH(lpData,mlLinH)) {

                        if(indxYST<MAXARROUTP) {
                             arrYST[indxYST]=ySt;
                             indxYST++;
                             }
                        }
                   else {
                        if(indxYFT<MAXARROUTP) {
                             arrYFT[indxYFT]=ySt;
                             indxYFT++;
                             }

                        }
                }
              else {                         /* vertical line */
                for(j=0;j<=mlLinV;j++) {
                   arrLine[j][0]=-1;
                   }
                ySt=yAct;
                tet=invTet[arrOutp[i][1]];
                x  =(int)((((double)arrOutp[i][0]-((double)heightAct/2.0)*arrSin[tet])/arrCos[tet])+0.5);
                xSt=(xAct+widthAct/2)+x;
                xSt=max(xSt,xAct);
                xSt=min(xSt,xAct+widthAct-1);
                if(xSt<(lpData->iXBegin)) goto END1;
                if(xSt>=(lpData->iXBegin)+(lpData->iWidth)) goto END1;

                for(j=0;j<indxXST;j++) {
                        if(abs(arrXST[j]-xSt)<=(lpData->iMinDistV)) {
                                goto END1;
                                }
                        }
                for(j=0;j<indxXFT;j++) {
                        if(arrXFT[j]==xSt) {
                                goto END1;
                                }
                        }
                ret=vLineTrack(lpData,xAct,yAct,widthAct,heightAct,xSt,ySt,direct);
                for(j=0;j<indxTr;j++) {
                   arrLine[ySt+j*direct-(lpData->iYBegin)][0]=arrTrace[j][0];
                   arrLine[ySt+j*direct-(lpData->iYBegin)][1]=arrTrace[j][1];
                   }

                  yy=0;xx=0;
                  for(jj=indxTr-1;jj>indxTr-(lpData->iMaxVGapV)-2;jj--) {
                        if(arrTrace[jj][0]>0) {
                                xx=(arrTrace[jj][0]+arrTrace[jj][1])/2;
                                yy=ySt+jj*direct;
                                break;
                                }
                        }
                   if(yy>0) {
                     ret=vLineTrackGap(lpData,xx,yy,1);
                     }

                  yy=0;lL=0;gap=0;xx=0;
                  tHH=min((lpData->iMinLV)/2,MAXRECTSIDE/4);

                  for(jj=0;jj<indxTr;jj++) {
                        if(arrTrace[jj][0]!=-1) {
                                lL++;
                                gap=0;
                                if((lL>tHH)&&(arrTrace[jj][0]>0)) {
                                      xx=(arrTrace[jj][0]+arrTrace[jj][1])/2;
                                      yy=ySt+jj*direct;
                                      break;
                                    }
                                }
                        else {
                                gap++;
                                if(gap>(lpData->iMaxVGapV)) lL=0;
                                }
                        }
                  if((yy==0)&&(lL>0)) {
                    for(jj=ySt+(indxTr-1)*direct-(lpData->iYBegin);jj<mlLinV;jj++) {
                        if(arrLine[jj][0]!=-1) {
                                lL++;
                                gap=0;
                                if((lL>tHH)&&(arrLine[jj][0]>0)) {
                                      xx=(arrLine[jj][0]+arrLine[jj][1])/2;
                                      yy=jj+(lpData->iYBegin);
                                      break;
                                    }
                                }
                        else {
                                gap++;
                                if(gap>(lpData->iMaxVGapV)) break;
                                }
                        }
                   }

                   if(yy>0) {
                     ret=vLineTrackGap(lpData,xx,yy,-1);
                     }
                   if(clLineV(lpData, mlLinV)) {
                        if(indxXST<MAXARROUTP) {
                             arrXST[indxXST]=xSt;
                             indxXST++;
                             }
                        }
                   else {
                        if(indxXFT<MAXARROUTP) {
                             arrXFT[indxXFT]=xSt;
                             indxXFT++;
                             }
                        }
                }

END1:;
              }

           iH++;
           } /* while iH */
         iV++;
         }   /* while iV */

return(0);

} /* of cleanLines() */
/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :compute accumulators                                     *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int compAccum(
   SREMOVAL  *lpData,     /* structure for removal */
   int          xAct,        /* X offset in bitmap (of rectangle area) in pixels */
   int          yAct,        /* Y offset in bitmap (of rectangle area) in pixels */
   int          widthAct,    /* width of rectangle area */
   int          heightAct,   /* height of rectangle area */
   int          VertHor      /* Vertical or/and horizontal lines */
                             /* 0 horizontal, 1 vertical, 2 hor. and vert */
   )

{
   int maxR, tet, i, ii, j, qt, id2, cGap, wImg;
   int x, y, xr, yr, yy, srT, mm, max1 = 0, idm1 = 0, ind;
   int tHH, tHV, test, fTh, lTh, outp0, outp1, outp2;
   int k1,xE, xrB, vR, lastJ;
  float ro;
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;
  LONG  ro1;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
/* quant for R */
    maxR=(int)(sqrt((double)((float)widthAct*(float)widthAct+(float)heightAct*(float)heightAct))+0.5);
    quantR=(maxR/MAXQUANTR)+1;
    indxOutp=0;
    if(maxR>=MAXQUANTR) return(-1);
    memset(arrAccum,0,4*MAXQUANTR*MAXQUANTTETA);
    srT=maxR/2;
    x=xAct/8;
    k1=xAct%8;
if(quantR==1) {
/* computation two-parameter formula for each x,y */
  xr=-widthAct/2;
  if(k1>0) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
             xr=-widthAct/2;
             yy=y;
             yy=(lpData->iImgHeight)-1-y;
             lY=(LONG)yy*(LONG)wImg;
             hptr=(lpData->lpPointer)+lY+(LONG)x;
             ind=(int)(*hptr);
             if(ind<0) {ind+=256;}
             ind=255-ind;
             if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               mask >>=k1;
               for(i=k1;i<8;i++) {
//                ch=(~hptr[0])&mask;
                ch=(ind)&mask;
                if(ch!=0) {

                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                   }
                mask >>=1;
                xr++;
                }
              }/* if ind */
             y++;
             yr--;
             }
       x++;
       xr=-widthAct/2+8-k1;
       }/* k1 */

  xE=(xAct+widthAct)/8;
  xrB=xr;
  while(x<xE) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
              xr=xrB;
              yy=y;
              yy=(lpData->iImgHeight)-1-y;
              lY=(LONG)yy*(LONG)wImg;
              hptr=(lpData->lpPointer)+lY+(LONG)x;
              ind=(int)(*hptr);
              if(ind<0) {ind+=256;}
              ind=255-ind;
              if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               for(i=0;i<8;i++) {
                ch=ind&mask;
                if(ch!=0) {

                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                   }
                  mask >>=1;
                  xr++;
                  }
                }/* if ind */
              y++;
              yr--;
              }
        x++;
        xrB+=8;
        }

  k1=(xAct+widthAct-1)%8;
  xrB=(xAct+widthAct-1)/8;
  x=xrB;
  xrB*=8;
  xrB=xrB-xAct-widthAct/2;
  if(k1<7) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
             xr=xrB;
             yy=y;
             yy=(lpData->iImgHeight)-1-y;
             lY=(LONG)yy*(LONG)wImg;
             hptr=(lpData->lpPointer)+lY+(LONG)x;
             ind=(int)(*hptr);
             if(ind<0) {ind+=256;}
             ind=255-ind;
             if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               for(i=0;i<=k1;i++) {
                ch=(ind)&mask;
                if(ch!=0) {

                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro1=(LONG)xr*arrCos1[tet]+(LONG)yr*arrSin1[tet];
/* R quantization */
                      qt=(int)(ro1/SCALE);
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }
                   }
                mask >>=1;
                xr++;
                }
              }/* if ind */
             y++;
             yr--;
             }
       }/* k1 */

     }
else {
/* computation two-parameter formula for each x,y */
   xr=-widthAct/2;
   if(k1>0) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
             xr=-widthAct/2;
             yy=y;
             yy=(lpData->iImgHeight)-1-y;
             lY=(LONG)yy*(LONG)wImg;
             hptr=(lpData->lpPointer)+lY+(LONG)x;
             ind=(int)(*hptr);
             if(ind<0) {ind+=256;}
             ind=255-ind;
             if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               mask >>=k1;
               for(i=k1;i<8;i++) {
               ch=ind&mask;
                if(ch!=0) {

                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor==0) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                     }
                   }
                 mask >>=1;
                 xr++;
                 }
                }/* ind */
              y++;
              yr--;
              }
        x++;
        xr=-widthAct/2+8-k1;
        }/* k1 */

  xE=(xAct+widthAct)/8;
  xrB=xr;
  while(x<xE) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
              xr=xrB;
              yy=y;
              yy=(lpData->iImgHeight)-1-y;
              lY=(LONG)yy*(LONG)wImg;
              hptr=(lpData->lpPointer)+lY+(LONG)x;
              ind=(int)(*hptr);
              if(ind<0) {ind+=256;}
              ind=255-ind;
              if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               for(i=0;i<8;i++) {
                ch=ind&mask;
                if(ch!=0) {
                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor==0) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                     }
                   }
                  mask >>=1;
                  xr++;
                  }
                }/* if ind */
              y++;
              yr--;
              }
        x++;
        xrB+=8;
        }

  k1=(xAct+widthAct-1)%8;
  xrB=(xAct+widthAct-1)/8;
  x=xrB;
  xrB*=8;
  xrB=xrB-xAct-widthAct/2;
  if(k1<7) {
    yr=heightAct/2;
    y=yAct;
        while(y<yAct+heightAct) {
             xr=xrB;
             yy=y;
             yy=(lpData->iImgHeight)-1-y;
             lY=(LONG)yy*(LONG)wImg;
             hptr=(lpData->lpPointer)+lY+(LONG)x;
             ind=(int)(*hptr);
             if(ind<0) {ind+=256;}
             ind=255-ind;
             if(gr_ar[ind]>0) {
               mask=LEFT_BIT;
               for(i=0;i<=k1;i++) {
                ch=(~hptr[0])&mask;
                if(ch!=0) {

                  if(VertHor) {
                   for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if((VertHor==0)||(VertHor==2)) {
                   for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                    }

                  if(VertHor==0) {
                   for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
                      ro=(float)((double)xr*arrCos[tet]+(double)yr*arrSin[tet]);
/* R quantization */
                      mm=1;
                      if(ro<(float)0.0) {
                        ro=-ro;
                        mm=-1;
                        }
                      qt=(int)((ro/(float)quantR)+(float)0.5);
                      qt*=mm;
                      qt=qt+srT;
                      if((qt>=0)&&(qt<MAXQUANTR)) {
                        arrAccum[qt][tet]++;
                        }
                      }
                     }
                   }
                 mask >>=1;
                 xr++;
                 }
                }/* ind */
              y++;
              yr--;
              }
        }/* k1 */


    }
        tHV=min((lpData->iMinLV)/2,heightAct/2);
        tHH=min((lpData->iMinLH)/2,widthAct/2);

if(VertHor) {
     for(i=0;i<maxR;i++) {
        fTh=-1;lTh=0;cGap=0;max1=0;
        for(tet=0;tet<RTETA1;tet+=QUANTTETA) {
               if(arrAccum[i][tet]>tHV) {
                     cGap=0;
                     lTh++;
                     if(fTh<0) fTh=tet;
                     if(max1<arrAccum[i][tet]) {
                        max1=arrAccum[i][tet];
                        idm1=tet;
                        }
                     }
               else {
                     cGap++;
                     if((cGap>1)&&(fTh>0)) {
                        if(lTh>1) {
                           for(j=fTh;j<tet;j++) {
                             if(j!=idm1) {
                                arrAccum[i][j]=0;
                                }
                             }
                           }
                       fTh=-1;
                       lTh=0;
                       }
                     }
               }
        if(lTh>1) {
               for(j=fTh;j<RTETA1;j++) {
                    if(j!=idm1) {
                        arrAccum[i][j]=0;
                        }
                    }

                }
        }
      }

if((VertHor==0)||(VertHor==2)) {
    for(i=0;i<maxR;i++) {
        fTh=-1;lTh=0;cGap=0;max1=0;
        for(tet=RTETA1;tet<RTETA2;tet+=QUANTTETA) {
               if(arrAccum[i][tet]>tHH) {
                     cGap=0;
                     lTh++;
                     if(fTh<0) fTh=tet;
                     if(max1<arrAccum[i][tet]) {
                        max1=arrAccum[i][tet];
                        idm1=tet;
                        }
                     }
               else {
                     cGap++;
                     if((cGap>1)&&(fTh>0)) {
                        if(lTh>1) {
                           for(j=fTh;j<tet;j++) {
                             if(j!=idm1) {
                                arrAccum[i][j]=0;
                                }
                             }
                           }
                       fTh=-1;
                       lTh=0;
                       }
                     }
               }
        if(lTh>1) {
               for(j=fTh;j<RTETA2;j++) {
                    if(j!=idm1) {
                        arrAccum[i][j]=0;
                        }
                    }

                }
        }
      }

if(VertHor) {
     for(i=0;i<maxR;i++) {
        fTh=-1;lTh=0;cGap=0;max1=0;
        for(tet=RTETA2;tet<MAXQUANTTETA;tet+=QUANTTETA) {
               if(arrAccum[i][tet]>tHV) {
                     cGap=0;
                     lTh++;
                     if(fTh<0) fTh=tet;
                     if(max1<arrAccum[i][tet]) {
                        max1=arrAccum[i][tet];
                        idm1=tet;
                        }
                     }
               else {
                     cGap++;
                     if((cGap>1)&&(fTh>0)) {
                        if(lTh>1) {
                           for(j=fTh;j<tet;j++) {
                             if(j!=idm1) {
                                arrAccum[i][j]=0;
                                }
                             }
                           }
                       fTh=-1;
                       lTh=0;
                       }
                     }
               }
        if(lTh>1) {
               for(j=fTh;j<MAXQUANTTETA;j++) {
                    if(j!=idm1) {
                        arrAccum[i][j]=0;
                        }
                    }

                }
        }
      }
        indxOutp=0;
for(i=0;i<maxR;i++) {
        for(tet=0;tet<MAXQUANTTETA;tet++) {
              test=0;
              if((arrTet[tet]<5)||(arrTet[tet]>160) ) {
                 if(arrAccum[i][tet]>tHV) test=1;
                 }
                else {
                 if(arrAccum[i][tet]>tHH) test=1;
                 }
              if(test) {
                arrOutp[indxOutp][0]=(i-srT)*quantR;
                arrOutp[indxOutp][1]=arrTet[tet];
                arrOutp[indxOutp][2]=arrAccum[i][tet];
                indxOutp++;
                if(indxOutp>=MAXARROUTP*MAXQUANTTETA) goto BR;
                }
              }
        }

BR:
          if((VertHor==0)||(VertHor==2)) {
             i=0;
             while(i<indxOutp-1) {
                   lastJ=i;
                   if(abs(90-arrOutp[i][1])<QUANTSONETETA) {
                      id2=0;

                      for(ii=0;ii<2;ii++) {
                        vR=arrOutp[i][0];
                        if(ii&&(!id2)) break;
                        j=i+1;
                        if(ii) {
                                if(i!=idm1) arrOutp[i][2]=0;
                                }
                        else {
                                max1=arrOutp[i][2];idm1=i;
                                }
                        while(j<indxOutp) {
                             if(abs(90-arrOutp[j][1])<QUANTSONETETA) {
                                 if((abs(vR-arrOutp[j][0])<2)&&abs(arrOutp[j][1]-arrOutp[i][1])<3)  {
                                     vR=arrOutp[j][0];
                                     if(ii) {
                                             if(j!=idm1) arrOutp[j][2]=0;
                                             }
                                     else {
                                             if(max1<arrOutp[j][2]) {
                                                max1=arrOutp[j][2];
                                                idm1=j;
                                                }
                                             id2=1;
                                             lastJ=j;
                                             }
                                     }
                                  else {
                                        break;
                                        }
                                 }

                          j++;
                          } /* while(j) */
                        } /* for(ii) */
                     }
                   i=lastJ+1;
                   } /* for(i) */

             for(i=0;i<indxOutp-1;i++) {
                   if(abs(90-arrOutp[i][1])<QUANTSONETETA) {
                        j=i+1;
                        max1=arrOutp[i][2];idm1=i;
                        while(j<indxOutp) {
                             if(abs(90-arrOutp[j][1])<QUANTSONETETA) {
                                   if(max1<arrOutp[j][2]) {
                                      max1=arrOutp[j][2];
                                      idm1=j;
                                      }
                                   }
                             j++;
                             } /* while(j) */
                         if(i!=idm1) {
                                outp0=arrOutp[i][0];
                                outp1=arrOutp[i][1];
                                outp2=arrOutp[i][2];
                                arrOutp[i][0]=arrOutp[idm1][0];
                                arrOutp[i][1]=arrOutp[idm1][1];
                                arrOutp[i][2]=arrOutp[idm1][2];
                                arrOutp[idm1][0]=outp0;
                                arrOutp[idm1][1]=outp1;
                                arrOutp[idm1][2]=outp2;
                                }
                     }
                   } /* for(i) */
                 }
          if(VertHor) {
             i=0;
             while(i<indxOutp-1) {
                   lastJ=i;
                   if(abs(arrOutp[i][1])<QUANTSONETETA) {
                      id2=0;

                      for(ii=0;ii<2;ii++) {
                        vR=arrOutp[i][0];
                        if(ii&&(!id2)) break;
                        j=i+1;
                        if(ii) {
                                if(i!=idm1) arrOutp[i][2]=0;
                                }
                        else {
                                max1=arrOutp[i][2];idm1=i;
                                }
                        while(j<indxOutp) {
                             if(abs(arrOutp[j][1])<QUANTSONETETA) {
                                 if((abs(vR-arrOutp[j][0])<2)&&abs(arrOutp[j][1]-arrOutp[i][1])<3)  {
                                     vR=arrOutp[j][0];
                                     if(ii) {
                                             if(j!=idm1) arrOutp[j][2]=0;
                                             }
                                     else {
                                             if(max1<arrOutp[j][2]) {
                                                max1=arrOutp[j][2];
                                                idm1=j;
                                                }
                                             id2=1;
                                             lastJ=j;
                                             }
                                     }
                                  else {
                                        break;
                                        }
                                 }

                          j++;
                          } /* while(j) */
                        } /* for(ii) */
                     }
                   i=lastJ+1;
                   } /* for(i) */
             i=0;
             while(i<indxOutp-1) {
                   lastJ=i;
                   if(abs(180-arrOutp[i][1])<QUANTSONETETA) {
                      id2=0;

                      for(ii=0;ii<2;ii++) {
                        vR=arrOutp[i][0];
                        if(ii&&(!id2)) break;
                        j=i+1;
                        if(ii) {
                                if(i!=idm1) arrOutp[i][2]=0;
                                }
                        else {
                                max1=arrOutp[i][2];idm1=i;
                                }
                        while(j<indxOutp) {
                             if(abs(180-arrOutp[j][1])<QUANTSONETETA) {
                                 if((abs(vR-arrOutp[j][0])<2)&&abs(arrOutp[j][1]-arrOutp[i][1])<3)  {
                                     vR=arrOutp[j][0];
                                     if(ii) {
                                             if(j!=idm1) arrOutp[j][2]=0;
                                             }
                                     else {
                                             if(max1<arrOutp[j][2]) {
                                                max1=arrOutp[j][2];
                                                idm1=j;
                                                }
                                             id2=1;
                                             lastJ=j;
                                             }
                                     }
                                  else {
                                        break;
                                        }
                                 }
                          j++;
                          } /* while(j) */
                        } /* for(ii) */
                     }
                   i=lastJ+1;
                   } /* for(i) */

             for(i=0;i<indxOutp-1;i++) {
                   if((abs(180-arrOutp[i][1])<QUANTSONETETA)||(abs(arrOutp[i][1])<QUANTSONETETA)) {
                        j=i+1;
                        max1=arrOutp[i][2];idm1=i;
                        while(j<indxOutp) {
                             if((abs(180-arrOutp[j][1])<QUANTSONETETA)||(abs(arrOutp[j][1])<QUANTSONETETA)) {
                                   if(max1<arrOutp[j][2]) {
                                      max1=arrOutp[j][2];
                                      idm1=j;
                                      }
                                   }
                             j++;
                             } /* while(j) */
                         if(i!=idm1) {
                                outp0=arrOutp[i][0];
                                outp1=arrOutp[i][1];
                                outp2=arrOutp[i][2];
                                arrOutp[i][0]=arrOutp[idm1][0];
                                arrOutp[i][1]=arrOutp[idm1][1];
                                arrOutp[i][2]=arrOutp[idm1][2];
                                arrOutp[idm1][0]=outp0;
                                arrOutp[idm1][1]=outp1;
                                arrOutp[idm1][2]=outp2;
                                }

                     }
                   } /* for(i) */
                 }

        return(0);
} /* of compAccum() */

/***************************************************************************
 *  PURPOSE      :horizontal line tracking                                 *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int hLineTrack(
   SREMOVAL *lpData,     /* structure for removal */
   int          xAct,        /* X offset in bitmap (of rectangle area) in pixels */
   int          yAct,        /* Y offset in bitmap (of rectangle area) in pixels */
   int          widthAct,    /* width of rectangle area */
   int          heightAct,   /* height of rectangle area */
   int          xSt,         /* start point (x)  */
   int          ySt,         /* start point (y)  */
   int          direct       /* direction        */
)
{
   int x, y, yy, yyy, xB, yB, Eps, Eps1, distUp, distBott, Ok, yBB, indxCh, chX;
   int countB, wImg;
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;
    wImg  =(((lpData->iLineWidth)+31)/32)*4;

  indxCh=0;
  indxTr=0;
  Eps=3;
  Eps1=2;
  chX=6;
  xB=xSt;yB=ySt;
  yBB=ySt;
  while((xB>=xAct)&&(xB<xAct+widthAct)) {
     Ok=0;x=xB;yy=yB;
     while(yy>(lpData->iYBegin)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        yy--;
        if((yB-yy)>Eps+1) break;
        }
     if(Ok) {
       distUp=yB-yy;
       }
     else {
        distUp=101;
        }
     y=yB;Ok=0;
     while(y<(lpData->iYBegin)+(lpData->iHeight)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        y++;
        if((y-yB)>Eps+1) break;
        }
     if(Ok) {
       distBott=y-yB;
       }
     else {
       distBott=101;
       }
     if(distUp<distBott) {
        y=yy;
        Ok=distUp;
        }
     else {
        Ok=distBott;
        }
     if(Ok>Eps) {
        arrTrace[indxTr][0]=-1;
        if(indxTr>MAXARR) return(-6);
        indxTr++;
        xB+=direct;
        indxCh=0;
        continue;
        }
     yB=y;
/* width of the line */
     yy=yB;Ok=1;countB=0;
     while(yy>(lpData->iYBegin)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxVGapH)) break;
                yy--;
                continue;
                }
        yy--;
        countB=0;
        if((yB-yy)>(lpData->iMaxWidthH)) {
                Ok=0;
                break;
                }
        }
     yy=min(yy+countB,yB);
     if(Ok) {
         arrTrace[indxTr][0]=yy;
         }
     else {
         arrTrace[indxTr][0]=-2;
         if(indxTr>MAXARR) return(-6);
         indxTr++;
         }

     yy=yB;countB=0;
     if(Ok) {
       while(yy<(lpData->iYBegin)+(lpData->iHeight)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxVGapH)) break;
                yy++;
                continue;
                }
        yy++;
        countB=0;
        if((yy-yB)>(lpData->iMaxWidthH)) {
                Ok=0;
                break;
                }
        }
       yy=max(yy-countB,yB);
       if(Ok) {
         arrTrace[indxTr][1]=yy;
         if(indxTr>MAXARR) return(-6);
         if(arrTrace[indxTr][1]-arrTrace[indxTr][0]>(lpData->iMaxWidthH)) {
            arrTrace[indxTr][0]=-2;
            Ok=0;
            }
         indxTr++;
         }
       else {
         arrTrace[indxTr][0]=-2;
         if(indxTr>MAXARR) return(-6);
         indxTr++;
         }
       }
     xB+=direct;
     if(Ok) {
        indxCh++;
        if((yB>arrTrace[indxTr-1][1])||(yB<arrTrace[indxTr-1][0])) {
            if(abs(yB-arrTrace[indxTr-1][1])<abs(yB-arrTrace[indxTr-1][0])) {
                yB=arrTrace[indxTr-1][1];
                }
            else {
                yB=arrTrace[indxTr-1][0];
                }
            }
        }
    if(abs(yB-yBB)>Eps1) yB=yBB;
    if(Ok) {
      if((indxCh%chX)==0)  yBB=yB;
      }
     }
     return(0);

}/* hLineTRack() */

/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :horizontal line tracking which is stopped on the first gap *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int hLineTrackGap(
   SREMOVAL *lpData,     /* structure for removal */
   int          xSt,         /* start point (x)  */
   int          ySt,         /* start point (y)  */
   int          direct       /* direction        */
)
{
   int x, y, yy, yyy, xB, yB, Eps, Eps1, distUp, distBott, Ok, cGap, indxW;
   int yBB, indxCh, chX, countB, wImg;
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;


    wImg  =(((lpData->iLineWidth)+31)/32)*4;
  Eps=3;
  Eps1=2;
  chX=6;
  xB=xSt;yB=ySt;
  cGap=0;
  indxW=0;
  indxCh=0;
  yBB=ySt;

  while((xB>=(lpData->iXBegin))&&(xB<(lpData->iXBegin)+(lpData->iWidth))) {
     Ok=0;x=xB;yy=yB;
     while(yy>(lpData->iYBegin)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        yy--;
        if((yB-yy)>Eps+1) break;
        }
     if(Ok) {
       distUp=yB-yy;
       }
     else {
        distUp=101;
        }
     y=yB;Ok=0;
     while(y<(lpData->iYBegin)+(lpData->iHeight)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {

               Ok=1;
               break;
               }
        y++;
        if((y-yB)>Eps+1) break;
        }
     if(Ok) {
       distBott=y-yB;
       }
     else {
       distBott=101;
       }
     if(distUp<distBott) {
        y=yy;
        Ok=distUp;
        }
     else {
        Ok=distBott;
        }
     if(Ok>Eps) {
        arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]=-1;
        cGap++;
        if(cGap>(lpData->iMaxHGapH)) return(0);
        xB+=direct;
        indxW++;
        indxCh=0;
        continue;
        }
     yB=y;

/* width of the line */
     yy=yB;Ok=1;countB=0;
     while(yy>(lpData->iYBegin)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxVGapH)) break;
                yy--;
                continue;
                }
        yy--;
        countB=0;
        if((yB-yy)>(lpData->iMaxWidthH)) {
                Ok=0;
                break;
                }
        }
     yy=min(yy+countB,yB);
     if(Ok) {
         arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]=yy;
         }
     else {
         arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]=-2;
         }

     yy=yB;countB=0;
     if(Ok) {
       while(yy<(lpData->iYBegin)+(lpData->iHeight)) {
                           yyy=yy;
                           yyy=(lpData->iImgHeight)-1-yy;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {

                countB++;
                if(countB>(lpData->iMaxVGapH)) break;
                yy++;
                continue;
                }
        yy++;
        countB=0;
        if((yy-yB)>(lpData->iMaxWidthH)) {
                Ok=0;
                break;
                }
        }
       yy=max(yy-countB,yB);
       if(Ok) {
         arrLine[xSt+indxW*direct-(lpData->iXBegin)][1]=yy;
         if(arrLine[xSt+indxW*direct-(lpData->iXBegin)][1]-arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]>(lpData->iMaxWidthH)) {
            arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]=-2;
            Ok=0;
            }
         }
       else {
         arrLine[xSt+indxW*direct-(lpData->iXBegin)][0]=-2;
         }
       }

     xB+=direct;
     if(Ok) {
        indxCh++;
        if((yB>arrLine[xSt+indxW*direct-(lpData->iXBegin)][1])||(yB<arrLine[xSt+indxW*direct-(lpData->iXBegin)][0])) {
            if(abs(yB-arrLine[xSt+indxW*direct-(lpData->iXBegin)][1])<abs(yB-arrLine[xSt+indxW*direct-(lpData->iXBegin)][0])) {
                yB=arrLine[xSt+indxW*direct-(lpData->iXBegin)][1];
                }
            else {
                yB=arrLine[xSt+indxW*direct-(lpData->iXBegin)][0];
                }
            }
        }
    if(abs(yB-yBB)>Eps1) yB=yBB;
    if(Ok) {
       if((indxCh%chX)==0)  yBB=yB;
       }
     indxW++;
     }
     return(0);

}/* hLineTRackGap() */

/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :clean one horizontal line                                *
 *                                                                         *
 ***************************************************************************/

int clLineH(
   SREMOVAL *lpData,     /* structure for removal */
   int          maxL         /* maximal line lenght  */
 )
 {
  int x, y, wImg, mask, i, jj, jB, gap, lL, ret, yy;
  int dif, pU, pB, dU, dB, cut, cB, cE, difW, cc;
  LONG lY, difL;
  float ratio;
  BYTE  *hptr;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
                gap=0;lL=0;ret=0;
                jB=0;
                cB=-1;
                cE=-1;
                for(i=0;i<maxL;i++) {
                     if(arrLine[i][0]==-1) {
                        gap++;
                        if((gap>(lpData->iMaxHGapH))&&(lL!=0)) {
                                if(lL<(lpData->iMinLH)) {
                                        for(jj=jB;jj<i;jj++) {
                                                arrLine[jj][0]=-1;
                                                }
                                        }
                                lL=0;
                                }
                        }
                      else {
                        if(lL==0) jB=i;
                        lL++;
                        if(lL>=(lpData->iMinLH)) {
                                if(ret==0) {
                                   ret=1;
                                   cB=jB;
                                   cE=lL;
                                   }
                                else {
                                   cE=lL;
                                   }
                                }
                        gap=0;
                        }
                     } /* for i */

                if(ret==0) return(ret);
                cE+=cB;
                cE--;
                if((lL!=0)&&(lL<(lpData->iMinLH))) {
                   for(jj=jB;jj<maxL;jj++) {
                       arrLine[jj][0]=-1;
                       }
                   }
                cc=0;
                if((cB>=0)&&(cE>0)) {
                        difL=0L;
                        for(i=cB;i<cE;i++) {
                                if(arrLine[i][0]>0) {
                                  difL+=(long)(arrLine[i][1]-arrLine[i][0]);
                                  cc++;
                                  }
                                }
                        }
                if(cc>0) {
                        dif=(int)((float)difL/(float)cc+0.5);
                        }
                else {
                        dif=1000;
                     }

                pU=arrLine[cB][0];
                pB=arrLine[cB][1];
                for(i=0;i<maxL;i++) {
                     if(arrLine[i][0]>0) {
                        x=i+(lpData->iXBegin);
                        dU=dB=0;
                        difW=arrLine[i][1]-arrLine[i][0]+1;
                        if(difW>(dif+(lpData->iMaxDifShapeH))) {
                            cut=difW-(dif+(lpData->iMaxDifShapeH));
                            if(arrLine[i][1]-pB>0) {
                                if(pU-arrLine[i][0]>=0) {
                                  ratio=(float)(pU-arrLine[i][0])/(float)((arrLine[i][1]-pB)+(pU-arrLine[i][0]));
                                  dU=(int)((float)(cut)*ratio);
                                  dB=cut-dU;
                                  }
                                else {
                                        dU=0;
                                        dB=cut;
                                        }
                                }
                            else {
                                dU=cut;
                                dB=0;
                                }
                            }
                        else {
                            pU=arrLine[i][0];
                            pB=arrLine[i][1];
                            }

                        for(y=arrLine[i][0]+dU;y<=arrLine[i][1]-dB;y++){
                            yy=y;
                            yy=(lpData->iImgHeight)-1-y;
                            lY=(LONG)yy*(LONG)wImg;
                            hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                            mask=LEFT_BIT;
                            mask >>=(x%8);
                            hptr[0]=hptr[0]|mask;
                            }
                        }
                     }
                return(ret);

 } /* of clLineH() */

/***************************************************************************
 *									   *
 *                                                                         *
 *  PURPOSE      :clean one vertical line                                  *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
int clLineV(
   SREMOVAL *lpData,     /* structure for removal */
   int          maxL         /* maximal line lenght  */
 )
 {
  int x, y, wImg, mask, i, jj, jB, gap, lL, ret, yy;
  int dif, pL, pR, dL, dR, cut, cB, cE, difW, cc;
  LONG lY, difL;
  float ratio;
  BYTE  *hptr;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
                gap=0;lL=0;ret=0;
                jB=0;
                cB=-1;
                cE=-1;
                for(i=0;i<maxL;i++) {
                     if(arrLine[i][0]==-1) {
                        gap++;
                        if((gap>(lpData->iMaxVGapV))&&(lL!=0)) {
                                if(lL<(lpData->iMinLV)) {
                                        for(jj=jB;jj<i;jj++) {
                                                arrLine[jj][0]=-1;
                                                }
                                        }
                                lL=0;
                                }
                        }
                      else {
                        if(lL==0) jB=i;
                        lL++;
                        if(lL>=(lpData->iMinLV)) {
                                if(ret==0) {
                                   ret=1;
                                   cB=jB;
                                   cE=lL;
                                   }
                                else {
                                   cE=lL;
                                   }
                                }
                        gap=0;
                        }
                     } /* for i */

                if(ret==0) return(ret);
                cE+=cB;
                cE--;
                if((lL!=0)&&(lL<(lpData->iMinLV))) {
                   for(jj=jB;jj<maxL;jj++) {
                       arrLine[jj][0]=-1;
                       }
                   }
                cc=0;
                if((cB>=0)&&(cE>0)) {
                        difL=0L;
                        for(i=cB;i<cE;i++) {
                                if(arrLine[i][0]>0) {
                                  difL+=(long)(arrLine[i][1]-arrLine[i][0]);
                                  cc++;
                                  }
                                }
                        }
                if(cc>0) {
                        dif=(int)((float)difL/(float)cc+0.5);
                        }
                else {
                        dif=1000;
                     }
                pL=arrLine[cB][0];
                pR=arrLine[cB][1];
                for(i=0;i<maxL;i++) {
                     if(arrLine[i][0]>0) {
                        y=i+(lpData->iYBegin);
                        dL=dR=0;
                        difW=arrLine[i][1]-arrLine[i][0]+1;
                        if(difW>(dif+(lpData->iMaxDifShapeV))) {
                            cut=difW-(dif+(lpData->iMaxDifShapeV));
                            if(arrLine[i][1]-pR>0) {
                                if(pL-arrLine[i][0]>=0) {
                                ratio=(float)(pL-arrLine[i][0])/(float)((arrLine[i][1]-pR)+(pL-arrLine[i][0]));
                                dL=(int)((float)(cut)*ratio);
                                dR=cut-dL;

                                }
                                else {
                                        dL=0;
                                        dR=cut;
                                        }
                                }
                            else {
                                dL=cut;
                                dR=0;
                                }
                            }
                        else {
                            pL=arrLine[i][0];
                            pR=arrLine[i][1];
                            }
                        for(x=arrLine[i][0]+dL;x<=arrLine[i][1]-dR;x++){
                            yy=y;
                            yy=(lpData->iImgHeight)-1-y;
                            lY=(LONG)yy*(LONG)wImg;
                            hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                            mask=LEFT_BIT;
                            mask >>=(x%8);
                            hptr[0]=hptr[0]|mask;
                            }
                        }
                     }
                return(ret);

 } /* of clLineV() */


/***************************************************************************
 *									   *

 *  PURPOSE      :vertical line tracking                                   *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
int vLineTrack(
   SREMOVAL *lpData,     /* structure for removal */
   int          xAct,        /* X offset in bitmap (of rectangle area) in pixels */
   int          yAct,        /* Y offset in bitmap (of rectangle area) in pixels */
   int          widthAct,    /* width of rectangle area */
   int          heightAct,   /* height of rectangle area */
   int          xSt,         /* start point (x)  */
   int          ySt,         /* start point (y)  */
   int          direct       /* direction        */
)
{
   int x, y, yyy, xx, xB, yB, Eps, Eps1, distLt, distRt, Ok, xBB, indxCh, chX;
   int countB, wImg, xbP, flgS, ttH;
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;

  indxCh=0;
  indxTr=0;
  ttH=40;
  flgS=0;
  xbP=0;
  Eps=3;
  Eps1=2;
  chX=6;
  xB=xSt;yB=ySt;
  xBB=xSt;
  while((yB>=yAct)&&(yB<yAct+heightAct)) {
     Ok=0;xx=xB;y=yB;
     while(xx>(lpData->iXBegin)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        xx--;
        if((xB-xx)>Eps+1) break;
        }
     if(Ok) {
       distLt=xB-xx;
       }
     else {
        distLt=101;
        }
     x=xB;Ok=0;
     while(x<(lpData->iXBegin)+(lpData->iWidth)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {

               Ok=1;
               break;
               }
        x++;
        if((x-xB)>Eps+1) break;
        }
     if(Ok) {
       distRt=x-xB;
       }
     else {
       distRt=101;
       }
     if(distLt<distRt) {
        x=xx;
        Ok=distLt;
        }
     else {
        Ok=distRt;
        }
     if(Ok>Eps) {
        arrTrace[indxTr][0]=-1;
        if(indxTr>MAXARR) return(-6);
        indxTr++;
        yB+=direct;
        indxCh=0;
        flgS=0;
        continue;
        }
     xB=x;

     if(xB==xbP) {
        flgS++;
        }
      else {
        flgS=0;
        }
     xbP=xB;

/* width of the line */
     xx=xB;Ok=1;countB=0;
     while(xx>(lpData->iXBegin)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxHGapV)) break;
                xx--;
                continue;
                }
        xx--;
        countB=0;
        if((xB-xx)>(lpData->iMaxWidthV)) {
                Ok=0;
                break;
                }
        }
     xx=min(xx+countB,xB);
     if(Ok) {
         arrTrace[indxTr][0]=xx;
         }
     else {
         arrTrace[indxTr][0]=-2;
         if(indxTr>MAXARR) return(-6);
         indxTr++;
         }

     xx=xB;countB=0;
     if(Ok) {
       while(xx<(lpData->iXBegin)+(lpData->iWidth)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxHGapV)) break;
                xx++;
                continue;
                }
        xx++;
        countB=0;
        if((xx-xB)>(lpData->iMaxWidthV)) {
                Ok=0;
                break;
                }
        }
       xx=max(xx-countB,xB);
       if(Ok) {
         arrTrace[indxTr][1]=xx;
         if(indxTr>MAXARR) return(-6);
         if(arrTrace[indxTr][1]-arrTrace[indxTr][0]>(lpData->iMaxWidthV)) {
            arrTrace[indxTr][0]=-2;
            Ok=0;
            }
         indxTr++;
         }
       else {
         arrTrace[indxTr][0]=-2;
         if(indxTr>MAXARR) return(-6);
         indxTr++;
         }
       }
     yB+=direct;
     if(Ok) {
        indxCh++;
        if((xB>arrTrace[indxTr-1][1])||(xB<arrTrace[indxTr-1][0])) {
            if(abs(xB-arrTrace[indxTr-1][1])<abs(xB-arrTrace[indxTr-1][0])) {
                xB=arrTrace[indxTr-1][1];
                }
            else {
                xB=arrTrace[indxTr-1][0];
                }
            }
        }
     else {flgS=0;}

    if((abs(xB-xBB)>Eps1)&&(flgS<ttH)) xB=xBB;
      if(Ok) {
         if((indxCh%chX)==0) {
                xBB=xB;
                if(flgS>=ttH) flgS=0;
                }
         }
     }
     return(0);
}/* vLineTRack() */

/***************************************************************************
 *									   *
 *  PURPOSE      :vertical line tracking which is stoped on the first gap  *
 *                                                                         *
 ***************************************************************************/
int  vLineTrackGap(
   SREMOVAL *lpData,     /* structure for removal */
   int          xSt,         /* start point (x)  */
   int          ySt,         /* start point (y)  */
   int          direct       /* direction        */
)
{
   int x, y, yyy, xx, xB, yB, Eps, Eps1, distLt, distRt, Ok, cGap, indxW;
   int xBB, indxCh, chX, countB, wImg;
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;

    wImg  =(((lpData->iLineWidth)+31)/32)*4;
  Eps=3;
  Eps1=2;
  xB=xSt;yB=ySt;
  cGap=0;
  indxW=0;
  indxCh=0;
  xBB=xSt;
  chX=6;

  while((yB>=(lpData->iYBegin))&&(yB<(lpData->iYBegin)+(lpData->iHeight))) {
     Ok=0;xx=xB;y=yB;
     while(xx>(lpData->iXBegin)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        xx--;
        if((xB-xx)>Eps+1) break;
        }
     if(Ok) {
       distLt=xB-xx;
       }
     else {
        distLt=101;
        }
     x=xB;Ok=0;
     while(x<(lpData->iXBegin)+(lpData->iWidth)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(x/8);
                           mask=LEFT_BIT;
                           mask >>=(x%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
               Ok=1;
               break;
               }
        x++;
        if((x-xB)>Eps+1) break;
        }
     if(Ok) {
       distRt=x-xB;
       }
     else {
       distRt=101;
       }
     if(distLt<distRt) {
        x=xx;
        Ok=distLt;
        }
     else {
        Ok=distRt;
        }
     if(Ok>Eps) {
        arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]=-1;
        cGap++;
        if(cGap>(lpData->iMaxVGapV)) return(0);
        yB+=direct;
        indxW++;
        indxCh=0;
        continue;
        }
     xB=x;

/* width of the line */
     xx=xB;Ok=1;countB=0;
     while(xx>(lpData->iXBegin)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {

                countB++;
                if(countB>(lpData->iMaxHGapV)) break;
                xx--;
                continue;
                }
        xx--;
        countB=0;
        if((xB-xx)>(lpData->iMaxWidthV)) {
                Ok=0;
                break;
                }
        }
     xx=min(xx+countB,xB);
     if(Ok) {
         arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]=xx;
         }
     else {
         arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]=-2;
         }

     xx=xB;countB=0;
     if(Ok) {
       while(xx<(lpData->iXBegin)+(lpData->iWidth)) {
                           yyy=y;
                           yyy=(lpData->iImgHeight)-1-y;
                           lY=(LONG)yyy*(LONG)wImg;
                           hptr=(lpData->lpPointer)+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch==0) {
                countB++;
                if(countB>(lpData->iMaxHGapV)) break;
                xx++;
                continue;
                }
        xx++;
        if((xx-xB)>(lpData->iMaxWidthV)) {
                Ok=0;
                break;
                }
        }
       xx=max(xx-countB,xB);
       if(Ok) {
         arrLine[ySt+indxW*direct-(lpData->iYBegin)][1]=xx;
         if(arrLine[ySt+indxW*direct-(lpData->iYBegin)][1]-arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]>(lpData->iMaxWidthV)) {
            arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]=-2;
            Ok=0;
            }
         }
       else {
         arrLine[ySt+indxW*direct-(lpData->iYBegin)][0]=-2;
         }
       }

     yB+=direct;
     if(Ok) {
        indxCh++;
        if((xB>arrLine[ySt+indxW*direct-(lpData->iYBegin)][1])||(xB<arrLine[ySt+indxW*direct-(lpData->iYBegin)][0])) {
            if(abs(xB-arrLine[ySt+indxW*direct-(lpData->iYBegin)][1])<abs(xB-arrLine[ySt+indxW*direct-(lpData->iYBegin)][0])) {
                xB=arrLine[ySt+indxW*direct-(lpData->iYBegin)][1];
                }
            else {
                xB=arrLine[ySt+indxW*direct-(lpData->iYBegin)][0];
                }
            }
        }
    if(abs(xB-xBB)>Eps1) xB=xBB;
    if(Ok) {
      if((indxCh%chX)==0)  xBB=xB;
      }

     indxW++;
     }
     return(0);
}/* vLineTRackGap() */
/***************************************************************************
 *  PURPOSE      : vertical histogram,                                     *
 *                                                                         *
 ***************************************************************************/
void histVer(STRUCTWB *lpWorkBuffer, int *hist, int x1, int x2, int y1, int y2){
  int  j, kk, curInd, yy;
  int  yD, ch;
  int lByte, rByte, rL, rR, w_bt;
  LONG nY;
  BYTE *hptr;
  BYTE mask;


    w_bt=(lpWorkBuffer -> widthImg);

    memset(hist,0x00,(unsigned int)(2*MAXSIZE));
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

   }/* of histVer()*/


/***************************************************************************
 *  PURPOSE      :calculate horizontal histogram for given alpha            *
 *                                                                         *
 ***************************************************************************/
int histH_alpha(STRUCTWB *lpWorkBuffer, int x1, int y1, int x2, int y2, LONG alpha, float tG, int *histH, int *lenH)
{
  int   sww, i ,j, dx, x1D8, dy, sum, ind, w_bt;
  int   kDirect, sumP, swtP2, swtP, yC, xx, x28;
  LONG  nY;
  BYTE   *hptr;
  int  *pp;

              if((tG<EPSTG)&&(tG>(-EPSTG))) {
                swtP2=32000;
                swtP=0;
                }
              else {
                swtP2=(int)((float)1/tG);
                if(swtP2<0) {swtP2=-swtP2;}
                swtP=swtP2/2;
                }
              kDirect=1;
              if(alpha>0) {kDirect=-1;}
              w_bt=(lpWorkBuffer -> widthImg);
              x1D8=x1/8;
              dy=y2-y1+1;
              dx=x2/8-x1D8-1;
              *lenH=dy;
              pp=histH;
              xx=dx+x1D8+1;
              x28=8-x2%8;
              i=0;
              while(i<dy)  {
                sum=0;
                sww=y1+i;
                yC=sww;

                sww=(lpWorkBuffer -> hightImg)-1-sww;
                nY=(LONG)sww*(LONG)w_bt;
                hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)x1D8;

                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                sumP=x1%8;
                sum+=FirstZer(ind, sumP);
                sumP=8-sumP;
                sumP+=swtP;
                hptr++;
                for(j=0;j<dx;j++)  {
                        if(sumP>swtP2) {
                           sumP=0;
                           yC+=kDirect;
                           if((yC<y1)||(yC>y2)) {break;}
                           hptr=hptr-(LONG)(kDirect*w_bt);
                           }
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        sum+=gr_ar[ind];
                        hptr++;
                        sumP+=8;
                        }
                if( xx<(lpWorkBuffer -> widthImg)) {
                  ind=(int)(*hptr);
                  if(ind<0) {ind+=256;}
                  ind=255-ind;
                  sum+=LastZer(ind, x28);
                  }
                *pp=sum;
                i++;
                pp++;
                }
        return(0);

 }/* end of histH_alpha() */

/***************************************************************************
 *  PURPOSE      :calculate horizontal histogram for few angles            *
 *                                                                         *
 ***************************************************************************/
int histHSkewed(STRUCTWB *lpWorkBuffer, float maxAlpha, float stepAlpha, int *histH, int *lenH, LONG *angleH, int  *scAngleH){
  int   i, nH, lenHH, minH, wH, kD;
  float alP;

   angleHist[0]=0L;
   nH=1;
   kD=1;
   for(i=1;i<=MAXHIST/2;i++) {
        if(nH<MAXHIST) {
                alP=(float)i*stepAlpha;
                if(alP<=maxAlpha) {
// printf("------------ linesForFonts, alP= %e \n", alP);
                    angleHist[nH]=(LONG)(alP*(float)SCALEAN);
                    tGAngle[nH] = (float)tan((double)(alP*PI)/180.0);
                    nH++;
                    if(nH<MAXHIST) {
                        angleHist[nH]= -angleHist[nH-1];
                        tGAngle[nH]  = -tGAngle[nH-1];
                        nH++;
                        }
                    }
                else {break;}
                }
        }

//  printf("------------ linesForFonts, maxAlpha= %e nH= %d \n", maxAlpha, nH);

   minH=32000;
   for(i=0;i<nH;i++) {
         lpHistHSk[i]=(int *)&histHSk[i][1];
         histH_alpha(lpWorkBuffer,lpWorkBuffer->begRect.x,lpWorkBuffer->begRect.y,
                     (lpWorkBuffer->begRect.x)+(lpWorkBuffer->Width)-1,(lpWorkBuffer->begRect.y)+(lpWorkBuffer->Height)-1,
                     angleHist[i],tGAngle[i],lpHistHSk[i],(int *)&lenHH);
         if(minH>lenHH) {minH=lenHH;}
         }
   wH=whichHist((WORD)(lpWorkBuffer->Width),(int  * *)lpHistHSk,(int)nH,(int)minH,
             (float *)&tGAngle[0]);

   for(i=1;i<minH;i++) {
        histH[i]=histHSk[wH][i];
        }
    if(minH >= (lpWorkBuffer->Height)) {minH = (lpWorkBuffer->Height)-1;}
   *lenH     =minH;
   *angleH   =angleHist[wH];
   *scAngleH =(int)SCALEAN;

//   printf("------------ wH= %d, angleHist[wH]= %ld, minH= %d \n", wH, angleHist[wH], minH);

   return(0);

}  /* histHSkewed() */
int histHSkewedFrame(STRUCTWB *lpWorkBuffer, float maxAlpha, float stepAlpha, int *histH, int *lenH, LONG *angleH, int  *scAngleH,
		int indFr){
  int   i, nH, lenHH, minH, wH, kD;
  int x1, x2, y1, y2;

  float alP;

   angleHist[0]=0L;
   nH=1;
   kD=1;
   for(i=1;i<=MAXHIST/2;i++) {
        if(nH<MAXHIST) {
                alP=(float)i*stepAlpha;
                if(alP<=maxAlpha) {
// printf("------------ linesForFonts, alP= %e \n", alP);
                    angleHist[nH]=(LONG)(alP*(float)SCALEAN);
                    tGAngle[nH] = (float)tan((double)(alP*PI)/180.0);
                    nH++;
                    if(nH<MAXHIST) {
                        angleHist[nH]= -angleHist[nH-1];
                        tGAngle[nH]  = -tGAngle[nH-1];
                        nH++;
                        }
                    }
                else {break;}
                }
        }

//  printf("------------ linesForFonts, maxAlpha= %e nH= %d \n", maxAlpha, nH);

   minH=32000;
   x1 = (lpWorkBuffer->infFrame)[indFr].x1;
   x2 = (lpWorkBuffer->infFrame)[indFr].x2;
   y1 = lpWorkBuffer->begRect.y;
   y2 = (lpWorkBuffer->begRect.y)+(lpWorkBuffer->Height)-1;

   for(i=0;i<nH;i++) {
         lpHistHSk[i]=(int *)&histHSk[i][1];
//         histH_alpha(lpWorkBuffer,lpWorkBuffer->begRect.x,lpWorkBuffer->begRect.y,
//                     (lpWorkBuffer->begRect.x)+(lpWorkBuffer->Width)-1,(lpWorkBuffer->begRect.y)+(lpWorkBuffer->Height)-1,
//                     angleHist[i],tGAngle[i],lpHistHSk[i],(int *)&lenHH);
         histH_alpha(lpWorkBuffer,x1, y1,
                     x2, y2,
                     angleHist[i],tGAngle[i],lpHistHSk[i],(int *)&lenHH);

         if(minH>lenHH) {minH=lenHH;}
         }
//   wH=whichHist((WORD)(lpWorkBuffer->Width),(int  * *)lpHistHSk,(int)nH,(int)minH,
//             (float *)&tGAngle[0]);
   wH=whichHist((WORD)(x2-x1+1),(int  * *)lpHistHSk,(int)nH,(int)minH,
             (float *)&tGAngle[0]);

   for(i=1;i<minH;i++) {
        histH[i]=histHSk[wH][i];
        }
    if(minH >= (lpWorkBuffer->Height)) {minH = (lpWorkBuffer->Height)-1;}
   *lenH     =minH;
   *angleH   =angleHist[wH];
   *scAngleH =(int)SCALEAN;

//   printf("------------ wH= %d, angleHist[wH]= %ld, minH= %d \n", wH, angleHist[wH], minH);

   return(0);

}  /* histHSkewed() */

/***************************************************************************
 *  PURPOSE      :horizontal lines recognition for CellBoxes               *
 *                                                                         *
 ***************************************************************************/

int linesHCellB(
  STRUCTWB *lpWorkBuffer,
  int      *hist,           /* pointer to histogram */
  int      lenH,            /* lenght of histogram  */
  int      lenLine,         /* minimal line lenght  */
  int      maxLenLine,      /* maximal line lenght  */
  LONG     alpha,
  int      scaleAngle
  )
{
  int   i, j, k, indT, indB, y1, ii;
  int   indc, tHDpi, begS, endS;
  int   lT, lB, max, maxP, aVG, aVi, aVp,  aVLoc = 0;
  int   lenH1, count ,min;
  LONG  aV;

     y1=(lpWorkBuffer->begRect.y);

     tHDpi=(int)((float)TH_WDT*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));

tHDpi += 6;

     if(lenH<MAXPROFILE-1) {hist[lenH]=0;}

     indT=1;
     indB=lenH;
     aVG =max(lenLine/3,10);
     lenH1=lenH-1;

     begS=max(2,indT);
     endS=min(lenH1,indB);
     if(endS>=MAXPROFILE) {endS=MAXPROFILE-1;}

     nOfLines=0;
     for(ii=begS;ii<=endS;ii++) {
        if((hist[ii+1]<=hist[ii])&&(hist[ii-1]<hist[ii])&&(hist[ii]!=0)) {
            max=hist[ii];

            if(max<aVG) {continue;}

            maxP=max-max/10;
            k=ii;
            lT=k;
            for(j=1;j<tHDpi;j++) {
             if(k-j>0) {
               if(maxP>hist[k-j]) {break;}
               lT=k-j;
               }
             }
            lB=k;
            for(j=1;j<tHDpi;j++) {
             if(k+j<lenH) {
               if(maxP>hist[k+j]) {break;}
               lB=k+j;
               }
             }

            if(lB-lT>=tHDpi) {goto NXT;}

            aV=0L;k=0;
            for(j=lT;j<=lB;j++) {
               k++;
               aV+=hist[j];
               }
            if(k>0) {aVLoc=(int)(aV/(LONG)k);}

            k=1;
            aVi=aVLoc;
            aV=(LONG)aVLoc;
            indc=0;
            for(i=lT-1;i>0;i--) {
               aVp=aVi/3;
               if(aVi-hist[i]>aVp+2) {indc=i+1;break;}
               k++;
               aV+=(LONG)hist[i];
               aVi=(int)(aV/(LONG)k);
               if(k>tHDpi) {indc=-1;break;}
               }
            if(indc<0) {goto NXT;}
            lT=indc;
            if(lT==0) {lT=1;}

            k=1;
            aVi=aVLoc;
            aV=(LONG)aVLoc;
            indc=0;
            for(i=lB+1;i<lenH;i++) {
               aVp=aVi/3;
               if(aVi-hist[i]>aVp+2) {indc=i-1;break;}
               k++;
               aV+=(LONG)hist[i];
               aVi=(int)(aV/(LONG)k);
               if(k>tHDpi) {indc=-1;break;}
               }
            if(indc<0)  {goto NXT;}
            if(indc==0) {lB=lenH-1;}
            else        {lB=indc;}

            count=0;
AGNI:       count++;
            if(lT-2>0) {
                min=hist[lT-2];
                if(lT-3>0) {
                        if(min>hist[lT-3]) {min=hist[lT-3];}
                        }
                if(2*min<hist[lT-1]) {lT--;}
                }
            if(lB+2<lenH) {
                min=hist[lB+2];
                if(lB+3<lenH) {
                        if(min>hist[lB+3]) {min=hist[lB+3];}
                        }
                if(2*min<hist[lB+1]) {lB++;}
                }
             if(count<2) {goto AGNI;}

            if(lB-lT>=tHDpi) {goto NXT;}
            k=0;
            for(j=lT;j<=lB;j++) {
                if(hist[j]>maxLenLine) {k=1;break;}
                }
            if(!k) {goto NXT;}

            suspLineH2(lpWorkBuffer,lT+y1,lB+y1,lenLine,alpha,scaleAngle);

/*????????????????????????????????????????????????????*/
            ii=lB+1;
/*????????????????????????????????????????????????????*/

NXT:
            max=0;
            }
        }

     return(0);
} /* of linesHCellB() */

void smoothH(int *hist, int n, int *outH, int tH1, int tH2){
int  i;
int flgMax, k ,j;
int iMin1, iMin2, Min1, Min2;

 outH[0] = n;
 hist++;
 outH++;

 for(i=0; i<=n-1; i++)  outH[i] = *(hist+(long)(i));
 flgMax = 0;
 for(i=0; i<=n-1; i++)  {
    if(!flgMax) {
            if(outH[i] > tH1)
                       flgMax++;
            }
    if(flgMax == 1) {
        if(outH[i] < tH2) {
                   Min1  = outH[i];
                   iMin1 = i;
                   if(i < n - 5) {
                          k = 0;
                          for(j =i+1;j < i+3;j++) {
                                   if(outH[j] < Min1) {
                                        Min1  = outH[j];
                                        iMin1 = j;
                                        }
                                   if(outH[j] > tH2 +2)
                                                  k = 1;
                                   }
                          if(!k) {
                             flgMax = 0;
                             continue;
                             }
                          k = 0;
                          for(j=iMin1;j > max(iMin1 - 4,1);j--) {
                                           if(outH[j] > tH1)
                                                       k =1;
                                           }
                          if(!k) {
                             flgMax = 0;
                             continue;
                             }
                          for(j=iMin1;j < min(iMin1 + 4,n-1);j++) {
                                           if(outH[j] > tH1)
                                                       k =1;
                                           }
                          if(!k) {
                             flgMax = 0;
                             continue;
                             }
                          Min2 = 10000;
                          iMin2 = 0;
                          for(j =iMin1+4;j < min(iMin1+15,n-1);j++) {
                                   if(outH[j] < tH2) {
                                        Min2  = outH[j];
                                        iMin2 = j;
                                        }
                                   }
                          if(iMin2 == 0)  {
                             flgMax = 0;
                             continue;
                             }
                          if(Min2 > tH2) {
                             flgMax = 0;
                             continue;
                             }
                          for(j=iMin2;j < min(iMin2 + 4,n-1);j++) {
                                           if(outH[j] > tH1)
                                                       k =1;
                                           }
                          if(!k) {
                             flgMax = 0;
                             continue;
                             }
                          if((iMin1 > 2) && (iMin1 < n-2) &&
                             (iMin2 > 2) && (iMin2 < n-2)) {
                             for(j=iMin1;j <= iMin2;j++) {
                               outH[j] += (outH[j-1] +outH[j+1]);
                               outH[j] /=3;
                               }
                            }
                          flgMax = 0;
                          }
                   else
                      break;
                   }

         }
    } /* for(i) */


} /* of smoothH() */

void enhanceHistForTextLineRec(int *hist, int lenH){
	int i, j, len, av, kk = 0;
	int th1, th2, stop;
	av= 0;
	for(i=2; i<lenH-2;i++){
//   printf("--------------------i= %d, hist[i]= %d \n",i, hist[i]);
		if(hist[i] > 100) {av += hist[i];kk++;}
	}
	if(kk > 0)
		av /= kk;
//	th1 = (2*max)/3;
//	th2 = max/2;
	th1 = (2*av)/3+10;
	th2 = av/2+10;

	len  = 0;
//printf("--------------------av= %d, th1= %d, th2= %d \n",av, th1, th2);

	for(i=2; i<lenH;i++){
		if(hist[i] > th1)
			len++;
		else
			len = 0;

		if(len > 20){

		   stop = min(i+20, lenH);
//printf("--------------------i= %d, stop= %d, lenH= %d \n",i, stop, lenH);
		   for(j=i; j<stop;j++){
			  if(hist[j] < th2){
//	        printf("--------------------j= %d, hist[j]= %d \n",j, hist[j]);
				 hist[j] /= 2;
//	        printf("--------------------j= %d, hist[j]= %d \n",j, hist[j]);
			  }
		   }
		   i = stop;
		   len = 0;
		}
	}

}
/***************************************************************************
 *  PURPOSE      :looking for text lines (font case)                       *
 *                                                                         *
 ***************************************************************************/
int  linesForFonts(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
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
             linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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
                   {linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);flgVertRemov = 1;}
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
                       linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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
                          linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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

} /* linesForFonts */

int  linesForFontsFrame(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
                   int wCharType, int lRemH, int lRemV, int horLineM, int indFr){
  int i, difL, shFt, kk, nn;
  int max,  y1, j, iAv;
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
// printf("--------------------tH1= %d, tH2= %d \n",tH1, tH2);

//     indFr=0;
     shFt=2;
     kk=1;
     difL=min(lenH,MAXPROFILE);
     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);

        av=0L;
        kk=0;
        iAv=0;
        for(i=shFt;i<difL;i++) {
           if(hist[i]>0) {av+=(LONG)hist[i];kk++;}
//  printf("--------------------i= %d, hist[i]= %d \n",i, hist[i]);
           }
        if(kk>0) {iAv=(int)(av/(LONG)kk);}
//      printf("--------------------iAv= %d \n",iAv);

        j=iAv*3;
        av=0L;
        kk=0;
        iAv=0;
        for(i=shFt;i<difL;i++) {
           if((hist[i]>0)&&(hist[i]<j)) {av+=(LONG)hist[i];kk++;}
           }
        if(kk>0) {iAv=(int)(av/(LONG)kk);}

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
//  printf("--------------------horLineM= %d, lRemH= %d \n",horLineM, lRemH);

     if(lRemH) {
          if(horLineM) {
            linesHCellB(lpWorkBuffer,hist,lenH,20,40,alpha,scaleAngle);
            callRemLineH(lpWorkBuffer,alpha,scaleAngle);
            if(horLineM >= 8)
                    lRemH = 0;
            }
          else
             filtrHist(lpWorkBuffer,hist,lenH,lpHistH,nn,(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),
                (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW)+(lpWorkBuffer -> Width)-1,alpha,scaleAngle);}
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
             linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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
                   {linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);flgVertRemov = 1;}
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
                       linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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
                          linesUnp(lpHistH,lenH,lpWorkBuffer,lineH);
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

//   printf("------------ linesForFontsFrame, lpWorkBuffer->infFrame[indFr]).nLines= %d indFr= %d \n", (lpWorkBuffer->infFrame[indFr]).nLines, indFr);
   j = y1; // to avoid compiler warnings
//   for(j=0;j<(lpWorkBuffer->infFrame[indFr]).nLines;j++) {
//       printf("------------ line j= %d, (lpWorkBuffer->line[indFr]).y1[j]= %d (lpWorkBuffer->line[indFr]).y2[j]= %d \n", j, (lpWorkBuffer->line[indFr]).y1[j], (lpWorkBuffer->line[indFr]).y2[j]);
//   }

     return(0);

} /* linesForFontsFrame */



void linesFrameM(int  indFr, STRUCTWB *lpWorkBuffer, int lineHeight, int Method)
{

   int i, len, flg_g, f_time, count_0, y1, indL, tH, delt, tH1;
   int minBl, tHlp1, tHlp2, flg_gB;


//       if(lineHeight<=0) lineHeight=10;

       if(lineHeight<=0) lineHeight=1000;
       len=hntHist[0];
       tH=TH_LEVEL_HIST;
       flg_g=0;
       flg_gB=0;
       count_0=0;
       f_time=0;
       indL=0;
       minBl=(int)((float)6*((float)(lpWorkBuffer -> Dpi.x)/(float)300.0));
//       tH1=max((lpWorkBuffer->  lettWidth)/5,minBl);

       tH1=0;
       if(Method==1) tH1=2;
       tHlp1=tH1;
       tHlp2=min(lineHeight/5,minBl);
       y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
       i=1;
       while(i<len) {

             if(flg_g>lineHeight) {
                i+=(-count_0);
                if(count_0) i++;
                (lpWorkBuffer->line[indFr]).y2[indL]=i+y1;
                indL++;
//                flgHCut=0;
                flg_g=0;
                flg_gB=0;
                tH1=tHlp1;
                count_0=0;
                if(indL>=MAXLINES) {
                        (lpWorkBuffer->infFrame[indFr]).nLines=indL;
                        return;
                        }
                }
             else {
                if(hntHist[i]>tH)  {
                     if(flg_g==0) {
                          (lpWorkBuffer->line[indFr]).y1[indL]=max(0,i+y1-1);
//                          max=0;
//                          countM=0;
//                          massL=0;
                          }
                     flg_g++;
                     flg_gB++;
                     count_0=0;
//                     if(flg_g>lineHeight) flgHCut=1;
                     if(flg_g>tH1+2) tH1=tHlp2;
                     if(flg_g>lineHeight-4) tH1=tHlp1;
//                     massL+=hntHist[i];
//                     if(max<hntHist[i]) max=hntHist[i];
//                     if(max>40) countM++;
//                     if(countM>15) tH=6;
                        }
                 else{
                    if(flg_g>lineHeight-4) tH1=tHlp1;
                    if(((flg_gB>TH_C_LEVEL_HIST-1)&&(count_0>tH1-1)) ){
                           delt=0;
                           i+=(delt-count_0);
                           (lpWorkBuffer->line[indFr]).y2[indL]=min(i+y1,(lpWorkBuffer->begRect.y)+(lpWorkBuffer -> Height)-1);
                           indL++;
//                           flgHCut=0;
                           if(indL>=MAXLINES) {
                                (lpWorkBuffer->infFrame[indFr]).nLines=indL;
                                return;
                                }
//                           tH=TH_LEVEL_HIST;
//                           }
                        }
                    if((flg_g>0)&&(count_0<tH1)) flg_g++;
                    count_0++;
                    if(count_0>tH1) {
                        flg_g=0;
                        flg_gB=0;
                        tH1=tHlp1;
//                        tH1=0;
                        }

                    }
                }
               i++;

               } /* while */

            if(flg_g>TH_C_LEVEL_HIST){
                  i-=count_0;
                  i+=(y1);
                  i=min(i,(lpWorkBuffer -> hightImg)-2);
                  (lpWorkBuffer->line[indFr]).y2[indL]=min(i,(lpWorkBuffer->begRect.y)+(lpWorkBuffer -> Height)-1);
                  indL++;
                  }
            (lpWorkBuffer->infFrame[indFr]).nLines=indL;

  } /* of linesFrameM() */

/***************************************************************************
 *									   *

 *  PURPOSE      :calculate horizontal histogram                           *
 *                                                                         *
 ***************************************************************************/
void  hist_h(STRUCTWB *lpWorkBuffer, int x1, int y1, int x2, int y2){
  int sww, i ,j, dx, x1D8, dy, sum, ind, w_bt;
  LONG nY;
  BYTE  *hptr;
  int  *pp;

              w_bt=(lpWorkBuffer -> widthImg);
              x1D8=x1/8;
              pp=(int *)hntHist;
              dy=y2-y1+1;
              dx=x2/8-x1D8-1;
              *pp=dy;
              pp++;i=0;
              while(i<dy)  {
                sum=0;
                sww=y1+i;
                sww=(lpWorkBuffer -> hightImg)-1-sww;
                nY=(LONG)sww*(LONG)w_bt;
                hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)x1D8;

                ind=(int)(*hptr);
                if(ind<0) {ind+=256;}
                ind=255-ind;
                sum+=FirstZer(ind,x1%8);
                hptr++;
                for(j=0;j<dx;j++)  {
                        ind=(int)(*hptr);
                        if(ind<0) {ind+=256;}
                        ind=255-ind;
                        sum+=gr_ar[ind];
                        hptr++;
                        }
                if( (j+x1D8+1)<(lpWorkBuffer -> widthImg)) {
                  ind=(int)(*hptr);
                  if(ind<0) {ind+=256;}
                  ind=255-ind;
                  sum+=LastZer(ind,8-x2%8);
                  }
//                hptr++;


                *pp=sum;
                i++;
                pp++;
                }

 }/* end of hist_h() */
/***************************************************************************

 *  PURPOSE      :is point x,y black or white                              *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int whatIsThisPreP(BYTE  *lpPointer, int  wImg, int wImgHeight, int xx, int yy){
  int  mask, ch;
  LONG lY;
  BYTE  *hptr;

                           yy=wImgHeight-1-yy;
                           lY=(LONG)yy*(LONG)wImg;
                           hptr=lpPointer+lY+(LONG)(xx/8);
                           mask=LEFT_BIT;
                           mask >>=(xx%8);
                           ch=(~hptr[0])&mask;
                           if(ch!=0) {
                                return(1);
                                }
                           else  {
                                return(0);
                                }
 }
/* of whatIsThisPreP() */
/***************************************************************************
 *                                                                         *
 *  PURPOSE      :is the point (xn,yn) white or black                      *
 *                                                                         *
 *  RETURN VALUE :function returns the values:                             *
 *                1 - black                                                *
 *                0 - white                                                *
 ***************************************************************************/
int whatIsThis(STRUCTWB *lpWorkBuffer, int xn, int yn){
    short mask, ch, w_bt;
    LONG nY;
    BYTE  *hptr;
        w_bt=(lpWorkBuffer -> widthImg);

        yn=(lpWorkBuffer -> hightImg)-1-yn;
        nY=(LONG)yn*(LONG)w_bt;
        hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);

         mask=LEFT_BIT;
         mask >>=(xn%8);
         ch=(~hptr[0])&mask;
         if(ch!=0) {
               return(1);
               }
         else {
               return(0);
               }
  } /* whatIsThis() */

/***************************************************************************
 *                                                                         *
 *  PURPOSE      :calculate gray level on the last byte for histogram      *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int LastZer(int ind, int k){
WORD maskB;

  maskB=MASK;
  maskB<<=k;
  ind&=maskB;
  return(gr_ar[ind]);

} /* of LastZer() */
/***************************************************************************
 *                                                                         *
 *  PURPOSE      :calculate gray level on the first byte for histogram     *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int FirstZer(int ind,int k){
WORD maskB;

  maskB=MASK;
  maskB>>=k;
  ind&=maskB;
  return(gr_ar[ind]);

} /* of FirstZer() */
/***************************************************************************
 *  PURPOSE      :which histogram is the best                              *
 *                                                                         *
 ***************************************************************************/
int whichHist(
   WORD    wWidth,          /* frame width on the image */
   int   **lpHists,        /* pointer to histograms    */
   int     nH,              /* number of histograms     */
   int     lenH,            /* lenght of histogram      */
   float  *lptG            /* pointer to tangents */
   )

{
  int     chH, maxV, dif, d, begH, endH, lH, iV, i, j;
  LONG    sum, totalS;
  int  *pH;
     chH=-1;
     maxV=0;
     totalS=0L;
     for(i=0;i<nH;i++) {
        d=(int)(lptG[i]*(float)wWidth);
        if(d<0) {d=-d;}
        begH=0;
        if(lptG[i]>(float)0.0) {begH=d;}
        endH=lenH;
        if(lptG[i]<0)         {endH=lenH-d;}
        lH=endH-begH-1;
        if(lH<=0) {continue;}
        pH=lpHists[i];
        sum=0L;
        for(j=begH+1;j<endH;j++) {
                dif=pH[j]-pH[j-1];
                if(dif>0) {sum+=(LONG)dif;}
                else      {sum-=(LONG)dif;}
                }
        iV=(int)((sum*10L)/(LONG)lH);
        if(iV>maxV) {maxV=iV;chH=i;}
        totalS+=(LONG)iV;
        }
     if(chH<0) {chH=0;}
     return(chH);

} /* whichHist() */
#define DENSTH  (float)0.63

/***************************************************************************
 *  PURPOSE      :put white pixels                                         *
 *                                                                         *
 ****************************************************************************/

int suspLineH2(STRUCTWB *lpWorkBuffer, int yT, int yB, int lenLine, LONG alpha, int scaleAngle){
    short w_bt;
    int   flg_hLM, eps, dd, tH8, tH12;
    int  lByte, rByte, rL, rR, k, xn, yyn, kk;
    int  y1, y2, kDirect, sumP, htH, gap;
    int  swtP, swtP2, yCT, yCB, x1, x2, cFlg, yCT1, yCB1;
    int  begLine, flgLine, begLyT = 0, begLyB = 0, densB = 0, lenWhite = 0;
    float tG, densFl;

        x1= (lpWorkBuffer->begRect.x);
        x2= x1+(lpWorkBuffer -> Width)-1;
        flg_hLM = lpWorkBuffer -> horLineMM;
        gap=(int)((float)10*((float)(lpWorkBuffer -> Dpi.x)/(float)200.0)+(float)0.5);

        tG=calculateTG(alpha,scaleAngle);
        if((tG<EPSTG)&&(tG>(-EPSTG))) {
                swtP2=32000;
                swtP=0;
                }
        else {
                swtP2=(int)((float)1/tG);
                if(swtP2<0) {swtP2=-swtP2;}
                swtP=swtP2/2;
                }
        kDirect=1;
        if(alpha>0) {kDirect=-1;}

        sumP=0;
        sumP+=swtP;
        k=yB-yT+1;
        htH=k/2;
        if((k%2)!=0) {htH++;}
        if(htH==0) {htH=1;}

        w_bt=(lpWorkBuffer -> widthImg);

        y1=(lpWorkBuffer->begRect.y);
        y2=y1+(lpWorkBuffer -> Height)-1;

        lByte=x1/8;
        rL   =x1%8;
        if(rL!=0) {lByte++;}

        rByte=x2/8;
        rR   =x2%8;
        if(rR!=7) {rByte--;}
        yCT=yT;
        yCB=yB;
        yCT1=max(yCT-1,y1);
        yCB1=min(yCB+1,y2);

        flgLine=0;
        begLine=0;
        if(flg_hLM) {
           tH8  = (int)((float)8*((float)(lpWorkBuffer -> Dpi.x)/(float)200.0)+(float)0.5);
           tH12 = (int)((float)12*((float)(lpWorkBuffer -> Dpi.x)/(float)200.0)+(float)0.5);
           eps  = 1;
           if(flg_hLM != 4)
                gap += 2;
           else
                gap -= 2;
           htH /= 2;
           if(htH == 0) htH++;
           dd = yCB - yCT;
           if(dd < tH8) {
              yCT  = max(yCT-eps,y1);
              yCB  = min(yCB+eps,y2);
              }
           if(dd > tH12) {
              eps  = (dd - tH12)/2;
              yCT  = yCT + eps;
              yCB  = yCB - eps;
              }
           yCT1 = max(yCT-1,y1);
           yCB1 = min(yCB+1,y2);
           }
        if(rL!=0) {
                  for(kk=rL;kk<8;kk++) {
                        if(sumP>swtP2) {
                           sumP=0;
                           yCT+=kDirect;
                           yCB+=kDirect;
                           yCT1=max(yCT-1,y1);
                           yCB1=min(yCB+1,y2);
                           }

                        xn=(lByte-1)*8+kk;
                        cFlg=0;
                        for(yyn=yCT1;yyn<=yCB1;yyn++) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               }
                           }
                        if((cFlg>=htH)&&(!flgLine)) {
                                flgLine=1;
                                begLine=xn;
                                begLyT=yCT;
                                begLyB=yCB;
                                densB=1;
                                lenWhite=0;
                                }
                        else {
                                if(cFlg>=htH) {
                                        densB++;
                                        lenWhite=0;
                                        }
                                if((cFlg<htH)&&(flgLine)) {
                                        lenWhite++;
                                        densFl=(float)densB/(float)(xn-begLine);
                                        if((densFl<(float)0.5)||(lenWhite>gap)) {
                                                lenWhite=0;
                                                flgLine=0;
                                                }
                                        }
                                }
                        }

                  sumP+=8-rL;
                  }

        for(k=lByte;k<=rByte;k++) {
                 if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       yCT1=max(yCT-1,y1);
                       yCB1=min(yCB+1,y2);
                       }

                 for(kk=0;kk<8;kk++) {
                        xn=k*8+kk;
                        cFlg=0;
                        for(yyn=yCT1;yyn<=yCB1;yyn++) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               }
                           }
                        if((cFlg>=htH)&&(!flgLine)) {
                                flgLine=1;
                                begLine=xn;
                                begLyT=yCT;
                                begLyB=yCB;
                                densB=1;
                                lenWhite=0;
                                }
                        else {
                                if(cFlg>=htH) {
                                        densB++;
                                        lenWhite=0;
                                        }
                                if((cFlg<htH)&&(flgLine)) {
                                        lenWhite++;
                                        densFl=(float)densB/(float)(xn-begLine);
                                        if((densFl<(float)0.5)||(lenWhite>gap)) {
                                                if(xn-begLine-lenWhite>lenLine) {
                                                     if(densFl>DENSTH) {
                                                        linesSH[nOfLines].lineTorL =begLyT;
                                                        linesSH[nOfLines].lineBorR =begLyB;
                                                        linesSH[nOfLines].begL     =begLine;
                                                        linesSH[nOfLines].endL     =xn-lenWhite;
                                                        nOfLines++;
                                                        if(nOfLines>=MAXLINESH) {return(-1);}
                                                        }
                                                     }
                                                lenWhite=0;
                                                flgLine=0;
                                                }
                                        }
                                }
                        }
                 sumP+=8;
                 }

          if(rR!=7) {
                  if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       yCT1=max(yCT-1,y1);
                       yCB1=min(yCB+1,y2);
                       }
                  for(kk=0;kk<=rR;kk++) {
                        cFlg=0;
                        xn=(rByte+1)*8+kk;
                        for(yyn=yCT1;yyn<=yCB1;yyn++) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               }
                           }
                        if((cFlg>=htH)&&(flgLine)) {
                           densB++;
                           lenWhite=0;
                           }
                        else {
                           if((cFlg<htH)&&(flgLine)) {
                              lenWhite++;
                              densFl=(float)densB/(float)(xn-begLine);
                              if((densFl<(float)0.5)||(lenWhite>gap)) {
                                 if(xn-begLine-lenWhite>lenLine) {
                                   if(densFl>DENSTH) {
                                     linesSH[nOfLines].lineTorL =begLyT;
                                     linesSH[nOfLines].lineBorR =begLyB;
                                     linesSH[nOfLines].begL     =begLine;
                                     linesSH[nOfLines].endL     =xn-lenWhite;
                                     nOfLines++;
                                     if(nOfLines>=MAXLINESH) {return(-1);}
                                     }
                                   }
                                 lenWhite=0;
                                 flgLine=0;
                                 }
                              }
                           }
                        }/* for kk */
                  }

          if(flgLine) {
                xn=(rByte+1)*8+rR;
                if(xn-begLine-lenWhite>lenLine) {
                  densFl=(float)densB/(float)(xn-begLine);
                  if(densFl>DENSTH) {
                     linesSH[nOfLines].lineTorL =begLyT;
                     linesSH[nOfLines].lineBorR =begLyB;
                     linesSH[nOfLines].begL     =begLine;
                     linesSH[nOfLines].endL     =xn-lenWhite;
                     nOfLines++;
                     if(nOfLines>=MAXLINESH) {return(-1);}
                     }
                  }
                }

  return(0);
  } /* suspLineH2() */
/***************************************************************************
 *  PURPOSE      :tan(alpha)                                                *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
float calculateTG(
   LONG      alpha,            /* angle            */
   int       scaleAngle       /* scale for angle  */
   ){
   float x;
     if(scaleAngle<=0) return(0);
     x=(float)(alpha*PI)/(float)180.0;
     x=x/(float)scaleAngle;
     return((float)tan((double)x));
}  /* calculateTG() */

void simpleFfiltrH(STRUCTWB *lpWorkBuffer,int  *hist, int lenH, LONG alpha, int scaleAngle){
 int  lev    = 3;
 int  lL     = 5;
 int  i, k,  cL, dd, widthTh, y1;
 int  rS, endS;
 long cc;

     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);

     widthTh  = (lpWorkBuffer -> Width)-1;
     widthTh *= 3;
     widthTh /= 5;
     cL = 0;
     for(i=1;i<lenH;i++)  {
        if(hist[i] < lev)  {
            cL++;
            }
        else {
            if((cL > lL) || (cL >= i-1)) {
                endS = i+9;
                if(endS > lenH) endS = lenH;
                rS = 0;
                for(k=i;k<endS;k++)
                    if(hist[k] < lev) { rS = k;break;}
                rS--;
                if(rS >= i) {
                  cc = 0;
                  for(k=i;k<=rS;k++)
                      cc += hist[k];
                  dd = rS - i + 1;
                  if(dd > 1) {
                       k = hist[i];
                       if(hist[rS] < k) k = hist[rS];
                       cc -= k; dd--;
                       }
                  if(dd > 0)  cc /= dd;
                  else        cc = 0;
                  if((cc > 30) && (cc > widthTh)) {
                      endS = rS + lL;
                      if(endS > lenH) endS = lenH;
                      dd = 1;
                      for(k=rS+1;k<endS;k++)
                        if(hist[k] > lev) {dd = 0;break;}

                      if(dd)
                         cc=putWhiteH(lpWorkBuffer,
                                      (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW),
                                      (lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW)+(lpWorkBuffer -> Width)-1,
                                       i+y1-1,rS+y1-1,i+y1-1,rS+y1-1,alpha,scaleAngle,0);
                    }
                  }
                }
            cL = 0;
            }
        }
}
/***************************************************************************
 *  PURPOSE      :put white pixels                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
int putWhiteH(STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB, int yTs, int yBs, LONG alpha, int scaleAngle, int addTest){
    short w_bt;
    LONG nY;
    BYTE *hptr;
    int  lByte, rByte, rL, rR, ynD, k, xn, yyn, yesB, kk;
    int  y1, y2, cc, kDirect, sumP, tHd, tHdr;
    int  swtP, swtP2, yCT, yCB, rTop, rBot, cFlg, tHM, yCT1, yCB1;
    int  iVert, dd, yCTs, yCBs, flgT;
    BYTE mask;
    float tG;
              tG=calculateTG(alpha,scaleAngle);
              if((tG<EPSTG)&&(tG>(-EPSTG))) {
                swtP2=32000;
                swtP=0;
                }
              else {
                swtP2=(int)((float)1/tG);
                if(swtP2<0) {swtP2=-swtP2;}
                swtP=swtP2/2;
                }
              kDirect=1;
              if(alpha>0) {kDirect=-1;}

        tHM=(int)((float)3*((float)(lpWorkBuffer -> Dpi.x)/(float)200.0)+(float)0.5);
        tHM=max(tHM,1);
        tHM=min(tHM,6);
        tHd=1;
        tHdr=1;
        dd=yBs-yTs;
        sumP=xL-((lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW));
        sumP+=swtP;
        w_bt=(lpWorkBuffer -> widthImg);
        y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
        y2=y1+(lpWorkBuffer -> Height)-1;
        lByte=xL/8;
        rL   =xL%8;
        if(rL!=0) {lByte++;}
        rByte=xR/8;
        rR   =xR%8;
        if(rR!=7) {rByte--;}
        cc=0;
        yCT=yT;
        yCB=yB;
        yCTs=yTs;
        yCBs=yBs;
        if(rL!=0) {
                  for(kk=rL;kk<8;kk++) {
                        if(sumP>swtP2) {
                           sumP=0;
                           yCT+=kDirect;
                           yCB+=kDirect;
                           yCTs+=kDirect;
                           yCBs+=kDirect;
                           }

                        yesB=0;
                        cFlg=0;
                        rTop=yCT;
                        rBot=yCB;
                        xn=(lByte-1)*8+kk;
                        for(yyn=yCT-1;yyn>=yCT-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-1) {rTop=yCT-cFlg;}
                                }


                        if(!yesB) {
                          for(yyn=yCT;yyn<=yCB;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 if(!whatIsThis(lpWorkBuffer,xn,yyn)) {yesB=1;break;}
                                 }
                             }
                          }
                        cFlg=0;
                        for(yyn=yCB+1;yyn<=yCB+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-1) {rBot=yCB+cFlg;}
                                }
                        if(yesB) {
                                 ynD=rTop;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
          if(lpWorkBuffer -> horLineMM == 2) {
                 if((rBot - rTop > 0) || (yCT != rTop)) rTop++;
                 }
          else {
             if(lpWorkBuffer -> horLineMM == 3) {
                 if((rBot - rTop > 0) || (yCB != rBot)) rTop--;
                 }
             }
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {

                                 (*hptr) |= mask;
                                 hptr -= (long)w_bt;
                                 }
                             }
                          }
                        else {cc++;}

                        }

             sumP+=8-rL;
             }
          flgT=0;
          for(k=lByte;k<=rByte;k++) {
                  if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       yCTs+=kDirect;
                       yCBs+=kDirect;
                       }
                  for(kk=0;kk<8;kk++) {
                        yesB=0;
                        cFlg=0;
                        xn=k*8+kk;
                        yCT1=yCT;
                        yCB1=yCB;
                        iVert=xn-xL+1;
                        tHd=tHdr;
                        if(addTest)  {
                          if(ind[iVert]>dd+4) {
                                yCT1=yCTs;
                                yCB1=yCBs;
                                if(yCB1-yCT1>1) {yCT1++;}
                                flgT++;
                                }
                          else {
                                if(flgT>3) {
                                     if(ind[iVert]>dd+3) {
                                          yCT1=yCTs;
                                          yCB1=yCBs;
                                          if(yCB1-yCT1>1) {yCT1++;}
                                          }
                                      else {
                                        if(iVert+1<ind[0]) {
                                               if(ind[iVert+1]>dd+3) {
                                                     yCT1=yCTs;
                                                     yCB1=yCBs;
                                                     if(yCB1-yCT1>1) {yCT1++;}
                                                     }
                                                else {flgT=0;}
                                                }
                                        else {flgT=0;}
                                        }
                                      }
                                else {flgT=0;}
                                }
                          }
                        rTop=yCT1;
                        rBot=yCB1;
                        for(yyn=yCT1-1;yyn>=yCT1-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-tHd) {rTop=yCT1-cFlg;}
                                }
                        if(!yesB) {
                          for(yyn=yCT1;yyn<=yCB1;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 if(!whatIsThis(lpWorkBuffer,xn,yyn)) {yesB=1;break;}
                                 }
                             }
                          }
                        cFlg=0;
                        for(yyn=yCB1+1;yyn<=yCB1+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-tHd) {rBot=yCB1+cFlg;}
                                }
                        if(yesB) {
                                 ynD=rTop;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
          if(lpWorkBuffer -> horLineMM == 2) {
                 if((rBot - rTop > 0) || (yCT != rTop)) rTop++;
                 }
          else {
             if(lpWorkBuffer -> horLineMM == 3) {
                 if((rBot - rTop > 0) || (yCB != rBot)) rTop--;
                 }
             }
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 (*hptr) |= mask;
                                 hptr -= (long)w_bt;
                                 }
                             }
                          }
                        else {cc++;}
                        }
                  sumP+=8;
                  }
          if(rR!=7) {
                  if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       }
                  for(kk=0;kk<=rR;kk++) {
                        yesB=0;
                        cFlg=0;
                        rTop=yCT;
                        rBot=yCB;
                        xn=(rByte+1)*8+kk;
                        for(yyn=yCT-1;yyn>=yCT-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-1) {rTop=yCT-cFlg;}
                                }


                        if(!yesB) {
                          for(yyn=yCT;yyn<=yCB;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 if(!whatIsThis(lpWorkBuffer,xn,yyn)) {yesB=1;break;}
                                 }
                             }
                          }
                        cFlg=0;
                        for(yyn=yCB+1;yyn<=yCB+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }

                        if(cFlg<tHM) {
                                yesB=1;
                                if(cFlg<tHM-1) {rBot=yCB+cFlg;}
                                }
                        if(yesB) {

                                 ynD=rTop;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
          if(lpWorkBuffer -> horLineMM == 2) {
                 if((rBot - rTop > 0) || (yCT != rTop)) rTop++;
                 }
          else {
             if(lpWorkBuffer -> horLineMM == 3) {
                 if((rBot - rTop > 0) || (yCB != rBot)) rTop--;
                 }
             }
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 (*hptr) |= mask;
                                 hptr -= (long)w_bt;
                                 }
                             }
                          }
                        else {cc++;}

                        }
             }
  return(cc);
  } /* putWhiteH() */

int remLineH2(STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB, LONG alpha, int scaleAngle){
    short w_bt;
    LONG nY;
    BYTE *hptr;
    int  lByte, rByte, rL, rR, ynD, k, xn, yyn, kk;
    int  y1, y2, kDirect, sumP, yesB;
    int  swtP, swtP2, yCT, yCB, rTop, rBot, cFlg, tHM;
    BYTE mask;
    float tG;
              tG=calculateTG(alpha,scaleAngle);
              if((tG<EPSTG)&&(tG>(-EPSTG))) {
                swtP2=32000;
                swtP=0;
                }
              else {
                swtP2=(int)((float)1/tG);
                if(swtP2<0) {swtP2=-swtP2;}
                swtP=swtP2/2;
                }
              kDirect=1;
              if(alpha>0) {kDirect=-1;}
        tHM=(int)((float)3*((float)(lpWorkBuffer ->Dpi.x)/(float)200.0)+(float)0.5);
        tHM=max(tHM,1);
        tHM=min(tHM,6);
        sumP=xL-(lpWorkBuffer->begRect.x);
        sumP+=swtP;
        w_bt=(lpWorkBuffer -> widthImg);

        y1=(lpWorkBuffer->begRect.y);
        y2=y1+(lpWorkBuffer -> Height)-1;

        lByte=xL/8;
        rL   =xL%8;
        if(rL!=0) {lByte++;}

        rByte=xR/8;
        rR   =xR%8;
        if(rR!=7) {rByte--;}
        yCT=yT;
        yCB=yB;
        if(rL!=0) {
                  for(kk=rL;kk<8;kk++) {
                        if(sumP>swtP2) {
                           sumP=0;
                           yCT+=kDirect;
                           yCB+=kDirect;
                           }
                        yesB=1;
                        cFlg=0;
                        rTop=yCT;
                        rBot=yCB;
                        xn=(lByte-1)*8+kk;
                        for(yyn=yCT-1;yyn>=yCT-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
//                                yesB=1;
                                rTop=yCT-cFlg;
                                }

                        cFlg=0;
                        for(yyn=yCB+1;yyn<=yCB+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }

                        if(cFlg<tHM) {
//                                yesB=1;
                                rBot=yCB+cFlg;
                                }

                        if(yesB) {
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 ynD=yyn;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
                                 hptr[0]|=mask;
                                 }
                             }
                          }
                        }

             sumP+=8-rL;
             }

          for(k=lByte;k<=rByte;k++) {
                  if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       }

                  for(kk=0;kk<8;kk++) {
                        yesB=1;
                        cFlg=0;
                        rTop=yCT;
                        rBot=yCB;
                        xn=k*8+kk;
                        for(yyn=yCT-1;yyn>=yCT-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
//                                yesB=1;
                                rTop=yCT-cFlg;
                                }

                        cFlg=0;
                        for(yyn=yCB+1;yyn<=yCB+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }

                        if(cFlg<tHM) {
//                                yesB=1;
                                rBot=yCB+cFlg;
                                }

                        if(yesB) {
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 ynD=yyn;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
                                 hptr[0]|=mask;
                                 }
                             }
                          }
                        }
                  sumP+=8;
                  }
          if(rR!=7) {
                  if(sumP>swtP2) {
                       sumP=0;
                       yCT+=kDirect;
                       yCB+=kDirect;
                       }

                  for(kk=0;kk<=rR;kk++) {
                        yesB=1;
                        cFlg=0;
                        rTop=yCT;
                        rBot=yCB;
                        xn=(rByte+1)*8+kk;
                        for(yyn=yCT-1;yyn>=yCT-tHM;yyn--) {
                           if(yyn>=y1) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }
                        if(cFlg<tHM) {
//                                yesB=1;
                                rTop=yCT-cFlg;
                                }
                        cFlg=0;
                        for(yyn=yCB+1;yyn<=yCB+tHM;yyn++) {
                           if(yyn<=y2) {
                               if(whatIsThis(lpWorkBuffer,xn,yyn)) {cFlg++;}
                               else                                {break;}
                               }
                           }

                        if(cFlg<tHM) {
//                                yesB=1;
                                rBot=yCB+cFlg;
                                }

                        if(yesB) {
                          for(yyn=rTop;yyn<=rBot;yyn++) {
                             if((yyn>=y1)&&(yyn<=y2)) {
                                 ynD=yyn;
                                 ynD=(lpWorkBuffer -> hightImg)-1-ynD;
                                 nY=(LONG)ynD*(LONG)w_bt;
                                 hptr=(lpWorkBuffer->lpRBuffer)+nY+(LONG)(xn/8);
                                 mask=LEFT_BIT;
                                 mask >>=kk;
                                 hptr[0]|=mask;
                                 }
                             }
                          }
                        }

             }
  return(0);

  } /* remLineH2() */

/***************************************************************************
 * PURPOSE     :horizontal and vertical line removal                       *                                 *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
int callRemLineH(STRUCTWB *lpWorkBuffer, LONG alpha, int scaleAngle){
  int      i;
     for(i=0;i<nOfLines;i++) {
           remLineH2(lpWorkBuffer,linesSH[i].begL,linesSH[i].endL,linesSH[i].lineTorL,linesSH[i].lineBorR,alpha,scaleAngle);
           }

     return(0);
 } /* callRemLineH() */


/***************************************************************************
 *  PURPOSE      :filtration narrow lines from histogram                   *
 *                                                                         *
 ***************************************************************************/
int filtrHist(
STRUCTWB *lpWorkBuffer,   /* pointer to work area - at least ????Kbytes         */
int      *histO,          /* pointer to histogram */
int       lenH,            /* lenght of histogram  */
int      *hist,           /* pointer to smooth histogram */
int       nn,              /* low treshold for histogram */
int       xL,
int       xR,
LONG      alpha,
int       scaleAngle
){
  int i, difL, shFt, flgL, flgC, flgC1, flgR, k;
  int tHC, y1, x1, cc, j, ctHL, tHDpi, maxL, maxL10, fInd, lInd, lTrue = 0;
  int frQ, lTs, lBs, lT, lB, y2;


     y1=(lpWorkBuffer->begRect.y)+(lpWorkBuffer->OffH);
     y2=y1+(lpWorkBuffer -> Height)-1;
     x1=(lpWorkBuffer->begRect.x)+(lpWorkBuffer->OffW);
//     tHDpi=(int)((float)TH_C*((float)(lpWorkBuffer -> Dpi.y)/(float)200.0));

     tHDpi = TH_C;

     shFt=1;
     difL=min(lenH,MAXPROFILE);

     ctHL=1;
     tHC=max(3*nn,10);
     flgL=1;
     flgC=0;
     flgC1=0;
     flgR=0;
     maxL=0;
     fInd=-1;

     for(i=shFt;i<difL;i++) {
          if(hist[i]<2*nn+1) {
                maxL10=maxL/10;
                if(maxL<70) {maxL10++;}
                if(maxL<30) {maxL10++;}
                if(lpWorkBuffer->chType < 4)
                           if(maxL<25) {maxL10=1;}
                if(maxL<20) {maxL10=1;}
                if(maxL<15) {maxL10=0;}

                lTrue=0;
                if(fInd<0) {lTrue=1;}
                else {
                           if(hist[fInd]<maxL10) {lTrue=1;}
                           }
                if(lTrue) {
                  if(i<lenH-1) {
                           if(hist[i]<hist[i-1]) {lInd=i;}
                           else                  {lInd=i-1;}
                           }
                  else {
                           if(i<lenH) {lInd=i;}
                           else       {lInd=-1;}
                           }
                  if(lInd>0)  {
                           if(hist[lInd]>=maxL10) {lTrue=0;}
                           }
                  }
                if(lTrue) {
                        cc=i-flgC;
                        if(cc<0) {cc=1;}
                        for(j=cc+1;j<i-1;j++) {
                             if(hist[j]>=maxL10) {break;}
                             flgC--;
                             }
                        for(j=i-1;j>cc+1;j--) {
                             if(hist[j]>=maxL10) {break;}
                             flgC--;
                             }

                        }
                if((flgC>0)&&(flgC<=tHDpi)&&(flgC1>0)&&(lTrue)) {
                        flgR++;
                        if(flgR>=ctHL) {
                            lT=i-flgC-flgR;
                            lB=i-flgR;
                            histVertR(lpWorkBuffer,(int *)ind,xL, xR, max(lT+y1-1-8,y1),min(lB+y1-1,y2));

                            lTs=lT;
                            lBs=lB;
                            memset(xxx ,0x00,(unsigned int)(40));
                            for(j=1;j<ind[0];j++) {
                               if(ind[j]<=flgC) {xxx[ind[j]]++;}
                               }
                            frQ=0;k=flgC;
                            for(j=1;j<=flgC;j++) {
                               if(frQ<xxx[j]) {frQ=xxx[j];k=j;}
                               }
                            if((k<flgC)&&(frQ>10)) {
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

                            cc=putWhiteH(lpWorkBuffer,xL,xR,
                                          lT+y1-1,lB+y1-1,lTs+y1-1,lBs+y1-1,alpha,scaleAngle,0);
                                for(j=i-flgC-flgR;j<=i-flgR;j++) {
                                        histO[j]=cc;
                                        hist[j]=cc;
                                        }


                                flgL=flgR;
                                flgC=0;
                                flgC1=0;
                                flgR=0;
                                }
                        }
                else {
                        flgL++;
                        flgC=0;
                        flgC1=0;
                        flgR=0;
                        maxL=0;
                        }
                }
          else  {
                if(flgL>=ctHL) {
                        if(flgC==0) {
                                if(i>2) {
                                        if(hist[i-1]<hist[i-2]) {fInd=i-1;}
                                        else                    {fInd=i-2;}
                                        }
                                else {
                                        if(i>1) {fInd=i-1;}
                                        else    {fInd=-1;}
                                        }
                                }
                        flgC++;
                        if(hist[i]>tHC) {
                                flgC1++;
                                if(maxL<hist[i]) {maxL=hist[i];}
                                }
                        }
                 else {
                      flgL=0;
                      flgC=0;
                      flgC1=0;
                      flgR=0;
                      }
                 }

        }

        if((flgC>0)&&(flgC1>0)) {
                maxL10=maxL/10;
                if(maxL<70) {maxL10++;}
                if(maxL<30) {maxL10++;}
                if(maxL<20) {maxL10=1;}
                lTrue=0;
                if(fInd<0) {lTrue=1;}
                else {
                           if(hist[fInd]<maxL10) {lTrue=1;}
                           }
                if(lTrue) {
                        cc=fInd;
                        if(cc<0) {cc=1;}
                        for(j=cc+1;j<difL;j++) {
                             if(hist[j]>=maxL10) {break;}
                             flgC--;
                             }
                        for(j=difL-1;j>cc+1;j--) {
                             if(hist[j]>=maxL10) {break;}
                             flgC--;
                             }
                        }
                }


     if((flgC>0)&&(flgC<=tHDpi)&&(flgC1>0)&&(lTrue)&&(flgL>=ctHL)) {
          cc=putWhiteH(lpWorkBuffer,xL,xR,
                    i-flgC-flgR+y1-1,i-flgR+y1-1,i-flgC-flgR+y1-1,i-flgR+y1-1,alpha,scaleAngle,0);
          for(j=i-flgC-flgR;j<=i-flgR;j++) {
              histO[j]=cc;
              hist[j]=cc;
              }

          }
     return(0);

} /* of filtrHist() */
/***************************************************************************
 *  PURPOSE      :initialization Teta array                                *
 *                                                                         *
 ***************************************************************************/
void initTeta(void)
    {
    invTet[0]=0;invTet[1]=1;invTet[2]=2;invTet[3]=3;invTet[4]=4;
    invTet[86]=5;invTet[87]=6;invTet[88]=7;invTet[89]=8;invTet[90]=9;
    invTet[91]=10;invTet[92]=11;invTet[93]=12;invTet[94]=13;
    invTet[176]=14;invTet[177]=15;invTet[178]=16;invTet[179]=17;

    }/* of initTeta() */

