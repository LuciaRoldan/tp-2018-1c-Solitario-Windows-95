#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>

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
int handshake(int socket_servidor) {

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
void inicializar_buffer_segun_keyword(void* buffer, t_esi_operacion instruccion){
	switch(instruccion.keyword){
	case GET:
	case STORE:
		buffer = malloc(sizeof(int)*5 + sizeof(t_esi_operacion));
		break;
	case SET:
		buffer = malloc(sizeof(int)*6 + sizeof(t_esi_operacion));
		break;
	}
}

/*int enviar_instruccion_sgte(FILE* archivo, int socket_Coordinador){
	char* line = malloc(sizeof(t_esi_operacion));
	fgets(line, 0, archivo);
	t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	free(line);
	ultima_instruccion = instruccion;
	void* buffer_instruccion;
	inicializar_buffer_segun_keyword(buffer_instruccion, instruccion);
	int tamanio_buffer = serializar_instruccion(buffer_instruccion, instruccion);
	int exito = enviar(socket_Coordinador, buffer_instruccion, tamanio_buffer, logger_esi);
	free(buffer_instruccion);
	return exito;
}

int ejecutar_instruccion(FILE* script, int socket_Coordinador){
	if(enviar_instruccion_sgte(script, socket_Coordinador) > 0){
		log_info(logger_esi, "Se ha enviado correctamente a instruccion al Planificador \n ");
		return 1;
		} else {
			log_info(logger_esi, "No se pudo enviar la instruccion al Planificador \n");
			return 0;
		}
}

void enviar_ultima_instruccion(int socket_destino){
	void* buffer_instruccion;
	inicializar_buffer_segun_keyword(buffer_instruccion, ultima_instruccion);
	//int tam_buffer = serializar_instruccion(buffer_instruccion, ultima_instruccion);
	//enviar(socket_destino, buffer_instruccion, tam_buffer, logger_esi);
	free(buffer_instruccion);
}*/

void informar_confirmacion(void* msj_recibido, int socket_destino, t_log* logger_esi){
	resultado_esi confirmacion;
	deserializar_confirmacion(&confirmacion, msj_recibido);
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
	void* buffer_confirmacion = malloc(sizeof(resultado_esi) + sizeof(int));
	serializar_confirmacion(buffer_confirmacion, &confirmacion);
	enviar(socket_destino, buffer_confirmacion, sizeof(resultado_esi) + sizeof(int), logger_esi);
	free(buffer_confirmacion);
}

void informar_fin_de_programa(sockets_conexiones conexiones){
	int envio = sizeof(int);
	serializar_id(&envio, 81);
	enviar(conexiones.socket_plani, &envio, sizeof(int), logger_esi);
	enviar(conexiones.socket_coordi, &envio, sizeof(int), logger_esi);
}

// Serializacion/Deserializacion
void serializar_confirmacion(void* buffer, resultado_esi *msj_confirmacion){
	int* id_protocolo = malloc(sizeof(int));
	*id_protocolo = 41;
	memcpy(buffer, id_protocolo, sizeof(int));
	memcpy(buffer, msj_confirmacion, sizeof(resultado_esi));
}
void deserializar_confirmacion(resultado_esi *confirmacion, void* buffer_receptor){
	memcpy(confirmacion, buffer_receptor, sizeof(resultado_esi));
}

/*int serializar_instruccion1(void* buffer, t_esi_operacion la_instruccion){
	int tamanio_clave, tamanio_valor, tam_buffer_aux, tam_envio;
	int tamanio_int = sizeof(int);
	int* id_protocolo = malloc(sizeof(int));
	*id_protocolo = 24;
	t_esi_operacion *ptr_instruccion = malloc(sizeof(t_esi_operacion));
	*ptr_instruccion = la_instruccion;
	memcpy(buffer, id_protocolo, sizeof(int));
	tam_envio = (sizeof(int)*2 + sizeof(la_instruccion._raw) + sizeof(la_instruccion.argumentos));
	memcpy(buffer + (sizeof(int)), &(la_instruccion.valido), sizeof(bool));
	memcpy(buffer + (sizeof(int)*2), &(la_instruccion.keyword), sizeof(int));
	switch(ptr_instruccion->keyword){
	case GET:
		tamanio_clave = strlen(ptr_instruccion->argumentos.GET.clave);
		void* buffer_aux_get = malloc(tamanio_clave + sizeof(int));
		memcpy(buffer_aux_get, &tamanio_clave, tamanio_clave);
		memcpy(buffer_aux_get + (sizeof(int)), ptr_instruccion->argumentos.GET.clave, tamanio_clave);
		tam_buffer_aux = sizeof(buffer_aux_get);
		memcpy(buffer + (sizeof(int)*5), buffer_aux_get, tam_buffer_aux);
		free(buffer_aux_get);
		break;
	case SET:
		tamanio_clave = strlen(ptr_instruccion->argumentos.SET.clave);
		tamanio_valor = strlen(ptr_instruccion->argumentos.SET.valor);
		void* buffer_aux_set = malloc(tamanio_clave + tamanio_valor + (sizeof(int)*2));
		memcpy(buffer_aux_set, &tamanio_clave, tamanio_clave);
		memcpy(buffer_aux_set + (sizeof(int)), ptr_instruccion->argumentos.SET.clave, tamanio_clave);
		memcpy(buffer_aux_set + sizeof(int) + tamanio_clave, &tamanio_valor, tamanio_valor);
		memcpy((buffer_aux_set + (sizeof(int)*2) + tamanio_clave), (ptr_instruccion->argumentos.SET.valor), tamanio_valor);
		tam_buffer_aux = sizeof(buffer_aux_get);
		memcpy(buffer + (sizeof(int)*5), buffer_aux_set, tam_buffer_aux);
		free(buffer_aux_set);
		break;
	case STORE:
		tamanio_clave = strlen(ptr_instruccion->argumentos.STORE.clave);
		void* buffer_aux_store = malloc(tamanio_clave + sizeof(int));
		memcpy(buffer_aux_store, &tamanio_clave, tamanio_clave);
		memcpy(buffer_aux_store + (sizeof(int)), ptr_instruccion->argumentos.STORE.clave, tamanio_clave);
		tam_buffer_aux = sizeof(buffer_aux_get);
		memcpy(buffer + (sizeof(int)*5), buffer_aux_store, tam_buffer_aux);
		free(buffer_aux_store);
	}
	int tam_raw = strlen(*(ptr_instruccion->_raw));
	memcpy(buffer + (sizeof(int)*5) + tam_buffer_aux, *(ptr_instruccion->_raw), tam_raw);
	int tamanio_buffer = sizeof(buffer);

	free(id_protocolo);
	free(ptr_instruccion);

	return tamanio_buffer ;

}*/

/*t_esi_operacion deserializar_instruccion1(void* buffer) {
	t_esi_operacion instruccion;
	int tamanio_clave, tamanio_valor, tamanio_raw;
	memcpy(&(instruccion.valido), (buffer + sizeof(int)), sizeof(int));
	memcpy(&(instruccion.keyword), (buffer + sizeof(int)*2), sizeof(int));
	memcpy(&tamanio_clave, (buffer + sizeof(int) * 3), sizeof(int));
	switch (instruccion.keyword) {
	case (GET):
		memcpy(&(instruccion.argumentos.GET.clave), (buffer + sizeof(int) * 4),tamanio_clave);
		memcpy(&tamanio_raw, (buffer + sizeof(int) * 4 + tamanio_clave), sizeof(int));
		memcpy(&(instruccion._raw), (buffer + (sizeof(int) * 5) + tamanio_clave), tamanio_raw);
		break;
	case (SET):
		memcpy(&(instruccion.argumentos.SET.clave), (buffer + sizeof(int) * 4),tamanio_clave);
		memcpy(&tamanio_valor, (buffer + sizeof(int)*4 + tamanio_clave), sizeof(int));
		memcpy(&(instruccion.argumentos.SET.valor), (buffer + (sizeof(int) * 5) + tamanio_clave), tamanio_valor);
		memcpy(&(instruccion._raw), (buffer + (sizeof(int) * 5) + tamanio_clave + tamanio_valor), sizeof(int));
		memcpy(&(instruccion._raw), (buffer + (sizeof(int) * 6) + tamanio_clave + tamanio_valor), tamanio_raw);
		break;
	case (STORE):
		memcpy(&(instruccion.argumentos.STORE.clave),(buffer + sizeof(int) * 4), tamanio_clave);
		memcpy(&tamanio_raw, (buffer + sizeof(int) * 4 + tamanio_clave), sizeof(int));
		memcpy(&(instruccion._raw), (buffer + (sizeof(int) * 5) + tamanio_clave), tamanio_raw); // Ver cómo resolver el tema de que s un puntero a string...
		break;
	}
	return instruccion;
}*/

// VA EN LAS COMMONS
int tamanio_instruccion2(t_esi_operacion instruccion){
	//char* clave, valor;
	int tamanio, tam_clave, tam_valor;
	int tamanio_base = sizeof(instruccion.valido) + sizeof(instruccion.keyword) + sizeof(instruccion._raw) + 2;
	switch(instruccion.keyword){
	case GET:
		tam_clave = strlen(instruccion.argumentos.GET.clave);
		//clave = instruccion.argumentos.GET.clave;
		return tamanio_base + tam_clave;
		break;
	case SET:
		tam_clave = strlen(instruccion.argumentos.SET.clave);
		tam_valor = strlen(instruccion.argumentos.SET.valor);
		//strcpy(&valor, instruccion.argumentos.SET.valor);
		tamanio = tamanio_base + tam_clave + tam_valor;
		return tamanio;
		break;
	case STORE:
		tam_clave = strlen(instruccion.argumentos.STORE.clave);
		//clave = instruccion.argumentos.STORE.clave;
		return tamanio_base + tam_clave;
		break;
	default:
		return -1;
		break;
	}
}

void serializar_instruccion2(void* buffer, t_esi_operacion la_instruccion){
	int tam_instruc = tamanio_instruccion2(la_instruccion);
	int id_protocolo = 24;
	t_esi_operacion *ptr_instruccion = malloc(sizeof(t_esi_operacion));
	*ptr_instruccion = la_instruccion;
	memcpy(buffer, &id_protocolo, sizeof(int));
	memcpy(buffer + (sizeof(int)), &tam_instruc, sizeof(int));
	memcpy(buffer + (sizeof(int)*2), ptr_instruccion, sizeof(tam_instruc));

	free(ptr_instruccion);
}


t_esi_operacion deserializar_instruccion2(void* buffer) {
	t_esi_operacion instruccion;
	int tamanio_recibido;
	memcpy(&tamanio_recibido, (buffer + sizeof(int)), sizeof(int));
	memcpy(&instruccion, (buffer + sizeof(int)*2), tamanio_recibido);
	return instruccion;
}

#endif /* ESI_FUNCIONES_H_ */
