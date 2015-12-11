#include "../headers/util.h"

inline void set4Bytes(LPBYTE out, int inp){
	/*LPBYTE b = (LPBYTE)&inp;
	*out = *b;*(out+1) = *(b+1);
	*(out+2) = *(b+2);*(out+3) = *(b+3);
*/
	*((DWORD*)out) = (DWORD)inp;

}
