/*
 * conexiones.h
 *
 *  Created on: 23 abr. 2018
 *      Author: utnso
 */



#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/collections/list.h>


t_log * logger;

//---------------------structs

typedef struct  {
  int id_mensaje;
  char primerMensaje [40];
  char segundoMensaje [40];
} __attribute__((packed)) Protocolo;


typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;


//------------------funciones

void _exit_with_error(int socket, char* error_msg, void * buffer);
int connect_to_server(char * ip, char * port);
void wait_handshake(int socket);
Protocolo read_mensaje();
void send_mensaje(int socket, Protocolo mensaje);
void * wait_content(int socket);


#endif /* CONEXIONES_H_ */
