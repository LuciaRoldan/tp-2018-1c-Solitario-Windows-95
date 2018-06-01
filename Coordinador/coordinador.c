#include "coordinador_funciones.h"

t_log * logger;
t_conexion conexionInstancia;
FILE* archivo_configuracion;
int socket_planificador;

int main(){
	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	info_archivo_config configuracion = leer_archivo_configuracion(archivo_configuracion);
	int socket_escucha = inicializar_coordinador(configuracion, logger);
	conectar_planificador(&socket_escucha, &socket_planificador, logger);

	pthread_t hilo_escucha;
	pthread_t hilo_planificador;

	//pthread_create(&hilo_escucha, 0, procesar_conexion, (int*)&socket_escucha, (t_log*) logger);
	//pthread_create(&hilo_planificador, 0, atender_planificador, (&socket_planificador, logger));

	return 0;
}

