#include "commons_propias.h"

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


int inicializar_servidor(char* ip, char* puerto){


	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET; //que se fije solon si es IPv4 o IPv6
	direccionServidor.sin_addr.s_addr = *ip;
	direccionServidor.sin_port = *puerto;

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
		setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
			_exit_with_error(servidor, "Fallo el bind", NULL);

		}

		printf("estoy escuchando\n");
		listen(servidor, 100);

	//--------------------------------------

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion;
	int cliente = accept(servidor, (void*) &direccionCliente,&tamanoDireccion);
	printf("recibi una conexion en %d\n", cliente);

	return cliente;
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

	Mensaje mensaje = {strcpy(mensaje.id_mensaje, "") , strcpy(mensaje.instruccion,  "")};	//creo estructura del mensaje



	char *id_mensaje = malloc(41);
	printf("Id Mensaje: \n");
	scanf("%s",id_mensaje);



	memcpy(mensaje.id_mensaje, id_mensaje, strlen(id_mensaje));
	free(id_mensaje);



	char *instruccion = malloc(41);
	printf("instruccion: \n");
	scanf("%s",instruccion);




	memcpy(mensaje.instruccion, instruccion, strlen(instruccion));
	free(instruccion);

return mensaje;
}


void send_mensaje(int socket, Mensaje mensaje) {

	log_info(logger, "Enviando mensaje");


	mensaje.id_mensaje = 99;

int resultado = send(socket, &mensaje, sizeof(Mensaje) , 0);
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


void send_content(int socket, void * content) {


int longitud = sizeof(&content);
//printf("Longitud del mensaje");
//scanf(%d,longitud);

  ContentHeader header = { .id = 33, .len = longitud };

  int result_header = send(socket, &header,sizeof(ContentHeader),0);


  int message_size = sizeof(Mensaje) + longitud;
  void * buf = malloc(message_size);

  memcpy(buf, &content, sizeof(message_size));




  free(content);



  log_info(logger, "Enviando contenido");
  int result_send =  send(socket, buf, message_size, 0);
  free(buf);

  if (result_send <= 0) {
    _exit_with_error(socket, "No se pudo enviar el contenido", NULL);
  }
}

void wait_confirmation(int socket) {
  int result = 1; // Dejemos creado un resultado por defecto
  /*
    19.   Ahora nos toca recibir la confirmacion del servidor.
          Si el resultado obvenido es distinto de 0, entonces hubo un error
  */
  log_info(logger, "Esperando confirmacion");
  if (recv(socket, &result, sizeof(int), 0) <= 0) {
    _exit_with_error(socket, "No se pudo recibir confirmacion", NULL);
  }

  if (result != 1) {
    _exit_with_error(socket, "Se recibio la confimación", NULL);
  }

  log_info(logger, "Comunicacion exitosa");
  close(socket);
}


void _exit_with_error(int socket, char* error_msg, void * buffer) {
  if (buffer != NULL) {
    free(buffer);
  }
  log_error(logger, error_msg);
  close(socket);
  exit_gracefully(1);
}


