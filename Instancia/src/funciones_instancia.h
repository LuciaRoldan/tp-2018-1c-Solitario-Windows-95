#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"
#include <semaphore.h>


///////////////////////// VARIABLES GLOBALES /////////////////////////

datos_configuracion configuracion;
int* acceso_tabla;
t_list* tabla_entradas;
char* clave_buscada;
int indice;
char* inicio_memoria;
int memoria_total;
int memoria_usada;
bool activa;
int id_instancia;
int puntero_circular;
int lugar_de_memoria;



typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char ipCoordinador[20];
	char puertoCoordinador[5];
	tipo_algoritmo_reemplazo algoritmoDeReemplazo;
	char puntoDeMontaje[100];
	int nombreInstancia;
	int intervaloDump;
}configuracion_propia;

typedef struct{
	char* clave;
	int numero_entrada;
	int cantidad_entradas;
	int tamanio_valor;
	int cantidad_operaciones;
	char* valor;
}estructura_clave;


configuracion_propia mi_configuracion;
char* ipCoordinador;
int puertoCoordinador;
tipo_algoritmo_reemplazo algoritmo_reemplazo;
int idInstancia;
int intervaloDump;
int cantidad_entradas;
int tamano_entrada;
char* memoria;
FILE* archivo_configuracion;
FILE*archivo;
t_log * logger;
pthread_mutex_t m_tabla;
int socket_coordinador;
sem_t s_compactacion;

///////////////////////// FUNCIONES /////////////////////////
///---Inicializacion---///
int handshake_instancia(int socket_coordinador, t_log* logger, int id_proceso);
datos_configuracion recibir_configuracion(int socket_coordinador, t_log* logger);
void deserializar_configuracion(void* buffer);
void leer_configuracion_propia(char* path, configuracion_propia* configuracion,t_log* logger);
void inicializar_instancia();

//---Procesamiento---///
t_esi_operacion recibir_instruccion(int socket_coordinador, t_log* logger);
t_esi_operacion deserializar_instruccion(void* buffer);
void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion,t_log* logger);
void serializar_pedido_desbloqueo(void* buffer, char* clave);
void guardar_archivo(char* clave,int tamanio_clave, t_log* logger);

//---Pedido status_clave---//
char* recibe_pedido_status();
int enviar_status_clave(char* clave);

//---Enviar---//
void enviar_fallo(int socket_coordinador, t_log* logger);
void enviar_exito(int socket_coordinador, t_log* logger);

//---Algoritmos de reemplazo---//
void implementar_algoritmo(estructura_clave* entrada_nueva, t_log* logger);
void aplicar_algoritmo_circular(estructura_clave* entrada_nueva, t_log* logger);
void aplicar_algoritmo_LRU(estructura_clave* entrada_nueva, t_log* logger);
void aplicar_algoritmo_BSU(estructura_clave* entrada_nueva, t_log* logger);

// AGREGO LAS QUE FALTABAN
void asignar_memoria(estructura_clave clave, int entradas_contiguas_necesarias, char* valor, t_log* logger);
bool existe_clave(char* clave);
int cantidad_entradas_ocupa(int tamanio_valor);
bool condicion_clave_entrada(void* datos);
void almacenar_valor(char* valor, int tamanio_valor);
void compactar();
void hilo_compactar();
void implementar_algoritmo();
void procesarID(int socket_coordinador, t_log* logger);
void borrar_entrada(void* entrada);
void sumar_operacion(void* entradas);
void dump();
void dumpear(void* datos);

#endif /* FUNCIONES_INSTANCIA_H_ */
