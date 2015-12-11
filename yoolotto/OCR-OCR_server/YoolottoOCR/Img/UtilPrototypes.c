/*
 * UtilPrototypes.c

 *
 *  Created on: Aug 19, 2012
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


int getFees(long m01,  long m1,	long t, long t1, long* pFee0, long* pFee1,	long* pFee2){
	long fee;

	fee = m01 + m1;

	*pFee0 = labs((long)(100L - t1));
	*pFee0 = *pFee0 + labs((long)(100L - t));
	*pFee0 = *pFee0*fee;
	*pFee0 = *pFee0/400L;

	*pFee1 = labs((long)(100L - 2L*t1));
	*pFee1 = *pFee1 + labs((long)(200L - t));
	*pFee1 = *pFee1*fee;
	*pFee1 = *pFee1/400L;

	*pFee2 = labs((long)(200L - t1));
	*pFee2 = *pFee2 + labs((long)(100L - 2L*t));
	*pFee2 = *pFee2*fee;
	*pFee2 = *pFee2/400L;
	return(0);
}

int allignArrays
(LPRADON radonStruct,
		LPBYTE pWork, WORD* p0, WORD* p1, WORD n0, WORD n1, DWORD* pDist, WORD* pAllign, WORD* pPos0, WORD* pPosEnd) {
	int x0, x1;
	int start, end, start0, end0, start1, end1;
	int startM;
	int i0, i1;
	int strip;
	int tgi1;

	long m0, m1, m01;
	long c0, c1, c2;
	long t, t1;
	long fee0, fee1, fee2;
	long feeS0, feeS1;
	long minLast, min;
	int k, k1;

	WORD* p00;
	WORD* p11;
	BYTE ind;
	double coeff;
	BYTE* pl;
	BYTE* pr;

	int* pStart;
	int* pEnd;
	int err;

	DWORD* sa0 = &radonStruct->dwbuff0[0];
	DWORD* sa1 = &radonStruct->dwbuff1[0];
	long s0;
	long s1;

	strip = (int)((long)n0*(long)radonStruct->tgP/100);
//	 printf("  n0= %d, radonStruct->tgP= %d, strip= %d \n",n0, radonStruct->tgP, strip);
	// return 0;


	if(radonStruct->local == 0)
	{pStart = &start0; pEnd = &end0;
	err = recMargin((int*)p0, n0, pStart, pEnd);
	if(err<0) return(err);
	}
	else {start0 = 0; end0 = n0-1;}
	n0 = end0 - start0 + 1;
	p00 = p0 + (long)start0;


	for(i0=0; i0<(int)n0; i0++) radonStruct->a0[i0] = *p00++;

//	 printf("  n0= %d \n",n0);
	// return 0;


	if(radonStruct->local == 0)
	{pStart = &start1; pEnd = &end1;
	err = recMargin((int*)p1, n1, pStart, pEnd);
	if(err<0) return(err);
	}
	else {start1 = 0; end1 = n1-1;}
	n1 = end1 - start1 + 1;
	p11 = p1 + (long)start1;
	/*
 {char buffLog[500];
  saveLog("------------------------------", "\n\n");
  sprintf(buffLog,"  radonStruct->local= %d, n0= %d \n",radonStruct->local, n0);
  saveLog("", buffLog);
  sprintf(buffLog,"  start0= %d, end0= %d, p0= %u, p00= %u, p00-p0= %u \n",start0, end0, (unsigned)p0, (unsigned)p00, (unsigned)(p00-p0));
  saveLog("", buffLog);
  sprintf(buffLog,"  n1= %d, start1= %d, end1= %d, p1= %u, p11= %u, p11-p1= %u \n",n1, start1, end1, (unsigned)p1, (unsigned)p11, (unsigned)(p11-p1));
  saveLog("", buffLog);

  sprintf(buffLog,"  sizeof(radonStruct->a0)= %d, sizeof(radonStruct->a1)= %d \n",sizeof(radonStruct->a0), sizeof(radonStruct->a1));
  saveLog("", buffLog);

// for(i1=0; i1<(int)n1; i1++) {radonStruct->a1[i1] = *p11++;  sprintf(buffLog,"  i1= %d, p11 = %u, p11-p1= %u \n",i1, (unsigned)p11, (unsigned)(p11-p1));saveLog("", buffLog);}
  for(i1=0; i1<(int)n1; i1++) {
  sprintf(buffLog,"  i1= %d, p11 = %u, p11-p1= %u \n",i1, (unsigned)p11, (unsigned)(p11-p1));
  saveLog("", buffLog);
  sprintf(buffLog,"  *p11 = %d, *(p11+1)= %d \n",*p11, *(p11+1));
  saveLog("", buffLog);
  radonStruct->a1[i1] = 1;
  sprintf(buffLog,"  radonStruct->a1[i1] = %d\n",radonStruct->a1[i1]);
  saveLog("", buffLog);

  radonStruct->a1[i1] = *p11++;}

 }
	 */
	for(i1=0; i1<(int)n1; i1++) {radonStruct->a1[i1] = *p11++;}

	pl = pWork;
	for(i1=0; i1<(int)n1; i1++)
	{pr = pl + (long)(n0-1);
	//      if(_FP_SEG(pl) !=  _FP_SEG(pr)) pl = pl + (long)n0;
	radonStruct->pIndex[i1] = pl;
	for(i0=0;i0<(int)n0;i0++) pl[i0] = 10;
	pl = pl + (long)n0;
	}



	sa0[0] = 0;
	for(i0=1; i0<=(int)n0; i0++) sa0[i0] = sa0[i0-1] + radonStruct->a0[i0-1];
	s0 = sa0[n0];
	if(s0 == 0L) m0 = 0L; else m0 = s0/(n0);
	if(radonStruct->local == 1) for(i0=0; i0<=(int)n0; i0++) sa0[i0] = i0*m0;
	sa1[0] = 0;
	for(i1=1; i1<=(int)n1; i1++) sa1[i1] = sa1[i1-1] + radonStruct->a1[i1-1];
	s1 = sa1[n1];
	if(s1 == 0L) m1 = 0L; else m1 = s1/(n1);
	if(radonStruct->local == 1) for(i1=0; i1<=(int)n1; i1++) sa1[i1] = i1*m1;

	if(m0 != 0)  coeff = (double)m1/(double)m0;
	else coeff = 1.0;
	m01 = m1;

	feeS0 = 100L/1;
	feeS1 = 100L/1;

	s0 = (long)((coeff*feeS0*s0)/100);
	for(i0=0; i0<=(int)n0; i0++) {
		radonStruct->a0[i0] = (int)(coeff*radonStruct->a0[i0]);
		sa0[i0] = (long)((coeff*feeS0*sa0[i0])/100);
	}

	s1 = (long)((feeS1*s1)/100);
	for(i1=0; i1<=(int)n1; i1++) {
		sa1[i1] = (long)((feeS1*sa1[i1])/100);
	}

	if((n0 != 0)&&(n1 != 0))
	{t = (100L*n0)/n1; t1 = (100L*n1)/n0;
	}
	else
	{*pDist = 1000;
	return(-1);
	}

	if((t>=200)||(t1>=200))
	{*pDist = 1000;
	return(-1);
	}


	{long* pFee0 = &fee0;
	long* pFee1 = &fee1;
	long* pFee2 = &fee2;

	getFees(m01, m1, t, t1, pFee0, pFee1, pFee2);

	fee0 = *pFee0;
	fee1 = *pFee1;
	fee2 = *pFee2;
	}

	radonStruct->fk = radonStruct->f[0];
	radonStruct->fk1 = radonStruct->f[1];
	for(i0=0; i0<(int)n0; i0++) {
		radonStruct->fk[i0] = FMAX;
		radonStruct->fk1[i0] = FMAX;
	}

	start = 0; end = strip;
	radonStruct->fk =radonStruct->f[0];
	if(end>(int)(n0-1)) end = (int)(n0-1);
	for(i0=end; i0>=start; i0--)
		radonStruct->fk[i0] = dAbs1(radonStruct->a0[i0] - radonStruct->a1[0]) + sa0[i0];

	if(end==(int)(n0-1))
		//      minLast = fk[n0-1] + feeS1*(n1-1);
		minLast = radonStruct->fk[n0-1] + (s1 - sa1[1]);
	else minLast = FMAX;
	x1 = 0;

	start =(int)(n0/n1) - strip; end = (int)(n0/n1) + strip;
	if(start<0) start = 0;
	radonStruct->fk1 = radonStruct->f[0]; radonStruct->fk = radonStruct->f[1];
	radonStruct->pIndex1 = radonStruct->pIndex[1];
	if(end>(int)(n0-1)) end = (int)(n0-1);
	startM = (start>=2 ? start : 2);
	for(i0=end; i0>=startM; i0--)
	{c0 = radonStruct->fk1[i0-1] + fee0;
	c1 = radonStruct->fk1[i0-2] + dAbs1(radonStruct->a0[i0-1] - ((radonStruct->a1[0]+radonStruct->a1[1])>>1)) + fee1;
	if(c1 < c0)
	{radonStruct->fk[i0] = dAbs1(radonStruct->a0[i0] - radonStruct->a1[1]) + c1;
	radonStruct->pIndex1[i0] = 1;
	}
	else
	{radonStruct->fk[i0] = dAbs1(radonStruct->a0[i0] - radonStruct->a1[1]) + c0;
	radonStruct->pIndex1[i0] = 0;
	}
	if(radonStruct->fk[i0] > FMAX) radonStruct->fk[i0] = FMAX;
	}
	if(start<=1)
	{radonStruct->fk[1] = dAbs1(radonStruct->a0[1] - radonStruct->a1[1]) + radonStruct->fk1[0] + fee0;
	if(radonStruct->fk[1] > FMAX) radonStruct->fk[1] = FMAX;
	radonStruct->pIndex1[1] = 0;
	}
	if(start==0)
	{
		radonStruct->fk[0] = dAbs1(radonStruct->a0[0] - radonStruct->a1[1]) + sa1[1];
		if(radonStruct->fk[0] > FMAX) radonStruct->fk[0] = FMAX;
	}
	if(start-1>=0) radonStruct->fk[start-1] = FMAX;
	if(start-2>=0) radonStruct->fk[start-2] = FMAX;

	if(end==(int)(n0-1)) //c2 = fk[n0-1] + feeS1*(n1-2);
	c2 = radonStruct->fk[n0-1] + (s1 - sa1[2]);
	else c2 = FMAX;
	if(c2 < minLast)
	{minLast = c2;
	x1 = 1;
	}

	for(i1=2; i1<(int)n1; i1++)
	{k=i1%2; k1=(i1-1)%2;
	radonStruct->fk1 = radonStruct->f[k1]; radonStruct->fk = radonStruct->f[k];
	radonStruct->pIndexi1 = radonStruct->pIndex[i1];
	tgi1 = (int)((long)n0*(long)i1/n1);
	start = tgi1 - strip; end = tgi1 + strip;
	if(start<0) start = 0;
	if(end>(int)(n0-1)) end = (int)(n0-1);
	startM = (start>=2 ? start : 2);
	for(i0=end; i0>=startM; i0--)
	{c0 = radonStruct->fk1[i0-1] + fee0;
	c1 = radonStruct->fk1[i0-2] + dAbs1(radonStruct->a0[i0-1] - ((radonStruct->a1[i1-1]+radonStruct->a1[i1])>>1)) + fee1;
	c2 = radonStruct->fk[i0-1] + dAbs1(((radonStruct->a0[i0-1]+radonStruct->a0[i0])>>1) - radonStruct->a1[i1-1]) + fee2;
	if(c1 < c0)
	{if(c1 < c2) {min = c1; radonStruct->pIndexi1[i0] = 1;}
	else {min = c2; radonStruct->pIndexi1[i0] = 2;}
	}
	else
	{if(c2 < c0) {min = c2; radonStruct->pIndexi1[i0] = 2;}
	else {min = c0; radonStruct->pIndexi1[i0] = 0;}
	}
	radonStruct->fk[i0] = dAbs1(radonStruct->a0[i0] - radonStruct->a1[i1]) + min;
	if(radonStruct->fk[i0] > FMAX) radonStruct->fk[i0] = FMAX;
	}
	if(start<=1)
	{c0 = radonStruct->fk1[0] + fee0;
	c2 = radonStruct->fk[0] + dAbs1(((radonStruct->a0[0]+radonStruct->a0[1])>>1) - radonStruct->a1[i1-1]) + fee2;
	if(c2 < c0)
	{radonStruct->fk[1] = dAbs1(radonStruct->a0[1] - radonStruct->a1[i1]) + c2;
	radonStruct->pIndexi1[1] = 2;
	}
	else
	{radonStruct->fk[1] = dAbs1(radonStruct->a0[1] - radonStruct->a1[i1]) + c0;
	radonStruct->pIndexi1[1] = 0;
	}
	if(radonStruct->fk[1] > FMAX) radonStruct->fk[1] = FMAX;
	}
	if(start==0)
	{
		radonStruct->fk[0] = dAbs1(radonStruct->a0[0] - radonStruct->a1[i1]) + sa1[i1];
		if(radonStruct->fk[0] > FMAX) radonStruct->fk[0] = FMAX;
	}
	if(start-1>=0) radonStruct->fk[start-1] = FMAX;
	if(start-2>=0) radonStruct->fk[start-2] = FMAX;

	if(end==(int)(n0-1))
		c1 = radonStruct->fk[n0-1] + (s1 - sa1[i1+1]);
	else c1 = FMAX;
	if(c1 < minLast)
	{minLast = c1;
	x1 = i1;
	}
	}

	min = minLast;
	x0 = (int)(n0-1);
	radonStruct->fk = radonStruct->f[(n1-1)%2];
	for(i0=start; i0<(int)(n0-1); i0++)
	{
		c1 = radonStruct->fk[i0] + (s0 - sa0[i0+1]);
		if(c1 < min)  {min = c1; x0 = i0; x1 = n1-1;}
	}

	if(min < FMAX/1000)
	{if(m1 != 0) *pDist = (1000*min)/(m1*n1);
	else *pDist = 1000;
	}
	else *pDist = 1000;

	*pPosEnd = (int)(x0+start0);
	while((x0 > 0)&&(x1 > 0)){
		pAllign[x0+start0] = (int)(x1+start1);
	    ind = radonStruct->pIndex[x1][x0];
	    if(ind == 0) {x0 = x0-1; x1 = x1-1;}
	    if(ind == 1){
	    	x0 = x0-2; x1 = x1-1;
	        if(dAbs1(radonStruct->a0[x0+1] - radonStruct->a1[x1]) < dAbs1(radonStruct->a0[x0+1] - radonStruct->a1[x1+1]))
		       pAllign[x0+1+start0] = (int)(x1+start1);
	        else
		      pAllign[x0+1+start0] = (int)(x1+1+start1);
	    }
	    if(ind == 2) {x0 = x0-1; x1 = x1-2;}
	    if((ind!=0)&&(ind!=1)&&(ind!=2)){
	    	*pDist = 1000;
	        return(-1);
	     }
	}
	pAllign[x0+start0] = (int)(x1+start1);
	*pPos0 = (int)(x0+start0);

	return(0);
}

//#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_ANDROID || __APPLE__ || __linux__ || LINUX || linux || UNIX)
// These functions are not part of the ANSI standard and are not available in the standard
// library of Apple or Linux platforms.
char *strlwr(char *str)
{
	char *c = str;
	while (*c) {
		*c = tolower(*c);
		c++;
	}

	return str;
}

char *strupr(char *str)
{
	char *c = str;
	while (*c) {
		*c = toupper(*c);
		c++;
	}

	return str;
}

int stricmp(const char *s1, const char *s2)
{
	for (;;) {
		if (*s1 != *s2) {
			int c1 = toupper((unsigned char)*s1);
			int c2 = toupper((unsigned char)*s2);

			if (c2 != c1) {
				return c2 > c1 ? -1 : 1;
			}
		} else {
			if (*s1 == '\0') {
				return 0;
			}
		}
		++s1;
		++s2;
	}
}

//#endif

