/*
 * CommonRec.h
 *
 *  Created on: Sep 7, 2012
 *      Author: Wicek
 */

#ifndef COMMONREC_H_
#define COMMONREC_H_
#include  "../headers/wintypes.h"

WORD mapVersionR, normVersionR;
WORD widthR, heightR, sizeR, deltaR, factorR;
WORD thR;
int recCharacter (BYTE  * pNormBuffer, BYTE  * pDataBuffer, long dataBufferLength, BYTE  * pSubsetMap, short * pResult);
int pictFromNormal (BYTE * pCharBuffer, BYTE * pNormBuffer);


#endif /* COMMONREC_H_ */
