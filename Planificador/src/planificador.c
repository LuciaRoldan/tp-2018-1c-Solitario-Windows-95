#include "funciones_plani.h"
#include <pthread.h>

/* COSAS POR HACER
 *
 *
 *
 */

int main(int argc, char* argv[]) {

	//Crear listas globales
		pcbs = list_create();
		esis_ready = list_create();
		esis_finalizados = list_create();
		claves_bloqueadas = list_create();
		clave_buscada = malloc(40);
		hay_hilos_por_cerrar = 0;
		pausar_planificador = 0;
		terminar_todo = 1;
		fin_de_programa = -1;
		rafaga_actual = 0;
		id_esi_ejecutando = -1;
		se_fue_uno = 0;
		vino_uno = 0;

	logger = log_create("planificador.log", "PLANIFICADOR", 1, LOG_LEVEL_INFO);
	sockets_planificador = inicializar_planificador(argv[1]); //leyendo archivo configuracion

	if(strcmp(algoritmo, "SJF_CD") == 0){
		desalojo = 1;
	}

	pthread_t hilo_escucha_esis;
	pthread_t hilo_coordinador;
	pthread_t hilo_manejar_esis;
	pthread_t hilo_consola;

	void* puntero_socket_esis = &sockets_planificador.socket_esis;
	void* puntero_socket_coordinador = &sockets_planificador.socket_coordinador;

	log_info(logger, "Por entrar al hilo del Esi y el socket_esis es %d\n", sockets_planificador.socket_esis);
	pthread_create(&hilo_escucha_esis, 0, recibir_esis, puntero_socket_esis);
	pthread_create(&hilo_manejar_esis, 0, manejar_esis, NULL);
	pthread_create(&hilo_coordinador, 0, manejar_coordinador, puntero_socket_coordinador);
	pthread_create(&hilo_consola, 0, ejecutar_consola, (void*) 0);

	//CIERRO HILOS
			while(hay_hilos_por_cerrar>0 || fin_de_programa<0){
				sem_wait(&s_cerrar_un_hilo);
				log_info(logger, "Vine a cerrar el hilo");
				hay_hilos_por_cerrar = 0;
				log_info(logger, "Hilo cerrado");
				sem_post(&s_hilo_cerrado);
				pthread_join(*hilo_a_cerrar, NULL);
				//hay_hilos_por_cerrar = 0;
				//pthread_mutex_unlock(&m_hilo_a_cerrar);
				//sem_post(&s_eliminar_pcb);
			}

//			while(!terminar_programa){
//						sem_wait(&s_cerrar_hilo);
//						if(!terminar_programa){
//						pthread_join(*hilo_a_cerrar, NULL);
//						log_info(logger, "Hilo cerrado");
//						pthread_mutex_unlock(&m_hilo_a_cerrar);
//						}
//				}

	//pthread_join(hilo_escucha_esis, NULL);
	pthread_join(&hilo_coordinador, NULL);
	//pthread_join(&hilo_consola, NULL);
	return 0;
}
