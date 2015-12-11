/*
 * LottoPrototype.c
 *
 *  Created on: Aug 11, 2012
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
#include "../headers/memmgr.h"
#include "../headers/logging.h"


//extern int imgIndx;
//void saveLog(char *str1, char *str2);

 void ClearZones (LPTICKETDATA ticketDataPtr){
    int   i;

//    printf(" numZones= %d\n",ticketDataPtr->numZones);
//    fflush(stdout);

    if (ticketDataPtr->numZones) {
        // release field DIBs
        int offset = ticketDataPtr->fieldChain;
//    printf(" offset= %d\n", offset);
//    fflush(stdout);
        while (offset) {
            LPFIELDDATA field = (LPFIELDDATA)((LPBYTE)ticketDataPtr + offset);

// if (field->ptrField != NULL)
//       printf(" field->next= %d\n", field->next);
// else
//	  printf(" field->ptrField = NULL\n");
//  fflush(stdout);

            if (field->ptrField != NULL && field->next) {// free all but the last
//      	printf(" free field= %s\n", field->name);
                free (field->ptrField);
            }
            offset = field->next;

//            printf(" offset= %d\n", offset);
        }
        ticketDataPtr->fieldChain = 0;

        // release zone structures
        for (i = 0; i < ticketDataPtr->numZones; i++)
            if (ticketDataPtr->zone[i].ptrZoneStruct != NULL) {
//     	printf(" free zone= %d\n", i);
                yl_free (ticketDataPtr->zone[i].ptrZoneStruct);
            }
        ticketDataPtr->numZones = 0;
    }

}
int compare (BYTE* f1, BYTE* f2)
{
    BYTE* a1 = f1 + OffFlBuffer + *(WORD*)(f1 + OffFlSize);
    BYTE* a2 = f2 + OffFlBuffer + *(WORD*)(f2 + OffFlSize);

    if (*(WORD*)(a1 + OffFlLogZoneNo)  >
        *(WORD*)(a2 + OffFlLogZoneNo))  return 0;
    if (*(WORD*)(a1 + OffFlLogZoneNo)  <
        *(WORD*)(a2 + OffFlLogZoneNo))  return 1;
    if (*(WORD*)(a1 + OffFlLogFieldNo) >
        *(WORD*)(a2 + OffFlLogFieldNo)) return 0;

    return 1;
}

int SortFields (LPTICKETDATA ticketDataPtr, LPBYTE prototypePtr)
{
    LPDWORD     prevRef;    // address of word with self-offset
    LPFIELDDATA selfAddr;   // address of self item
    int        more = 1;

    if (!ticketDataPtr->fieldChain) {
        sprintf(ticketDataPtr->error," SortFields; No fields defined in prototype\n)");
        return -1;
    }

    while (more) {
        more = 0;
        prevRef  = &ticketDataPtr->fieldChain;
        selfAddr = (LPFIELDDATA)((LPBYTE)ticketDataPtr + *prevRef);

        while (selfAddr->next) {
            LPFIELDDATA nextAddr = (LPFIELDDATA)((LPBYTE)ticketDataPtr +
                             selfAddr->next); // address of next item
            if (compare ((BYTE*)prototypePtr + selfAddr->descr,
            		(BYTE*)prototypePtr + nextAddr->descr)) {
                prevRef  = &selfAddr->next;
                selfAddr = nextAddr;
            } else {
                int selfOffset = *prevRef;
                *prevRef         = selfAddr->next;
                selfAddr->next   = nextAddr->next;
                nextAddr->next   = selfOffset;
                prevRef          = &nextAddr->next;
                more             = 1;            // something changed
            }
        }
    }
    return 0;
}
int reallocMemTicketData (LPTICKETDATA* pticketDataPtr, DWORD size)
{
    int buffSize  = 1000;
    LPBYTE lpPtr;
    LPTICKETDATA ticketDataPtr = *pticketDataPtr;

    if (!ticketDataPtr->allocLength) {
    	ticketDataPtr->allocLength = sizeof(TICKETDATA);
    }

    if (ticketDataPtr->allocLength < size) {                 // see if it is necessary
    	 recalculateAllocBuffer(&size, buffSize);
//  printf(" ------------size= %u \n",size);

    	 lpPtr = yl_calloc("LP1", size, sizeof(BYTE));
   	     if(lpPtr == NULL) {
   	  	     return -1;
   	      }
      memcpy (lpPtr, ticketDataPtr, ticketDataPtr->allocLength);
      yl_free(*pticketDataPtr);
   	  *pticketDataPtr = (LPTICKETDATA)lpPtr;
   	  ticketDataPtr = *pticketDataPtr;
   	  ticketDataPtr->allocLength = size;
    }
    return 1;
}
int reallocMemoryBuffer (LPHLPBYTE hPtr, DWORD size)
{
//	LOGD("reallocMemoryBuffer %p, %d", hPtr, size);
	if( NULL == hPtr){
//		LOGD("NULL hPtr!");
		return -1;
	}

	void *ptr = NULL;

	if( NULL == hPtr->ptr ){
//		LOGD("NULL hPtr->ptr, creating");
		ptr = yl_malloc("realloc", size);
	}else {
		ptr = realloc(hPtr->ptr, size);
	}

	if( NULL == ptr ){
//		LOGD("OOM oops");
		hPtr->ptr = NULL;
		return -1;
	}

	hPtr->size = size;
	hPtr->ptr = ptr;

	return 1;

    LPBYTE lpPtr;
//    LPTICKETDATA ticketDataPtr = *pticketDataPtr;
    if ( hPtr == NULL){
//    	LOGD("---reallocMemoryBuffer, hptr == NULL !!!!!!");
    	return -1;
    }
//    LOGD("---try to reallocMemoryBuffer, hPtr->size= %ld, size= %u\n", hPtr->size, size);
    if ( hPtr->ptr == NULL || hPtr->size < size) {
//LOGD("---reallocMemoryBuffer, from  hPtr->size= %ld;  to size= %u\n", hPtr->size, size);

		lpPtr = yl_calloc("LP2", size, sizeof(BYTE));
  	        if(lpPtr == NULL) {
  	  	       return -1;
  	        }
  	      if (hPtr->ptr != NULL && hPtr->size > 0) {
  	        memcpy (lpPtr, hPtr->ptr, hPtr->size);
  	        yl_free(hPtr->ptr);
  	        hPtr->ptr = NULL;
  	      }
  	      if (hPtr->ptr == NULL) {
  	    	hPtr->ptr = lpPtr;
  	    	hPtr->size = size;
  	      }
    }
    return 1;
}
void freeh(LPHLPBYTE hPtr){
    if ( hPtr == NULL) return;
    if ( hPtr->ptr == NULL || hPtr->size == 0) return;
	yl_free(hPtr->ptr);
	hPtr->size = 0;
}

int reallocMemoryBufferTest (LPHLPBYTE hPtr, DWORD size)
{
    LPBYTE lpPtr;
//    LPTICKETDATA ticketDataPtr = *pticketDataPtr;
    if ( hPtr == NULL){
//    	LOGD("---reallocMemoryBuffer, hptr == NULL !!!!!!");
    	return -1;
    }
//LOGD("---try to reallocMemoryBuffer, hPtr->size= %ld, size= %u\n", hPtr->size, size);
   if ( hPtr->ptr == NULL || hPtr->size < size) {
//LOGD("---reallocMemoryBuffer, from  hPtr->size= %ld;  to size= %u\n", hPtr->size, size);

   	     lpPtr = yl_calloc("LP3", size, sizeof(BYTE));
  	        if(lpPtr == NULL) {
//   	LOGD("---lpPtr == NULL\n");
  	  	       return -1;
  	        }
  	      if (hPtr->ptr != NULL && hPtr->size > 0) {
//LOGD("---memcpy  hPtr->size= %ld\n", hPtr->size);
  	        memcpy (lpPtr, hPtr->ptr, hPtr->size);
  	        yl_free(hPtr->ptr);
  	      }
     return 0;
  	      if (hPtr->ptr == NULL) {
  	    	hPtr->ptr = lpPtr;
  	    	hPtr->size = size;
  	      }
    }
    return 1;

}

int ScanFDF (LPTICKETDATA* pticketDataPtr, LPBYTE prototypePtr)
{
    LPDWORD chain;
    DWORD   offset;
    DWORD   size;
    int    i;
    LPTICKETDATA ticketDataPtr = *pticketDataPtr;

//    {int k;
//    LPWORD ptrTmp = (LPWORD)prototypePtr;
//    WORD ch;
//      for(k=0;k<10;k++){
//   	   ch = *(ptrTmp+k);
//   	   printf("  k= %d, ch= %d \n",k,ch);
//      }
//    }

    ticketDataPtr->numZones  = *(LPWORD)(prototypePtr + OffFdZoneNo);

//printf("  ticketDataPtr->numZones= %d \n",ticketDataPtr->numZones);
    ticketDataPtr->numFields = 0;

//    if (memcmp ((LPSTR)(prototypePtr + 12 + 4 * ticketDataPtr->numZones),
//        "This file ", 10) != 0)
//    	prototypePtr[1] = '2';

    for (i = 0; i < ticketDataPtr->numZones; i++) {

        long zoneOff = *(LPDWORD)(prototypePtr + OffFdOffZoneDescr + i * 4);
        long fldInd  = zoneOff + OffZdZoneStruct +
                    *(LPWORD)(prototypePtr + zoneOff + OffZdSize);
        ticketDataPtr->numFields += *(LPWORD)(prototypePtr + fldInd  + OffZdFldNo);


//     printf("  zoneOff= %ld, fldInld= %ld, ticketDataPtr->numFields= %d \n",zoneOff, fldInd, ticketDataPtr->numFields);
    }

    // increase the size of FormData to accomodate all zones and fields
    size = sizeof (TICKETDATA)  - sizeof (ZONEDATA)    +  // necessary part
           sizeof (ZONEDATA)  * ticketDataPtr->numZones +  // zone info
           sizeof (FIELDDATA) * ticketDataPtr->numFields;  // field index info
//   printf("  size= %u, alloc= %d \n",size,ticketDataPtr->allocLength);

    if (reallocMemTicketData (pticketDataPtr, size)<0) return -1;
    ticketDataPtr = *pticketDataPtr;

//    printf("  new alloc= %d \n",ticketDataPtr->allocLength);



//  printf("  numZones= %d \n",*(LPWORD)(prototypePtr + OffFdZoneNo));
//  printf("  zoneOff= %u \n",*(LPDWORD)(prototypePtr + OffFdOffZoneDescr));


    // fill in field info
    offset = sizeof (TICKETDATA) - sizeof (ZONEDATA) +
    		ticketDataPtr->numZones * sizeof (ZONEDATA);

// printf("  offset= %d \n",offset);
    chain = &ticketDataPtr->fieldChain;


    for (i = 0; i < ticketDataPtr->numZones; i++) {
        UINT j;
        long zoneOff = *(LPDWORD)(prototypePtr + OffFdOffZoneDescr + i * 4);
        long fldInd  = zoneOff + OffZdZoneStruct +
                    *(LPWORD)(prototypePtr + zoneOff + OffZdSize);

//        int zoneOff = getLeftBytes((int*)(prototypePtr + OffFdOffZoneDescr + i * 4), 4);
//        int fldInd  = zoneOff + OffZdZoneStruct +
//        		getLeftBytes((int*)(prototypePtr + zoneOff + OffZdSize),2);

//        int fldNum  = getLeftBytes((int*)(prototypePtr + fldInd  + OffZdFldNo),2);
        int fldNum = *(LPWORD)(prototypePtr + fldInd  + OffZdFldNo);

//    printf("  zoneOff= %ld, fldInd= %ld, fldNum= %d \n",zoneOff, fldInd, fldNum);

        for (j = 0; j < fldNum; j++) {
            LPFIELDDATA field;
            int tmp, j1;
//            int fldOff = getLeftBytes((int*)(prototypePtr + fldInd +
//                                                OffZdOffFldDescr + j * 4),4);
            LONG fldOff = *(LPDWORD)(prototypePtr + fldInd +
                                                OffZdOffFldDescr + j * 4);
//            printf("  fldOff= %ld \n",fldOff);

//         printf("  offset= %u, ticketDataPtr= %ld, end of= %ld \n",offset, (long)ticketDataPtr,
//        		 (long)((LPBYTE)ticketDataPtr + ticketDataPtr->allocLength));


            field = (LPFIELDDATA)((LPBYTE)ticketDataPtr + offset);
//            field->next  = NULL;    // so far no link forward
            field->next  = 0;    // so far no link forward
            field->descr = fldOff;  // offset to field descriptor in FDF
            field->zone  = i;       // number of physical zone
//            field->flags = NULL;    // clear flags
            field->flags = 0;    // clear flags
            *chain = offset;        // link from prev. item to current
            chain  = &field->next;  // save address of current link fwd

//       printf("  field= %ld, field->zone= %ld \n",(long)field, (long)&field->zone);

            tmp = *(LPWORD)(prototypePtr + fldOff);
//            tmp = getLeftBytes((int*)(prototypePtr + fldOff),2);
            j1 = j+1;


            if (tmp != j1) {
//                if (MsgBoxEx (lpFormData->hwnd, MB_ICONQUESTION | MB_YESNO,
//                        MSG_FDFFLDNUM) == IDNO) return FALSE;
//                *(int*)(prototypePtr + fldOff) = j + 1;
//            	printf(" tmp %d != j1 %d \n",tmp, j1);
                *(LPWORD)(prototypePtr + fldOff) = j1;
//                set4Bytes((LPBYTE)(prototypePtr + fldOff), j1);
            }
            offset += sizeof (FIELDDATA);

        }
    }

//    {int k;
//    LPWORD ptrTmp = (LPWORD)prototypePtr;
//    WORD ch;
//      for(k=0;k<10;k++){
//   	   ch = *(ptrTmp+k);
//   	   printf(" 2 k= %d, ch= %d \n",k,ch);
//      }
//    }

//    *chain = NULL;
    *chain = 0;

    if (ticketDataPtr->numZones) {
//   printf("  SortFields\n");
        SortFields (ticketDataPtr, prototypePtr);
    }

    return 0;
}

//-----------------------------------------------------------------------------
//      Function to load ticket prototype from disk in the memory block.
//      Input:   ticket prototype structure .


int LoadPrototype (LPTICKETDATA* pticketDataPtr, int pathMode){
	FILE       *f;
	int        len;
    int       fSize;
    int       rc;
	char fileName[500];
	LPBYTE    ptr;
	LPTICKETDATA ticketDataPtr = *pticketDataPtr;

    ticketDataPtr->error[0] = 0;
    if (ticketDataPtr->prototypePtr != NULL) {
        free (ticketDataPtr->prototypePtr);
        ticketDataPtr->prototypePtr = NULL;
    }
//  printf(" ClearZones \n");

    ClearZones (ticketDataPtr);

    ticketDataPtr->fieldAddress = NULL;

    if (!ticketDataPtr->prototypeName[0]) {
    	sprintf(ticketDataPtr->error,"no prototype name \n");
        return -1;
    }
    fileName[0] = 0;
    if(pathMode)
	    strcat(fileName, ticketDataPtr->prototypePath);
	strcat(fileName, ticketDataPtr->prototypeName);

//	saveLog(" open file= ",fileName);
//	saveLog(" \n "," ");

    f= fopen(fileName, "r");
    if(f == NULL) {
    	sprintf(ticketDataPtr->error,"LoadPrototype (\"%s\") fopenfailed: %s\n",fileName, strerror(errno));
    	return -1;
    }
    fseek(f, 0L,SEEK_END);

    if(ferror(f)){
      	sprintf(ticketDataPtr->error,"LoadPrototype (\"%s\") fseek failed: %s\n",fileName, strerror(errno));
    	return -1;
    }

    len = ftell(f);
    fseek(f, 0L,SEEK_SET);

//  printf("file %s length= %d\n",fileName, len);

  ptr = yl_calloc("LP4", len, sizeof(BYTE));
  if(ptr == NULL) {
  	sprintf(ticketDataPtr->error,"LoadPrototype (\"%s\") yl_calloc failed: %s\n",fileName, strerror(errno));
  	return -1;
  }
  fSize = fread((char *)ptr, len, 1, f);
  if(ferror(f) || fSize*len != len){
      if(ferror(f))
        	sprintf(ticketDataPtr->error,"LoadPrototype (\"%s\") read failed: %s\n",fileName, strerror(errno));
      else
          sprintf(ticketDataPtr->error,"LoadPrototype (\"%s\") read failed: (size= %d) != (len= %d\n)",fileName, fSize*len, len);
  	return -1;
  }
  close_file(f);
  ticketDataPtr->prototypePtr = ptr;

// printf("ScanFDF \n");
    rc = ScanFDF (pticketDataPtr, ticketDataPtr->prototypePtr);
    return rc;
}



void createCosSinTable(LPRADON radonStruct)

{int i;
 double radPhiMin = radonStruct->phiMin*PI/1800;
 double radStepPhi = radonStruct->stepPhi*PI/1800;

// printf("radonStruct->phiMin = %d, radonStruct->stepPhi= %d  \n",radonStruct->phiMin, radonStruct->stepPhi);

// printf("radPhiMin = %e, radStepPhi= %e  \n",radPhiMin, radStepPhi);

// printf("radonStruct->sizePhi = %d\n",radonStruct->sizePhi);


 for(i=0; i<radonStruct->sizePhi; i++)
    {radonStruct->cosArray[i] = (long)((cos(radPhiMin + i*radStepPhi))*SCALE);
    radonStruct->sinArray[i] = (long)((sin(radPhiMin + i*radStepPhi))*SCALE);
    }
}

void byteWeights(LPRADON radonStruct)

{int i;
 int k;
 int m;
 int weight;


 for(i=0; i<256; i++)
    {m=i;
     weight = 0;
     for(k=0; k<8; k++)
        {weight = weight + 1 - m%2;
         m = m/2;
        }
     radonStruct->byteWeight[i] = weight;
//  printf("  i = %d, weight= %d\n",i, weight);
    }
}



int radT(LPRADON radonStruct){

 int i,j,k,m;
 int byteWidthTicket;
 int jmax,imin,imax;
 long acosArray[SIZEPHI];
 long bsinArray[SIZEPHI];
 int weight;
 int nd;
 WORD* pRadon[SIZEPHI];
 LPBYTE  p;
 LPBYTE  p1;
 double stepDHalf;
 int    scaleexp = 16;

 byteWeights(radonStruct);

 byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;
 stepDHalf = ((double)radonStruct->stepD - 1.0)/2;
 jmax = radonStruct->heightRec/radonStruct->stepD - 1;
 imin = radonStruct->originX/8;
 imax = imin + radonStruct->widthRec/8 - 1;

// printf("  byteWidthTicket= %d, stepDHalf= %e \n",byteWidthTicket, stepDHalf);
// printf("  jmax= %d, imin= %d, imax= %d\n",jmax, imin, imax);

// printf("  radonStruct->sizePhi= %d\n",radonStruct->sizePhi);


 for(m=0; m<radonStruct->sizePhi; m++)
    {acosArray[m] =
    		(long)(((stepDHalf+radonStruct->originY)*radonStruct->cosArray[m] -
                             3.5*radonStruct->sinArray[m] - radonStruct->dMin)/radonStruct->stepD);
     bsinArray[m] = 8*radonStruct->sinArray[m]/(long)radonStruct->stepD;

     pRadon[m] = radonStruct->pRadonBuffer + ((DWORD)m)*((DWORD)radonStruct->sizeD);

     for(k=0; k<radonStruct->sizeD; k++) *((pRadon[m])+k) = 0;
    }


 for(j=0; (int)j<(int)jmax; j++)
    {p = radonStruct->pTicketBuff +
         (radonStruct->heightTicket - 1 - j*radonStruct->stepD - radonStruct->originY)*byteWidthTicket;
     for(i=imin; i<=imax; i++)
        {weight = 0;
         p1 = p;
         for(k=0; k<radonStruct->stepD; k++)
            {weight += radonStruct->byteWeight[(*p1)];
             p1 -= byteWidthTicket;
            }
         if(weight != 0)
            {for(m=0; m<radonStruct->sizePhi; m++)
                {nd = (int)(((acosArray[m] - i*bsinArray[m]))>>scaleexp);
                 *((pRadon[m]) + nd) += weight;
                }
            }
         p++;
        }
     for(m=0; m<radonStruct->sizePhi; m++) acosArray[m] += radonStruct->cosArray[m];
    }


  for(m=0; m<radonStruct->sizePhi; m++)
	  radonStruct->pRadon[m] = radonStruct->pRadonBuffer + (m)*(radonStruct->sizeD);


 return(0);
}


int radTL(LPRADON radonStruct){

 int i,j,k,m;
 int byteWidthTicket;
 int imin,imax;
 int acosArray[SIZEPHI];
 int a;
 BYTE byte;
 int x;
 int nd;
 WORD* pRadon[SIZEPHI];
 LPBYTE p;
 int    scaleexp = 16;

 for(m=0; m<radonStruct->sizePhi; m++)
    {acosArray[m] = radonStruct->originY*radonStruct->cosArray[m] - radonStruct->dMin;
     pRadon[m] = radonStruct->pRadonBuffer + (m)*(radonStruct->sizeD);
     for(k=0; k<radonStruct->sizeD; k++) *((pRadon[m])+k) = 0;
    }

 byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;
 imin = radonStruct->originX/8;
 imax = imin + radonStruct->widthRec/8 - 1;

 for(j=0; j<radonStruct->heightRec; j++)
    {p = radonStruct->pTicketBuff +
         (radonStruct->heightTicket - 1 - j - radonStruct->originY)*byteWidthTicket;
     for(i=imin; i<=imax; i++)
        {if((byte = *(p+(long)i)) != 255)
           {for(m=0; m<radonStruct->sizePhi; m++)
               {x = i<<3;
                a = acosArray[m] - x*radonStruct->sinArray[m];
                for(k=0; k<8; k++)
                   {if(!((byte<<((x++)%8))&128))
                    {nd = (a/radonStruct->stepD)>>scaleexp;
                     (*((pRadon[m]) + nd))++;
                    }
                    a -= radonStruct->sinArray[m];
                   }
               }
           }
        }
     for(m=0; m<radonStruct->sizePhi; m++) acosArray[m] += radonStruct->cosArray[m];
    }

 for(m=0; m<radonStruct->sizePhi; m++)
	 radonStruct->pRadon[m] = radonStruct->pRadonBuffer + (m)*(radonStruct->sizeD);


 return(0);
}




/*****************************************************************
*                                                                *
*  skewAngle                                                     *
*                                                                *
*  return value:     -1 - skew angle is out of range             *
*                                                                *
*****************************************************************/

int skewAngle(LPRADON radonStruct){

 int i,j;
 int nTeta = 0;
 int norm;
 int max;
 long integral = 0;
 int dif;
 long sum;
 int t;

 WORD* p1;
 WORD* p2;

 for(i=0; i<radonStruct->sizePhi; i++)
   {t = i*radonStruct->stepPhi + radonStruct->phiMin;
     if(t > 0)
       norm = (int)((radonStruct->heightRec*radonStruct->cosArray[i] +
    		   radonStruct->widthRec*radonStruct->sinArray[i])/SCALE);
     else
       norm = (int)((radonStruct->heightRec*radonStruct->cosArray[i] -
    		   radonStruct->widthRec*radonStruct->sinArray[i])/SCALE);

//printf("  norm= %d \n", norm);
     p1 = radonStruct->pRadon[i];
     p2 = p1 + 1;
     sum = 0;
     for(j=1; j<radonStruct->sizeD; j++)
        {dif = ((*p2++) -(*p1++));
         if(dif > 0) sum += (long)dif;
                else sum -= (long)dif;
         }

     if(norm != 0) radonStruct->var[i] = (DWORD)(sum*100L)/norm;
              else radonStruct->var[i] = 0;

//     printf("  sum= %ld, radonStruct->var[i]= %d \n", sum, radonStruct->var[i]);
    }

 max = 0;
 for(i=0; i<radonStruct->sizePhi; i++)
 {if(radonStruct->var[i] >= max) {max = radonStruct->var[i]; nTeta = i;}
  integral += (long)radonStruct->var[i];

// printf("  max= %d, integral= %ld \n", max, integral);

 }


 integral = integral/radonStruct->sizePhi;

 radonStruct->teta = (int)nTeta*(int)radonStruct->stepPhi + radonStruct->phiMin;


 if(((0.9*max) < integral)||(nTeta < 2)||(nTeta > radonStruct->sizePhi-3))
        return(-1);

 return(0);

}


/*************************************************************************
*                                                                        *
*    radonMainG                                                          *
*                                                                        *
*    return value:     -1 - skew angle is out of range                   *
*                                                                        *
*************************************************************************/

int radonMainG(LPRADON radonStruct)

{int i;
int err = 0;
int retVal;
int nTeta;
int shift;
WORD* p;
int aPhiMin;
int aPhiMax;
int aPhiM;
long cosM;
long sinM;

//char buff[500];

byteWeights(radonStruct);


radonStruct->sizePhi = ((radonStruct->phiMax - radonStruct->phiMin)/radonStruct->stepPhi + 1);


//printf("  radonStruct->sizePhi= %d \n", radonStruct->sizePhi);

createCosSinTable(radonStruct);

//printf("  byteWeights \n");
//return 0;

radonStruct->stepDScale = radonStruct->stepD*SCALE;

//printf("  radonStruct->stepDScale= %d \n", radonStruct->stepDScale);

if(radonStruct->phiMin < 0) aPhiMin = -radonStruct->phiMin; else aPhiMin = radonStruct->phiMin;
if(radonStruct->phiMax < 0) aPhiMax = -radonStruct->phiMax; else aPhiMax = radonStruct->phiMax;
if(aPhiMax > aPhiMin) aPhiM = aPhiMax; else aPhiM = aPhiMin;

cosM = (long)((cos(aPhiM*PI/1800))*SCALE);
sinM = (long)((sin(aPhiM*PI/1800))*SCALE);

//printf("cosM= %ld, sinM= %ld\n",cosM, sinM );
//saveLog("", buff);

radonStruct->dMin = (long)radonStruct->originY*cosM -
		(long)(radonStruct->originX+radonStruct->widthRec)*sinM;
radonStruct->dMax = (long)(radonStruct->originY+radonStruct->heightRec)*cosM +
		(long)(radonStruct->originX+radonStruct->widthRec)*sinM;

radonStruct->sizeD = (int)((radonStruct->dMax - radonStruct->dMin)/radonStruct->stepDScale + 1);
radonStruct->stepDHalf = ((double)radonStruct->stepD + 1.0)/2;



//printf("  radonStruct->dMin= %ld, radonStruct->dMax= %ld \n",radonStruct->dMin, radonStruct->dMax);
//printf("  radonStruct->sizeD= %d, radonStruct->stepDHalf= %d \n",radonStruct->sizeD, radonStruct->stepDHalf);


if(radonStruct->local == 0)
      {err = radT(radonStruct);
       }
if(radonStruct->local == 1)
       {err = radTL(radonStruct);
       }
if(err < 0) return(err);


//{int mm, kk;
//for(mm=0; mm<radonStruct->sizePhi; mm++) {
//    for(kk=0; kk<radonStruct->sizeD; kk++){
//  	printf("  mm= %d, kk= %d, value= %d \n",mm, kk, *((radonStruct->pRadon[mm])+kk));
//    }
//   }
//}



retVal = skewAngle(radonStruct);
//sprintf(buff,"local= %d retVal= %d\n",radonStruct->local, retVal);
//saveLog("skewAngle;  ", buff);

//printf("  retVal= %d, radonStruct->local= %d, radonStruct->teta= %d \n",retVal, radonStruct->local, radonStruct->teta);

if(radonStruct->local == 1) retVal = 0;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ignore too big angle???
//if(retVal < 0) return(retVal);
retVal = 0;


nTeta = (int)((radonStruct->teta - radonStruct->phiMin)/radonStruct->stepPhi);

//printf("  nTeta= %d \n",nTeta);

if(radonStruct->teta > 0)
  {radonStruct->dMinTeta = (long)radonStruct->originY*radonStruct->cosArray[nTeta] -
	                       (long)(radonStruct->originX+radonStruct->widthRec)*radonStruct->sinArray[nTeta];
  radonStruct->dMaxTeta =  (long)(radonStruct->originY+radonStruct->heightRec)*radonStruct->cosArray[nTeta] -
		                   (long)radonStruct->originX*radonStruct->sinArray[nTeta];
  }
else
 {radonStruct->dMinTeta = (long)radonStruct->originY*radonStruct->cosArray[nTeta] -
	                      (long)radonStruct->originX*radonStruct->sinArray[nTeta];
 radonStruct->dMaxTeta =  (long)(radonStruct->originY+radonStruct->heightRec)*radonStruct->cosArray[nTeta] -
		                  (long)(radonStruct->originX+radonStruct->widthRec)*radonStruct->sinArray[nTeta];
 }

//printf("  radonStruct->dMinTeta= %ld, radonStruct->dMaxTeta= %ld \n",radonStruct->dMinTeta, radonStruct->dMaxTeta);

shift =  (radonStruct->dMinTeta - radonStruct->dMin)/radonStruct->stepDScale;
radonStruct->sizeDTeta = (int)((radonStruct->dMaxTeta - radonStruct->dMinTeta)/radonStruct->stepDScale + 1);

//printf("  shift= %d, radonStruct->sizeDTeta= %d \n",shift, radonStruct->sizeDTeta);

p = radonStruct->pRadonBuffer + (DWORD)nTeta*(DWORD)radonStruct->sizeD + shift;
for(i=0; i<radonStruct->sizeDTeta; i++) *(radonStruct->pRadonMainTeta+(DWORD)i) = *p++;

return(retVal);
}


int radMT(LPRADON radonStruct){
/*
(BYTE huge * pFormBuffer,
 WORD widthForm,
 WORD heightForm,
 WORD originX,
 WORD originY,
 WORD widthRec,
 WORD heightRec,
 long cosTeta,
 long sinTeta,
 long dMinTeta,
 WORD sizeDTeta,
 WORD stepD,
 WORD far * pRadonMainTeta
)
*/
 int i,j,k,m;
 int byteWidthForm;
 int jmax,imin,imax;
 int acos,bsin;
 int weight;
 int nd;
 LPBYTE p;
 LPBYTE p1;
 double stepDHalf;
 int scaleexp = 16;

 int cosTeta;
 int sinTeta;

  cosTeta = (int)((cos(radonStruct->teta*PI/1800))*SCALE);
  sinTeta = (int)((sin(radonStruct->teta*PI/1800))*SCALE);

  if(radonStruct->teta > 0)
    {radonStruct->dMinTeta = radonStruct->originY*cosTeta -
                (radonStruct->originX+radonStruct->widthRec)*sinTeta;
    radonStruct->dMaxTeta = (radonStruct->originY+radonStruct->heightRec)*cosTeta -
    		radonStruct->originX*sinTeta;
    }
  else
    {radonStruct->dMinTeta = radonStruct->originY*cosTeta -
	  radonStruct->originX*sinTeta;
    radonStruct->dMaxTeta = (radonStruct->originY+radonStruct->heightRec)*cosTeta -
                (radonStruct->originX+radonStruct->widthRec)*sinTeta;
    }

  radonStruct->sizeDTeta = ((((radonStruct->dMaxTeta - radonStruct->dMinTeta)/radonStruct->stepD)>>SCALEEXP) + 1);


 byteWeights(radonStruct);

 for(k=0; k<radonStruct->sizeDTeta; k++) *(radonStruct->pRadonMainTeta+k) = 0;

 byteWidthForm = ((radonStruct->widthTicket + 31)/32)*4;
 stepDHalf = ((double)radonStruct->stepD - 1.0)/2;
 jmax = radonStruct->heightRec/radonStruct->stepD - 1;
 imin = radonStruct->originX/8;
 imax = imin + radonStruct->widthRec/8 - 1;
 acos = (int)(((radonStruct->stepDHalf+radonStruct->originY)*cosTeta - 3.5*sinTeta - radonStruct->dMinTeta)/radonStruct->stepD);
 bsin = 8L*sinTeta/radonStruct->stepD;

 for(j=0; (int)j<(int)jmax; j++)
    {p = radonStruct->pTicketBuff +
         (radonStruct->heightTicket - 1 - j*radonStruct->stepD  - radonStruct->originY)*byteWidthForm;
     for(i=imin; i<=imax; i++)
        {weight = 0;
         p1 = p;
         for(m=0; m<radonStruct->stepD; m++)
            {weight += radonStruct->byteWeight[(*p1)];
             p1 -= byteWidthForm;
            }
         if(weight != 0)
            {nd = ((acos - i*bsin))>>scaleexp;
             *(radonStruct->pRadonMainTeta + nd) += weight;
            }
         p++;
        }
     acos += cosTeta;
    }

 return(0);
}

//#define labs(a) ((a) > 0 ? (a) : -(a))



int recMargin(int* p,  int n, int* pStart,  int * pEnd)

{int i;
// int t, t2;
 long tv;
// int s;
 long sum;
 int mean;
 int mean1;
 long meanv;
 long a, b;
 int k;
 int v[SIZEPROJ];
 int v1[SIZEPROJ];

// LOGD("recMargin %p,%d,%p, %p\n", p, n, pStart, pEnd);

 if((n < 7)||(TRIM==0)) {*pStart = 0; *pEnd = n-1; return(0);}

// printf("  recMargin p[0]= %d, p[1]= %d\n", p[0], p[1]);

 buffer1[0] = (int)((p[0] + p[1])/2);
 for(i=1; i<n-1; i++) buffer1[i] = (int)((p[i-1] + p[i] + p[i+1])/3);
 buffer1[n-1] = (int)((p[n-2] + p[n-1])/2);

 sum = 0L;
 for(i=0; i<n; i++)  sum += buffer1[i];
 mean = (int)(sum/n);

 for(i=0; i<n; i++)  v1[i] = (int)buffer1[i] - mean;

 sum = 0L;
 for(i=0; i<n; i++)  sum += (v1[i] > 0 ? v1[i] : 0L);
 mean1 = (int)(sum/n);

 sum = 0;
 for(i=1; i<n-1; i++)
    {
     v[i] = labs(buffer1[i+1] - 2L*buffer1[i] + buffer1[i-1]);
     sum += v[i];
    }
 meanv = sum/n;

 if(meanv) for(i=1; i<n-1; i++) v[i] = (100L*v[i])/meanv;
 else for(i=1; i<n-1; i++) v[i] = 0;

//   t = 150; t2 = t/3;
//     t = 300; t2 = t/3;
     tv = 350L;

 for(i=2; i<n-5; i++)
    {if(v[i] > tv) {
        if(((int)buffer1[i+1] - mean < mean1)||
           ((int)buffer1[i+2] - mean < mean1)||
           ((int)buffer1[i+3] - mean < mean1)) {
           if(2L*buffer1[i] >
                 buffer1[i+1]+buffer1[i+2] + 2*mean1) {
              if(buffer1[i-2]+buffer1[i-1] >
                 buffer1[i+1]+buffer1[i+2] + 2*mean1)
                 break;
           }
        }
     }
    }
 *pStart = i;
  k = ((*pStart)-1)/3;
  if(k<2) k = 2;
  a = v1[k];
  b = v1[(*pStart)-1]; if(b < mean1) *pStart = 0;
  for(i=k; i<*pStart; i++) {
       if((v1[i] + mean1/2)*(*pStart-3) < (*pStart-1-i)*a + (i-2)*b)
          {*pStart = 0; break;}
       if((v1[i] - mean1/2)*(*pStart-3) > (*pStart-1-i)*a + (i-2)*b)
          {*pStart = 0; break;}
  }

 for(i=n-3; i>4; i--)
    {if(v[i] > tv) {
         if((buffer1[i-1] - mean < mean1)||
            (buffer1[i-2] - mean < mean1)||
            (buffer1[i-3] - mean < mean1)){
             if(buffer1[i-2]+buffer1[i-1] <
                 2*buffer1[i] - 2*mean1) {
                 if(buffer1[i-2]+buffer1[i-1] <
                    buffer1[i+1]+buffer1[i+2] - 2*mean1)
                    break;
             }
         }
    }
    }
 *pEnd = i;
  a = v1[(*pEnd)+1]; if(a < mean1) *pEnd = n-1;
  k = n-1 - (n-1 - (*pEnd)-1)/3;
  if(k>n-3) k = n-3;
  b = v1[k];
  for(i=k; i>*pEnd; i--) {
       if((v1[i] + mean1/2)*(n-4-(*pEnd)) < (n-3-i)*a + (i-(*pEnd)-1)*b)
          {*pEnd = n-1; break;}
       if((v1[i] - mean1/2)*(n-4-(*pEnd)) > (n-3-i)*a + (i-(*pEnd)-1)*b)
          {*pEnd = n-1; break;}
  }

 if((*pStart == 1)||(*pStart == 2)) *pStart = 0;
 if((*pEnd == n-2)||(*pEnd == n-3)) *pEnd = n-1;
 if(*pStart > n/2) *pStart = 0;
 if(*pEnd < n/2) *pEnd = n-1;
 if(*pStart > *pEnd) { *pStart = 0; *pEnd = n-1;}

 return(0);

}

int radDT(LPRADON radonStruct){
/*
(BYTE huge * pFormBuffer,
 WORD widthForm,
 WORD heightForm,
 WORD originX,
 WORD originY,
 WORD widthRec,
 WORD heightRec,
 long cosTeta,
 long sinTeta,
 long dMinTeta,
 WORD sizeDTeta,
 WORD stepD,
 WORD far * pRadonDualTeta
)
*/

 int i,j,k,m;
 int byteWidthTicket;
 int jmax,imin,imax;
 int asin,bcos;
 int weight;
 int nd;
 LPBYTE p;
 LPBYTE p1;
 double stepDHalf;
 int scaleexp = 16;

 long cosTeta;
 long sinTeta;

  cosTeta = (long)((cos(radonStruct->teta*PI/1800))*SCALE);
  sinTeta = (long)((sin(radonStruct->teta*PI/1800))*SCALE);

  if(radonStruct->teta > 0)
    {radonStruct->dMinTeta = (long)radonStruct->originY*sinTeta +
	                         (long)radonStruct->originX*cosTeta;
    radonStruct->dMaxTeta = (long)(radonStruct->originY+radonStruct->heightRec)*sinTeta +
    		                (long)(radonStruct->originX+radonStruct->widthRec)*cosTeta;
    }
  else
    {radonStruct->dMinTeta = (long)(radonStruct->originY+radonStruct->heightRec)*sinTeta +
	                         (long)radonStruct->originX*cosTeta;
    radonStruct->dMaxTeta = (long)radonStruct->originY*sinTeta +
    		                (long)(radonStruct->originX+radonStruct->widthRec)*cosTeta;
    }

  radonStruct->sizeDTeta = (int)((((radonStruct->dMaxTeta - radonStruct->dMinTeta)/radonStruct->stepD)>>SCALEEXP) + 1L);

 byteWeights(radonStruct);


 for(k=0; k<radonStruct->sizeDTeta; k++) *(radonStruct->pRadonDualTeta+k) = 0;

 byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;
 stepDHalf = ((double)radonStruct->stepD - 1.0)/2;
 jmax = radonStruct->heightRec/radonStruct->stepD - 1;
 imin = radonStruct->originX/8;
 imax = imin + radonStruct->widthRec/8 - 1;
 asin = (((stepDHalf+radonStruct->originY)*sinTeta + 3.5*cosTeta - radonStruct->dMinTeta)/radonStruct->stepD);
 bcos = 8L*cosTeta/radonStruct->stepD;

 for(j=0; (int)j<(int)jmax; j++)
    {p = radonStruct->pTicketBuff +
           (radonStruct->heightTicket - 1 - j*radonStruct->stepD  - radonStruct->originY)*byteWidthTicket;
     for(i=imin; i<=imax; i++)
        {weight = 0;
         p1 = p;
         for(m=0; m<radonStruct->stepD; m++)
            {weight += radonStruct->byteWeight[(*p1)];
             p1 -= byteWidthTicket;
            }
         if(weight != 0)
            {nd = ((asin + i*bcos))>>scaleexp;
             *(radonStruct->pRadonDualTeta + nd) += weight;
            }
         p++;
        }
     asin += sinTeta;
    }

 return(0);
}

int radDTL(LPRADON radonStruct){
	 int i,j,k,m;
	 int byteWidthTicket;
	 int imin,imax;
	 int asin, a;
	 BYTE byte;
	 WORD x;
	 DWORD nd;
	 LPBYTE p;
//	 LPBYTE p1;
//	 double stepDHalf;
	 int scaleexp = 16;

	 long cosTeta;
	 long sinTeta;

	  cosTeta = (long)((cos(radonStruct->teta*PI/1800))*SCALE);
	  sinTeta = (long)((sin(radonStruct->teta*PI/1800))*SCALE);

	  if(radonStruct->teta > 0)
	    {radonStruct->dMinTeta = (long)radonStruct->originY*sinTeta +
		                         (long)radonStruct->originX*cosTeta;
	    radonStruct->dMaxTeta = (long)(radonStruct->originY+radonStruct->heightRec)*sinTeta +
	    		                (long)(radonStruct->originX+radonStruct->widthRec)*cosTeta;
	    }
	  else
	    {radonStruct->dMinTeta = (long)(radonStruct->originY+radonStruct->heightRec)*sinTeta +
		                         (long)radonStruct->originX*cosTeta;
	    radonStruct->dMaxTeta = (long)radonStruct->originY*sinTeta +
	    		                (long)(radonStruct->originX+radonStruct->widthRec)*cosTeta;
	    }

	  radonStruct->sizeDTeta = (int)((((radonStruct->dMaxTeta - radonStruct->dMinTeta)/radonStruct->stepD)>>SCALEEXP) + 1);


 for(k=0; k<radonStruct->sizeDTeta; k++) *(radonStruct->pRadonDualTeta+k) = 0;

 byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;
 imin = radonStruct->originX/8;
 imax = imin + radonStruct->widthRec/8 - 1;
 asin = radonStruct->originY*sinTeta - radonStruct->dMinTeta;

 for(j=0; j<heightRec; j++)
    {p = radonStruct->pTicketBuff +
         (DWORD)(radonStruct->heightTicket - 1 - j - radonStruct->originY)*(DWORD)byteWidthTicket;
     for(i=imin; i<=imax; i++)
        {if((byte = *(p+(long)i)) != 255)
           {x = i<<3;
            a = asin + x*cosTeta;
            for(m=0; m<8; m++)
               {if(!((byte<<((x++)%8))&128))
                   {nd = ((DWORD)a/radonStruct->stepD)>>scaleexp;
                   (*(radonStruct->pRadonDualTeta + nd))++;
                   }
                a += cosTeta;
               }
           }
        }
     asin += sinTeta;
    }
 return(0);
}


void recalculateAllocBuffer(LPDWORD size, int modulo){
		*size = (*size/modulo + 1)*modulo;
}
//    ticketRecognitionInit

int ticketRecognitionInit(LPIMGDATA img, LPRADON radonStruct, RECTANGLE* area, int IDVersion, HLPBYTE* workAreaOutPtr, LPBYTE* iDStructOutPtr,
int * pAngle, LPPHI  pPhi) {
  LPBYTE workAreaPtr;
  LPBYTE iDStructPtr;
  WORD* wiDStructPtr;


  WORD* pRadonMainTeta1;
  WORD* pRadonDualTeta1;
  int  sizeDTetaX;
  int  sizeDTetaY;
  long  dMinTetaX;
  long  dMinTetaY;
  int  step;

  int sum;

  int i,j,k;
  int retVal;
  int err;
  DWORD wh;
  DWORD sizeWorkAreaAlloc;
  DWORD sizeIDStructAlloc;
  DWORD sizeIDStruct;


  int shift;

  int x1Teta, x2Teta, y1Teta, y2Teta;
  int* px1Teta = &x1Teta;
  int* px2Teta = &x2Teta;
  int* py1Teta = &y1Teta;
  int* py2Teta = &y2Teta;

  double xTeta0, yTeta0;
//  RADON radonStruct;

  radonStruct->stepPhi = pPhi -> stepPhi;

  radonStruct->phiMin = (int)ceil(pPhi -> phiMin);
  radonStruct->phiMax = (int)floor(pPhi -> phiMax);


  radonStruct->phiMinG  = radonStruct->phiMin;
  radonStruct->phiMaxG  = radonStruct->phiMax;
  radonStruct->stepPhiG = radonStruct->stepPhi;

  radonStruct->pTicketBuff   = img->pixPtr;
  radonStruct->widthTicket     = img->bi.biWidth;
  radonStruct->heightTicket    = img->bi.biHeight;
  radonStruct->originX       = ((area -> wOriginX)/8)*8;
  radonStruct->originY       = area -> wOriginY;
  radonStruct->widthRec      = (((area -> wOriginX) + (area -> wWidth))/8)*8 - radonStruct-> originX;
  radonStruct->heightRec     = area -> wHeight;

  radonStruct->byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;

if(radonStruct->widthTicket > radonStruct->heightTicket) wh = (DWORD)radonStruct->widthTicket;
                      else wh = (DWORD)radonStruct->heightTicket;

sizeWorkAreaAlloc = 3*(2*wh*wh/STEPXG)/STEPYG + 101;
recalculateAllocBuffer(&sizeWorkAreaAlloc, 300000);

//printf("  sizeWorkAreaAlloc= %d \n",sizeWorkAreaAlloc);

//workAreaPtr = yl_calloc("LP5", sizeWorkAreaAlloc, sizeof(BYTE));
workAreaPtr = workAreaOutPtr->ptr;
if(workAreaPtr == NULL) {
	sprintf(img->error,"ticketRecognitionInit (\"%s\") yl_calloc failed: %s\n",img->szIMGName, strerror(errno));
	return -1;
}
if(workAreaOutPtr->size <sizeWorkAreaAlloc) {
	sprintf(img->error,"ticketRecognitionInit (\"%s\") too small workArea \n",img->szIMGName);
	return -1;
}


workAreaPtr = workAreaOutPtr->ptr;
workAreaPtr = workAreaPtr + 8;  // ?????

sizeIDStructAlloc =
          (24 + 2*(radonStruct->widthTicket/STEPYG + radonStruct->heightTicket/STEPXG + 2) + 100);

sizeIDStructAlloc = 2*sizeIDStructAlloc;
recalculateAllocBuffer(&sizeIDStructAlloc, 4000);

//printf("  sizeIDStructAlloc= %d \n",sizeIDStructAlloc);

iDStructPtr = yl_calloc( "LP6", sizeIDStructAlloc, sizeof(BYTE));
if(iDStructPtr == NULL) {
	sprintf(img->error,"ticketRecognitionInit (\"%s\") yl_calloc failed: %s\n",img->szIMGName, strerror(errno));
	return -1;
}
*iDStructOutPtr = iDStructPtr; // ??????
wiDStructPtr = (WORD*)(iDStructPtr + 8);

//{char buffLog[500];

// sprintf(buffLog,"  sizeIDStructAlloc= %d,  iDStructPtr = %u\n",sizeIDStructAlloc, (unsigned)iDStructPtr);
// saveLog("", buffLog);
//}


radonStruct->pRadonBuffer   = (WORD*)workAreaPtr;

sizeIDStruct = 0;
shift = 0;
radonStruct-> stepD    = STEPXG;
radonStruct->pRadonMainTeta = wiDStructPtr + shift + 4;

radonStruct-> local = 0;

if( (radonStruct->phiMaxG < radonStruct->stepPhiG) && (-radonStruct->phiMinG < radonStruct->stepPhiG) ) {
	radonStruct->teta = 0;
}
else {
	retVal = radonMainG(radonStruct);
   	  if(retVal < 0)
   		    {yl_free(iDStructPtr);
   		     *iDStructOutPtr = NULL;
    	     return(retVal);
    	    }
}
retVal = radMT(radonStruct);


     if(retVal < 0)
	    {yl_free(iDStructPtr);
	      *iDStructOutPtr = NULL;
          return(retVal);
         }

*pAngle = radonStruct-> teta;

//printf("  *pAngle= %d \n",*pAngle);

sizeDTetaX = radonStruct->sizeDTeta;
dMinTetaX = radonStruct->dMinTeta;
*(wiDStructPtr + shift + 0L) = 2*(sizeDTetaX + 3);
*(wiDStructPtr + shift + 1L) = sizeDTetaX;
*(wiDStructPtr + shift + 2L) = (WORD)(dMinTetaX/SCALE);
*(wiDStructPtr + shift + 3L) = radonStruct->stepD;

sizeIDStruct = sizeIDStruct + (2*(sizeDTetaX + 4));
//printf("  sizeIDStruct= %d \n",sizeIDStruct);



err = recMargin((int*)radonStruct->pRadonMainTeta, sizeDTetaX, py1Teta, py2Teta);

//printf("  recMargin,err= %d \n",err);
//printf("1  y1Teta= %d, y2Teta= %d \n",y1Teta,y2Teta);

    if(err<0)
	    {yl_free(iDStructPtr);
	           *iDStructOutPtr = NULL;
               return(err);
              }
yTeta0 = (double)dMinTetaX/SCALE + 0.5;


shift = shift + (long)(sizeDTetaX + 4);

radonStruct-> pRadonDualTeta = wiDStructPtr + shift + 4;

//printf("1  shift= %d \n", shift);

//printf("  radDT \n");

retVal = radDT(radonStruct);

//printf("  radDT,retVal= %d \n",retVal);

     if(retVal < 0)
	    {yl_free(iDStructPtr);
	     *iDStructOutPtr = NULL;
         return(retVal);
        }

     sizeDTetaY = radonStruct->sizeDTeta;
     dMinTetaY = radonStruct->dMinTeta;
*(wiDStructPtr + shift + 0L) = 2*(sizeDTetaY + 3);
*(wiDStructPtr + shift + 1L) = sizeDTetaY;
*(wiDStructPtr + shift + 2L) = (WORD)(dMinTetaY/SCALE);
*(wiDStructPtr + shift + 3L) = radonStruct->stepD;

sizeIDStruct = sizeIDStruct + (2*(sizeDTetaY + 4));

err = recMargin((int*)radonStruct->pRadonDualTeta, sizeDTetaY, px1Teta, px2Teta);

    if(err<0)
    {yl_free(iDStructPtr);
                *iDStructOutPtr = NULL;
               return(err);
              }
xTeta0 = (double)dMinTetaY/SCALE + 0.5;

shift = shift + (sizeDTetaY + 4);

//printf("2  shift= %d, KLEVELS= %d \n",shift,KLEVELS );

for(k=1;k<KLEVELS;k++)  {
	radonStruct->stepD = STEPXG<<k;
    step = 1<<k;

//printf("3  k= %d, shift= %d, KLEVELS= %d \n",k, shift,KLEVELS );

    sizeDTetaX = sizeDTetaX>>1;
    *(wiDStructPtr + shift + 0L) = 2*(sizeDTetaX + 3);
    *(wiDStructPtr + shift + 1L) = sizeDTetaX;
    *(wiDStructPtr + shift + 2L) = (WORD)(dMinTetaX/SCALE);
    *(wiDStructPtr + shift + 3L) = radonStruct->stepD;

    pRadonMainTeta1 = wiDStructPtr + shift + 4L;

    for(i=0; i<sizeDTetaX; i++)    {
    	sum = 0;
         for(j=0; j<step; j++)
             sum = sum + *(radonStruct->pRadonMainTeta + i*step + (long)j);
         *(pRadonMainTeta1 + i) = (sum/step);
       }

    sizeIDStruct = sizeIDStruct + (2*(sizeDTetaX + 4));

    shift = shift + (sizeDTetaX + 4);

// printf("4  k= %d, shift= %d, KLEVELS= %d \n",k, shift,KLEVELS );

    sizeDTetaY = sizeDTetaY>>1;
    *(wiDStructPtr + shift + 0L) = 2*(sizeDTetaY + 3);
    *(wiDStructPtr + shift + 1L) = sizeDTetaY;
    *(wiDStructPtr + shift + 2L) = (WORD)(dMinTetaY/SCALE);
    *(wiDStructPtr + shift + 3L) = radonStruct->stepD;

    pRadonDualTeta1 = wiDStructPtr + shift + 4L;

    for(i=0; i<sizeDTetaY; i++)     {
    	sum = 0;
         for(j=0; j<step; j++)
             sum = sum + *(radonStruct->pRadonDualTeta + i*step + j);
         *(pRadonDualTeta1 + i) = (sum/step);
       }

    sizeIDStruct = sizeIDStruct + (2*(sizeDTetaY + 4));

    shift = shift + (long)(sizeDTetaY + 4);
   }



  wiDStructPtr = (WORD*)iDStructPtr;
*(wiDStructPtr + 2) = (WORD)(sizeIDStruct%65536);
*(wiDStructPtr + 3) = (WORD)(sizeIDStruct/65536);



//if(IDVersion>=2)
//  {lpwIDStructF = (WORD huge *)lpIDStructF;
//   *(lpwIDStructF + 2L) = (WORD)(sizeIDStruct%65536);
//   *(lpwIDStructF + 3L) = (WORD)(sizeIDStruct/65536);

//   GlobalUnlock(hgIDStructF);
//  }

{double xTeta[4];
 double yTeta[4];

 double cosTetaD, sinTetaD;

 int x[4];
 int y[4];

 int* lpiWorkArea;
 workAreaPtr = (LPBYTE)(workAreaOutPtr->ptr +8);
 lpiWorkArea = (int*)workAreaPtr;

 cosTetaD = cos(radonStruct->teta*PI/1800);
 sinTetaD = sin(radonStruct->teta*PI/1800);

 xTeta[0] = xTeta0 + (*px1Teta)*STEPYG; yTeta[0] = yTeta0 + (*py1Teta)*STEPXG;
 xTeta[1] = xTeta0 + (*px2Teta)*STEPYG; yTeta[1] = yTeta0 + (*py1Teta)*STEPXG;
 xTeta[2] = xTeta0 + (*px2Teta)*STEPYG; yTeta[2] = yTeta0 + (*py2Teta)*STEPXG;
 xTeta[3] = xTeta0 + (*px1Teta)*STEPYG; yTeta[3] = yTeta0 + (*py2Teta)*STEPXG;


 for(i=0; i<4; i++)
    {x[i] = (int)(xTeta[i]*cosTetaD - yTeta[i]*sinTetaD);
     y[i] = (int)(xTeta[i]*sinTetaD + yTeta[i]*cosTetaD);

//    printf("  i= %d, x[i]= %d, y[i]= %d \n",i, x[i], y[i]);

    lpiWorkArea[2*i] = x[i];
     lpiWorkArea[2*i+1] = y[i];
    }


 // rewrite margins in the permanent place

//  quadMargin.xP1 = x[0];
//  quadMargin.yP1 = y[0];
//  quadMargin.xP2 = x[1];
//  quadMargin.yP2 = y[1];
//  quadMargin.xP3 = x[2];
//  quadMargin.yP3 = y[2];
//  quadMargin.xP4 = x[3];
//  quadMargin.yP4 = y[3];

// GlobalUnlock(hgWorkArea);
// yl_free(workAreaPtr);yl_free(iDStructPtr); ???
}
//{char buffLog[500];

// sprintf(buffLog," return 0; *iDStructOutPtr = %u, sizeIDStruct= %d\n",(unsigned)(*iDStructOutPtr), sizeIDStruct);
// saveLog("", buffLog);
//}

return(0);
}
void ReleasePrototypes (LPTICKETDATA ticketDataPtr){

    if (!ticketDataPtr->numDescr) return;          // not allocated

    if(ticketDataPtr->ptrLink)
    	yl_free(ticketDataPtr->ptrLink);
    if(ticketDataPtr->ppIDStruct0)
    	yl_free(ticketDataPtr->ppIDStruct0);
    ticketDataPtr->numDescr    = 0;                // set all references to zero
    ticketDataPtr->ptrLink  = NULL;
    ticketDataPtr->ppIDStruct0 = NULL;
}

//----------------------------------------------------------------------------
// release ID-Structs, obtained by InitRecForm


int ReadPrototype (FILE   *f, char* fName,  LPBYTE* ptrOut, LPBYTE tmpBuf){
    LPBYTE ptrTarget;
    int    size, sizeInp;
    int   namelth;
    int   startInp;
    int   offset;
    int    len;

//    char buff[500];

    fseek(f, 0L,SEEK_END);
    if(ferror(f)){
//        sprintf(buff,"ReadPrototype (\"%s\") fseek failed: %s\n",fName, strerror(errno));
//        saveLog("\n ", buff);

    	return -1;
    }
    len = ftell(f);
    fseek(f, 0L,SEEK_SET);
    if(len > TMPBUFSIZE){
//        sprintf(buff,"ReadPrototype len ( %d ) > TMPBUFSIZE( %d )\n",len, TMPBUFSIZE);
//        saveLog("\n ", buff);

    	return -1;
    }

//    sprintf(buff,"%d\n",len);
//    saveLog("\n-------- len= ", buff);


    // read in the buffer
    size = fread(tmpBuf, len, 1, f);
    if(ferror(f) || size*len != len){

//        sprintf(buff,"%d\n",len);
//        saveLog("\n-------- len= ", buff);

    	return -1;
    }

    size *= len;
//    sprintf(buff,"%d\n",size);
//    saveLog("\n--------read OK, size= ", buff);

    // read OK, check if it is PRT-file
    if (*(LPWORD)tmpBuf != 0x4446) {
        return -2;
    }

   // get the location of ID-Struct and assert that the length is sufficient
    if (size > TMPBUFSIZE) size = TMPBUFSIZE;
    startInp = *(LPWORD)(tmpBuf + 6);
    sizeInp = (*(LPWORD)(tmpBuf + startInp) + 2);

//    sprintf(buff,"---- size= %d, startInp= %d, sizeInp= %d\n",size, startInp, sizeInp);
//    saveLog("", buff);

    if (size < 8 || (startInp + 2 > size) || (startInp + sizeInp > size)) {
//     printf("  prototype length problem; size= %d, startInp= %d, sizeInp= %d \n",size, startInp, sizeInp);
        return -3;
    }
//  printf("  size= %d, startInp= %d, sizeInp= %d \n",size, startInp, sizeInp);
    size = sizeInp;



//    startInp = getLeftBytes((int*)(tmpBuf + OffFdOffIDStruct),2);
// printf("  startInp= %d \n",startInp);
//    {int numZones;
//       numZones  = getLeftBytes((int*)(tmpBuf + OffFdZoneNo),2);
//        printf("  numZones= %d \n",numZones);
//    }
//    sizeInp = getLeftBytes((int*)(tmpBuf + startInp),2) +2;
// printf("  sizeInp= %d \n",sizeInp);

//#define ID_ORIGIN (*(LPWORD)(buf + 6))
//#define ID_SIZE   (*(LPWORD)(buf + ID_ORIGIN) + 2)



    // see if there is room in this chunk. If not, allocate another:
    ptrTarget = *ptrOut;
//    offset = *(int*)ptrTarget;
    offset = *(LPDWORD)ptrTarget;

    namelth = strlen (fName);
//   printf("  namelth= %d \n",namelth);
//   sprintf(buff,"---- offset= %d, namelth= %d\n",offset, namelth);
//   saveLog("", buff);

    if (offset + size + namelth + 3 >= PROTBUFF) {
//	   printf("  namelth= %d \n",namelth);

//  	 printf("  too small memory for this prototype offset + size + namelth= %d \n",offset + size + namelth);
   	    return -4;
    }


    // save data and file name in the chunk and increment offset
    memcpy (ptrTarget + offset, tmpBuf + startInp, size);
    strcpy  ((char*)(ptrTarget + offset + size), fName);    // save full file name

//    *(int*)ptrTarget += size + namelth + 1;
    *(LPDWORD)ptrTarget += size + namelth + 1;

 //  sprintf(buff,"---- next offset= %d\n",*(int*)ptrTarget);
//  saveLog("", buff);

    return 0;
}

int  LoadPrototypes (LPTICKETDATA ticketDataPtr)
{

	FILE   *f;
    char   path[500];
    char   fName[500];
    int    i;
    char   Prt[2];

    LPBYTE  ptrLink, ptrCurr, ptrBuf;
//    int     namelth;
    int     retP;

//    ReleaseProtChain (ticketDataPtr);    // release previous chain, if any

    strcpy (path, ticketDataPtr->prototypePath);
    saveLog("LoadPrototypes path ", path);

    if (strlen (path) == 0) {
    	sprintf(ticketDataPtr->error,"LoadPrototypes:  directory not defined\n");
        saveLog("LP Error: ", ticketDataPtr->error);
    	return -1;
    }



    ptrLink = yl_calloc( "LP7", PROTBUFF, sizeof(BYTE));
    if(ptrLink == NULL) {
//    	printf("yl_calloc failed");
    	sprintf(ticketDataPtr->error,"LoadProtDChain:  yl_calloc failed\n");
    	return -1;
    }
    ticketDataPtr->ptrLink = ptrLink;
    *(int*) ptrLink = 6;  // offset to 1-st prototype
//    *(LPWORD)(ptrLink + 4) = ptrLink;
    ptrBuf = yl_calloc( "LP8", TMPBUFSIZE, sizeof(BYTE));


    //strlwr (path);
    strcat (path, "LOTTO_");
    i = 1;
    ptrCurr = ptrLink;
    while(1) {
    	Prt[0] = '0'+i;
    	Prt[1] = 0;
    	strcpy(fName, path);
    	strcat (fName, Prt);
    	strcat (fName, ".PRT");
        f= fopen(fName, "r");
        if(f == NULL)
        	break;

  saveLog("", "\n");
  saveLog("reading file=  ", fName);
  saveLog("", "\n");
//  printf("LoadPrototypes:  path= %s\n", path);

        retP = ReadPrototype (f, fName, &ptrCurr, ptrBuf);
        close_file(f);
        if (retP == -1) break;
        i++;
    }

//    *(int)(ptrCurr+*(int*)ptrCurr) = 0;
//    *(LPWORD)(ptrCurr + *(long *)ptrCurr) = 0;   // set last-block marker
//    *(long *)ptrCurr = 0;                          // set end-of-chain marker

    *(ptrCurr + *ptrCurr) = 0;   // set last-block marker
    *(LPDWORD)ptrCurr = 0;                          // set end-of-chain marker

   i = 0;                                           // enumerate files
   ptrCurr = ptrLink;
   while (ptrCurr) {
      int  size;
//      LPBYTE next = *(LPBYTE*)ptrCurr;
      LPBYTE next = (LPBYTE)(*(LPDWORD)ptrCurr);
      ptrCurr += 6;
//      while (size == *(int)ptrCurr) {
      size = *(LPWORD)ptrCurr;
   	  while (size) {
         ((LPBYTE *)ptrBuf)[i++] = ptrCurr;
         ptrCurr += 2 + size + strlen ((char*)(ptrCurr + 2 + size)) + 1;
         size = *(LPWORD)ptrCurr;
      }
      ptrCurr = next;
   }

   ticketDataPtr->ppIDStruct0 = (LPBYTE*) ptrBuf;   // save list address
   ticketDataPtr->numDescr    = i;                     // save number of files



//   saveLog("ticketDataPtr->numDescr: %d\n", ticketDataPtr->numDescr);
   return i;
}

int  LoadPrototypesForType (LPTICKETDATA ticketDataPtr, char *protBaseName)
{

	FILE   *f;
    char   path[500];
    char   fName[500];
    int    i;
    char   Prt[2];

    LPBYTE  ptrLink, ptrCurr, ptrBuf;
//    int     namelth;
    int     retP;



//    ReleaseProtChain (ticketDataPtr);    // release previous chain, if any

    strcpy (path, ticketDataPtr->prototypePath);
//    saveLog("LoadPrototypes path ", path);

    if (strlen (path) == 0) {
    	sprintf(ticketDataPtr->error,"LoadPrototypes:  directory not defined\n");
        saveLog("LP Error: ", ticketDataPtr->error);
    	return -1;
    }


    ptrLink = yl_calloc( "LP7", PROTBUFF, sizeof(BYTE));
    if(ptrLink == NULL) {
//    	printf("yl_calloc failed");
    	sprintf(ticketDataPtr->error,"LoadProtDChain:  yl_calloc failed\n");
    	return -1;
    }
    ticketDataPtr->ptrLink = ptrLink;
    *(int*) ptrLink = 6;  // offset to 1-st prototype
//    *(LPWORD)(ptrLink + 4) = ptrLink;
    ptrBuf = yl_calloc( "LP8", TMPBUFSIZE, sizeof(BYTE));


    //strlwr (path);
    strcat (path, protBaseName);
    strcat (path, "_");


    i = 1;
    ptrCurr = ptrLink;
    while(1) {
    	Prt[0] = '0'+i;
    	Prt[1] = 0;
    	strcpy(fName, path);
    	strcat (fName, Prt);
    	strcat (fName, ".PRT");
        f= fopen(fName, "r");
        if(f == NULL)
        	break;

  saveLog("", "\n");
  saveLog("reading file=  ", fName);
  saveLog("", "\n");
//  printf("reading file= %s\n", fName);
//  printf("LoadPrototypes:  path= %s\n", path);

        retP = ReadPrototype (f, fName, &ptrCurr, ptrBuf);
        close_file(f);
        if (retP == -1) break;
        i++;


// printf("---- ptrCurr= %lu\n",(unsigned long)ptrCurr);
// printf("---- ptrLink= %lu\n",(unsigned long)ptrLink);

    }


       {
        LPDWORD ptrCurrDW;
        WORD *ptrCurrW;
        ptrCurrDW = (LPDWORD)ptrCurr;
        int shift =  *ptrCurrDW;
//        printf("---- shift= %d\n",shift);
//        printf("---- (ptrCurr + shift)= %lu, *(ptrCurr + shift)= %d\n",(unsigned long)(ptrCurr + shift), *(ptrCurr + shift));
        ptrCurrW = (LPWORD)(ptrCurr + shift);
        *ptrCurrW = 0;   // set last-block marker
        *ptrCurrDW = 0;

//      *(ptrCurr + *ptrCurr) = 0;   // set last-block marker
//        *(LPDWORD)ptrCurr = 0;       // set end-of-chain marker
       }

   i = 0;                                           // enumerate files
   ptrCurr = ptrLink;
   while (ptrCurr) {
      int  size;
//      LPBYTE next = (LPBYTE*)(*ptrCurr);
      LPBYTE next = (LPBYTE)(*(LPDWORD)ptrCurr);

// printf("---- next= %u\n",(unsigned int)next);

      ptrCurr += 6;
//      while (size == *(int)ptrCurr) {
      size = *(LPWORD)ptrCurr;
//   printf("---- size= %d, size+1= %d, , size+2= %d\n",size, *(LPWORD)(ptrCurr+2), *(LPWORD)(ptrCurr+4));
   	  while (size) {
         ((LPBYTE *)ptrBuf)[i++] = ptrCurr;
         ptrCurr += 2 + size + strlen ((char*)(ptrCurr + 2 + size)) + 1;
         size = *(LPWORD)ptrCurr;
       }
      ptrCurr = next;
   }

   ticketDataPtr->ppIDStruct0 = (LPBYTE*) ptrBuf;   // save list address
   ticketDataPtr->numDescr    = i;                     // save number of files

//   saveLog("ticketDataPtr->numDescr: %d\n", ticketDataPtr->numDescr);
   return i;
}



int  ticketRecognitionStruct(LPIMGDATA img, LPRADON radonStruct, LPBYTE pTicketDescr, HLPBYTE* pWorkAreaPtr, LPBYTE* pTicketStruct,
		int iAngle, int tgpA)
{

 LPBYTE workAreaPtr;
 LPBYTE ticketStructPtr;

 int retVal;
 int err;
 long shift;
 long wh;
 DWORD sizeWorkAreaAlloc;
 DWORD sizeTicketStructAlloc;
 long sizeTicketStruct;
 int  sizeGlobalStruct;
 WORD*  pwTicketStruct;
 WORD*  pwTicketDescr;
 int  offsetGlobal;
 WORD* pwGlobalStruct;
// int* pwGlobalStruct;
 int  sizeDX0;
 int  dMinX0;
 int  stepDX0;
 WORD* pRadonMainTeta0;
 int sizeDY0;
 int dMinY0;
 int stepDY0;
 WORD* pRadonDualTeta0;
 long size0;
 long size1;
 long size2;
 long size3;
 long size4;
 DWORD* pDist;
 WORD* pPos0;
 WORD* pPosEnd;
 DWORD dist;
 WORD pos0;
 WORD  posEnd;
 WORD* pAllign;

 int x1Teta, x2Teta, y1Teta, y2Teta;
 int* px1Teta = &x1Teta;
 int* px2Teta = &x2Teta;
 int* py1Teta = &y1Teta;
 int* py2Teta = &y2Teta;

 double xTeta0, yTeta0;

// char buff[500];

 radonStruct->local = 0;

 pDist = &dist;
 pPos0 = &pos0;
 pPosEnd = &posEnd;

 radonStruct->pTicketBuff   = img -> pixPtr;
 radonStruct->widthTicket     = img ->bi.biWidth;
 radonStruct->heightTicket    = img ->bi.biHeight;
 radonStruct->originX       = 0;
 radonStruct->originY       = 0;
 radonStruct->widthRec      = (radonStruct->widthTicket/8)*8;
 radonStruct->heightRec     = radonStruct->heightTicket;

 radonStruct->byteWidthTicket = ((radonStruct->widthTicket + 31)/32)*4;

 pwTicketDescr = (WORD*)pTicketDescr;
 offsetGlobal = *(pwTicketDescr+3);
 pwGlobalStruct = (WORD*)(pTicketDescr + offsetGlobal + 2);
 sizeGlobalStruct = *(pwGlobalStruct - 1L);
 shift = (long)(*pwGlobalStruct/2) + 1;
 sizeDX0 = *(pwGlobalStruct+1);
 dMinX0 = (int)(*(pwGlobalStruct+2));
 stepDX0 = *(pwGlobalStruct+3);
 pRadonMainTeta0 = pwGlobalStruct+4;
 pwGlobalStruct = pwGlobalStruct + shift;
 sizeDY0 = *(pwGlobalStruct+1);
 dMinY0 = (int)(*(pwGlobalStruct+2));
 stepDY0 = *(pwGlobalStruct+3);
 pRadonDualTeta0 = pwGlobalStruct+4;

 sizeTicketStruct = (DWORD)(*(pwTicketDescr + offsetGlobal/2));
 shift = (long)(offsetGlobal/2 + 1 + sizeTicketStruct/2);


// printf("  offsetGlobal= %d, sizeGlobalStruct= %d \n",offsetGlobal,sizeGlobalStruct);
// printf("  shift= %ld, sizeDX0= %d \n",shift, sizeDX0);
// printf("  dMinX0= %d, stepDX0= %d \n",dMinX0, stepDX0);
// printf("  sizeDY0= %d, dMinY0= %d \n",sizeDY0, dMinY0);
// printf("  stepDY0= %d, sizeTicketStruct= %ld \n",stepDY0, sizeTicketStruct);

//   	  	      sprintf(buff,"offsetGlobal= %d, sizeGlobalStruct= %d\n",offsetGlobal, sizeGlobalStruct);
//   	   	      saveLog("", buff);
//   	  	      sprintf(buff,"shift= %ld, sizeDX0= %d\n",shift, sizeDX0);
//   	   	      saveLog("", buff);


 radonStruct->tgP = (int)tgpA;

 radonStruct->phiMin = iAngle - 50;
 radonStruct->phiMax = iAngle + 50;
 radonStruct->stepPhi = radonStruct->stepPhiG;

 if(radonStruct->widthTicket > radonStruct->heightTicket) wh = (long)radonStruct->widthTicket;
                       else wh = (long)radonStruct->heightTicket;
// size0 = 2L*(2L*wh/STEPXG)*(2*phiMax/stepPhi + 1) + 101L;
 size0 = 2*(2*wh/STEPXG)*((radonStruct->phiMax-radonStruct->phiMin)/radonStruct->stepPhi + 1) + 101;
 size1 = 3*(2*wh*wh/STEPXG)/STEPYG + 101;
 size1 = (size0 > size1 ? size0 : size1);
 size2 = radonStruct->widthTicket/STEPYG;
 size3 = radonStruct->heightTicket/STEPXG;
 if(size2 > size3) size4 = size2*size2;
              else size4 = size3*size3;
 if(size1 > size4) sizeWorkAreaAlloc = size1;
              else sizeWorkAreaAlloc = size4;
 sizeWorkAreaAlloc = sizeWorkAreaAlloc*3/2;
 sizeTicketStructAlloc =
           (long)(44 + 2*(radonStruct->widthTicket/STEPYG + radonStruct->heightTicket/STEPXG + 2) + 100);

 recalculateAllocBuffer(&sizeWorkAreaAlloc, 300000);

//   printf("  sizeWorkAreaAlloc= %d \n",sizeWorkAreaAlloc);

       workAreaPtr = pWorkAreaPtr->ptr;
//       workAreaPtr = yl_calloc( "LP9", sizeWorkAreaAlloc, sizeof(BYTE));
       if(workAreaPtr == NULL) {
       	sprintf(img->error,"ticketRecognitionInit (\"%s\") yl_calloc failed: %s\n",img->szIMGName, strerror(errno));
       	return -1;
       }
       if(sizeWorkAreaAlloc > pWorkAreaPtr->size) {
       	sprintf(img->error,"ticketRecognitionInit (\"%s\") too small size of workArea\n",img->szIMGName);
       	return -1;
       }

//       *pWorkAreaPtr = workAreaPtr;
//       pWorkAreaPtr->ptr = workAreaPtr;
//       pWorkAreaPtr->size = sizeWorkAreaAlloc;
       workAreaPtr = workAreaPtr + 8;  // ?????

       recalculateAllocBuffer(&sizeTicketStructAlloc, 4000);
//printf("  sizeTicketStructAlloc= %d \n",sizeTicketStructAlloc);

       ticketStructPtr = yl_calloc( "LP10", sizeTicketStructAlloc, sizeof(BYTE));
       if(ticketStructPtr == NULL) {
       	sprintf(img->error,"ticketRecognitionInit (\"%s\") yl_calloc failed: %s\n",img->szIMGName, strerror(errno));
       	return -1;
       }
       *pTicketStruct = ticketStructPtr;
       pwTicketStruct= (WORD*)(ticketStructPtr + 8);
       radonStruct->pRadonBuffer   = (WORD*)workAreaPtr;
       radonStruct->stepD          = STEPXG;

       radonStruct->pRadonMainTeta = &buffer[0];


// teta = iAngle;

   //phiMinG, phiMaxG, stepPhiG - static variables defined in initRecForm

//   phiMin = iAngle - 50;
//   phiMax = iAngle + 50;
//   stepPhi = stepPhiG;

//	printf("  radonMainG \n");
//  	      sprintf(buff,"radonMainG\n");
//   	      saveLog("", buff);

    	retVal = radonMainG(radonStruct);

//	      sprintf(buff,"retVal= %d\n",retVal);
// 	      saveLog("", buff);

//  printf("  radonMainG,retVal= %d \n",retVal);
//  printf("  radonStruct->teta= %d \n", radonStruct->teta);

   if(retVal < 0)
	    {yl_free(ticketStructPtr);
	    *pTicketStruct = NULL;
     	  return(retVal);
      	 }


      retVal = radMT(radonStruct);


//     printf("  radMT,retVal= %d \n",retVal);

      if(retVal < 0)
	    {yl_free(ticketStructPtr);
	       *pTicketStruct = NULL;
           return(retVal);
          }

      iAngle = radonStruct->teta;
//printf("  iAngle= %d \n",iAngle);

      radonStruct->tetaG = radonStruct->teta;        //static variable for extractZoneStruct

     err = recMargin((int*)radonStruct->pRadonMainTeta, radonStruct->sizeDTeta, py1Teta, py2Teta);
//     printf("  recMargin,err= %d \n",err);
//     printf("  y1Teta= %d, y2Teta= %d \n",y1Teta,y2Teta);

     if(err<0)
   	           {yl_free(ticketStructPtr);
   	           *pTicketStruct = NULL;
               return(err);
               }
     yTeta0 = (double)radonStruct->dMinTeta/SCALE + 0.5;

     pAllign = pwTicketStruct + 9;

     err = allignArrays (radonStruct, workAreaPtr, pRadonMainTeta0, radonStruct->pRadonMainTeta,
    		             sizeDX0, radonStruct->sizeDTeta, pDist, pAllign, pPos0, pPosEnd);

         if(err < 0)
	           {yl_free(ticketStructPtr);
	           *pTicketStruct = NULL;
              return(err);
            }

 *(ticketStructPtr + 0) = 2*(sizeDX0 + 8);
 *(ticketStructPtr + 1) = sizeDX0;
 *(ticketStructPtr + 2) = radonStruct->sizeDTeta;
 *(ticketStructPtr + 3) = (int)dMinX0;
 *(ticketStructPtr + 4) = (int)(radonStruct->dMinTeta/SCALE);
 *(ticketStructPtr + 5) = stepDX0;
 *(ticketStructPtr + 6) = radonStruct->stepD;
 *(ticketStructPtr + 7) = pos0;
 *(ticketStructPtr + 8) = posEnd;

 sizeTicketStruct = (long)(2*(sizeDX0 + 9));

 shift = (long)(sizeDX0 + 9);

 ticketStructPtr = ticketStructPtr + shift;

 radonStruct->stepD          = STEPYG;

 radonStruct->pRadonDualTeta = &buffer[0];

// retVal = radonDualTeta();
 retVal = radDT(radonStruct);
// printf("  radDT,retVal= %d \n",retVal);

      if(retVal < 0)
      {yl_free(ticketStructPtr);
          *pTicketStruct = NULL;
           return(retVal);
          }

 err = recMargin((int*)radonStruct->pRadonDualTeta, radonStruct->sizeDTeta, px1Teta, px2Teta);
// printf("  recMargin2,err= %d \n",err);
// printf("  x1Teta= %d, x2Teta= %d \n",x1Teta, x2Teta);


     if(err<0)
         {yl_free(ticketStructPtr);
              *pTicketStruct = NULL;
                return(retVal);
               }
 xTeta0 = (double)radonStruct->dMinTeta/SCALE + 0.5;

 pAllign = pwTicketStruct + 9;
 err = allignArrays (radonStruct, workAreaPtr, pRadonDualTeta0, radonStruct->pRadonDualTeta,
		             sizeDY0, radonStruct->sizeDTeta, pDist, pAllign, pPos0, pPosEnd);
         if(err < 0)
         {yl_free(ticketStructPtr);
             *pTicketStruct = NULL;
              return(err);
            }

 *(ticketStructPtr + 0L) = 2*(sizeDY0 + 8);
 *(ticketStructPtr + 1L) = sizeDY0;
 *(ticketStructPtr + 2L) = radonStruct->sizeDTeta;
 *(ticketStructPtr + 3L) = (int)dMinY0;
 *(ticketStructPtr + 4L) = (int)(radonStruct->dMinTeta/SCALE);
 *(ticketStructPtr + 5L) = stepDY0;
 *(ticketStructPtr + 6L) = radonStruct->stepD;
 *(ticketStructPtr + 7L) = pos0;
 *(ticketStructPtr + 8L) = posEnd;

 sizeTicketStruct = sizeTicketStruct + (long)(2*(sizeDY0 + 9));
 pwTicketStruct = (WORD*)ticketStructPtr;

 *(pwTicketStruct + 2) = (int)(sizeTicketStruct%65536);
 *(pwTicketStruct + 3) = (int)(sizeTicketStruct/65536);

 {int i;

  double xTeta[4];
  double yTeta[4];

  double cosTetaD, sinTetaD;

  int x[4];
  int y[4];

  int* lpiWorkArea;
  workAreaPtr = (LPBYTE)(pWorkAreaPtr->ptr +8);
  lpiWorkArea = (int*)workAreaPtr;

  cosTetaD = cos(radonStruct->teta*PI/1800);
  sinTetaD = sin(radonStruct->teta*PI/1800);

  xTeta[0] = xTeta0 + (*px1Teta)*STEPYG; yTeta[0] = yTeta0 + (*py1Teta)*STEPXG;
  xTeta[1] = xTeta0 + (*px2Teta)*STEPYG; yTeta[1] = yTeta0 + (*py1Teta)*STEPXG;
  xTeta[2] = xTeta0 + (*px2Teta)*STEPYG; yTeta[2] = yTeta0 + (*py2Teta)*STEPXG;
  xTeta[3] = xTeta0 + (*px1Teta)*STEPYG; yTeta[3] = yTeta0 + (*py2Teta)*STEPXG;

  for(i=0; i<4; i++)
     {x[i] = (int)(xTeta[i]*cosTetaD - yTeta[i]*sinTetaD);
      y[i] = (int)(xTeta[i]*sinTetaD + yTeta[i]*cosTetaD);


//      printf("  i= %d, x[i]= %d, y[i]= %d \n",i, x[i], y[i]);

      lpiWorkArea[2*i] = x[i];
      lpiWorkArea[2*i+1] = y[i];
     }

//  quadMargin.xP1 = x[0];
//  quadMargin.yP1 = y[0];
//  quadMargin.xP2 = x[1];
//  quadMargin.yP2 = y[1];
//  quadMargin.xP3 = x[2];
//  quadMargin.yP3 = y[2];
//  quadMargin.xP4 = x[3];
//  quadMargin.yP4 = y[3];

//  GlobalUnlock(hgWorkArea);
 }

 return(0);
}

int IdentifyTicketCall (LPRADON radonStruct, HLPBYTE* pWorkAreaPtr, LPTICKETDATA ticketDataPtr, LPBYTE iDStructPtr){
    WORD     wDescrNo = 0, wFlags;
    int ver = 1;
//    char buffLog[500];

    if (!ticketDataPtr->numDescr      && LoadPrototypes (ticketDataPtr) <= 0) return -1;
    ticketDataPtr->ptrOut = yl_calloc( "LP11", 3100 * 2, sizeof(int));
    if(ticketDataPtr->ptrOut == NULL)  return -1;
    wFlags = 0;

//    saveLog(" ---------- 1 IdentifyTicketCall  ", "\n");
/*
	{
	  char buffLog[500];
          saveLog("before identifyTicket", "\n");
	      sprintf(buffLog," radonStruct= %u, radonStruct->a1[1]= %d\n",(unsigned)radonStruct, radonStruct->a1[1]);
	      saveLog("", buffLog);
	  }
*/

    ticketDataPtr->returnCode = identifyTicket(radonStruct, ticketDataPtr->numDescr, ticketDataPtr->ppIDStruct0, ver, iDStructPtr, pWorkAreaPtr,
    		                                 &wDescrNo, &ticketDataPtr->wFlip, ticketDataPtr->confidence, ticketDataPtr->tgp, ticketDataPtr->wPar, wFlags, ticketDataPtr->ptrOut);

    if (wDescrNo >= 0 && wDescrNo < ticketDataPtr->numDescr) {
        strcpy (ticketDataPtr->prototypeName,
              (LPSTR)(ticketDataPtr->ppIDStruct0[wDescrNo]) + *(LPWORD)(ticketDataPtr->ppIDStruct0[wDescrNo]) + 2);
        saveLog("ticketDataPtr->prototypeName  ", ticketDataPtr->prototypeName);
        saveLog("  ", "\n");
    }
    else
    	ticketDataPtr->prototypeName[0] = 0;

    if (ticketDataPtr->returnCode < 0) {
        return -2;
    }
    return 1;
}
int identifyTicket(LPRADON radonStruct, WORD numDescr, LPBYTE* ppIDStruct0, int IDVersion, LPBYTE pIDStructInp, HLPBYTE* workAreaOutPtr,
                   WORD * pwDescrNo, WORD * pwFlip, WORD * pwConfidence, WORD tgp, WORD wPar, WORD wFlags, int * pwOut){

 LPBYTE workAreaPtr;
 BYTE * lpWorkArea;
 LPWORD * ppwIDStruct0;
 BYTE * pIDStruct;
 BYTE  * pIDStructF;

 int err;
 DWORD sizeWorkAreaAlloc;
 WORD * pwIDStruct;
 WORD * pwIDStructF = NULL;
 WORD * pwIDStruct0;
 WORD sizeDX0;
 WORD * pRadonMainTeta0;
 WORD sizeDX1;
 WORD sizeDX1F = 0;
 WORD sDX1 = 0;
 WORD * pRadonMainTetaF = NULL;
 WORD * pRMT = NULL;
 WORD sizeDY0;
 WORD * pRadonDualTeta0;
 WORD sizeDY1;
 WORD sizeDY1F = 0;
 WORD sDY1 = 0;
 WORD * pRadonDualTetaF = NULL;
 WORD * pRDT = NULL;
 DWORD * pDist;
 WORD *  pPos0;
 WORD *  pPosEnd;
 DWORD distX;
 DWORD distY;
 DWORD dist;
 WORD pos0;
 WORD posEnd;
 WORD * pAllign;
 DWORD sizeX;
 DWORD sizeY;

// WORD i, j, j1, k, k1, kmax;
 int i, j, j1, k, k1, kmax;
 WORD wFlag[] = {1, 1, 1, 1};
 WORD wTout;
 WORD wNin;
 WORD wNinM[] = {3, 6, 16000, 16000};
 WORD nOut;
 WORD dmax;
 WORD index;
 WORD fl;
 long shift;
 long sizeIDStruct0;
 int conf1 = 0, conf2 = 0;
 int imgSize = 0, prtSize;
 float ratioSize = 0;
// char buff[500];



 //---------------------------------------------
// sprintf(buff,"numDescr= %d\n",numDescr);
 //saveLog(" ", buff);
 //---------------------------------------------
/*
	{
	  char buffLog[500];
       saveLog("start identifyTicket", "\n");
	      sprintf(buffLog," radonStruct= %u, radonStruct->a1[1]= %d\n",(unsigned)radonStruct, radonStruct->a1[1]);
	      saveLog("", buffLog);
   }
*/

 if(numDescr < 1) return(-1);

 ppwIDStruct0 = (LPWORD *)ppIDStruct0;

 radonStruct->tgP = tgp;
 wTout = wPar;

 if(wFlags != 0)
     {for(k=0; k<KLEVELS; k++)
          {wFlag[k] = wFlags%2;
           wFlags = wFlags/2;
          }
     }

 if(numDescr == 1)
    {wFlag[0] = 1;
     kmax = 0;
    }
 else kmax = KLEVELS - 1;

 pDist = &dist;
 pPos0 = &pos0;
 pPosEnd = &posEnd;

 pIDStruct = pIDStructInp;
 pwIDStruct = (WORD *)(pIDStruct + 8);
 if(IDVersion >= 2)
    {pIDStructF = pIDStructInp+1;
     pwIDStructF = (WORD *)(pIDStructF + 8);
    }
 wNin = numDescr;
 if(IDVersion == 1) wNin = numDescr;
 if(IDVersion == 2) wNin = 2*numDescr;
 if(IDVersion == 3) wNin = 4*numDescr;

 if(wNin >= NPRT) {return-1;}

//---------------------------------------------
//saveLog("wNin= %d\n",wNin);
//---------------------------------------------

 for(i=0; i<numDescr; i++) {num_PRT[i] = i; flip_PRT[i] = 0; }
 if(IDVersion >= 2)
    {for(i=0; i<numDescr; i++) {num_PRT[i+numDescr] = i;flip_PRT[i+numDescr] = 1; }}
 if(IDVersion == 3)
    {for(i=0; i<numDescr; i++) {num_PRT[i+2*numDescr] = i; flip_PRT[i+2*numDescr] = 2; num_PRT[i+3*numDescr] = i; flip_PRT[i+3*numDescr] = 3; }}


 for(i=0; i<wNin; i++) {
     pwIDStruct0 = *(ppwIDStruct0 + (long)(num_PRT[i]));
     sizeIDStruct0 = *pwIDStruct0/2;
     pwIDStruct0 = pwIDStruct0 + 1;

     *pwDescrNo = (WORD)(-1);
     shift = 0;
     for(k1=0; k1<2*KLEVELS; k1++){
//  printf("2  k1= %d, shift= %ld, sizeIDStruct0= %ld\n",k1, shift, sizeIDStruct0);
         {if(shift >= sizeIDStruct0) {*pwDescrNo = i; return(-1);}
          shift = shift + (long)(*(pwIDStruct0 + shift + 1)) + 4;
         }
     }
     if(shift >= sizeIDStruct0 + 2L)
        {*pwDescrNo = i; return(-1); }
     }

//	{
//	  char buffLog[500];
//    saveLog("before for(k=kmax; (int)k>=0; k--)", "\n");
//	      sprintf(buffLog," radonStruct= %u, radonStruct->a1[1]= %d\n",(unsigned)radonStruct, radonStruct->a1[1]);
//	      saveLog("", buffLog);
// }

 for(k=kmax; (int)k>=0; k--)
//    {if(k==0) local_PRT = 0; else local_PRT = 1;
    {if(k==0) radonStruct->local = 0; else radonStruct->local = 1;
     if(wFlag[k] != 0)
        {for(j=0; j<wNin; j++) d_PRT[j]=1001;
         shift = 0;
         for(k1=0; k1<2*k; k1++)
             shift = shift + (long)(*(pwIDStruct + shift + 1L)) + 4L;
         sizeDX1 = *(pwIDStruct + shift + 1L);
         pRadonMainTeta_PRT = pwIDStruct + shift + 4L;

         shift = shift + (long)(*(pwIDStruct + shift + 1L)) + 4L;
         sizeDY1 = *(pwIDStruct + shift + 1L);
         pRadonDualTeta_PRT = pwIDStruct + shift + 4L;

         if(IDVersion >= 2){
             shift = 0;
             for(k1=0; k1<2*k; k1++)
                 shift = shift + (long)(*(pwIDStructF + shift + 1L)) + 4L;
             sizeDX1F = *(pwIDStructF + shift + 1L);
             pRadonMainTetaF = pwIDStructF + shift + 4L;

             shift = shift + (long)(*(pwIDStructF + shift + 1L)) + 4L;
             sizeDY1F = *(pwIDStructF + shift + 1L);
             pRadonDualTetaF = pwIDStructF + shift + 4L;
            }
     if(k==kmax)
        imgSize = sizeDX1*sizeDY1;
         sizeX = 6*(DWORD)sizeDX1*(DWORD)sizeDX1 + 100L;
         sizeY = 6*(DWORD)sizeDY1*(DWORD)sizeDY1 + 100L;

         sizeWorkAreaAlloc = (sizeX > sizeY ? sizeX : sizeY);
         recalculateAllocBuffer(&sizeWorkAreaAlloc, 4000);


         workAreaPtr = workAreaOutPtr->ptr;
         if(sizeWorkAreaAlloc > workAreaOutPtr->size) {
         	   return -1;
         }
//         workAreaPtr = yl_calloc( "LP12", sizeWorkAreaAlloc, sizeof(BYTE));
//         if(workAreaPtr == NULL || sizeWorkAreaAlloc > workAreaOutPtr->size) {
//         	return -1;
         //}

//         workAreaOutPtr->ptr = workAreaPtr;
//         workAreaOutPtr->size = sizeWorkAreaAlloc;
         lpWorkArea = workAreaPtr + 8;

/*
     	{
     	  char buffLog[500];
         saveLog("before for(i=0; i<wNin; i++){", "\n");
     	      sprintf(buffLog," radonStruct= %u, radonStruct->a1[1]= %d\n",(unsigned)radonStruct, radonStruct->a1[1]);
     	      saveLog("", buffLog);
      }
*/
         for(i=0; i<wNin; i++){
             pwIDStruct0 = *(ppwIDStruct0 + (long)(num_PRT[i]));
             pwIDStruct0 = pwIDStruct0 + 1L;

             shift = 0;
             for(k1=0; k1<2*k; k1++)
                 shift = shift + (long)(*(pwIDStruct0 + shift + 1L)) + 4L;
             sizeDX0 = *(pwIDStruct0 + shift + 1L);
             pRadonMainTeta0 = pwIDStruct0 + shift + 4L;

             shift = shift + (long)(*(pwIDStruct0 + shift + 1L)) + 4L;
             sizeDY0 = *(pwIDStruct0 + shift + 1L);
             pRadonDualTeta0 = pwIDStruct0 + shift + 4L;
         if(k==kmax) {
            prtSize = sizeDX0*sizeDY0;
         ratioSize = 0;
         if(imgSize > 0)
        	 ratioSize = (float)prtSize/(float)imgSize;
         }
//   printf("------------------------------------prtSize= %d, imgSize= %d, idx= %d \n",prtSize, imgSize, num_PRT[i] );
//   printf("------------------------------------ratioSize= %2.4f, i= %d \n",ratioSize, i);

             if((((sizeDX0 < 3*sizeDX1)&&(sizeDX1 < 3*sizeDX0)&&
                (sizeDY0 < 3*sizeDY1)&&(sizeDY1 < 3*sizeDY0))||
                ((sizeDX0 < 3*sizeDY1)&&(sizeDY1 < 3*sizeDX0)&&
                (sizeDY0 < 3*sizeDX1)&&(sizeDX1 < 3*sizeDY0))) && ratioSize > 0.6)
                {if(flip_PRT[i] == 0)
                    {pRMT = pRadonMainTeta_PRT;
                     sDX1 = sizeDX1;
                     pRDT = pRadonDualTeta_PRT;
                     sDY1 = sizeDY1;
                    }
                 if(flip_PRT[i] == 1)
                    {pRMT = pRadonMainTetaF;
                     sDX1 = sizeDX1F;
                     pRDT = pRadonDualTetaF;
                     sDY1 = sizeDY1F;
                    }
                 if(flip_PRT[i] == 2)
                    {pRMT = pRadonDualTetaF;
                     sDX1 = sizeDY1F;
                     pRDT = pRadonMainTeta_PRT;
                     sDY1 = sizeDX1;
                    }
                 if(flip_PRT[i] == 3)
                    {pRMT = pRadonDualTeta_PRT;
                     sDX1 = sizeDY1;
                     pRDT = pRadonMainTetaF;
                     sDY1 = sizeDX1F;
                    }

                 pAllign = (WORD *)&buffer[0];
/*
                 {char buffLog[500];

                  sprintf(buffLog,"  i= %d, wNin= %d \n",i, wNin);
                  saveLog("", buffLog);
                  sprintf(buffLog,"  flip_PRT[i]= %d, sDX1= %d, sDY1= %d \n",flip_PRT[i], sDX1, sDY1);
                  saveLog("", buffLog);
                  sprintf(buffLog,"  sizeX0= %d, sizeY= %d, IDVersion= %d, numDescr= %d \n",sizeX, sizeY, IDVersion, numDescr);
                  saveLog("", buffLog);
                  sprintf(buffLog,"  sizeDX0= %d, sizeDX1= %d, sizeDY0= %d, sizeDY1= %d \n",sizeDX0, sizeDX1, sizeDY0, sizeDY1);
                  saveLog("", buffLog);
         		  sprintf(buffLog,"  sizeWorkAreaAlloc= %d, pRMT - pwIDStruct= %u, (pRDT - pwIDStruct) = %u \n",sizeWorkAreaAlloc, (unsigned)(pRMT - pwIDStruct),(unsigned)(pRDT - pwIDStruct));
         		  saveLog("", buffLog);
                 }

              	{
              	  char buffLog[500];
                  saveLog("before allignArrays", "\n");
              	      sprintf(buffLog," radonStruct= %u, radonStruct->a1[1]= %d\n",(unsigned)radonStruct, radonStruct->a1[1]);
              	      saveLog("", buffLog);
               }
*/
                 err = allignArrays (radonStruct, lpWorkArea, pRadonMainTeta0, pRMT,
                		             sizeDX0, sDX1, pDist, pAllign, pPos0, pPosEnd);

                 if(err < -1) {return(err);}
                 distX = *pDist;
                 pAllign = (WORD *)&buffer[0];

                 err = allignArrays (radonStruct, lpWorkArea, pRadonDualTeta0, pRDT,
                		             sizeDY0, sDY1, pDist, pAllign, pPos0, pPosEnd);

                 if(err < -1) {return(err);}
                 distY = *pDist;

                 //---------------------------------------------

                 //saveLog("distX= %d, distY= %d, i= %d\n",dist, distY, i);
                 //---------------------------------------------

                 dist = (2*distX + distY)/6;
                 if(dist > 1000) dist = 1000;
                }
             else dist = 1000;


             for(j=0; j<wNin; j++){
                if(dist < d_PRT[j]){
                   index = num_PRT[i];
                    fl = flip_PRT[i];
                    for(j1=i-1; j1>=j; j1--){
                        d_PRT[j1+1] = d_PRT[j1];
                         num_PRT[j1+1] = num_PRT[j1];
                         flip_PRT[j1+1] = flip_PRT[j1];

                        }
                     d_PRT[j] = (WORD)dist;
                     num_PRT[j] = index;
                     flip_PRT[j] = fl;

                     break;
                    }
                }

            }
//         for(j=0; j<wNin; j++){
//             printf("dist=%d, index= %d \n",d_PRT[j], num_PRT[j]);
//         }

         dmax = (WORD)((100L + wTout)*d_PRT[0]/100);
         nOut = 1;
         for(j=1; j<wNin; j++)
            {if(d_PRT[j] < dmax) nOut += 1;
                        else break;
            }
         if(nOut == 1) conf1 = 100;
         else
           {if(d_PRT[0] == 0) conf1 = 100;
            else conf1 = (int)((d_PRT[1] - d_PRT[0])*100L/d_PRT[0]*100L/wTout);
            if(conf1 > 100) conf1 = 100;
           }
         wNin = (nOut < wNinM[k] ? nOut : wNinM[k]);
        }
     if(wNin == 1)
        {wFlag[0] = 1;
    	 if(wFlag[0]) k = 0;
         else break;
        }
    }

 *pwDescrNo = num_PRT[0];
 *pwFlip = flip_PRT[0];

 conf2 = (int)((500 - (int)d_PRT[0])/5);
 if(conf2 < 0) conf2 = 0;

 *pwConfidence        = (WORD)conf2;
 *(pwConfidence + 1L) = (WORD)conf1;

 for(i=0;i<1000; i++)
    {pwOut[3*i+100] = d_PRT[i];
     pwOut[3*i+101] = num_PRT[i];
     pwOut[3*i+102] = flip_PRT[i];
    }

 return(0);
}

void loadOnePhiL(int mPhi, long *pRad, int *phiR, long *distR, int *sizeDTR, LPRADON radonStruct)

{

 long  shift;
 long  mD;
 WORD *p;
 long sizeDTeta;

  *phiR   = radonStruct->phiMin + (int)radonStruct->stepPhi*mPhi;
  if(*phiR > 0)
         {radonStruct->dMinTeta = (long)radonStruct->originY*radonStruct->cosArray[mPhi] -
                   (long)(radonStruct->originX+radonStruct->widthRec)*radonStruct->sinArray[mPhi];
         radonStruct->dMaxTeta = (long)(radonStruct->originY+radonStruct->heightRec)*radonStruct->cosArray[mPhi] -
                   (long)radonStruct->originX*radonStruct->sinArray[mPhi];
       }
    else {
    	radonStruct->dMinTeta = (long)radonStruct->originY*radonStruct->cosArray[mPhi] -
                  (long)radonStruct->originX*radonStruct->sinArray[mPhi];
    	radonStruct->dMaxTeta = (long)(radonStruct->originY+radonStruct->heightRec)*radonStruct->cosArray[mPhi] -
                  (long)(radonStruct->originX+radonStruct->widthRec)*radonStruct->sinArray[mPhi];
       }
     shift =  (radonStruct->dMinTeta - radonStruct->dMin)/radonStruct->stepDScale;
     sizeDTeta = (WORD)((radonStruct->dMaxTeta - radonStruct->dMinTeta)/radonStruct->stepDScale + 1L);
     p = radonStruct->pRadonBuffer + (DWORD)mPhi*(DWORD)radonStruct->sizeD;
     for(mD=shift; mD<shift + sizeDTeta; mD++) {
         pRad[mD - shift]  = (long)(*(p + mD));
         distR[mD - shift] = (long)(mD - shift)*(long)radonStruct->stepD + (long)radonStruct->dMinTeta/(long)SCALE;
         }
    *sizeDTR = sizeDTeta;
}
long angleTest(long *pRad, long sizeDR)
{
 int  i, k;
 long diff, max;

   diff = 0;
   for(i=10;i<(int)(sizeDR- 10);i++) {
      max = 0;
      for(k=i-2;k<i+2;k++)
         if(pRad[i] - pRad[k] > max) max =  pRad[i] - pRad[k];
      diff += max;
      }
   return diff;
}


#define SIZED   1000
long   pRadon1[SIZED];
long   distReal1[SIZED];
int         sizeDTReal1;
int         phiReal1;

int getAngle(LPBYTE pFormBuffer, WORD widthF, WORD  heightF, WORD beginX, WORD beginY, WORD widthR, WORD heightR,
		int angMin, int angMax, int angStep){
	 int ret;
	 long * pCosArray;
	 long * pSinArray;
	 int aPhiMin;
	 int aPhiMax;
	 int aPhiM;
//	 long cosM;
//	 long sinM;
	 DWORD wh;
//	 long  scaleMine = SCALE;

	 DWORD sizeWorkAreaAlloc;
	 LPBYTE lpWorkArea;
//	 WORD * pRadonBuffer;
	 int   angle;
	 RADON radonStruct;

	 byteWeights(&radonStruct);
	 radonStruct.stepPhi = angStep;
	 radonStruct.phiMin = angMin;
	 radonStruct.phiMax = angMax;
	 radonStruct.stepD = STEPXG;
	 radonStruct.sizePhi = (WORD)((radonStruct.phiMax - radonStruct.phiMin)/radonStruct.stepPhi + 1);

	 if(radonStruct.sizePhi >= SIZEPHI) return -1;
	 createCosSinTable(&radonStruct);
	 radonStruct.stepDScale = (long)radonStruct.stepD*SCALE;

	 if(radonStruct.phiMin < 0) aPhiMin = -radonStruct.phiMin; else aPhiMin = radonStruct.phiMin;
	 if(radonStruct.phiMax < 0) aPhiMax = -radonStruct.phiMax; else aPhiMax = radonStruct.phiMax;
	 if(aPhiMax > aPhiMin) aPhiM = aPhiMax; else aPhiM = aPhiMin;

//	 cosM = (long)((cos(aPhiM*PI/1800))*SCALE);
//	 sinM = (long)((sin(aPhiM*PI/1800))*SCALE);
	 radonStruct.dMin = -(long)(beginX+widthR);
	 radonStruct.dMax = (long)(beginY+heightR) +
		        (long)(beginX+widthR);
	 radonStruct.dMin *= SCALE;
	 radonStruct.dMax *= SCALE;
	 radonStruct.sizeD     = (int)((radonStruct.dMax - radonStruct.dMin)/radonStruct.stepDScale + 1);
	 radonStruct.stepDHalf = ((double)radonStruct.stepD + 1.0)/2;

	 pCosArray   = &radonStruct.cosArray[0];
	 pSinArray   = &radonStruct.sinArray[0];
	 if(widthF > heightF) wh = (DWORD)widthF;
	                 else wh = (DWORD)heightF;
	 sizeWorkAreaAlloc = 2*(2L*wh*wh/STEPXG)/STEPYG + (LONG)SIZEPHI +10;
	 recalculateAllocBuffer(&sizeWorkAreaAlloc, 300000);
	 lpWorkArea = yl_calloc( "LP13", sizeWorkAreaAlloc, sizeof(BYTE));
	 if(lpWorkArea == NULL) {
	       	return -1;
     }
	 {long kk;
	  DWORD *ptrH;
	  long size = sizeWorkAreaAlloc;
	   ptrH = (DWORD *)lpWorkArea;
	   for(kk=0;kk<size/sizeof(DWORD);kk++) {
	       *ptrH = 0;
	       ptrH++;
        }
     }
	 radonStruct.pRadonBuffer   = (WORD *)(lpWorkArea + 8L);
	 radonStruct.pTicketBuff = pFormBuffer;
	 radonStruct.widthTicket = widthF;
	 radonStruct.heightTicket = heightF;
	 radonStruct.byteWidthTicket = ((radonStruct.widthTicket + 31)/32)*4;
	 radonStruct.originX = ((beginX)/8)*8;
	 radonStruct.originY = beginY;
	 radonStruct.widthRec = widthR;
	 radonStruct.heightRec = heightR;

	  radonStruct.phiMinG  = radonStruct.phiMin;
	  radonStruct.phiMaxG  = radonStruct.phiMax;
	  radonStruct.stepPhiG = radonStruct.stepPhi;

      ret = radTL(&radonStruct);
     if(ret < 0) return(-10000);
	 {
	 long n, nGlob;
	 int i;
	  angle = radonStruct.sizePhi/2;
	  nGlob = 0;
	  for(i=0;i<(int)radonStruct.sizePhi-1;i++) {
	     loadOnePhiL(i,  (long *)pRadon1, (int *)&phiReal1, (long *)distReal1, (int *)&sizeDTReal1, &radonStruct);
	     n = angleTest((long *)pRadon1, sizeDTReal1);
	     if(n > nGlob) {
	        nGlob = n;
	        angle = i;
	        }
	     }
	  angle = radonStruct.phiMin + (int)radonStruct.stepPhi*angle;
		 if(lpWorkArea != NULL)
			 yl_free(lpWorkArea);
//	    angle = getPhi(angle);
	 }
	  return(angle);
}


