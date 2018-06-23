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

int enviar(int* socket_destino, void* envio, int tamanio_del_envio, int id, t_log* logger){
	//void* buffer = malloc(sizeof(int) + tamanio_del_envio);

	//memcpy(buffer, &id, sizeof(int));
	//memcpy((buffer + (sizeof(int))), envio, tamanio_del_envio);

	int bytes_enviados = send(*socket_destino, envio, tamanio_del_envio, 0);

 	if(bytes_enviados <= 0){
 		_exit_with_error(*socket_destino, "No se pudo enviar el mensaje", NULL, logger);
 	}
	free(envio);
 	return bytes_enviados;
 }

int recibir(int* socket_receptor, void* buffer_receptor, int tamanio_que_recibo, t_log* logger){

	int bytes_recibidos = recv(*socket_receptor, buffer_receptor, tamanio_que_recibo, MSG_WAITALL);
	if (bytes_recibidos <= 0) {
			_exit_with_error(*socket_receptor, "Error recibiendo el contenido", NULL, logger);
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

int aceptar_conexion(int* socket_escucha){
	struct sockaddr_in cliente;
	socklen_t tamanio = sizeof(cliente);
	int socket_cliente = accept(*socket_escucha, (struct sockaddr *) &cliente, &tamanio);
	return socket_cliente;
}


//revisar bien el mensaje que recibe

void serializar_handshake(void* buffer, t_handshake handshake){

	memcpy(buffer, &handshake.proceso, sizeof(int));
	memcpy((buffer + (sizeof(int))) , &handshake.id_proceso, sizeof(int));
}

void deserializar_handshake(void *buffer_recepcion,t_handshake handshake_recibido){
	memcpy(&(handshake_recibido.id_proceso),buffer_recepcion, sizeof(int));
	memcpy( &(handshake_recibido.proceso),(buffer_recepcion + (sizeof(int))), sizeof(int));


}

t_header deserializarHeader(void* bufferHeader){
	t_header header;

	memcpy(&(header.id_mensaje),bufferHeader, sizeof(int));
	memcpy( &(header.largo_mensaje),(bufferHeader + (sizeof(int))), sizeof(int));

	return header;
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


