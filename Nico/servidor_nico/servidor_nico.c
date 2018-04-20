#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <commons/log.h>


int main(void){
	t_log * logger;

	logger = log_create("servidor_nico.log", "servidor_nico", true, LOG_LEVEL_INFO);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8081);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("Fallo el bind");
		return 1;

	}

	printf("estoy escuchando\n");
	listen(servidor, 100);

//--------------------------------------

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion;
	int cliente = accept(servidor, (void*) &direccionCliente,&tamanoDireccion);

	printf("recibi una conexion en %d\n", cliente);

	send(cliente, "Hola cliente", 13, 0);
	send(cliente, ":D", 3, 0);

	char* buffer = malloc(5);

	int datosRecibidos = recv(cliente, buffer, 4, MSG_WAITALL);

	buffer[datosRecibidos] = '\0';
	printf("Me llegaron %d bytes, %s \n", datosRecibidos, buffer);

	free(buffer);

	return 0;




}
