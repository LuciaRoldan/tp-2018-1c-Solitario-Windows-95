################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funciones_algoritmos.c \
../src/funciones_aux.c \
../src/funciones_aux_listas.c \
../src/funciones_consola.c \
../src/funciones_envio_mensajes.c \
../src/funciones_esis.c \
../src/funciones_inicializar_y_cerrar.c \
../src/funciones_principales.c \
../src/planificador.c \
../src/planificador_consola.c 

OBJS += \
./src/funciones_algoritmos.o \
./src/funciones_aux.o \
./src/funciones_aux_listas.o \
./src/funciones_consola.o \
./src/funciones_envio_mensajes.o \
./src/funciones_esis.o \
./src/funciones_inicializar_y_cerrar.o \
./src/funciones_principales.o \
./src/planificador.o \
./src/planificador_consola.o 

C_DEPS += \
./src/funciones_algoritmos.d \
./src/funciones_aux.d \
./src/funciones_aux_listas.d \
./src/funciones_consola.d \
./src/funciones_envio_mensajes.d \
./src/funciones_esis.d \
./src/funciones_inicializar_y_cerrar.d \
./src/funciones_principales.d \
./src/planificador.d \
./src/planificador_consola.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias" -I/home/utnso/workspace/parsi -I"/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


