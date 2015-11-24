/*
 * UtilRecognition.h
 *
 *  Created on: Aug 31, 2012
 *      Author: Wicek
 */

#ifndef UTILRECOGNITION_H_
#define UTILRECOGNITION_H_

#include  "../headers/wintypes.h"
#include  "../headers/Prototype.h"
#define SIZE_ERR1        -101       /* error if area for segmentation is too small */
#define SIZE_ERR2        -102       /* offset in rectangle area out of the area */
#define SIZE_ERR3        -103       /* width or height of rectangle area too big (max 2400) */
#define SIZE_ERR4        -104       /* rectangele area offset out of bit map   */
#define LINE_ERR1        -105       /* can not find line   */
#define LINE_ERR2        -155       /* removal error */
#define LINE_ERR3        -156       /* point outside the image (skewed histogram calculation) */
#define HIST_SIZE        -157       /* size of skewed histogram is too big */
#define DLL_ERR          -777       /* dll licence error */
#define CELLBOX_ERR      -171
#define TRAPEZOID_ERR    -190


#define  MARG_H         4
#define  DIFC_V         2
#define  DROPBOX        0
#define  CHECKBOX       4
#define  CHECKBOXDROP   5
#define  MULTILINEBOXD  6
#define  MULTILINEBOX   7
#define  MULTIBOX       8
#define  LINES          11
#define  GTESS          61941800
#define  ADD_G          18
#define  MAXAUTOC       60
#define  MINAUTOC       0
#define  MAXCC          100
#define  MAXNUMBCUTS    700
//#define  PI             3.14159265359
#define  MINSIZE        5          /* minimum size of rectangle */
#define  MAXSIZE        2900
#define  FRAMEWIDTH     120        /* frame width for 200 dpi */
#define MAXRECTSIDE   200       /* maximal size of rectangle side */
#define MAXARROUTP    30        /* size of output array */
#define MAXQUANTR     300       /* maximal quantization of R */
#define MAXQUANTTETA  18        /* maximal quantization of teta */
#define MAXQUANTTETAA 20        /* maximal quantization of teta */
#define MAXLINELENGHT 4000      /* maximal lenght of line */
#define  MAXARR         300         /* max size of arrays */
#define QUANTTETA     2         /* quant of teta (Deg) */
#define QUANTSONETETA 5         /* number of quants for one teta (vert/horizon) */
#define MAXLINELENGHT 4000      /* maximal lenght of line */
#define MAXARROUTP    30        /* size of output array */
#define RTETA1        5         /* first teta region border  */
#define RTETA2        14        /* second teta region border  */
#define  MAXPROFILE     2900
#define LENLINE 4000
#define NCHAR   700
#define CWMAX   200
#define SCALEAN     10000
#define  MAXHIST        44
#define  EPSTG          (float)0.00000001
#define  TH_WDT         14
#define MAXNUMBMIN      10
#define MAXLINSP        100
#define  MAXLINESH      10         /* maximum numbers of lines */
#define TH_C            12
#define TH_C2            5
#define TH_END          15
#define TH_LEVEL_HIST    1                //level of histogram to accept line
#define TH_C_LEVEL_HIST     4                //number of lines of above level
#define  MASK           0x00FF     /* number of lines in the picture segment */
#define  LEFT_BIT       0x80        /* left bit = 1 */
#define  RIGHT_BIT      0x01       /* right bit = 1 */
#define  MAXSIZECHAR2   40         /* half of maximum size of characters */
#define  MAXSIZECHAR    96         /* maximum size of characters */
#define  MINWIDTHMARK   2
#define   SCALE2         10000


typedef struct {                    /* structure for frame, line, and  */
                                    /* specks removal functions        */

   BYTE     *lpPointer;         /* pointer to image buffer    */
   int          iLineWidth;         /* real line width in the image (in pixels) */
   int          iImgHeight;         /* height of the image                      */
   int          iXBegin;            /* X offset in bitmap (of rectangle area) in pixels */
   int          iYBegin;            /* Y offset in bitmap (of rectangle area) in pixels */
   int          iWidth;             /* width of rectangle area */
   int          iHeight;            /* height of rectangle area */
   int          iMethod;            /* removal option
                                        < 10 -horizontal and vertical line removal on
                                        >=10 -horizontal and vertical line removal off
                                             0/10 -old cell box processing
                                             1/11 -new cell box processing
                                                   cellWidth is treated as an exact value
                                             2/12 -new cell box processing
                                                   cellWidth is treated as an estimation
                                     */


   int          iMinLH;             /* minimal lenght of horizontal line */
   int          iMaxHGapH;          /* maximal lenght of horizontal gap for horizontal line */
   int          iMaxVGapH;          /* maximal lenght of vertical gap for horizontal line */
   int          iMaxWidthH;         /* maximal width of horizontal line  */
   int          iMinDistH;          /* minimal distance between horizontal lines */
   int          iMaxDifShapeH;      /* maximal diference of shape horizontal line */

   int          iMinLV;             /* minimal lenght of vertical line */
   int          iMaxHGapV;          /* maximal lenght of horizontal gap for vertical line */
   int          iMaxVGapV;          /* maximal lenght of vertical gap for vertical line */
   int          iMaxWidthV;         /* maximal width of vertical line  */
   int          iMinDistV;          /* minimal distance between vertical lines */
   int          iMaxDifShapeV;      /* maximal diference of shape vertical line */

   int          iSpeckSizeH;        /* horizontal speck size (only for specks cleaning) */
   int          iSpeckSizeV;        /* vertical speck size   (only for specks cleaning) */
   int          iCellWidth;         /* cell box width */
   LONG         xxxxxxx;            /* xxxxxxxxxxxxxxxxxxxxxxxxx */
   int          iFiltration;        /* outside filtration */
   int          iRemoval;           /* removal or extraction */


    } SREMOVAL;
    typedef struct                              /* structure for lines     */
    {
        int indTop;                             /* top histogram index     */
        int indBot;                             /* bottom histogram index  */
        int lIns;                               /* if it is line between next line and current  */
    } TMPLINES;
    typedef struct                              /* structure for lines     */
    {
        int indTop;                             /* top histogram index     */
        int indBot;                             /* bottom histogram index  */
    } TMPLINES1;

    typedef struct {

       int          lineTorL;           /* line top or left coef. */
       int          lineBorR;           /* line bot or right coef.*/
       int          begL;               /* begin  coef.           */
       int          endL;               /* end    coef.           */
       } LINESTRUCT;

    int arrTrace[MAXARR+2][2];
    int indxTr;
    BYTE arrExc[256];
    int invTet[200];
    int   quantR;                   /* quant of R */
    int   arrAccum[MAXQUANTR][MAXQUANTTETAA];
    int   indxOutp;
    int   arrOutp[MAXARROUTP*MAXQUANTTETAA][3];
    int   arrLine[MAXLINELENGHT][2];
    int   arrYST[MAXARROUTP];
    int   indxYST;
    int   arrXST[MAXARROUTP];
    int   indxXST;
    int   arrYFT[MAXARROUTP];
    int   indxYFT;
    int   arrXFT[MAXARROUTP];
    int   indxXFT;
    int   hntHist[MAXSIZE];
    int  ind[LENLINE];
    int   lowProf[MAXPROFILE];
    int   upProf[MAXPROFILE];
    int   traceI[MAXLTRACE];
    long  Cost[150];
    long  Cost2[150];
    int var1[LENLINE];
    int var2[LENLINE];
    int confM[LENLINE];
    int dMaxSeg[LENLINE];
    BYTE bufferSeg[2048];
    int  cut[NCHAR];
    int  cA[CWMAX+2];
    long ccc[LENLINE];
    int  xxx[NCHAR];
    int  histHSk[MAXHIST][MAXPROFILE];
    int   scAngleH;
    LONG  angleH;
    LONG  angleHist[MAXHIST];
    float tGAngle[MAXHIST];
    int  *lpHistHSk[MAXHIST];
    int  *lpHistH;
    int  *lpHistH2;
    int  nOfLines;
    int  arrInd[MAXNUMBMIN];
    TMPLINES tmpLines[MAXLINES];
    int  maxIndxLines;
    int tabLine[3][MAXLINSP];
    LINESTRUCT linesSH[MAXLINESH];
    int   drawHist[MAXPROFILE];
    int  histVTmpT[MAXPROFILE];
    int  dd[LENLINE];
    int  begVertLine;
    int  endVertLine;
    LNFR  lineWork[MAXFRAMES];               // array of tops and bottoms lines in frames
    INFFR infFrameWork[MAXFRAMES];           // array of infos for each frame, beginning, end, nof lines
    TMPLINES1 tmpLines1[MAXLINES];
    int           maxIndxLines1;
    long  shfX;
    int   markConst[MAXNUMBCUTS];
    int   markConst2[MAXNUMBCUTS];
    int   indxMarkC;
    int   indxMarkC2;
    int   pOutT[3100];
    int   avArea;
    int   flgUNPROP;
    int   thist[LENLINE];
    long  c[LENLINE];
//    int   cA[CWMAX+2];
//    int   ind[LENLINE];
    int   x[NCHAR];
    int  traceL[MAXLTRACE];
    int  traceC[MAXLTRACE];
    int  lengthTraceL;
    int  lengthTraceC;
    BYTE  normPicture[16*16];
    LPBYTE lpAlphaMap;
    int   indTopBott;
    int   indBottTop;
    POINT topBottEndTr[MAXLTRACE];
    POINT bottTopEndTr[MAXLTRACE];
    int   countI;



// ---------------------------functions ------------------------
    void initRemStruct(  SREMOVAL  *lpData  );
    int cleanSpecks( SREMOVAL  *lpData  );
    int cleanSpecksOut( SREMOVAL  *lpData  );
    void  remBlackA(STRUCTWB *lpWorkBuffer, int x1, int y1, int x2, int y2);
    int searchCloseP( SREMOVAL *lpData,  POINT *lTopP, POINT *lBottomP,  POINT *rTopP, POINT *rBottomP, int typeField);
    int cleanLines( SREMOVAL *lpData);
    int compAccum(SREMOVAL *lpData, int xAct, int yAct, int widthAct, int heightAct, int VertHor);
    int hLineTrackGap( SREMOVAL *lpData, int xSt, int ySt, int direct);
    int clLineH(SREMOVAL *lpData, int maxL);
    int clLineV(SREMOVAL *lpData, int maxL);
    int vLineTrack(SREMOVAL *lpData, int xAct, int yAct, int widthAct, int heightAct, int xSt, int ySt, int direct);
    int  vLineTrackGap(SREMOVAL *lpData, int xSt, int  ySt, int direct);
    int hLineTrack(SREMOVAL *lpData, int xAct, int yAct, int widthAct, int heightAct, int xSt, int ySt, int direct );
    void histVer(STRUCTWB *lpWorkBuffer, int *hist, int x1, int x2, int y1, int y2);
    int histHSkewed(STRUCTWB *lpWorkBuffer, float maxAlpha, float stepAlpha, int *histH, int *lenH, LONG *angleH, int  *scAngleH);
    int histHSkewedFrame(STRUCTWB *lpWorkBuffer, float maxAlpha, float stepAlpha, int *histH, int *lenH, LONG *angleH, int  *scAngleH,
    		int indFr);
    int linesHCellB(STRUCTWB *lpWorkBuffer, int *hist, int lenH, int lenLine, int maxLenLine, LONG alpha, int scaleAngle);
    int  linesForFonts(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
                       int wCharType, int lRemH, int lRemV, int horLineM);
    int  linesForFontsRet_Num(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
                       int wCharType, int lRemH, int lRemV, int horLineM);
    int  linesForFontsFrame(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineH, LONG  alpha, int scaleAngle,
                       int wCharType, int lRemH, int lRemV, int horLineM, int indFr);
    void  hist_h(STRUCTWB *lpWorkBuffer, int x1, int y1, int x2, int y2);
    int whatIsThisPreP(BYTE  *lpPointer, int  wImg, int wImgHeight, int xx, int yy);
    int FirstZer(int ind,int k);
    int LastZer(int ind, int k);
    int whichHist(WORD wWidth, int **lpHists, int nH, int lenH, float  *lptG);
    int suspLineH2(STRUCTWB *lpWorkBuffer, int yT, int yB, int lenLine, LONG alpha, int scaleAngle);
    int whatIsThis(STRUCTWB *lpWorkBuffer, int xn, int yn);
    float calculateTG(LONG  alpha, int scaleAngle);
    void simpleFfiltrH(STRUCTWB *lpWorkBuffer,int  *hist, int lenH, LONG alpha, int scaleAngle);
    int putWhiteH(STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB, int yTs, int yBs, LONG alpha, int scaleAngle, int addTest);
    int callRemLineH(STRUCTWB *lpWorkBuffer, LONG alpha, int scaleAngle);
    int filtrHist(STRUCTWB *lpWorkBuffer, int *histO, int lenH, int *hist, int nn, int xL, int xR, LONG alpha, int scaleAngle);
    void initTeta(void);
    void arrExcF(int nB);
    void cleanSpeck(SREMOVAL *lpData, int wXBegin, int wYBegin);
    int sizeSpot(SREMOVAL *lpData, int wXBegin, int wYBegin);
    int singleLineWh(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight, int startH, int endH);
    int  multiLineWh(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight);
    void maxLineH(STRUCTWB *lpWorkBuffer, int nOfFr, int *hist, int lenH );
    int filtrHistVert(STRUCTWB *lpWorkBuffer, int *hist, int lenH, int xLI, int ver, int *histH, int lenHH, int action);
    int linesUnp(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int lineHeight);
    void histVert(STRUCTWB *lpWorkBuffer );
    int outOfL(STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB, int ver);
    void putWhiteV( STRUCTWB *lpWorkBuffer, int xL, int xR, int yT, int yB);
    void putWhiteVBt(STRUCTWB *lpWorkBuffer, int ver, int xL, int xR, int yT, int yB);
    int isItVertL(STRUCTWB *lpWorkBuffer, int gap, int xL, int xR, int yT, int yB);
    int maxYstruct(STRUCTWB *lpWorkBuffer);
    int searchBeg(STRUCTWB *lpWorkBuffer, int nFr);
    int searchEnd(STRUCTWB * lpWorkBuffer, int nFr);
    void linLoc(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int    iBot, int lineHeight, int  levelC);
    void shiftBord(STRUCTWB *lpWorkBuffer, int *hist, int iTop, int iBot, int lineHeight, int  levelC );
    void prePackL(int ind);
    int srchB(int *hist, int lenH, int iTop, int iBot, int ind, int drc);
    void cpyHist(int *lpHOtp,  int *lpHInp, int lenH);
    void  cleanHist(int *lpH, int len);
    void linesUnp2(int *hist, int lenH, STRUCTWB *lpWorkBuffer, int tHBottom);
    int filtrHistInsL(STRUCTWB *lpWorkBuffer, int *hist, int lenH, int xLI, int xRI, TMPLINES *lpTmpL, int maxIL, long  alpha,
                                 int  scaleAngle, int wCharType);
    void histVertR(STRUCTWB *lpWorkBuffer, int *hist, int x1, int x2, int y1, int y2);
    int  leftBordB(STRUCTWB *lpWorkBuffer, int xLI, int xRI, int yT, int yB);
    int  rightBordB(STRUCTWB *lpWorkBuffer, int xLI, int xRI, int yT, int yB);

    int isLineValid(STRUCTWB *lpWorkBuffer, int wCharType);
    int sendNextFont( STRUCTWB * lpWorkBuffer, char *fieldName);
    int skewTraces(int *Trace, POINT pP1, POINT pP2);
    void setEmptyBuff(LPBYTE lpExtractCh, int width, int height);
    int strongMassTest(STRUCTWB *lpWorkBuffer, int tHm);
    int  min_left(STRUCTWB *lpWorkBuffer);
    int max_current(STRUCTWB *lpWorkBuffer);
    int smSignHnd(STRUCTWB *lpWorkBuffer);
    int whichFrame(STRUCTWB *lpWorkBuffer, int x);
    void shiftTopBordA(STRUCTWB *lpWorkBuffer, int yIn, int iEnd, int *hist);
    int sendToBuffer(STRUCTWB *lpWorkBuffer, LPBYTE lpExtractCh, int width, int height, int charIndex);
    int SegmChars2(STRUCTWB *lpWorkBuffer, int scal, int kindD, int *lpLwidth, char *fieldName, int sizeW);
    int SegmChars(STRUCTWB *lpWorkBuffer, int scal, int kindD, int *lpLwidth, char *fieldName, int sizeW);
    void profiles(STRUCTWB *lpWorkBuffer, int indF);
    int  SegmProfiles(STRUCTWB *lpWorkBuffer, int indF, int k);
    int searchBordLine(STRUCTWB  *lpWorkBuffer, int indF);
    int testE(int scal,STRUCTWB *lpWorkBuffer);
    int  minMaxLok(STRUCTWB * lpWorkBuffer, int indF, int kext, int kindD);
    void histV(STRUCTWB *lpWorkBuffer);
    int segFixedPitch(WORD wDpiX, WORD wDpiY, int * pH, int n, int * pCw, int * pCut,  int * pNCut, long *pCost, int version);
    int segFixedPitch2(WORD wDpiX, WORD wDpiY, int * pH, int n, int * pCw, int * pCut,  int * pNCut, long *pCost, int version, int segmMode);
    int segFixedPitchPartition(WORD wDpiX, WORD wDpiY, int * pH, int n, int cw, int * pCut, int * pNCut, long * pCost, int version);
    int  segFont(IMAGEL *pImage, int * pH, int n,  long angle, long shift, RECCHARSTRUCT * pRecCharStruct,
                 int *pCw, int *pCut, int *pNCut, long *pCost, int *pOut, int version, char *fieldName, int segmMode);
    int  recChar(LPBYTE pBuffer, int width, int height, IMAGEL * pImage, RECCHARSTRUCT * pRecCharStruct, char * pChar, int * pConf);
    int extractChar(IMAGEL *lpImage, long angleH, long shiftX, int leftInd, int rightInd, LPBYTE lpExtractCh, int *lpWidth, int  *lpHeight);
    int  filtrMark(STRUCTWB  *lpWorkBuffer);
    void shiftSegm(int* tab, int k, int* len);
    int segmUNP(int  begHOtp, STRUCTWB * lpWorkBuffer);
    int firstSpaces(STRUCTWB *lpWorkBuffer);
    int rightM(STRUCTWB *lpWorkBuffer, int crL);
    int srIndMark(STRUCTWB *lpWorkBuffer, int crL, int crR, int x);
    int sendCh(int scal,STRUCTWB *lpWorkBuffer,int kindD);
    int coupleLett(STRUCTWB *lpWorkBuffer);
    int apostrof(STRUCTWB *lpWorkBuffer, int clean, int kindD);
    int massCh(STRUCTWB *lpWorkBuffer);
    int isMonoSpace1(STRUCTWB *lpWorkBuffer, int kindD);
    int divideFr(int scal,STRUCTWB *lpWorkBuffer, int kindD, int divCh);
    int divideMin(int scal,STRUCTWB  *lpWorkBuffer);
    int strongDiv(int scal, STRUCTWB *lpWorkBuffer, int kindD);
    int  firstIndex(STRUCTWB *lpWorkBuffer, int *lpkMin, int *lpkMax);
    int  nextIndex(STRUCTWB *lpWorkBuffer, int *lpkMin, int *lpkMax, int kindD);
    int testW(STRUCTWB *lpWorkBuffer);
    int maxYmark(int xx, int *prevI, STRUCTWB *lpWorkBuffer);
    void adjY(int *lpTr, int *lpLen, int add);
    void adjL(int *lpTr, int *lpLen, int add);
    int minYmark(int xx, int *prevI, STRUCTWB *lpWorkBuffer);
    int insideBox(STRUCTWB *lpWorkBuffer, int kindD);
    int checkI(STRUCTWB * lpWorkBuffer);
    int checkITrush(STRUCTWB  *lpWorkBuffer);
    int checkIRight(int scal, STRUCTWB  *lpWorkBuffer);
    int checkIOCR(STRUCTWB *lpWorkBuffer);
    int checkIRightOCR(int scal, STRUCTWB *lpWorkBuffer);
    int checkIOCRsm(STRUCTWB *lpWorkBuffer);
    int checkIROCRsm(int scal, STRUCTWB *lpWorkBuffer);
    int searchTrMax(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn);
    void invTraceMax(STRUCTWB *lpWorkBuffer);
    int isBigSegm(STRUCTWB *lpWorkBuffer);
    int searchTrMin(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn);
    int testOCRCp1(STRUCTWB *lpWorkBuffer);
    int testOCRCp2(int ii, STRUCTWB *lpWorkBuffer, int x1, int x2);
    int testOCRO(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1, int x2, int x_s, int y_c, int y_max);
    int cutOCRCpL(int scal, STRUCTWB *lpWorkBuffer, int kMax, int kMin, int kindD, int ii);
    int traceBtTp(int scal,STRUCTWB *lpWorkBuffer, int kMax);
    int traceTpBt(int scal, STRUCTWB *lpWorkBuffer, int kMin);
    int cutDec(int scal,STRUCTWB *lpWorkBuffer, int kMax, int kMin, int ii);
    int rightGlue(STRUCTWB *lpWorkBuffer);
    int testOCRCpL(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1);
    int tryCutTb(int scal, STRUCTWB *lpWorkBuffer, int xx, int crL, int rx);
    int tryCutBt(int scal, STRUCTWB *lpWorkBuffer, int xx, int crL, int rn);
    int highTestOCR(STRUCTWB *lpWorkBuffer, int x, int y, int what, int crL, int indFr, int ii);
    int massRight(STRUCTWB *lpWorkBuffer, int crL);
    int testLineTop(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1);
    int testLineBott(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1);
    int testLineMid(int ii, STRUCTWB *lpWorkBuffer, int dm, int gm, int x1, int ww);
    int maxminBottTop(STRUCTWB *lpWorkBuffer, int *xMn, int *xMx);
    int maxGlob(STRUCTWB *lpWorkBuffer, int gm, int range, int xMn, int xMx);
    int lastTestMin(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1);
    int minGlob(STRUCTWB *lpWorkBuffer, int dm, int range, int xMn, int xMx);
    int maxminTopBott(STRUCTWB *lpWorkBuffer, int *xMn, int *xMx);
    int lastTestMax(STRUCTWB *lpWorkBuffer, int dm, int gm, int x1);
    int searchTrMinC(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn, int rx);
    int searchTrMaxC(int scal, STRUCTWB *lpWorkBuffer, int xn, int yn, int rn);

    void secondTestForLine(STRUCTWB *lpWorkBuffer, int idF, int sizeWImg);
    void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
#endif /* UTILRECOGNITION_H_ */



