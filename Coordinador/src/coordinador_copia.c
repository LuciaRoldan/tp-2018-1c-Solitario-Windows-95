
#include <stdio.h>
#include <stdlib.h>
#include "funciones_coordi.h"

t_log * logger;
t_conexion conexionInstancia;
int socket_planificador;

int main(){
	puts("Arrancamos...");
	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	puts("Logger creado");

	info_archivo_config configuracion;
	leer_archivo_configuracion(&configuracion, logger);

	puts("Se paso a archivo");
	int socket_escucha = inicializar_coordinador(configuracion, logger);
	puts("Creo socket");
	conectar_planificador(&socket_escucha, &socket_planificador, logger);
	puts("Me conecte");

	// pthread_t hilo_escucha;
	// pthread_t hilo_planificador;

	//pthread_create(&hilo_escucha, 0, procesar_conexion, (int*)&socket_escucha, (t_log*) logger);
	//pthread_create(&hilo_planificador, 0, atender_planificador, (&socket_planificador, logger));


	return 0;
}
