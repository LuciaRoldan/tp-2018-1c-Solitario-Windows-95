#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"

///////////////////////// VARIABLES GLOBALES /////////////////////////
datos_configuracion configuracion;
t_dictionary* diccionario_memoria;
char* claveActual;

typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char ipCoordinador[20];
	char puertoCoordinador[5];
	char algoritmoDeReemplazo[6];
	char puntoDeMontaje[100];
	int nombreInstancia;
	int intervaloDump;
}configuracion_propia;

///////////////////////// FUNCIONES /////////////////////////
int handshake(int* socket_coordinador, t_log* logger, int id_proceso);
void recibir_configuracion(int socket_coordinador, t_log* logger);
void deserializar_configuracion(void* buffer);
void inicializar_instancia();
void enviar_fallo(int socket_coordinador, t_log* logger);
void enviar_exito(int socket_coordinador, logger);
void leer_configuracion_propia(configuracion_propia* configuracion,t_log* logger);
t_esi_operacion recibir_instruccion(int socket_coordinador, t_log* logger);
t_esi_operacion deserializar_instruccion(void* buffer,t_log* logger);
char* recibe_pedido_status(int socket_coordinador, t_log* logger);
void enviar_a_desbloquear_clave(int socket_coordinador, char* clave, t_log* logger);
void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion,t_log* logger);
void serializar_pedido_desbloqueo(void* buffer, char* clave);
void guardar_archivo(char* clave, t_log* logger);


#endif /* FUNCIONES_INSTANCIA_H_ */
