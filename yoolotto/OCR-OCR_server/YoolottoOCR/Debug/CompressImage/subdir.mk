################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CompressImage/huffman.c \
../CompressImage/jpgDecoder.c \
../CompressImage/jpgEncoder.c \
../CompressImage/jpgEncoder_old.c 

OBJS += \
./CompressImage/huffman.o \
./CompressImage/jpgDecoder.o \
./CompressImage/jpgEncoder.o \
./CompressImage/jpgEncoder_old.o 

C_DEPS += \
./CompressImage/huffman.d \
./CompressImage/jpgDecoder.d \
./CompressImage/jpgEncoder.d \
./CompressImage/jpgEncoder_old.d 


# Each subdirectory must supply rules for building sources it contributes
CompressImage/%.o: ../CompressImage/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


