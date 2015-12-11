#include <stdlib.h>
#include "../headers/logging.h"
#include "../headers/memmgr.h"

#ifdef DEBUG_ALLOC

static int AllocCount = 0;
unsigned long BytesAllocated = 0;

void *yl_malloc( char *tag, size_t size ){
	void *ptr = malloc(size);

	if( NULL == ptr ){
		LOGE("NULL MALLOC!");
	} else {
		BytesAllocated += size;
	}

//	LOGD("%s MEMMGR malloc addr:%p (allocation %d) (%d) total %ld", tag, ptr, ++AllocCount, size, BytesAllocated);

	return ptr;
}

void *yl_calloc(char *tag, size_t count, size_t size){
	void *ptr = calloc( count, size );

	if( NULL == ptr ){
			LOGE("NULL CALLOC!");
		} else {
			BytesAllocated += size;
		}

//	LOGD("%s MEMMGR Calloc addr:%p (allocation %d) (%d, %d) total %ld", tag, ptr, ++AllocCount, count, size, BytesAllocated);

	return ptr;
}

void yl_free( void *ptr ){
//	LOGD("MEMMGR free %p (%d remaining)", ptr, --AllocCount);
	free(ptr);
}

#endif
