#include <stdio.h>
#include <stdlib.h>

#include "Funciones_coordinador.h"


int main(){

	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	log_operaciones = log_create("Operaciones.log", "LOGOP", true, LOG_LEVEL_INFO);
	log_info(logger,"Loggers creados");

	leer_archivo_configuracion();
	inicializar_coordinador();
	conectar_planificador();
	log_info(logger,"Inicializacion completa");

	pthread_t hilo_escucha;
	pthread_t hilo_planificador;

	pthread_create(&hilo_escucha, 0, procesar_conexion, NULL);
	pthread_create(&hilo_planificador, 0, atender_planificador, NULL);

	log_info(logger, "Hilos principales creados");

	while(!terminar_programa){
			sem_wait(&s_cerrar_hilo);
			if(!terminar_programa){
			pthread_join(*hilo_a_cerrar, NULL);
			log_info(logger, "Hilo cerrado");
			pthread_mutex_unlock(&m_hilo_a_cerrar);
			}
	}

	pthread_join(hilo_escucha, NULL);
	pthread_join(hilo_planificador, NULL);
	close(socket_escucha);
	log_info(logger, "El coordinador termino su ejecucuion");

	return 0;
}
