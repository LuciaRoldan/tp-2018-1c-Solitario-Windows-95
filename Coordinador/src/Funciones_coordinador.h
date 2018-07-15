#ifndef FUNCIONES_COORDINADOR_H_
#define FUNCIONES_COORDINADOR_H_

#include <commons_propias/commons_propias.h>
#include <commons/bitarray.h>

#define ABORTO_ESI -99
#define EXITO_ESI 63
#define FALLO_ESI 64
#define FIN_ESI -100

t_log * logger;
t_conexion conexionInstancia;
int socket_planificador;
int socket_escucha;
t_list* lista_esis;
t_list* lista_instancias;
int socket_esi_buscado;
int socket_instancia_buscado;
t_dictionary* diccionario_claves;
int ultima_instancia_EL;

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

info_archivo_config info_coordinador;

typedef struct{
	int socket;
	int id;
}hilo_proceso;

typedef struct{
	int socket;
	int id;
	pthread_t hilo;
}nodo;


///////////////////////// FUNCIONES /////////////////////////

//INICIALIZACION
void leer_archivo_configuracion();
void inicializar_coordinador();
void conectar_planificador();

//COMUNICACION
int enviar_configuracion_instancia(int socket);
int enviar_pedido_esi(int esi_id, t_esi_operacion instruccion);
int enviar_status_clave(int socket, status_clave status);
int enviar_pedido_valor(int socket, char* clave, int id);
int enviar_confirmacion(int socket, int confirmacion, int id);
int recibir_confirmacion(int socket);
char* recibir_pedido_clave(int socket);
status_clave recibir_status(int socket);
t_esi_operacion recibir_instruccion(int socket);

//CONEXION
int handshake(int socket);
void procesar_conexion();
void atender_planificador();
void atender_esi(void* datos_esi);
void atender_instancia(void* datos_instancia);
int desconectar_instancia(int socket);

//PRROCESAMIENTO
int procesar_mensaje(int socket);
int procesar_instruccion(t_esi_operacion instruccion, int socket);

//DE LISTAS
nodo* seleccionar_instancia(char* clave);
nodo* encontrar_esi(int socket);
nodo* buscar_instancia(char* clave);
int verificar_existencia_instancia(nodo nodo);
void reemplazar_instancia(nodo nodo);
bool condicion_socket_esi(void* datos);
bool condicion_socket_instancia(void* datos);

//DE HILOS
void agregar_nuevo_esi(int socket_esi, int id_esi);
void agregar_nueva_instancia(int socket_instancia, int id_instancia);
void serializar_hilo_proceso(void* buffer, hilo_proceso hilo);
hilo_proceso deserializar_hilo_proceso(void *buffer_recepcion);

#endif /* FUNCIONES_COORDINADOR_H_ */
