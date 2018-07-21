#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "commons_propias/commons_propias.h"
#include "funciones_plani.h"
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


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

// Tipo operaciones de consola
typedef enum {
	BLOQUEAR,
	DESBLOQUEAR,
	PAUSAR,
	CONTINUAR,
	LISTAR,
	KILL,
	STATUS,
	DEADLOCK,
	INVALIDO
} op_consola;

///---Variables Globales Auxiliares---///
int id_buscado;
int id_esi_ejecutando;
char* clave_buscada;
pcb* esi_abortado;
pthread_t* hilo_a_cerrar;
int hay_hilos_por_cerrar;
int pausar_planificador;
int terminar_todo;


///---SEMAFOROS---///
pthread_mutex_t m_recibir_resultado_esi;
pthread_mutex_t m_hilo_a_cerrar;

//inicializar
pthread_mutex_t m_lista_pcbs;
pthread_mutex_t m_lista_esis_ready;
pthread_mutex_t m_lista_claves_bloqueadas;
pthread_mutex_t m_id_buscado;
pthread_mutex_t m_clave_buscada;
pthread_mutex_t m_id_esi_ejecutando;

sem_t s_cerrar_un_hilo;
sem_t s_hilo_cerrado;
sem_t s_eliminar_pcb;

sem_t s_podes_procesar_un_pedido;
sem_t s_podes_procesar_una_respuesta;
sem_t s_planificar;



sem_t un_semaforo;

//FUNCIONES


//testing

//////////-----INICIALIZACION-----//////////
int handshake_esi(int socket_esi);
void handshake_coordinador(int socket_coordinador);
pcb* crear_pcb_esi(int socket_cliente, int id_esi, pthread_t hilo_esi);
sockets inicializar_planificador();
void leer_archivo_configuracion();
void conectarse_al_coordinador(int socket_coordinador);
void inicializar_semaforos();



/////-----CONSOLA-----/////
void ejecutar_consola();

// Operaciones internas de consola
op_consola analizar_linea(char* linea);
char**  string_to_array(char* text, char* separator);

// Comandos de consola
void pausar_planificacion();
void continuar_planificacion();
void bloquear(char * clave, int id);
void desbloquear(char * clave);
void listar_procesos_encolados(char* recurso);
void kill(int id);
void pedir_status(char* clave);
void deadlock();

//MANEJAR ESIS
void recibir_esis(void* socket_esis);
void manejar_esis();
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
void mover_esi_a_finalizados(int id_esi);

//Operaciones sobre claves_bloqueadas
void liberar_clave(char* clave);

///---RUTINAS DE CIERRE DE PLANI---///
void cerrar_planificador();
void cerrar_sockets();
void cerrar_hilos();

///---FUNCIONES AUXILIARES---///
//ids_iguales
bool ids_iguales_pcb(void* pcbb);
bool ids_iguales_cola_de_esis(void* id);
bool es_el_primer_esi_ready(void *pcbb);
bool no_es_el_primer_esi_ready(void *pcbb);

bool claves_iguales_nodo_clave(void* nodo_clave);
void quitar_esi_de_cola_bloqueados(void* clave_bloq);
void imprimir_id_esi(void* esi);
void sumar_ultima_rafaga(int id_esi);
void sumar_retardo(void* pcbb);
void sumar_retardo_otros_ready();
void procesar_motivo_aborto(int protocolo);
void cerrar_cosas_de_un_esi(void* pcb_esi);
void destruir_pcb(void* pcb);
void eliminar_esi_en_espera(void* pcbb);
void borrar_nodo_clave(void* clave);
void eliminar_esi_en_espera(void* esi);
void free_esi_finalizado(void* id);
void cerrar_planificador();
void despedir_esi(void* pcb);
void quitar_primer_esi_de_cola_bloqueados(char* clave);

//////////-----HABLAR CON COORDINADOR-----//////////

void manejar_coordinador();
void procesar_pedido(t_esi_operacion instruccion);
void mostrar_status_clave(status_clave status);
void recibir_status_clave();
void informar_coordi_kill(int id_Esi);

//////////MENSAJES CON COORDINADOR//////////

//ENVIAR COORDINADOR
void informar_aborto_coordinador_clave_no_id();
void informar_aborto_coordinador_clave_no_b();
void informar_exito_coordinador();
void informar_bloqueo_coordinador();



//////////MENSAJES CON ESIS//////////

#endif /* PLANIFICADOR_FUNCIONES_H_ */
