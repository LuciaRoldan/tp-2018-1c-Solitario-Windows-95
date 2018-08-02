#include "planificador_punto_hache.h"

//--Encontrar el ID de un ESI en las PCBs--//
bool ids_iguales_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	return pcb_esi->id == id_buscado;
}

//--Sumar 1 al retardo de los demas ESIs ready--//
void sumar_retardo_otros_ready(){
	//pthread_mutex_lock(&m_lista_esis_ready);
	list_iterate(esis_ready, sumar_retardo_menos_primero);
	//pthread_mutex_unlock(&m_lista_esis_ready);

}
void sumar_retardo_menos_primero(void* pcbb){
	pcb* pcb_esi = pcbb;
	pcb* el_primer_esi = list_get(esis_ready, 0);
	if (el_primer_esi->id != pcb_esi->id){
		pcb_esi->retardo+=1;
	}
}
void sumar_retardo(void* pcbb){
	pcb* pcb = pcbb;
	pcb->retardo+=1;
}

bool es_el_primer_esi_ready(void* pcbb){
	pcb* pcb_esi_ready = pcbb;
	return pcb_esi_ready->id == id_esi_ejecutando;
}
bool no_es_el_primer_esi_ready(void *pcbb){
	pcb* pcb_esi_ready = pcbb;
	return pcb_esi_ready->id != id_esi_ejecutando;
}

//--Mostrar estimaciones--//
void mostrar_estimacion(void* pcbb){
	pcb* esi = pcbb;
	if(strcmp(algoritmo, "HRRN") == 0){
		float tiempo_de_respuesta = (esi->retardo + esi->ultimaEstimacion) / esi->ultimaEstimacion;
		log_info(logger, "El W del ESI %d es: %f", esi->id, esi->retardo);
		log_info(logger, "El S del ESI %d es: %f", esi->id, esi->ultimaEstimacion);
		log_info(logger, "El tiempo de respuesta del esi %d es: %f", esi->id, tiempo_de_respuesta);
	} else {
		log_info(logger, "La estimacion del esi %d es: %f", esi->id, esi->ultimaEstimacion);
	}
}

//--Encontrar una clave en una lista--//
bool claves_iguales_nodo_clave(void* nodo_clave){
	clave_bloqueada* una_clave = (clave_bloqueada*) nodo_clave;

	if(strcmp(una_clave->clave, clave_buscada) == 0){
		return true;
	} else {
		return false;
	}
}

//--Buscar ESI en una cola de esis bloqueados--//
bool ids_iguales_cola_de_esis(void* id){
	int* id_esi = (int*) id;
	return *id_esi == id_buscado;
}

//--Mostrar la ultima estimacion de los esis--// //puede q vuele
void mostrar_ultima_estimacion(void* pcbb){
	pcb* pcb_esi = pcbb;
	calcular_estimacion(pcb_esi);
	log_info(logger, "La ultima estimacion de ESI %d es: %d", pcb_esi->id, pcb_esi->ultimaEstimacion);
}

//--Funcioncitas para liberar memoria etc cuando se cierran cosas--//

void free_esi_finalizado(void* id){
	int* id_finalizado = id;
	free(id_finalizado);
}

void despedir_esi(void* esi){
	pcb* pcb_esi = esi;
	void* envio = malloc(sizeof(int));
	serializar_id(envio, 85);
	enviar(pcb_esi->socket, envio, sizeof(int), logger);
	free(envio);
}

void borrar_nodo_clave(void* clave){
	clave_bloqueada* clave_bloq = clave;
	free(clave_bloq->clave);
	list_iterate(clave_bloq->esis_en_espera, eliminar_esi_en_espera);
	free(clave_bloq);
}

void eliminar_esi_en_espera(void* esi){
	int* esi_en_espera = esi;
	free(esi_en_espera);
}

//--Destruir PCB y liberar memoria--//
void destruir_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	free(pcb_esi);
}

void despedir_esi_vivo(void* pcbb){
	pcb* pcb_esi = pcbb;
	//pthread_mutex_lock(&m_id_buscado);
	id_buscado = pcb_esi->id;
	if(!list_any_satisfy(esis_finalizados, ids_iguales_pcb)){
		enviar_esi_exit(pcb_esi->socket);
		abortar_esi(pcb_esi->id);
	}
	//pthread_mutex_unlock(&m_id_buscado);
}

void liberar_claves_tomadas_por_finalizado(void* clave){
	clave_bloqueada* clave_bloq = clave;
	if(clave_bloq->esi_que_la_usa == id_buscado){
		liberar_clave(clave_bloq->clave);
	}
}
