/*
 * Prototype.h
 *
 *  Created on: Aug 11, 2012
 *      Author: Wicek
 */

#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

#include "wintypes.h"
//----------------------------------------------------------------------------
//          Structure of Prototype file File:
//
//      FDFHDR
//      IDSTRUCT
//          ZONEDESCR1 + ZONEDESCR2 + ZONEDESCR3    (fdZoneNo times)
//              FLDDESCR + FLDATTR                  (zdFldNo  times)
//              ..................
//          ....................................
#define NUM_LINESTATUS      50
#define OffFdID             0
#define OffFdDpiX           2
#define OffFdDpiY           4
#define OffFdOffIDStruct    6
#define OffFdZoneNo         8
#define OffFdOffZoneDescr  10

#define OffIsSize           0
#define OffIsFormStruct     2

#define OffZdOrd            0
#define OffZdRect           2
#define OffZdSize          10
#define OffZdZoneStruct    12

#define OffZdFldNo          0
#define OffZdOffFldDescr    2

#define OffZdStepXL         0
#define OffZdStepYL         2
#define OffZdPhiMinL        4
#define OffZdPhiMaxL        6
#define OffZdStepPhiL       8
#define OffZdTgpL          10
#define OffZdMargin        12

#define OffFlOrd            0
#define OffFlRect           2
#define OffFlSize          10
#define OffFlBuffer        12

#define OffFlTitle          0
#define OffFlType          32
#define OffFlAlphabet      34
#define OffFlCellConst     36
#define OffFlCellKnown     38
#define OffFlCellWidth     40
#define OffFlHorLineRem    42
#define OffFlVerLineRem    44
#define OffFlTextLineRec   46
#define OffFlLineHeight    48
#define OffFlHorSpecSize   50
#define OffFlVerSpecSize   52
#define OffFlIgnSubSigns   54
#define OffFlIgnSubSignsPar 56
#define OffFlDictIndex     58
#define OffFlContext       60
#define OffFlConfThreshold 62
#define OffFlIsConcat      64
#define OffFlSkipIf1Blank  66
#define OffFlStopIfAnyBlank 68
#define OffFlLogZoneNo     70
#define OffFlLogFieldNo    72
#define OffFlValidation    74
#define OffFlExtractOnly   76
#define OffFlSubset        78

#define STEPXG 8
#define STEPYG 8
#define SIZEPHI 401
#define SIZEPROJ 2000

#define PI 3.14159265358
#define SCALE 65536L
#define SCALEEXP 16
#define TRIM 2      // 0 - no trimming,  1 - trimming, 2 - trimming only for
                    // black margins
#define KLEVELS 4   // number of levels in form classification
#define PROTBUFF    80000
#define TMPBUFSIZE  20000
#define FMAX 1073741824L
#define FIELDMARGIN 4
#define EXTRACTMARGIN 8
#define FORMHEIGHT 10000
#define MAXNO_LIB 20

#define MAXCHARS   2000
#define NPRT 16000
WORD  d_PRT[NPRT];
WORD  num_PRT[NPRT];
WORD  flip_PRT[NPRT];
WORD  local_PRT;
WORD * pRadonMainTeta_PRT;
WORD * pRadonDualTeta_PRT;


WORD buffer[SIZEPROJ];
WORD buffer1[SIZEPROJ];
WORD buffer2[SIZEPROJ];
BYTE * pFormLine[FORMHEIGHT];
BYTE* pTBuffer;
WORD widthT;
WORD heightT;
WORD byteWidthT;
WORD originX;
WORD originY;
WORD widthRec;
WORD heightRec;



#define dAbs1(A) ((A) > 0 ? (A) : -(A))



typedef struct tagRADON {
   LPBYTE pTicketBuff;
   int    widthTicket;
   int    heightTicket;
   int    byteWidthTicket;
   int    originX;
   int    originY;
   int    widthRec;
   int    heightRec;

   WORD* pRadonBuffer;  // word
   WORD* pRadon[SIZEPHI]; // word
   WORD* pRadonMainTeta; // word
   WORD* pRadonDualTeta; // word
   int teta;
   int tetaG;

   int sizePhi;
   int sizeD;
   int sizeDTeta;
   int stepPhi;
   int stepPhiG;
   int stepD;
   int phiMin;
   int phiMax;
   int phiMinG;
   int phiMaxG;
   long dMin;
   long dMax;
   long dMinTeta;
   long dMaxTeta;
   int local;
   int tgP;
   int byteWeight[256];
   int bitWeight[8][256];
   long cosArray[SIZEPHI];
   long sinArray[SIZEPHI];
   int stepDScale;
   int stepDHalf;
   DWORD var[SIZEPHI];
   DWORD dwbuff0[SIZEPROJ];
   DWORD dwbuff1[SIZEPROJ];
   int  a0[SIZEPROJ];
   int  a1[SIZEPROJ];
   LPBYTE pIndex[SIZEPROJ];
   LPBYTE pIndex1;
   LPBYTE pIndexi1;

   DWORD f[2][SIZEPROJ];
   DWORD* fk;
   DWORD* fk1;

} RADON;
typedef RADON* LPRADON;

typedef struct tagHLPBYTE {
    LPBYTE     ptr;
    long       size;
} HLPBYTE;
typedef HLPBYTE* LPHLPBYTE;

typedef struct tagIMAGEH {
	HLPBYTE    himgPtr;
    WORD       wWidth;
    WORD       wHeight;
    WORD       wDpiX;
    WORD       wDpiY;
} IMAGEH;
typedef IMAGEH* LPIMAGEH;


typedef struct tagRECTANGLE {
    int    wOriginX;
    int    wOriginY;
    int    wWidth;
    int    wHeight;
} RECTANGLE;
typedef RECTANGLE* LPRECTANGLE;

typedef struct tagQUADRANGLE {
	int    p1X;
	int    p1Y;
	int    p2X;
	int    p2Y;
	int    p3X;
	int    p3Y;
	int    p4X;
	int    p4Y;
} QUADRANGLE;
typedef QUADRANGLE* LPQUADRANGLE;

typedef struct tagIMAGEL{
 BYTE* lpPointer;
 int         iLineWidth;
 int         iImgHeight;
 int         iXBegin;
 int         iYBegin;
 int         iWidth;
 int         iHeight;
 int         iDpiX;
 int         iDpiY;
} IMAGEL;
typedef IMAGEL * LPIMAGEL;

typedef struct tagZONEDATA {
    LPBYTE      ptrZoneStruct;       // pointer to zone struct
    int         iAngle;             // local zone angle
    QUADRANGLE  ZoneQuad;           // zone quadrangle
} ZONEDATA;
typedef ZONEDATA * LPZONEDATA;

typedef struct tagFIELDDATA {
	char        name[32];
	DWORD       next;               // offset to next item
	DWORD       descr;              // offset to field descriptor in prt
    WORD        zone;               // number of physical zone
    WORD        flags;              // outcome of recognition
    QUADRANGLE  quad;               // field quadrangle
    LPBYTE      ptrField;          // pointer to field
} FIELDDATA;
typedef FIELDDATA* LPFIELDDATA;
typedef struct tagFIELD {
    char*   result;
    char*   cand1;
    char*   cand2;
    LPWORD  conf;
    LPQUADRANGLE quad;
} FIELD;
typedef FIELD* LPFIELD;

typedef struct tagPHI {
    int     phiMin;
    int     phiMax;
    int     stepPhi;
} PHI;
typedef PHI* LPPHI;

typedef struct tagSTEPS {
    int stepX;  // step (resolution) for main radon transformation in pixels
    int stepY;  // step (resolution) for dual radon transformation in pixels
} STEPS;
typedef STEPS* LPSTEPS;

typedef struct tagSTRINGS {
    char* lpCand1;
    char* lpCand2;
    char* lpResult;
    char* lpReject;
} STRINGS;
typedef STRINGS* LPSTRINGS;

struct FONT_LIB_DEF {
    WORD          index;
    unsigned char *lpLib;
    int           size;
	LPSTR         name;
    char          chars[128];
	int           flag;
};
#define FONT_LIB struct FONT_LIB_DEF
typedef FONT_LIB * LPFONT_LIB;
FONT_LIB  Lib_Table[MAXNO_LIB];

typedef struct tagHDR {
                char filler1[2];
                WORD number;
                char filler2[14];
                WORD index;
                char filler3[8];
                char chars[128];
                char zero;
} HDR;
typedef HDR * LPHDR;


typedef struct tagTICKETDATA {

	  LPBYTE      prototypePtr;
      PHI         phi;                // global image's phi
      int         tgp;                // distortion
      int         wPar;               // calibration
//      LPBYTE      workArea;
      HLPBYTE     workArea;
      int         returnCode;
      FONT_LIB*        LIB[MAXNO_LIB];

    int           numDescr;           // number of FDF files
	LPBYTE*       ppIDStruct0;         // address of ID-Chain
	LPBYTE        ptrLink;
	int *         ptrOut;

    int         ticketConf;          // confidence threshold for a ticket
    int         dpiMin;            // min DPI for auto-correction
    int         dpiMax;            // max DPI for auto-correction
    int         dpiOpt;            // default DPI for auto-correction
    int         wCorrectionLevel;

    int         iAngle;               // global form's angle
    WORD         wFlip;                 // actual orient.(1=180,2=270,3=90)
    int         sizeWImg;
    int         sizeHImg;

    LPBYTE      ptrFontLib;          // memory block with font libraries
    char        prototypeName[100];       // extracted prototype name
    char        prototypePath[500];       // extracted prototype psth
    char        libraryPath[500];
    char        currentImgName[100];       // extracted prototype name
    char        error[500];       // extracted prototypeName
    WORD         confidence[2];       // confidences of ticket's identification
    LPBYTE      ptrTicketStruct;       // Form Structure (Form Map)

    LPFIELDDATA fieldAddress;       // pointer of current FIELDDATA
    LPBYTE      ptrField;             // current field
    LPBYTE      fieldBMPPtr;         // field BMP
    STRINGS     strings;            // recognized strings in hgWorkArea
    LPWORD      flConf;             // confidences for each character
    LPQUADRANGLE flQuad;            // quardangles of each character
    QUADRANGLE lineQuad[NUM_LINESTATUS];
    int        lineQuadStatus[NUM_LINESTATUS];
    double     angleOut;

    int         allocLength;        // allocated length or zero
    int         usedLength;         // used length or zero
    DWORD       fieldChain;         // offset to first logical field
    int         numZones;           // number of zones
    int         numFields;          // number of fields
    ZONEDATA    zone[1];            // array of zone data
} TICKETDATA;
typedef TICKETDATA * LPTICKETDATA;
struct OUTIMGS_STRUCT {
	int width;
	int height;
	LPBYTE lpImgBuffer;
	struct OUTIMGS_STRUCT *ptrNext;
};
#define OUTIMGS struct OUTIMGS_STRUCT
typedef OUTIMGS * LPOUTIMGS;

typedef struct tagFIELDSRESULTS {
	double angle;
	char name[32];
    char result[2000];
//    char cand2[2000];
    WORD flConf[2000];
    QUADRANGLE flQuad[2000];
    QUADRANGLE lineQuad[NUM_LINESTATUS];
    int        lineQuadStatus[NUM_LINESTATUS];
    LPOUTIMGS ptrOutImages;
}FIELDSRESULTS;
typedef FIELDSRESULTS * LPFIELDSRESULTS;
#define  NUMFIELDS_RESULTS 9
typedef struct tagTICKETRESULTS {
	int status;
	int type;
	int typeCnf;
	int yShift;
    FIELDSRESULTS fieldsResults[NUMFIELDS_RESULTS];
} TICKETRESULTS;
typedef TICKETRESULTS * LPTICKETRESULTS;

//--------------------------------------------------------------------------
#define CHARSIZE 256		/* maximum dimension of a character	*/
WORD  xOriginR[2024];
WORD  yOriginR[2024];
WORD  xCornerR[2024];
WORD  yCornerR[2024];
BYTE  string[2024];
BYTE  workBuffer[60000];
BYTE extractCh[1024*5];
unsigned char * lpLibrary;
int   libSize;
WORD * xOriginFd;
WORD * yOriginFd;
WORD * xCornerFd;
WORD * yCornerFd;
int  bRecContinueFd;
LPBYTE alphabet;
WORD  widthFD,heightFD,sizeFD,deltaFD,factorFD;
WORD  mapVersionFD,normVersionFD;
BYTE * pCharLine[CHARSIZE];

#define NCHAR_G 2000
#define  MAXMARKERS     120
#define  MAXFRAMES      12
#define  MAXLINES       60
#define  MAXLTRACE      200
#define  NUMBBOX        200
#define NORMSIZE_G 96
#define CHARSIZE_G (1024+16)

LPBYTE lpLine[CHARSIZE_G];

//int hxFD[CHARSIZE_G];
//int hyFD[CHARSIZE_G];
//int hFD [CHARSIZE_G];
float * sxiFD[CHARSIZE_G];
float * syiFD[CHARSIZE_G];
//LPBYTE pMemNormFD;
LPBYTE pMemMomentsFD;

int hx[CHARSIZE];
int hy[CHARSIZE];
int h [CHARSIZE];
LPBYTE pMemNorm;
//float * sxi[CHARSIZE];
//float * syi[CHARSIZE];

typedef struct
{double m;
 double mx;
 double my;
 double mxx;
 double mxy;
 double myy;
} MOMENTS;
typedef MOMENTS * LPMOMENTS;

typedef struct
{double m;
 double cx;
 double cy;
 double mxx;
 double mxy;
 double myy;
} MOMENTSFD;
typedef MOMENTSFD * LPMOMENTSFD;

typedef struct                              /* structure for lines in the*/
                                            /* one frame */
{
    int y1[MAXLINES];                       /* top lines    in the frame */
    int y2[MAXLINES];                       /* bottom lines in the frame */
} LNFR;

typedef struct                              /* structure for the frame   */
{
    int x1;                                 /* frame left   border       */
    int x2;                                 /* frame right  border       */
    int nLines;                             /* number of lines           */
} INFFR;

/*
typedef struct {
    int        size;
    int        nNormVersion;
    int        mapVersion;
    int        confThreshold;
    int        iAlphaIndex;
    } RECCHARSTRUCT_WB;
*/
typedef struct
{int size;
 int normVersion;
 int mapVersion;
 int confThreshold;
 int iAlphaIndx;
} RECCHARSTRUCT;

    typedef struct {                            /* structure for markers     */
        int  l_m;                               /* left marker               */
        int  r_m;                               /* right marker              */
        int  g_m;                               /* upper border              */
        int  d_m;                               /* lower border              */
        } CURMARKER;

    typedef struct {                            /* structure for current line*/
        int  x1;                                /* left marker               */
        int  y1;                                /* upper border              */
        int  x2;                                /* right marker              */
        int  y2;                                /* lower border              */
        int  ym1;                               /* midium upper border       */
        int  ym2;                               /* midium lower border       */
        } CURLINE;

typedef struct                              /* structure for work area   */
{

  BYTE *lpRBuffer;                     /* address to image buffer   */
  int        widthImg;                      /* width of image            */
  int        hightImg;                      /* image hight               */
  POINT      begRect;                       /* rectangle left upper      */
                                            /* corner                    */

  int        Width;                         /* rectangle width           */
  int        Height;                        /* rectangle hight           */
  int        OffW;                          /* x offset in rectangle     */
  int        OffH;                          /* y offset in rectangle     */
  POINT      Dpi;                           /* x dpi, y dpi              */
  LNFR       line[MAXFRAMES];               /* array of tops and bottoms */
                                            /* lines in frames           */

  INFFR      infFrame[MAXFRAMES];           /* array of info for each    */
                                            /* frame, beginning, end, and*/
                                            /* nof lines                 */

  int        frNumber;                      /* nof frames                */
  CURMARKER  mark[MAXMARKERS];              /* markers in initial        */
                                            /* division                  */

  int        newLine;                       /* 1 if new line             */
  int        newSegm;                       /* 1 if new segment          */
  CURLINE    curLine[MAXFRAMES];            /* array of current line     */
  int        markNumber;                    /* current number of segments*/
  int        curMark1;                      /* index of first marker in  */
                                            /* current segment           */

  int        curMark2;                      /* index of last marker in   */
                                            /*  current segment          */
  int        traceL[MAXLTRACE];             /* left trace, trace[0]=y_top*/
  int        traceR[MAXLTRACE];             /* right trace               */
  int        traceC[MAXLTRACE];             /* current trace             */
  int        lengthTraceL;                  /* length of left trace      */
  int        lengthTraceR;                  /* length of right trace     */
  int        lengthTraceC;                  /* length of current trace   */
  int        maxMark[MAXMARKERS];           /* local maximas of low      */
                                            /* profile                   */

  int        minMark[MAXMARKERS];           /* local minimas of up       */
                                            /* profile                   */

  int        indMaxMark;                    /* number of local maximas   */
  int        indMinMark;                    /* number of local minimas   */
  int        indCurMaxMark;                 /* current index of l.maximas*/
  int        indCurMinMark;                 /* current index of l.minimas*/
  int        flgTraceR;                     /* flag right trace          */
  int        blank;                         /* counting blank places     */
  int        flgCut;                        /* flag cutting              */
  int        avLineHigh;                    /* average line high         */
  int        bottPrevLine;                  /* bottom previous line      */
  int        indTopCurLine;                 /* index of the top current  */
                                            /* line                      */

  int        monoSpace;                     /* width for mono space OCR  */
  int        OCRSpace;                      /* blank OCR space           */
  int        numbOfChar;                    /* current number of         */
                                            /* character in the field    */
//  int            boxCellWidth;              /* cell width in the field   */
  int            lettWidth;                 /* letter width   */
  int            trush;                     /* 1 if trush                */
  LONG           xxxxxxx;                   /* xxxxxxxxxxxxxxxxxxxxxxxxx */
  int            fieldType;                 /* field type                */
  int            segmType;                  /* segmentation type         */
  POINT          newLeftTopP;               /* left top frames corner    */
  POINT          newLeftBottomP;            /* left bottom frames corner */
  POINT          newRightTopP;              /* right top frames corner   */
  POINT          newRightBottomP;           /* right bottom frames corner*/
  STRUCT4POINTS  arrCellP[NUMBBOX];         /* cell boxes coeficients    */
  int            lenLeftC;                  /* lenth of cells array      */
  int            currCellIndx;              /* current cells index       */
  int            hightHline;                /* horizontal lines hight    */
  int            CellW;                     /* cell width in the field   */
int   boundLeft;
int   indxBound;
int   upBord;
int   lowBord;
int   thWid;
int   linkRM[5];
int   highGlob;
int   bottBord;
int   Method;
//int   FAR *lpFrq;
int   flgUNP;
int   indUnpB;
int   indUnpE;
int   indUnpSp;

int   flgBarC;
int   flgItalic;
int   hndOrOCR;

int   heightForVert;
int   flgSpSegm;

int   smallSignPer;
int   maxLH;
int   realWidth;
RECCHARSTRUCT *recChar;
long  angH;
long  shfH;
int   lettWidthG;
int   nOfSpConstSegm;
int   chType;
int   avArea;
int   horLineMM;
int   txtLineVer;
int   txtLineHight;
int   smallSignStrat;
int   nOffCut600;

} STRUCTWB;


typedef struct  // structure for frames and lines removal functions
{int iMinLH;    // minimal lenght of horizontal line
 int iMinLV;    // minimal lenght of vertical line
} PREPROCINFO;


typedef struct    // structure for text line recognition
{int iVer;        // 0 - hand printed version
                  // 1 - machine printed version
 int iLineHeight; // it is used only for iVer = 1
} TEXTLINE;

typedef struct   // structure for segmentation and cell boxes
{int iVer;       // for segmentation   0 - variable cell width
                 //                    1 - constant cell width
                 // for cell boxes     0 - old metodh
                 //                    1 - new method
 int iCellInfo;  // 0 - unknown or approximately known cell width
                 // 1 known cell width
 int iCellWidth; // if iCellInfo = 0 then  0 - unknown cellWidth
} SEGMENT;

typedef struct   // structure for specks removal
{int iHSize;     // horizontal size of speck (0 - off)
 int iVSize;     // vertical size of speck   (0 - off)
} SPECK;

typedef struct      //structure for line removal
{int iHorLineRem;   // 0 - off
 int iVerLineRem;   // 1 - on
} LINEREM;
typedef struct
{int iIgnSubSigns;
 int iIgnSubSignsPar;
} IGNSUBSIGNS;

typedef struct
{int iContextOn;
 int iDictIndx;
 int iCorrectionLevel;
} CONTEXT;

typedef struct
{LPBYTE lpExtractCh;
 int iWidthSegment;
 int iHeightSegment;
} EXTRACTCHAR;

typedef struct
{LPBYTE lpNormPict;
 int iSize;
 int iNormVersion;
} RECCHAR;

typedef struct
{int * xOrigin;
 int * yOrigin;
 int * xCorner;
 int * yCorner;
} COORDINATES;



typedef struct
{LPBYTE lpBuffer;
 int width;
 int height;
} PATT;
typedef PATT * LPPATT;

//--------------------------------------------------------------------------
#endif /* PROTOTYPE_H_ */
