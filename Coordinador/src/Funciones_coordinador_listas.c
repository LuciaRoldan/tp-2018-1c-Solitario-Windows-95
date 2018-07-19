#include "Funciones_coordinador.h"

/////////////////////////////////////////////// FUNCIONES DE LISTAS ///////////////////////////////////////////////

void eliminar_nodo(void* datos){
	nodo* un_nodo = datos;
	free(un_nodo);
}

void eliminar_nodo_clave(void* datos){
	nodo_clave* un_nodo = datos;
	free(un_nodo->clave);
	free(un_nodo);
}

bool condicion_socket_esi(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.socket == socket_esi_buscado;
}

bool condicion_socket_instancia(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.socket == socket_instancia_buscado;
}

bool condicion_clave(void* datos){
	nodo_clave un_nodo = *((nodo_clave*) datos);
	return (strcmp(un_nodo.clave, clave_buscada) == 0);
}

bool condicion_socket_clave(void* datos){
	nodo_clave un_nodo = *((nodo_clave*) datos);
	return un_nodo.nodo_instancia.socket == socket_instancia_buscado;
}

bool condicion_id_instancia(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.id == id_instancia_buscado;
}

void reemplazar_instancia(nodo un_nodo){
	list_remove_and_destroy_by_condition(lista_instancias, condicion_socket_instancia, eliminar_nodo);
	list_add(lista_instancias, &un_nodo);
}

bool clave_accesible(char* clave){
	pthread_mutex_lock(&m_clave_buscada);
	clave_buscada = malloc(strlen(clave)+1);
	memcpy(clave_buscada, clave, strlen(clave)+1);
	nodo_clave* n_clave = list_find(lista_claves, condicion_clave);
	pthread_mutex_unlock(&m_clave_buscada);
	pthread_mutex_lock(&m_id_instancia_buscado);
	id_instancia_buscado = n_clave->nodo_instancia.id;
	log_info(logger, "Voy a buscar");
	bool resultado = list_any_satisfy(lista_instancias, condicion_id_instancia);
	pthread_mutex_unlock(&m_id_instancia_buscado);
	return resultado;
}

nodo* buscar_instancia(char* clave){
	nodo* nodo_instancia;
	pthread_mutex_lock(&m_lista_claves);
	pthread_mutex_lock(&m_clave_buscada);
	clave_buscada = malloc(strlen(clave)+1);
	memcpy(clave_buscada, clave, strlen(clave)+1);
	if(list_any_satisfy(lista_claves, condicion_clave)){
		nodo_instancia = list_find(lista_claves, condicion_clave); //Esto no deberia funcionar?????
	} else {
		nodo_instancia = seleccionar_instancia(clave);
		nodo_clave* nodo_ = malloc(sizeof(nodo_clave));
		nodo_->clave = clave;
		nodo_->nodo_instancia = *nodo_instancia;
		list_add(lista_claves, nodo_);
	}
	free(clave_buscada);
	pthread_mutex_unlock(&m_clave_buscada);
	pthread_mutex_unlock(&m_lista_claves);
	return nodo_instancia;
}

nodo* seleccionar_instancia(char* clave){
	nodo* instancia_seleccionada;

	nodo* nodo_auxiliar;
	int minimo_LSU = 1000000;

	char char_KE;
	int cantidad_letras_KE, id_instancia;
	div_t resultado_KE;

	pthread_mutex_lock(&m_lista_instancias);
	switch(info_coordinador.algoritmo_distribucion){
	case EL:
		log_info(logger, "Tamanio: %d", list_size(lista_instancias));
		pthread_mutex_lock(&m_ultima_instancia_EL);
		log_info(logger, "+++++++ EL es: %d +++++++", ultima_instancia_EL);
		instancia_seleccionada = list_get(lista_instancias, ultima_instancia_EL);
		//instancia_seleccionada = list_get(lista_instancias, 0);
		if(ultima_instancia_EL++ == list_size(lista_instancias)){ultima_instancia_EL = 0;}
		pthread_mutex_unlock(&m_ultima_instancia_EL);
		break;
	case LSU:
		pthread_mutex_lock(&m_lista_claves);
		for(int contador = 0; contador < list_size(lista_instancias); contador++){
			nodo_auxiliar = list_get(lista_instancias, contador);
			socket_instancia_buscado = nodo_auxiliar->socket;
			if(list_count_satisfying(lista_claves, condicion_socket_clave) < minimo_LSU){
				instancia_seleccionada = nodo_auxiliar;
			}
		}
		pthread_mutex_unlock(&m_lista_claves);
		break;
	case KE:
		memcpy(&char_KE, clave, sizeof(char));
		if( 25 % list_size(lista_instancias) == 0){
			cantidad_letras_KE = 25 / list_size(lista_instancias);
		} else {
			resultado_KE = div(25,list_size(lista_instancias));
			cantidad_letras_KE = resultado_KE.quot + 1;
		}

		if( cantidad_letras_KE % (int)char_KE == 0){
			id_instancia = cantidad_letras_KE / (int)char_KE;
		} else {
			resultado_KE = div(cantidad_letras_KE,(int)char_KE); //(int)'a' = 97
			id_instancia = resultado_KE.quot + 1;
		}
		pthread_mutex_lock(&m_id_instancia_buscado);
		id_instancia_buscado = id_instancia;
		list_find(lista_instancias, condicion_id_instancia);
		pthread_mutex_unlock(&m_id_instancia_buscado);
		break;
	}
	pthread_mutex_unlock(&m_lista_instancias);
	return instancia_seleccionada;
}

nodo* encontrar_esi(int socket){//verificar semaforos
	pthread_mutex_lock(&m_socket_esi_buscado);
	socket_esi_buscado = socket;
	pthread_mutex_lock(&m_lista_esis);
	nodo* el_nodo = list_find(lista_esis, condicion_socket_esi);
	pthread_mutex_unlock(&m_lista_esis);
	pthread_mutex_unlock(&m_socket_esi_buscado);
	log_info(logger, "Socket encontrado: %d, y su id: %d", el_nodo->socket, el_nodo->id);
	return el_nodo;
}
