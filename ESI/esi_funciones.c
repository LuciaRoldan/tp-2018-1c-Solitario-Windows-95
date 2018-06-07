#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "esi.h"

info_arch_config leer_arch_configuracion(FILE* arch_configuracion, t_conexion conexion_plani, t_conexion conexion_coordi){
	info_arch_config datos_configuracion;
	FILE* archivo = fopen("Configuracion ESI.txt", "r");
	fscanf(archivo, "%s %s %s %s", datos_configuracion.ip_coordinador,
			datos_configuracion.puerto_coordinador, datos_configuracion.ip_planificador,
			datos_configuracion.puerto_planificador);
	conexion_plani.ip = datos_configuracion.ip_planificador;
	conexion_plani.puerto = datos_configuracion.puerto_planificador;
	conexion_coordi.ip = datos_configuracion.ip_coordinador;
	conexion_coordi.puerto = datos_configuracion.puerto_coordinador;
	return datos_configuracion;
}

int handshake(int* socket_servidor, t_log* logger) {

		t_handshake yo = { ESI, idEsi };
		int id_recibido;

		enviar(socket_servidor, &yo, sizeof(t_handshake), 80, logger);
		recibir(socket_servidor, &id_recibido, sizeof(int), logger);

		if (id_recibido != 80) {
			log_info(logger, "Conexion invalida");
			return -1;
		} else return 1;
	}

int ejecutar_instruccion(FILE* script, int socket_Coordinador, t_log* logger_esi){
			if(!feof(script)){
				if(enviar_instruccion_sgte(socket_Coordinador, script, logger_esi) > 0){
					log_info(logger_esi, "Se ha enviado correctamente a instruccion al Planificador \n ");
					} else log_info(logger_esi, "No se pudo enviar la instruccion al Planificador \n");
				} else {
						log_info(logger_esi, "No hay instrucciones disponibles. \n");
						return -1;
						}
			return 1;
}

int enviar_instruccion_sgte(int socket_Coordinador, FILE* archivo){
	resultados_esi resultado;
	char* line = scanf("%s", archivo);
	t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	int exito = enviar(socket_Coordinador,&instruccion, sizeof(t_esi_operacion), 24, logger_esi);
	if (exito) {return 1;} else return -1;
}

#endif /* ESI_FUNCIONES_H_ */

