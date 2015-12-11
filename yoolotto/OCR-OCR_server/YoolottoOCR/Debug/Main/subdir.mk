################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Main/LotteryTypes.c \
../Main/json.c \
../Main/main.c \
../Main/memmgr.c \
../Main/ocr.c \
../Main/util.c 

OBJS += \
./Main/LotteryTypes.o \
./Main/json.o \
./Main/main.o \
./Main/memmgr.o \
./Main/ocr.o \
./Main/util.o 

C_DEPS += \
./Main/LotteryTypes.d \
./Main/json.d \
./Main/main.d \
./Main/memmgr.d \
./Main/ocr.d \
./Main/util.d 


# Each subdirectory must supply rules for building sources it contributes
Main/%.o: ../Main/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


