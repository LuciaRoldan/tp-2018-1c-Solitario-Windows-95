#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"

///////////////////////// VARIABLES GLOBALES /////////////////////////
datos_configuracion configuracion;

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
void recibir_configuracion(int socket_coordinador,datos_configuracion configuracion, t_log* logger);
void inicializar_instancia();
void leer_configuracion_propia(configuracion_propia* configuracion,t_log* logger);
void recibir_instruccion(int socket_coordinador, t_esi_operacion instruccion, t_log* logger);
void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion);
void enviar_a_desbloquear_clave(int* socket_coordinador, int clave, t_log* logger);
void guardar_archivo(char* clave, char* value);
int handshake(int* socket_coordinador, t_log* logger, int id_proceso);

#endif /* FUNCIONES_INSTANCIA_H_ */
