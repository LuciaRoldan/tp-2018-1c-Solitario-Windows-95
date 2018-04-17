#include "cliente.h"

void configure_logger();
int  connect_to_server(char * ip, char * port);
void wait_handshake(int socket);

void exit_gracefully(int return_nr);
void _exit_with_error(int socket, char* error_msg, void * buffer);


int main(){
configure_logger();
int socket = connect_to_server(IP,PUERTO);
wait_handshake(socket);
send_handshake(socket, mensaje);
void * content = wait_content(socket);
send_md //??

}

void configure_logger() {

	logger = log_create("cliente.log"), "cliente", true, LOG_LEVEL_INFO);

}


int connect_to_server(char * ip, char * port) {

struct addrinfo hints;
struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC //verifica solo si es IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM ; //protocolo TCP


	getaddrinfo(ip, port, &hints, &server_info); //guarda en server_info datos de conexion

	int server_socket = socket(server_info->ai_family, server_info->ai_addr, server_info->ai_addrlen); // creo socket "server_socket"

	int res = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen); //me conecto al servidor

	freeaddrinfo(server_info); //libero server_info

	if (res < 0) {

		_exit_with_error(server_socket, "No me pude conectar al servidor", NULL);

	}

	log_info(logger, "Conectado!"); //exito
	return server_socket;
}


void wait_handshake(int socket){
	char * handshake = "prueba 1 2 3"; //handshake

	char * buffer = (char*) calloc(sizeof(char), strlen(handshake) + 1); //buffer del tamaño del string + /0

	int result_recv = recv(socket, buffer, strlen(handshake), MSG_WAITALL); //recepción del mensaje , conexion | donde guardar | cant de bytes | flags


	if(result_recv <= 0) {

		_exit_with_error(socket, "No se puedo hacer un handshake", buffer);
	}

	if (strcmp(buffer, handshake) != 0) {

	_exit_with_error(socket, "No se pudo recibir handshake", buffer);

	}

	log_info(logger, "Handshake recibido: '%s'". buffer); //exito
	free(buffer); //libero memoria del calloc del buffer

}
