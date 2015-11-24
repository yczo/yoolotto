################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Statistics/CovarianceMatrix.cpp \
../Statistics/Data.cpp \
../Statistics/Determinant.cpp \
../Statistics/Eigen.cpp \
../Statistics/IProjection.cpp \
../Statistics/Lista.cpp \
../Statistics/Matrix.cpp \
../Statistics/PPProjection.cpp \
../Statistics/Statistics.cpp \
../Statistics/Variance.cpp \
../Statistics/Vector.cpp \
../Statistics/VectorStorage.cpp \
../Statistics/Whitening.cpp \
../Statistics/dcdflib.cpp \
../Statistics/elem.cpp \
../Statistics/ipmpar.cpp 

OBJS += \
./Statistics/CovarianceMatrix.o \
./Statistics/Data.o \
./Statistics/Determinant.o \
./Statistics/Eigen.o \
./Statistics/IProjection.o \
./Statistics/Lista.o \
./Statistics/Matrix.o \
./Statistics/PPProjection.o \
./Statistics/Statistics.o \
./Statistics/Variance.o \
./Statistics/Vector.o \
./Statistics/VectorStorage.o \
./Statistics/Whitening.o \
./Statistics/dcdflib.o \
./Statistics/elem.o \
./Statistics/ipmpar.o 

CPP_DEPS += \
./Statistics/CovarianceMatrix.d \
./Statistics/Data.d \
./Statistics/Determinant.d \
./Statistics/Eigen.d \
./Statistics/IProjection.d \
./Statistics/Lista.d \
./Statistics/Matrix.d \
./Statistics/PPProjection.d \
./Statistics/Statistics.d \
./Statistics/Variance.d \
./Statistics/Vector.d \
./Statistics/VectorStorage.d \
./Statistics/Whitening.d \
./Statistics/dcdflib.d \
./Statistics/elem.d \
./Statistics/ipmpar.d 


# Each subdirectory must supply rules for building sources it contributes
Statistics/%.o: ../Statistics/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -std=c++0x -fpermissive -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


