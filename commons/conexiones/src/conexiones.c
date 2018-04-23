#include "conexiones.h"

void configure_logger() {

	logger = log_create("cliente.log", "LOG", true, LOG_LEVEL_INFO); //creacion de log
}



int connect_to_server(char * ip, char * port){
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
		_exit_with_error(server_socket, "No me pude conectar al servidor", NULL);
	}

		log_info(logger, "Conectado!"); //logeo exito
		return server_socket;
}

void wait_handshake(int socket){
	char *handshake = "Handshake"; //el servidor deberia mandar lo mismo que dice el protocolo


	char *buffer = (char*) calloc(sizeof(char), strlen(handshake) + 1);

	int result_recv = recv(socket, buffer, strlen(handshake), MSG_WAITALL); //recibo el handshake del servidor

	if(result_recv <= 0) {
		_exit_with_error(socket, "No se pudo recibir el handshke", buffer);


	}

	if(strcmp(buffer, handshake) != 0) {
		_exit_with_error(socket, "No llego el handshake esperado", buffer);
	}

	log_info(logger, "Handshake recibido: '%s'", buffer);
	free(buffer);
	}


Mensaje read_mensaje(){

	Mensaje mensaje = {.primerMensaje = "", .segundoMensaje = ""};	//creo estructura del mensaje



	char *primerMensaje = malloc(41);
	printf("Primer mensaje: \n");
	scanf("%s",primerMensaje);



	memcpy(mensaje.primerMensaje, primerMensaje, strlen(primerMensaje));
	free(primerMensaje);



	char *segundoMensaje = malloc(41);
	printf("Segundo mensaje: \n");
	scanf("%s",segundoMensaje);




	memcpy(mensaje.segundoMensaje, segundoMensaje, strlen(segundoMensaje));
	free(segundoMensaje);

return mensaje;
}

void send_mensaje(int socket, Protocolo mensaje) {

	log_info(logger, "Enviando mensaje");


	mensaje.id_mensaje = 99;

int resultado = send(socket, &mensaje, sizeof(Protocolo) , 0);
if(resultado <= 0) {

	_exit_with_error(socket, "No se pudo enviar el mensaje", NULL);
}

}


void *wait_content(int socket) {

log_info(logger, "Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));
  // 13.1. Reservamos el suficiente espacio para guardar un ContentHeader
  ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));

  // 13.2. Recibamos el header en la estructura y chequiemos si el id es el correcto.
  //      No se olviden de validar los errores!

  if (recv(socket, header, sizeof(ContentHeader), 0) <= 0) {
    _exit_with_error(socket, "No se pudo recibir el encabezado del contenido", header);
  }

  if (header->id != 18) {
    _exit_with_error(socket, "Id incorrecto, deberia ser 18", header);
  }

  log_info(logger, "Esperando el contenido (%d bytes)", header->len);
/*

      14.   Ahora, recibamos el contenido variable. Ya tenemos el tamaño,
            por lo que reecibirlo es lo mismo que veniamos haciendo:
      14.1. Reservamos memoria
      14.2. Recibimos el contenido en un buffer (si hubo error, fallamos, liberamos y salimos

*/

  void * buf = calloc(sizeof(char), header->len + 1);
  if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
    free(buf);
    _exit_with_error(socket, "Error recibiendo el contenido", header);
  }

  /*

      15.   Finalmente, no te olvides de liberar la memoria que pedimos
            para el header y retornar el contenido recibido.
*/

  log_info(logger, "Contenido recibido '%s'", (char*) buf);
  free(header);
  return buf;
}



void _exit_with_error(int socket, char* error_msg, void * buffer) {
  if(buffer != NULL){
    free(buffer);
  }
}
