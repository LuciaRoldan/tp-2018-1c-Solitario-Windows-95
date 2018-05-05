#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <Commons_propias/commons_propias.h>

//structs

typedef enum {LSU, EL, KE}tipo_algoritmo;

typedef struct{				//Va en commons_propias
	int cantidad_entradas;
	int tamano_entrada;
}datos_configuracion;

//declares

t_conexion conexion;
t_log * logger;


#endif /* COORDINADOR_H_ */
