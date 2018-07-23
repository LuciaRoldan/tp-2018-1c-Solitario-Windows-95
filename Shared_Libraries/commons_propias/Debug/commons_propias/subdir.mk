################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commons_propias/commons_propias.c 

OBJS += \
./commons_propias/commons_propias.o 

C_DEPS += \
./commons_propias/commons_propias.d 


# Each subdirectory must supply rules for building sources it contributes
commons_propias/%.o: ../commons_propias/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


