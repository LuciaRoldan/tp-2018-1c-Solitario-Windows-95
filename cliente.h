#ifndef CLIENTE_H_
#define CLIENTE_H_

/*
    Este es el archivo de header de nuestro programa.
    Sirve para definir cosas que pueden sernos utiles en varios lados,
    o simplemente para poder delegar #includes de bibliotecas, estructuras
    o los prototipos de las funciones que querramos hacer publicas.
    Las guardas #ifndef, #define y #endif sirven para darle informacion al
    preprocesador de c sobre algunas cosas. Estas son importantes en
    el header, ya que nos protegen de problemas si hay doble inclusion
    del mismo archivo de header 2 veces.
*/

// Aca incluimos las bibliotecas que vamos a usar
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

// Definimos algunas constantes para nuestro código
#define IP "127.0.0.1"
#define PUERTO "8080"

// Definimos algunas variables globales
t_log * logger;

// A continuacion estan las estructuras con las que nos vamos a manejar.
typedef struct  {
  int id_mensaje;
  int legajo;
  char nombre[40];
  char apellido [40];
} __attribute__((packed)) Alumno;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

// Finalmente, los prototipos de las funciones que vamos a implementar
void configure_logger();
int  connect_to_server(char * ip, char * port);
void wait_hello(int socket);
Alumno read_hello();
void send_hello(int socket, Alumno alumno);
void * wait_content(int socket);
void send_md5(int socket, void * content);
void wait_confirmation(int socket);
void exit_gracefully(int return_nr);
void _exit_with_error(int socket, char* error_msg, void * buffer);
#endif /* CLIENTE_H_ */
