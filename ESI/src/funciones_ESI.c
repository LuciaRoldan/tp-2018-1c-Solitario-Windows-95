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
nodo* parsear(FILE* archivo){
	nodo* raiz = malloc(sizeof(nodo));
	nodo* ptr_aux = raiz;
	nodo* ptr_ant = NULL;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, archivo)) != -1) {
		t_esi_operacion parsed = parse(line);
		if(parsed.valido){
			ptr_aux->instruccion = parsed;
			ptr_aux->anterior = ptr_ant;
			ptr_ant = ptr_aux;
			ptr_aux->sgte  = malloc(sizeof(nodo));
			ptr_aux = ptr_aux->sgte;
			destruir_operacion(parsed);
		} else {
			fprintf(stderr, "La linea <%s> no es valida\n", line);
			exit(EXIT_FAILURE);
		}
	}
	free(ptr_aux);
	ptr_ant->sgte = NULL;
	free(ptr_ant);
	free(line);
	return raiz;
}

int enviar_instruccion(nodo* ptr_sentencia, int socket_Coordinador){
	t_esi_operacion instruccion = ptr_sentencia->instruccion;
	int tamanio_buffer = tamanio_instruccion(instruccion);
	void* buffer_instruccion = malloc(tamanio_buffer);
	serializar_instruccion(buffer_instruccion, instruccion);
	int exito = enviar(socket_Coordinador, buffer_instruccion, tamanio_buffer, logger_esi);
	free(buffer_instruccion);
	return exito;
}

int ejecutar_instruccion_sgte(int socket_Coordinador){
	if(enviar_instruccion(sentencia_actual->sgte, socket_Coordinador) > 0){
		log_info(logger_esi, "Se ha enviado correctamente a instruccion al Planificador \n ");
		sentencia_actual = sentencia_actual->sgte;
		return 1;
		} else {
			log_info(logger_esi, "No se pudo enviar la instruccion al Planificador \n");
			return 0;
		}
}

void ejecutar_ultima_instruccion(int socket_destino){
	enviar_instruccion(sentencia_actual->anterior, socket_destino);
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
	void* buffer_confirmacion = malloc(sizeof(resultado_esi) + sizeof(int));
	serializar_confirmacion(buffer_confirmacion, &confirmacion);
	enviar(socket_destino, buffer_confirmacion, sizeof(resultado_esi) + sizeof(int), logger_esi);
	free(buffer_confirmacion);
}

void informar_fin_de_programa(sockets_conexiones conexiones){
	int envio;
	serializar_id(&envio, 81);
	enviar(conexiones.socket_plani, &envio, sizeof(int), logger_esi);
	enviar(conexiones.socket_coordi, &envio, sizeof(int), logger_esi);
}

void liberar_lista(nodo* raiz){
	nodo *aux1 = raiz;
	nodo *aux2 = raiz->sgte;
	while (aux1->sgte != NULL){
		free(aux1->anterior);
		free(aux1->sgte);
		aux1 = aux2;
		aux2 = aux2->sgte;
	}
	free(aux1);
	free(aux2);
	free(raiz);
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

/*/ ETÁ EN LAS COMMONS
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
}*/

#endif /* ESI_FUNCIONES_H_ */
