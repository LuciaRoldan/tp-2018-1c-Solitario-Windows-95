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
#include <commons/config.h>
#include <unistd.h>
#include <pthread.h>



///////////////////////// INSTRUCCIONES /////////////////////////

typedef enum{ //posibles instrucciones
	GET,
	SET,
	STORE
} InstruccionAtomica;

typedef char Clave [30]; //keys

typedef enum { //posibles estados del ESI
	NUEVO,
	LISTO,
	BLOQUEANDO,
	EJECUTANDO
} estado;

//////////////////////////// ALGORITMOS REEMPLAZO ////////////////////////////

typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo;


//////////////////////////// MENSAJES ////////////////////////////

//ESI-COORDINADOR
typedef struct{ //mensaje que manda el ESI al Coordinador
	InstruccionAtomica instruccion;
	Clave clave;
} MenasajeEsi;

//ESI-PLANIFICADOR
typedef struct {
	int id;
	estado estado;
	int retardo; //tiempo en ser atendido el esi la ultima vez que ejecuto
	int ultimaRafaga; //con el retardo y la ultimaRafaga se calcula la estimacion ?
	int ultimaEstimacion;
} pcb;

//PLANIFICADOR-ESI
typedef enum{
	EJECUTAR,
	BLOQUEARSE,
	DESBLOQUEARSE
} AccionESI;

typedef struct{ //mensaje que manda Planificador a ESI
	AccionESI instruccion;
} InstruccionESI;

//INSTANCIA-COORDINADOR
typedef struct{
	char ipCoordinador[];
	int puertoCoordinador;
	tipo_algoritmo algoritmo_reemplazo;
	int cantidad_entradas;
	int tamano_entrada;
	int espacion_para_memoria;
} datos_configuracion;

//TRES TIPOS DE MENSAJES
typedef struct {
	char instruccion[];
	uint32_t intruccion_long;
	int clave;
	uint32_t clave_long;
}__attribute__((packed)) Mensaje_tipo1;

typedef struct {
	char instruccion[];
	uint32_t instruccon_long;
	int clave;
	uint32_t clave_long;
	int value;
	uint32_t value_long;
}__attribute__((packed)) Mensaje_tipo2;

typedef struct {
	char instruccion[];
}__attribute__((packed)) Mensaje_tipo3;

/////////////////////////// CONEXIONES ///////////////////////////

//PARA EL PLANIFICADOR Y EL COORDINADOR
typedef struct {
	char* ip [10];
	char* puerto [5];
}t_conexion;


typedef struct  {
  int id_mensaje;
  char instruccion [10];
} __attribute__((packed)) Mensaje;
//no se deberian usar punteros en las structs cuando van a ser llamadas por sockets


typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

//////////////////////////// FUNCIONES ////////////////////////////

//funcoines de salida
void _exit_with_error(int socket, char* error_msg, void * buffer, t_log logger);
void exit_gracefully(int return_nr, t_log logger);

//funciones de conexion
int inicializar_servidor(char* ip, char* puerto, t_log* logger);//inicializa un servidor y espera conexiones
int connect_to_server(char* ip, char * port, t_log* logger);//devuelve el socket con el que se establece la conexion a un servidor

//funciones de recv() y send()
void send_string(int socket, char* mensaje);
void wait_string(int socket, int len);

//funciones de recv() y send() variables para despues de serializacion
void send_mensaje(int socket, Mensaje mensaje);//envia un struct tipo Mensaje
void wait_mensaje(int socket);
int wait_content(int socket, *buffer, t_log logger);//espera contenido de tamaño variable
void send_content(int socket, void * content, int id, t_log logger);//envia contenido de tamaño variable


#endif /* COMMONS_PROPIAS_H_ */
