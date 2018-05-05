#include "commons_propias.h"

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




int connect_to_server(char * ip, char * port, t_log * logger){
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


int inicializar_servidor(char* ip, char* puerto, t_log * logger){


	//struct sockaddr_in direccionServidor;
	     // fill in my IP for me

	struct addrinfo hints, *res;



	memset(&hints,0,sizeof hints);


	hints.ai_family = AF_UNSPEC; //que se fije solon si es IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;


	getaddrinfo(ip, puerto, &hints, &res);
	int servidor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);


	int activado = 1;
		setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

		if(bind(servidor, (void*) &hints, sizeof(hints)) != 0){
			_exit_with_error(servidor, "Fallo el bind", NULL);

		}

		printf("estoy escuchando\n");
		log_info(logger, "Escuchando!");
		listen(servidor, 100);

	//--------------------------------------

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion;
	int cliente = accept(servidor, (void*) &direccionCliente,&tamanoDireccion);
	printf("recibi una conexion en %d\n", cliente);



		return cliente;

}



/*
int send_string(int socket, char* mensaje){
int len, bytes_sent;

len = strlen(mensaje);


bytes_sent= send(socket,mensaje,len,0);


return bytes_sent;

}
*/
int recv_string(int socket){

int bytes_recv;
char* mensaje_recibido [10];

bytes_recv= recv(socket,mensaje_recibido, 11,0);
printf("%s",&mensaje_recibido);
return bytes_recv;
}






void send_mensaje(int socket, Mensaje mensaje) {

	log_info(logger, "Enviando mensaje");


	mensaje.id_mensaje = 99;

int resultado = send(socket, &mensaje, sizeof(Mensaje) , 0);
if(resultado <= 0) {

	_exit_with_error(socket, "No se pudo enviar el mensaje", NULL);
}

}
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

void *wait_content(int socket) {

log_info(logger, "Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));

  ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));

  if (recv(socket, header, sizeof(ContentHeader), 0) <= 0) {
    _exit_with_error(socket, "No se pudo recibir el encabezado del contenido", header);
  }

  if (header->id != 18) {
    _exit_with_error(socket, "Id incorrecto, deberia ser 18", header);
  }

  log_info(logger, "Esperando el contenido (%d bytes)", header->len);


  void * buf = calloc(sizeof(char), header->len + 1);
  if (recv(socket, buf, header->len, MSG_WAITALL) <= 0) {
    free(buf);
    _exit_with_error(socket, "Error recibiendo el contenido", header);
  }


  log_info(logger, "Contenido recibido '%s'", (char*) buf);
  free(header);
  return buf;
}


void send_content(int socket, void * content, int id) {


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
    _exit_with_error(socket, "No se pudo enviar el contenido", NULL);
  }
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

  log_destroy(logger);
  exit(return_nr);

}
