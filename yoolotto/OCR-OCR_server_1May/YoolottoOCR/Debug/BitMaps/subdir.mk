################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BitMaps/BMP.c \
../BitMaps/PreProcessing.c \
../BitMaps/RotateBMP.c \
../BitMaps/scratchOffCleaning.c 

OBJS += \
./BitMaps/BMP.o \
./BitMaps/PreProcessing.o \
./BitMaps/RotateBMP.o \
./BitMaps/scratchOffCleaning.o 

C_DEPS += \
./BitMaps/BMP.d \
./BitMaps/PreProcessing.d \
./BitMaps/RotateBMP.d \
./BitMaps/scratchOffCleaning.d 


# Each subdirectory must supply rules for building sources it contributes
BitMaps/%.o: ../BitMaps/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


