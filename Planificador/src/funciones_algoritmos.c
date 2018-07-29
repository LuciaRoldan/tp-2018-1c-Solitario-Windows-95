#include "funciones_plani.h"

///---FUNCIONES PLANIFICACION---///

//--ORDENAR PCBS--//
void ordenar_pcbs(){
	pthread_mutex_lock(&m_lista_esis_ready);
	if(strcmp(algoritmo, "SJF_CD") == 0){
	planificacionSJF_CD();
	}
	else if(strcmp(algoritmo, "SJF_SD") == 0){
	planificacionSJF_SD();
	}
	else if(strcmp(algoritmo, "HRRN") == 0){
	planificacionHRRN();
	}
	else{
	log_info(logger, "Algoritmo invalido en ordenar_pcbs");
	}
	pthread_mutex_unlock(&m_lista_esis_ready);
}


void calcular_estimacion(pcb* una_pcb){
	if(strcmp(algoritmo, "SJF_CD") == 0){
		calcular_estimacion_SJF(una_pcb);
	}
		else if(strcmp(algoritmo, "SJF_SD") == 0){
		calcular_estimacion_SJF(una_pcb);
	}
		else if(strcmp(algoritmo, "HRRN") == 0){
		calcular_estimacion_HRRN(una_pcb);
	}
		else{
		log_info(logger, "Algoritmo invalido en ordenar_pcbs");
	}
}


//--Planificaciones--//

//-Con desalojo-//
void planificacionSJF_CD(){
	log_info(logger, "Estoy en SJF_CD. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		list_iterate(esis_ready, mostrar_estimacion);
		list_sort(esis_ready, algoritmo_SJF_CD);
	}
}

//-Sin desalojo-//
void planificacionSJF_SD(){
	//log_info(logger, "Estoy en SJF_SD. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1 && (se_fue_uno == 1 || vino_uno == 1)){
		log_info(logger, "Voy a ordenar. La cantidad de esis ready es %d", list_size(esis_ready));
		//list_iterate(esis_ready, calcular_estimacion);
		list_iterate(esis_ready, mostrar_estimacion);
		list_sort(esis_ready, algoritmo_SJF_SD);
		se_fue_uno = 0;
		vino_uno = 0;
	}
}

void planificacionHRRN(){
	//log_info(logger, "Estoy en HRRN. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1 && (se_fue_uno == 1 || vino_uno == 1)){
		//list_iterate(esis_ready, calcular_estimacion);
		log_info(logger, "Voy a ordenar. La cantidad de esis ready es %d", list_size(esis_ready));
		list_iterate(esis_ready, mostrar_estimacion);
		list_sort(esis_ready, algoritmo_HRRN);
		se_fue_uno = 0;
		vino_uno = 0;
	}
}

//--Calcular estimaciones--//

void calcular_estimacion_SJF(pcb* pcb_esi){
	float proxima_rafaga =  (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	log_info(logger, "La ultimaRafaga del esi %d es: %d", pcb_esi->id, pcb_esi->ultimaRafaga);
	log_info(logger, "La ultima estimacion del esi %d es: %f", pcb_esi->id, pcb_esi->ultimaEstimacion);
	pcb_esi->ultimaEstimacion = proxima_rafaga;
	log_info(logger, "La estimacion del ESI %d es: %f", pcb_esi->id, proxima_rafaga);
}

void calcular_estimacion_HRRN(pcb* pcb_esi){
	float estimacion = (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	float tiempo_de_respuesta = (pcb_esi->retardo + estimacion) / estimacion;
	log_info(logger, "El W del ESI %d es: %f", pcb_esi->id, pcb_esi->retardo);
	log_info(logger, "El S del ESI %d es: %f", pcb_esi->id, pcb_esi->ultimaEstimacion);
	pcb_esi->ultimaEstimacion = estimacion;
	log_info(logger, "La nueva estimacion del ESI %d es: %f", pcb_esi->id, pcb_esi->ultimaEstimacion);
	log_info(logger, "El tiempo de respuesta del ESI %d es: %f", pcb_esi->id, tiempo_de_respuesta);
}

//--Algoritmos para ordenar---//

bool algoritmo_SJF_CD(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	if ( pcb1->ultimaEstimacion <= pcb2->ultimaEstimacion){
		return true;
	} else {
		return false;
	}
}

bool algoritmo_SJF_SD(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	if(es_el_primer_esi_ready(pcb1)){
		return true;
	} else if (es_el_primer_esi_ready(pcb2)){
		return false;
	} else {

		if ( pcb1->ultimaEstimacion <= pcb2->ultimaEstimacion){
			return true;
		} else {
			return false;
		}
	}
}


bool algoritmo_HRRN(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	float tiempo_de_respuesta1 = (pcb1->retardo + pcb1->ultimaEstimacion) / pcb1->ultimaEstimacion;
	float tiempo_de_respuesta2 = (pcb2->retardo + pcb2->ultimaEstimacion) / pcb2->ultimaEstimacion;


	if(es_el_primer_esi_ready(pcb1)){
		return true;
	} else if (es_el_primer_esi_ready(pcb2)){
		return false;
	} else {

		if(tiempo_de_respuesta1 >= tiempo_de_respuesta2){
			return true;
		} else {
			return false;
		}
	}

}

