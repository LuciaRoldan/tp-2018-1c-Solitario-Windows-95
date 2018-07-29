#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

#include "planificador_punto_hache.h"

#include "commons_propias/commons_propias.h"
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


//--ESTRUCTURAS--//

typedef struct{ //estructura que va a contener todas las claves y el estado de cada una
	char* clave;
	int esi_que_la_usa; //ESI que la esta usando ahora. O sea que hizo un GET.
	t_list* esis_en_espera; //ESIS bloqueados esperando a que se desbloquee la clave.
} clave_bloqueada;

typedef struct{
	int socket_coordinador;
	int socket_esis;
} sockets;

//Tipo operaciones de consola
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

//////////-----VARIABLES GLOBALES-----//////////

sockets sockets_planificador;
t_conexion conexion_planificador;
t_conexion conexion_coordinador;
t_log * logger;
char* algoritmo;
int estimacion_inicial;
float alpha;

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
pthread_t* hilo_a_cerrar;
int hay_hilos_por_cerrar;
int pausar_planificador;
int terminar_todo;
int fin_de_programa;
int rafaga_actual;
int se_fue_uno;
int vino_uno;
int desalojo;

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
pthread_mutex_t m_estoy_mandando_a_ejecutar;

sem_t s_cerrar_un_hilo;
sem_t s_hilo_cerrado;
sem_t s_eliminar_pcb;

sem_t s_podes_procesar_un_pedido;
sem_t s_podes_procesar_una_respuesta;
sem_t s_planificar;

//////////----------FUNCIONES----------//////////


/////-----CONSOLA-----/////
void ejecutar_consola();
op_consola analizar_linea(char* linea);
char**  string_to_array(char* text, char* separator);


/////-----FUNCIONES CONSOLA-----/////
void pausar_planificacion();
void continuar_planificacion();
void bloquear(char * clave, int id);
void desbloquear(char * clave);
void listar_procesos_encolados(char* recurso);
void kill_esi(int id);
void pedir_status(char* clave);
void deadlock();

//--Funciones auxiliares consola--//
void enviar_esi_kill(int socket_esi);
void recibir_status_clave();
void mostrar_status_clave(status_clave status);
void imprimir_id_esi(void* esi);
//para deadlock
bool el_esi_espera_la_clave(int esi_que_la_usa, clave_bloqueada* nodo_clave);
bool ids_iguales_ints(void* id1);
bool el_esi_la_esta_esperando(void* clave);
bool el_esi_la_espera(void* clave);
bool el_duenio_esta_en_deadlock(clave_bloqueada* nodo_clave);



/////-----FUNCIONES ALGORITMOS-----/////
void ordenar_pcbs();
void calcular_estimacion(pcb* una_pcb);

void planificacionSJF_CD();
void planificacionSJF_SD();
void planificacionHRRN();

void calcular_estimacion_SJF(pcb* una_pcb);
void calcular_estimacion_HRRN(pcb* una_pcb);

bool algoritmo_SJF_SD(void* pcb1, void* pcb2);
bool algoritmo_SJF_CD(void* pcb_1, void* pcb_2);
bool algoritmo_HRRN(void* pcb1, void* pcb2);


/////-----FUNCIONES AUXILIARES LISTAS-----/////
void sumar_retardo(void* pcbb);
void sumar_retardo_otros_ready();
void mostrar_ultima_estimacion(void* pcbb);
void mostrar_estimacion(void* pcbb);
void sumar_retardo_menos_primero(void* pcbb);

bool ids_iguales_pcb(void* pcbb);
bool es_el_primer_esi_ready(void *pcbb);
bool no_es_el_primer_esi_ready(void *pcbb);
bool claves_iguales_nodo_clave(void* nodo_clave);
bool ids_iguales_cola_de_esis(void* id);

//para liberar memoria
void free_esi_finalizado(void* id);
void despedir_esi(void* pcb);
void eliminar_esi_en_espera(void* pcbb);
void borrar_nodo_clave(void* clave);
void destruir_pcb(void* pcb);


/////-----FUNCIONES AUXILIARES-----/////
void actualizar_rafaga_si_hubo_desalojo(pcb* esi_a_ejecutar);
void planificar_si_corresponde();
void procesar_pedido(t_esi_operacion instruccion);
void actualizar_rafaga_y_estimar(pcb* pcbb);
void procesar_motivo_aborto(int protocolo);
void liberar_instruccion(t_esi_operacion instruccion);


/////-----FUNCIONES ENVIO MENSAJES-----/////
//enviar
void enviar_solicitud_ejecucion(pcb* pcbb);
void informar_aborto_coordinador_clave_no_id();
void informar_aborto_coordinador_clave_no_b();
void informar_exito_coordinador();
void informar_bloqueo_coordinador();
void informar_coordi_kill(int id_Esi);
//recibir
int recibir_un_int(int socket);


/////-----FUNCIONES ESIS-----/////
pcb* crear_pcb_esi(int socket_cliente, int id_esi, pthread_t hilo_esi);
void mover_esi_a_bloqueados(char* clave_buscada, int esi_id);
void abortar_esi(int id_esi);
void mover_esi_a_finalizados(int id_esi);
void liberar_clave(char* clave);
void quitar_esi_de_cola_bloqueados(void* clave_bloq);


/////-----FUNCIONES INICIALIZAR Y CERRAR COSAS-----/////
//inicializar
int handshake_esi(int socket_esi);
void handshake_coordinador(int socket_coordinador);
sockets inicializar_planificador(char* path);
void leer_archivo_configuracion(char* path);
void conectarse_al_coordinador(int socket_coordinador);
void inicializar_semaforos();
//cerrar
void desencadenar_cerrar_planificador();
void cerrar_cosas_de_un_esi(void* pcb_esi);
void cerrar_planificador();


/////-----FUNCIONES PRINCIPALES-----/////
void recibir_esis(void* socket_esis);
void manejar_esis();
void manejar_esi(void* pcb_esi);
void manejar_coordinador();




void quitar_primer_esi_de_cola_bloqueados(char* clave);

#endif /* PLANIFICADOR_FUNCIONES_H_ */
