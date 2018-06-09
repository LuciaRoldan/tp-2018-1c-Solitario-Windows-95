#ifndef FUNCIONES_COORDINADOR_H_
#define FUNCIONES_COORDINADOR_H_

#include <commons_propias/commons_propias.h>
#include <commons/bitarray.h>

t_log * logger;
t_conexion conexionInstancia;
int socket_planificador;

///////////////////////// STRUCTS /////////////////////////

typedef enum {LSU, EL, KE} tipo_algoritmo_distribucion;

typedef struct{
	char ip[16];
	int puerto_escucha;
	int algoritmo_distribucion;
	int cantidad_entradas;
	int tamano_entrada;
	int retardo;
}info_archivo_config;

///////////////////////// FUNCIONES /////////////////////////

//INICIALIZACION
void leer_archivo_configuracion(info_archivo_config* configuracion);
int inicializar_coordinador(info_archivo_config configuracion);
void conectar_planificador(int* socket_escucha);

//COMUNICACION
int enviar_configuracion_instancia(int* socket, info_archivo_config configuracion);
int enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion);
int enviar_status_clave(int* socket, status_clave* status);
int enviar_pedido_valor(int* socket, char* clave);
int enviar_confirmacion(int* socket, int* confirmacion, int id);
int recibir_confirmacion(int* socket);
char* recibir_pedido_clave(int* socket);
status_clave recibir_status(int* socket);
t_esi_operacion recibir_instruccion(int* socket);

//FUNCIONAMIENTO INTERNO
int handshake(int* socket);
int procesar_mensaje(int id, int* socket);
void conectar_esi(int* socket, int id_recibido);
void conectar_instancia(int* socket, int id_recibido);
void desconectar_instancia();
int* buscar_instancia(char* clave);
int procesar_instruccion(t_esi_operacion instruccion, int* socket);

#endif /* FUNCIONES_COORDINADOR_H_ */
