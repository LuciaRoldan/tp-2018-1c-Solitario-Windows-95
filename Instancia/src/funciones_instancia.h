#ifndef FUNCIONES_INSTANCIA_H_
#define FUNCIONES_INSTANCIA_H_

#include "commons_propias/commons_propias.h"

typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char ipCoordinador[20];
	char puertoCoordinador[5];
	char algoritmoDeReemplazo[6];
	char puntoDeMontaje[100];
	char nombreInstancia[100];
	int intervaloDump;
}configuracion_propia;

///////////////////////// FUNCIONES /////////////////////////
datos_configuracion recibir_configuracion(int* socket_coordinador,t_log* logger);
void inicializar_instancia();
void leer_configuracion_propia(configuracion_propia* configuracion);
t_esi_operacion recibir_instruccion(int* socket_coordinador, t_log* logger);
void procesar_instruccion(int* socket_coordinador, t_esi_operacion instruccion);
void enviar_a_desbloquear_clave(int* socket_coordinador, int clave, t_log* logger);
void guardar_archivo(char* clave, char* value);

#endif /* FUNCIONES_INSTANCIA_H_ */
