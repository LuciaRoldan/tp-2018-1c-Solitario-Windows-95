#ifndef INSTANCIA_H_
#define INSTANCIA_H_
#include <Commons_propias/commons_propias.h>

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
void inicializar_instancia();
void leer_configuracion_propia();
void leer_archivo_configuracion(configuracion);
t_esi_operacion recibir_instruccion();
void procesar_instruccion(t_esi_operacion instruccion);
void enviar_a_desbloquear_clave(int clave);
void guardar_archivo(int clave, int * value);



#endif /* INSTANCIA_H_ */
