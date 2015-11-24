################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Postprocessing/Postprocessing.c \
../Postprocessing/PostprocessingCA.c \
../Postprocessing/PostprocessingNY.c \
../Postprocessing/PostprocessingOtherSt.c \
../Postprocessing/PostprocessingTX.c \
../Postprocessing/PostprocessingTX2.c \
../Postprocessing/PosttprocessingPA.c 

OBJS += \
./Postprocessing/Postprocessing.o \
./Postprocessing/PostprocessingCA.o \
./Postprocessing/PostprocessingNY.o \
./Postprocessing/PostprocessingOtherSt.o \
./Postprocessing/PostprocessingTX.o \
./Postprocessing/PostprocessingTX2.o \
./Postprocessing/PosttprocessingPA.o 

C_DEPS += \
./Postprocessing/Postprocessing.d \
./Postprocessing/PostprocessingCA.d \
./Postprocessing/PostprocessingNY.d \
./Postprocessing/PostprocessingOtherSt.d \
./Postprocessing/PostprocessingTX.d \
./Postprocessing/PostprocessingTX2.d \
./Postprocessing/PosttprocessingPA.d 


# Each subdirectory must supply rules for building sources it contributes
Postprocessing/%.o: ../Postprocessing/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


