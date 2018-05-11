#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <Commons_propias/commons_propias.h>
//#include "coordinador_funciones.h"


typedef enum {LSU, EL, KE} tipo_algoritmo;

typedef struct{
	char* ip;
	char* puerto_escucha;
	int* algoritmo_distribucion;
	int* cantidad_entradas;
	int* tamano_entrada;
	int* retardo;
}info_archivo_config;

typedef enum{			//PARA LA COMMONS
	PLANIFICADOR,
	COORDINADOR,
	INSTANCIA,
	ESI
}proceso;

#endif /* COORDINADOR_H_ */
