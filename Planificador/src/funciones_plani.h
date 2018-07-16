#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "commons_propias/commons_propias.h"
#include <semaphore.h>


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



///---Variables Globales Auxiliares---///
int id_buscado;
int id_esi_ejecutando;
char* clave_buscada;
pcb* esi_abortado;


///---SEMAFOROS---///
pthread_mutex_t un_mutex;
sem_t un_semaforo;

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
/////-----CONSOLA-----/////
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

bool algoritmo_SJF_SD(void* pcb1, void* pcb2);
bool algoritmo_SJF_CD(void* pcb_1, void* pcb_2);
bool algoritmo_HRRN(void* pcb1, void* pcb2);

void actualizar_ultima_estimacion_SJF();
void actualizar_ultima_estimacion_HRRN();

void calcular_estimacion_SJF(void* pcb);
void calcular_estimacion_HRRN(void* pcb);

//Operaciones sobre PCBs
void registrar_exito_en_pcb(int id_esi);
void mover_esi_a_bloqueados(char* clave_buscada, int esi_id);
void abortar_esi(int id_esi);

//Operaciones sobre claves_bloqueadas
void liberar_clave(char* clave);



///---FUNCIONES AUXILIARES---///
//ids_iguales
bool ids_iguales_pcb(void* pcbb);
bool ids_iguales_cola_de_esis(void* id);
bool es_el_primer_esi_ready(void *pcbb);
bool no_es_el_primer_esi_ready(void *pcbb);

//claves_iguales
bool claves_iguales_nodo_clave(void* nodo_clave);
void quitar_esi_de_cola_bloqueados(void* clave_bloq);
void imprimir_id_esi(void* esi);
void sumar_ultima_rafaga(int id_esi);
void sumar_retardo(void* pcbb);
void sumar_retardo_otros_ready();
void procesar_motivo_aborto(int protocolo);

//////////-----HABLAR CON COORDINADOR-----//////////

void manejar_coordinador();
void procesar_pedido(t_esi_operacion instruccion);
void mostrar_status_clave(status_clave status);
void recibir_status_clave();

//////////MENSAJES CON COORDINADOR//////////

//ENVIAR COORDINADOR
void informar_aborto_coordinador_clave_no_id();
void informar_aborto_coordinador_clave_no_b();
void informar_exito_coordinador();
void informar_bloqueo_coordinador();



//////////MENSAJES CON ESIS//////////

#endif /* PLANIFICADOR_FUNCIONES_H_ */
