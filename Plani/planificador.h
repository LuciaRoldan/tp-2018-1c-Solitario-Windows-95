#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <Commons_propias/commons_propias.h>

//STRUCTS
typedef struct{
	int IDEsi;
	struct Esi* sgte;
} ColaDeEsi; //esto es una cola o un esi ???


typedef struct{ //estructura que va a contener todas las claves y el estado de cada una
//	Clave clave;
	int esiQueLaUsa; //ESI que la esta usando ahora. O sea que hizo un GET.
	ColaDeEsi esisEnEspera; //ESIS bloqueados esperando a que se desbloquee la clave.
} ClaveBloqueada;

typedef struct{
	ClaveBloqueada clave;
	ClaveBloqueada* sgte;
} ClavesBloqueadas;

typedef enum{
	SJF_CD,
	SJF_SD,
	HRRN
} AlgoritmoPlanificacion;


//FUNCIONES


t_log logger;



#endif /* PLANIFICADOR_H_ */

