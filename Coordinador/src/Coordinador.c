
#include <stdio.h>
#include <stdlib.h>

#include "Funciones_coordinador.h"


int main(){

	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	log_info(logger,"Logger creado");

	leer_archivo_configuracion();

	log_info(logger,"Se paso a archivo");
	inicializar_coordinador();
	conectar_planificador();

	pthread_t hilo_escucha;
	pthread_t hilo_planificador;

	pthread_create(&hilo_escucha, 0, procesar_conexion, NULL);
	pthread_create(&hilo_planificador, 0, atender_planificador, NULL);

	log_info(logger, "Hilos ya creados");
	sleep(60);
	log_info(logger, "Me desperte");
	pthread_join(hilo_escucha, NULL);
	pthread_join(hilo_planificador, NULL);
	close(socket_escucha);

	return 0;
}
