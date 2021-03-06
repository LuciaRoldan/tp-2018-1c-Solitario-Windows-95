#include <pthread.h>

#include "planificador_punto_hache.h"

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
		pausar_planificador = 1;
		terminar_todo = 1;
		fin_de_programa = -1;
		rafaga_actual = 0;
		id_esi_ejecutando = -1;
		se_fue_uno = 0;
		vino_uno = 0;
		se_cerro_todo = -1;
		esi_a_finalizar = -1;
		cerrar_hilos = -1;

	logger = log_create("planificador.log", "PLANIFICADOR", 0, LOG_LEVEL_INFO);
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

	pthread_create(&hilo_escucha_esis, 0, recibir_esis, puntero_socket_esis);
	pthread_create(&hilo_manejar_esis, 0, manejar_esis, NULL);
	pthread_create(&hilo_coordinador, 0, manejar_coordinador, puntero_socket_coordinador);
	pthread_create(&hilo_consola, 0, ejecutar_consola, (void*) 0);

	//CIERRO HILOS
	//hay_hilos_por_cerrar > 0 || fin_de_programa < 0 || se_cerro_todo < 0
			while(1){
				sem_wait(&s_cerrar_un_hilo);
				hay_hilos_por_cerrar = 0;
				log_info(logger, "Hilo cerrado");
				if (terminar_todo == -1){
					sem_post(&s_esi_despedido);
				}
				sleep(1);
				pthread_join(*hilo_a_cerrar, NULL);
				if(se_cerro_todo == 1){
					break;
				}
			}

//			while(!terminar_programa){
//						sem_wait(&s_cerrar_hilo);
//						if(!terminar_programa){
//						pthread_join(*hilo_a_cerrar, NULL);
//						log_info(logger, "Hilo cerrado");
//						pthread_mutex_unlock(&m_hilo_a_cerrar);
//						}
//				}

	pthread_join(&hilo_escucha_esis, NULL);
	pthread_join(&hilo_coordinador, NULL);
	pthread_join(&hilo_consola, NULL);
	pthread_join(&hilo_manejar_esis, NULL);
	return 0;
}
