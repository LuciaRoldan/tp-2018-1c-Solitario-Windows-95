#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"

///////////////////////// VARIABLES GLOBALES /////////////////////////

datos_configuracion configuracion;
int* acceso_tabla;
t_list* tabla_entradas;
char* clave_buscada;
int indice;
char* inicio_memoria;
int memoria_total;
int memoria_usada;


typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char ipCoordinador[20];
	char puertoCoordinador[5];
	char algoritmoDeReemplazo[6];
	char* puntoDeMontaje;
	int nombreInstancia;
	int intervaloDump;
}configuracion_propia;

typedef struct{
	char* clave;
	int numero_entrada;
	int cantidad_entradas;
	int tamanio_valor;
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

int socket_coordinador;

///////////////////////// FUNCIONES /////////////////////////
int handshake_instancia(int socket_coordinador, t_log* logger, int id_proceso);
datos_configuracion recibir_configuracion(int socket_coordinador, t_log* logger);
void deserializar_configuracion(void* buffer);
void inicializar_instancia();
void enviar_fallo(int socket_coordinador, t_log* logger);
void enviar_exito(int socket_coordinador, t_log* logger);/* {
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 25);
	enviar(socket_coordinador, buffer, sizeof(int), logger);
	log_info(logger, "Le respondi al coordinador");
	free(buffer);
}*/
void leer_configuracion_propia(configuracion_propia* configuracion,t_log* logger);
t_esi_operacion recibir_instruccion(int socket_coordinador, t_log* logger);
t_esi_operacion deserializar_instruccion(void* buffer);
char* recibe_pedido_status(int socket_coordinador, t_log* logger);
void enviar_a_desbloquear_clave(int socket_coordinador, char* clave, t_log* logger);
void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion,t_log* logger);
void serializar_pedido_desbloqueo(void* buffer, char* clave);
void guardar_archivo(char* clave,int tamanio_clave, t_log* logger);

// AGREGO LAS QUE FALTABAN
void asignar_memoria(estructura_clave clave, int entradas_contiguas_necesarias, char* valor, t_log* logger);
bool existe_clave(char* clave);
int cantidad_entradas_ocupa(int tamanio_valor);
bool condicion_clave_entrada(void* datos);
void almacenar_valor(char* valor, int tamanio_valor);
void compactar();
void implementar_algoritmo();


#endif /* FUNCIONES_INSTANCIA_H_ */
