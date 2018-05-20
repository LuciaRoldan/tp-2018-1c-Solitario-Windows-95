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

		log_info(logger, "Conectado!"); //logeo exito
		return server_socket;
}

int inicializar_servidor(char* ip, char* puerto, t_log * logger){
	struct addrinfo hints, *res;

	memset(&hints,0,sizeof hints);

	hints.ai_family = AF_UNSPEC; //que se fije solon si es IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;

	getaddrinfo(ip, puerto, &hints, &res);
	int servidor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (servidor == -1){
        log_info(logger, "No se pudo crear el socket");
    }
    log_info(logger, "Socket creado");


	int activado = 1;
		setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

		if(bind(servidor, (void*) &hints, sizeof(hints)) != 0){
			_exit_with_error(servidor, "Fallo el bind", NULL, logger);

		}

		printf("estoy escuchando\n");
		log_info(logger, "Escuchando!");
		listen(servidor, 100);

return servidor;

		}

//Funciones para enviar y recibir cosas serializadas

int enviar(int* socket_destino, void* envio,int tamanio_del_envio, t_log* logger){
	int payload_enviado, bytes_enviados;
	bytes_enviados=send(*socket_destino,envio,tamanio_del_envio,0);
	if(bytes_enviados <= 0){
		_exit_with_error(*socket_destino, "No se pudo enviar el mensaje", NULL, logger);
	}
	return payload_enviado;
}

int recibir(int* socket_receptor, void* buffer_receptor,int tamanio_que_recibo, t_log* logger){
	int payload_recibido, bytes_recibidos;
	bytes_recibidos=recv(*socket_receptor,buffer_receptor,tamanio_que_recibo,MSG_WAITALL);
	if (bytes_recibidos <= 0) {
			_exit_with_error(*socket_receptor, "Error recibiendo el contenido", NULL, logger);
		}
	return payload_recibido;
}

int recv_string(int socket, char* mensaje_recibido [10]){
	int bytes_recv;
	bytes_recv= recv(socket,mensaje_recibido, 11,0);
	printf("%s",&mensaje_recibido);
	return bytes_recv;
}

void send_mensaje(int socket, Mensaje mensaje, t_log* logger) {

	log_info(logger, "Enviando mensaje");
	mensaje.id_mensaje = 99;
	int resultado = send(socket, &mensaje, sizeof(Mensaje) , 0);
	if(resultado <= 0) {
		_exit_with_error(socket, "No se pudo enviar el mensaje", NULL, logger);
	}
}

int wait_content(int socket, char *buffer, t_log * logger) {

	log_info(logger, "Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));

	ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));
	int id = strcpy(header->id,id);

	if (recv(socket, header, sizeof(ContentHeader), 0) <= 0) {
		_exit_with_error(socket, "No se pudo recibir el encabezado del contenido", header, logger);
	}
	log_info(logger, "Esperando el contenido (%d bytes)", header->len);

	void * buf = calloc(sizeof(char), header->len + 1);
	if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
		free(buf);
		_exit_with_error(socket, "Error recibiendo el contenido", header, logger);
	}

	log_info(logger, "Contenido recibido '%s'", (char*) buffer);
	return header;
}

void send_content(int socket, void * content, int id, t_log* logger) {

	int longitud = sizeof(&content);
	ContentHeader header = { .id = id, .len = longitud };
	int result_header = send(socket, &header,sizeof(ContentHeader),0);

	int message_size = sizeof(Mensaje) + longitud;
	void * buf = malloc(message_size);

	memcpy(buf, &content, sizeof(message_size));
	free(content);

	log_info(logger, "Enviando contenido");
	int result_send =  send(socket, buf, message_size, 0);
	free(buf);

	if (result_send <= 0) {
		_exit_with_error(socket, "No se pudo enviar el contenido", NULL, logger);
	}
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

int aceptar_conexion(int* socket_escucha, t_log* logger){
	struct sockaddr_in cliente;
	socklen_t tamanio = sizeof(cliente);
	int socket_cliente = accept(*socket_escucha, (struct sockaddr *) &cliente, &tamanio);
	return socket_cliente;
}


//revisar bien el mensaje que recibe




// COMMONS ESI //


/*
typedef enum{
	GET,
	SET,
	STORE
} InstruccionAtomica;

typedef char Clave [30];


//mensaje que manda el ESI al Coordinador

typedef struct{
	InstruccionAtomica instruccion;
	Clave clave;
} MenasajeEsi;

*/

// COMMONS PLANIFICADOR //


// COMMONS CONEXIONES //

/*
int send_string(int socket, char* mensaje){
int len, bytes_sent;

len = strlen(mensaje);


bytes_sent= send(socket,mensaje,len,0);


return bytes_sent;

}
*/

/*
void wait_mensaje(int socket){

	log_info(logger, "Esperando mensaje");

	struct Mensaje buffer = malloc(sizeof(struct Mensaje));

	if (recv(socket, buffer, sizeof(ContentHeader), 0) <= 0) {
	    _exit_with_error(socket, "No se pudo recibir el encabezado del contenido", header);
	  }

	  if (header->id != 18) {
	    _exit_with_error(socket, "Id incorrecto, deberia ser 18", header);
	  }

}
*/

