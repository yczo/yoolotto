/*
 * jpgDecoder.h
 *
 *  Created on: Dec 2, 2014
 *      Author: Wicek
 *      based on ujpeg.h
 */

#ifndef JPGDECODER_H_
#define JPGDECODER_H_


// result codes for ujDecode()
typedef enum _uj_result {
    UJ_OK           = 0,  // no error, decoding successful
    UJ_NO_CONTEXT   = 1,  // called uj* function without image handle
    UJ_NOT_DECODED  = 2,  // image has not yet been decoded
    UJ_INVALID_ARG  = 3,  // invalid argument
    UJ_IO_ERROR     = 4,  // file I/O error
    UJ_OUT_OF_MEM   = 5,  // out of memory
    UJ_NO_JPEG      = 6,  // not a JPEG file
    UJ_UNSUPPORTED  = 7,  // unsupported format
    UJ_SYNTAX_ERROR = 8,  // syntax error
    UJ_INTERNAL_ERR = 9,  // internal error
    __UJ_FINISHED         // used internally, will never be reported
} ujResult;

// plane (color component) structure
typedef struct _uj_plane {
    int width;              // visible width
    int height;             // visible height
    int stride;             // line size in bytes
    unsigned char *pixels;  // pixel data
} ujPlane;


////////////////////////////////////////////////////////////////////////////////
// C INTERFACE                                                                //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// data type for uJPEG image handles
typedef void* ujImage;

// return the error code of the last uJPEG operation
extern ujResult ujGetError(void);

// create a uJPEG image context
extern ujImage ujCreate(void);

// tell the context not to decode image data (only parse headers)
extern void ujDisableDecoding(ujImage img);

// tell the context whether which chroma upsampling mode to use
#define UJ_CHROMA_MODE_FAST      1  // low-quality pixel repetition (fast)
#define UJ_CHROMA_MODE_ACCURATE  0  // accurate bicubic upsampling (slower)
#define UJ_CHROMA_MODE_DEFAULT   0  // default mode: accurate
extern void ujSetChromaMode(ujImage img, int mode);

// decode a JPEG image from memory
// img:  the handle to the uJPEG image to decode to;
//       if it is NULL, a new instance will be created
// jpeg: a pointer to the JPEG image file in memory
// size: the size of the JPEG image file in memory
// returns the image handle on success or NULL on failure; use ujGetError to
// get a more detailed error description
extern ujImage ujDecode(ujImage img, const void* jpeg, const int size);

// decode a JPEG image from a file
// img:  the handle to the uJPEG image to decode to;
//       if it is NULL, a new instance will be created
// filename: the name of the file to load
// returns the image handle on success or NULL on failure; use ujGetError to
// get a more detailed error description
extern ujImage ujDecodeFile(ujImage img, const char* filename);

// determine whether a picture has been successfully decoded
extern int ujIsValid(ujImage img);

// determine the dimensions of a decoded picture
extern int ujGetWidth(ujImage img);
extern int ujGetHeight(ujImage img);

// determine whether a decoded picture is grayscale (0) or color (1)
extern int ujIsColor(ujImage img);

// determine the amount of memory required to hold a decoded and converted
// picture
extern int ujGetImageSize(ujImage img);

// retrieve a pointer to the internal buffer of a decoded plane
// num is the plane number: 0 = Y (luminance), 1 = Cb, 2 = Cr.
// returns a pointer or NULL in case of failure
extern ujPlane* ujGetPlane(ujImage img, int num);

// retrieve decoded and converted picture
// If called with dest == NULL, uJPEG will create an internal buffer to hold
// the decoded and converted picture and returns the pointer to this buffer.
// If called with dest != NULL, uJPEG will convert the image into a user-
// supplied buffer and return the address of that buffer.
// This function can be called with dest=NULL multiple times without performance
// penalty.
// If conversion failed, this function returns NULL; use ujGetError to get a
// more detailed error description.
extern unsigned char* ujGetImage(ujImage img, unsigned char* dest);

// destroy a uJPEG image handle
extern void ujDestroy(ujImage img);

// destroy a uJPEG image handle and make the handle variable unusable
// (preferred to ujDestroy)
#define ujFree(img) do { ujDestroy(img); img = NULL; } while (0)
typedef struct _uj_code {
    unsigned char bits, code;
} ujVLCCode;

typedef struct _uj_cmp {
    int width, height;
    int stride;
    unsigned char *pixels;
    int cid;
    int ssx, ssy;
    int qtsel;
    int actabsel, dctabsel;
    int dcpred;
} ujComponent;

typedef struct _uj_ctx {
    const unsigned char *pos;
    int valid, decoded;
    int no_decode;
    int fast_chroma;
    int size;
    int length;
    int width, height;
    int mbwidth, mbheight;
    int mbsizex, mbsizey;
    int ncomp;
    ujComponent comp[3];
    int qtused, qtavail;
    unsigned char qtab[4][64];
    ujVLCCode vlctab[4][65536];
    int buf, bufbits;
    int block[64];
    int rstinterval;
    unsigned char *rgb;
    int exif_le;
    int co_sited_chroma;
} ujContext;

#ifdef __cplusplus
}

#endif


////////////////////////////////////////////////////////////////////////////////
// C++ INTERFACE                                                              //
////////////////////////////////////////////////////////////////////////////////




#endif /* JPGDECODER_H_ */
