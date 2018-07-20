#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>
#include <parsi/parser.h>

// Configuracion ESI
sockets_conexiones leer_arch_configuracion(){
	sockets_conexiones conexion;
	configuracion_esi = config_create("ESI/Configuracion_ESI.cfg");
	conexion.socket_coordi = conectarse_al_Coordinador();
	conexion.socket_plani = conectarse_al_Planificador();
	return conexion;
}

// Conexiones
int conectarse_al_Coordinador(){
	int socket = connect_to_server(config_get_string_value(configuracion_esi, "IP_COORDINADOR"), config_get_string_value(configuracion_esi, "PORT_COORDINADOR"), logger_esi);
	return socket;
}

int conectarse_al_Planificador(){
	int socket = connect_to_server(config_get_string_value(configuracion_esi, "IP_PLANIFICADOR"), config_get_string_value(configuracion_esi, "PORT_PLANIFICADOR"), logger_esi);
	return socket;
}

// Handshake
int handshake_del_esi(int socket_servidor) {

	t_handshake yo = { idEsi, ESI };
	t_handshake proceso_recibido;
	void* buffer = malloc(sizeof(int)*3);
	void *hs_recibido = malloc(sizeof(int)*2);
	int protocolo;

	serializar_handshake(buffer, yo);
	enviar(socket_servidor, buffer, sizeof(int)*3, logger_esi);
	log_info(logger_esi, "Envie el Handshake");
	free(buffer);

	protocolo = recibir_int(socket_servidor, logger_esi);
	log_info(logger_esi, "Id de protocolo recibido: %d \n", protocolo);
	if (cumple_protocolo(protocolo, 80) == 0) {
		log_info(logger_esi, "Conexion invalida: error en protocolo.");
		return -1;
	}

	recibir(socket_servidor, hs_recibido, sizeof(int)*2, logger_esi);
	proceso_recibido = deserializar_handshake(hs_recibido);
	free(hs_recibido);

	switch(proceso_recibido.proceso){
		case PLANIFICADOR:
			log_info(logger_esi, "Soy ESI %d y me conecte con el Planificador", idEsi);
			return 1;
			break;
		case COORDINADOR:
			log_info(logger_esi, "Soy ESI %d y me conecte con el Coordinador", idEsi);
			return 1;
			break;
		default:
			log_info(logger_esi, "Conexion invalida: error de proceso no válido.");
			return -1;
			break;
	}
}

// Verificaciones
int cumple_protocolo(int mensaje, int nro){
	if(mensaje == nro){
		return 1;
	} else return 0;
}

// Acciones
t_esi_operacion parsear_linea(FILE* archivo){
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	read = getline(&line, &len, archivo);
	t_esi_operacion parsed = parse(line);
	return parsed;
}

int enviar_instruccion(t_esi_operacion instruccion, int socket_Coordinador){
	int tamanio_buffer = tamanio_buffer_instruccion(instruccion);
	void* buffer_instruccion = malloc(tamanio_buffer);
	serializar_instruccion(buffer_instruccion, instruccion);
	log_info(logger_esi, "Fin de serializacion de instruccion");
	int exito = enviar(socket_Coordinador, buffer_instruccion, tamanio_buffer, logger_esi);
	log_info(logger_esi, "Instruccion enviada al COORDINADOR.");
	free(buffer_instruccion);
	return exito;
}

int ejecutar_instruccion_sgte(FILE* archivo, int socket_Coordinador){
	t_esi_operacion operacion = parsear_linea(archivo);
	log_info(logger_esi,"Parseado de instruccion completo");
	ultima_instruccion = operacion;
	if(enviar_instruccion(operacion, socket_Coordinador) > 0){
		log_info(logger_esi, "Se ha enviado correctamente a instruccion al Coordinador \n ");
		return 1;
		} else {
			log_info(logger_esi, "No se pudo enviar la instruccion al Coordinador \n");
			return 0;
		}
}

void ejecutar_ultima_instruccion(int socket_destino){
	enviar_instruccion(ultima_instruccion, socket_destino);
}

void informar_confirmacion(int confirmacion, int socket_destino, t_log* logger_esi){
	switch(confirmacion){
		case 84:
			log_info(logger_esi, "Instruccion ejecutada satisfactoriamente.");
			break;
		case 85:
			log_info(logger_esi, "Fallo al ejecutar la instruccion.");
			break;
		case 86:
			log_info(logger_esi, "Error de clave muy larga.");
			break;
		case 87:
			log_info(logger_esi, "Error de clave no identificada.");
			break;
		case 88:
			log_info(logger_esi, "Error de clave inaccesible.");
			break;
		case 89:
			log_info(logger_esi, "Error de clave no bloqueada.");
			break;
		case 90:
			log_info(logger_esi, "Bloquear ESI.");
			break;
		default:
			break;
	}
	void* buffer_confirmacion = malloc(sizeof(int));
	serializar_id(buffer_confirmacion, confirmacion);
	enviar(socket_destino, buffer_confirmacion, sizeof(int), logger_esi);
	free(buffer_confirmacion);
}

void informar_fin_de_programa(sockets_conexiones conexiones, int flag){
	int envio;
	serializar_id(&envio, 81);
	if(flag == 0){
		log_info(logger_esi, "Fin de script.\n");
		enviar(conexiones.socket_coordi, &envio, sizeof(int), logger_esi);
		enviar(conexiones.socket_plani, &envio, sizeof(int), logger_esi);
	} else{
		log_info(logger_esi, "Fallo de clave. Abortando ESI.\n");
		int aborto;
		serializar_id(&aborto, 85);
		enviar(conexiones.socket_plani, &aborto, sizeof(int), logger_esi);
	}
}

// Serializacion/Deserializacion
void serializar_confirmacion(void* buffer, resultado_esi *msj_confirmacion){
	int id_protocolo = 41;
	memcpy(buffer, &id_protocolo, sizeof(int));
	memcpy(buffer, msj_confirmacion, sizeof(resultado_esi));
}
resultado_esi deserializar_confirmacion(void* buffer_receptor){
	resultado_esi confirmacion;
	memcpy(&confirmacion, buffer_receptor + sizeof(int), sizeof(resultado_esi));
	return confirmacion;
}

#endif /* ESI_FUNCIONES_H_ */
