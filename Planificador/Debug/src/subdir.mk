################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funciones_consola.c \
../src/funciones_plani.c \
../src/planificador.c 

OBJS += \
./src/funciones_consola.o \
./src/funciones_plani.o \
./src/planificador.o 

C_DEPS += \
./src/funciones_consola.d \
./src/funciones_plani.d \
./src/planificador.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias" -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


