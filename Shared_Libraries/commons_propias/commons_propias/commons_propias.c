#include "commons_propias.h"


int connect_to_server(char * ip, char * port, t_log *  logger){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0 ,sizeof(hints));
	hints.ai_family = AF_UNSPEC; //se fija solo si es IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM; //protocolo TCP

	getaddrinfo(ip, port, &hints, &server_info); //guarda en server_info los datos de la conexion

	int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol); //creo el socket con server info

	int valorConnect = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info); //libero el arreglo

	if (valorConnect < 0) { //ver niveles de logs de commons, verifico por error
		_exit_with_error(server_socket, "No me pude conectar al servidor", NULL, logger);
	}

		log_info(logger, "Me conecte al servidor!"); //logeo exito
		return server_socket;
}

int inicializar_servidor(int puerto, t_log * logger){

	struct sockaddr_in configuracion;
	int servidor;

	servidor = socket(AF_INET, SOCK_STREAM, 0);

	configuracion.sin_family = AF_INET;
	configuracion.sin_addr.s_addr = INADDR_ANY;
	configuracion.sin_port = htons( puerto );

	if( bind(servidor,(struct sockaddr *)&configuracion , sizeof(configuracion)) < 0) {
		_exit_with_error(servidor, "Fallo el bind", NULL, logger);
	}

	listen(servidor, 100);
	log_info(logger, "Soy servidor y estoy escuchando!");

	return servidor;
}

//Funciones para enviar y recibir cosas serializadas

int enviar(int socket_destino, void* envio, int tamanio_del_envio, t_log* logger){
	int bytes_enviados = send(socket_destino, envio, tamanio_del_envio, 0);
 	if(bytes_enviados <= 0){
 		printf("Bytes_enviados: %d\n", bytes_enviados);
 		_exit_with_error(socket_destino, "No se pudo enviar el mensaje", NULL, logger);
 	}
 	return bytes_enviados;
 }

int recibir(int socket_receptor, void* buffer_receptor, int tamanio_que_recibo, t_log* logger){

	int bytes_recibidos = recv(socket_receptor, buffer_receptor, tamanio_que_recibo, MSG_WAITALL);
	if (bytes_recibidos <= 0) {
			_exit_with_error(socket_receptor, "Error recibiendo el contenido", NULL, logger);
		}

	return bytes_recibidos;
}

void _exit_with_error(int socket, char* error_msg, void * buffer, t_log* logger) {
	if (buffer != NULL) {
		free(buffer);
	}
	log_error(logger, error_msg);
	close(socket);
	exit_gracefully(1, logger);
}

void exit_gracefully(int return_nr, t_log* logger) {
	log_destroy(logger);
	exit(return_nr);
}

int aceptar_conexion(int socket_escucha){
	struct sockaddr_in cliente;
	socklen_t tamanio = sizeof(cliente);
	int socket_cliente = accept(socket_escucha, (struct sockaddr *) &cliente, &tamanio);
	return socket_cliente;
}

int recibir_int(int socket, t_log* logger){
	int id;
	recibir(socket, &id, sizeof(int), logger);
	return id;
}


////////////////////----------SERIALIZAR Y DESEREALIZAR----------////////////////////

//////////-----PARA TODOS-----//////////

void serializar_id(void* envio, int id){
	memcpy(envio, &id, sizeof(int));

}

int deserializar_id(void* buffer_mensaje){
	int id;
	memcpy(&id, buffer_mensaje, sizeof(int));
	return id;
}

void serializar_handshake(void* buffer, t_handshake handshake){
	int* id_protocolo = malloc(sizeof(int));
	*id_protocolo = 80;
	t_handshake *info_handshake = malloc(sizeof(t_handshake));
	*info_handshake = handshake;
	memcpy(buffer, id_protocolo, sizeof(int));
	memcpy((buffer + (sizeof(int))), info_handshake, sizeof(t_handshake));
	free(info_handshake);
	free(id_protocolo);
}

t_handshake deserializar_handshake(void *buffer_recepcion){
	t_handshake handshake_recibido;
	memcpy(&handshake_recibido.id, buffer_recepcion, sizeof(int));
	memcpy(&handshake_recibido.proceso, buffer_recepcion + sizeof(int), sizeof(int));
	return handshake_recibido;
}

void serializar_instruccion(void* buffer, t_esi_operacion la_instruccion){
	int tamanio_clave, tamanio_valor, tam_buffer_aux;
	int tamanio_mensaje = tamanio_instruccion(la_instruccion) - 2*sizeof(int);
	int* id_protocolo = malloc(sizeof(int));
	*id_protocolo = 82;

	memcpy(buffer, id_protocolo, sizeof(int));
	memcpy(buffer + sizeof(int), &tamanio_mensaje, sizeof(int)); // Ver si conviene... Cambie tamaño int por tamanio mensaje
	memcpy(buffer + (sizeof(int)*2), &(la_instruccion.valido), sizeof(bool));
	memcpy(buffer + (sizeof(int)*2 + sizeof(bool)), &(la_instruccion.keyword), sizeof(int));
	//Hasta aca guardamos el protocolo, el tamaño para buffer, el bool y la keyword


	switch(la_instruccion.keyword){
	case GET:
		tamanio_clave = strlen(la_instruccion.argumentos.GET.clave);
		tam_buffer_aux = tamanio_clave + sizeof(int);
		void* buffer_aux_get = malloc(tam_buffer_aux);
		memcpy(buffer_aux_get, &tamanio_clave, sizeof(int));
		memcpy(buffer_aux_get + (sizeof(int)), &la_instruccion.argumentos.GET.clave, tamanio_clave);
		//Hasta aca tenemos en un buffer nuevo con el tamaño de la clave y la clave

		memcpy(buffer + sizeof(int)*3 + sizeof(bool), buffer_aux_get, tam_buffer_aux);
		free(buffer_aux_get);
		break;

	case SET:
		tamanio_clave = strlen(la_instruccion.argumentos.SET.clave);
		tamanio_valor = strlen(la_instruccion.argumentos.SET.valor);
		tam_buffer_aux = tamanio_clave + tamanio_valor + (sizeof(int)*2);
		void* buffer_aux_set = malloc(tam_buffer_aux);
		memcpy(buffer_aux_set, &tamanio_clave, sizeof(int));
		memcpy(buffer_aux_set + (sizeof(int)), &la_instruccion.argumentos.SET.clave, tamanio_clave);
		memcpy(buffer_aux_set + sizeof(int) + tamanio_clave, &tamanio_valor, sizeof(int));
		memcpy((buffer_aux_set + (sizeof(int)*2) + tamanio_clave), &la_instruccion.argumentos.SET.valor, tamanio_valor);
		//Hasta aca tenemos en un buffer nuevo con el tamaño de la clave, la clave, el tamaño del valor y el valor

		memcpy(buffer + sizeof(int)*3 + sizeof(bool), buffer_aux_set, tam_buffer_aux);
		free(buffer_aux_set);
		break;

	case STORE:
		tamanio_clave = strlen(la_instruccion.argumentos.STORE.clave);
		tam_buffer_aux = tamanio_clave + sizeof(int);
		void* buffer_aux_store = malloc(tam_buffer_aux);
		memcpy(buffer_aux_store, &tamanio_clave, sizeof(int));
		memcpy(buffer_aux_store + (sizeof(int)), &la_instruccion.argumentos.STORE.clave, tamanio_clave);
		//Hasta aca tenemos en un buffer nuevo con el tamaño de la clave y la clave

		memcpy(buffer + sizeof(int)*3 + sizeof(bool), buffer_aux_store, tam_buffer_aux);
		free(buffer_aux_store);
	}

	int tam_raw = sizeof(la_instruccion._raw);
	memcpy(buffer + sizeof(int)*3 + sizeof(bool) + tam_buffer_aux, &tam_raw, sizeof(int));
	memcpy(buffer + sizeof(int)*4 + sizeof(bool) + tam_buffer_aux, la_instruccion._raw, tam_raw);


	free(id_protocolo);

}

t_esi_operacion deserializar_instruccion(void* buffer) {
	t_esi_operacion instruccion;
	int tamanio_clave, tamanio_valor, tamanio_raw;
	memcpy(&(instruccion.valido), buffer, sizeof(bool));
	memcpy(&(instruccion.keyword), (buffer + sizeof(bool)), sizeof(int));
	memcpy(&tamanio_clave, (buffer + sizeof(bool) + sizeof(int)), sizeof(int));

	switch (instruccion.keyword) {
	case (GET):
		memcpy(&(instruccion.argumentos.GET.clave), (buffer + sizeof(bool) + sizeof(int)*2),tamanio_clave);
		memcpy(&tamanio_raw, (buffer + sizeof(bool) + sizeof(int)*2 + tamanio_clave), sizeof(int));
		memcpy(&instruccion._raw, (buffer + sizeof(bool) + sizeof(int)*3 + tamanio_clave), tamanio_raw);
		break;
	case (SET):
		memcpy(&(instruccion.argumentos.SET.clave), (buffer + sizeof(bool) + sizeof(int)*2), tamanio_clave);
		memcpy(&tamanio_valor, (buffer + sizeof(bool) + sizeof(int)*2 + tamanio_clave), sizeof(int));
		memcpy(&(instruccion.argumentos.SET.valor), (buffer + sizeof(bool) + sizeof(int)*3 + tamanio_clave), tamanio_valor);
		memcpy(&tamanio_raw, (buffer + sizeof(bool) + sizeof(int)*3 + tamanio_clave + tamanio_valor), sizeof(int));
		memcpy(&instruccion._raw, (buffer + sizeof(bool) + sizeof(int)*4 + tamanio_clave + tamanio_valor), tamanio_raw);
		break;
	case (STORE):
		memcpy(&(instruccion.argumentos.STORE.clave),(buffer + sizeof(bool) + sizeof(int)*2), tamanio_clave);
		memcpy(&tamanio_raw, (buffer + sizeof(bool) + sizeof(int)*2 + tamanio_clave), sizeof(int));
		memcpy(&instruccion._raw, (buffer + sizeof(bool) + sizeof(int)*3 + tamanio_clave), tamanio_raw);
		break;
	}
	return instruccion;
}

int tamanio_instruccion(t_esi_operacion instruccion){
	char* clave, valor;
	int tamanio_base = sizeof(instruccion.valido) + sizeof(instruccion.keyword) +strlen(*instruccion._raw) + 4*sizeof(int);
	switch(instruccion.keyword){
		case GET:
			clave = instruccion.argumentos.GET.clave;
			return tamanio_base + strlen(instruccion.argumentos.GET.clave);
			break;
		case SET:
			clave = instruccion.argumentos.SET.clave;
			valor = instruccion.argumentos.SET.valor;
			return tamanio_base + strlen(instruccion.argumentos.SET.clave) + strlen(instruccion.argumentos.SET.valor) + sizeof(int);
			break;
		case STORE:
			clave = instruccion.argumentos.STORE.clave;
			return tamanio_base + strlen(instruccion.argumentos.STORE.clave);
			break;
		default:
			return -1;
			break;
	}
}



//////////PARA PLANIFICADOR//////////

void deserializar_pedido_coordinador(void* buffer, pedido_esi* pedido){ //HACER
}

// COMMONS CONEXIONES //

/*
int send_string(int socket, char* mensaje){
int len, bytes_sent;

len = strlen(mensaje);


bytes_sent= send(socket,mensaje,len,0);


return bytes_sent;

}
*/


