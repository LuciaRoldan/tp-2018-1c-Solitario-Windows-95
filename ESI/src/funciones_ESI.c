#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>

t_config* leer_arch_configuracion(){
	t_config* configuracion_esi = config_create("ESI/Configuracion_ESI.cfg");
	idEsi = config_get_int_value(configuracion_esi, "ID");
	/*info_arch_config datos_config;
	FILE* archivo = fopen("../Configuracion_ESI.txt", "r");
	fscanf(archivo, "%d %s %s %s %s",
			&(datos_config.id_esi),
			datos_config.ip_coordinador,
			datos_config.puerto_coordinador, datos_config.ip_planificador,
			datos_config.puerto_planificador);
	fclose(archivo);*/

	return configuracion_esi;
}

int conectarse_al_Coordinador(t_config * arch_config, t_log* logger){
	int socket = connect_to_server(config_get_string_value(arch_config, "IP_COORDINADOR"), config_get_string_value(arch_config, "PORT_COORDINADOR"), logger);
	return socket;
}

int conectarse_al_Planificador(t_config * arch_config, t_log* logger){
	int socket = connect_to_server(config_get_string_value(arch_config, "IP_PLANIFICADOR"), config_get_string_value(arch_config, "PORT_PLANIFICADOR"), logger);
	return socket;
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

int enviar_instruccion_sgte(int* socket_Coordinador, FILE* archivo, t_log* logger_esi){
	char* line;
	fscanf(archivo, "%s", line);
	t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	free(line);
	int exito = enviar(socket_Coordinador,&instruccion, sizeof(t_esi_operacion), 24, logger_esi);
	return exito;
}

int ejecutar_instruccion(FILE* script, int* socket_Coordinador, t_log* logger_esi){
			if(!feof(script)){
				if(enviar_instruccion_sgte(socket_Coordinador, script, logger_esi) > 0){
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

#endif /* ESI_FUNCIONES_H_ */
