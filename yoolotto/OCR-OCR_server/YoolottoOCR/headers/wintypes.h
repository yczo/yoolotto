/*
 * wintypes.h
 *
 *  Created on: Jul 26, 2012
 *      Author: Wicek
 */

#ifndef WINTYPES_H_
#define WINTYPES_H_

#include <string.h>

#define MAXTXTLINES   160
#define MINTXTLINES    20

//#define WriteInfoOut(str,n) lprintf("%s\n", str );

#define MAXPATH 2000

#if (IOS || __APPLE__)
#include <MacTypes.h>
#define LONG SInt32
#define UINT UInt32
#define WORD UInt16
#define DWORD UInt32
#define INT32 SInt32
#define BOOL int

#else
#define LONG long
#define UINT unsigned int
#define WORD unsigned short
#define DWORD unsigned int
#define INT32 int
#define BOOL int

#endif

typedef unsigned char *LPBYTE;
//#define WORD2 unsigned int
typedef char* LPSTR;
#define BYTE unsigned char
typedef WORD* LPWORD;
typedef DWORD* LPDWORD;



typedef struct tagRECT {
   LONG left;
   LONG top;
   LONG right;
   LONG bottom;
} RECT;

typedef struct tagPOINT {
  WORD x;
  WORD y;
} POINT, *PPOINT;

typedef struct {                    /* structure for cell box extraction  */
    POINT lT;
    POINT lB;
    POINT rT;
    POINT rB;
    } STRUCT4POINTS;

typedef struct {                    /* structure for cell box extraction  */
    DWORD   x;
    DWORD  y;

    } POINTDW;

 typedef struct {
       LONG    A1;
       LONG    B1;
       LONG    C1;
       LONG    A2;
       LONG    B2;
       LONG    C2;
       } COEF;

//typedef struct tagWORD {
//    BYTE       b1;
//    BYTE       b2;
//} WORD;
//typedef WORD * LPWORD;


#define max(x,y) ( x < y ? y : x )
#define min(x,y) ( x < y ? x : y )
#define BI_RGB 0

#endif /* WINTYPES_H_ */
