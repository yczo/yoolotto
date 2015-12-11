# Makefile template for shared library

CC = gcc # C compiler
CFLAGS = -fPIC -Wall -Wextra -O3 -g # C flags
LDFLAGS = -shared  # linking flags
RM = rm -f  # rm command
TARGET_LIB = $(SRC)/libOCR.so # target lib

SRC = /home/kanika/yoolotto_multi/yoolotto/OCR-OCR_server/YoolottoOCR

ODIR = obj
CC=gcc
SRCS =	$(SRC)/Main/ocr.c $(SRC)/Main/util.c $(SRC)/Main/memmgr.c $(SRC)/Main/LotteryTypes.c $(SRC)/Main/json.c\
	$(SRC)/BitMaps/BMP.c $(SRC)/BitMaps/PreProcessing.c $(SRC)/BitMaps/RotateBMP.c $(SRC)/BitMaps/scratchOffCleaning.c\
	$(SRC)/Img/Field.c $(SRC)/Img/Library.c $(SRC)/Img/LottoPrototype.c $(SRC)/Img/Recognition.c $(SRC)/Img/RemoveBackground.c $(SRC)/Img/UtilPrototypes.c\
        $(SRC)/Img/UtilRecognition.c $(SRC)/Img/UtilRecognition2.c $(SRC)/Img/UtilRecognition3.c $(SRC)/Img/UtilRecognition4.c $(SRC)/Img/UtilRecognition5.c\
	$(SRC)/Postprocessing/Postprocessing.c	$(SRC)/Postprocessing/PostprocessingTX.c $(SRC)/Postprocessing/PostprocessingCA.c $(SRC)/Postprocessing/PosttprocessingPA.c $(SRC)/Postprocessing/PostprocessingNY.c $(SRC)/Postprocessing/PostprocessingOtherSt.c $(SRC)/Postprocessing/PostprocessingTX2.c\
	$(SRC)/lottoType/BarcodeI25.c $(SRC)/lottoType/canny.c $(SRC)/lottoType/HashTable_Lib.c $(SRC)/lottoType/imageQualitySWT.c $(SRC)/lottoType/imgQuality.c\
        $(SRC)/lottoType/searchForLogoCA.c $(SRC)/lottoType/searchForLogoNY.c $(SRC)/lottoType/searchForLogo.c\
        $(SRC)/CompressImage/huffman.c $(SRC)/CompressImage/jpgDecoder.c $(SRC)/CompressImage/jpgEncoder.c $(SRC)/CompressImage/jpgEncoder_old.c\

#SRCS = $(SRC)/$(_SRCS) # source files
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^ -lm
 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm
 
 
clean:
	-${RM} ${OBJS} 
 


	

