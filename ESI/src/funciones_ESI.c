#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>

t_config* leer_arch_configuracion(){
	t_config* configuracion_esi = config_create("ESI/Configuracion_ESI.cfg");
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

int handshake(int socket_servidor, int idProceso, t_log* logger) {

	t_handshake yo = { idProceso, ESI };
	t_handshake proceso_recibido;
	void* buffer = malloc(sizeof(int)*3);
	void *hs_recibido = malloc(sizeof(int)*2);
	int protocolo;

	serializar_handshake(buffer, yo);
	enviar(socket_servidor, buffer, sizeof(int)*3, 80, logger);
	log_info(logger, "Envie");
	free(buffer);

	protocolo = recibir_int(socket_servidor, logger);
	if (protocolo != 80) {
		log_info(logger, "Conexion invalida");
		return -1;
	}

	recibir(socket_servidor, hs_recibido, sizeof(int)*2, logger);
	proceso_recibido = deserializar_handshake(hs_recibido);
	free(hs_recibido);

	switch(proceso_recibido.id){
		case PLANIFICADOR:
			log_info(logger, "Me conecte con el Planificador");
			return 1;
			break;
		case COORDINADOR:
			log_info(logger, "Me conecte con el Coordinador");
			return 1;
			break;
		default:
			log_info(logger, "Conexion invalida");
			return -1;
			break;
	}
}

int enviar_instruccion_sgte(FILE* archivo, int socket_Coordinador, t_log* logger_esi){
	char* line;
	fgets(line, 0, archivo);
	t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	free(line);
	int exito = enviar(socket_Coordinador,&instruccion, sizeof(t_esi_operacion), 24, logger_esi);
	return exito;
}

int ejecutar_instruccion(FILE* script, int socket_Coordinador, t_log* logger){
	if(!feof(script)){
		if(enviar_instruccion_sgte(script, socket_Coordinador, logger) > 0){
			log_info(logger, "Se ha enviado correctamente a instruccion al Planificador \n ");
			return 1;
			} else {
				log_info(logger, "No se pudo enviar la instruccion al Planificador \n");
				return 0;
			}
		} else {
			log_info(logger, "No hay instrucciones disponibles. \n");
			return 0;
		}
}

resultado_esi deserializar_confirmacion(void* buffer_receptor){
	resultado_esi confirmacion;
	memcpy(&confirmacion, (buffer_receptor + (sizeof(int))), sizeof(resultado_esi));
	return confirmacion;
}

void informar_confirmacion(void* msj_recibido, int socket_destino, t_log* logger_esi){
	resultado_esi confirmacion = deserializar_confirmacion(msj_recibido);
	switch(confirmacion){
		case EXITO:
			log_info(logger_esi, "Instruccion ejecutada satisfactoriamente.");
			break;
		case FALLO:
			log_info(logger_esi, "Fallo al ejecutar la instruccion.");
			break;
		case PEDIUNACLAVEMUYLARGA:
			log_info(logger_esi, "Error de clave muy larga.");
			break;
		case PEDIUNACLAVENOID:
			log_info(logger_esi, "Error de clave no identificada.");
			break;
		case PEDIUNACLAVEINACC:
			log_info(logger_esi, "Error de clave inaccesible.");
			break;
	}
	enviar(socket_destino, &confirmacion, sizeof(resultado_esi), 41, logger_esi);
}

#endif /* ESI_FUNCIONES_H_ */
