#include "funciones_plani.h"
#include <pthread.h>

/* COSAS POR HACER
 *
 *
 *
 */

int main() {

	logger = log_create("planificador.log", "PLANIFICADOR", 1, LOG_LEVEL_INFO);
	sockets_planificador = inicializar_planificador(); //leyendo archivo configuracion

	pthread_t hilo_escucha_esis;
	pthread_t hilo_coordinador;
	pthread_t hilo_manejar_esis;
	pthread_t hilo_consola;


	//Crear listas globales
	pcbs = list_create();
	esis_ready = list_create();
	esis_finalizados = list_create();
	claves_bloqueadas = list_create();
	clave_buscada = malloc(40);
	hay_hilos_por_cerrar = 0;
	pausar_planificador = 0;


	void* puntero_socket_esis = &sockets_planificador.socket_esis;
	void* puntero_socket_coordinador = &sockets_planificador.socket_coordinador;

	log_info(logger, "Por entrar al hilo del Esi y el socket_esis es %d\n", sockets_planificador.socket_esis);
	pthread_create(&hilo_escucha_esis, 0, recibir_esis, puntero_socket_esis);
	pthread_create(&hilo_manejar_esis, 0, manejar_esis, NULL);
	pthread_create(&hilo_coordinador, 0, manejar_coordinador, puntero_socket_coordinador);
	pthread_create(&hilo_consola, 0, ejecutar_consola, (void*) 0);

	//CIERRO HILOS
			if(hay_hilos_por_cerrar){
				sem_wait(&s_cerrar_un_hilo);
				log_info(logger, "Vine a cerrar el hilo");
				pthread_join(*hilo_a_cerrar, NULL);
				log_info(logger, "Hilo cerrado");
				hay_hilos_por_cerrar = 0;
				pthread_mutex_unlock(&m_hilo_a_cerrar);
				sem_post(&s_hilo_cerrado);
				sem_post(&s_eliminar_pcb);
			}

	//pthread_join(hilo_escucha_esis, NULL);
	pthread_join(hilo_coordinador, NULL);
	return 0;
}
