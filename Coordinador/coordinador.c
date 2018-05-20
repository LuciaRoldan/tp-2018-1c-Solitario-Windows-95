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

	return 0;
}

