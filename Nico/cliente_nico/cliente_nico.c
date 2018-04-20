#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <string.h>



int main(void){
//	t_log * logger;

	//logger = log_create("servidor_nico.log", "servidor_nico", true, LOG_LEVEL_INFO);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(8081);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);

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
