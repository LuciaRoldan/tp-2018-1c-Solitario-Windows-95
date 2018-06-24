#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "commons_propias/commons_propias.h"


//STRUCTS

/*typedef struct{
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

*/
typedef enum{
	SJF_CD,
	SJF_SD,
	HRRN
} algoritmo_planificacion;


typedef struct{
	int socket_coordinador;
	int socket_esis;
} sockets;

//////////-----VARIABLES GLOBALES-----//////////

sockets sockets_planificador;
t_conexion conexion_planificador;
t_conexion conexion_coordinador;
t_log * logger;
algoritmo_planificacion algoritmoPlanificacion; //FALTA LEER DE ARCHIVO
int estimacionInicial; //FALTA LEER DE ARCHIVO

//char* clavesInicialmenteBloqueadas; //es una lista

/*
struct ColaDeEsi *colaDeReadyEsis;
struct ColaDeEsi *colaDeBloqueadoEsis;
struct ColaDeEsi *colaDeFinalizadoEsis;

struct ClavesBloqueadas *clavesBloqueadas;
*/



//FUNCIONES


//testing

//////////-----INICIALIZACION-----//////////
//int handshake_esi(int* socket_esi);
void handshake_coordinador(int socket_coordinador);
//pcb crear_pcb_esi(int socket_cliente, int id_esi);
sockets inicializar_planificador();
void leer_archivo_configuracion();
void conectarse_al_coordinador(int socket_coordinador);


/*
//consola
void manejar_consola();
void pausar_planificador();
void bloquear_esi(char* clave, int esi_id);
void desbloquearEsi(char* clave);
//ColaDeEsi listar(Clave clave);
void kill(int id_esi);
int status(char* clave);
int* deadlock();
void kill_esis(ColaDeEsi esis);



//manejar esis
void recibir_esis(void* socket_esis);
void manejar_esi(pcb pcb_esi);



void mover_esi_a_bloqueado(int id_esi);
void mover_esi_a_ready(int id_esi);
void mover_esi_a_finalizado(int id_esi);
void ejecutar_proximo_esi();
int clave_tomada_por_otro_esi(int id_esi);
void actualizar_pcb_esi_bloqueeado(int id_esi); //atomico
void asignar_clave_esi(char* clave, int id_esi);
void actualizar_pcb_esi_asignado(int id_esi); //atomico. siempre de a uno




//////////-----HABLAR CON COORDINADOR-----//////////
*/
void manejar_coordinador();
/*
void responder_a_pedido_coordinador(int socket_coodinador, pedido_esi pedido);
void mostrar_status_clave(status_clave status);

//////////MENSAJES CON COORDINADOR//////////
//RECIBIR COORDINADOR */
pedido_esi recibir_pedido_coordinador(int socket_coordinador);
/*
status_clave recibir_status_clave(int socket_coordinador, status_clave status);

//ENVIAR COORDINADOR
void informar_bloqueo_coordinador(int socket_coordinador, int id_esi);
void informar_exito_coordinador(int socket_coordinador, int id_esi);


//////////MENSAJES CON ESIS//////////
//RECIBIR ESIS
resultado_esi recibir_resultado_esi(int socket_esi);


//ENVIAR ESIS

*/

#endif /* PLANIFICADOR_FUNCIONES_H_ */
