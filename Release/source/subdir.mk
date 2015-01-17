################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/main.cpp \
../source/support__bioinformatics.cpp \
../source/support__file_io.cpp \
../source/support__general.cpp \
../source/uORF__compile.cpp \
../source/uORF__process.cpp 

OBJS += \
./source/main.o \
./source/support__bioinformatics.o \
./source/support__file_io.o \
./source/support__general.o \
./source/uORF__compile.o \
./source/uORF__process.o 

CPP_DEPS += \
./source/main.d \
./source/support__bioinformatics.d \
./source/support__file_io.d \
./source/support__general.d \
./source/uORF__compile.d \
./source/uORF__process.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"C:\Users\Garin\College\Research_Sayood\microORF_project\include" -I"C:\Users\Garin\College\Research_Sayood\microORF_project\externals" -include"C:\Users\Garin\College\Research_Sayood\microORF_project\externals\lbg_clustering.h" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


