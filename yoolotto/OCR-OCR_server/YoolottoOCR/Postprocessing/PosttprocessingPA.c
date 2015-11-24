/*
 * PosttprocessingPA.c
 *
 *  Created on: Aug 30, 2013
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
#include "../headers/BarCode.h"


void ticketPostProcessing_PA(LPTICKETDATA ticketDataPtr, LPTICKETRESULTS ticketResults){
	int i;
//	  printf ("Results before post-processing----------------------------------------\n");
//	  saveLog("Results before post-processing----------------------------------------", "\n");
//		printResults_1(ticketResults);
//	 printf ("-----------------------------------------------------------------------\n");
//	 saveLog("-----------------------------------------------------------------------", "\n");


    ticketResults->type = ticketTypeInt;
    for(i=0;i<NUMFIELDS_RESULTS;i++){
    	if(ticketResults->fieldsResults[i].name[0] != 0){
//    		if(strcmp(ticketResults->fieldsResults[i].name, "LOGO3") == 0) {postProcessingLogo_TX(ticketResults);setLogoSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "NUMBERS") == 0) ticketResults->status = postProcessingNumbers_TX(ticketResults, i);
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEA") == 0) {postProcessingDate_TX(ticketResults);setDateSlices(ticketResults);}
//    		if(strcmp(ticketResults->fieldsResults[i].name, "DATEN1") == 0) postProcessingDate1_TX(ticketResults);
    	}
    }

}
