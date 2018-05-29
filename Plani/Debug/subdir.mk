################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../planificador.c \
../planificador_funciones.c 

OBJS += \
./planificador.o \
./planificador_funciones.o 

C_DEPS += \
./planificador.d \
./planificador_funciones.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias" -I"/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


