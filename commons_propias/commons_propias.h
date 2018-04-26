/*
 * commons_propias.h
 *
 *  Created on: 24 abr. 2018
 *      Author: utnso
 */

#ifndef COMMONS_PROPIAS_H_
#define COMMONS_PROPIAS_H_

#include <stdio.h>
#include <readline/rlstdc.h>
#include <readline/rltypedefs.h>
#include <readline/keymaps.h>
#include <readline/tilde.h>
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <readline/history.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <unistd.h>


t_log * logger;

//---------------------structs

typedef struct  {
  int id_mensaje;
  char* instruccion [10];
} __attribute__((packed)) Mensaje;


typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;


//------------------funciones

void _exit_with_error(int socket, char* error_msg, void * buffer);
int connect_to_server(char * ip, char * port);

void wait_handshake(int socket);
Mensaje read_mensaje();
void send_mensaje(int socket, Mensaje mensaje);
void * wait_content(int socket);
void send_content(int socket, void * content);
int inicializar_servidor(char* ip, char* puerto);





#endif /* COMMONS_PROPIAS_H_ */