
#include <stdio.h>
#include <string.h>

int waitForInput() {
  char input1[100];
  char input2[100];
  char input3[100];
  char input4[100];
  char input5[100];
  char input6[100];
  char output1[100];
  int validchoice = 0;
  int iCh = 0;
  
  strcpy(input1, "1. temporary not supported \n");
  strcpy(input2, "2. OCR for BMP format\n");
  strcpy(input3, "3. OCR for RGPA format\n");
  strcpy(input4, "4. OCR for RGP565 format\n");
  strcpy(input5, "5. converters\n");
  strcpy(input6, "6. exit\n");

  
  do {
   printf(input1);
   printf(input2);
   printf(input3);
   printf(input4);
   printf(input5);
   printf(input6);
  
   fflush(stdout);
   
//   printf("\n");
//   printf("\n");   
   
     fgets(output1, 50, stdin);
  
//   if(strlen(output1) == 1){
      iCh = output1[0] -'0';
      if(iCh > 0 && iCh< 7){
          validchoice = 1;
      }
//      printf("output1[0]= %s; iCh= %d\n", output1,iCh);
//   }
 } while (validchoice == 0);

 return iCh;
 
 }
