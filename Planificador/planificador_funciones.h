#include "planificador.h"
#include "planificador.c"

void inicializar_planificador(){
	leer_archivo_configuracion();
	inicializar_servidor();
}

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: puertoEscucha, algoritmoPlanificacion, estimacionInicial, ipCoordinador, puertoCoordinador y clavesInicialmenteBloqueadas
	configuracion = fopen("archivo_configuracion_planificador.txt", "r");
	fscanf(configuracion, "%s %d %d %s %s %s", &puertoEscucha, &algoritmoPlanificacion, &estimacionInicial , &ipCoordinador, &puertoCoordinador, &clavesInicialmenteBloqueadas);
	fclose(configuracion);
}

void inicializar_servidor(){
	t_config* configuracion=config_create("/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Config");
	strcpy(conexion.ip,config_get_string_value(configuracion,"IP_COORDINADOR"));
	strcpy(conexion.puerto,config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
	connect_to_server(conexion.ip, conexion.puerto, logger);
}

void escuchar_esis(){
	wait_content(atoi(*puertoEscucha));

}
