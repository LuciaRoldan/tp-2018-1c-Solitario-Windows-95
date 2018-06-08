#ifndef INSTANCIA_FUNCIONES_H_
#define INSTANCIA_FUNCIONES_H_

#include "../Commons_propias/Commons_propias/commons_propias.h"

typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo_reemplazo;

typedef struct{
	char* ipCoordinador;
	int puertoCoordinador;
	char* nombreInstancia;
	int intervaloDump;
}configuracion_propia;

///////////////////////// FUNCIONES /////////////////////////
datos_configuracion recibir_configuracion(int* socket_coordinador,t_log* logger);
void inicializar_instancia();
configuracion_propia leer_configuracion_propia(FILE* archivo);
t_esi_operacion recibir_instruccion(int* socket_coordinador, t_log* logger);
void procesar_instruccion(int* socket_coordinador, t_esi_operacion instruccion);
void enviar_a_desbloquear_clave(int* socket_coordinador, int clave, t_log* logger);
void guardar_archivo(char* clave, char* value);



#endif /* INSTANCIA_FUNCIONES_H_ */
