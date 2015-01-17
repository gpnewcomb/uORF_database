################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../externals/lbg_clustering.cpp 

OBJS += \
./externals/lbg_clustering.o 

CPP_DEPS += \
./externals/lbg_clustering.d 


# Each subdirectory must supply rules for building sources it contributes
externals/%.o: ../externals/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"C:\Users\Garin\College\Research_Sayood\microORF_project\include" -I"C:\Users\Garin\College\Research_Sayood\microORF_project\externals" -include"C:\Users\Garin\College\Research_Sayood\microORF_project\externals\lbg_clustering.h" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


