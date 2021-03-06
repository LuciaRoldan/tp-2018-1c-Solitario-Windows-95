#ifndef FUNCIONES_COORDINADOR_H_
#define FUNCIONES_COORDINADOR_H_

#include <commons_propias/commons_propias.h>
#include <commons/bitarray.h>
#include <semaphore.h>
#include <commons/log.h>


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
}nodo;

typedef struct{
	nodo nodo_instancia;
	char* clave;
}nodo_clave;

///////////////////// VARIABLES GLOBALES ////////////////////

t_log * logger;
t_log* log_operaciones;
int id_esi_buscado;

//Ya tienen semaforo
t_esi_operacion operacion_ejecutando;
nodo* esi_ejecutando;
nodo* instancia_seleccionada;
pthread_t* hilo_a_cerrar;
int ultima_instancia_EL;
t_list* lista_claves;
int socket_instancia_buscado;
int socket_esi_buscado;
info_archivo_config info_coordinador;
t_list* lista_instancias;
t_list* lista_esis;
int socket_planificador; //Deberia poner un semaforo para esta variable?
int socket_escucha;
char* clave_buscada;
int id_instancia_buscado;
bool terminar_programa;
int conexion_hecha;


sem_t s_cerrar_hilo;
sem_t s_borrar_elemento;

///////////////////////// FUNCIONES /////////////////////////

//INICIALIZACION
void leer_archivo_configuracion(char* path);
void inicializar_coordinador();
void conectar_planificador();
void inicializar_semaforos();

//COMUNICACION
int enviar_configuracion_instancia(int socket);
int enviar_status_clave(int socket, status_clave status);
int enviar_pedido_valor(int socket, char* clave, int id);
int enviar_confirmacion(int socket, int confirmacion, int id);
int enviar_operacion(int socket, t_esi_operacion instruccion);
int recibir_confirmacion(int socket);
char* recibir_pedido_clave(int socket);
status_clave recibir_status(int socket);
t_esi_operacion recibir_instruccion(int socket);
void enviar_clave(char* clave, int socket);

//CONEXION
int handshake(int socket);
void procesar_conexion();
void atender_planificador();
void atender_esi(void* datos_esi);
void atender_instancia(void* datos_instancia);
void desconectar_instancia(int socket);
void enviar_claves(hilo_proceso mis_datos);

//PRROCESAMIENTO
int procesar_mensaje(int socket);
int procesar_instruccion(t_esi_operacion instruccion, int socket);
void liberar_instruccion();
void aplicar_retardo();

//DE LISTAS
nodo* seleccionar_instancia(char* clave);
nodo* encontrar_esi(int socket);
nodo* buscar_instancia(char* clave);
void reemplazar_instancia(nodo nodo);
bool condicion_socket_esi(void* datos);
bool condicion_socket_instancia(void* datos);
bool condicion_clave(void* datos);
bool condicion_id_esi(void* datos);
bool condicion_id_instancia(void* datos);
bool condicion_socket_clave(void* datos);
void eliminar_nodo(void* datos);
void eliminar_nodo_clave(void* datos);
bool clave_accesible(char* clave);
void cerrar_instancias();
void fin_instancia(void* datos);
int buscar_instancia_ficticia(char* clave);
void compactar_intancias();

//DE HILOS
void agregar_nuevo_esi(int socket_esi, int id_esi);
void agregar_nueva_instancia(int socket_instancia, int id_instancia);
void serializar_hilo_proceso(void* buffer, hilo_proceso hilo);
hilo_proceso deserializar_hilo_proceso(void *buffer_recepcion);

#endif /* FUNCIONES_COORDINADOR_H_ */
