#include <stdio.h> // Por dependencia de readline
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/collections/list.h>

//mando todas las commons por las dudas
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <Commons_propias/commons_propias.h>

// Definimos algunas variables globales
t_log * logger;

// A continuacion estan las estructuras con las que nos vamos a manejar.
/*typedef struct  {
  int id_mensaje;
  int legajo;
  char nombre[40];
  char apellido [40];
} __attribute__((packed)) Alumno;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;
*/

// Finalmente, los prototipos de las funciones que vamos a implementar
void configure_logger();
int  connect_to_server(char * ip, char * port);


void * wait_content(int socket);
void send_md5(int socket, void * content);
void wait_confirmation(int socket);
void exit_gracefully(int return_nr);
void _exit_with_error(int socket, char* error_msg, void* buffer);

