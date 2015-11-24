################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lottoType/BarcodeI25.c \
../lottoType/HashTable_Lib.c \
../lottoType/canny.c \
../lottoType/imageQualitySWT.c \
../lottoType/imgQuality.c \
../lottoType/searchForLogo.c \
../lottoType/searchForLogoCA.c \
../lottoType/searchForLogoNY.c 

OBJS += \
./lottoType/BarcodeI25.o \
./lottoType/HashTable_Lib.o \
./lottoType/canny.o \
./lottoType/imageQualitySWT.o \
./lottoType/imgQuality.o \
./lottoType/searchForLogo.o \
./lottoType/searchForLogoCA.o \
./lottoType/searchForLogoNY.o 

C_DEPS += \
./lottoType/BarcodeI25.d \
./lottoType/HashTable_Lib.d \
./lottoType/canny.d \
./lottoType/imageQualitySWT.d \
./lottoType/imgQuality.d \
./lottoType/searchForLogo.d \
./lottoType/searchForLogoCA.d \
./lottoType/searchForLogoNY.d 


# Each subdirectory must supply rules for building sources it contributes
lottoType/%.o: ../lottoType/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


