

#ifndef FUNCIONES_PLANI_H_
#define FUNCIONES_PLANI_H_

#include <commons_propias/commons_propias.h>

//STRUCTS
typedef struct{
	int PID;
	int context;

}resultado_esi;


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


void inicializar_planificador(int* socketCoordinador, int* socketEsis, t_log * logger);
void leer_archivo_configuracion();
int conectarse_al_coordinador();
void manejar_esi();
void procesar_exito_o_error(resultado_esi * mensaje);
void manejar_coordinador();
void recibir_esis(int socketEsis);


t_conexion conexion_planificador;
t_conexion conexion_coordinador;


#endif /* FUNCIONES_PLANI_H_ */
