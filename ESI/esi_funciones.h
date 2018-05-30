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
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = { ESI, 0 };
		int id_recibido;

		enviar(socket_servidor, &yo, sizeof(t_handshake), 80, logger);
		recibir(socket_servidor, &id_recibido, sizeof(int), logger);

		if (id_recibido != 80) {
			log_info(logger, "Conexion invalida");
			return -1;
		}

		recibir(socket_servidor, &proceso_recibido, sizeof(t_handshake), logger);

		if (!conexion_hecha) {
			conexion_hecha = 1;
			return 1;
		} else {
			return -1;
		}

	}

int buscar_instruccion(int socket_Planificador, t_log* logger_esi){
			if(!feof(script1)){
				if(enviar_instruccion_sgte(socket_Planificador, script1)){
					log_info(logger_esi, "Se ha enviado correctamente a instruccion al Planificador \n ");
					} else {
					log_info(logger_esi, "No se pudo enviar la instruccion al Planificador \n");
					enviar(&socket_Planificador, 60, sizeof(int), logger_esi);
					}
				} else {
					if(!feof(script2)){
						if(enviar_instruccion_sgte(socket_Planificador, script2)){
							log_info(logger_esi, "Se ha enviado correctamente a instruccion al Planificador \n ");
							return 1;
							} else {
								log_info(logger_esi, "No se pudo enviar la instruccion al Planificador \n");
								return 0;
								}
						} else {
							log_info(logger_esi, "No hay instrucciones disponibles. \n");
							return 0;
						}
					}
			return 0;
}

int enviar_instruccion_a_ejecutar(int socket_Coordinador, t_esi_operacion* mensaje, t_log* logger_esi){
	enviar(socket_Coordinador, mensaje->keyword, sizeof(mensaje->keyword), logger_esi);
	enviar(socket_Coordinador, mensaje->argumentos, sizeof(mensaje->argumentos), logger_esi);
	return 1;
}


#endif /* ESI_FUNCIONES_H_ */
