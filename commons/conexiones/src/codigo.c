
/*




#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <commons/log.h>
#include <string.h>



int main(void){
//	t_log * logger;



	configure_logger();
	int socket = connect_to_server("127.0.0.1", "8081");
	wait_handshake(socket);
	Protocolo handshake = read_handshake();
	send_mensaje(socket, mensaje);
	void * content = wait_content(socket);
	send_md5(socket, content);
	wait_confirmation(socket);
	exit_gracefully(0);
}
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(8081);


void configure_logger() {

	logger = log_create("cliente.log", "LOG", true, LOG_LEVEL_INFO); //creacion de log




}

int connect_to_server(char * ip, char * port){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0 sizeof(hints));
	hints.ai_family = AF_UNSPEC; //se fija solo si es IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAN; //protocolo TCP

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




}

void wait_handshake(int socket){
	char * handshake = "Handshake"; //el servidor deberia mandar lo mismo que dice el protocolo


	char * buffer = (char*) calloc(sizeof(char), strlen(handshake) + 1);

	int result_recv = recv(socket, buffer, strlen(hola), MSG_WAITALL); //recibo el handshake del servidor

	if(result_recv <= 0) {
		_exit_with_error(socket, "No se pudo recibir el handshke", buffer);


	}

	if(strcmp(buffer, handshake) != 0 {
		_exit_with_error(socket, "No llego el handshake esperado");
	}

	log_info(logger, "Handshake recibido: '%s'", buffer);
	free(buffer);
	}


	Protocolo read_mensaje(){

	Protocolo mensaje = {.primerMensaje = "", .segundoMensaje = ""};	//creo estructura del mensaje

	char * primerMensaje = readline("primerMensaje: "); //readline reserva memoria

	memcpy(mensaje.primerMensaje, primerMensaje, strlen(primerMensaje));
	free(primerMensaje);




	char * segundoMensaje = readline("segundoMensaje: ");

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

void * wait_content(int socket) {

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


      14.   Ahora, recibamos el contenido variable. Ya tenemos el tamaño,
            por lo que reecibirlo es lo mismo que veniamos haciendo:
      14.1. Reservamos memoria
      14.2. Recibimos el contenido en un buffer (si hubo error, fallamos, liberamos y salimos

  void * buf = calloc(sizeof(char), header->len + 1);
  if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
    free(buf);
    _exit_with_error(socket, "Error recibiendo el contenido", header);
  }


      15.   Finalmente, no te olvides de liberar la memoria que pedimos
            para el header y retornar el contenido recibido.


  log_info(logger, "Contenido recibido '%s'", (char*) buf);
  free(header);
  return buf;
}

void send_md5(int socket, void * content) {

    16.   Ahora calculemos el MD5 del contenido, para eso vamos
          a armar el digest:


  void * digest = malloc(MD5_DIGEST_LENGTH);
  MD5_CTX context;
  MD5_Init(&context);
  MD5_Update(&context, content, strlen(content) + 1);
  MD5_Final(digest, &context);

  free(content);


    17.   Luego, nos toca enviar a nosotros un contenido variable.
          A diferencia de recibirlo, para mandarlo es mejor enviarlo todo de una,
          siguiendo la logida de 1 send - N recv.
          Asi que:


  //      17.1. Creamos un ContentHeader para guardar un mensaje de id 33 y el tamaño del md5

  ContentHeader header = { .id = 33, .len = MD5_DIGEST_LENGTH };


          17.2. Creamos un buffer del tamaño del mensaje completo y copiamos el header y la info de "digest" allí.
          Recuerden revisar la función memcpy(ptr_destino, ptr_origen, tamaño)!


  int message_size = sizeof(ContentHeader) + MD5_DIGEST_LENGTH;
  void * buf = malloc(message_size);

  memcpy(buf, &header, sizeof(ContentHeader));
  memcpy(buf + sizeof(ContentHeader), digest, MD5_DIGEST_LENGTH);

  free(digest);

    18.   Con todo listo, solo nos falta enviar el paquete que armamos y liberar la memoria que usamos.
          Si, TODA la que usamos, eso incluye a la del contenido del mensaje que recibimos en la función
          anterior y el digest del MD5. Obviamente, validando tambien los errores.


  log_info(logger, "Enviando MD5");
  int result_send =  send(socket, buf, message_size, 0);
  free(buf);

  if (result_send <= 0) {
    _exit_with_error(socket, "No se pudo enviar el md5", NULL);
  }
}

void wait_confirmation(int socket) {
  int result = 1; // Dejemos creado un resultado por defecto

    19.   Ahora nos toca recibir la confirmacion del servidor.
          Si el resultado obvenido es distinto de 0, entonces hubo un error

  log_info(logger, "Esperando confirmacion");
  if (recv(socket, &result, sizeof(int), 0) <= 0) {
    _exit_with_error(socket, "No se pudo recibir confirmacion", NULL);
  }

  if (result != 1) {
    _exit_with_error(socket, "El md5 no coincidio", NULL);
  }

  log_info(logger, "Los MD5 concidieron!");
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


void exit_gracefully(int return_nr) {

    20.   Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion

  log_destroy(logger);
  exit(return_nr);
}



}

//	int cliente = socket(AF_INET, SOCK_STREAM, 0);




	//int activado = 1;
	//setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("No se pudo conectar");
		return 1;}
	while(1){
	char mensaje[1000];
	scanf("%s", mensaje);

	send(cliente, mensaje, strlen(mensaje), 0);
	}

return 0;
}



#endif  CONEXIONES_H_ */


