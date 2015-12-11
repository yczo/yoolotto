/*
 * Field.c
 *
 *  Created on: Aug 22, 2012
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
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include "../headers/logging.h"

#include "../headers/BarCode.h"
#include "../headers/ocr.h"

//extern int imgIndx;
extern int hHistPreProc[H_SIZE_PREPROC];
int testLogoField2(LPTICKETDATA ticketDataPtr, int y1);
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist);
int getxShiftForNumbersField(int *vHist, int start, int d);
int getyShiftForNumbersField(int *vHist, int start, int d);
void verticalLineRemovalField(LPBYTE pTBuffer, int width, int height, int y1, int y2);
void setFieldImageInfo(LPTICKETRESULTS ticketResults, int idxResults, LPTICKETDATA ticketDataPtr);
int getYShiftBottomNumbersField(int *vHist, int dY);
int getLeftSideDateTop(int *vHist);
int getNumbersCnf_CA(LPFIELDSRESULTS lpFieldsResults);
int  postProcessingNumbers_CA(LPTICKETRESULTS ticketResults, int i);
int calculateLines(int *hHist);
int testFoRET_NUM_recognition(LPFIELDDATA field);
//void repairField(LPIMGDATA img, LPTICKETDATA ticketData);

int notDone;
int TrimField (LPTICKETDATA ticketDataPtr){
	int i = 0, j = 0;
	int nofirst = 0;
	int skip    = 0;
	int newline = 0;
	int delim   = 0;
	char ch;

	while (ticketDataPtr->strings.lpResult[i]) {
		ch = ticketDataPtr->strings.lpResult[i];
		delim = (ch == '\r');
		if (ch <= ' ') {
			if (nofirst) {
				skip = 1;
				if (ch == '\r')
					newline = 1;
			}
			i++;
			continue;
		}
		nofirst = 1;

		if (skip) {
			ticketDataPtr->strings.lpResult[j] =
					ticketDataPtr->strings.lpCand1 [j] =
							ticketDataPtr->strings.lpCand2 [j] = (newline) ? '\r' : ' ';
			ticketDataPtr->flConf          [j] = 100;
			memset (&ticketDataPtr->flQuad [j], 0, sizeof (QUADRANGLE));
			j++;
			skip    = 0;
			newline = 0;
		}
		ticketDataPtr->strings.lpResult[j] = ticketDataPtr->strings.lpResult[i];
		ticketDataPtr->strings.lpCand1 [j] = ticketDataPtr->strings.lpCand1 [i];
		ticketDataPtr->strings.lpCand2 [j] = ticketDataPtr->strings.lpCand2 [i];
		ticketDataPtr->flConf          [j] = ticketDataPtr->flConf          [i];
		ticketDataPtr->flQuad          [j] = ticketDataPtr->flQuad          [i];
		i++;
		j++;
	}
	ticketDataPtr->strings.lpResult[j] = 0;
	ticketDataPtr->strings.lpCand1 [j] = 0;
	ticketDataPtr->strings.lpCand2 [j] = 0;
	return delim;
}



int ProcessField (LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPRADON radonStruct, LPTICKETRESULTS ticketResults)
{
	DWORD  offset=0;         // offset to current field info block
	WORD   currentZone;    // number of current zone
	DWORD  protlth = 0;         // length of prototype file
	int delim   = 0;
	LPZONEDATA zone;
	double angleOut = 0;

	BYTE* lpProt = ticketDataPtr->prototypePtr;
	char zeroLine[1] = "";      // zero-string to refer in case of failure
	int  i, fieldNumber;
	int debug;



//	  char bufLog[1000];

	fieldNumber = 1;

	pMemNorm = NULL;
	pMemMomentsFD = NULL;

	notDone = 0;
	trapezoidVer = 0;
	while(fieldNumber<=ticketDataPtr->numFields) {


		if (fieldNumber == 1) {
			offset      = ticketDataPtr->fieldChain;
			currentZone = 0xffff;
			protlth = ticketDataPtr->allocLength;
		}
//		  printf("fieldNumber= %d, offset= %d, protlth= %d \n ", fieldNumber,offset, protlth);
//		  fflush(stdout);
		while  (1) {
			char        name[60];       // name of the field
			LPFIELDDATA field      = (LPFIELDDATA)((LPBYTE)ticketDataPtr + offset);
			BYTE*  descr      = lpProt + field->descr;
			DWORD attrOffset = field->descr + OffFlBuffer +
					*(WORD *)(descr + OffFlSize);
			BYTE*  attr       = lpProt + attrOffset;
			int        firstField;
			FIELD      f;

			WORD logZoneNum  = 0xfff0;
			WORD validation  = 2;
			int failed      = 0;
			int extractOnly = 0;
			if (protlth >= attrOffset + OffFlSubset) {
				logZoneNum  = *(WORD*)(attr + OffFlLogZoneNo);
				validation  = *(WORD*)(attr + OffFlValidation);
				extractOnly = *(WORD*)(attr + OffFlExtractOnly);
			}
			offset = field->next;   // set pointer to the next field

			// reset skip if we are in the next logical zone
			if (currentZone != logZoneNum) {
				currentZone  = logZoneNum;
				firstField   = 1;
			} else
				firstField   = 0;

			for (i = 0; i < 32; i++)
				name[i] = attr[i];
			name[32] = 0;
//			LOGD("ProcessField  name:  %s", name);
			strcpy(field->name, name);

//       if(strcmp(field->name, "DATEN1") != 0 && strcmp(field->name, "DATEA1") != 0)
		 if(strcmp(field->name, "RET_NUM") != 0)
		{// totally skip field

			// extract field

//		  printf("extractTicketsFieldcall \n ");
//		  fflush(stdout);
			zone = &ticketDataPtr->zone[field->zone];  // get zone data block
			if(extractTicketsFieldcall(img, ticketDataPtr, field, radonStruct, ticketResults, name, &angleOut)  < 0)   {

				//                printf ("extractFormsField returns %i\n",
				//                		ticketDataPtr->returnCode);
				failed = 1;                  // set failure flag
				// set field quadrangle from prototype
				for (i = 0; i < sizeof (QUADRANGLE); i++) {
					field->quad.p1X = field->quad.p4X =
							*(WORD*)(descr);
					field->quad.p1Y = field->quad.p2Y =
							*(WORD*)(descr + 2);
					field->quad.p2X = field->quad.p3X =
							field->quad.p1X + *(WORD*)(descr + 4);
					field->quad.p3Y = field->quad.p4Y =
							field->quad.p1Y + *(WORD*)(descr + 6);
				}
			}


//			  printf("after extractTicketsFieldcall;name= %s \n ",name);
//			  printf(" ticketData->returnCode= %d\n", ticketDataPtr->returnCode);
//			  fflush(stdout);
			if(strcmp(name, "LOGO") == 0 && failed == 0){
				int yShift;
				//	char buff[500];
				int h;
				ticketResults->yShift = 0;
				yShift = testLogoField2(ticketDataPtr, field->quad.p1Y);
				if(abs(yShift) > 20) {
					field->quad.p1Y -= yShift;
					field->quad.p2Y -= yShift;
					field->quad.p3Y -= yShift;
					field->quad.p4Y -= yShift;
					if(field->quad.p1Y<8) field->quad.p1Y=8;if(field->quad.p2Y<8) field->quad.p2Y=8;
					if(field->quad.p3Y<8) field->quad.p3Y=8;if(field->quad.p4Y<8) field->quad.p4Y=8;
					h = img->bi.biHeight-8;
					if(field->quad.p1Y>h) field->quad.p1Y=h;if(field->quad.p2Y>h) field->quad.p2Y=h;
					if(field->quad.p3Y>h) field->quad.p3Y=h;if(field->quad.p4Y>h) field->quad.p4Y=h;

					extractFieldAgain(img, field, ticketDataPtr, name, &angleOut);
					ticketResults->yShift = yShift;
				}

				extractOnly = 1;
				ticketResults->type = getLogoType(ticketDataPtr);
			}

			if (!failed) {
				initLineQuads(ticketDataPtr, field, angleOut);
				if (extractOnly)
					failed = 0;
			 	//                  printf ("Recognition not requested\n");
				else{
					// recognize field
//					LOGD ("Before RecognitionCall");
//					    if(strcmp(name, "NUMBERS") == 0){
//					    	repairField(img, ticketDataPtr);
//						        	saveField(img, ticketDataPtr);
//					        }
					if (RecognitionCall (img, ticketDataPtr)<0) {                        // if failed:
						failed = 1;                              // set failure flag
					}
//					LOGD("recognition returns %i", ticketDataPtr->returnCode);
				}
			}

//			  if(strcmp(name, "RET_NUM") == 0){
//						  if(strcmp(name, "LOGO3") == 0){
//			     if(strcmp(name, "DATEN1") == 0 || strcmp(name, "DATEA1") == 0){
//		     if(strcmp(name, "DATEN") == 0 || strcmp(name, "DATEA") == 0){
//		     if(strcmp(name, "NUMBERS") == 0 || strcmp(name, "NUMBERS1") == 0){
//			        	saveField(img, ticketDataPtr);
//			        }


			// set strings parameter to empty
			if (extractOnly || failed) {
				ticketDataPtr->strings.lpResult = zeroLine;
				ticketDataPtr->strings.lpCand1  = zeroLine;
				ticketDataPtr->strings.lpCand2  = zeroLine;
				ticketDataPtr->strings.lpReject = zeroLine;
				//        	return -1;
			}

			delim = TrimField (ticketDataPtr);
			// get rid of compiler warnings
			debug = delim;
			delim = debug;
			debug = firstField;
			debug = validation;
			if (ticketDataPtr->strings.lpResult[0]) {
/*
				logInfo_recalculateCharsPoints(img, &field->quad, &ticketDataPtr->strings, ticketDataPtr->flQuad);
	 if(strcmp(name, "NUMBERS") == 0){
			    char  fName[500];
			    strcpy(fName, "LineF_18.bmp");
			    printf("  name= %s\n", fName);
			    fflush(stdout);
//				extractLineAndSave(img->ptr, 527, 871,
//						           993, 1022, fName);
				extractLineAndSave(ticketDataPtr->fieldBMPPtr, 527, 871,
						           771, 802, fName);


			    }
*/

				recalculateCharsPoints(img, &field->quad, &ticketDataPtr->strings, ticketDataPtr->flQuad);

			}
//LOGD("3. ProcessField(); size of workAreaPtr:  %ld, field name:  %s\n", ticketDataPtr->workArea.size, field->name);
			// see if this and next fields must be skipped

			// convert to uppercase
			strupr  (ticketDataPtr->strings.lpResult);
			strupr  (ticketDataPtr->strings.lpCand1);
			strupr  (ticketDataPtr->strings.lpCand2);

			// save recognized field
			f.result = ticketDataPtr->strings.lpResult;
			f.cand1  = ticketDataPtr->strings.lpCand1;
			f.cand2  = ticketDataPtr->strings.lpCand2;
			f.conf   = ticketDataPtr->flConf;
			f.quad   = ticketDataPtr->flQuad;
//LOGD("4. ProcessField(); size of workAreaPtr:  %ld, field name:  %s\n", ticketDataPtr->workArea.size, field->name);
			if (f.result[0]) {                  // show string

// if(strcmp(name, "NUMBERS") == 0){
//	  printf("------- field name= %s \n",field->name);
//	  printf("%s\n",f.result);
// }
/*
				LOGD ("recognition:-----------");
				LOGD (f.result);
				LOGD ("-----------------------\n");

				int iCnf;
				for(iCnf = 0;iCnf<strlen(f.result);iCnf++){
					if(f.result[iCnf] != 13){
						LOGD("%c,(%d)", f.result[iCnf], f.conf[iCnf]);
					}
					else{
						LOGD("\n");
					}

				}
*/
			}
//		 	   sprintf(bufLog,"111111111111111111111-------!!!!!!!!!!!!!!!!!! ProcessField !!!!!!!! \n");
//		       saveLog(bufLog, "");

			copyFieldResults(ticketResults, &f, fieldNumber-1, field->name, ticketDataPtr);
// repeat recognition for Date if RET_NUM is out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if(strcmp(name, "DATEN") == 0 || strcmp(name, "DATEA") == 0){
				if(testFoRET_NUM_recognition(field)){
//printf("------------------------- repeat recognition for Date if RET_NUM is out -------------------------\n");
//fflush(stdout);

//    	saveField(img, ticketDataPtr);
					   if(strcmp(name, "DATEN") == 0) strcpy(name, "RET_NUMN");
					   if(strcmp(name, "DATEA") == 0) strcpy(name, "RET_NUMA");
					   strcpy(field->name, name);
					   fieldNumber++;


						zone = &ticketDataPtr->zone[field->zone];  // get zone data block
						if(extractTicketsFieldcall(img, ticketDataPtr, field, radonStruct, ticketResults, name, &angleOut)  < 0)   {

							//                printf ("extractFormsField returns %i\n",
							//                		ticketDataPtr->returnCode);
							failed = 1;                  // set failure flag
							// set field quadrangle from prototype
							for (i = 0; i < sizeof (QUADRANGLE); i++) {
								field->quad.p1X = field->quad.p4X =
										*(WORD*)(descr);
								field->quad.p1Y = field->quad.p2Y =
										*(WORD*)(descr + 2);
								field->quad.p2X = field->quad.p3X =
										field->quad.p1X + *(WORD*)(descr + 4);
								field->quad.p3Y = field->quad.p4Y =
										field->quad.p1Y + *(WORD*)(descr + 6);
							}
						}

//	  	saveField(img, ticketDataPtr);


					   if (!failed) {
						   initLineQuads(ticketDataPtr, field, angleOut);
						   if (extractOnly)
							   failed = 0;
						   else{
							   if (RecognitionCall (img, ticketDataPtr)<0) {                        // if failed:
								   failed = 1;                              // set failure flag

							   }
						   }
					   }
					   if (extractOnly || failed) {
						  ticketDataPtr->strings.lpResult = zeroLine;
						  ticketDataPtr->strings.lpCand1  = zeroLine;
						  ticketDataPtr->strings.lpCand2  = zeroLine;
						  ticketDataPtr->strings.lpReject = zeroLine;
						//        	return -1;
					   }
				       delim = TrimField (ticketDataPtr);
					   if (ticketDataPtr->strings.lpResult[0]) {
						  recalculateCharsPoints(img, &field->quad, &ticketDataPtr->strings, ticketDataPtr->flQuad);
					   }
						// convert to uppercase
						   strupr  (ticketDataPtr->strings.lpResult);
						   strupr  (ticketDataPtr->strings.lpCand1);
						   strupr  (ticketDataPtr->strings.lpCand2);

						// save recognized field
						   f.result = ticketDataPtr->strings.lpResult;
						   f.cand1  = ticketDataPtr->strings.lpCand1;
						   f.cand2  = ticketDataPtr->strings.lpCand2;
						   f.conf   = ticketDataPtr->flConf;
						   f.quad   = ticketDataPtr->flQuad;

						   copyFieldResults(ticketResults, &f, fieldNumber-1, field->name, ticketDataPtr);

				}
			}

//			if(US_State == 2 && (ticketTypeInt < 3 || ticketTypeInt == 5) &&strcmp(name, "NUMBERS") == 0){ temporary !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if(US_State == 2 && (ticketTypeInt < -100) &&strcmp(name, "NUMBERS") == 0){
//			if(US_State == 2 && (ticketTypeInt < 3) &&strcmp(name, "NUMBERS") == 0){
				int pen = 0;
//			  printField(&ticketResults->fieldsResults[fieldNumber-1]);
			  ticketResults->status = postProcessingNumbers_CA(ticketResults, fieldNumber-1);

			  pen = getNumbersCnf_CA(&ticketResults->fieldsResults[fieldNumber-1]);
//printf ("------------------------------------pen= %d\n", pen);
//			  printField(&ticketResults->fieldsResults[fieldNumber-1]);

			    if(pen > 0) {
			       trapezoidVer = 1;
				   if (!failed) {
					   initLineQuads(ticketDataPtr, field, angleOut);
					   if (extractOnly)
						   failed = 0;
					   else{
						   if (RecognitionCall (img, ticketDataPtr)<0) {                        // if failed:
							   failed = 1;                              // set failure flag

						   }
					   }
				   }
				   if (extractOnly || failed) {
					  ticketDataPtr->strings.lpResult = zeroLine;
					  ticketDataPtr->strings.lpCand1  = zeroLine;
					  ticketDataPtr->strings.lpCand2  = zeroLine;
					  ticketDataPtr->strings.lpReject = zeroLine;
					//        	return -1;
				   }
                   if(trapezoidVer == 1) {
				      delim = TrimField (ticketDataPtr);
				      if (ticketDataPtr->strings.lpResult[0]) {
					     recalculateCharsPoints(img, &field->quad, &ticketDataPtr->strings, ticketDataPtr->flQuad);
				      }
					// convert to uppercase
					   strupr  (ticketDataPtr->strings.lpResult);
					   strupr  (ticketDataPtr->strings.lpCand1);
					   strupr  (ticketDataPtr->strings.lpCand2);

					// save recognized field
					   f.result = ticketDataPtr->strings.lpResult;
					   f.cand1  = ticketDataPtr->strings.lpCand1;
					   f.cand2  = ticketDataPtr->strings.lpCand2;
					   f.conf   = ticketDataPtr->flConf;
					   f.quad   = ticketDataPtr->flQuad;

					   copyFieldResults(ticketResults, &f, fieldNumber-1, field->name, ticketDataPtr);
                   }
			    } // if(pen > 0) {

			}


		}// totally skip field
			fieldNumber++;
//LOGD("5. ProcessField(); size of workAreaPtr:  %ld, field name:  %s\n", ticketDataPtr->workArea.size, field->name);
			if (!offset) break;     // if this is the last field, done

			// look into next field's parameters
			field      = (LPFIELDDATA)((LPBYTE)ticketDataPtr + offset);
			descr      = ticketDataPtr->prototypePtr + field->descr;
			attrOffset = field->descr + OffFlBuffer +
					*(WORD*)(descr + OffFlSize);
			if (protlth >= attrOffset + OffFlSubset) {
				attr       = ticketDataPtr->prototypePtr + attrOffset;
				logZoneNum = *(WORD*)(attr + OffFlLogZoneNo);
			}
			if (currentZone != logZoneNum) break;   // if end of zone, done
//LOGD("6. ProcessField(); size of workAreaPtr:  %ld, field name:  %s\n", ticketDataPtr->workArea.size, field->name);
		}
//LOGD("7. ProcessField(); size of workAreaPtr:  %ld\n", ticketDataPtr->workArea.size);

	}
	if(pMemNorm != NULL) {yl_free(pMemNorm); pMemNorm = NULL;}
	if(pMemMomentsFD != NULL) {yl_free(pMemMomentsFD); pMemMomentsFD = NULL;}
// get rid of compiler warnings
	{
	  LPZONEDATA zz = zone;
	     zone = zz;

	}

	return (offset);
}
/*
int  logInfo_recalculateCharsPoints(LPIMGDATA pImg, LPQUADRANGLE pFieldQuad,
		LPSTRINGS pResults, LPQUADRANGLE pPattern){
	WORD wWidthT;
	WORD wHeightT;
	WORD i;

	LPSTR lpResultBuffer;

	int origX0;
	int origY0;
	int origX1;
	int origY1;
	int xOrig;
	int yOrig;
	int xCorn;
	int yCorn;
	int x;
	int y;
	int xn;
	int yn;
	long xl;
	long yl;

	int dx;
	int dy;
	double r;

	long cosTeta;
	long sinTeta;
	LPQUADRANGLE pTmp;

	char * resultTmp;
	char buffLog[500];
	int line;

	wWidthT = pImg ->bi.biWidth;
	wHeightT = pImg ->bi.biHeight;

	origX0 = (int)(pFieldQuad ->p1X);
	origY0 = (int)(pFieldQuad -> p1Y);
	origX1 = EXTRACTMARGIN;
	origY1 = EXTRACTMARGIN;

	dx = (int)(pFieldQuad -> p2X) - origX0;
	dy = (int)(pFieldQuad -> p2Y) - origY0;
	r = sqrt((double)((long)dx*(long)dx + (long)dy*(long)dy));
	if(r != 0.0)
	{cosTeta = (long)((dx/r)*SCALE);
	sinTeta = (long)((dy/r)*SCALE);
	}
	else
	{cosTeta = SCALE;
	sinTeta = 0L;
	}
    sprintf(buffLog," cosTeta= %d, sinTeta= %d\n", cosTeta, sinTeta);
    saveLog("", buffLog);

    sprintf(buffLog," origX0= %d, origY0= %d, origX1= %d, origY1= %d\n", origX0, origY0, origX1, origY1);
    saveLog("", buffLog);
    sprintf(buffLog," dx= %d, dy= %d\n", dx, dy);
    saveLog("", buffLog);
    sprintf(buffLog," pFieldQuad -> p1X= %d, pFieldQuad -> p2X= %d, pFieldQuad -> p1Y= %d, pFieldQuad -> p2Y= %d\n", pFieldQuad -> p1X, pFieldQuad -> p2X, pFieldQuad -> p2Y, pFieldQuad -> p3Y);
    saveLog("", buffLog);


	lpResultBuffer = pResults -> lpReject;

	resultTmp = pResults ->lpResult;


	line = 0;
	for(i=0; i<1000; i++){
		if(*(lpResultBuffer+(long)i) == 0) break;

//    sprintf(buffLog," i= %d\n", i);
//    saveLog("", buffLog);


	pTmp = pPattern+(DWORD)i;

  if(line == 18) {
    sprintf(buffLog," resultTmp[i]= %d, resultTmp[i]= %c\n", resultTmp[i], resultTmp[i]);
    saveLog("", buffLog);
  }


	xOrig = (int)(pTmp -> p1X);
	yOrig = (int)(pTmp -> p1Y);
	xCorn = (int)(pTmp -> p3X);
	yCorn = (int)(pTmp -> p3Y);

	x = xOrig-origX1; y = yOrig-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;

//	(pPattern+(DWORD)i) -> p1X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
//	(pPattern+(DWORD)i) -> p1Y = (WORD)yn;

	  if(line == 18) {
	    sprintf(buffLog," p1X= %d, p1Y= %d\n", xn, yn);
	    saveLog("", buffLog);
	  }


	x = xCorn-origX1; y = yOrig-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
//	(pPattern+(DWORD)i) -> p2X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
//	(pPattern+(DWORD)i) -> p2Y = (WORD)yn;

	  if(line == 18) {
	    sprintf(buffLog," p2X= %d, p2Y= %d\n", xn, yn);
	    saveLog("", buffLog);
	  }


	x = xCorn-origX1; y = yCorn-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
//	(pPattern+(DWORD)i) -> p3X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
//	(pPattern+(DWORD)i) -> p3Y = (WORD)yn;

	  if(line == 18) {
	    sprintf(buffLog," p3X= %d, p3Y= %d\n", xn, yn);
	    saveLog("", buffLog);
	  }

	x = xOrig-origX1; y = yCorn-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
//	(pPattern+(DWORD)i) -> p4X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
//	(pPattern+(DWORD)i) -> p4Y = (WORD)yn;

	  if(line == 18) {
	    sprintf(buffLog," p4X= %d, p4Y= %d\n", xn, yn);
	    saveLog("", buffLog);
	  }

    if(resultTmp[i] == 13) line++;
	}

	return(0);
}
*/
int  recalculateCharsPoints(LPIMGDATA pImg, LPQUADRANGLE pFieldQuad,
		LPSTRINGS pResults, LPQUADRANGLE pPattern){
	WORD wWidthT;
	WORD wHeightT;
	WORD i;

	LPSTR lpResultBuffer;

	int origX0;
	int origY0;
	int origX1;
	int origY1;
	int xOrig;
	int yOrig;
	int xCorn;
	int yCorn;
	int x;
	int y;
	int xn;
	int yn;
	long xl;
	long yl;

	int dx;
	int dy;
	double r;

	long cosTeta;
	long sinTeta;
	LPQUADRANGLE pTmp;

	wWidthT = pImg ->bi.biWidth;
	wHeightT = pImg ->bi.biHeight;

	origX0 = (int)(pFieldQuad ->p1X);
	origY0 = (int)(pFieldQuad -> p1Y);
	origX1 = EXTRACTMARGIN;
	origY1 = EXTRACTMARGIN;

	dx = (int)(pFieldQuad -> p2X) - origX0;
	dy = (int)(pFieldQuad -> p2Y) - origY0;
	r = sqrt((double)((long)dx*(long)dx + (long)dy*(long)dy));
	if(r != 0.0)
	{cosTeta = (long)((dx/r)*SCALE);
	sinTeta = (long)((dy/r)*SCALE);
	}
	else
	{cosTeta = SCALE;
	sinTeta = 0L;
	}


	lpResultBuffer = pResults -> lpReject;

	for(i=0; i<1000; i++)
	{if(*(lpResultBuffer+(long)i) == 0) break;


	pTmp = pPattern+(DWORD)i;

	xOrig = (int)(pTmp -> p1X);
	yOrig = (int)(pTmp -> p1Y);
	xCorn = (int)(pTmp -> p3X);
	yCorn = (int)(pTmp -> p3Y);

	x = xOrig-origX1; y = yOrig-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
	(pPattern+(DWORD)i) -> p1X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
	(pPattern+(DWORD)i) -> p1Y = (WORD)yn;

	x = xCorn-origX1; y = yOrig-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
	(pPattern+(DWORD)i) -> p2X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
	(pPattern+(DWORD)i) -> p2Y = (WORD)yn;

	x = xCorn-origX1; y = yCorn-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
	(pPattern+(DWORD)i) -> p3X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
	(pPattern+(DWORD)i) -> p3Y = (WORD)yn;

	x = xOrig-origX1; y = yCorn-origY1;
	xl = x*cosTeta - y*sinTeta;
	xn = origX0 + (int)(xl>>SCALEEXP);
	if(xn < 0) xn = 0;
	if(xn > (int)wWidthT - 1) xn = (int)wWidthT - 1;
	(pPattern+(DWORD)i) -> p4X = (WORD)xn;
	yl = x*sinTeta + y*cosTeta;
	yn = origY0 + (int)(yl>>SCALEEXP);
	if(yn < 0) yn = 0;
	if(yn > (int)wHeightT - 1) yn = (int)wHeightT - 1;
	(pPattern+(DWORD)i) -> p4Y = (WORD)yn;
	}

	return(0);
}


int extractTicketsFieldcall(LPIMGDATA img, LPTICKETDATA ticketDataPtr, LPFIELDDATA field, LPRADON radonStruct,
		LPTICKETRESULTS ticketResults, char * fName, double *angleOut){
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;

	BYTE* lpProt;

	IMAGEH       fieldH;
	LPDWORD      lpImage;
	DWORD        offBits;
	BYTE*        descr;
	LPZONEDATA   zone;
	int        err;

	lpProt = ticketDataPtr->prototypePtr;
	descr      = lpProt + field->descr;
	zone = &ticketDataPtr->zone[field->zone];

	ticketDataPtr->fieldAddress = NULL;

	if (zone->ptrZoneStruct == NULL) {              // if no zone map:
		if (zone->iAngle || !createZoneStructCall (img, ticketDataPtr, field->zone, radonStruct))
			return -10;     // zone map cannot be obtained
	}

	//printf("------------fieldH.himgPtr= %u \n",  (unsigned int)fieldH.himgPtr.ptr);
	//       if(imgIndx == 2)
	//          	return -1;
	fieldH.himgPtr.ptr = NULL;
	fieldH.himgPtr.size = 0;

	err = 0;
	fieldH.himgPtr.ptr = yl_calloc("F1", 8, sizeof(BYTE));
	fieldH.himgPtr.size = 8;

	//        err = reallocMemoryBuffer (&fieldH.himgPtr, 8);
	if (err < 0 || fieldH.himgPtr.ptr == NULL) {
		return -11;     // out of memory
	}

	offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	lpImage = (LPDWORD) fieldH.himgPtr.ptr;
	*lpImage = 0;
	*(lpImage + 1) = offBits;


	ticketDataPtr->returnCode = extractTicketsField(img, descr, *(LPWORD)(lpProt + OffFdDpiX), *(LPWORD)(lpProt + OffFdDpiY),
			zone->ptrZoneStruct, zone->iAngle, &field->quad, &fieldH, ticketResults, fName, angleOut);

	if (ticketDataPtr->returnCode < 0) {
		freeh (&fieldH.himgPtr);
		//            fieldH.himgPtr.ptr = NULL;
		//            field->ptrField = NULL;  // ??????????? check it up  !!!!!!!!!!!!!!!!!
		return -12;
	}

	{//	LPBYTE ptr = fieldH.himgPtr.ptr;
		//        	LPBYTE biPtr = ptr + BFOOFBITSOFF+4;
		// fill in BITMAPFILEHEADER


		bf = (LPBITMAPFILEHEADER) fieldH.himgPtr.ptr;
		//	    bf->bfType= 0x424d;
		//        bf->bfSize =  offBits + (((fieldH.wWidth /** biBitCount*/ + 31) /32) * 4) * fieldH.wHeight;
		//	    bf->bfReserved1 = 0;
		//	    bf->bfReserved2 = 0;
		//	    bf->bfOffBits =  offBits;
		bf->bfType[0] = 0x42;
		bf->bfType[1] = 0x4d;
		set4Bytes(bf->bfSize, offBits + (((fieldH.wWidth /** biBitCount*/ + 31) /32) * 4) * fieldH.wHeight);
		bf->bfReserved1[0] = 0;
		bf->bfReserved1[1] = 0;
		set4Bytes(bf->bfOffBits, offBits);


		// fill in BITMAPINFOHEADER

		bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
		set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
		set4Bytes(bi->biWidth, fieldH.wWidth);
		set4Bytes(bi->biHeight, fieldH.wHeight);
		bi->biPlanes[0] = 1;
		bi->biPlanes[1] = 0;
		bi->biBitCount[0]= 1;
		bi->biBitCount[1]= 0;
		set4Bytes(bi->biCompression, BI_RGB);
		set4Bytes(bi->biSizeImage, 0);
		set4Bytes(bi->biXPelsPerMeter, fieldH.wDpiX);
		set4Bytes(bi->biYPelsPerMeter, fieldH.wDpiY);
		set4Bytes(bi->biClrUsed, 0);
		set4Bytes(bi->biClrImportant, 0);


		// generate pair of RGBQUADs (black and white)
		qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
		*(LPDWORD)qd       = 0x00000000;
		*(LPDWORD)(qd + 1) = 0x00ffffff;

	}

	if(ticketDataPtr->fieldBMPPtr)
		free (ticketDataPtr->fieldBMPPtr);

	ticketDataPtr->fieldBMPPtr = fieldH.himgPtr.ptr;


	ticketDataPtr->fieldAddress = field;

	return 0;

}

void setField_RET_NUM(WORD *p1X, WORD *p1Y, WORD *p2X, WORD *p2Y, WORD *p3X, WORD *p3Y, WORD *p4X, WORD *p4Y, int widthT, int heightT){
    int h_t;

     switch(US_State){
     case 0:
     case 1:
    	 h_t = *p1Y/3;
//    	 h_t = 0;
    	 *p1Y = h_t; *p2Y = h_t;
//printf("*p1X= %d, *p3X= %d, widthT-100= %d\n",*p1X, *p3X, widthT-100);
//    	 if(*p1X < 100)	 *p1X += 100;
//    	 if(*p4X < 100)	 *p4X += 100;
//    	 if(*p2X > widthT-100)	 *p2X -= 100;
//    	 if(*p3X > widthT-100)	 *p3X -= 100;

    	 break;
     default:
    	 break;
     }
     if(*p1Y < 10) *p1Y = 10;
     if(*p2Y < 10) *p2Y = 10;
//     printf("*p1X= %d, *p2X= %d\n",*p1X, *p2X);
//     printf("*p1Y= %d, *p4Y= %d\n",*p1Y, *p4Y);
//     if(*p3Y > heightT-10) *p3Y = heightT-10;
//     if(*p4Y > heightT-10) *p4Y = heightT-10;

}

int extractTicketsField(LPIMGDATA pImg, LPBYTE lpFieldDescr, WORD wDpiX0, WORD wDpiY0, LPBYTE pZoneStruct,
		int iAngle,  LPQUADRANGLE pFieldQuad, IMAGEH* pFieldH, LPTICKETRESULTS ticketResults, char *fName,
		double *angleOut){


	// HGLOBAL hgZoneStruct;
	BYTE* lpZoneStruct;

	WORD wDpiX;
	WORD wDpiY;

	int err;

	WORD originX0;
	WORD originY0;

	WORD origX0;
	WORD origY0;
	WORD cornX0;
	WORD cornY0;

	// HGLOBAL hgBuffer;
	LPBYTE pBuffer;

	WORD * pwBuffer;
	DWORD sizeBuffer;

	WORD* pwidth;
	WORD* pheight;
	WORD width;
	WORD height;

	WORD widthRec0;
	WORD heightRec0;

	WORD* pwFieldDescr;

	RECT fieldRect0;
	RECT * pFieldRect0;
	BYTE  * lpFieldBuffer0;
	WORD sizeFieldBuffer0;

	WORD p1X;
	WORD p1Y;
	WORD p2X;
	WORD p2Y;
	WORD p3X;
	WORD p3Y;
	WORD p4X;
	WORD p4Y;

	long shift;

	WORD extractMargin;
	int angleFlag = 1;



	pwidth = &width;
	pheight = &height;

	pTBuffer   = pImg ->pixPtr;
	widthT     = pImg -> bi.biWidth;
	heightT    = pImg -> bi.biHeight;
	wDpiX      = pImg -> bi.biXPelsPerMeter;
	wDpiY      = pImg -> bi.biYPelsPerMeter;


	// printf("----widthT= %d, heightT= %d \n",  widthT,heightT);

	if(((int)wDpiX < 80)||((int)wDpiX > 420)) return(-112);
	if(((int)wDpiY < 80)||((int)wDpiY > 420)) return(-112);

	byteWidthT = ((widthT + 31)/32)*4;

	pwFieldDescr = (WORD *)lpFieldDescr;
	originX0       = *(pwFieldDescr+1L);
	originY0       = *(pwFieldDescr+2L);
	widthRec0      = *(pwFieldDescr+3L);
	heightRec0     = *(pwFieldDescr+4L);

	if(((int)wDpiX0 < 80)||((int)wDpiX0 > 420)) return(-112);
	if(((int)wDpiY0 < 80)||((int)wDpiY0 > 420)) return(-112);

	if((originX0 != 0)||(originY0 != 0)||(widthRec0 != 5)||(heightRec0 != 5))
	{

		//  hgZoneStruct = *phgZoneStruct;
		lpZoneStruct = pZoneStruct + 8;

		pFieldRect0 = &fieldRect0;

		origX0 = originX0-FIELDMARGIN;
		origY0 = originY0-FIELDMARGIN;
		cornX0 = originX0+widthRec0+FIELDMARGIN;
		cornY0 = originY0+heightRec0+FIELDMARGIN;

		if((int)origX0 < 0) origX0 = 0;
		if((int)origY0 < 0) origY0 = 0;
		if((int)cornX0 > (int)widthT - 1) cornX0 = widthT - 1;
		if((int)cornY0 > (int)heightT - 1) cornY0 = heightT - 1;

		pFieldRect0 -> left = origX0;
		pFieldRect0 -> top = origY0;
		pFieldRect0 -> right = cornX0;
		pFieldRect0 -> bottom = cornY0;

		sizeFieldBuffer0 = *(pwFieldDescr+5L);

		if(sizeFieldBuffer0 != 0)
		{shift = 6L;
		lpFieldBuffer0 = (BYTE *)(pwFieldDescr+shift);

		err = removeFieldBack
				(pTBuffer, widthT, heightT,
						iAngle, lpZoneStruct,
						pFieldRect0, lpFieldBuffer0
				);

		if(err < 0) { return(err);}
		}

		pFieldQuad -> p1X = originX0;
		pFieldQuad -> p1Y = originY0;
		pFieldQuad -> p2X = originX0+widthRec0;
		pFieldQuad -> p2Y = originY0;
		pFieldQuad -> p3X = originX0+widthRec0;
		pFieldQuad -> p3Y = originY0+heightRec0;
		pFieldQuad -> p4X = originX0;
		pFieldQuad -> p4Y = originY0+heightRec0;

		err =
				mapQ(pFieldQuad, iAngle, lpZoneStruct, 0);
		if(err < 0) { return(err);}

		//  GlobalUnlock(hgZoneStruct);

		p1X = (pFieldQuad -> p1X);
		if((int)p1X < 0) p1X = 0;
		p1Y = (pFieldQuad -> p1Y);
		if((int)p1Y < 0) p1Y = 0;
		p2X = (pFieldQuad -> p2X);
		if(p2X > widthT - 1) p2X = widthT - 1;
		p2Y = (pFieldQuad -> p2Y);
		if((int)p2Y < 0) p2Y = 0;
		p3X = (pFieldQuad -> p3X);
		if(p3X > widthT - 1) p3X = widthT - 1;
		p3Y = (pFieldQuad -> p3Y);
		if(p3Y > heightT - 1) p3Y = heightT - 1;
		p4X = (pFieldQuad -> p4X);
		if((int)p4X < 0) p4X = 0;
		p4Y = (pFieldQuad -> p4Y);
		if(p4Y > heightT - 1) p4Y = heightT - 1;

		shift = 12L + sizeFieldBuffer0;

		if(*(lpFieldDescr + shift) == 94)      // rotates field on +90 degrees ('^')
		{WORD tx, ty;                       //
		tx = p4X;                          //
		ty = p4Y;                          //
		p4X = p3X;                         //
		p4Y = p3Y;                         //
		p3X = p2X;                         //
		p3Y = p2Y;                         //
		p2X = p1X;                         //
		p2Y = p1Y;                         //
		p1X = tx;                          //
		p1Y = ty;                          //
		}


		if(*(lpFieldDescr + shift) == '<')      // rotates field on -90 degrees
		{WORD tx, ty;                       //
		tx = p1X;                          //
		ty = p1Y;                          //
		p1X = p2X;                         //
		p1Y = p2Y;                         //
		p2X = p3X;                         //
		p2Y = p3Y;                         //
		p3X = p4X;                         //
		p3Y = p4Y;                         //
		p4X = tx;                          //
		p4Y = ty;                          //
		}


		if(*(lpFieldDescr + shift) == '>')      // rotates field on +90 degrees
		{WORD tx, ty;                       //
		tx = p4X;                          //
		ty = p4Y;                          //
		p4X = p3X;                         //
		p4Y = p3Y;                         //
		p3X = p2X;                         //
		p3Y = p2Y;                         //
		p2X = p1X;                         //
		p2Y = p1Y;                         //
		p1X = tx;                          //
		p1Y = ty;                          //
		}

		//  hgBuffer = pFieldH -> hgBuffer;
		pBuffer = pFieldH->himgPtr.ptr;

		pwidth = &width;
		pheight = &height;

		extractMargin = EXTRACTMARGIN;
		{
			HLPBYTE hpFieldH;
			LPBYTE dPtr;
			hpFieldH.ptr = pFieldH->himgPtr.ptr;
			dPtr = hpFieldH.ptr;hpFieldH.ptr = dPtr;
			if(  ticketResults->yShift > 0){
				p1Y -= ticketResults->yShift;
				p2Y -= ticketResults->yShift;
				if(p1Y < 8) p1Y = 8;
				if(p2Y < 8) p2Y = 8;
				if(p1Y > heightT-8) p1Y = heightT-8;
				if(p2Y > heightT-8) p2Y = heightT-8;
			}
			if(  ticketResults->yShift < 0){
				p3Y -= ticketResults->yShift;
				p4Y -= ticketResults->yShift;
				if(p3Y < 8) p3Y = 8;
				if(p4Y < 8) p4Y = 8;
				if(p3Y > heightT-8) p3Y = heightT-8;
				if(p4Y > heightT-8) p4Y = heightT-8;
			}

			if(strcmp(fName, "DATEN1") == 0 || strcmp(fName, "DATEA1") == 0){
			   if(p1Y > 120 && p2Y > 120) {p1Y -= 40; p2Y -= 40;}
			   if(p1X > 220 && p4X > 220) {p1X -= 20; p4X -= 20;}
//		       printf(" fName= %s,  p1X = %d  \n", fName, p1X);
			   {
				int vHist[widthT/2];
				WORD byteWidthHist = ((widthT + 31)/32)*4;
				int leftX = 0;
//					  int i;
                if(p1X < widthT/2) {
				   histVPtrPix(pTBuffer, byteWidthHist, heightT, widthT/2, p1X, widthT/2, p1Y, p4Y, vHist);
				   leftX = getLeftSideDateTop(vHist);
				   p1X += leftX;
				   p4X += leftX;
//				   printf("   leftX= %d \n", leftX);
//				    for(i=0;i<vHist[0];i++){
//					       printf("   i= %d, vHist= %d \n", i, vHist[i]);
//				     }

			     }

			   }
			}


			if(strcmp(fName, "NUMBERS") == 0 || strcmp(fName, "NUMBERS1") == 0){
				if(p1X > 200){
					// x shift
					int h_size = 400, xShift;
					int ddTest = 100;
					int vHist[h_size];
					WORD byteWidthHist = ((widthT + 31)/32)*4;
					//	  int i;

					histVPtrPix(pTBuffer, byteWidthHist, heightT, h_size, p1X-ddTest, p1X+h_size-ddTest, p1Y, p4Y, vHist);
					xShift = getxShiftForNumbersField(vHist, ddTest, ddTest/2);
					if(xShift > 0){
						if(xShift < 70) xShift += 10;
						p1X -= xShift;
						p4X -= xShift;
						if(p1X < 8) p1X = 8;
						if(p4X < 8) p4X = 8;
					}
					//	 for(i=0;i<vHist[0];i++){
					//		    printf("   i= %d, vHist= %d \n", i, vHist[i]);
					//	  }
				}
				if(strcmp(fName, "NUMBERS") == 0 || strcmp(fName, "NUMBERS1") == 0){
					if(pImg->bi.biWidth -p2X > 150){
						p2X += 50;
						p3X += 50;
					}
					//      printf("   p1Y= %d, pImg->bi.biWidth= %d\n", p1Y, pImg->bi.biHeight);

				}
				if(pImg->bi.biWidth -p2X > 150){
					p2X += 50;
					p3X += 50;
				}

				//      printf("   p1Y= %d, pImg->bi.biWidth= %d\n", p1Y, pImg->bi.biHeight);
				if( p1Y > 400){
					// y shift
					int yShift;
					int ddTest1 = 100, ddTest2 = 20;
					int vHist[200];
					WORD byteWidthHist = ((widthT + 31)/32)*4;
					//		  int i;

					histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  p1X, p2X, p1Y-ddTest1, p1Y+ddTest2, vHist);

					//	  		 for(i=0;i<vHist[0];i++){
					//	  			    printf("   i= %d, vHist= %d \n", i, vHist[i]);
					//	  		  }


					yShift = getyShiftForNumbersField(vHist, ddTest1, 2*ddTest2);

					//	       printf("   yShift= %d \n", yShift);

					if(yShift > 0){
						yShift += 5;
						p1Y -= yShift;
						p2Y -= yShift;
						if(p1Y < 8) p1Y = 8;
						if(p2Y < 8) p2Y = 8;
					}

				}
				//     printf("   heightT-p4Y= %d \n", heightT-p4Y);
				if( heightT-p4Y > 400){
					// y shift bottom
					int yShift;
					int ddTest1;
					//		  int vHist[2000];
					WORD byteWidthHist = ((widthT + 31)/32)*4;
//					int i;
					ddTest1 = heightT-p4Y - 200;
					//       printf("   ddTest1= %d, p4Y-p1Y= %d \n", ddTest1, p4Y-p1Y);

					histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  p1X, p2X, p1Y, p4Y+ddTest1, hHistPreProc);

					//	  		 for(i=0;i<hHistPreProc[0];i++){
					//	  			    printf("   i= %d, vHist= %d \n", i, hHistPreProc[i]);
					//	  		  }


					yShift = getYShiftBottomNumbersField(hHistPreProc, p4Y-p1Y);

					//	       printf("   yShift= %d \n", yShift);

					if(yShift > 0){
						p3Y = p1Y + yShift;
						if(p3Y > heightT) p3Y = heightT-8;
						p4Y = p3Y;
					}

				}

				verticalLineRemovalField(pTBuffer, widthT, heightT, p1Y, p4Y);

			}

			if((strcmp(fName, "DATEA") == 0 || strcmp(fName, "DATEN") == 0) && US_State < 0){  // temporary turn off
				WORD byteWidthHist = ((widthT + 31)/32)*4;
				int i, iSt, cc, av;

//        printf("2p1Y= %d, p3Y= %d, heightT= %d\n",p1Y,p3Y,heightT);

				histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  p1X+40, p2X-40, p1Y, p4Y, hHistPreProc);
//					  		 for(i=0;i<hHistPreProc[0];i++){
//					  			    printf("   vHist[ %d ]= %d \n", i, hHistPreProc[i]);
//					  		  }
				cc = 0;av = 0;iSt = 0;
		  		 for(i=hHistPreProc[0];i>hHistPreProc[0]/4;i--){
		  			 if(hHistPreProc[i] > 100 && hHistPreProc[i-1] > 100) {
		  				 if(cc == 0) iSt = i;
		  				 cc++;av += hHistPreProc[i];
		  			 }
		  			 else {cc = 0; av = 0;}
		  			 if (cc> 100) break;
		  		 }
//  			    printf("   cc= %d \n", cc);
		  		 if(cc > 100){
		  			 av /= cc;
//  			    printf("   av= %d \n", av);
		  			 if(av > 300){
		  				iSt -= 15;
// 			    printf("   iSt= %d, p3Y= %d \n", iSt, p3Y);
						p3Y -= (hHistPreProc[0]-(iSt-cc));
//	       printf("   p3Y= %d, p1Y= %d \n", p3Y, p1Y);
						if(p3Y < p1Y) p3Y = p4Y;
					    p4Y = p3Y;
//		    printf("   p3Y= %d, p1Y= %d \n", p3Y, p1Y);
		  			 }
		  		 }

			}
			if((strcmp(fName, "DATEN") == 0 || strcmp(fName, "DATEA") == 0 ||
				strcmp(fName, "RET_NUMN") == 0 || strcmp(fName, "RET_NUMA") == 0 )  && US_State <= 1){ // LOTTO TX
//	 printf("----------extractTicketsField; fName= %s, p1Y= %d, p2Y = %d, heightT= %d\n", fName, p1Y, p2Y, heightT);
				switch(ticketTypeInt){
				case 1:
				case 2:
				case 3:
				case 4:
				case 7:
				case 8:
				    {
			        int xxD1 = p1X-10;
			        int xxD4 = p4X-10;
//			        printf("p1Y= %d, p3Y= %d, heightT= %d\n",p1Y,p3Y,heightT);
			        if(xxD1>8 && xxD4>8){
				       p1X -= xxD1;p4X -= xxD4;
		             }
					if(heightT-p3Y > 400){
						p3Y += 200;
						p4Y += 200;
					}
				  } break;
				}
			}
			if(strcmp(fName, "LOGO3") == 0 && US_State <= 1){ // LOTTO TX
//	 printf("----------extractTicketsField; p3Y= %d, p4Y = %d, heightT= %d\n", p3Y, p4Y, heightT);
				switch(ticketTypeInt){
				case 1:
				case 2:
				case 3:
				case 5:
				case 6:
				case 8:
				   {
		           int xxD1 = p1X-10;
		           int xxD4 = p4X-10;
		           if(xxD1>8 && xxD4>8){
			          p1X -= xxD1;p4X -= xxD4;
	                }
					if(p2X > widthT/2){
						int dd = p3X-p2X;
						p2X = widthT/2;
						p3X = widthT/2 +dd;
					}
					if(heightT-p3Y > 500){
						p3Y += 200;
						p4Y += 200;
					}

				   } break;
				}
			}

			 if((strcmp(fName, "NUMBERS") == 0 || strcmp(fName, "NUMBERS1") == 0) && US_State <= 1){
//			  printf("   p1X= %d, p4X= %d\n", p1X, p4X);
					int xxD1 = p1X-10;
					int xxD4 = p4X-10;
					if(xxD1>8 && xxD4>8){
						   p1X -= xxD1;p4X -= xxD4;
				    }
//				      printf(" p1X= %d, p4X=%d\n", p1X, p4X);
//				      printf(" p2X= %d, p3X=%d\n", p2X, p3X);
					if(ticketTypeInt < 6 || ticketTypeInt >= 7)
//					if(ticketTypeInt < 9)
					{
// left side
					   int h_size = 300;
					   int lSide, rSide = p1X+h_size;
					   int top, bott, hh, i, numLines;
					   int ok = 0, step = 100;
					   int th = 2;

					   WORD byteWidthHist = ((widthT + 31)/32)*4;
					      lSide = p1X;
						  hh= (p4Y-p1Y)/3;top = p1Y+hh; bott = p4Y-hh;
						  for(i=0;i<5;i++){
						      rSide = lSide+h_size;
							  if(rSide < widthT){
							    histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  lSide, rSide, top, bott, hHistPreProc);
							    numLines = calculateLines(hHistPreProc);
//		                        if(i>1) th = 0;
//	    printf("1;  %s; lSide= %d, rSide= %d, top= %d, bott= %d, numLines= %d, i= %d, th= %d\n", pImg->szIMGName, lSide, rSide, top, bott, numLines, i, th);

							    if(numLines > th){
//  	   printf(" numLines= %d, th=%d, ok= %d\n", numLines, th, ok);
							    	ok = 1;
							       break;
							    }

/*
						        {char buff[1000];int ii;
							      sprintf(buff,"%s; lSide= %d, rSide= %d, top= %d, bott= %d\n", pImg->szIMGName, lSide, rSide, top, bott);
							          saveLogFile(buff, "\n", "log_test.txt");
							          for(ii = 1;ii<hHistPreProc[0];ii++) {
							            sprintf(buff," hHistPreProc[ %d] = %d\n", ii, hHistPreProc[ii]);
							            saveLogFile(buff, "", "log_test.txt");
							          }
							      }
*/

							  }
						      lSide += step;
						  }
//   printf(" p1X= %d, p4X=%d, ok= %d\n", p1X, p4X, ok);
						  if(ok==1 && lSide > p1X){
							  th = 2;
							  ok = 0;
							  lSide -= step;
							  step = 20;
							  for(i=0;i<5;i++){
							      rSide = lSide+h_size;
								  if(rSide < widthT){
								    histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  lSide, rSide, top, bott, hHistPreProc);
								    numLines = calculateLines(hHistPreProc);
//								    if(i>1) th = 0;
//   printf("2; %s; lSide= %d, rSide= %d, top= %d, bott= %d, numLines= %d\n", pImg->szIMGName, lSide, rSide, top, bott, numLines);
								    if(numLines > th){
								    	ok = 1;
								       break;
								    }
								  }
							      lSide += step;
							  }
							  if(ok == 1){
								  p1X = lSide-step/2;
								  p4X = lSide-step/2;
							  }else{
								  p1X = lSide+step/2;
								  p4X = lSide+step/2;
							  }
						  }
//   printf(" p1X= %d, p4X=%d\n", p1X, p4X);
   // right side
                         h_size = 300;
   					     ok = 0; step = 100;
   					      rSide = p2X;
   						  for(i=0;i<5;i++){
   						      lSide = rSide-h_size;
   							  if(lSide > 0){
   							    histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  lSide, rSide, top, bott, hHistPreProc);
   							    numLines = calculateLines(hHistPreProc);
//   		    printf("1;  %s; lSide= %d, rSide= %d, top= %d, bott= %d, numLines= %d\n", pImg->szIMGName, lSide, rSide, top, bott, numLines);
   							    if(numLines > 0){
   							    	ok = 1;
   							       break;
   							    }

   /*
   						        {char buff[1000];int ii;
   							      sprintf(buff,"%s; lSide= %d, rSide= %d, top= %d, bott= %d\n", pImg->szIMGName, lSide, rSide, top, bott);
   							          saveLogFile(buff, "\n", "log_test.txt");
   							          for(ii = 1;ii<hHistPreProc[0];ii++) {
   							            sprintf(buff," hHistPreProc[ %d] = %d\n", ii, hHistPreProc[ii]);
   							            saveLogFile(buff, "", "log_test.txt");
   							          }
   							      }
   */

   							  }
   						      rSide -= step;
   						  }
   						  if(ok==1 && rSide < p2X){
   							  ok = 0;
   							  rSide += step;
   							  step = 20;
   							  for(i=0;i<5;i++){
   							      lSide = rSide-h_size;
   								  if(lSide > 0){
   								    histHTest_X1_X1(pTBuffer, byteWidthHist, heightT,  lSide, rSide, top, bott, hHistPreProc);
   								    numLines = calculateLines(hHistPreProc);

//   						    printf("2; %s; lSide= %d, rSide= %d, top= %d, bott= %d, numLines= %d\n", pImg->szIMGName, lSide, rSide, top, bott, numLines);
   								    if(numLines > 0){
   								    	ok = 1;
   								       break;
   								    }
   								  }
   							      rSide -= step;
   							  }
   							  if(ok == 1){
   								  p2X = rSide+step/2;
   								  p3X = rSide+step/2;
   							  }else{
   								  p2X = rSide-step/2;
   								  p3X = rSide-step/2;
   							  }
   						  }
//      printf(" p2X= %d, p3X=%d\n", p2X, p3X);


					}


					{ float yy = (float)heightT/(float)p1Y;
					if(yy < 2.9){
						if(p1Y > 400){
							p1Y -= 200;
							p2Y -= 200;
						}
					}
// printf("----------extractTicketsField; p1Y= %d, p2Y = %d, heightT= %d, xx= %f\n", p1Y, p2Y, heightT, yy);
					}
			//		 printf("----------extractTicketsField; p1X= %d\n", p1X);
			 }
			 if(strcmp(fName, "RET_NUMN") == 0 || strcmp(fName, "RET_NUMA") == 0){
				 setField_RET_NUM(&p1X, &p1Y, &p2X, &p2Y, &p3X, &p3Y, &p4X, &p4Y, widthT, heightT);
//				 printf("2----------extractTicketsField; fName= %s, p1Y= %d, p2Y = %d, heightT= %d\n", fName, p1Y, p2Y, heightT);

			 }

 //			US_state   ???????????????????????????????????????????????????????????????????????????????
             if(US_State == 2 && heightT - p3Y > 400){
            	 p3Y += 300;
            	 p4Y = p3Y;
	 		}
/*
			 if(strcmp(fName, "DATEN") == 0 || strcmp(fName, "DATEA") == 0){
				 p1X = 10;p2X = 1079;
				 p4X = p1X;p3X = p2X;
				 p1Y = 226;p2Y = 226;
				 p3Y = 1608;p4Y = 1608;
			 }
*/
			pFieldQuad -> p1X = p1X;
			pFieldQuad -> p1Y = p1Y;
			pFieldQuad -> p2X = p2X;
			pFieldQuad -> p2Y = p2Y;
			pFieldQuad -> p3X = p3X;
			pFieldQuad -> p3Y = p3Y;
			pFieldQuad -> p4X = p4X;
			pFieldQuad -> p4Y = p4Y;
			/*
  {char buff[500];
   saveLog("extractField; name= ", fName);
   sprintf(buff,"\n widthT= %d, heightT= %d\n", widthT, heightT);
   saveLog("", buff);

   sprintf(buff,"\n p1X= %d, p2X= %d, p3X= %d, p4X= %d\n", p1X, p2X, p3X, p4X);
   saveLog("", buff);
   sprintf(buff,"\n p1Y= %d, p2Y= %d, p3Y= %d, p4Y= %d\n", p1Y, p2Y, p3Y, p4Y);
   saveLog("", buff);
       }
			 */

			//  if(strcmp(fName, "NUMBERS") == 0)
			//    printf("----------extractTicketsField; widthT= %d, heightT= %d, p1X= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthT, heightT, p1X, p1Y, p2X-p1X, p1Y-p4Y);

// printf("----------extractTicketsField; p1X= %d\n", p1X);
/*
 if(strcmp(fName, "NUMBERS") == 0){
   printf("\n p1X= %d, p2X= %d, p3X= %d, p4X= %d\n", p1X, p2X, p3X, p4X);
   printf("\n p1Y= %d, p2Y= %d, p3Y= %d, p4Y= %d\n", p1Y, p2Y, p3Y, p4Y);
   printf("\n w= %d, h= %d \n", p2X- p1X, p3Y-p1Y);
   printf("\n widthT= %d, heightT= %d \n", widthT, heightT);
 }
 fflush(stdout);
*/

/*
			err =
					extractField(pTBuffer, widthT, heightT,
							p1X, p1Y,
							p2X, p2Y,
							p4X, p4Y,
							&pFieldH->himgPtr,
							//         phgBuffer,
							pwidth,
							pheight,
							extractMargin,
							fName,
							angleFlag,
							angleOut
					);
			if(err < 0) return(err);
		}
*/
		err =
				extractFieldAdj(pTBuffer, widthT, heightT,
						&p1X, &p1Y,
						&p2X, &p2Y,
						&p4X, &p4Y,
						&pFieldH->himgPtr,
						//         phgBuffer,
						pwidth,
						pheight,
						extractMargin,
						fName,
						angleFlag,
						angleOut
				);
		if(err < 0) return(err);
	}
		pFieldQuad -> p1X = p1X;
		pFieldQuad -> p1Y = p1Y;
		pFieldQuad -> p2X = p2X;
		pFieldQuad -> p2Y = p2Y;
		pFieldQuad -> p3X = p3X;
		pFieldQuad -> p3Y = p3Y;
		pFieldQuad -> p4X = p4X;
		pFieldQuad -> p4Y = p4Y;

		//  hgBuffer = *phgBuffer;
/*
	 if(strcmp(fName, "NUMBERS") == 0){
		   printf("2 p1X= %d, p2X= %d, p3X= %d, p4X= %d\n", p1X, p2X, p3X, p4X);
		   printf("2 p1Y= %d, p2Y= %d, p3Y= %d, p4Y= %d\n", p1Y, p2Y, p3Y, p4Y);
		   printf("2 angleOut= %e, angleFlag= %d\n", *angleOut, angleFlag);
		   printf("2 w= %d, h= %d \n", *pwidth, *pheight);

	 }
	 fflush(stdout);
*/
		sizeBuffer = (DWORD)(*pwidth)*(DWORD)(*pheight + 2*extractMargin);

		pwBuffer = (WORD *)pFieldH->himgPtr.ptr;
		*(pwBuffer+2)= (WORD)(sizeBuffer%65536);
		*(pwBuffer+3)= (WORD)(sizeBuffer/65536);
		//  GlobalUnlock(hgBuffer);
	}

	//  pFieldH -> hgBuffer = hgBuffer;
	pFieldH -> wWidth   = *pwidth;
	pFieldH -> wHeight  = *pheight;
	pFieldH -> wDpiX    = wDpiX;
	pFieldH -> wDpiY    = wDpiY;

	//  saveLog("extractField end ", "\n");
	{LPBYTE debbugB = pBuffer;
	        pBuffer = debbugB;
	}



	return(0);
}


int  createZoneStructCall (LPIMGDATA img, LPTICKETDATA ticketDataPtr, WORD wZoneNo, LPRADON radonStruct)
{
	//    BYTE * pFormDescr;

	ticketDataPtr->zone[wZoneNo].ptrZoneStruct = NULL;

	if (!ticketDataPtr->prototypePtr) {
		return -1;
	}

	ticketDataPtr->zone[wZoneNo].iAngle = ticketDataPtr->iAngle;
	ticketDataPtr->returnCode = createZoneStruct(img, ticketDataPtr->prototypePtr, &ticketDataPtr->workArea,  ticketDataPtr->ptrTicketStruct,
			&ticketDataPtr->zone[wZoneNo].iAngle,  wZoneNo + 1,
//			&ticketDataPtr->zone[wZoneNo].ptrZoneStruct, &ticketDataPtr->zone[wZoneNo].ZoneQuad, radonStruct, ticketDataPtr);
	&ticketDataPtr->zone[wZoneNo].ptrZoneStruct, &ticketDataPtr->zone[wZoneNo].ZoneQuad, radonStruct);

	if (ticketDataPtr->returnCode < 0) {
		// set flags once again: we had failure
		ticketDataPtr->zone[wZoneNo].iAngle = 1;
		if (ticketDataPtr->zone[wZoneNo].ptrZoneStruct) {
			yl_free(ticketDataPtr->zone[wZoneNo].ptrZoneStruct);
			ticketDataPtr->zone[wZoneNo].ptrZoneStruct = NULL;
		}
		return 0;
	}
	return 1;
}

int createZoneStruct(LPIMGDATA pImg, LPBYTE pTDescr, HLPBYTE* pWorkArea,  LPBYTE pTicketStruct,
//		int* pAngle,  WORD wZoneNo,  LPBYTE* pZoneStruct, LPQUADRANGLE pZoneQuad, LPRADON radonStruct, LPTICKETDATA ticketDataPtr) {
	int* pAngle,  WORD wZoneNo,  LPBYTE* pZoneStruct, LPQUADRANGLE pZoneQuad, LPRADON radonStruct) {

	int iAngle;

	BYTE * lpWorkArea;
	BYTE* lpTicketStruct;
	DWORD sizeZoneStructAlloc;
	DWORD sizeZoneStruct;
	WORD * lpwZoneStruct;
	BYTE * lpZoneStruct;

	WORD wDpiX;
	WORD wDpiY;
	WORD wDpiX0;
	WORD wDpiY0;

	WORD zoneMargin;

	int retVal;
	int err;
	long shift;
	DWORD sizeWorkAreaAlloc;
	WORD * pwFormDescr;


	WORD sizeDFX0;
	int dMinFX0;
	WORD stepDFX0;
	WORD sizeDFX1;
	int dMinFX1;
	WORD stepDFX1;
	WORD * pRadonMainTeta0;
	WORD sizeDFY0;
	int dMinFY0;
	WORD stepDFY0;
	WORD sizeDFY1;
	int dMinFY1;
	WORD stepDFY1;
	WORD * pRadonDualTeta0;

	DWORD size1;
	DWORD size2;

	DWORD dist;
	DWORD * pDist;
	WORD  *  pPosX0;
	WORD  *  pPosEndX;
	WORD posX0;
	WORD posEndX;
	WORD * pAllignX;
	WORD *  pPosY0;
	WORD *  pPosEndY;
	WORD posY0;
	WORD posEndY;
	WORD * pAllignY;

	WORD originX0;
	WORD originY0;

	DWORD offsetZone;
	WORD wZoneNoFD;
	WORD widthRec0;
	WORD heightRec0;

	WORD * pwZoneDescr;
	WORD * pwZoneStructX;
	WORD * pwZoneStructY;
	WORD cornerX;
	WORD cornerY;

	WORD p1X;
	WORD p1Y;
	WORD p2X;
	WORD p2Y;
	WORD p3X;
	WORD p3Y;
	WORD p4X;
	WORD p4Y;

	WORD sizeZoneStruct0;
	DWORD offsetNFields;
	WORD wNFields;

	WORD nFields;
	int stepXL;
	int stepYL;
	int phiMinL;
	int phiMaxL;
	WORD stepPhiL;
	int iZoneMargin;

	radonStruct->local = 1;

	*pAngle = radonStruct->tetaG;

	iAngle = *pAngle;

	pDist = &dist;
	pPosX0 = &posX0;
	pPosEndX = &posEndX;
	pPosY0 = &posY0;
	pPosEndY = &posEndY;

	pTBuffer   = pImg ->pixPtr;
	widthT     = pImg -> bi.biWidth;
	heightT    = pImg -> bi.biHeight;
	wDpiX      = pImg -> bi.biXPelsPerMeter;
	wDpiY      = pImg -> bi.biYPelsPerMeter;



	if(((int)wDpiX < 80)||((int)wDpiX > 420)) return(-112);
	if(((int)wDpiY < 80)||((int)wDpiY > 420)) return(-112);

	byteWidthT = ((widthT + 31)/32)*4;

	pwFormDescr = (WORD *)pTDescr;
	wDpiX0 = *(pwFormDescr+1);
	wDpiY0 = *(pwFormDescr+2);
	offsetZone = *(pwFormDescr+5L+(DWORD)(2*(wZoneNo-1))) +
			65536*(*(pwFormDescr+6L+(DWORD)(2*(wZoneNo-1))));
	pwZoneDescr = (WORD*)(pTDescr + offsetZone);
	wZoneNoFD = *pwZoneDescr;
	if(wZoneNoFD != wZoneNo)
		return(-1);

	originX0       = *(pwZoneDescr+1L);
	originY0       = *(pwZoneDescr+2L);
	widthRec0      = *(pwZoneDescr+3L);
	heightRec0     = *(pwZoneDescr+4L);

	sizeZoneStruct0 = *(pwZoneDescr+5L);
	offsetNFields = 6L+(long)(sizeZoneStruct0/2);
	wNFields = *(pwZoneDescr+offsetNFields);


	if(((int)wDpiX0 < 80)||((int)wDpiX0 > 420)) return(-112);
	if(((int)wDpiY0 < 80)||((int)wDpiY0 > 420)) return(-112);

	pwZoneStructX = pwZoneDescr + 6L;


	sizeZoneStruct = (DWORD)(*(pwZoneDescr + 5L));
	nFields = *(pwZoneDescr + 6L + sizeZoneStruct/2);
	shift = (long)(6L + sizeZoneStruct/2 + 1L + 2*nFields);
	stepXL      = (int)*(pwZoneDescr + shift + 0);
	stepYL      = (int)*(pwZoneDescr + shift + 1);
	phiMinL     = (int)*(pwZoneDescr + shift + 2);
	phiMaxL     = (int)*(pwZoneDescr + shift + 3);
	stepPhiL    =     *(pwZoneDescr + shift + 4);
	radonStruct->tgP         =     *(pwZoneDescr + shift + 5);
	iZoneMargin = (int)*(pwZoneDescr + shift + 6);


//	 printf("------------phiMinL= %d \n",  phiMinL);
//	 fflush(stdout);
	phiMinL = -20; // ??????????????????????????????

			shift = (long)(*pwZoneStructX/2) + 1L;
			sizeDFX0 = *(pwZoneStructX+1L);
			dMinFX0 = (int)(*(pwZoneStructX+2L));
			stepDFX0 = *(pwZoneStructX+3L);
			pRadonMainTeta0 = pwZoneStructX+4L;
			pwZoneStructY = pwZoneStructX + shift;
			shift = (long)(*pwZoneStructY/2) + 1L;
			sizeDFY0 = *(pwZoneStructY+1L);
			dMinFY0 = (int)(*(pwZoneStructY+2L));
			stepDFY0 = *(pwZoneStructY+3L);
			pRadonDualTeta0 = pwZoneStructY+4L;

			// hgFormStruct = *phgFormStruct;
			lpTicketStruct = (BYTE*)pTicketStruct + 8;

			pZoneQuad -> p1X = originX0;
			pZoneQuad -> p1Y = originY0;
			pZoneQuad -> p2X = originX0+widthRec0;
			pZoneQuad -> p2Y = originY0;
			pZoneQuad -> p3X = originX0+widthRec0;
			pZoneQuad -> p3Y = originY0+heightRec0;
			pZoneQuad -> p4X = originX0;
			pZoneQuad -> p4Y = originY0+heightRec0;

			zoneMargin = (WORD)((iZoneMargin*wDpiX0 + 300*4)/(300*8))*8;

 //printf("zoneMargin=%d, pZoneQuad -> p1X= %d, pZoneQuad -> p1Y= %d, pZoneQuad -> p2X= %d,pZoneQuad -> p3Y= %d \n",zoneMargin, pZoneQuad -> p1X, pZoneQuad -> p1Y, pZoneQuad -> p2X, pZoneQuad -> p3Y);
 //printf("pTicketStruct= %u \n", (unsigned int)pTicketStruct);
 //fflush(stdout);
			err =
					mapQ(pZoneQuad, iAngle, lpTicketStruct, zoneMargin);
			//      GlobalUnlock(hgFormStruct);
			if(err < 0) return(err);

			if((pZoneQuad -> p1X) < (pZoneQuad -> p4X))
				originX = (pZoneQuad -> p1X);
			else originX = (pZoneQuad -> p4X);
			if((int)originX < 0) originX = 0;
			originX = (originX/8)*8;
			if((pZoneQuad -> p1Y) < (pZoneQuad -> p2Y))
				originY = (pZoneQuad -> p1Y);
			else originY = (pZoneQuad -> p2Y);
			if((int)originY < 0) originY = 0;
			if((pZoneQuad -> p2X) < (pZoneQuad -> p3X))
				cornerX = (pZoneQuad -> p3X);
			else cornerX = (pZoneQuad -> p2X);
			if(cornerX > widthT - 1) cornerX = widthT - 1;
			cornerX = (cornerX/8)*8;
			if((pZoneQuad -> p3Y) < (pZoneQuad -> p4Y))
				cornerY = (pZoneQuad -> p4Y);
			else cornerY = (pZoneQuad -> p3Y);
			if(cornerY > heightT - 1) cornerY = heightT - 1;
			widthRec  = cornerX - originX;
			heightRec = cornerY - originY;

			size1 = (widthRec + 2*zoneMargin)/stepYL + 1;
			size2 = (heightRec + 2*zoneMargin)/stepXL + 1;
			if(size1 > size2) sizeWorkAreaAlloc = sizeDFY0*size1 + 100L;
			else sizeWorkAreaAlloc = sizeDFX0*size2 + 100L;
			sizeWorkAreaAlloc = sizeWorkAreaAlloc*3/2;

			// err = getMemoryHuge(phgWorkArea, sizeWorkAreaAlloc, 500000L);
//	LOGD("1. createZoneStruct(); sizeDFY0= %d, sizeDFX0= %d, widthRec= %d, heightRec= %d, sizeWorkAreaAlloc= %u\n", sizeDFY0, sizeDFX0, widthRec, heightRec, sizeWorkAreaAlloc);

			recalculateAllocBuffer(&sizeWorkAreaAlloc, 500000);
//printf(" pWorkArea->size= %ld, sizeWorkAreaAlloc= %d \n", pWorkArea->size, sizeWorkAreaAlloc);
//fflush(stdout);
			if(pWorkArea->ptr == NULL || pWorkArea->size < sizeWorkAreaAlloc) {
//				yl_free(pWorkArea->ptr);
//				pWorkArea->ptr = yl_calloc("F2", sizeWorkAreaAlloc, sizeof(BYTE));
//				if(pWorkArea->ptr == NULL) {
					sprintf(pImg->error,"createZoneStruct (\"%s\") yl_calloc failed: %s\n",pImg->szIMGName, strerror(errno));
					return -1;
//				}
//				(pWorkArea)->size = sizeWorkAreaAlloc;

			}

			lpWorkArea = (BYTE *)(pWorkArea->ptr + 8);

			sizeZoneStructAlloc =
					(DWORD)(44 + 2*((widthRec + 2*zoneMargin)/stepYL + (heightRec + 2*zoneMargin)/stepXL + 2) + 100);

			// err = getMemoryHuge(phgZoneStruct, sizeZoneStructAlloc, 4000L);
			recalculateAllocBuffer(&sizeZoneStructAlloc, 4000);

//			sizeZoneStructAlloc += 4000; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// *pZoneStruct = yl_calloc(sizeWorkAreaAlloc, sizeof(BYTE));
			*pZoneStruct = yl_calloc("F3", sizeZoneStructAlloc, sizeof(BYTE));


			//  LOGD("sizeZoneStrictAlloc: %d", sizeZoneStructAlloc);
			if(*pZoneStruct == NULL) {
				sprintf(pImg->error,"createZoneStruct (\"%s\") yl_calloc failed: %s\n",pImg->szIMGName, strerror(errno));
				return -1;
			}

			// hgZoneStruct = *phgZoneStruct;
			lpZoneStruct = (BYTE *)(*pZoneStruct);
			lpwZoneStruct = ((WORD *)lpZoneStruct) + 4;

			radonStruct->teta = iAngle;
			radonStruct->stepD          = stepXL;
			radonStruct->phiMin = iAngle + phiMinL; //???????????????????????????????
			radonStruct->phiMax = iAngle + phiMaxL;
			radonStruct->stepPhi = stepPhiL;

			// local = 1;

			radonStruct->pRadonBuffer = (WORD *)lpWorkArea;
			radonStruct->pRadonMainTeta = &buffer[0];

			retVal = radonMainG(radonStruct);
			if(retVal < 0) {
				return(retVal);
			}
			//      retVal = radonMainTeta();
			retVal = radMT(radonStruct);
			if(retVal < 0) {
				return(retVal);
			}

			// yl_free(pZoneStruct);

			iAngle = radonStruct->teta;
			*pAngle = iAngle;

			pAllignX = lpwZoneStruct + 9L;

			err = allignArrays(radonStruct, lpWorkArea,
					pRadonMainTeta0,
					radonStruct->pRadonMainTeta,
					sizeDFX0,
					radonStruct->sizeDTeta,
					pDist,
					pAllignX,
					pPosX0,
					pPosEndX
			);
			if(err < 0) { return(err);}

			*(lpwZoneStruct + 0L) = 2*(sizeDFX0 + 8);
			*(lpwZoneStruct + 1L) = sizeDFX0;
			*(lpwZoneStruct + 2L) = radonStruct->sizeDTeta;
			*(lpwZoneStruct + 3L) = (WORD)dMinFX0;
			*(lpwZoneStruct + 4L) = (WORD)(radonStruct->dMinTeta/SCALE);
			*(lpwZoneStruct + 5L) = stepDFX0;
			*(lpwZoneStruct + 6L) = radonStruct->stepD;
			*(lpwZoneStruct + 7L) = posX0;
			*(lpwZoneStruct + 8L) = posEndX;

			sizeDFX1 = radonStruct->sizeDTeta;
			dMinFX1  = (int)(radonStruct->dMinTeta/SCALE);
			stepDFX1 = radonStruct->stepD;

			sizeZoneStruct = (DWORD)(2*(sizeDFX0 + 9));
			shift = (long)(sizeDFX0 + 9);
			lpwZoneStruct = lpwZoneStruct + shift;

			radonStruct->stepD          = stepYL;
			radonStruct->pRadonDualTeta = &buffer[0];

			retVal = radDTL(radonStruct);
			if(retVal < 0) { return(retVal);}

			pAllignY = lpwZoneStruct + 9L;

			err = allignArrays(radonStruct,  lpWorkArea,  pRadonDualTeta0, radonStruct->pRadonDualTeta,
					sizeDFY0, radonStruct->sizeDTeta, pDist, pAllignY, pPosY0, pPosEndY );
			if(err < 0) { return(err);}

			*(lpwZoneStruct + 0L) = 2*(sizeDFY0 + 8);
			*(lpwZoneStruct + 1L) = sizeDFY0;
			*(lpwZoneStruct + 2L) = radonStruct->sizeDTeta;
			*(lpwZoneStruct + 3L) = (WORD)dMinFY0;
			*(lpwZoneStruct + 4L) = (WORD)(radonStruct->dMinTeta/SCALE);
			*(lpwZoneStruct + 5L) = stepDFY0;
			*(lpwZoneStruct + 6L) = radonStruct->stepD;
			*(lpwZoneStruct + 7L) = posY0;
			*(lpwZoneStruct + 8L) = posEndY;

			sizeDFY1 = radonStruct->sizeDTeta;
			dMinFY1  = (int)(radonStruct->dMinTeta/SCALE);
			stepDFY1 = radonStruct->stepD;

			sizeZoneStruct = sizeZoneStruct + (DWORD)(2*(sizeDFY0 + 9));

			lpwZoneStruct = (WORD *)lpZoneStruct;
			*(lpwZoneStruct + 2L) = (WORD)(sizeZoneStruct%65536);
			*(lpwZoneStruct + 3L) = (WORD)(sizeZoneStruct/65536);

			lpZoneStruct = lpZoneStruct + 8L;


			pZoneQuad -> p1X = originX0;
			pZoneQuad -> p1Y = originY0;
			pZoneQuad -> p2X = originX0+widthRec0;
			pZoneQuad -> p2Y = originY0;
			pZoneQuad -> p3X = originX0+widthRec0;
			pZoneQuad -> p3Y = originY0+heightRec0;
			pZoneQuad -> p4X = originX0;
			pZoneQuad -> p4Y = originY0+heightRec0;

			err =
					mapQ(pZoneQuad, iAngle, lpZoneStruct, 0);
			if(err < 0) return(err);

//	 printf("pZoneQuad -> p1X= %d, pZoneQuad -> p1Y= %d, pZoneQuad -> p2X= %d, pZoneQuad -> p3Y= %d\n",pZoneQuad -> p1X, pZoneQuad -> p1Y, pZoneQuad -> p2X, pZoneQuad -> p3Y);
//	 fflush(stdout);

//	 printf("widthT=%d, heightT= %d\n",widthT, heightT);
//	 fflush(stdout);

			p1X = (pZoneQuad -> p1X);
			if((int)p1X < 0) p1X = 0;
			p1Y = (pZoneQuad -> p1Y);
			if((int)p1Y < 0) p1Y = 0;
			p2X = (pZoneQuad -> p2X);
			if(p2X > widthT - 1) p2X = widthT - 1;
			p2Y = (pZoneQuad -> p2Y);
			if((int)p2Y < 0) p2Y = 0;
			p3X = (pZoneQuad -> p3X);
			if(p3X > widthT - 1) p3X = widthT - 1;
			p3Y = (pZoneQuad -> p3Y);
			if(p3Y > heightT - 1) p3Y = heightT - 1;
			p4X = (pZoneQuad -> p4X);
			if((int)p4X < 0) p4X = 0;
			p4Y = (pZoneQuad -> p4Y);
			if(p4Y > heightT - 1) p4Y = heightT - 1;

			pZoneQuad -> p1X = p1X;
			pZoneQuad -> p1Y = p1Y;
			pZoneQuad -> p2X = p2X;
			pZoneQuad -> p2Y = p2Y;
			pZoneQuad -> p3X = p3X;
			pZoneQuad -> p3Y = p3Y;
			pZoneQuad -> p4X = p4X;
			pZoneQuad -> p4Y = p4Y;

// get rid of compiler warnings
			sizeDFX1 = stepDFX1;
			stepDFX1 = sizeDFX1;
			sizeDFY1 = stepDFY1;
			stepDFY1 = sizeDFY1;
			dMinFY1 = dMinFX1;
			dMinFX1 = dMinFY1;
			p1X = wNFields;


			return(0);
}

int  extractField(LPBYTE pFormBufferA, WORD widthFormA, WORD heightFormA,
		int ax, int ay, int bx, int by, int dx, int dy,
		LPHLPBYTE hpFieldBufferAInp, WORD *pwidth, WORD *pheight, WORD extractMargin, char *fName,
		int angleFlag, double *angleOut){
	int err;
	int x;
	int y;
	double r;
	long cosTeta;
	long sinTeta;
	WORD width;
	WORD height;
	WORD byteWidth;
	DWORD sizeFieldBufferAlloc;
	DWORD sizeFieldBuffer;
	// HGLOBAL hgFieldBufferA;
	BYTE  * pFieldBufferA;
	WORD  * pwFieldBufferA;
	DWORD * pdwFieldBufferA;

	DWORD size;
	DWORD offset;
	DWORD dw0=0, dw1=0;

	WORD byteWidthHist = ((widthFormA + 31)/32)*4;
	float maxAlpha = 0.9;
	float stepAlpha = 0.1;
	float ang, ang2;
	// int  scAngleH;
	float shfx1, shfx2, shfy1, shfy2;
	float siN, coS, tG;
	int angleTest;

	byteWidthT = ((widthFormA + 31)/32)*4;
	//	 char buff[500];

//	printf("------------ fName= %s------------------ \n", fName);
//    printf("------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx= %d, by = %d \n", widthFormA, heightFormA, ax, ay, bx, by);
//    printf("------------ dx= %d, dy= %d, extractMargin= %d, angleFlag= %d\n",  dx, dy, extractMargin, angleFlag);

//    printf("------------ angleOut= %e\n",  *angleOut);
//    fflush(stdout);

	// x shift
	ang = *angleOut;
	if(angleFlag != 0) {

		angleTest = getAngle(pFormBufferA, widthFormA, heightFormA, ax, ay, bx-ax, dy-ay,
				-60, 60, 5);
//		 if(strcmp(fName, "NUMBERS") == 0) {
//			   printf("------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
//		 printf("------------ angleTest= %d\n", angleTest);
//		 }
		//sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
		//saveLog("", buff);

		//  getAngleFromHistH(pFormBufferA, byteWidthHist, heightFormA,
		// 		              ax, ay, bx, dy, maxAlpha, stepAlpha, &ang);

		//  ang  = -(float)angleTest/10.0;
		ang2 = -(float)angleTest/10.0;

		if(strcmp(fName, "NUMBERS") != 0 || strcmp(fName, "NUMBERS1") != 0){
			getAngleFromHistH(pFormBufferA, byteWidthHist, heightFormA,
					ax, ay, bx, dy, maxAlpha, stepAlpha, &ang);
//      printf("------------ angleTest= %d, ang2= %e, ang= %e\n", angleTest, ang2, ang);
			if(fabs(ang-ang2) < 1)
				ang = ang2;
		}else
			ang  = ang2;
		// if(strcmp(fName, "NUMBERS") == 0)
		//   printf("------------ ang= %e \n", ang);

		//   sprintf(buff,"------------ ang= %e \n", ang);
		//   saveLog("", buff);

		//  ang = 3;
	}
//	 if(strcmp(fName, "NUMBERS") == 0)
  //printf("1------------ ang= %e \n", ang);


//	if(US_State == 2) {
//		if(fabs(ang) < fabs(fieldAngleFromBarCode) ){ char buff[200];
//			 sprintf(buff, "------------ fieldAngleFromBarCode= %e, ang= %e \n", fieldAngleFromBarCode, ang);
//		     saveLog("", buff);
//	         ang = fieldAngleFromBarCode;
//		}
//	}

//	if(strcmp(fName, "NUMBERS") == 0)
//      printf("2------------ ang= %e, fieldAngleFromBarCode= %e\n", ang, fieldAngleFromBarCode);
//	  ang = 0.6;

	*angleOut = ang;
//	aaa


	siN = sin((double)(ang*PI)/180.0);
	coS = cos((double)(ang*PI)/180.0);
	tG  = tan((double)(ang*PI)/180.0);
	//   printf("------------ siN= %e, coS= %e, tG= %e \n", siN, coS, tG);

	shfy1 = (bx-ax)*siN;
	shfx1 = shfy1*tG;
	shfx2 = (dy-ay)*siN+shfx1;
	shfy2 = (dy-ay)*coS-(dy-ay-shfy1);
	//   printf("------------ shfx1= %e, shfx2 = %e, shfy1= %e, shfy2= %e \n", shfx1, shfx2, shfy1, shfy2);

	//   printf("------------ ax= %d bx= %d \n", ax, bx);
	//   printf("------------ ay= %d by= %d \n", ay, by);
	//   printf("------------ dx= %d dy= %d \n", dx, dy);

	ax = ax + (int)(shfx1+0.5);
	ay = ay + (int)(shfy1+0.5);
	dx = dx + (int)(shfx2+0.5);
	dy = dy + (int)(shfy2+0.5);
	if(ax >= widthFormA) ax = widthFormA-1;
	if(ax < 0) ax = 0;
	if(ay >= heightFormA) ay = heightFormA-1;
	if(ay < 0) ay = 0;
	if(dx >= widthFormA) dx = widthFormA-1;
	if(dx < 0) ax = 0;
	if(dy >= heightFormA) dy = heightFormA-1;
	if(dy < 0) dy = 0;

//	   sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx= %d, dy = %d \n", widthFormA, heightFormA, ax, ay, bx, dy);
//	   saveLog("", buff);

	//   sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
	//   saveLog("", buff);
/*
if(strcmp(fName, "NUMBERS") == 0) {
	   printf("------------ ax= %d bx= %d \n", ax, bx);
	   printf("------------ ay= %d by= %d \n", ay, by);
	   printf("------------ dx= %d dy= %d \n", dx, dy);
}
*/
	x = bx - ax;
	y = by - ay;
	r = sqrt((double)((long)x*(long)x + (long)y*(long)y));
	width = ((WORD)r) + 1;
	if(r != 0.0)
	{cosTeta = (long)((x/r)*SCALE);
	sinTeta = (long)((y/r)*SCALE);
	}
	else
	{cosTeta = SCALE;
	sinTeta = 0L;
	}
	x = dx - ax;
	y = dy - ay;
	r = sqrt((double)((long)x*(long)x + (long)y*(long)y));
	height = ((WORD)r) + 1;

	if(ax+width >= widthFormA)
		width = widthFormA-1 - ax;
	if(ay+height >= heightFormA)
		height = heightFormA-1 - ay;


	byteWidth = ((width + 2*extractMargin + 31)/32)*4;


	if(widthFormA < width)
		return (-100);

	if(heightFormA < height)
		return (-100);

	// sizeFieldBufferAlloc =
	//     (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin) +
	//     (DWORD)(offset/2) + 100;

	// sizeFieldBuffer = (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin);

	// hgFieldBufferA = *phgFieldBufferA;

	if(hpFieldBufferAInp->ptr != NULL)
   {
// printf("------------ fName= %s; pdwFieldBufferA \n", fName);
		pdwFieldBufferA = (DWORD *)(hpFieldBufferAInp->ptr);
	   dw0 =  *pdwFieldBufferA; dw1 = *(pdwFieldBufferA + 1);
	   size = dw0;
	   if(size != 0L) offset = 8;
	   else offset = dw1;
//   printf("------------ size= %d, offset= %d  \n", size, offset);
	}
	else offset = 8;

//   printf("------------ size= %d, offset= %d  \n", size, offset);

	sizeFieldBufferAlloc =
			(DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin) +
			(DWORD)(offset/2) + 100L;
	sizeFieldBuffer = (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin);

	recalculateAllocBuffer(&sizeFieldBufferAlloc, 300000);
	err = reallocMemoryBuffer (hpFieldBufferAInp, sizeFieldBufferAlloc);
	if(err < 0) {
		//       	sprintf(pImg->error,"createZoneStruct (\"%s\") yl_calloc failed: %s\n",pImg->szIMGName, strerror(errno));
		return -1;
	}
//   printf("------------ sizeFieldBufferAlloc= %u, sizeFieldBuffer= %u  \n", sizeFieldBufferAlloc, sizeFieldBuffer);
//   printf("------------ hpFieldBufferAInp->size= %lu  \n", hpFieldBufferAInp->size);

	// hgFieldBufferA = *phgFieldBufferA;
	pFieldBufferA = (BYTE *)hpFieldBufferAInp->ptr;
	pwFieldBufferA = (WORD *)pFieldBufferA;
	if(offset != 8)
	{pdwFieldBufferA = (DWORD *)pFieldBufferA;
	*pdwFieldBufferA        = dw0;
	*(pdwFieldBufferA + 1L) = dw1;
	}

	pFieldBufferA = pFieldBufferA + offset;
// printf(" total size= %ld, offset= %d, sizeFieldBufferAlloc= %d \n", hpFieldBufferAInp->size, offset, sizeFieldBufferAlloc);

//	 sprintf(buff," total size= %ld, offset= %d, sizeFieldBufferAlloc= %d \n", hpFieldBufferAInp->size, offset, sizeFieldBufferAlloc);
//	 saveLog("", buff);
	// sprintf(buff," fieldStart= %u, fieldEnd= %u \n", (unsigned)pFieldBufferA, (unsigned)(pFieldBufferA+ sizeFieldBufferAlloc));
	// saveLog("", buff);

//	 printf(" widthFormA= %d, heightFormA= %d, ax= %d, ay= %d \n", widthFormA, heightFormA, ax, ay);
//	 printf(" cosTeta= %ld, sinTeta= %ld, width= %d, height= %d, extractMargin= %d \n", cosTeta, sinTeta, width, height, extractMargin);
// printf("------------ fName= %s; pdwFieldBufferA \n", fName);

//    fflush(stdout);

	err = desKew
			(pFormBufferA,
					widthFormA,
					heightFormA,
					ax,
					ay,
					cosTeta,
					sinTeta,
					pFieldBufferA,
					width,
					height,
					extractMargin
			);

	if(err < 0) {
		return(err);
	}
	*pwidth = width + 2*extractMargin;
	*pheight = height + 2*extractMargin;

// get rid of compiler warnings
	{WORD  * pP;
	sizeFieldBufferAlloc = sizeFieldBuffer;
	pP = pwFieldBufferA;
	pwFieldBufferA = pP;
	}
//	 sprintf(buff," fName= %s, *pwidth= %d, *pheight= %d \n", fName, *pwidth, *pheight);
//	 saveLog("", buff);

	return(0);
}
int  extractFieldAdj(LPBYTE pFormBufferA, WORD widthFormA, WORD heightFormA,
		LPWORD axInp, LPWORD ayInp, LPWORD bxInp, LPWORD byInp, LPWORD dxInp, LPWORD dyInp,
		LPHLPBYTE hpFieldBufferAInp, WORD *pwidth, WORD *pheight, WORD extractMargin, char *fName,
		int angleFlag, double *angleOut){
	int err;
	int x;
	int y;
	double r;
	long cosTeta;
	long sinTeta;
	WORD width;
	WORD height;
	WORD byteWidth;
	DWORD sizeFieldBufferAlloc;
	DWORD sizeFieldBuffer;
	// HGLOBAL hgFieldBufferA;
	BYTE  * pFieldBufferA;
	WORD  * pwFieldBufferA;
	DWORD * pdwFieldBufferA;

	DWORD size;
	DWORD offset;
	DWORD dw0=0, dw1=0;

	WORD byteWidthHist = ((widthFormA + 31)/32)*4;
	float maxAlpha = 0.9;
	float stepAlpha = 0.1;
	float ang, ang2;
	// int  scAngleH;
	float shfx1, shfx2, shfy1, shfy2;
	float siN, coS, tG;
	int angleTest;
	int ax, ay, bx, by, dx, dy;
	ax = *axInp; ay = *ayInp; bx = *bxInp; by = *byInp; dx = *dxInp; dy = *dyInp;

//	if(strcmp(fName, "NUMBERS") == 0) {
//		   printf("1------------ ax= %d bx= %d \n", ax, bx);
//	}
//	fflush(stdout);

	byteWidthT = ((widthFormA + 31)/32)*4;
	//	 char buff[500];

//	printf("------------ fName= %s------------------ \n", fName);
//    printf("------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx= %d, by = %d \n", widthFormA, heightFormA, ax, ay, bx, by);
//    printf("------------ dx= %d, dy= %d, extractMargin= %d, angleFlag= %d\n",  dx, dy, extractMargin, angleFlag);

//    printf("------------ angleOut= %e\n",  *angleOut);
//    fflush(stdout);

	// x shift
	ang = *angleOut;
	if(angleFlag != 0) {

		angleTest = getAngle(pFormBufferA, widthFormA, heightFormA, ax, ay, bx-ax, dy-ay,
				-60, 60, 5);
//		 if(strcmp(fName, "NUMBERS") == 0) {
//		 if(strcmp(fName, "DATEA") == 0) {
//			   printf("------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
//		 printf("------------ angleTest= %d\n", angleTest);
//		 }
		//sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
		//saveLog("", buff);

		//  getAngleFromHistH(pFormBufferA, byteWidthHist, heightFormA,
		// 		              ax, ay, bx, dy, maxAlpha, stepAlpha, &ang);

		//  ang  = -(float)angleTest/10.0;
		ang2 = -(float)angleTest/10.0;

//		if(strcmp(fName, "NUMBERS") != 0 || strcmp(fName, "NUMBERS1") != 0){

		if(US_State != 2 && strcmp(fName, "NUMBERS") != 0 && strcmp(fName, "NUMBERS1") != 0){
			getAngleFromHistH(pFormBufferA, byteWidthHist, heightFormA,
					ax, ay, bx, dy, maxAlpha, stepAlpha, &ang);
//      printf("------------ angleTest= %d, ang2= %e, ang= %e\n", angleTest, ang2, ang);
			if(fabs(ang-ang2) < 1)
				ang = ang2;
		}else
			ang  = ang2;

//	if(strcmp(fName, "NUMBERS") == 0) {
//	 if(strcmp(fName, "DATEA") == 0) {
//         printf("------------ ang= %e\n",  ang);
	//}
		// if(strcmp(fName, "NUMBERS") == 0)
		//   printf("------------ ang= %e \n", ang);

		//   sprintf(buff,"------------ ang= %e \n", ang);
		//   saveLog("", buff);

		//  ang = 3;
	}
//	if(strcmp(fName, "NUMBERS") == 0) {
//		   printf("1------------ ax= %d bx= %d \n", ax, bx);
//	}
//	fflush(stdout);
//	 if(strcmp(fName, "NUMBERS") == 0)
//  printf("1------------ ang= %e \n", ang);


//	if(US_State == 2) {
//		if(fabs(ang) < fabs(fieldAngleFromBarCode) ){ char buff[200];
//			 sprintf(buff, "------------ fieldAngleFromBarCode= %e, ang= %e \n", fieldAngleFromBarCode, ang);
//		     saveLog("", buff);
//	         ang = fieldAngleFromBarCode;
//		}
//	}

//	if(strcmp(fName, "NUMBERS") == 0)
//      printf("2------------ ang= %e, fieldAngleFromBarCode= %e\n", ang, fieldAngleFromBarCode);
//	  ang = 0.6;

	*angleOut = ang;
//	aaa


	siN = sin((double)(ang*PI)/180.0);
	coS = cos((double)(ang*PI)/180.0);
	tG  = tan((double)(ang*PI)/180.0);
	//   printf("------------ siN= %e, coS= %e, tG= %e \n", siN, coS, tG);

	shfy1 = (bx-ax)*siN;
	shfx1 = shfy1*tG;
	shfx2 = (dy-ay)*siN+shfx1;
	shfy2 = (dy-ay)*coS-(dy-ay-shfy1);
	//   printf("------------ shfx1= %e, shfx2 = %e, shfy1= %e, shfy2= %e \n", shfx1, shfx2, shfy1, shfy2);

	//   printf("------------ ax= %d bx= %d \n", ax, bx);
	//   printf("------------ ay= %d by= %d \n", ay, by);
	//   printf("------------ dx= %d dy= %d \n", dx, dy);

	ax = ax + (int)(shfx1+0.5);
	ay = ay + (int)(shfy1+0.5);
	dx = dx + (int)(shfx2+0.5);
	dy = dy + (int)(shfy2+0.5);
	if(ax >= widthFormA) ax = widthFormA-1;
	if(ax < 0) ax = 0;
	if(ay >= heightFormA) ay = heightFormA-1;
	if(ay < 0) ay = 0;
	if(dx >= widthFormA) dx = widthFormA-1;
	if(dx < 0) ax = 0;
	if(dy >= heightFormA) dy = heightFormA-1;
	if(dy < 0) dy = 0;

//	   sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx= %d, dy = %d \n", widthFormA, heightFormA, ax, ay, bx, dy);
//	   saveLog("", buff);

	//   sprintf(buff,"------------ widthFormA= %d, heightFormA= %d, ax= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthFormA, heightFormA, ax, ay, bx-ax, dy-ay);
	//   saveLog("", buff);
/*
if(strcmp(fName, "NUMBERS") == 0) {
	   printf("------------ ax= %d bx= %d \n", ax, bx);
	   printf("------------ ay= %d by= %d \n", ay, by);
	   printf("------------ dx= %d dy= %d \n", dx, dy);
}
fflush(stdout);
*/
    *axInp = ax; *ayInp = ay; *bxInp = bx; *byInp = by; *dxInp = dx; *dyInp = dy;

	x = bx - ax;
	y = by - ay;
	r = sqrt((double)((long)x*(long)x + (long)y*(long)y));
	width = ((WORD)r) + 1;
	if(r != 0.0)
	{cosTeta = (long)((x/r)*SCALE);
	sinTeta = (long)((y/r)*SCALE);
	}
	else
	{cosTeta = SCALE;
	sinTeta = 0L;
	}
	x = dx - ax;
	y = dy - ay;
	r = sqrt((double)((long)x*(long)x + (long)y*(long)y));
	height = ((WORD)r) + 1;

	if(ax+width >= widthFormA)
		width = widthFormA-1 - ax;
	if(ay+height >= heightFormA)
		height = heightFormA-1 - ay;


	byteWidth = ((width + 2*extractMargin + 31)/32)*4;


	if(widthFormA < width)
		return (-100);

	if(heightFormA < height)
		return (-100);

	// sizeFieldBufferAlloc =
	//     (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin) +
	//     (DWORD)(offset/2) + 100;

	// sizeFieldBuffer = (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin);

	// hgFieldBufferA = *phgFieldBufferA;

	if(hpFieldBufferAInp->ptr != NULL)
   {
// printf("------------ fName= %s; pdwFieldBufferA \n", fName);
		pdwFieldBufferA = (DWORD *)(hpFieldBufferAInp->ptr);
	   dw0 =  *pdwFieldBufferA; dw1 = *(pdwFieldBufferA + 1);
	   size = dw0;
	   if(size != 0L) offset = 8;
	   else offset = dw1;
//   printf("------------ size= %d, offset= %d  \n", size, offset);
	}
	else offset = 8;

//   printf("------------ size= %d, offset= %d  \n", size, offset);

	sizeFieldBufferAlloc =
			(DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin) +
			(DWORD)(offset/2) + 100L;
	sizeFieldBuffer = (DWORD)(byteWidth)*(DWORD)(height + 2*extractMargin);

	recalculateAllocBuffer(&sizeFieldBufferAlloc, 300000);
	err = reallocMemoryBuffer (hpFieldBufferAInp, sizeFieldBufferAlloc);
	if(err < 0) {
		//       	sprintf(pImg->error,"createZoneStruct (\"%s\") yl_calloc failed: %s\n",pImg->szIMGName, strerror(errno));
		return -1;
	}
//   printf("------------ sizeFieldBufferAlloc= %u, sizeFieldBuffer= %u  \n", sizeFieldBufferAlloc, sizeFieldBuffer);
//   printf("------------ hpFieldBufferAInp->size= %lu  \n", hpFieldBufferAInp->size);

	// hgFieldBufferA = *phgFieldBufferA;
	pFieldBufferA = (BYTE *)hpFieldBufferAInp->ptr;
	pwFieldBufferA = (WORD *)pFieldBufferA;
	if(offset != 8)
	{pdwFieldBufferA = (DWORD *)pFieldBufferA;
	*pdwFieldBufferA        = dw0;
	*(pdwFieldBufferA + 1L) = dw1;
	}

	pFieldBufferA = pFieldBufferA + offset;
// printf(" total size= %ld, offset= %d, sizeFieldBufferAlloc= %d \n", hpFieldBufferAInp->size, offset, sizeFieldBufferAlloc);

//	 sprintf(buff," total size= %ld, offset= %d, sizeFieldBufferAlloc= %d \n", hpFieldBufferAInp->size, offset, sizeFieldBufferAlloc);
//	 saveLog("", buff);
	// sprintf(buff," fieldStart= %u, fieldEnd= %u \n", (unsigned)pFieldBufferA, (unsigned)(pFieldBufferA+ sizeFieldBufferAlloc));
	// saveLog("", buff);

//	 printf(" widthFormA= %d, heightFormA= %d, ax= %d, ay= %d \n", widthFormA, heightFormA, ax, ay);
//	 printf(" cosTeta= %ld, sinTeta= %ld, width= %d, height= %d, extractMargin= %d \n", cosTeta, sinTeta, width, height, extractMargin);
// printf("------------ fName= %s; pdwFieldBufferA \n", fName);

//    fflush(stdout);

	err = desKew
			(pFormBufferA,
					widthFormA,
					heightFormA,
					ax,
					ay,
					cosTeta,
					sinTeta,
					pFieldBufferA,
					width,
					height,
					extractMargin
			);

	if(err < 0) {
		return(err);
	}
	*pwidth = width + 2*extractMargin;
	*pheight = height + 2*extractMargin;

//	 sprintf(buff," fName= %s, *pwidth= %d, *pheight= %d \n", fName, *pwidth, *pheight);
//	 saveLog("", buff);
// get rid of compiler warnings
	{ WORD *pP;
	sizeFieldBufferAlloc = sizeFieldBuffer;
	pP = pwFieldBufferA;
	pwFieldBufferA = pP;
	}

	return(0);
}

int desKew(BYTE * pFormBuffer, WORD widthForm, WORD heightForm, int originX, int originY,
		long cosTeta, long sinTeta, BYTE * pFieldBuffer, WORD width, WORD height, WORD extractMargin){
	WORD i,m;
	int j;
	DWORD k;
	BYTE * pField;
	BYTE byteForm;
	BYTE byteField;
	WORD x,y;
	long xScale,yScale;
	WORD byteWidth;
	DWORD sizeFieldBuffer;
	DWORD halfScale;
	WORD extractMargin8;
	WORD width8;
	WORD scaleexp;
	BYTE masc[] = {127, 191, 223, 239, 247, 251, 253, 254};
	int j1,j2,j3,jmin,jmax;

	// char buff[500];

//	  printf(" widthForm= %d, heightForm= %d, originX= %d, originY = %d \n", widthForm, heightForm, originX, originY);
//	  printf(" width= %d, height= %d \n", width, height);

	// sprintf(buff," widthForm= %d, heightForm= %d, originX= %d, originY = %d \n", widthForm, heightForm, originX, originY);
	// saveLog("", buff);
	// sprintf(buff," width= %d, height= %d \n", width, height);
	// saveLog("", buff);



	scaleexp = 16;
	halfScale = 1L<<15;
	extractMargin8 = extractMargin/8;

	byteWidth = ((width + 2*extractMargin + 31)/32)*4;
	sizeFieldBuffer = (DWORD)byteWidth*(DWORD)(height + 2*extractMargin);
	width8 = width/8;

	j1 = (int)(originY + ((width*sinTeta)>>scaleexp));
	j2 = (int)(originY + ((height*cosTeta)>>scaleexp));
	j3 = (int)(originY + ((width*sinTeta + height*cosTeta)>>scaleexp));

	jmin = (int)(originY - 1);
	if(j1<jmin) jmin = j1;
	if(j2<jmin) jmin = j2;
	if(j3<jmin) jmin = j3;
	if(jmin<0) jmin = 0;

	jmax = (int)(originY + 1);
	if(j1>jmax) jmax = j1;
	if(j2>jmax) jmax = j2;
	if(j3>jmax) jmax = j3;
	if(jmax>=(int)heightForm) jmax = (int)(heightForm - 1);

	for(j=jmin; j<=jmax; j++)
		pFormLine[j] =
				pFormBuffer + (long)(heightForm - j - 1)*(long)byteWidthT;

	for(k=0L; k<sizeFieldBuffer; k++) *(pFieldBuffer+k) = 255;
	/*
 sprintf(buff," jmin= %d, jmax= %d\n", jmin, jmax);
 saveLog("", buff);
// printf(" width= %d, height= %d \n", width, height);
 {
 sprintf(buff," widthForm= %d, heightForm= %d, byteWidthT= %d \n", widthForm, heightForm, byteWidthT);
 saveLog("", buff);
 sprintf(buff," width= %d, height= %d, byteWidth= %d, extractMargin= %d, extractMargin8= %d\n", width, height, byteWidth, extractMargin, extractMargin8);
 saveLog("", buff);
 { unsigned int aa, bb;
    aa = heightForm*byteWidthT;
    bb = (unsigned int)(pFormBuffer+(long)(heightForm)*(long)byteWidthT);
 sprintf(buff," sizeForm= %u, formStart= %u, formEnd= %u\n", aa, (unsigned int)pFormBuffer, bb);
 saveLog("", buff);
 }
 }
	 */
	// return -1;
	for(j=0; j<(int)height; j++)
	{pField = pFieldBuffer +
		(long)(height + extractMargin - j - 1)*(long)byteWidth +
		(long)extractMargin8;

	//  sprintf(buff," pField= %u\n", (unsigned)pField);
	//  saveLog("", buff);


	xScale = halfScale - j*sinTeta;
	yScale = halfScale + j*cosTeta;

	for(i=0; i<width8; i++)
	{byteField = 255;
	for(m=0; m<8; m++)
	{x = originX + (int)(xScale>>scaleexp);
	y = originY + (int)(yScale>>scaleexp);
	byteForm = *(pFormLine[y]+(long)(x>>3));

	//           sprintf(buff,"1 y= %d, byteForm= %u\n", y, (unsigned)(pFormLine[y]+(long)(x>>3)));
	//           saveLog("", buff);

	if(!((byteForm<<(x%8))&128)) byteField &= masc[m];
	xScale += cosTeta;
	yScale += sinTeta;
	}
	*(pField+(long)i) = byteField;
	}
	if((width%8) != 0)
	{byteField = 255;
	for(m=0; m<(width%8); m++)
	{x = originX + (int)(xScale>>scaleexp);
	y = originY + (int)(yScale>>scaleexp);
	byteForm = *(pFormLine[y]+(long)(x>>3));
	//    sprintf(buff,"2 y= %d, byteForm= %u\n", y, (unsigned)(pFormLine[y]+(long)(x>>3)));
	//    saveLog("", buff);

	if(!((byteForm<<(x%8))&128)) byteField &= masc[m];
	xScale += cosTeta;
	yScale += sinTeta;
	}
	*(pField+(long)width8) = byteField;
	}
	}
	// sprintf(buff," out\n");
	// saveLog("", buff);
	return(0);
}

int RecognitionCall (LPIMGDATA img, LPTICKETDATA ticketDataPtr) {
	BYTE* lpProt;
	BYTE* pFieldDescr;
	BYTE * pDataBlock;
	LPFIELDDATA field = ticketDataPtr->fieldAddress;
	IMGDATA fieldImg;
	LPBYTE ptr = ticketDataPtr->fieldBMPPtr;
	DWORD bitsOfset = *(LPDWORD)(ptr + BFOOFBITSOFF);
	LPBYTE biPtr = ptr + BFOOFBITSOFF+4;

	lpProt = ticketDataPtr->prototypePtr;
/*
	if(lpProt == NULL){
		LOGD("1. RecognitionCall(); lpProt=NULL!!!; field name:  %s\n", field->name);
	}
	else{
		LOGD("1. RecognitionCall(); dpiX= %d, field name:  %s\n", *(LPWORD)(lpProt + OffFdDpiX), field->name);
	}
*/

	fieldImg.pixPtr = ptr+bitsOfset;
	fieldImg.bi.biWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	fieldImg.bi.biHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	fieldImg.bi.biXPelsPerMeter = *(LPDWORD)(biPtr + BIXPELSPERMETEROFF);
	fieldImg.bi.biYPelsPerMeter = *(LPDWORD)(biPtr + BIYPELSPERMETEROFF);

	fieldImg.bi.biWidthPad = ((fieldImg.bi.biWidth+31)/32)*4;

	blackMarginRemoval(&fieldImg, ticketDataPtr->fieldAddress->name);

//	printf(" field name:  %s\n", field->name);
//	printf("-------------------------!!! fieldImg.bi.biWidth= %d\n", fieldImg.bi.biWidth);


	strcpy(fieldImg.szIMGName, img->szIMGName);
	//	lpProt = ticketDataPtr->prototypePtr;
	pFieldDescr = lpProt + field->descr;


//	LOGD("ticketDataPtr->workarea: %p", &ticketDataPtr->workArea);
//	LOGD("workarea ptr: %p", ticketDataPtr->workArea.ptr);

//LOGD("1. RecognitionCall; size of workAreaPtr:  %ld\n", ticketDataPtr->workArea.size);
//	reallocMemoryBuffer(&ticketDataPtr->workArea, (DWORD)(MAXCHARS * 22 + 4));
	if(sizeof(DWORD)*(MAXCHARS * 22 + 4) > ticketDataPtr->workArea.size  )
		return -1;
	pDataBlock = ticketDataPtr->workArea.ptr;

//LOGD("2. RecognitionCall; size of workAreaPtr:  %ld\n", ticketDataPtr->workArea.size);

/*
	LOGD("pDataBlock=%p", pDataBlock);

	if(lpProt == NULL){
		LOGD("2. RecognitionCall(); lpProt=NULL!!!; field name:  %s\n", field->name);
	}
	else{
		LOGD("2. RecognitionCall(); dpiX= %d, field name:  %s\n", *(LPWORD)(lpProt + OffFdDpiX), field->name);
	}

	LOGD("RecognitionCall pDataBlock=%p", pDataBlock);
*/
	ticketDataPtr->strings.lpCand1  = (char*)pDataBlock;
	ticketDataPtr->strings.lpCand2  = (char*)(pDataBlock + MAXCHARS + 1);
	ticketDataPtr->strings.lpResult = (char*)(pDataBlock + MAXCHARS * 2 + 2);
	ticketDataPtr->strings.lpReject = (char*)(pDataBlock + MAXCHARS * 3 + 3);
	ticketDataPtr->flConf = (LPWORD) (pDataBlock  + MAXCHARS * 4 + 4);
	ticketDataPtr->flQuad = (LPQUADRANGLE)(pDataBlock  + MAXCHARS * 6 + 4);

	ticketDataPtr->sizeWImg = img->bi.biWidth;
	ticketDataPtr->sizeHImg = img->bi.biHeight;
/*
	{char buffLog[500];
	if(lpProt == NULL)
		sprintf(buffLog,"3. RecognitionCall(); lpProt=NULL!!!; field name:  %s\n", field->name);
	else
		sprintf(buffLog,"3. RecognitionCall(); dpiX= %d, field name:  %s\n", *(LPWORD)(lpProt + OffFdDpiX), field->name);
	LOGD(buffLog);
	}

	LOGD("  bitsOfset= %d, biWidth= %d, biHeight=%d, biXPelsPerMeter= %d, biYPelsPerMeter= %d \n", bitsOfset, fieldImg.bi.biWidth, fieldImg.bi.biHeight, fieldImg.bi.biXPelsPerMeter, fieldImg.bi.biYPelsPerMeter);
	//return 0;
	//   saveLog("", buff);
	LOGD("Calling recognition, tdp: %p, &tdp->strings:%p", ticketDataPtr, &ticketDataPtr->strings);
*/

	return  Recognition(&fieldImg, ticketDataPtr, pFieldDescr, *(LPWORD)(lpProt + OffFdDpiX),  *(LPWORD)(lpProt + OffFdDpiY),  ticketDataPtr->wCorrectionLevel,
			&ticketDataPtr->strings, ticketDataPtr->flConf, ticketDataPtr->flQuad);

//	LOGD("after Recognition  \n");
	//    saveLog("", buff);


	//	char zeroLine[1] = "";
	//	ticketDataPtr->strings.lpResult = zeroLine;
	//	ticketDataPtr->strings.lpCand1  = zeroLine;
	//	ticketDataPtr->strings.lpCand2  = zeroLine;
	//	ticketDataPtr->strings.lpReject = zeroLine;
//	    return 0;
}
void histVPtrPix(LPBYTE ptrPix, int widthImg, int hightImg, int MaxSize, int x1, int x2, int y1, int y2, int *vHist)
{
	int   j, kk, curInd, yy;
	int  yD, ch;
	int lByte, rByte, rL, rR, w_bt;
	long nY;
	BYTE *hptr;
	BYTE mask;

	w_bt= widthImg;
	memset(vHist,0x00,(unsigned int)(sizeof(int)*MaxSize));
	vHist[0]=x2-x1+1;
	if(vHist[0] >= MaxSize) {x2 = x1+MaxSize-2;vHist[0]=x2-x1+1;}

	lByte=x1/8;
	rL   =x1%8;
	if(rL!=0) {lByte++;}

	rByte=x2/8;
	rR   =x2%8;
	if(rR!=7) {rByte--;}

	for(yy=y1;yy<=y2;yy++) {
		yD=yy;
		yD= hightImg-1-yD;
		nY=(long)yD*(long)w_bt;
		hptr=ptrPix+nY+(long)(lByte-1);

		curInd=1;
		if(rL!=0) {
			curInd=9-rL;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				mask >>=rL;
				for(kk=1;kk<curInd;kk++) {
					ch=(~hptr[0])&mask;
					if(ch!=0) {
						vHist[kk]++;
					}
					mask >>=1;
				}
			}
		}

		for(j=lByte;j<=rByte;j++) {
			hptr+=1L;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				for(kk=0;kk<8;kk++) {
					ch=(~hptr[0])&mask;
					if(ch!=0) {
						vHist[curInd]++;
					}
					curInd++;
					mask >>=1;
				}
			}
			else {curInd+=8;}
		}

		if(rR!=7) {
			hptr+=1L;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				for(kk=0;kk<=rR;kk++) {
					ch=(~hptr[0])&mask;
					if(ch!=0) {
						vHist[curInd]++;
					}
					curInd++;
					mask >>=1;
				}
			}
		}
	}
}/* of histVPtrPix()*/

void cleanMargin(LPBYTE ptrPix, int widthImg, int hightImg, int x1, int x2){
	int   j, kk, curInd,  yy;
	int  yD;
	int lByte, rByte, rL, rR, w_bt;
	long nY;
	BYTE *hptr;
	BYTE mask;

	w_bt= widthImg;
	lByte=x1/8;
	rL   =x1%8;
	if(rL!=0) {lByte++;}

	rByte=x2/8;
	rR   =x2%8;
	if(rR!=7) {rByte--;}

	for(yy=0;yy<hightImg;yy++) {
		yD=yy;
		yD= hightImg-1-yD;
		nY=(long)yD*(long)w_bt;
		hptr=ptrPix+nY+(long)(lByte-1);

		curInd=1;
		if(rL!=0) {
			curInd=9-rL;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				mask >>=rL;
				for(kk=1;kk<curInd;kk++) {
					//                          mask=LEFT_BIT;
					//                          mask >>=kk;
					hptr[0]|=mask;

					//	                      ch=(~hptr[0])&mask;
					//	                      if(ch!=0) {
					//	                           vHist[kk]++;
					//	                           }
					mask >>=1;
				}
			}
		}

		for(j=lByte;j<=rByte;j++) {
			hptr+=1L;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				for(kk=0;kk<8;kk++) {
					hptr[0]|=mask;
					//	                      ch=(~hptr[0])&mask;
					//	                      if(ch!=0) {
					//	                           vHist[curInd]++;
					//	                           }
					//	                      curInd++;
					mask >>=1;
				}
			}
			else {curInd+=8;}
		}

		if(rR!=7) {
			hptr+=1L;
			if((*hptr)<255) {
				mask=LEFT_BIT;
				for(kk=0;kk<=rR;kk++) {
					hptr[0]|=mask;
					//	                      ch=(~hptr[0])&mask;
					//	                      if(ch!=0) {
					//	                           vHist[curInd]++;
					//	                           }
					//	                      curInd++;
					mask >>=1;
				}
			}
		}
	}
}/* of cleanMargin()*/

void getBmarginIdx(int *vHist, int h_size, int th, int* i1, int* i2){
	int mass, flg;
	int i;

	mass = 0;
	flg = 0; *i1 = 0; *i2 = 0;
	for(i=3;i<h_size;i++){
		mass = (vHist[i-2]+vHist[i-1]+vHist[i])/3;
		if(mass > th){
			if(flg == 0){
				*i1 = i-2;
				flg++;
			}
		}
		if(mass < th){
			if(flg == 1){
				*i2 = i;
				flg++;
			}
		}
		if(flg > 1) break;
	}
	if(*i1 > 0 && *i2 == 0)
		*i2 = h_size-1;
}
void getBmarginIdx_0(int *vHist, int h_size,  int* i1, int* i2){
	int av = 0;
	int i, cc, k;

	*i1 = 0; *i2 = 0;
	cc = 0;
	for(i=10;i<h_size-10;i++){
		if(vHist[i] > 5){
			av += vHist[i];
			cc++;
		}
	}
	if(cc>0)
		av /= cc;
	av += av;
	//    printf("   av= %d\n", av);

	cc = 0;
	for(i=1;i<h_size;i++){
		if(vHist[i] > av || i > 150){
			cc = i;
			break;
		}
	}
	//	printf("   cc= %d \n", cc);

	k = 0;
	if(cc > 0) {
		for(i=cc;i<h_size;i++){
			if(vHist[i] < av || i > 250){
				k = i;
				break;
			}
		}
		if(k-cc>8){
			//	printf("   k= %d, cc= %d \n", k, cc);
			if(cc > 4) cc--;
			*i1 = cc;
			*i2 = k+1;
		}
	}
	//	printf("   av= %d \n", av);
}
void getBlackLine_L(int *vHist,  int th, int* i1, int* i2){
	int i, cc, k, k1, k2;
	int stop = 100;

	*i1 = 0; *i2 = 0;
	cc = 0;
	k = 0;
	if(vHist[0] < stop) stop = vHist[0];
	for(i=1;i<stop;i++){
		if(vHist[i] > th)
			cc++;
		else
			cc = 0;
		if(cc > 2) {k=i;break;}
	}
	k1 = k;
	k2 = k;
	if(k > 0){
		for(i=k;i>0;i--){
			if(vHist[i] == 0) {k1 = i;break;}
		}
		for(i=k;i<stop;i++){
			if(vHist[i] == 0) {k2 = i;break;}
		}
	}
	if(k2-k1 > 20 || k2-k1 < 2)
		return;
	*i1 = k1; *i2 = k2;
}


void blackMarginRemoval(LPIMGDATA img, char* name){
	int h_size = 500;
	int h_size1 = 400;
	LPBYTE ptr = img->pixPtr;
	int widthImg = img->bi.biWidthPad;
	int hightImg = img->bi.biHeight;
	int vHist[h_size];
	int i1, i2, d;
	// int i;
	histVPtrPix(ptr, widthImg, hightImg, h_size, 0, h_size, 0, hightImg-1, vHist);


	/*
	if(name != NULL && strcmpi(name, "NUMBERS") == 0){
		int i;
		printf("   widthImg= %d, hightImg= %d \n", widthImg, hightImg);
		 for(i=0;i<h_size1;i++){
			    printf("   i= %d, vHist= %d \n", i, vHist[i]);
		  }
	}
	 */

	getBmarginIdx_0(vHist, h_size,  &i1, &i2);
	d = i2-i1;
	if(d > 5) {
// printf("1-------   i1= %d, i2= %d, d= %d  \n", i1, i2, d);
		cleanMargin(ptr, widthImg, hightImg, i1, i2);
	}
	else {
		getBmarginIdx(vHist, h_size, (2*hightImg/3), &i1, &i2);

		//   printf("   i1= %d, i2= %d \n", i1, i2);

		d = i2-i1;
		if(d > 5) {
			if(d< 200) i2 +=8;
			i1 -= 8;
			if(i1 < 0) i1 = 0;
			cleanMargin(ptr, widthImg, hightImg, i1, i2);
		}
	}
	histVPtrPix(ptr, widthImg, hightImg, h_size, 0, h_size, 0, hightImg-1, vHist);
	getBlackLine_L(vHist, (hightImg*4)/5,  &i1, &i2);
	d = i2-i1;
	//   printf("   i1= %d, i2= %d, d= %d \n", i1, i2, d);
	if(d > 0) {
		cleanMargin(ptr, widthImg, hightImg, i1, i2);
	}


	histVPtrPix(ptr, widthImg, hightImg, 500, img->bi.biWidth-400, img->bi.biWidth-1, 0, hightImg-1, vHist);

	getBmarginIdx(vHist, h_size1, (2*hightImg/3), &i1, &i2);
	d = i2-i1;
	if(d > 5) {
		if(d< 200) i1 -=8;
		cleanMargin(ptr, widthImg, hightImg, img->bi.biWidth-400+i1, img->bi.biWidth-400+i2);
	}


}

int   histTmp[3000];
/*
double getFieldMass(LPTICKETDATA ticketDataPtr){
	double mass = 0;
	int i;
	LPBYTE ptr = ticketDataPtr->fieldBMPPtr ;

	LPBYTE biPtr = ptr + BFOOFBITSOFF+4;
	int wWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	int wWidthPad = ((wWidth + 31) /32) * 4;

	histVPtrPix(ptr, wWidthPad, wHeight, wWidth-1, 0, wWidth-1, 0, wHeight-1, histTmp);
	for(i=1;i<wWidth;i++){
		mass += histTmp[i];
	}
	mass = ((double)(wWidthPad*wHeight))/mass;
	//    printf("   mass= %e \n", mass);

	return mass;
}
*/
void copyFieldResults(LPTICKETRESULTS ticketResults, FIELD* f, int idx, char *name, LPTICKETDATA ticketDataPtr){
	int i, len;
	if(idx < NUMFIELDS_RESULTS) {
		len = strlen(f->result);
		strcpy(ticketResults->fieldsResults[idx].name, name);
		strcpy(ticketResults->fieldsResults[idx].result, f->result);
//		strcpy(ticketResults->fieldsResults[idx].cand2, f->cand2);
		for(i=0;i<len;i++){
			ticketResults->fieldsResults[idx].flConf[i] = f->conf[i];
		}
		copyQuad(ticketResults->fieldsResults[idx].flQuad, f->quad, 0, len-1);
		copyLineQuads(&ticketResults->fieldsResults[idx], ticketDataPtr);
	}
}
void setTicketTypeFromMass(LPTICKETRESULTS ticketResults, double fMass){
	int i;
	ticketResults->type = -1;
	ticketResults->typeCnf = 0;
	if(fMass < 0.5) ticketResults->type = 0; // Mega million
	if(fMass > 0.8) ticketResults->type = 1; // Power ball
	for(i=0;i<NUMFIELDS_RESULTS;i++){
		ticketResults->fieldsResults[i].name[0] = 0;
	}
}


int testLogoField2(LPTICKETDATA ticketDataPtr, int y1){
	//        int i, k, cc;
	int i, k;
	int start, startN, stop;
	//        char buff[500];
	int d, yShift, cc;
	double ccD;
	//        int mass;

	LPBYTE ptr = ticketDataPtr->fieldBMPPtr;

	LPBYTE biPtr = ptr + BFOOFBITSOFF+4;

	int wWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);

	int wWidthPad = ((wWidth + 31) /32) * 4;
	maxBlackWidth(ptr, wWidthPad, wHeight,  8, wWidth-9, 8, wHeight-9, histTmp, 2990);


	// clean trash
	{int s1[5], s2[5];
	for(i = 0;i<5;i++) {
		s1[i] = 0; s2[i] = 0;
	}
	k = 0;
	for(i = 1;i<histTmp[0];i++) {
		if(histTmp[i] > 7) {if( s1[k] == 0) {s1[k] = i;}}
		else {if(s1[k] != 0){ s2[k] = i;k++;}}
		if(k>=5)break;
	}
	if(k >= 5)k--;
	if(s1[k] != 0){ s2[k] = histTmp[0];}
	for(i = 0;i<5;i++) {
		//      	     sprintf(buff,"s1[i]= %d, s2[i]= %d\n",s1[i], s2[i]);
		//  	      	     saveLog("", buff);

		if(s1[i]<s2[i] && s2[i]-s1[i] < 40){
			if(s1[i] > 10) s1[i] -= 4;
			if(s2[i] < histTmp[0]-10) s2[i] += 4;
			for(k = s1[i];k<s2[i];k++)
				histTmp[k] = 0;
		}
	}
	}


	// logo start, stop
	start = histTmp[0];
	for(i = 1;i<histTmp[0];i++) {
		if(histTmp[i] > 2) {start = i;break;}
		//   	     sprintf(buff,"mass; i= %d, histTmp[i]= %d\n",i, histTmp[i]);
		//   	     saveLog("", buff);
	}
	stop = 1;
	for(i = histTmp[0]-1;i>0;i--) {
		if(histTmp[i] > 2) {stop = i;break;}
		//   	     sprintf(buff,"mass; i= %d, histTmp[i]= %d\n",i, histTmp[i]);
		//   	     saveLog("", buff);
	}
	d = stop-start;

	// shift up
	if(start < 5 && histTmp[0]-stop > 50 && d > 5){
		yShift = histTmp[0]-stop - 10;
		//    	      sprintf(buff,"-----------shift up   yShift= %d\n",yShift);
		//    	      saveLog("", buff);
		return yShift;
	}

	// maybe shift up
	startN = start;
	if(d < 100 && start < 50 && histTmp[0]-stop > 50){
		for(i = 1;i<start;i++) {
			if(histTmp[i] > 1) {startN = i;break;}
		}
		d = stop-startN;
		if(startN < start && startN < 20 && d > 10) {
			yShift = histTmp[0]-stop - 10;
			//          	    sprintf(buff,"-----------shift up   yShift= %d, d= %d\n",yShift, d);
			//          	    saveLog("", buff);
			return yShift;
		}

	}
	// shift down
	if(start > 50 && histTmp[0]-stop < 5){
		yShift = 10 - start;
		//       	      sprintf(buff,"-----------shift down   histTmp[0]-stop - 10= %d\n",yShift);
		//       	      saveLog("", buff);
		return yShift;
	}
	if(d < 40 || d > 300){
		cc = 0;
		for(i = 2;i<histTmp[0];i++) {
			if(histTmp[i]!=histTmp[i-1]) cc++;
		}
		ccD = (double)cc/(double)histTmp[0];
		if(ccD>0.05 && y1 < 300){
			// shift up
			yShift = 140;
			if(yShift> y1-8) yShift = y1-8;
			return yShift;
		}
		if(ccD<0.02){
			// shift down
			yShift = -140;
			return yShift;
		}

	}

	return 0;
}

int getLogoType(LPTICKETDATA ticketDataPtr){
	int i;
	int start,  stop, mass;
	//        char buff[500];
	int d;
	LPBYTE ptr = ticketDataPtr->fieldBMPPtr;

	LPBYTE biPtr = ptr + BFOOFBITSOFF+4;
	int wWidth = *(LPDWORD)(biPtr + BIWIDTHOFF);
	int wHeight = *(LPDWORD)(biPtr + BIHEIGHTOFF);
	int wWidthPad = ((wWidth + 31) /32) * 4;

	maxBlackWidth(ptr, wWidthPad, wHeight,  8, wWidth-9, 8, wHeight-9, histTmp, 2990);
	// logo start, stop
	start = histTmp[0];
	for(i = 1;i<histTmp[0];i++) {
		if(histTmp[i] > 2) {start = i;break;}
		//   	     sprintf(buff,"mass; i= %d, histTmp[i]= %d\n",i, histTmp[i]);
		//   	     saveLog("", buff);
	}
	stop = 1;
	for(i = histTmp[0]-1;i>0;i--) {
		if(histTmp[i] > 2) {stop = i;break;}
		//   	     sprintf(buff,"mass; i= %d, histTmp[i]= %d\n",i, histTmp[i]);
		//   	     saveLog("", buff);
	}
	d = stop-start+1;
	mass = 0;
	for(i = start;i<=stop;i++) {
		mass += histTmp[i];
	}
	// 	      mass /= d;
	// 	 	     sprintf(buff,"------------------   start= %d, stop= %d, mass= %d, stop-start= %d\n",start, stop, mass, d);
	//	  	     saveLog("", buff);
	if(d > 110) return 0;
	return 1;

}

int extFieldAgain(LPIMGDATA pImg, LPFIELDDATA field, LPTICKETDATA ticketDataPtr, IMAGEH* pFieldH, char* fName, double *angleOut){
	WORD p1X = field->quad.p1X;
	WORD p1Y = field->quad.p1Y;
	WORD p2X = field->quad.p2X;
	WORD p2Y = field->quad.p2Y;
	WORD p4X = field->quad.p4X;
	WORD p4Y = field->quad.p4Y;
	WORD* pwidth;
	WORD* pheight;
	WORD width;
	WORD height;
	int  err;
	WORD * pwBuffer;
	DWORD sizeBuffer;
	WORD wDpiX;
	WORD wDpiY;
	int angleFlag = 1;





	pTBuffer   = pImg ->pixPtr;
	widthT     = pImg -> bi.biWidth;
	heightT    = pImg -> bi.biHeight;
	wDpiX      = pImg -> bi.biXPelsPerMeter;
	wDpiY      = pImg -> bi.biYPelsPerMeter;
	pwidth = &width;
	pheight = &height;
	HLPBYTE hpFieldH;
	hpFieldH.ptr = pFieldH->himgPtr.ptr;

	//       if(strcmp(fName, "NUMBERS") == 0)
	//         printf("----------extFieldAgain; widthT= %d, heightT= %d, p1X= %d, ay= %d, bx-ax= %d, dy-ay = %d \n", widthT, heightT, p1X, p1Y, p2X-p1X, p1Y-p2Y);

	err =
			extractField(pTBuffer, widthT, heightT,
					p1X, p1Y,
					p2X, p2Y,
					p4X, p4Y,
					&pFieldH->himgPtr,
					pwidth,
					pheight,
					EXTRACTMARGIN,
					fName,
					angleFlag,
					angleOut
			);
	if(err < 0) return(err);

	//  hgBuffer = *phgBuffer;

	sizeBuffer = (DWORD)(*pwidth)*(DWORD)(*pheight + 2*EXTRACTMARGIN);

	pwBuffer = (WORD *)pFieldH->himgPtr.ptr;
	*(pwBuffer+2)= (WORD)(sizeBuffer%65536);
	*(pwBuffer+3)= (WORD)(sizeBuffer/65536);
	//  GlobalUnlock(hgBuffer);
	pFieldH -> wWidth   = *pwidth;
	pFieldH -> wHeight  = *pheight;
	pFieldH -> wDpiX    = wDpiX;
	pFieldH -> wDpiY    = wDpiY;

// get rid of compiler warnings
	{HLPBYTE hpP;
	  hpP = hpFieldH;
	  hpFieldH = hpP;
	}
	return 0;

}

//  pFieldH -> hgBuffer = hgBuffer;

int extractFieldAgain(LPIMGDATA pImg, LPFIELDDATA field, LPTICKETDATA ticketDataPtr, char* fName, double *angleOut){
	LPBITMAPFILEHEADER bf;
	LPBITMAPINFOHEADER bi;
	LPRGBQUAD          qd;
	IMAGEH       fieldH;
	DWORD        offBits;
	int  err;
	LPDWORD      lpImage;

	fieldH.himgPtr.ptr = NULL;
	fieldH.himgPtr.size = 0;

	err = 0;
	fieldH.himgPtr.ptr = yl_calloc("f4", 8, sizeof(BYTE));
	fieldH.himgPtr.size = 8;

	if (err < 0 || fieldH.himgPtr.ptr == NULL) {
		return -11;     // out of memory
	}
	if(ticketDataPtr->fieldBMPPtr) {
		free (ticketDataPtr->fieldBMPPtr);
		ticketDataPtr->fieldBMPPtr = NULL;
	}
	offBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER) +
			sizeof (RGBQUAD) * 2;
	lpImage = (LPDWORD) fieldH.himgPtr.ptr;
	*lpImage = 0;
	*(lpImage + 1) = offBits;

	err = extFieldAgain(pImg, field, ticketDataPtr, &fieldH, fName, angleOut);
	if (err < 0 || fieldH.himgPtr.ptr == NULL) {
		return -11;     // out of memory
	}


	bf = (LPBITMAPFILEHEADER) fieldH.himgPtr.ptr;
	bf->bfType[0] = 0x42;
	bf->bfType[1] = 0x4d;
	set4Bytes(bf->bfSize, offBits + (((fieldH.wWidth /** biBitCount*/ + 31) /32) * 4) * fieldH.wHeight);
	bf->bfReserved1[0] = 0;
	bf->bfReserved1[1] = 0;
	set4Bytes(bf->bfOffBits, offBits);

	bi = (LPBITMAPINFOHEADER)((LPBYTE)bf + sizeof (BITMAPFILEHEADER));
	set4Bytes(bi->biSize, sizeof (BITMAPINFOHEADER));
	set4Bytes(bi->biWidth, fieldH.wWidth);
	set4Bytes(bi->biHeight, fieldH.wHeight);
	bi->biPlanes[0] = 1;
	bi->biPlanes[1] = 0;
	bi->biBitCount[0]= 1;
	bi->biBitCount[1]= 0;
	set4Bytes(bi->biCompression, BI_RGB);
	set4Bytes(bi->biSizeImage, 0);
	set4Bytes(bi->biXPelsPerMeter, fieldH.wDpiX);
	set4Bytes(bi->biYPelsPerMeter, fieldH.wDpiY);
	set4Bytes(bi->biClrUsed, 0);
	set4Bytes(bi->biClrImportant, 0);

	// generate pair of RGBQUADs (black and white)
	qd = (LPRGBQUAD)((LPBYTE)bi + sizeof (BITMAPINFOHEADER));
	*(LPDWORD)qd       = 0x00000000;
	*(LPDWORD)(qd + 1) = 0x00ffffff;

	if(ticketDataPtr->fieldBMPPtr)
		free (ticketDataPtr->fieldBMPPtr);
	ticketDataPtr->fieldBMPPtr = fieldH.himgPtr.ptr;


	ticketDataPtr->fieldAddress = field;
	return 0;
}
int getxShiftForNumbersField(int *vHist, int start, int d){
	int xShift, i, cc, k;
	cc = 0;
	k = 0;
	xShift = 0;
	for(i = start;i>start-d;i--){
		if(vHist[i] > 10) cc++;
		else cc = 0;
		if(cc > 2) {k = i;break;}
	}
	if(k == 0) return xShift;

	for(i = k;i>1;i--){
		if(vHist[i] == 0) {xShift = start -i;break;}
	}
	return xShift;
}
int getYShiftBottomNumbersField(int *vHist, int dY){
	int i, k, ccV;
	int av1, start;

//	yShift = 0;
	av1 = 0;
	start = (dY*2)/3; ccV = 0;k = 0;
	for(i = start;i<vHist[0]-2;i++){
		av1 = (vHist[i]+vHist[i+1])/2;
		if(av1 < 10) ccV++;
		else ccV= 0;

		if(ccV > 8) {k=i;break;}
	}
	if(k < dY) k = 0;
	return k;
}

int getyShiftForNumbersField(int *vHist, int start, int d){
	int yShift, i, cc;
	int av1, av2, min, iMin;
	cc = 0;
	yShift = 0;
	av1 = 0;
	for(i = start;i<vHist[0]-2;i++){
		av1 += vHist[i];
		cc++;
	}
	if(cc > 0) av1 /= cc;

	//        printf("   av1= %d \n", av1);

	if(av1 < 5) return yShift;
	cc = 0;
	av2 = 0;
	for(i = start;i>start-d;i--){
		av2 += vHist[i];
		cc++;
	}
	if(cc > 0) av2 /= cc;

	//        printf("   av2= %d \n", av2);

	if(av2*2 < av1) {
		cc = 0;
		av2 = 0;
		for(i = start;i>start-5;i--){
			av2 += vHist[i];
			cc++;
		}
		if(cc > 0) av2 /= cc;
		//            printf("   av2= %d \n", av2);
		if(av2*2 < av1) return yShift;
	}

	min = 100; iMin = -1;
	for(i = start;i>start-d;i--){
		av1 = (vHist[i]+vHist[i-1]+vHist[i-2])/3;
		if(min > av1) {min = av1;iMin = i;}
	}

	//      printf("   iMin= %d \n", iMin);

	if(iMin < 0) return yShift;
	yShift = start - iMin;
	return yShift;
}
void getBlackLine2_L(int *vHist,  int thR, int* i1, int* i2){
	int i,  k;
	int stop = vHist[0];
	int a1, a2, ratioL, ratioR;
	int a3, a4;
	//    	char buff[500];

	*i1 = 0; *i2 = 0;
//	cc = 0;
	k = 0;
	for(i=1;i<stop-20;i++){
		a1 = (vHist[i]+vHist[i+1])/2;
		a2 = (vHist[i+2]+vHist[i+3])/2;
		ratioL = 0;
		if(a2 > 60) {
			if(a1 == 0 ) ratioL = 100;
			else ratioL = a2/a1;
			if(ratioL > 10){
				a3 = (vHist[i+1]+vHist[i+2])/2;
				a4 = (vHist[i+3]+vHist[i+4])/2;
				if(a4 > a2) {
					if(a3 == 0 ) i++;
					else if(a4/a3 > ratioL) i++;
				}
				//   	   		sprintf(buff," ratioL= %d, i= %d\n",ratioL, i);
				//   	   		saveLog("-------", buff);

				//        printf("   ratioL= %d, i= %d\n", ratioL, i);
				for(k=i+2;k<i+10;k++){
					a1 = (vHist[k]+vHist[k+1])/2;
					a2 = (vHist[k+2]+vHist[k+3])/2;
					ratioR = 0;
					if(a1 > 60) {
						if(a2 == 0 ) ratioR = 100;
						else ratioR = a1/a2;
					}
					//        printf("   ratioR= %d, k= %d\n", ratioR, k);

					//   		sprintf(buff,"   ratioR= %d, k= %d\n", ratioR, k);
					//	   	saveLog("-------", buff);

					if(ratioR>10) {
						*i1 = i+2;*i2 = k+2;return;
					}
				}

			}
		}
	}
}


void verticalLineRemovalField(LPBYTE pTBuffer, int width, int height, int y1, int y2){
	int h_size = width/2+20;
	int vHist[h_size];
	WORD byteWidthHist = ((width + 31)/32)*4;
	int i1, i2, d;
	//    	char buff[500];

	//	  	  int i;

	histVPtrPix(pTBuffer, byteWidthHist, height, h_size, 8, h_size, y1, y2, vHist);
	//		   for(i=0;i<vHist[0];i++){
	//		  		    printf("   i= %d, vHist= %d, h= %d \n", i, vHist[i], y2-y1);
	//		  	  }


	getBlackLine2_L(vHist, 10,  &i1, &i2);
	d = i2-i1;
	//         printf("   i1= %d, i2= %d, d= %d \n", i1, i2, d);
	//    		sprintf(buff,"   i1= %d, i2= %d, d= %d \n", i1, i2, d);
	// 	   	saveLog("-------", buff);

	if(d > 0) {
		cleanMargin(pTBuffer, byteWidthHist, height, i1, i2);
	}
	{int yyC = (y2+y1)/2;
	 int yyD = (y2-y1)/5;
	 int x1, x2;
	 int i, k, n, kSt1, kSt2;
	 int vHistAv10[h_size/10+10];
	 int th1, th2, ratioTh = 18;
	 int ratio1, ratio2, cc1, cc2, cc3;

//	 char bufLog[500];

	 x1 = width/2;x2 = width-1;
	 y1 = yyC-yyD;y2 = yyC+yyD;

	histVPtrPix(pTBuffer, byteWidthHist, height, h_size, x1, x2, y1, y2, vHist);
	    n = 1;
	    kSt1 = 1;
	    kSt2 = 1;
	   for(i=1;i<vHist[0];i++){
		   kSt1 = kSt2;kSt2+=10;
		   if(kSt2 >=vHist[0]) break;
		   vHistAv10[n] = 0;
		   for(k=kSt1;k<kSt2;k++){
			   vHistAv10[n] += vHist[k];
		   }
		   vHistAv10[n] /= (kSt2-kSt1);
		   n++;
	   }
	   vHistAv10[0] = n-1;
	   th1 = (y2-y1)/3; th2 = (y2-y1)/5;
	   n = -1;
	   for(i=vHistAv10[0];i>vHistAv10[0]-5;i--){
		   if(vHistAv10[i]> th2) {n = i;break;}
	   }
	   for(i=n;i>n-10;i--){
		   if(vHistAv10[i]> th1) {n = i;break;}
	   }

// printf("   n= %d, vHistAv10[0]= %d, vHistAv10[vHistAv10[0]]= %d\n", n, vHistAv10[0], vHistAv10[vHistAv10[0]]);

	   if(n < 0) return;
	   k = n;
	   cc1 = 0; cc2 = 0;cc3 = 0;
	   ratio1 = 0;
	   for(i=n;i>2;i--){
		   if(vHistAv10[i-1] > 0)
		       ratio1 = vHistAv10[i]/vHistAv10[i-1];
		   else
			   ratio1 = vHistAv10[i];
		   if(vHistAv10[i-2] > 0)
		       ratio2 = vHistAv10[i]/vHistAv10[i-2];
		   else
			   ratio2 = vHistAv10[i];
		   if(ratio1<ratio2) ratio1 = ratio2;

 //printf("   i= %d, ratio1= %d\n", i, ratio1);

		   if(vHistAv10[i] > th1) cc1++;
		   if(vHistAv10[i] > th2 && vHistAv10[i] < th1) cc2++;
		   if(vHistAv10[i] < th2) cc2++;
		   if(ratio1 > ratioTh) {k = i;break;}
	   }
//   printf("   th1= %d, th2= %d\n", th1, th2);
//       printf("   k= %d, ratio1= %d, cc1= %d, cc2= %d. cc3= %d, cc1/3+1= %d\n", k, ratio1, cc1, cc2, cc3, cc1/3+1);

       if(cc1 > 9 && cc1 < 30 && cc3 == 0 && cc2 < cc1/3+1 && k > 30){
    	  k -= 2;
    	  cc1 = 0;cc2 = 0;
   	      for(i=k;i>k-10;i--){
   	    	if(vHistAv10[i] > th1) cc1++;
   		     if(vHistAv10[i] > th2) cc2++;
   	      }
//   printf("   th1= %d, th2= %d\n", th1, th2);
//  printf("   cc1= %dcc2= %d\n", cc1, cc2);
   	      if(cc1>0 || cc2>1) return;
   	      i1 = x1+k*10; i2 = x2;
//  printf("   cleanMargin, i1= %d, i2= %d\n", i1, i2);
   	      cleanMargin(pTBuffer, byteWidthHist, height, i1, i2);
       }

/*
	   for(i=1;i<vHistAv10[0];i++){
//			  		    printf("   i= %d, vHist= %d \n", i, vHist[i]);
		     sprintf(bufLog,"   i= %d, vHistAv10= %d, , y2-y1=% d \n", i, vHistAv10[i],  y2-y1);
		     saveLog(bufLog, "");

	  	  }


			   for(i=0;i<vHist[0];i++){
//			  		    printf("   i= %d, vHist= %d \n", i, vHist[i]);
				     sprintf(bufLog,"   i= %d, vHist= %d, , y2-y1=% d \n", i, vHist[i],  y2-y1);
				     saveLog(bufLog, "");

			  	  }
*/
	}

}

void setFieldImageInfo(LPTICKETRESULTS ticketResults, int idxResults, LPTICKETDATA ticketDataPtr){
	LPBITMAPFILEHEADER bf = (LPBITMAPFILEHEADER)(ticketDataPtr->fieldBMPPtr);
	LPBITMAPINFOHEADER bi = (LPBITMAPINFOHEADER)(ticketDataPtr->fieldBMPPtr + sizeof (BITMAPFILEHEADER));

	LPOUTIMGS lpImgs = ticketResults->fieldsResults[idxResults].ptrOutImages;
	lpImgs->width = getLeftBytes((int*)bi->biWidth, 4);
	lpImgs->height = getLeftBytes((int*)bi->biHeight, 4);
	lpImgs->lpImgBuffer = ticketDataPtr->fieldBMPPtr + getLeftBytes((int*)bf->bfOffBits, 4);
	lpImgs->ptrNext = NULL;

}

void initLineQuads(LPTICKETDATA ticketDataPtr, LPFIELDDATA field, double angleOut){
	int i;
	ticketDataPtr->angleOut = angleOut;
	for(i=0;i<NUM_LINESTATUS;i++){
		ticketDataPtr->lineQuadStatus[i] = 0;
		ticketDataPtr->lineQuad[i].p1X = field->quad.p1X;
		ticketDataPtr->lineQuad[i].p2X = field->quad.p2X;
		ticketDataPtr->lineQuad[i].p3X = field->quad.p3X;
		ticketDataPtr->lineQuad[i].p4X = field->quad.p4X;
		ticketDataPtr->lineQuad[i].p1Y = field->quad.p1Y;
		ticketDataPtr->lineQuad[i].p2Y = field->quad.p2Y;
		ticketDataPtr->lineQuad[i].p3Y = field->quad.p3Y;
		ticketDataPtr->lineQuad[i].p4Y = field->quad.p4Y;

	}
}
void copyLineQuads(LPFIELDSRESULTS lpFresults, LPTICKETDATA ticketDataPtr){
	int i;
	lpFresults->angle = ticketDataPtr->angleOut;
	for(i=0;i<NUM_LINESTATUS;i++){
		lpFresults->lineQuadStatus[i] = ticketDataPtr->lineQuadStatus[i];
		lpFresults->lineQuad[i].p1X = ticketDataPtr->lineQuad[i].p1X;
		lpFresults->lineQuad[i].p2X = ticketDataPtr->lineQuad[i].p2X;
		lpFresults->lineQuad[i].p3X = ticketDataPtr->lineQuad[i].p3X;
		lpFresults->lineQuad[i].p4X = ticketDataPtr->lineQuad[i].p4X;
		lpFresults->lineQuad[i].p1Y = ticketDataPtr->lineQuad[i].p1Y;
		lpFresults->lineQuad[i].p2Y = ticketDataPtr->lineQuad[i].p2Y;
		lpFresults->lineQuad[i].p3Y = ticketDataPtr->lineQuad[i].p3Y;
		lpFresults->lineQuad[i].p4Y = ticketDataPtr->lineQuad[i].p4Y;
	}
}

int getLeftSideDateTop(int *vHist){
	int shift = 0, i;
	int iSp, ccSp;
	int sPTab[10], idxSp = 0;
	iSp = 0; ccSp = 0;
	for(i=1;i<vHist[0];i++){
		if(vHist[i] < 3) {
			if(ccSp == 0) iSp = i;
			ccSp++;
		} else{
			if(ccSp > 100 && idxSp < 10) {
				sPTab[idxSp] = iSp+ccSp/2;
				idxSp++;
			}
			ccSp = 0;
		}
	}
	if(idxSp > 0)
		shift = sPTab[idxSp-1];

	return shift;
}
int calculateLines(int *hHist){
	int i, len = hHist[0];
	int cc0, cc1, flg = 0;
	int cc1Prev, max, maxPrev;
//	int tabLen = 100;
//    int tabLines[tabLen];
    int numLines = 0;

//	for(i=0;i<tabLen;i++)
//		tabLines[i] = 0;
	cc0 = 0; cc1 = 0;max = 0;
	for(i=10;i<len-10;i++){
		if(hHist[i] == 0) {
			if(cc0 == 0) {cc1Prev = cc1; maxPrev = max;}
			cc0++;
			if(cc0 > 3 && cc1Prev > 5 && maxPrev >10 && flg == 0) {numLines++;flg = 1;}
//			if(cc0 > 3 && flg == 0) {
//				printf("cc0= %d, cc1Prev= %d, maxPrev= %d\n",cc0, cc1Prev, maxPrev);
//				numLines++;flg = 1;
//			}
			cc1 = 0;
			max = 0;
		}else{
			if(max < hHist[i]) max = hHist[i];
			flg = 0;
			cc1++;
			cc0 = 0;
		}
	}
	return numLines;
}

int testFoRET_NUM_recognition(LPFIELDDATA field){
   int ret = 0;
	switch(US_State){
	case 0:
	case 1:
//printf("-----------p1Y= %d\n", field->quad.p1Y);
		if(field->quad.p1Y > 200){
//			field->quad.p1Y = field->quad.p1Y/2;
//			field->quad.p2Y = field->quad.p1Y;
			ret = 1;
		}
		break;
	default:
		break;
	}
	return ret;
}
