#include "planificador_punto_hache.h"

//--Funciones consola--//
void pausar_planificacion(){
	pausar_planificador = -1;
	if(list_size(esis_ready) > 0){
		sem_post(&s_planificar); //o sea pausaron en el medio de la ejec
	}
}
void continuar_planificacion(){
	pausar_planificador = 1;
}
void bloquear(char* clave, int id){
	mover_esi_a_bloqueados(clave, id);
}
void desbloquear(char * clave){
	liberar_clave(clave);
}

void pedir_status(char* clave){
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer_pedido_clave = malloc(tamanio_buffer);
	serializar_string(buffer_pedido_clave, clave, 21);
	enviar(sockets_planificador.socket_coordinador, buffer_pedido_clave, tamanio_buffer, logger);
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

void mostrar_status_clave(status_clave status){
	if (status.contenido != NULL){
		printf("\t *** El valor de la clave es: %s\n", status.contenido);
	} else {
		printf("\t *** La clave existe pero esta vacia.\n");
	}
	if (status.id_instancia_actual > 0){
		printf("\t ***La clave se encuentra actualmente es la instancia: %d\n", status.id_instancia_actual);
	} else {
		printf("\t *** La clave no está actualmente asignada a ninguna instancia.\n");
	}
	printf("\t *** La clave se guardaria en la instancia: %d\n", status.id_instancia_nueva);
	printf("\t *** Los ESIs a la espera de la clave son: ");
	clave_buscada = status.clave;
	clave_bloqueada* clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	list_iterate(clave->esis_en_espera, imprimir_id_esi);
}

void listar_procesos_encolados(char* recurso){
	printf("---> Los ESIs a la espera del recurso %s son: ", recurso);
	clave_buscada = recurso;
	clave_bloqueada* clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	list_iterate(clave->esis_en_espera, imprimir_id_esi);
}

void kill_esi(int id){
	id_buscado = id;
	if (list_any_satisfy(pcbs, ids_iguales_pcb)){
		log_info(logger, "ESI %d sera finalizado por funcion 'kill' de consola.", id);
		esi_a_finalizar = id;
		if(list_size(esis_ready) == 0){
			mover_esi_a_finalizados(id);
		}
	} else {
		log_info(logger, "No conozco a ese ESI");
	}
}

void enviar_esi_kill(int socket_esi){
	int protocolo;
	serializar_id(&protocolo, 91);
	enviar(socket_esi, &protocolo, sizeof(int), logger);
}

void enviar_esi_aborto(int socket_esi){
	int protocolo;
	serializar_id(&protocolo, 62);
	enviar(socket_esi, &protocolo, sizeof(int), logger);
}

void enviar_esi_exit(int socket_esi){
	int protocolo;
	serializar_id(&protocolo, 85);
	enviar(socket_esi, &protocolo, sizeof(int), logger);
}

//--Deadlock--//
int deadlock(){
	int cantidad_claves_bloqueadas = list_size(claves_bloqueadas);
	t_list* esis_en_deadlock = list_create();
	while(cantidad_claves_bloqueadas > 0){
		clave_bloqueada* nodo_clave = list_get(claves_bloqueadas, cantidad_claves_bloqueadas-1);
		if(el_duenio_esta_en_deadlock(nodo_clave)){
			list_add(esis_en_deadlock, &nodo_clave->esi_que_la_usa);
		}
		cantidad_claves_bloqueadas--;
	}
	log_info(logger, "Termine de analizar deadlocks");
	int cantidad_esis_en_deadlock = list_size(esis_en_deadlock);
	log_info(logger, "La cantidad de ESIS en deadlock es: %d", cantidad_esis_en_deadlock);
	while(cantidad_esis_en_deadlock > 0){
		int* esi = list_get(esis_en_deadlock, cantidad_esis_en_deadlock-1);
		log_info(logger, "El esi de ID: %d se encuentra en deadlock", *esi);
		cantidad_esis_en_deadlock--;
	}
	if (cantidad_esis_en_deadlock > 0){
		return 1;
	} else {
		return 0;
	}
}

bool el_duenio_esta_en_deadlock(clave_bloqueada* nodo_clave){
	log_info(logger, "Analizando si el esi de ID %d con la clave %s se encuentra en deadlock", nodo_clave->esi_que_la_usa, nodo_clave->clave);
	//Yo se que el esi que la usa tiene una clave, entonces voy a buscar si hay otra clave que
	//ese esi esta esperando, y si hay, me voy a fijar si el esi que la tiene esta esperando esta clave.
	pthread_mutex_lock(&m_id_buscado);
	if(list_size(nodo_clave->esis_en_espera)>0){
		log_info(logger, "Hay por lo menos un esi en la cola de espera de la clave que le pertenece al esi %d", nodo_clave->esi_que_la_usa);
		id_buscado = nodo_clave->esi_que_la_usa;
		if(list_any_satisfy(claves_bloqueadas, el_esi_la_esta_esperando)){
			log_info(logger, "El esi %d se encuentra bloqueado esperando una clave", nodo_clave->esi_que_la_usa);
			clave_bloqueada* bloqueada = list_find(claves_bloqueadas, el_esi_la_esta_esperando);
			log_info(logger, "La clave que ese esi espera es: %s", bloqueada->clave);
			log_info(logger, "El esi que ocupa esa clave es: %d", bloqueada->esi_que_la_usa);
			pthread_mutex_unlock(&m_id_buscado);
			if(el_esi_espera_la_clave(bloqueada->esi_que_la_usa, nodo_clave)){
				log_info(logger, "Voy a returnear que el ESI %d se encuentra en deadlock", bloqueada->esi_que_la_usa);
				return true;
			}
		}
	}
	pthread_mutex_unlock(&m_id_buscado);
	return false;
}

bool el_esi_espera_la_clave(int esi_que_la_usa, clave_bloqueada* nodo_clave){
	pthread_mutex_lock(&m_id_buscado);
	id_buscado = esi_que_la_usa;

	log_info(logger, "Me fijo si el ESI %d espera la clave %s", esi_que_la_usa, nodo_clave->clave);

	if(list_any_satisfy(nodo_clave->esis_en_espera, ids_iguales_ints)){
		pthread_mutex_unlock(&m_id_buscado);
		return true;
	} else {
		pthread_mutex_unlock(&m_id_buscado);
		return false;
	}
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

void imprimir_id_esi(void* esi){
	int id_esi;
	memcpy(&id_esi, esi, sizeof(int));
	printf("%d  ", id_esi);
}
