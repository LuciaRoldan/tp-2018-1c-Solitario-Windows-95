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
  char instruccion [10];
} __attribute__((packed)) Mensaje;
//no se deberian usar punteros en las structs cuando van a ser llamadas por sockets
// no nos va a alcanzar con un solo tipo de mensaje, tenemos que hacer varios dependiendo de quien lo reciba



typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;


//------------------funciones

//funcoines de salida
void _exit_with_error(int socket, char* error_msg, void * buffer);
void exit_gracefully(int return_nr);

//funciones de conexion

int inicializar_servidor(char* ip, char* puerto);//inicializa un servidor y espera conexiones
int connect_to_server(char * ip, char * port);//devuelve el socket con el que se establece la conexion a un servidor

//funciones de recv() y send()
void send_string(int socket, char* mensaje);
void wait_string(int socket, int len);


//funciones de recv() y send() variables para despues de serializacion
void send_mensaje(int socket, Mensaje mensaje);//envia un struct tipo Mensaje
void wait_mensaje(int socket);
void * wait_content(int socket);//espera contenido de tamaño variable
void send_content(int socket, void * content);//envia contenido de tamaño variable


void prueba_magica(int n);



#endif /* COMMONS_PROPIAS_H_ */
