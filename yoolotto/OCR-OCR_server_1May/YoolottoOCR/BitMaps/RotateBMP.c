/*
 * RotateBMP.c
 *
 *  Created on: Sep 28, 2012
 *      Author: Wicek
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include  "../headers/Functions.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"

typedef union tagQBYTE {
    DWORD dw;
    struct {
        WORD w1;
        WORD w2;
    };
    struct {
        BYTE b1;
        BYTE b2;
        BYTE b3;
        BYTE b4;
    };
} QBYTE;
typedef QBYTE * LPQBYTE;

typedef union tagDBYTE {
    WORD w;
    struct {
        BYTE b1;
        BYTE b2;
    };
} DBYTE;
typedef DBYTE * LPDBYTE;

BYTE  ltab_const[256] = {
0, 128, 64, 192, 32, 160, 96, 224, 16, 144,
80, 208, 48, 176, 112, 240, 8, 136, 72, 200,
40, 168, 104, 232, 24, 152, 88, 216, 56, 184,
120, 248, 4, 132, 68, 196, 36, 164, 100, 228,
20, 148, 84, 212, 52, 180, 116, 244, 12, 140,
76, 204, 44, 172, 108, 236, 28, 156, 92, 220,
60, 188, 124, 252, 2, 130, 66, 194, 34, 162,
98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
10, 138, 74, 202, 42, 170, 106, 234, 26, 154,
90, 218, 58, 186, 122, 250, 6, 134, 70, 198,
38, 166, 102, 230, 22, 150, 86, 214, 54, 182,
118, 246, 14, 142, 78, 206, 46, 174, 110, 238,
30, 158, 94, 222, 62, 190, 126, 254, 1, 129,
65, 193, 33, 161, 97, 225, 17, 145, 81, 209,
49, 177, 113, 241, 9, 137, 73, 201, 41, 169,
105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
5, 133, 69, 197, 37, 165, 101, 229, 21, 149,
85, 213, 53, 181, 117, 245, 13, 141, 77, 205,
45, 173, 109, 237, 29, 157, 93, 221, 61, 189,
125, 253, 3, 131, 67, 195, 35, 163, 99, 227,
19, 147, 83, 211, 51, 179, 115, 243, 11, 139,
75, 203, 43, 171, 107, 235, 27, 155, 91, 219,
59, 187, 123, 251, 7, 135, 71, 199, 39, 167,
103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
15, 143, 79, 207, 47, 175, 111, 239, 31, 159,
95, 223, 63, 191, 127, 255
};

void filltab  (LPBYTE tab){
   memcpy(tab, ltab_const, 256);
}
DWORD combine1 (DWORD data, WORD addons)
{
 BYTE  dh, dl, ah, al, bh, bl, tmpB;
 WORD  dx, ax, bx, tmpW;
 DWORD tmp, tmp1;
 int   cl;
/* ----- */
   tmp  = data & 0xffff0000;
   dx   = (WORD)(tmp >> 16);
   tmp  = data & 0x0000ffff;
   ax   = (WORD)tmp;
   bx   = addons;
/* ----- */
   tmpW = dx & 0xff00;
   dh   = (BYTE)(tmpW >> 8);
   tmpW = dx & 0x00ff;
   dl   = (BYTE)tmpW;
/* ----- */
   tmpW = ax & 0xff00;
   ah   = (BYTE)(tmpW >> 8);
   tmpW = ax & 0x00ff;
   al   = (BYTE)tmpW;
/* ----- */
   tmpW = bx & 0xff00;
   bh   = (BYTE)(tmpW >> 8);
   tmpW = bx & 0x00ff;
   bl   = (BYTE)tmpW;
/* ----------------------------- */
/* ----- */
  tmpB = dh;
  dh   = dl;
  dl   = tmpB;
  dx   = (WORD)dh;
  dx <<=8;
  tmpW = (WORD)dl;
  dx   = dx | tmpW;
/* ----- */
  tmpB = ah;
  ah   = al;
  al   = tmpB;
  ax   = (WORD)ah;
  ax <<=8;
  tmpW = (WORD)al;
  ax   = ax | tmpW;
/* ----- */
    cl    = bh%32;
    tmp   = (DWORD)ax;
	tmp <<=16;
	tmp1  = (DWORD)dx;
	tmp   = tmp | tmp1;
	tmp >>=cl;
	tmpW  = dx;
	dx    = (WORD)tmp;
	cl    -=16;
	if(cl > 0) {
      tmp   = (DWORD)tmpW;
      tmp <<=16;
      tmp >>=cl;
      tmpW  = (WORD)tmp;
      dx    = dx | tmpW;
      }

    cl    = bh%32;
    tmp   = (DWORD)bx;
	tmp <<=16;
	tmp1  = (DWORD)ax;
	tmp   = tmp | tmp1;
    tmp >>=cl;
    tmpW  = ax;
	ax    = (WORD)tmp;

	cl    -=16;
	if(cl > 0) {
      tmp   = (DWORD)tmpW;
      tmp <<=16;
      tmp >>=cl;
      tmpW  = (WORD)tmp;
      ax    = ax | tmpW;
      }

	tmp   = (DWORD)dx;
	tmp <<=16;
	tmp1  = (DWORD)ax;
	tmp   = tmp | tmp1;
    return tmp;
}

LPBYTE rotate90 (LPBYTE oldPtr)
{
	LPBYTE      newPtr;
    DWORD       bfOffBits, bfSize;
    UINT        xbytes1, xbytes2, leftover, i;
    LONG        ln, delta, dwords;
    LPBITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER bi2;
    BYTE *      source;
    BYTE *      target;
    BYTE        mask;
    DWORD       tailing;
    UINT        offsets[32];
    int         w, h, bC;
    int         w2, h2;
    LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(oldPtr);


    bfOffBits = getLeftBytes((int*)bf->bfOffBits, 4);
    bi        = (LPBITMAPINFOHEADER)(oldPtr + sizeof (BITMAPFILEHEADER));
    w = getLeftBytes((int*)bi->biWidth, 4);
    h = getLeftBytes((int*)bi->biHeight, 4);
    bC = getLeftBytes((int*)bi->biBitCount, 2);
    xbytes1   = (int)((w * bC + 31) / 32) * 4;
    xbytes2   = (int)((h * bC + 31) / 32) * 4;

    bfSize    = bfOffBits + xbytes2 * w;
    newPtr    = yl_calloc("r1", bfSize, sizeof(BYTE));
    if(newPtr == NULL) {
    	printf("yl_calloc failed");
    	return NULL;
    }
    memcpy (newPtr, oldPtr, (UINT) bfOffBits);    // duplicate header
//    ((LPBITMAPFILEHEADER)newPtr)->bfSize = bfSize;  // store new size
    set4Bytes(((LPBITMAPFILEHEADER)newPtr)->bfSize, bfSize);
    bi2 = (LPBITMAPINFOHEADER)(newPtr + sizeof (BITMAPFILEHEADER));

    set4Bytes(bi2->biWidth, getLeftBytes((int*)bi->biHeight, 4));
    set4Bytes(bi2->biHeight, getLeftBytes((int*)bi->biWidth, 4));
    set4Bytes(bi2->biXPelsPerMeter, getLeftBytes((int*)bi->biXPelsPerMeter, 4));
    set4Bytes(bi2->biYPelsPerMeter, getLeftBytes((int*)bi->biYPelsPerMeter, 4));
    w2 = getLeftBytes((int*)bi2->biWidth, 4);
    h2 = getLeftBytes((int*)bi2->biHeight, 4);

    switch (bC) {
        case 1:
            target = newPtr + bfOffBits;
            dwords = (xbytes2 * h2) >> 2;
            for (ln = 0; ln < dwords; ln++) {
                *(LPDWORD)target = 0xffffffff;
                target += 4;
            }

            source   = oldPtr + bfOffBits;
            target   = newPtr + bfOffBits + xbytes2 * (h2 - 1);
            leftover = (UINT)((LONG)xbytes1 * 8 - w);
            tailing  = ~(0xffffffff << leftover);
            tailing  = (tailing >> 24) | ((tailing >>  8) & 0x0000ff00) |
                       ((tailing <<  8) & 0x00ff0000) | (tailing << 24);
            mask     = 0x80;
            delta    = xbytes2 * 32L;
            dwords   = (xbytes1 >> 2) * delta;
            for (i = 0; i < 32; i++)
                offsets[i] = xbytes2 * i;

            for (i = 0; i < h; i++) {
                if (leftover)
                    *(DWORD *)(source + xbytes1 - 4) |= tailing;
                for (ln = 0; ln < dwords; ln += delta) {
                    QBYTE q;
                    q.w1 = ~(*(WORD *)source);
                    q.w2 = ~(*(WORD *)(source + 2));
                    if (q.dw) {
                        #define SETPIXEL(bit, line) if (bit) \
                            *(target - ln - offsets[line]) ^= mask
                        if (q.w1) {
                            if (q.b1) {
                                SETPIXEL (q.b1 & 0x80,  0);
                                SETPIXEL (q.b1 & 0x40,  1);
                                SETPIXEL (q.b1 & 0x20,  2);
                                SETPIXEL (q.b1 & 0x10,  3);
                                SETPIXEL (q.b1 & 0x08,  4);
                                SETPIXEL (q.b1 & 0x04,  5);
                                SETPIXEL (q.b1 & 0x02,  6);
                                SETPIXEL (q.b1 & 0x01,  7);
                            }
                            if (q.b2) {
                                SETPIXEL (q.b2 & 0x80,  8);
                                SETPIXEL (q.b2 & 0x40,  9);
                                SETPIXEL (q.b2 & 0x20, 10);
                                SETPIXEL (q.b2 & 0x10, 11);
                                SETPIXEL (q.b2 & 0x08, 12);
                                SETPIXEL (q.b2 & 0x04, 13);
                                SETPIXEL (q.b2 & 0x02, 14);
                                SETPIXEL (q.b2 & 0x01, 15);
                            }
                        }
                        if (q.w2) {
                            if (q.b3) {
                                SETPIXEL (q.b3 & 0x80, 16);
                                SETPIXEL (q.b3 & 0x40, 17);
                                SETPIXEL (q.b3 & 0x20, 18);
                                SETPIXEL (q.b3 & 0x10, 19);
                                SETPIXEL (q.b3 & 0x08, 20);
                                SETPIXEL (q.b3 & 0x04, 21);
                                SETPIXEL (q.b3 & 0x02, 22);
                                SETPIXEL (q.b3 & 0x01, 23);
                            }
                            if (q.b4) {
                                SETPIXEL (q.b4 & 0x80, 24);
                                SETPIXEL (q.b4 & 0x40, 25);
                                SETPIXEL (q.b4 & 0x20, 26);
                                SETPIXEL (q.b4 & 0x10, 27);
                                SETPIXEL (q.b4 & 0x08, 28);
                                SETPIXEL (q.b4 & 0x04, 29);
                                SETPIXEL (q.b4 & 0x02, 30);
                                SETPIXEL (q.b4 & 0x01, 31);
                            }
                        }
                        #undef SETPIXEL
                    }
                    source += 4;
                }
                if (!(mask >>= 1)) {
                    mask = 0x80;
                    target++;
                }
            }
            break;
        default:
            free (newPtr);
            return NULL;
    }
    return newPtr;
}

LPBYTE rotate180 (LPBYTE oldPtr)
{
    BYTE        ltab[256];
    BYTE *      newPtr;
    DWORD       bfOffBits, bfSize;
    UINT        xbytes, i, j, leftover;
    long        l, dwords;
    LPBITMAPINFOHEADER bi;
    BYTE *      source;
    BYTE *      target;
    int         w, h, bC;
    LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(oldPtr);

    bfSize    = getLeftBytes((int*)bf->bfSize, 4);
    bfOffBits = getLeftBytes((int*)bf->bfOffBits, 4);
    newPtr    = yl_calloc("r2", bfSize, sizeof(BYTE));
    if(newPtr == NULL) {
    	printf("yl_calloc failed");
    	return NULL;
    }
    bi        = (LPBITMAPINFOHEADER)(oldPtr + sizeof (BITMAPFILEHEADER));
    w = getLeftBytes((int*)bi->biWidth, 4);
    h = getLeftBytes((int*)bi->biHeight, 4);
    bC = getLeftBytes((int*)bi->biBitCount, 2);
    xbytes   = (int)((w * bC + 31) / 32) * 4;

    memcpy (newPtr, oldPtr, (UINT) bfOffBits);    // duplicate header
    target = newPtr + bfOffBits;
    switch (bC) {
        case 1:
            leftover = (int)((LONG)xbytes * 8 - w);
//            TRACE ("Xbytes = %i, Width = %i, Leftover = %i", xbytes, (int)bi->biWidth, leftover);
            source = oldPtr + bfOffBits + xbytes * h;
            if (leftover >= 24) {
               source   -= 3;
               leftover -= 24;
            } else if (leftover >= 16) {
               source   -= 2;
               leftover -= 16;
            } else if (leftover >= 8) {
               source   -= 1;
               leftover -= 8;
            }
//            TRACE (" / %i\n", leftover);
            dwords = (h * xbytes) >> 2;
            if (leftover) {
                filltab (ltab);
                for (l = 0; l < dwords; l++) {
//0.65
                    DWORD w;
                    source -= 4;
                    w = combine1 (*(LPDWORD)source,
                                  *(source - 1) | (((WORD)leftover) << 8));
                    *(LPDWORD)target = ((DWORD)(ltab [(BYTE)(w >> 16)]) <<  0) |
                                       ((DWORD)(ltab [(BYTE)(w >> 24)]) <<  8) |
                                       ((DWORD)(ltab [(BYTE)(w >>  0)]) << 16) |
                                       ((DWORD)(ltab [(BYTE)(w >>  8)]) << 24);
                    target += 4;
                }
            } else {
//0.38
                filltab (ltab);
                for (l = 0; l < dwords; l++) {
                    source -= 4;
                    *(LPDWORD)target = ((DWORD)(ltab [*(source + 3)]) <<  0) |
                                       ((DWORD)(ltab [*(source + 2)]) <<  8) |
                                       ((DWORD)(ltab [*(source + 1)]) << 16) |
                                       ((DWORD)(ltab [*(source + 0)]) << 24);
                    target += 4;
                }
            }
            break;

        case 24:
            leftover = xbytes - 3 * w;
            source   = oldPtr + bfOffBits + xbytes * h - leftover;
            for (i = 0; i < h; i++) {
                for (j = 0; j < w; j++) {
                    source -= 3;
                    *(target + 0) = *(source + 0);
                    *(target + 1) = *(source + 1);
                    *(target + 2) = *(source + 2);
                    target += 3;
                }
                source -= leftover;
                target += leftover;
            }
            break;

        default:
            free (newPtr);
            return NULL;
    }
    return newPtr;
}
LPBYTE rotate270 (LPBYTE oldPtr){
    BYTE *      newPtr;
    DWORD       bfOffBits, bfSize;
    UINT        xbytes1, xbytes2, leftover, i;
    long        ln, delta, dwords;
    LPBITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER bi2;
    BYTE *      source;
    BYTE *      target;
    BYTE        mask;
    DWORD       tailing;
    UINT        offsets[32];
    int         w, h, bC;
    int         w2, h2;
    LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(oldPtr);


    bfOffBits = getLeftBytes((int*)bf->bfOffBits, 4);
    bi        = (LPBITMAPINFOHEADER)(oldPtr + sizeof (BITMAPFILEHEADER));
    w = getLeftBytes((int*)bi->biWidth, 4);
    h = getLeftBytes((int*)bi->biHeight, 4);
    bC = getLeftBytes((int*)bi->biBitCount, 2);
    xbytes1   = (int)((w * bC + 31) / 32) * 4;
    xbytes2   = (int)((h * bC + 31) / 32) * 4;

    bfSize    = bfOffBits + xbytes2 * w;

//    bfOffBits = ((LPBITMAPFILEHEADER)oldPtr)->bfOffBits;
//    bi        = (LPBITMAPINFOHEADER)(oldPtr + sizeof (BITMAPFILEHEADER));
//    xbytes1   = (int)((bi->biWidth  * bi->biBitCount + 31) / 32) * 4;
//    xbytes2   = (int)((bi->biHeight * bi->biBitCount + 31) / 32) * 4;

//    bfSize    = bfOffBits + xbytes2 * bi->biWidth;
    newPtr    = yl_calloc("r3", bfSize, sizeof(BYTE));
    if(newPtr == NULL) {
    	printf("yl_calloc failed");
    	return NULL;
    }

    memcpy (newPtr, oldPtr, (UINT) bfOffBits);    // duplicate header
    set4Bytes(((LPBITMAPFILEHEADER)newPtr)->bfSize, bfSize);
    bi2 = (LPBITMAPINFOHEADER)(newPtr + sizeof (BITMAPFILEHEADER));

    set4Bytes(bi2->biWidth, getLeftBytes((int*)bi->biHeight, 4));
    set4Bytes(bi2->biHeight, getLeftBytes((int*)bi->biWidth, 4));
    set4Bytes(bi2->biXPelsPerMeter, getLeftBytes((int*)bi->biXPelsPerMeter, 4));
    set4Bytes(bi2->biYPelsPerMeter, getLeftBytes((int*)bi->biYPelsPerMeter, 4));
    w2 = getLeftBytes((int*)bi2->biWidth, 4);
    h2 = getLeftBytes((int*)bi2->biHeight, 4);

//    ((LPBITMAPFILEHEADER)newPtr)->bfSize = bfSize;  // store new size
//    bi2 = (LPBITMAPINFOHEADER)(newPtr + sizeof (BITMAPFILEHEADER));
//    bi2->biWidth         = bi->biHeight;
//    bi2->biHeight        = bi->biWidth;
//    bi2->biXPelsPerMeter = bi->biXPelsPerMeter;
//    bi2->biYPelsPerMeter = bi->biYPelsPerMeter;

    switch (bC) {
        case 1:
            target = newPtr + bfOffBits;
            dwords = (xbytes2 * h2) >> 2;
            for (ln = 0; ln < dwords; ln++) {
                *(LPDWORD)target = 0xffffffff;
                target += 4;
            }

            source   = oldPtr + bfOffBits + xbytes1 * (h - 1);
            target   = newPtr + bfOffBits;
            leftover = (UINT)((LONG)xbytes1 * 8 - w);
            tailing  = ~(0xffffffff << leftover);
            tailing  = (tailing >> 24) | ((tailing >>  8) & 0x0000ff00) |
                       ((tailing <<  8) & 0x00ff0000) | (tailing << 24);
            mask     = 0x80;
            delta    = xbytes2 * 32L;
            dwords   = (xbytes1 >> 2) * delta;
            for (i = 0; i < 32; i++)
                offsets[i] = xbytes2 * i;

            for (i = 0; i < h; i++) {
                if (leftover)
                    *(DWORD *)(source + xbytes1 - 4) |= tailing;
                for (ln = 0; ln < dwords; ln += delta) {
                    QBYTE q;
                  //q.dw = ~(*(LPDWORD)source);
                    q.w1 = ~(*(WORD *)source);
                    q.w2 = ~(*(WORD *)(source + 2));
                    if (q.dw) {
                        #define SETPIXEL(bit, line) if (bit) \
                            *(target + ln + offsets[line]) ^= mask;
                        if (q.w1) {
                            if (q.b1) {
                                SETPIXEL (q.b1 & 0x80,  0);
                                SETPIXEL (q.b1 & 0x40,  1);
                                SETPIXEL (q.b1 & 0x20,  2);
                                SETPIXEL (q.b1 & 0x10,  3);
                                SETPIXEL (q.b1 & 0x08,  4);
                                SETPIXEL (q.b1 & 0x04,  5);
                                SETPIXEL (q.b1 & 0x02,  6);
                                SETPIXEL (q.b1 & 0x01,  7);
                            }
                            if (q.b2) {
                                SETPIXEL (q.b2 & 0x80,  8);
                                SETPIXEL (q.b2 & 0x40,  9);
                                SETPIXEL (q.b2 & 0x20, 10);
                                SETPIXEL (q.b2 & 0x10, 11);
                                SETPIXEL (q.b2 & 0x08, 12);
                                SETPIXEL (q.b2 & 0x04, 13);
                                SETPIXEL (q.b2 & 0x02, 14);
                                SETPIXEL (q.b2 & 0x01, 15);
                            }
                        }
                        if (q.w2) {
                            if (q.b3) {
                                SETPIXEL (q.b3 & 0x80, 16);
                                SETPIXEL (q.b3 & 0x40, 17);
                                SETPIXEL (q.b3 & 0x20, 18);
                                SETPIXEL (q.b3 & 0x10, 19);
                                SETPIXEL (q.b3 & 0x08, 20);
                                SETPIXEL (q.b3 & 0x04, 21);
                                SETPIXEL (q.b3 & 0x02, 22);
                                SETPIXEL (q.b3 & 0x01, 23);
                            }
                            if (q.b4) {
                                SETPIXEL (q.b4 & 0x80, 24);
                                SETPIXEL (q.b4 & 0x40, 25);
                                SETPIXEL (q.b4 & 0x20, 26);
                                SETPIXEL (q.b4 & 0x10, 27);
                                SETPIXEL (q.b4 & 0x08, 28);
                                SETPIXEL (q.b4 & 0x04, 29);
                                SETPIXEL (q.b4 & 0x02, 30);
                                SETPIXEL (q.b4 & 0x01, 31);
                            }
                        }
                        #undef SETPIXEL
                    }
                    source += 4;
                }
                source -= xbytes1 * 2;
                if (!(mask >>= 1)) {
                    mask = 0x80;
                    target++;
                }
            }
            break;
        default:
            free (newPtr);
            return NULL;
    }
    return newPtr;
}
void  reloadHeaders(LPIMGDATA img, LPBYTE ptr){


}
void rotate (LPIMGDATA img, int angle)
{
	LPBYTE newPtr = NULL;
    if (!img->ptr) return;
    switch (angle) {
        case 90:  newPtr = rotate90  (img->ptr); break;
        case 180: newPtr = rotate180 (img->ptr); break;
        case 270: newPtr = rotate270 (img->ptr); break;
    }
   if(newPtr) {
	   yl_free(img->ptr);
	   copyHeadersBMP(newPtr, img);
   }

}
