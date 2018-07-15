#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "commons_propias/commons_propias.h"


//STRUCTS

typedef struct{
	int id_esi;
	int* sgte;
} cola_de_esis; //esto es una cola o un esi ??? */


typedef struct{ //estructura que va a contener todas las claves y el estado de cada una
	char* clave;
	int esi_que_la_usa; //ESI que la esta usando ahora. O sea que hizo un GET.
	t_list* esis_en_espera; //ESIS bloqueados esperando a que se desbloquee la clave.
} clave_bloqueada;

//typedef struct{
//	clave_bloqueada clave;
//	claves_bloqueadas* sgte;
//} claves_bloqueadas;


//typedef enum{
//	SJF_CD,
//	SJF_SD,
//	HRRRN
//} algoritmo_planificacion;


typedef struct{
	int socket_coordinador;
	int socket_esis;
} sockets;



//////////-----VARIABLES GLOBALES-----//////////

sockets sockets_planificador;
t_conexion conexion_planificador;
t_conexion conexion_coordinador;
t_log * logger;
char* algoritmo;
int estimacion_inicial;
float alpha;
//char* claves_inicialmente_bloqueadas; //es una lista

t_list* pcbs;
t_list* esis_ready;
t_list* esis_bloqueados;
t_list* esis_finalizados;
t_list* claves_bloqueadas;

int id_esi_ejecutando;

///---Variables Globales Auxiliares---///
int id_buscado;
char* clave_buscada;
pcb* esi_abortado;

//FUNCIONES


//testing

//////////-----INICIALIZACION-----//////////
int handshake_esi(int socket_esi);
void handshake_coordinador(int socket_coordinador);
pcb crear_pcb_esi(int socket_cliente, int id_esi);
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


*/
//MANEJAR ESIS
void recibir_esis(void* socket_esis);
void manejar_esi(void* pcb_esi);

//Planificar
void planificar();
void ordenar_pcbs();
void planificacionSJF_CD();
void planificacionSJF_SD();
void planificacionHRRN();

bool algoritmo_SJF(void* pcb1, void* pcb2);
bool algoritmo_HRRN(void* pcb1, void* pcb2);

//Operaciones sobre PCBs
void registrar_exito_en_pcb(int id_esi);
void mover_esi_a_bloqueados(char* clave, clave_bloqueada* nodo_clave_buscada, int esi_id);
void abortar_esi(int id_esi);

//Operaciones sobre claves_bloqueadas
void liberar_clave(char* clave);


///---FUNCIONES AUXILIARES---///
//ids_iguales
bool ids_iguales_pcb(void* pcbb);
bool ids_iguales_cola_de_esis(void* id);

//claves_iguales
bool claves_iguales_nodo_clave(void* nodo_clave);

void* quitar_esi_de_cola_bloqueados(void* clave_bloq);

void imprimir_id_esi(void* esi);


/*

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
void procesar_pedido(t_esi_operacion instruccion);
void mostrar_status_clave(status_clave status);
void recibir_status_clave();

//////////MENSAJES CON COORDINADOR//////////
//RECIBIR COORDINADOR */
//pedido_esi recibir_pedido_coordinador(int socket_coordinador);


//ENVIAR COORDINADOR
void informar_aborto_coordinador();
void informar_exito_coordinador();
void informar_fallo_coordinador();


//////////MENSAJES CON ESIS//////////
//RECIBIR ESIS
//resultado_esi recibir_resultado_esi(int socket_esi);


//ENVIAR ESIS



#endif /* PLANIFICADOR_FUNCIONES_H_ */
