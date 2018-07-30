#include "planificador_punto_hache.h"

/////-----OPERACIONES SOBRE PCBS-----/////

//--Crear PCB--//
pcb* crear_pcb_esi(int socket_esi_nuevo, int id_esi, pthread_t hilo_esi){
	pcb* pcb_esi;
	pcb_esi = malloc(sizeof(pcb));
	pcb_esi->id = id_esi;
	pcb_esi->socket = socket_esi_nuevo;
	pcb_esi->ultimaEstimacion = estimacion_inicial;
	pcb_esi->ultimaRafaga = estimacion_inicial;
	pcb_esi->retardo = 0;
	pcb_esi->hilo = hilo_esi;
	return pcb_esi;
}

/////-----MOVIENDO ESIS-----/////

//--Mover ESI a cola de bloqueados de una clave--//
void mover_esi_a_bloqueados(char* clave, int esi_id){
	pthread_mutex_lock(&m_clave_buscada);
	clave_buscada = clave;

	clave_bloqueada* nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	pthread_mutex_unlock(&m_clave_buscada);
	int* id = malloc(sizeof(int));
	memcpy(id, &esi_id, sizeof(int));
	list_add(nodo_clave_buscada->esis_en_espera, id);

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = esi_id;
	void* one_pcb = list_remove_by_condition(esis_ready, ids_iguales_cola_de_esis);
	pcb* esi_bloqueado = one_pcb;

	actualizar_rafaga_y_estimar(esi_bloqueado);

	log_info(logger, "Esi %d colocado en cola de espera de la clave %s", esi_id, clave_buscada);
	log_info(logger, "La cantidad de ESIs ready es: %d", list_size(esis_ready));

	pthread_mutex_unlock(&m_id_buscado);
	pthread_mutex_unlock(&m_lista_esis_ready); //arranca en mover a bloqueados
}

//--Abortar ESI--//
void abortar_esi(int id_esi){
	pcb* esi_abortado;

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_abortado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_abortado = esi_abortado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	log_info(logger, "ESI abortado: %d", esi_abortado->id);

	list_remove_by_condition(esis_ready, ids_iguales_pcb);

	actualizar_rafaga_y_estimar(esi_abortado);

	pthread_mutex_unlock(&m_id_buscado);

	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_abortado, sizeof(int));
	list_add(esis_finalizados, id);
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_abortado);

	if(fin_de_programa != 1){
	enviar_esi_kill(esi_abortado->socket); //va?
	}

	cerrar_cosas_de_un_esi(esi_abortado);
}

//--Mover ESI a finalizados--//
void mover_esi_a_finalizados(int id_esi){
	pcb* esi_finalizado;

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_finalizado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_finalizado = esi_finalizado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	log_info(logger, "ESI finalizado: %d", esi_finalizado->id);

	pthread_mutex_lock(&m_lista_esis_ready);
	list_remove_by_condition(esis_ready, ids_iguales_pcb);
	pthread_mutex_unlock(&m_lista_esis_ready);

	actualizar_rafaga_y_estimar(esi_finalizado);

	pthread_mutex_unlock(&m_id_buscado);

	id_buscado = id_esi_finalizado;
	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_finalizado, sizeof(int));
	list_add(esis_finalizados, id);
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_finalizado);

	cerrar_cosas_de_un_esi(esi_finalizado);
}

/////-----OPERACIONES SOBRE CLAVE_BLOQUEADA-----/////
void liberar_clave(char* clave){
	//hacer el lock ANTES de llamar a esta funcion

	if(!list_is_empty(claves_bloqueadas)){
		log_info(logger, "pase el primer if");
		pthread_mutex_lock(&m_lista_claves_bloqueadas);
		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = clave;
		if(list_any_satisfy(claves_bloqueadas, claves_iguales_nodo_clave)){

			clave_bloqueada* nodo_clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
			pthread_mutex_unlock(&m_lista_claves_bloqueadas);
			pthread_mutex_unlock(&m_clave_buscada);

			log_info(logger, "El ESI que ocupaba la clave era: %d", nodo_clave->esi_que_la_usa);

			nodo_clave->esi_que_la_usa = 0;
			log_info(logger, "La clave %s se libero", clave);

			if(!list_is_empty(nodo_clave->esis_en_espera)){
				void* el_esi = list_remove(nodo_clave->esis_en_espera, 0);
				int* id_esi_ahora_ready = el_esi;
				nodo_clave->esi_que_la_usa = *id_esi_ahora_ready;

				pthread_mutex_lock(&m_id_buscado);
				id_buscado = *id_esi_ahora_ready;
				pthread_mutex_unlock(&m_id_buscado);
				log_info(logger, "y es ahora ocupada por el ESI %d", nodo_clave->esi_que_la_usa);
				void* un_esi = list_find(pcbs, ids_iguales_pcb);
				pcb* el_nuevo_esi_ready = un_esi;

				el_nuevo_esi_ready->ultimaRafaga = 0; //apa

				list_add(esis_ready, el_nuevo_esi_ready);

				vino_uno = 1;

				if(list_size(esis_ready) == 1){
					sem_post(&s_planificar);
				}
			}

		} else {
			pthread_mutex_lock(&m_lista_claves_bloqueadas);
			pthread_mutex_lock(&m_clave_buscada);
		}
	}
}

//--Sacar ESI de la cola de bloqueados de una clave--//
void quitar_esi_de_cola_bloqueados(void* clave_bloq){
	clave_bloqueada* clave = clave_bloq;
	if(!list_is_empty(clave->esis_en_espera)){
		if(list_find(clave->esis_en_espera, ids_iguales_cola_de_esis) != NULL){
			list_remove_by_condition(clave->esis_en_espera, ids_iguales_cola_de_esis);
		}
	}
}


