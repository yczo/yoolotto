/*
 * RemoveBackground.c
 *
 *  Created on: Aug 25, 2012
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



#define  GRID_X       50   /* max size of rectangle grid in bits (x)  */
#define  GRID_Y       50   /* max size of rectangle grid in bits (y)  */
#define  MAXGRID      1000     /* max number of rectangles in grid        */
#define  SCALEC       65536L  /* scale for linear coeficient calculation */
#define  EPS_NX        4      /* size of neighbourhood  (x)              */
#define  EPS_NY        4      /* size of neighbourhood  (y)              */
int       gr_ar[]={0,
/*1*/              1,
/*2*/              1,2,
/*4*/              1,2,2,3,
/*8*/              1,2,2,3,2,3,3,4,
/*16*/             1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
/*32*/             1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
/*64*/             1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
/*128*/            1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
/*192*/            2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
                   };

BYTE      mask_L[]={0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00};
BYTE      mask_R[]={0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00};
BYTE      mask_One[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
BYTE      mask_0[]={0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};


RECT          gridTabProt[MAXGRID];   /* prototype grid array */
STRUCT4POINTS gridTabForm[MAXGRID];   /* form grid array */
int  numbOfGrid;
RECT          protRect;


COEF lCoef;
COEF lCoef2;

int angleGlob;
LPBYTE lpZoneGlob;
//RECT   protRect;

int removeFieldBack(
   BYTE *lpPointer,     /* pointer to current ticket buffer  */
   WORD       Width,         /* width of current ticket           */
   WORD       Height,        /* height of current ticket          */
   int        angle,         /* angle for map                   */
   LPBYTE     lpZoneStruct,  /* pointer to zone structure       */
   RECT    *lpFieldRect,   /* pointer to field rectangle      */
   BYTE  *BackStruct     /* pointer to background buffer    */
   ){
  int ret, i, ii, x, y, x1, x2, wImg, yy, ind;
  int xB, xE, k1, k2, yyB, yyE, xxB, xxE;
  int ch, mask, xCur, yCur;
  int LineWidth, ImgHeight;
  LONG lY;
  BYTE  *hptr;

    angleGlob=angle;
    lpZoneGlob=lpZoneStruct;

    protRect.left   = lpFieldRect->left;
    protRect.right  = lpFieldRect->right-1;
    protRect.top    = lpFieldRect->top;
    protRect.bottom = lpFieldRect->bottom-1;


    LineWidth=protRect.right - protRect.left+1;
    ImgHeight=protRect.bottom- protRect.top+1;
    wImg  =((LineWidth+31)/32)*4;

    ret=grid(Width,Height,(RECT*)&protRect,(RECT*)&gridTabProt[0],(STRUCT4POINTS *)&gridTabForm[0],
    		(int *)&numbOfGrid,GRID_X,GRID_Y,angle,lpZoneStruct,SCALEC);

    if(ret<0) {
             return(-1);
             }

/* rectangles loop */
    for(i=0;i<numbOfGrid;i++)  {
        ret=linearCoef((RECT*)&gridTabProt[i],(STRUCT4POINTS *)&gridTabForm[i],(COEF *)&lCoef);
            if(ret<0) {
//              MessageBeep(0);
//              MessageBox(hWnd,"linearCoef() ERROR !!!",NULL, MB_OK );
              return(-2);
              }
/* y loop */
        yyB=gridTabProt[i].top-protRect.top;
        yyE=gridTabProt[i].bottom-protRect.top;
        xxB=gridTabProt[i].left-protRect.left;
        xxE=gridTabProt[i].right-protRect.left;


        for(y=yyB;y<=yyE;y++) {
/* x loop */
              yCur=y+protRect.top;
              k1=xxB%8;
              xB=xxB/8;
              x1=xB;
              if(k1>0) x1++;
              k2=xxE%8;
              xE=xxE/8;
              x2=xE;
              if(k2<7) x2--;

/* first byte */
                 if(k1>0) {
                   yy=y;
                   yy=ImgHeight-1-y;
                   lY=(long)yy*(long)wImg;
                   hptr=BackStruct+lY+(LONG)xB;
                   ind=(int)(*hptr);
                   if(ind<0) {ind+=256;}
                   ind=255-ind;
                   ind&=mask_R[k1];
                   if(gr_ar[ind]>0) {
                       mask=LEFT_BIT;
                       mask >>=k1;
                       for(ii=k1;ii<8;ii++) {
                         ch=ind&mask;
                         if(ch!=0) {
                            putImgF(xB*8+protRect.left+ii,yCur,SCALEC,lpPointer,Width,Height,EPS_NX,EPS_NY);
                            }
                         mask >>=1;
                         }
                       }
                   }

/* interior bytes */
              for(x=x1;x<=x2;x++) {
                   xCur=x*8+protRect.left;
                   yy=y;
                   yy=ImgHeight-1-y;
                   lY=(LONG)yy*(LONG)wImg;
//                   hptr=(lpBackStruct->lpPointer)+lY+(LONG)x;
                   hptr=BackStruct+lY+(LONG)x;
                   ind=(int)(*hptr);
                   if(ind<0) {ind+=256;}
                   ind=255-ind;
                   if(gr_ar[ind]>0) {
                      mask=LEFT_BIT;
                      for(ii=0;ii<8;ii++) {
                        ch=ind&mask;
                        if(ch!=0) {
                            putImgF(xCur+ii,yCur,SCALEC,lpPointer,Width,Height,EPS_NX,EPS_NY);
                            }
                        mask >>=1;
                        }
                      }
                   } /* x loop */

/* last byte */
                 if(k2<7) {
                   yy=y;
                   yy=ImgHeight-1-y;

                   lY=(LONG)yy*(LONG)wImg;
                   hptr=BackStruct+lY+(LONG)xE;
                   ind=(int)(*hptr);
                   if(ind<0) {ind+=256;}
                   ind=255-ind;
                   ind&=mask_L[7-k2];
                   if(gr_ar[ind]>0) {
                       mask=LEFT_BIT;
                       for(ii=0;ii<=k2;ii++) {
                          ch=ind&mask;
                          if(ch!=0) {
                            putImgF(xE*8+protRect.left+ii,yCur,SCALEC,lpPointer,Width,Height,EPS_NX,EPS_NY);
                              }
                          mask >>=1;
                          }
                       }
                   }

              } /* y loop */

        } /* rectangles loop */

//wsprintf(str,"1:x=%d,y=%d  ",x,y);
//TextOut(hDCC,2,20,str,strlen(str));

    return(0);

} /* removeFieldBack() */

int linearCoef(
   RECT           *Prot,   /* prototype rectangle */
   STRUCT4POINTS  *Form,   /* polygon */
   COEF           *Coef      /* coeficients */
   )
{
  int den1, den2;
  LONG A1, B1, rD;

/* b1 */
    den1=(Prot->bottom)-(Prot->top);
    if(den1<2) return(-1);
    B1=(LONG)((Form->lB).x-(Form->lT).x)*SCALEC;

    rD=B1%(LONG)den1;
    B1/=(LONG)den1;
    if(rD>den1>>1) B1++;
    (Coef->B1)=B1;

/* a1 */
    den2=(Prot->right)-(Prot->left);
    if(den2<2) return(-1);
    A1=(LONG)((Form->rT).x-(Form->lT).x)*SCALEC;

    rD=A1%(LONG)den2;
    A1/=(LONG)den2;
    if(rD>den2>>1) A1++;
    (Coef->A1)=A1;

/* c1 */
    (Coef->C1)=(LONG)(Form->lT).x*SCALEC-A1*(long)(Prot->left)-B1*(LONG)(Prot->top);

/* b2 */
    B1=(LONG)((Form->lB).y-(Form->lT).y)*SCALEC;

    rD=B1%(LONG)den1;
    B1/=(LONG)den1;
    if(rD>den1>>1) B1++;
    (Coef->B2)=B1;

/* a2 */
    A1=(LONG)((Form->rT).y-(Form->lT).y)*SCALEC;

    rD=A1%(LONG)den2;
    A1/=(LONG)den2;
    if(rD>den2>>1) A1++;
    (Coef->A2)=A1;

/* c2 */
    (Coef->C2)=(LONG)(Form->lT).y*SCALEC-A1*(long)(Prot->left)-B1*(LONG)(Prot->top);

    return(0);

} /* linearCoef() */



int grid(
   WORD       Width,              /* width of current form           */
   WORD       Height,             /* height of current form          */
   RECT         *Prot,       /* prototype rectangle          */
   RECT          *gTabProt,   /* grid for prototype rectangle */
   STRUCT4POINTS  *gTabForm,   /* grid for polygon             */
   int            *nOfGrid,    /* number of grid rectangles    */
   int               gridX,       /* max size of rectangle grid in bits (x) */
   int               gridY,       /* max size of rectangle grid in bits (y) */
   int               angle,       /* angle for map                */
   LPBYTE            lpZoneStruct,/* pointer to zone structure    */
   long              Scale
     )
{
  int h, v, nH, nV, wH, wV, rH,rV;
  int i, iH, j;
  DWORD   acc, rD;
  POINT    p, pF;
  POINTDW   pFL;

    h=(Prot->bottom)-(Prot->top);
    v=(Prot->right)-(Prot->left);
    nV=1;
    wV=v;
    rV=0;
    if(v>gridX) {
        nV=v/gridX;
        nV++;
        wV=v/nV;
        rV=v%nV;
        }

    wH=h;
    nH=1;
    rH=0;
    if(h>gridY) {
        nH=h/gridY;
        nH++;
        wH=h/nH;
        rH=h%nH;
        }

    *nOfGrid=nV*nH;
    if(*nOfGrid>=MAXGRID) return(-1);

/* top vertical line */
    p.y=(Prot->top);
    p.x=(Prot->left);

    for(i=0;i<nV;i++) {
         gTabProt[i].left=p.x;
         gTabProt[i].top =p.y;

         map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
         acc=pFL.x;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.x=(int)acc;
         if(pF.x<0) pF.x=0;
         if(pF.x>(int)Width-1) pF.x=(int)Width-1;

         acc=pFL.y;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.y=(int)acc;
         if(pF.y<0) pF.y=0;
         if(pF.y>(int)Height-1) pF.y=(int)Height-1;


         gTabForm[i].lT.x=pF.x;
         gTabForm[i].lT.y=pF.y;
         if(i>0) {
           gTabProt[i-1].right=p.x;

           gTabForm[i-1].rT.x=pF.x;
           gTabForm[i-1].rT.y=pF.y;
           }
        p.x+=wV;
        }
    p.x=(Prot->right);
    gTabProt[nV-1].right=p.x;
         map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
         acc=pFL.x;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.x=(int)acc;
         if(pF.x<0) pF.x=0;
         if(pF.x>(int)Width-1) pF.x=(int)Width-1;


         acc=pFL.y;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.y=(int)acc;
         if(pF.y<0) pF.y=0;
         if(pF.y>(int)Height-1) pF.y=(int)Height-1;


    gTabForm[nV-1].rT.x=pF.x;
    gTabForm[nV-1].rT.y=pF.y;

/* left horizontal line */
    p.y=(Prot->top)+wH;
    p.x=(Prot->left);

    iH=nV;
    for(i=1;i<nH;i++) {
         gTabProt[iH].left      =p.x;
         gTabProt[iH].top       =p.y;
         gTabProt[iH-nV].bottom =p.y;

         map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
         acc=pFL.x;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.x=(int)acc;
         if(pF.x<0) pF.x=0;
         if(pF.x>(int)Width-1) pF.x=(int)Width-1;


         acc=pFL.y;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.y=(int)acc;
         if(pF.y<0) pF.y=0;
         if(pF.y>(int)Height-1) pF.y=(int)Height-1;


         gTabForm[iH].lT.x   =pF.x;
         gTabForm[iH].lT.y   =pF.y;
         gTabForm[iH-nV].lB.x=pF.x;
         gTabForm[iH-nV].lB.y=pF.y;

         iH+=nV;
         p.y+=wH;
         }
    p.y=(Prot->bottom);
    gTabProt[nV*nH-nV].bottom =p.y;
    map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
    acc=pFL.x;
    rD=acc%Scale;
    acc/=Scale;
    if(rD>Scale>>1) acc++;
    pF.x=(int)acc;
    if(pF.x<0) pF.x=0;
    if(pF.x>(int)Width-1) pF.x=(int)Width-1;


    acc=pFL.y;
    rD=acc%Scale;
    acc/=Scale;
    if(rD>Scale>>1) acc++;
    pF.y=(int)acc;
    if(pF.y<0) pF.y=0;
    if(pF.y>(int)Height-1) pF.y=(int)Height-1;

    gTabForm[nV*nH-nV].lB.x   =pF.x;
    gTabForm[nV*nH-nV].lB.y   =pF.y;


/* interior grid */
    p.y=(Prot->top)+wH;

    iH=0;
    for(j=0;j<nH;j++) {
       p.x=(Prot->left)+wV;
       for(i=1;i<nV;i++) {
         gTabProt[i+iH].bottom   =p.y;

         map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
         acc=pFL.x;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.x=(int)acc;
         if(pF.x<0) pF.x=0;
         if(pF.x>(int)Width-1) pF.x=(int)Width-1;


         acc=pFL.y;
         rD=acc%Scale;
         acc/=Scale;
         if(rD>Scale>>1) acc++;
         pF.y=(int)acc;
         if(pF.y<0) pF.y=0;
         if(pF.y>(int)Height-1) pF.y=(int)Height-1;

         gTabForm[i+iH].lB.x     =pF.x;
         gTabForm[i+iH].lB.y     =pF.y;
         gTabForm[i-1+iH].rB.x   =pF.x;
         gTabForm[i-1+iH].rB.y   =pF.y;

         if(j<nH-1) {
            gTabProt[i-1+iH+nV].right =p.x;

            gTabProt[i+iH+nV].top     =p.y;
            gTabProt[i+iH+nV].left    =p.x;

            gTabForm[i-1+iH+nV].rT.x  =pF.x;
            gTabForm[i-1+iH+nV].rT.y  =pF.y;

            gTabForm[i+iH+nV].lT.x    =pF.x;
            gTabForm[i+iH+nV].lT.y    =pF.y;
            }
         p.x+=wV;
         } /* for V */
       p.x=(Prot->right);
       map(p.x,p.y,(DWORD *)&pFL.x,(DWORD *)&pFL.y,angle,lpZoneStruct);
       acc=pFL.x;
       rD=acc%Scale;
       acc/=Scale;
       if(rD>Scale>>1) acc++;
       pF.x=(int)acc;
       if(pF.x<0) pF.x=0;
       if(pF.x>(int)Width-1) pF.x=(int)Width-1;


       acc=pFL.y;
       rD=acc%Scale;
       acc/=Scale;
       if(rD>Scale>>1) acc++;
       pF.y=(int)acc;
       if(pF.y<0) pF.y=0;
       if(pF.y>(int)Height-1) pF.y=(int)Height-1;


       gTabForm[nV-1+iH].rB.x     =pF.x;
       gTabForm[nV-1+iH].rB.y     =pF.y;
       if(j<nH-1) {
           gTabProt[2*nV-1+iH].right =p.x;
           gTabForm[2*nV-1+iH].rT.x  =pF.x;
           gTabForm[2*nV-1+iH].rT.y  =pF.y;
           }

       iH+=nV;
       p.y+=wH;
       if(j==nH-2) p.y+=rH;
       }


       return(0);
} /* grid() */
int  map(WORD x, WORD y,  DWORD * pX, DWORD * pY, int angle, BYTE *lpStruct){
 WORD  * lpwStruct;
 WORD sizeDX0;
 WORD sizeDY0;
 WORD sizeDX1;
 WORD sizeDY1;
 int dMinX0;
 int dMinX1;
 int dMinY0;
 int dMinY1;
 WORD stepDX0;
 WORD stepDX1;
 WORD stepDY0;
 WORD stepDY1;
 WORD posX0;
 WORD posEndX;
 WORD posY0;
 WORD posEndY;
 WORD * pAllignX;
 WORD * pAllignY;

 long cosTeta;
 long sinTeta;
 WORD nx;
 WORD ny;
 WORD nx1;
 WORD ny1;
 int x1;
 int y1;
 long xr;
 long yr;
 double kx;
 double ky;

 long shift;

 lpwStruct = (WORD *)lpStruct;

 shift = (long)(*lpwStruct/2) + 1L;

 sizeDX0 =       *(lpwStruct + 1L);
 sizeDX1 =       *(lpwStruct + 2L);
 dMinX0  = (int)(*(lpwStruct + 3L));
 dMinX1  = (int)(*(lpwStruct + 4L));
 stepDX0 =       *(lpwStruct + 5L);
 stepDX1 =       *(lpwStruct + 6L);
 posX0 =         *(lpwStruct + 7L);
 posEndX =       *(lpwStruct + 8L);
 pAllignX = lpwStruct + 9L;

 lpwStruct = lpwStruct + shift;

 sizeDY0 =       *(lpwStruct + 1L);
 sizeDY1 =       *(lpwStruct + 2L);
 dMinY0 = (int)(*(lpwStruct + 3L));
 dMinY1 = (int)(*(lpwStruct + 4L));
 stepDY0 =       *(lpwStruct + 5L);
 stepDY1 =       *(lpwStruct + 6L);
 posY0 =         *(lpwStruct + 7L);
 posEndY =       *(lpwStruct + 8L);
 pAllignY = lpwStruct + 9L;

 cosTeta = (long)((cos(angle*PI/1800))*SCALE);
 sinTeta = (long)((sin(angle*PI/1800))*SCALE);

     kx = (x - dMinY0)/((double)stepDY0); if(kx < 0.0) kx = 0.0;
     ky = (y - dMinX0)/((double)stepDX0); if(ky < 0.0) ky = 0.0;
     nx = (WORD)kx;
     ny = (WORD)ky;
     kx = kx - nx;
     ky = ky - ny;

     if(posX0>ny) ny = posX0;
     if(ny >posEndX) ny = posEndX;
     if(posY0>nx) nx = posY0;
     if(nx >posEndY) nx = posEndY;

        nx1 = *(pAllignY+(DWORD)nx);
                 x1 = dMinY1 + nx1*stepDY1 + (int)(kx*stepDY1);
        ny1 = *(pAllignX+(DWORD)ny);
                 y1 = dMinX1 + ny1*stepDX1 + (int)(ky*stepDX1);
        xr = x1*cosTeta - y1*sinTeta;  if(xr < 0) xr = 0;
        yr = x1*sinTeta + y1*cosTeta;  if(yr < 0) yr = 0;
        *pX = (DWORD)xr;
        *pY = (DWORD)yr;

 return(0);
}
int mapQ(LPQUADRANGLE pQuad, int angle, LPBYTE lpStruct, int margin) {


 WORD  * lpwStruct;
 WORD sizeDX0;
 WORD sizeDY0;
 WORD sizeDX1;
 WORD sizeDY1;
 int dMinX0;
 int dMinX1;
 int dMinY0;
 int dMinY1;
 WORD stepDX0;
 WORD stepDX1;
 WORD stepDY0;
 WORD stepDY1;
 WORD posX0;
 WORD posEndX;
 WORD posY0;
 WORD posEndY;
 WORD * pAllignX;
 WORD * pAllignY;

 long cosTeta;
 long sinTeta;
 int x[4];
 int y[4];
 WORD nx;
 WORD ny;
 WORD nx1;
 WORD ny1;
 int x1;
 int y1;
 long xr;
 long yr;
 WORD x2[4];
 WORD y2[4];
 double kx;
 double ky;

 long shift;

 WORD i;
 return 0;

 lpwStruct = (WORD *)lpStruct;

 shift = (long)(*lpwStruct/2) + 1L;


 sizeDX0 =       *(lpwStruct + 1L);
 sizeDX1 =       *(lpwStruct + 2L);
 dMinX0  = (int)(*(lpwStruct + 3L));
 dMinX1  = (int)(*(lpwStruct + 4L));
 stepDX0 =       *(lpwStruct + 5L);
 stepDX1 =       *(lpwStruct + 6L);
 posX0 =         *(lpwStruct + 7L);
 posEndX =       *(lpwStruct + 8L);
 pAllignX = lpwStruct + 9L;


 lpwStruct = lpwStruct + shift;

 sizeDY0 =       *(lpwStruct + 1L);
 sizeDY1 =       *(lpwStruct + 2L);
 dMinY0 = (int)(*(lpwStruct + 3L));
 dMinY1 = (int)(*(lpwStruct + 4L));
 stepDY0 =       *(lpwStruct + 5L);
 stepDY1 =       *(lpwStruct + 6L);
 posY0 =         *(lpwStruct + 7L);
 posEndY =       *(lpwStruct + 8L);
 pAllignY = lpwStruct + 9L;

 cosTeta = (long)((cos(angle*PI/1800))*SCALE);
 sinTeta = (long)((sin(angle*PI/1800))*SCALE);

 x[0] = pQuad -> p1X; y[0] = pQuad -> p1Y;
 x[1] = pQuad -> p2X; y[1] = pQuad -> p2Y;
 x[2] = pQuad -> p3X; y[2] = pQuad -> p3Y;
 x[3] = pQuad -> p4X; y[3] = pQuad -> p4Y;

 x[0] = x[0] - margin; y[0] = y[0] - margin;
 x[1] = x[1] + margin; y[1] = y[1] - margin;
 x[2] = x[2] + margin; y[2] = y[2] + margin;
 x[3] = x[3] - margin; y[3] = y[3] + margin;

 printf("x[0]=%d, y[0]= %d, x[1]= %d, y[1]= %d \n",x[0], y[0], x[1], y[1]);
return 0;
 for(i= 0; i<4; i++)
    {kx = (x[i] - dMinY0)/((double)stepDY0); if(kx < 0) kx = 0;
     ky = (y[i] - dMinX0)/((double)stepDX0); if(ky < 0) ky = 0;
     nx = (WORD)kx;
     ny = (WORD)ky;
     kx = kx - nx;
     ky = ky - ny;

     if(posX0>ny) ny = posX0;
     if(ny >posEndX) ny = posEndX;
     if(posY0>nx) nx = posY0;
     if(nx >posEndY) nx = posEndY;

        nx1 = *(pAllignY+(DWORD)nx);
                 x1 = dMinY1 + nx1*stepDY1 + (int)(kx*stepDY1);
        ny1 = *(pAllignX+(DWORD)ny);
                 y1 = dMinX1 + ny1*stepDX1 + (int)(ky*stepDX1);
        xr = x1*cosTeta - y1*sinTeta;  if(xr < 0) xr = 0;
        yr = x1*sinTeta + y1*cosTeta;  if(yr < 0) yr = 0;
        x2[i] = (WORD)(((DWORD)xr)>>SCALEEXP);
        y2[i] = (WORD)(((DWORD)yr)>>SCALEEXP);
        if(x2[i] > widthT - 1) x2[i] = widthT - 1;
        if(y2[i] > heightT - 1) y2[i] = heightT - 1;

    }

 pQuad -> p1X = x2[0]; pQuad -> p1Y = y2[0];
 pQuad -> p2X = x2[1]; pQuad -> p2Y = y2[1];
 pQuad -> p3X = x2[2]; pQuad -> p3Y = y2[2];
 pQuad -> p4X = x2[3]; pQuad -> p4Y = y2[3];

 return(0);
}

int linearMapL(
     POINT  *lpPoint,
     POINTDW *lpPointWy,
     COEF   *Coef
     )

{
   int x, y;
   long acc;

     x=(lpPoint->x);
     y=(lpPoint->y);

     acc=(Coef->A1)*(long)x+(Coef->B1)*(long)y+(Coef->C1);
     (lpPointWy->x)=(DWORD)acc;


     acc=(Coef->A2)*(LONG)x+(Coef->B2)*(LONG)y+(Coef->C2);
     (lpPointWy->y)=(DWORD)acc;

     return(0);
} /* linearMapL() */

int putImgF(
   int        xx,            /* (xx,yy) point to process */
   int        yy,
   LONG       Scale,
   BYTE       *lpPointer,     /* pointer to buffer               */
   WORD       Width,         /* width   of buffer               */
   WORD       Height,        /* height  of buffer               */
   int        epsNX,         /* size of neighbourhood      (x)  */
   int        epsNY          /* size of neighbourhood      (y)  */
   )
{

    int    x, y, yI, xB, xE, wImg, yB, yE;
    int    x1, x2, k1, k2, dif, dX, dY, h;
    LONG   acc, rD;
    POINTDW pFL;
    POINT  pF;
    LONG   lY;
    BYTE *hptr;


    pF.x=xx;
    pF.y=yy;

    linearMapL((POINT *)&pF,(POINTDW *)&pFL,(COEF *)&lCoef);

    acc=pFL.x;
    rD=acc%Scale;
    acc/=Scale;
    if(rD>Scale>>1) {acc++;dX=-1;}
    else            {dX=0;}
    pF.x=(int)acc;

    acc=pFL.y;
    rD=acc%Scale;
    acc/=Scale;
    if(rD>Scale>>1) {acc++;dY=-1;}
    else            {dY=0;}
    pF.y=(int)acc;



    wImg  =((Width+31)/32)*4;

     if(epsNX%2) {
        h=(epsNX/2);
        x1=pF.x+dX;
        x2=x1+h+1;
        x1-=h;
        }
     else {
        h=(epsNX/2);
        x1=pF.x-h;
        x2=pF.x+h;
        }

     if(epsNY%2) {
        h=(epsNY/2);
        yB=pF.y+dY;
        yE=yB+h+1;
        yB-=h;
        }
     else {
        h=(epsNY/2);
        yB=pF.y-h;
        yE=pF.y+h;
        }


     xB=x1/8;
     xE=x2/8;
     k1=8-x1%8;
     k2=x2%8+1;
     dif=1;
     if(xB==xE) dif=0;


      for(y=yB;y<=yE;y++)  {
         for(x=xB;x<=xE;x++)  {

         if((x>=0)&&(x<wImg)&&(y>=0)&&(y<(int)Height)) {
                           yI=y;
                           yI=Height-1-y;
                           lY=(LONG)yI*(LONG)wImg;
                           hptr=lpPointer+lY+(LONG)x;
                           switch(dif) {
                                case 0:
                                        hptr[0]|=(~(mask_L[k1]|mask_R[k2]));
                                        break;
                                case 1:
                                        if(x==xB) {
                                           hptr[0]|=(~mask_L[k1]);
                                           break;
                                           }
                                        else {
                                           if(x==xE) {
                                              hptr[0]|=(~mask_R[k2]);
                                              break;
                                              }
                                           hptr[0]|=(~mask_L[8]);
                                           break;
                                           }

                                }

                           }
          }
        }
        return(0);

} /* putImgF() */
