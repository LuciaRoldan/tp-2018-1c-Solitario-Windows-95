#ifndef COMMONS_PROPIAS_H_
#define COMMONS_PROPIAS_H_

#include <stdio.h>
#include <readline/rlstdc.h>
#include <readline/rltypedefs.h>
#include <readline/keymaps.h>
#include <readline/tilde.h>
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
#include <parsi/parser.h>


///////////////////////// ESTADOS /////////////////////////

typedef enum { //posibles estados del ESI
	NUEVO,
	LISTO,
	BLOQUEANDO,
	EJECUTANDO
} estado;

//////////////////////////// PROCESOS ////////////////////////////

typedef enum{
	PLANIFICADOR,
	COORDINADOR,
	INSTANCIA,
	ESI
}proceso;

//////////////////////////// MENSAJES ////////////////////////////
//TODOS o CASI TODOS
typedef struct{
	proceso proceso;
	int id_proceso;
}t_handshake;

typedef struct{
	char* clave;
	int id_instancia;
	char* contenido;
}status_clave;

//ESI-COORDINADOR


//ESI-PLANIFICADOR
typedef struct {
	int id;
	estado estado;
	int retardo; //tiempo en ser atendido el esi la ultima vez que ejecuto
	int ultimaRafaga; //con el retardo y la ultimaRafaga se calcula la estimacion ?
	int ultimaEstimacion;
} pcb;


typedef enum{
	EJECUTAR,
	BLOQUEARSE,
	DESBLOQUEARSE
} AccionESI;

 typedef enum{
 	EXITO,
 	MEBLOQUIE, //(y manda clave)
 	PEDIUNACLAVEMUYLARGA,
 	PEDIUNACLAVENOID,
 	PEDIUNACLAVEINACC,
} resultados_esi;

//INSTANCIA-COORDINADOR
typedef struct{
	int cantidad_entradas;
	int tamano_entrada;
} datos_configuracion;

//COORDINADOR-PLANIFICADOR
typedef struct{
	int esi_id;
	t_esi_operacion instruccion;
}pedido_esi;

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
void _exit_with_error(int socket, char* error_msg, void * buffer, t_log * logger);
void exit_gracefully(int return_nr, t_log* logger);

//funciones de conexion
int inicializar_servidor(int puerto, t_log * logger);//inicializa un servidor y espera conexiones
int connect_to_server(char* ip, char * port, t_log * logger);//devuelve el socket con el que se establece la conexion a un servidor
int aceptar_conexion(int* socket_escucha);

//funciones de send() y recv()
int enviar(int* socket_destino, void* envio, int tamanio_del_envio, int id, t_log* logger);
int recibir(int* socket_receptor, void* buffer_receptor,int tamanio_que_recibo, t_log* logger);

//funciones de serializacion, una por tipo de mensaje?

#endif /* COMMONS_PROPIAS_H_ */
