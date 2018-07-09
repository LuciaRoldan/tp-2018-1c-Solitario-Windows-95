#ifndef FUNCIONES_COORDINADOR_H_
#define FUNCIONES_COORDINADOR_H_

#include <commons_propias/commons_propias.h>
#include <commons/bitarray.h>

t_log * logger;
t_conexion conexionInstancia;
int socket_planificador;
int socket_escucha;
t_list* lista_esis;
int socket_esi_buscado;

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

typedef struct{
	int socket;
	int id;
}hilo_proceso;

typedef struct{
	int socket;
	int id;
	pthread_t hilo;
}nodo_esi;

///////////////////////// FUNCIONES /////////////////////////

//INICIALIZACION
void leer_archivo_configuracion(info_archivo_config* configuracion);
void inicializar_coordinador(info_archivo_config configuracion);
void conectar_planificador();

//COMUNICACION
int enviar_configuracion_instancia(int socket, info_archivo_config configuracion);
int enviar_pedido_esi(int esi_id, int socket, t_esi_operacion instruccion);
int enviar_status_clave(int socket, status_clave* status);
int enviar_pedido_valor(int socket, char* clave);
int enviar_confirmacion(int socket, int* confirmacion, int id);
int recibir_confirmacion(int socket);
char* recibir_pedido_clave(int socket);
status_clave recibir_status(int socket);
t_esi_operacion recibir_instruccion(int socket);

//FUNCIONAMIENTO INTERNO
int handshake(int socket);
int procesar_mensaje(int id, int socket);
void procesar_conexion();
void atender_planificador();
void atender_esi(void* datos_esi);
void atender_instancia(void* datos_instancia);
void desconectar_instancia();
int* buscar_instancia(char* clave);
int procesar_instruccion(t_esi_operacion instruccion, int socket);
void agregar_nuevo_esi(int socket_esi, int id_esi);
void agregar_nueva_instancia(int socket_instancia, int id_instancia);
bool condicion_join_esi(void* datos);

//DE HILOS
void serializar_hilo_proceso(void* buffer, hilo_proceso hilo);
hilo_proceso deserializar_hilo_proceso(void *buffer_recepcion);

#endif /* FUNCIONES_COORDINADOR_H_ */
