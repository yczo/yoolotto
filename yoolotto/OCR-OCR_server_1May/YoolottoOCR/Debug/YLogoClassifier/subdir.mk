################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../YLogoClassifier/BinStorage.cpp \
../YLogoClassifier/FeaturesCompression.cpp \
../YLogoClassifier/GraphicsGems.cpp \
../YLogoClassifier/IExtractedFeaturesSet.cpp \
../YLogoClassifier/IFeatureExtraction.cpp \
../YLogoClassifier/ILFeatureExtraction.cpp \
../YLogoClassifier/IOptimizedFunction.cpp \
../YLogoClassifier/IVectorSpaceClassifier.cpp \
../YLogoClassifier/IYImageClassifier.cpp \
../YLogoClassifier/ImagePreprocessForClassify.cpp \
../YLogoClassifier/LMSpeedup.cpp \
../YLogoClassifier/LogoFeatureExtraction.cpp \
../YLogoClassifier/PLLargeMarginClassifier.cpp \
../YLogoClassifier/PPFileTypes.cpp \
../YLogoClassifier/PixelGrabber.cpp \
../YLogoClassifier/PixelUtils.cpp \
../YLogoClassifier/RelevantFeaturesSelector.cpp \
../YLogoClassifier/Simplex.cpp \
../YLogoClassifier/YLLogoClassifierEvaluation.cpp \
../YLogoClassifier/YLogoClasses.cpp \
../YLogoClassifier/YLogoClassifier.cpp \
../YLogoClassifier/YLogoClassifyEngine.cpp \
../YLogoClassifier/ylogo_classifier.cpp 

OBJS += \
./YLogoClassifier/BinStorage.o \
./YLogoClassifier/FeaturesCompression.o \
./YLogoClassifier/GraphicsGems.o \
./YLogoClassifier/IExtractedFeaturesSet.o \
./YLogoClassifier/IFeatureExtraction.o \
./YLogoClassifier/ILFeatureExtraction.o \
./YLogoClassifier/IOptimizedFunction.o \
./YLogoClassifier/IVectorSpaceClassifier.o \
./YLogoClassifier/IYImageClassifier.o \
./YLogoClassifier/ImagePreprocessForClassify.o \
./YLogoClassifier/LMSpeedup.o \
./YLogoClassifier/LogoFeatureExtraction.o \
./YLogoClassifier/PLLargeMarginClassifier.o \
./YLogoClassifier/PPFileTypes.o \
./YLogoClassifier/PixelGrabber.o \
./YLogoClassifier/PixelUtils.o \
./YLogoClassifier/RelevantFeaturesSelector.o \
./YLogoClassifier/Simplex.o \
./YLogoClassifier/YLLogoClassifierEvaluation.o \
./YLogoClassifier/YLogoClasses.o \
./YLogoClassifier/YLogoClassifier.o \
./YLogoClassifier/YLogoClassifyEngine.o \
./YLogoClassifier/ylogo_classifier.o 

CPP_DEPS += \
./YLogoClassifier/BinStorage.d \
./YLogoClassifier/FeaturesCompression.d \
./YLogoClassifier/GraphicsGems.d \
./YLogoClassifier/IExtractedFeaturesSet.d \
./YLogoClassifier/IFeatureExtraction.d \
./YLogoClassifier/ILFeatureExtraction.d \
./YLogoClassifier/IOptimizedFunction.d \
./YLogoClassifier/IVectorSpaceClassifier.d \
./YLogoClassifier/IYImageClassifier.d \
./YLogoClassifier/ImagePreprocessForClassify.d \
./YLogoClassifier/LMSpeedup.d \
./YLogoClassifier/LogoFeatureExtraction.d \
./YLogoClassifier/PLLargeMarginClassifier.d \
./YLogoClassifier/PPFileTypes.d \
./YLogoClassifier/PixelGrabber.d \
./YLogoClassifier/PixelUtils.d \
./YLogoClassifier/RelevantFeaturesSelector.d \
./YLogoClassifier/Simplex.d \
./YLogoClassifier/YLLogoClassifierEvaluation.d \
./YLogoClassifier/YLogoClasses.d \
./YLogoClassifier/YLogoClassifier.d \
./YLogoClassifier/YLogoClassifyEngine.d \
./YLogoClassifier/ylogo_classifier.d 


# Each subdirectory must supply rules for building sources it contributes
YLogoClassifier/%.o: ../YLogoClassifier/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -std=c++0x -fpermissive -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


