#include "funciones_plani.h"

// CONSOLA
void ejecutar_consola(){
	char * linea;
	char * clave;
	char* recurso;
	int el_id;

	while(1) {
		linea = readline("\n > Ingrese un comando: ");
		if(!string_is_empty(linea)){
			add_history(linea);
			printf("---> La linea ingresada fue: %s\n", linea);
			string_to_lower(linea);

			if(!strncmp(linea, "exit", 4)) {
				printf("Cerrando Consola. Hasta luego. \n");
				free(linea);
				desencadenar_cerrar_planificador();
				exit(1);
				break;
			}

			op_consola operacion = analizar_linea(linea);
			char **palabras = string_to_array(linea, " ");

			switch(operacion){
			case BLOQUEAR:
				if(palabras[1] != NULL && palabras[2] != NULL){
					if(palabras[3]==NULL){
						printf("Usted quiere bloquear un ESI.\n");
						printf("Operacion: %s ---- ", palabras[0]);
						printf("Clave: %s ---- ", palabras[1]);
						printf("ID: %s ---- ", palabras[2]);
						clave = malloc(strlen(palabras[1]));
						strcpy(clave, palabras[1]+1);
						el_id = atoi(palabras[2]);
						bloquear(clave, el_id);
					} else {
						printf("Demasiados argumentos para la operacion 'bloquear <clave> <id>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave> <id>\n");
				}
				break;
			case DESBLOQUEAR:
				if(palabras[1] != NULL){
					if(palabras[2]==NULL){
					printf("Usted quiere desbloquear un ESI de para una clave.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("Clave: %s ---- ", palabras[1]);
					clave = malloc(strlen(palabras[1])+1);
					strcpy(clave, palabras[1]);
					desbloquear(clave);
					} else {
						printf("Demasiados argumentos para la operacion 'desbloquear <clave>' \n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave> \n");
				}
				break;
			case KILL:
				if(palabras[1] != NULL){
					if(palabras[2]==NULL){
						printf("Usted quiere finalizar un proceso.\n");
						printf("Operacion: %s ---- ", palabras[0]);
						printf("ID: %s\n", palabras[1]);
						el_id = atoi(palabras[1]);
						kill_esi(el_id);
					} else {
						printf("Demasiados argumentos para la operacion 'kill <id>' \n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <id>\n");
				}
				break;
			case PAUSAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere pausar la planificacion.\n");
					printf("Operacion: %s\n", palabras[0]);
					pausar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de pausar.\n");
				}
				break;
			case CONTINUAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere continuar la planificacion.\n");
					printf("Operacion: %s\n", palabras[0]);
					continuar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de continuar.\n");
				}
				break;
			case LISTAR:
				if(palabras[1] != NULL) {
					if(palabras[2]==NULL){
						printf("Usted quiere listar los procesos en cola de espera para un recurso.\n");
						printf("Operacion: %s ---- ", palabras[0]);
						printf("Recurso: %s\n", palabras[1]);
						recurso = palabras[1];
						listar_procesos_encolados(recurso);
					} else {
						printf("Demasiados argumentos para la operacion de 'listar <recurso>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <recurso> \n");
				}
				break;
			case STATUS:
				if(palabras[1] != NULL) {
					if(palabras[2]==NULL){
						printf("Usted quiere ver el estado de una clave.\n");
						printf("Operacion: %s ---- ", palabras[0]);
						printf("Clave: %s\n", palabras[1]);
						clave = palabras[1];
						pedir_status(clave);
					} else {
						printf("Demasiados argumentos para la operacion de 'status <clave>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave>\n");
				}
				break;
			case DEADLOCK:
				if(palabras[1] == NULL) {
					printf("Usted quiere analizar deadlocks.\n");
					printf("Operacion: %s\n", palabras[0]);
					deadlock();
				} else {
					printf("Demasiados argumentos para la operacion de deadlock. \n");
				}
				break;
		   default:
			   printf("Comando no reconocido. Ingrese nuevamente. \n");
			   break;
			}
		}
  }
  free(linea);
  exit(1);
}

op_consola analizar_linea(char* linea){

	if(string_starts_with(linea, "bloquear")) {
		return BLOQUEAR;
	}
	if(string_starts_with(linea, "desbloquear")) {
		return DESBLOQUEAR;
	}
	if(string_starts_with(linea, "kill")){
		return KILL;
	}
	if(string_starts_with(linea, "pausar")){
		return PAUSAR;
	}
	if(string_starts_with(linea, "continuar")){
		return CONTINUAR;
	}
	if(string_starts_with(linea, "listar")){
		return LISTAR;
	}
	if(string_starts_with(linea, "status")){
		return STATUS;
	}
	if(string_starts_with(linea, "deadlock")){
		return DEADLOCK;
	}
	return INVALIDO;
}

char**  string_to_array(char* text, char* separator) {
    char **array_values = string_split(text, separator);
    int i = 0;
    while (array_values[i] != NULL) {
	    string_trim(&(array_values[i]));
	    i++;
    }
    return array_values;
}


//FUNCIONES AUXILIARES CONSOLA//
void pausar_planificacion(){
	pausar_planificador = -1;
}
void continuar_planificacion(){
	pausar_planificador = 1;
}
void bloquear(char* clave, int id){
	mover_esi_a_bloqueados(clave, id);
}
void desbloquear(char * clave){
	quitar_primer_esi_de_cola_bloqueados(clave);
}

void quitar_primer_esi_de_cola_bloqueados(char* clave){
	clave_bloqueada* nodo_clave;

	//si HAY claves bloqueadas
	if(!list_is_empty(claves_bloqueadas)){
		log_info(logger, "pase el primer if");
		clave_buscada = clave;
		pthread_mutex_lock(&m_lista_claves_bloqueadas);
		if(list_any_satisfy(claves_bloqueadas, claves_iguales_nodo_clave)){
			void* aux = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
			nodo_clave = aux;
			if(list_size(nodo_clave->esis_en_espera) > 0){
				pthread_mutex_unlock(&m_lista_claves_bloqueadas);
				log_info(logger, "pase el list find");

				//SACO al primer esi de la lista de bloqueados de esa clave
				void* primero = list_remove(nodo_clave->esis_en_espera, 0);
				int* primer_esi_en_espera = primero;
				log_info(logger, "el id del que removi es: %d", *primer_esi_en_espera);
				pthread_mutex_lock(&m_id_buscado);
				id_buscado = *primer_esi_en_espera;
				void* un_esi = list_find(pcbs, ids_iguales_pcb);
				pthread_mutex_unlock(&m_id_buscado);

				pcb* el_nuevo_esi_ready = un_esi;
				//agrego al esi a ready
				if(desalojo == 1){
				calcular_estimacion(el_nuevo_esi_ready);
				log_info(logger, "La nueva estimacion del esi %d es: %f", el_nuevo_esi_ready->id, el_nuevo_esi_ready->ultimaEstimacion);
				}
				list_add(esis_ready, el_nuevo_esi_ready);
				log_info(logger, "Agregue al esi %d a ready", *primer_esi_en_espera);
				nodo_clave->esi_que_la_usa = *primer_esi_en_espera;
				log_info(logger, "Ahora el esi %d tiene la clave %s", *primer_esi_en_espera, nodo_clave->clave);

				if(list_size(esis_ready) == 1){
					sem_post(&s_planificar);
				}

			} else {
					liberar_clave(clave);
					}
		}
	} else { pthread_mutex_unlock(&m_lista_claves_bloqueadas);
	}
}


void listar_procesos_encolados(char* recurso){
	printf("---> Los ESIs a la espera del recurso %s son: ", recurso);
	clave_buscada = recurso;
	clave_bloqueada* clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	list_iterate(clave->esis_en_espera, imprimir_id_esi);
}
void kill_esi(int id){
	id_buscado = id;
	pcb* pcb_esi = list_find(pcbs, ids_iguales_pcb);
	log_info(logger, "ESI %d sera abortado por funcion 'kill' de consola.", pcb_esi->id);
	informar_coordi_kill(pcb_esi->id);
	enviar_esi_kill(pcb_esi->socket);
	abortar_esi(pcb_esi->id);
}
void pedir_status(char* clave){
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer_pedido_clave = malloc(tamanio_buffer);
	serializar_string(buffer_pedido_clave, clave, 21);
	enviar(sockets_planificador.socket_coordinador, buffer_pedido_clave, tamanio_buffer, logger);
	sleep(2);
	//se recibe la respuesta por otro hilo
}

void deadlock(){
	int cantidad_claves_bloqueadas = list_size(claves_bloqueadas);
	t_list* esis_en_deadlock = list_create();
	while(cantidad_claves_bloqueadas>0){
		clave_bloqueada* nodo_clave = list_get(claves_bloqueadas, cantidad_claves_bloqueadas);
		if(el_duenio_esta_en_deadlock(nodo_clave)){
			list_add(esis_en_deadlock, &nodo_clave->esi_que_la_usa);
		}
		cantidad_claves_bloqueadas--;
	}
	int cantidad_esis_en_deadlock = list_size(esis_en_deadlock);
	while(cantidad_esis_en_deadlock>0){
		int* esi = list_get(esis_en_deadlock, cantidad_esis_en_deadlock);
		log_info(logger, "El esi de ID: %d se encuentra en deadlock", *esi);
		cantidad_esis_en_deadlock--;
	}
}

bool el_duenio_esta_en_deadlock(clave_bloqueada* nodo_clave){
	log_info(logger, "Analizando si el esi de ID %d se encuentra en deadlock", nodo_clave->esi_que_la_usa);
	//Yo se que el esi que la usa tiene una clave, entonces voy a buscar si hay otra clave que
	//ese esi esta esperando, y si hay, me voy a fijar si el esi que la tiene esta esperando esta clave.
	if(list_size(nodo_clave->esis_en_espera)>0){
		log_info(logger, "Hay por lo menos un esi en la cola de espera de la clave que le pertenece al esi %d", nodo_clave->esi_que_la_usa);
		pthread_mutex_lock(&m_id_buscado);
		id_buscado = nodo_clave->esi_que_la_usa;
		if(list_any_satisfy(claves_bloqueadas, el_esi_la_esta_esperando)){
			log_info(logger, "El esi %d se encuentra bloqueado esperando una clave", nodo_clave->esi_que_la_usa);
			clave_bloqueada* bloqueada = list_find(claves_bloqueadas, el_esi_la_espera);
			log_info(logger, "La clave que ese esi espera es: %s", bloqueada->clave);
			pthread_mutex_unlock(&m_id_buscado);
			if(el_esi_espera_la_clave(bloqueada->esi_que_la_usa, nodo_clave)){
				return true;
			}
		}
	}
	pthread_mutex_unlock(&m_id_buscado);
	return false;
}

bool el_esi_la_esta_esperando(void* clave){
	clave_bloqueada* bloqueada = clave;
	return list_any_satisfy(bloqueada->esis_en_espera, ids_iguales_ints);
}

bool el_esi_la_espera(void* clave){
	clave_bloqueada* bloqueada = clave;
	return bloqueada->esi_que_la_usa == id_buscado;
}

bool ids_iguales_ints(void* id1){
	int* id_1 = id1;
	return *id_1 == id_buscado;
}

bool el_esi_espera_la_clave(int esi_que_la_usa, clave_bloqueada* nodo_clave){
	pthread_mutex_lock(&m_id_buscado);
	id_buscado = esi_que_la_usa;
	if(list_any_satisfy(nodo_clave->esis_en_espera, ids_iguales_ints)){
		pthread_mutex_unlock(&m_id_buscado);
		return true;
	} else {
		pthread_mutex_unlock(&m_id_buscado);
		return false;
	}
}

//--Recibir status del Coordinador--//
void recibir_status_clave(){
	status_clave status;
	int tamanio = recibir_int(sockets_planificador.socket_coordinador, logger);
	log_info(logger, "Tamanio recibido del coordinador: %d", tamanio);
	void* buffer = malloc(tamanio);
	recibir(sockets_planificador.socket_coordinador, buffer, tamanio, logger);
	status = deserializar_status_clave(buffer);
	log_info(logger, "Recibido el status_clave de la clave: %s", status.clave);
	mostrar_status_clave(status);
	free(buffer);
}

void imprimir_id_esi(void* esi){
	int id_esi;
	memcpy(&id_esi, esi, sizeof(int));
	printf("%d  ", id_esi);
}

void enviar_esi_kill(int socket_esi){
	int protocolo;
	serializar_id(&protocolo, 91);
	enviar(socket_esi, &protocolo, sizeof(int), logger);
}
