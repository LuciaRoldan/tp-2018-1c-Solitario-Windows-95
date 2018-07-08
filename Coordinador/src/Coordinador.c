
#include <stdio.h>
#include <stdlib.h>

#include "Funciones_coordinador.h"


int main(){

	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	log_info(logger,"Logger creado");

	info_archivo_config configuracion;
	leer_archivo_configuracion(&configuracion);

	log_info(logger,"Se paso a archivo");
	int socket_escucha = inicializar_coordinador(configuracion);
	conectar_planificador(socket_escucha);

	pthread_t hilo_escucha;
	pthread_t hilo_planificador;

	//pthread_create(&hilo_escucha, 0, procesar_conexion, (int*)&socket_escucha, (t_log*) logger);
	//pthread_create(&hilo_planificador, 0, atender_planificador, (&socket_planificador, logger));

	int socket_esi = aceptar_conexion(socket_escucha);
	handshake(socket_esi);
	int socket_instancia = aceptar_conexion(socket_escucha);
	handshake(socket_instancia);

	close(socket_escucha);

	return 0;
}
