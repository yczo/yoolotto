#ifndef _UTIL_H
#define _UTIL_H

#include "../headers/wintypes.h"

void set4Bytes(LPBYTE out, int inp);
int trapezoidVer;
typedef struct {                    /* structure for frame, line, and  */
                                    /* specks removal functions        */

   BYTE     *lpPointer;         /* pointer to image buffer    */
   int          iLineWidth;         /* real line width in the image (in pixels) */
   int          iImgHeight;         /* height of the image                      */
   int          iXBegin;            /* X offset in bitmap (of rectangle area) in pixels */
   int          iYBegin;            /* Y offset in bitmap (of rectangle area) in pixels */
   int          iWidth;             /* width of rectangle area */
   int          iHeight;            /* height of rectangle area */

   int          iSpeckSizeH;        /* horizontal speck size (only for specks cleaning) */
   int          iSpeckSizeV;        /* vertical speck size   (only for specks cleaning) */


    } SPECKCOUNT;
   typedef struct {
       int          ccV;
       int          ccH;
       int          ccSpeck;
       int          state;
       int          order;

        } EDGEINFO;


int segmentationSwitchFlag;
int segmentationCnf;
#endif
