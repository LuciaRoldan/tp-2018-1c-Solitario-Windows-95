#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "commons_propias/commons_propias.h"

//VARIABLES
t_log * logger;
char* puertoEscucha; //CREO que no es necesario, el puerto de escucha esta implicito
// en el socket de escucha, lo retorna la funcion int listen();
AlgoritmoPlanificacion algoritmoPlanificacion;
int estimacionInicial;
char* ipCoordinador[10]; //no es necesario
char* puertoCoordinador[5];// no es necesario
char* clavesInicialmenteBloqueadas; //es una lista //va a haber que parsearlo. paja.
//FILE* configuracion; no sirve


struct ColaDeEsi *colaDeReadyEsis;
struct ColaDeEsi *colaDeBloqueadoEsis;
struct ColaDeEsi *colaDeFinalizadoEsis;

struct ClavesBloqueadas *clavesBloqueadas;

t_conexion conexion_planificador;
t_conexion conexion_coordinador;







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

//inicializando
int handshake_esi(int* socket_esi);
pcb crear_pcb_esi(int socket_cliente, int id_esi);
void inicializar_planificador(int* socket_coordinador, int* socket_esis, t_log* logger);
void leer_archivo_configuracion();
void conectarse_al_coordinador(int* socket_coordinador);



//consola
void manejar_consola();
void pausarPlanificador();
void bloquearEsi(int clave, int id_esi);
void desbloquearEsi(char* clave);
//ColaDeEsi listar(Clave clave);
void kill(int id_esi);
int status(char* clave);
int* deadlock();
void killEsis(ColaDeEsi esis);



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




//hablar con coordinador
void manejar_coordinador();
void responder_a_pedido_coordinador(int socket_coodinador, pedido_esi pedido);
void mostrar_status_clave(status_clave status);




//MENSAJES
//del esi
resultado_esi recibir_resultado_esi(int* socket_esi, t_log* logger);
int recibir_tipo_mensaje_coordinador(int socket_coordinador);
pedido_esi recibir_pedido_coordinador(int socket_coordiandor);
status_clave recibir_status_clave(int socket_coordinador, status_clave status);
void informar_bloqueo_coordinador(int socket_coordinador, int id_esi);
void informar_exito_coordinador(int socket_coordinador, int id_esi);


#endif /* PLANIFICADOR_FUNCIONES_H_ */
