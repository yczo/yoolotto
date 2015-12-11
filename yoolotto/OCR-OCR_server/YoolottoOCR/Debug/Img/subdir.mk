################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Img/Field.c \
../Img/Library.c \
../Img/LottoPrototype.c \
../Img/Recognition.c \
../Img/RemoveBackground.c \
../Img/UtilPrototypes.c \
../Img/UtilRecognition.c \
../Img/UtilRecognition2.c \
../Img/UtilRecognition3.c \
../Img/UtilRecognition4.c \
../Img/UtilRecognition5.c 

OBJS += \
./Img/Field.o \
./Img/Library.o \
./Img/LottoPrototype.o \
./Img/Recognition.o \
./Img/RemoveBackground.o \
./Img/UtilPrototypes.o \
./Img/UtilRecognition.o \
./Img/UtilRecognition2.o \
./Img/UtilRecognition3.o \
./Img/UtilRecognition4.o \
./Img/UtilRecognition5.o 

C_DEPS += \
./Img/Field.d \
./Img/Library.d \
./Img/LottoPrototype.d \
./Img/Recognition.d \
./Img/RemoveBackground.d \
./Img/UtilPrototypes.d \
./Img/UtilRecognition.d \
./Img/UtilRecognition2.d \
./Img/UtilRecognition3.d \
./Img/UtilRecognition4.d \
./Img/UtilRecognition5.d 


# Each subdirectory must supply rules for building sources it contributes
Img/%.o: ../Img/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


