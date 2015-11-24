/*
 * Library.c
 *
 *  Created on: Aug 31, 2012
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



 void initLib (LPTICKETDATA ticketDataPtr){
	  int i;
	for(i=0;i<MAXNO_LIB;i++)
		ticketDataPtr->LIB[i] = &Lib_Table[i];
//printf("  addFilesToTheLib \n");
	 addFilesToTheLib(ticketDataPtr->libraryPath);
  }

  int readHeader (unsigned char * pDataBuffer,  unsigned char * pHeader){
	  int i,offset;
	  offset = pDataBuffer[10];
	  for (i=0;i<offset;i++)
	 	pHeader[i] = pDataBuffer[i];

	  return (0);
	 }
	  unsigned char * getFromTable(LPTICKETDATA ticketDataPtr, int index, int *lpLibSize)
	  {
	  int i;
	     for(i=0;i<MAXNO_LIB;i++)  {
//	    	 if((ticketDataPtr->LIB[i])){
//	    		 printf("  ticketDataPtr->LIB[i]->index= %d \n", ticketDataPtr->LIB[i]->index);
//	    	 }

	  	   if((ticketDataPtr->LIB[i]) && (ticketDataPtr->LIB[i]->index == index)) { /* library already loaded */
	  		   *lpLibSize = ticketDataPtr->LIB[i]->size;
	  		   return(ticketDataPtr->LIB[i]->lpLib);
	  	   }
	     }
	   return NULL;
	  }

 unsigned char *getLibName(LPTICKETDATA ticketDataPtr, int index, char *libName, int *lpLibSize)
	  {
	  unsigned char *lpLib;
	  /* first test table of libraries */
	    lpLib = getFromTable(ticketDataPtr, index, lpLibSize);
	    if(lpLib)
	  	  return lpLib;
	    return NULL;
	  }

  unsigned char *loadLibrary_G(LPTICKETDATA ticketDataPtr, int index, int *lpLibSize)
	  {
	  char    libName[256];
	  unsigned char *lpLib;

//	  printf("  index= %d \n", index);
//	  printf("  libName= %s \n", libName);

	    lpLib = getLibName(ticketDataPtr, index, libName, lpLibSize);
	    if(lpLib && (*lpLibSize) > 0)
	  	  return lpLib;
	    return NULL;

	  /*
	     hFile = _open (libName, _O_RDONLY | _O_BINARY);
	      if (hFile == HFILE_ERROR) {
	          return NULL;
	      }

	     *lpLibSize = _lseek (hFile, 0, SEEK_END);
	     _lseek (hFile, 0, SEEK_SET);
	     if (!(*lpLibSize)) {
	         _close (hFile);
	         return NULL;
	     }
	     lpLib = (unsigned char*) malloc (*lpLibSize);
	     if (!lpLib) {
	        _close (hFile);
	        return NULL;
	      }
	     ret = _read (hFile, lpLib, (unsigned int)(*lpLibSize));
	     if (ret != (*lpLibSize)) {
	          _close (hFile);
	  		yl_free(lpLib);
	  //        GlobalUnlock (hLib);
	  //        GlobalFree   (hLib);
	          return NULL;
	      }
	     _close (hFile);
	    libName[0] = 0;
	    strcatNTWName(libName, index);
	    addToTable(index, lpLib, (*lpLibSize), libName);
	    return lpLib;
	  */
	  }

  void addLibToTable(LPHDR lpHdr, unsigned char *lpLib, int LibSize, LPSTR name)
  {
  int i;
     for(i=0;i<MAXNO_LIB;i++)  {
  	   if(Lib_Table[i].size == 0) { /* first empty space */
  		 Lib_Table[i].index = lpHdr->index;
  		 Lib_Table[i].lpLib = lpLib;
  		 Lib_Table[i].size  = LibSize;
	     if(Lib_Table[i].name) {
  			   yl_free(Lib_Table[i].name);
  		   }
	       Lib_Table[i].name  = strdup(name); // release this memory later
             if(lpHdr->number < 128) {
  		        strncpy(Lib_Table[i].chars, lpHdr->chars, lpHdr->number);
  		        Lib_Table[i].chars[lpHdr->number] = 0;
  		   }
             Lib_Table[i].flag = 0;
  		   return;
  	   }
     }

  }

  int addNewLib(LPSTR libFullName, LPSTR libName, char * error)
  {
  FILE *f;
  unsigned char *lpLib;
//  int     lpLibSize;
  int     len;
  size_t fSize = 0;

//  char buf[2048];

//  sprintf(buf, "addNewLib( %s, %s)", libFullName, libName);
//  saveLog( "", buf);

  f= fopen(libFullName, "r");
  if(f == NULL) {
  	sprintf(error,"addNewLib (\"%s\") open failed: %s\n",libFullName, strerror(errno));
//printf("addNewLib (\"%s\") open failed: %s\n",libFullName, strerror(errno));
  	return -1;
  }
// printf("library name: %s\n\n",libFullName);

  fseek(f, 0L,SEEK_END);

  if(ferror(f)){
	  sprintf(error,"addNewLib (\"%s\") fseek failed: %s\n",libFullName, strerror(errno));
  	return -2;
  }

  len = ftell(f);
// printf("fseek %s, len = %d\n",libFullName, len);
  fseek(f, 0L,SEEK_SET);
  lpLib = yl_calloc("LIB1", len, sizeof(BYTE));
  if(lpLib == NULL) {
	  sprintf(error,"addNewLib (\"%s\") yl_calloc failed: %s\n",libFullName, strerror(errno));
  	return -3;
  }
  fSize = fread((char *)lpLib, len, 1, f);
  if(ferror(f) || fSize*len != len){
      if(ferror(f))
    	  sprintf(error,"addNewLib (\"%s\") read failed: %s\n",libFullName, strerror(errno));
      else
          sprintf(error,"addNewLib (\"%s\") read failed: (size= %d) != (len= %d\n)",libFullName, fSize*len, len);
	  yl_free(lpLib);
	  lpLib = NULL;
  	  return -4;
  }
  close_file(f);
     {
     LPHDR lpHdr;
       lpHdr = (LPHDR)lpLib;
       addLibToTable(lpHdr, lpLib, len, libName);
     }
     return 1;
  }
/*
  void saveLog_1(char *str1, char *str2, char *fName){
     FILE *f;
  //   int  len;
     char fileName[500];
     char buff[2000];
     strcpy(fileName, "c:\\Yoolotto\\zzz_project\\log\\");
     strcat(fileName, fName);
     f = fopen(fileName, "a");
      if(f == NULL) {
      	fprintf(stderr,"savelog (\"%s\") failed: %s\n",fileName, strerror(errno));
      	return;
      }
  //    fseek(f, 0L,SEEK_END);
  //    len = ftell(f);

     buff[0] = 0;
     if(str1) {
       strcat(buff, str1);
       strcat(buff, " ");
     }
     if(str2)
       strcat(buff, str2);

  //   if(buff[0]) fprintf (f, "%s\n", buff);
     if(buff[0]) fprintf (f, "%s", buff);
     fclose (f);
  }
*/
  int addFilesToTheLib(char *path)
  {
  char searchName[500];
  char fileName[500];
  char lottoName[500];
  char error[500];
  char num[4];
  int  ret;
  int i, count;

//  char buffLog[2000];

     count = 0;
     strcpy(lottoName, "LOTTO_");
     for(i=1;i<=MAXNO_LIB;i++){
         strcpy(fileName, lottoName);
//         num[0] = '0'+i;
//         num[1] = 0;
         sprintf(num,"%d",i);
         strcat(fileName, num);
         strcat(fileName, ".LIB");
         strcpy(searchName, path);
         strcat(searchName, fileName);
//    printf("---- adding files to the library; fileName= %s, searchName= %s\n", fileName, searchName);

//         sprintf(buffLog,"---- adding files to the library; fileName= %s, searchName= %s\n", fileName, searchName);
//         saveLog_1(buffLog, "\n", "log.txt");
   		 ret = addNewLib(searchName, fileName, error);

   		 if( ret < 0 ){
//   			 saveLog( "addNewLib error:", error);
   			count++;
   			if(count > 1)
   			 return ret;
   		 }
   		 else
   			count = 0;

     }
     return 0;
  }

  void releaseLibMemory(void)
  {
  int i;
     for(i=0;i<MAXNO_LIB;i++)  {
  	   if(Lib_Table[i].size != 0) {
	     if(Lib_Table[i].name) {
  			   yl_free(Lib_Table[i].name);
  			 Lib_Table[i].name = NULL;
  		   }
	     if(Lib_Table[i].lpLib) {
  			   yl_free(Lib_Table[i].lpLib);
  			   Lib_Table[i].lpLib = NULL;
  		   }
	     Lib_Table[i].size = 0;
       }
  	   else
  		   break;
     }
  }


