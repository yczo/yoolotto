#ifndef _MEMMGR_H
#define _MEMMGR_H

#include <stdlib.h>
//#define DEBUG_ALLOC 1

#ifdef DEBUG_ALLOC

void *yl_malloc( char *tag, size_t size );
void *yl_calloc( char *tag, size_t count, size_t size);
void yl_free( void *ptr );

#else

#define yl_malloc(a,b) malloc(b)
#define yl_calloc(a,b,c) calloc(b,c)
#define yl_free(a) free(a)

#endif

#endif
