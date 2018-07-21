################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Coordinador.c \
../src/Funciones_coordinador_comunicaciones.c \
../src/Funciones_coordinador_conexiones.c \
../src/Funciones_coordinador_hilos.c \
../src/Funciones_coordinador_inicializacion.c \
../src/Funciones_coordinador_listas.c \
../src/Funciones_coordinador_procesamiento.c 

OBJS += \
./src/Coordinador.o \
./src/Funciones_coordinador_comunicaciones.o \
./src/Funciones_coordinador_conexiones.o \
./src/Funciones_coordinador_hilos.o \
./src/Funciones_coordinador_inicializacion.o \
./src/Funciones_coordinador_listas.o \
./src/Funciones_coordinador_procesamiento.o 

C_DEPS += \
./src/Coordinador.d \
./src/Funciones_coordinador_comunicaciones.d \
./src/Funciones_coordinador_conexiones.d \
./src/Funciones_coordinador_hilos.d \
./src/Funciones_coordinador_inicializacion.d \
./src/Funciones_coordinador_listas.d \
./src/Funciones_coordinador_procesamiento.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias" -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


