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

	int descrip_socket, optval = 1;
		struct sockaddr_in servername;

		/* Create the socket. */
		descrip_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (descrip_socket < 0) {
			perror("socket");
			return -1;
		}

		/* Set socket options. */
		if (setsockopt(descrip_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)
				== -1) {
			perror("setsockopt");
			return -2;
		}

		/* Fill ip / port info. */
		servername.sin_family = AF_INET;
		servername.sin_addr.s_addr = htonl(INADDR_ANY);
		servername.sin_port = htons(puerto);

		/* Give the socket a name. */
		if (bind(descrip_socket, (struct sockaddr *) &servername, sizeof servername) < 0) {
			perror("bind");
			return -3;
		}

		/* Listen to incoming connections. */
		if (listen(descrip_socket, 127) < 0) {
			perror("listen");
			return -4;
		}



	/*int descrip_socket; //Declaramos el descriptor

		struct addrinfo infoSocket, *infoServidor; //Declaramos las estructuras

		memset(&infoSocket, 0, sizeof(infoSocket)); //Seteamos el valor de la informacion de la direccion a 0
		//Asignamos los valores de info de la conexion
		infoSocket.ai_family = AF_UNSPEC; //Indica a getaddrinfo(3) que la direccion va a ser de tipo IPv4 o IPv6
		infoSocket.ai_flags = AI_PASSIVE; //Solo para cuando se quiere utilizar el socket para un servidor
		infoSocket.ai_socktype = SOCK_STREAM; //Socket de tipo TCP/IP

		char * dirIP = NULL;
		char * port = malloc(sizeof(int));
		memcpy(port, &puerto, sizeof(int));

		//Obtenemos la direccion y los datos del socket y los mete en infoServidor
		if (getaddrinfo(dirIP, port, &infoSocket, &infoServidor) != 0) {
			perror("No se pudo obtener la direccion correctamente.");
			return -1;
		}

		//Creamos el socket
		descrip_socket = socket(infoServidor->ai_family, infoServidor->ai_socktype,
				infoServidor->ai_protocol);
		if (descrip_socket == -1) {
			freeaddrinfo(infoServidor);
			perror("No se pudo crear el socket");
			return -1;
		}

		while (bind(descrip_socket, infoServidor->ai_addr, infoServidor->ai_addrlen)<0){}*/



	/*struct addrinfo configuracion;
	int servidor;

	servidor = socket(AF_INET, SOCK_STREAM, 0);

	configuracion.ai_family = AF_INET;
	configuracion.ai_addr.s_addr = INADDR_ANY;
	configuracion.ai_flags = AI_PASSIVE;
	configuracion.ai_socktype = SOCK_STREAM;
	configuracion.sin_port = htons( puerto );

	while(bind(servidor,(struct sockaddr *)&configuracion , sizeof(configuracion)) < 0){}*/

	listen(descrip_socket, 100);
	log_info(logger, "Soy servidor y estoy escuchando!");

	return descrip_socket;
}

//Funciones para enviar y recibir cosas serializadas

int enviar(int socket_destino, void* envio, int tamanio_del_envio, t_log* logger){
	int bytes_enviados = send(socket_destino, envio, tamanio_del_envio, 0);
 	if(bytes_enviados <= 0){
 		log_info(logger, "Quise enviar a %d", socket_destino);
 		_exit_with_error(socket_destino, "No se pudo enviar el mensaje", NULL, logger);
 	}
 	return bytes_enviados;
 }

int recibir(int socket_receptor, void* buffer_receptor, int tamanio_que_recibo, t_log* logger){

	int bytes_recibidos = recv(socket_receptor, buffer_receptor, tamanio_que_recibo, MSG_WAITALL);
	/*if (bytes_recibidos <= 0) {
			_exit_with_error(socket_receptor, "Error recibiendo el contenido", NULL, logger);
		}*/

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

void serializar_int(void* envio, int id, int protocolo){
	memcpy(envio, &protocolo, sizeof(int));
	memcpy(envio + sizeof(int), &id, sizeof(int));

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
	int tamanio_clave, tamanio_valor;
	int tamanio_mensaje = tamanio_buffer_instruccion(la_instruccion) - 2*sizeof(int);
	int id_protocolo = 82;

	memcpy(buffer, &id_protocolo, sizeof(int));
	memcpy(buffer + sizeof(int), &tamanio_mensaje, sizeof(int));
	memcpy(buffer + (sizeof(int)*2), &(la_instruccion.valido), sizeof(bool));
	memcpy(buffer + (sizeof(int)*2 + sizeof(bool)), &(la_instruccion.keyword), sizeof(int));

	switch(la_instruccion.keyword){
	case GET:
		tamanio_clave = strlen(la_instruccion.argumentos.GET.clave)+1;
		memcpy(buffer + sizeof(int)*3 + sizeof(bool), &tamanio_clave, sizeof(int));
		memcpy(buffer + sizeof(int)*4 + sizeof(bool), la_instruccion.argumentos.GET.clave, tamanio_clave);
		break;

	case SET:
		tamanio_clave = strlen(la_instruccion.argumentos.SET.clave)+1;
		tamanio_valor = strlen(la_instruccion.argumentos.SET.valor)+1;
		memcpy(buffer + sizeof(int)*3 + sizeof(bool), &tamanio_clave, sizeof(int));
		memcpy(buffer + sizeof(int)*4 + sizeof(bool), la_instruccion.argumentos.SET.clave, tamanio_clave);
		memcpy(buffer + sizeof(int)*4 + sizeof(bool) + tamanio_clave, &tamanio_valor, sizeof(int));
		memcpy(buffer + sizeof(int)*5 + sizeof(bool) + tamanio_clave, la_instruccion.argumentos.SET.valor, tamanio_valor);
		break;

	case STORE:
		tamanio_clave = strlen(la_instruccion.argumentos.STORE.clave)+1;
		memcpy(buffer + sizeof(int)*3 + sizeof(bool), &tamanio_clave, sizeof(int));
		memcpy(buffer + sizeof(int)*4 + sizeof(bool), la_instruccion.argumentos.STORE.clave, tamanio_clave);
	}

}

t_esi_operacion deserializar_instruccion(void* buffer) {
	t_esi_operacion instruccion;
	int tamanio_clave, tamanio_valor;
	memcpy(&(instruccion.valido), buffer, sizeof(bool));
	memcpy(&(instruccion.keyword), (buffer + sizeof(bool)), sizeof(int));
	memcpy(&tamanio_clave, (buffer + sizeof(bool) + sizeof(int)), sizeof(int));
	switch (instruccion.keyword) {
	case (GET):
		instruccion.argumentos.GET.clave = malloc(tamanio_clave);
		memcpy(instruccion.argumentos.GET.clave, (buffer + sizeof(bool) + sizeof(int)*2),tamanio_clave);
		break;
	case (SET):
		instruccion.argumentos.SET.clave = malloc(tamanio_clave);
		memcpy((instruccion.argumentos.SET.clave), (buffer + sizeof(bool) + sizeof(int)*2), tamanio_clave);
		memcpy(&tamanio_valor, (buffer + sizeof(bool) + sizeof(int)*2 + tamanio_clave), sizeof(int));
		instruccion.argumentos.SET.valor = malloc(tamanio_valor);
		memcpy((instruccion.argumentos.SET.valor), (buffer + sizeof(bool) + sizeof(int)*3 + tamanio_clave), tamanio_valor);
		break;
	case (STORE):
		instruccion.argumentos.STORE.clave = malloc(tamanio_clave);
		memcpy((instruccion.argumentos.STORE.clave),(buffer + sizeof(bool) + sizeof(int)*2), tamanio_clave);
		break;
	}
	return instruccion;
}

int tamanio_buffer_instruccion(t_esi_operacion instruccion){
	int tamanio_base = sizeof(instruccion.valido) + sizeof(instruccion.keyword) + 3*sizeof(int);
	switch(instruccion.keyword){
		case GET:
			return tamanio_base + strlen(instruccion.argumentos.GET.clave)+1;
			break;
		case SET:
			return tamanio_base + strlen(instruccion.argumentos.SET.clave) + 1 + strlen(instruccion.argumentos.SET.valor) + 1 + sizeof(int);
			break;
		case STORE:
			return tamanio_base + strlen(instruccion.argumentos.STORE.clave) +1;
			break;
		default:
			return -1;
			break;
	}
}

void serializar_configuracion_inicial_instancia(void* buffer, datos_configuracion configuracion){
	/*int* id_protocolo = malloc(sizeof(int));
	*id_protocolo = 0;
	datos_configuracion *info_configuracion = malloc(sizeof(datos_configuracion));
	*info_configuracion = configuracion;
	memcpy(buffer, id_protocolo, sizeof(int));
	memcpy((buffer + (sizeof(int))), info_configuracion, sizeof(t_handshake));
	free(info_configuracion);
	free(id_protocolo);*/

	int id_protocolo = 0;
	memcpy(buffer, &id_protocolo, sizeof(int));
	memcpy(buffer + sizeof(int), &configuracion.cantidad_entradas, sizeof(int));
	memcpy(buffer + sizeof(int)*2, &configuracion.tamano_entrada, sizeof(int));
}

datos_configuracion deserializar_configuracion_inicial_instancia(void* buffer) {
	datos_configuracion configuracion;
	memcpy(&(configuracion.cantidad_entradas), buffer, sizeof(int));
	memcpy(&(configuracion.tamano_entrada), (buffer + (sizeof(int))), sizeof(int));
	return configuracion;
}

void serializar_status_clave(void* buffer, status_clave status){
	int id_protocolo = 83;
	int tamanio_clave = strlen(status.clave)+1;
	int tamanio_contenido = (strlen(status.contenido)+1)*sizeof(char);
	int tamanio_total = tamanio_buffer_status(status)- sizeof(int)*2;
	memcpy(buffer, &id_protocolo, sizeof(int));
	memcpy(buffer + sizeof(int), &tamanio_total, sizeof(int));
	memcpy(buffer + sizeof(int)*2, &tamanio_clave, sizeof(int));
	memcpy(buffer + sizeof(int)*3, status.clave, tamanio_clave);
	memcpy(buffer + sizeof(int)*3 + tamanio_clave, &status.id_instancia_actual, sizeof(int));
	memcpy(buffer + sizeof(int)*4 + tamanio_clave, &status.id_instancia_nueva, sizeof(int));
	memcpy(buffer + sizeof(int)*5 + tamanio_clave, &tamanio_contenido, sizeof(int));
	memcpy(buffer + sizeof(int)*6 + tamanio_clave, status.contenido, tamanio_contenido);
}

status_clave deserializar_status_clave(void* buffer) {//Se hicieron dos recibir antes, uno para el protocolo y otro para el tamaño del buffer
	status_clave status;
	int tamanio_clave, tamanio_contenido;
	memcpy(&tamanio_clave, buffer, sizeof(int));
	status.clave = malloc(tamanio_clave);
	memcpy(status.clave, buffer + sizeof(int), tamanio_clave);
	memcpy(&status.id_instancia_actual, buffer + sizeof(int) + tamanio_clave, sizeof(int));
	memcpy(&status.id_instancia_nueva, buffer + sizeof(int)*2 + tamanio_clave, sizeof(int));
	memcpy(&tamanio_contenido, buffer + sizeof(int)*3 + tamanio_clave, sizeof(int));
	status.contenido = malloc(tamanio_clave);
	memcpy(status.contenido, buffer + sizeof(int)*4 + tamanio_clave, tamanio_contenido);
	return status;
}

int tamanio_buffer_status(status_clave status){
	return sizeof(int)*6 + (strlen(status.clave) + strlen(status.contenido) + 2) * sizeof(char);
}

void serializar_string(void* buffer, char* cadena, int protocolo){//Se guarda dos veces el tamaño para recibirlo afuera de deserializar y poder declarar el buffer y para poder recibirlo dentro de deserializacion y saber de que tamaño hacer el memcpy
	int tamanio = (strlen(cadena)+1) * sizeof(char) + sizeof(int); 				// recibe el protocolo porque la mismma funcion se puede usar muchas veces
	int tamanio2 = (strlen(cadena)+1) * sizeof(char);
	memcpy(buffer, &protocolo, sizeof(int));
	memcpy(buffer + sizeof(int), &tamanio, sizeof(int)*2);
	memcpy(buffer + sizeof(int)*2, &tamanio2, sizeof(int));
	memcpy(buffer + sizeof(int)*3, cadena, tamanio);
}

void deserializar_string(void* buffer, char* mensaje){
	int tamanio;
	memcpy(&tamanio, buffer, sizeof(int));
	memcpy(mensaje, buffer + sizeof(int), tamanio);
}

int tamanio_buffer_string(char* cadena){
	return strlen(cadena) * sizeof(char) + sizeof(int)*3 + sizeof(char);
}


