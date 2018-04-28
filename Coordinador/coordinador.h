#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <Commons_propias/commons_propias.h>

//structs

typedef struct {
	char* ip [10];
	char* puerto [5];

}t_conexion;


//declares

t_conexion conexion;
t_log * logger;


#endif /* COORDINADOR_H_ */
