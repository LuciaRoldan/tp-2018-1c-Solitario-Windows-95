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



typedef struct{
	int id_mensaje;
	int largo_mensaje;
} t_header;

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
	int id;
	proceso proceso;
} __attribute__((packed)) t_handshake;

typedef struct{
	char* clave;
	int id_instancia;
	char* contenido;
}status_clave;

//ESI-PLANIFICADOR
typedef struct {
	int id;
	estado estado;
	int retardo; //tiempo en ser atendido el esi la ultima vez que ejecuto
	int ultimaRafaga; //con el retardo y la ultimaRafaga se calcula la estimacion ?
	int ultimaEstimacion;
	int socket;
} pcb;

typedef enum{
	EJECUTAR,
	BLOQUEARSE,
	DESBLOQUEARSE
} AccionESI;

//ESI-COORDINADOR
 typedef enum{
 	EXITO,
	FALLO,
 	PEDIUNACLAVEMUYLARGA,
 	PEDIUNACLAVENOID,
 	PEDIUNACLAVEINACC,
} resultado_esi;

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
	char* ip;
	char* puerto;
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
int aceptar_conexion(int socket_escucha);

//funciones de enviar y recibir
int enviar(int socket_destino, void* envio, int tamanio_del_envio, t_log* logger);
int recibir(int socket_receptor, void* buffer_receptor,int tamanio_que_recibo, t_log* logger);
int recibir_int(int socket, t_log* logger);

//funciones de serializacion, una por tipo de mensaje

//serializar
void serializar_id(void* envio, int id);
void serializar_int(void* envio, int id, int protocolo);
void serializar_handshake(void* buffer, t_handshake handshake);
void serializar_instruccion(void* buffer, t_esi_operacion la_instruccion);
void serializar_configuracion_inicial_instancia(void* buffer, datos_configuracion configuracion);
void serializar_pedido_esi(void* buffer, pedido_esi pedido);
void serializar_status_clave(void* buffer, status_clave status);
void serializar_string(void* buffer, char* cadena, int protocolo);

//deserealizar
int deserializar_id(void* buffer_mensaje);
t_handshake deserializar_handshake(void *buffer_recepcion);
void deserializar_pedido_coordinador(void* buffer, pedido_esi* pedido);
t_esi_operacion deserializar_instruccion(void* buffer);
datos_configuracion deserializar_configuracion_inicial_instancia(void* buffer);
status_clave deserializar_status_clave(void* buffer);
pedido_esi deserializar_pedido_esi(void* buffer);
char* deserializar_string(void* buffer);

//tamaños de buffers
int tamanio_buffer_instruccion(t_esi_operacion instruccion);
int tamanio_buffer_status(status_clave status);
int tamanio_buffer_string(char* cadena);

#endif /* COMMONS_PROPIAS_H_ */
