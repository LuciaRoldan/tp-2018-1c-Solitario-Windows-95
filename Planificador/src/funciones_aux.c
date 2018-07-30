#include "planificador_punto_hache.h"

///---PLANIFICAR---///
void planificar_si_corresponde(){
	ordenar_pcbs();
}

void actualizar_rafaga_si_hubo_desalojo(pcb* esi_a_ejecutar){
	pthread_mutex_lock(&m_id_esi_ejecutando);
	pthread_mutex_lock(&m_id_buscado);

	if((esi_a_ejecutar->id != id_esi_ejecutando) && (id_esi_ejecutando != -1)){
		id_buscado = id_esi_ejecutando;

		if (list_any_satisfy(esis_ready, ids_iguales_pcb)){ //hubo desalojo pq sigue en ready
			void* esii = list_find(esis_ready, ids_iguales_pcb);
			pcb* esi_desalojado = esii;
			esi_desalojado->ultimaRafaga += rafaga_actual;
			rafaga_actual = 0;
			log_info(logger, "Actualice la ultimaRafaga del Esi %d a: %d", esi_desalojado->id, esi_desalojado->ultimaRafaga);
			//if(desalojo == 1){
			//calcular_estimacion(esi_desalojado);
			//log_info(logger, "La nueva estimacion del esi desalojado %d es: %f", esi_desalojado->id, esi_desalojado->ultimaEstimacion);
			//}
		}
	}
	pthread_mutex_unlock(&m_id_buscado);
	pthread_mutex_unlock(&m_id_esi_ejecutando);
}



//--Procesar instruccion del Coordinador--//
void procesar_pedido(t_esi_operacion instruccion){

	clave_bloqueada* nodo_clave_buscada;
	pcb* pcb_pedido_esi;

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi_ejecutando;
	pcb_pedido_esi = list_find(pcbs, ids_iguales_pcb);
	pthread_mutex_unlock(&m_id_buscado);

	if (pcb_pedido_esi != NULL){ //VERIFICO QUE EL ESI SEA VALIDO

	switch(instruccion.keyword){
	case(GET):

		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.GET.clave;
		//memcpy(clave_buscada, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);

		if(nodo_clave_buscada == NULL){
			//No existe el elemento clave_bloqueada para esa clave en mi lista de claves bloqueadas, la creo
			clave_bloqueada* clave_nueva = malloc(sizeof(clave_bloqueada));

			clave_buscada = instruccion.argumentos.GET.clave;

			clave_nueva->esi_que_la_usa = pcb_pedido_esi->id;

			char* clave_esi_nueva = malloc(strlen(instruccion.argumentos.GET.clave)+1);
			memcpy(clave_esi_nueva, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
			clave_nueva->clave = clave_esi_nueva;
			clave_nueva->esis_en_espera = list_create();

			pthread_mutex_lock(&m_lista_claves_bloqueadas);
			list_add(claves_bloqueadas, clave_nueva);
			pthread_mutex_unlock(&m_lista_claves_bloqueadas);
			log_info(logger, "Nueva clave_bloqueada creada %s para el ESI %d", clave_buscada, clave_nueva->esi_que_la_usa);
			informar_exito_coordinador();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					//Existe esa clave_bloqueada en mi lista de claves pero no esta asignada a ningun ESI
					pthread_mutex_lock(&m_lista_claves_bloqueadas);

					nodo_clave_buscada->esi_que_la_usa = id_esi_ejecutando;

					pthread_mutex_unlock(&m_lista_claves_bloqueadas);
					log_info(logger, "Clave %s asignada al ESI %d", clave_buscada, id_esi_ejecutando);
					informar_exito_coordinador();
				}

				else {
					if(nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
					//Me hacen un GET sobre una clave que ya tenia asignado ese ESI entonces no hago nada.
					log_info(logger, "GET realizado por el ESI %d sobre la clave %s que ya le pertenecia", id_esi_ejecutando, clave_buscada);
					informar_exito_coordinador();
					}

					else {
						//Me hacen un GET sobre una clave que estaba asignada a otro ESI entonces lo pongo en
						//la lista de espera de esa clave y lo saco de ready
						log_info(logger, "GET realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						//clave_buscada se mantiene igual para que la use la funcion de mover_esi_a_bloqueados cuando recibo respuesta del esi!!
						informar_bloqueo_coordinador();
					}
				}
			}
		//free(instruccion.argumentos.GET.clave);
			break;

	case(SET):

		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.SET.clave;
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);
		log_info(logger, "Nodo_clave_buscada en SET el id del esi que la usa es: %d", nodo_clave_buscada->esi_que_la_usa);

		if(nodo_clave_buscada == NULL){
				log_info(logger, "SET realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, id_esi_ejecutando);
				informar_aborto_coordinador_clave_no_id();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					log_info(logger, "SET realizado por el ESI %d sobre una clave %s sin ESIS asignados", id_esi_ejecutando, clave_buscada);
					informar_aborto_coordinador_clave_no_b();
				}

				else {
					if (nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
						log_info(logger, "SET realizado por el ESI %d sobre la clave que le pertenecia %s", id_esi_ejecutando, clave_buscada);
						informar_exito_coordinador();
					}

					else {
						log_info(logger, "SET realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						informar_aborto_coordinador_clave_no_b();
					}
				}
			}
		//free(instruccion.argumentos.SET.clave);
		//free(instruccion.argumentos.SET.valor);
			break;

	case(STORE):
		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.STORE.clave;
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);

			if(nodo_clave_buscada == NULL){
				log_info(logger, "STORE realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, id_esi_ejecutando);
				informar_aborto_coordinador_clave_no_id();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					log_info(logger, "STORE realizado por el ESI %d sobre una clave %s sin ESIS asignados", id_esi_ejecutando, clave_buscada);
					informar_aborto_coordinador_clave_no_b();
				}

				else {
					if (nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
						log_info(logger, "STORE realizado por el ESI %d sobre la clave que le pertenecia %s", id_esi_ejecutando, clave_buscada);
						liberar_clave(clave_buscada);
						informar_exito_coordinador();
					}

					else {
						log_info(logger, "STORE realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						informar_aborto_coordinador_clave_no_b();
					}
				}
			}
	break;
	}
	//free(instruccion.argumentos.STORE.clave);
	} else {
		log_info(logger, "Pedido invalido. No conozco al ESI %d", id_esi_ejecutando);
	}
}

actualizar_rafaga_y_estimar(pcb* pcb_esi){
	pcb_esi->ultimaRafaga += rafaga_actual;
	log_info(logger, "Actualice la ultimaRafaga del Esi %d a: %d", pcb_esi->id, pcb_esi->ultimaRafaga);
	calcular_estimacion(pcb_esi);
	rafaga_actual = 0;
	se_fue_uno = 1; //para planificar sin desalojo
}


//--Informar motivo aborto ESI--//
void procesar_motivo_aborto(int protocolo){
	switch (protocolo){
	case(86):
		log_info(logger, "ESI abortado por fallo en el tamanio de la clave");
		break;
	case(87):
		log_info(logger, "ESI abortado por clave no identificada");
		break;
	case(88):
		log_info(logger, "ESI abortado por clave inaccesible");
		break;
	case (89):
		log_info(logger, "ESI abortado por clave no bloqueada");
		break;
	default:
		log_info(logger, "ESI abortado por ESI desconocido");
		break;
	}
}


//--Liberar instruccion. No la uso--//
void liberar_instruccion(t_esi_operacion instruccion){
	switch(instruccion.keyword){
	case GET:
		free(instruccion.argumentos.GET.clave);
		break;
	case SET:
		free(instruccion.argumentos.SET.clave);
		free(instruccion.argumentos.SET.valor);
		break;
	case STORE:
		free(instruccion.argumentos.STORE.clave);
		break;
	}
}
