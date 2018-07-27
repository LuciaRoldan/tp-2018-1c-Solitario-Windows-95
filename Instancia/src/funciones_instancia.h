#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"
#include <semaphore.h>


///////////////////////// VARIABLES GLOBALES /////////////////////////

datos_configuracion configuracion_coordi;
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
int puntero_pagina;
int puntero_entrada;
int puntero_pagina_buscado;
//t_bitarray* precencia;


typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char ipCoordinador[20];
	char puertoCoordinador[5];
	int algoritmoDeReemplazo;
	char puntoDeMontaje[100];
	char nombreInstancia[10];
	//int nombreInstancia;
	int intervaloDump;
}configuracion_propia;

typedef struct{
	char* clave;
	int numero_pagina;
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
void recibir_configuracion(int socket_coordinador, t_log* logger);
void deserializar_configuracion(void* buffer);
void leer_configuracion_propia(char* path, configuracion_propia* configuracion);
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
void enviar_fallo(int socket_coordinador);
void enviar_exito(int socket_coordinador);

//---Algoritmos de reemplazo---//
int implementar_algoritmo(estructura_clave* entrada_nueva, int entradas_contiguas_necesarias);
int aplicar_algoritmo_circular(estructura_clave* entrada_nueva);
int aplicar_algoritmo_LRU(estructura_clave* entrada_nueva);
int aplicar_algoritmo_BSU(estructura_clave* entrada_nueva);
int buscar_mayor_bsu_atomico(int necesarias);
int buscar_mayor_lru_atomico(int necesarias);
int entradas_atomicas_contiguas(int puntero, int necesarias);
void enviar_pedido_compactacion();
int recibir_orden_compactacion();
int usar_algoritmo(estructura_clave* entrada_nueva);
int cantidad_entradas_atomicas();
int entradas_atomicas_vacias_contiguas(int necesarias);
void obtener_puntero_entrada();
bool condicion_tiene_puntero_entrada(void* datos);


// AGREGO LAS QUE FALTABAN
int asignar_memoria(estructura_clave* clave, int entradas_contiguas_necesarias, char* valor);
bool existe_clave(char* clave);
bool condicion_clave_entrada(void* datos);
void almacenar_valor(char* valor, int tamanio_valor);
void compactar();
int cantidad_entradas_ocupa(int tamanio_valor);
void procesarID(int socket_coordinador, t_log* logger);
void borrar_entrada(void* entrada);
void sumar_operacion(void* entradas);
void dump();
void dumpear(void* datos);
int any_entrada_bitmap_libre();
int buscar_siguiente_entrada_ocupada(int inicio_indice);
void reemplazar_y_destruir(int indice, estructura_clave* estructura_nueva);

#endif /* FUNCIONES_INSTANCIA_H_ */
