#include "coordinador_funciones.h"


t_conexion conexion;
t_log * logger;
t_conexion conexionInstancia;
FILE* archivo_configuracion;

int main(){
	/*
	t_config* configuracion=config_create("/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Config");
	strcpy(conexion.ip,config_get_string_value(configuracion,"IP_COORDINADOR"));
	strcpy(conexion.puerto,config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	 */
	logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);
	info_archivo_config datos_configuracion = inicializar_coordinador(archivo_configuracion);

	return 0;
}

/*
void inicializar_coordinador;
void leer_archivo_configuracion;
void enviar_configuracion_instancia;
void configurar_planificador;
void esperar_contenido;
void atender_conexion_esi;
*/
