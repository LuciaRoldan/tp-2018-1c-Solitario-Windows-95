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

bool condicion_id_esi(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.id == id_esi_buscado;
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
	if(list_size(lista_claves) > 0){
		clave_buscada = malloc(strlen(clave)+1);
		memcpy(clave_buscada, clave, strlen(clave)+1);
		if(list_any_satisfy(lista_claves, condicion_clave)){
			nodo_clave* n_clave = list_find(lista_claves, condicion_clave);
			id_instancia_buscado = n_clave->nodo_instancia.id;
			bool resultado = list_any_satisfy(lista_instancias, condicion_id_instancia);
			return resultado;
		}
		return true;
		free(clave_buscada);
	}
	return true;
}

nodo* buscar_instancia(char* clave){
	log_info(logger, "Estoy buscando una instancia");
	nodo* nodo_instancia;
	nodo_clave* nodito;
	clave_buscada = malloc(strlen(clave)+1);
	memcpy(clave_buscada, clave, strlen(clave)+1);
	if(list_any_satisfy(lista_claves, condicion_clave)){
		nodito = list_find(lista_claves, condicion_clave);
		id_instancia_buscado = nodito->nodo_instancia.id;
		nodo_instancia = list_find(lista_instancias, condicion_id_instancia);
	} else {
		nodo_instancia = seleccionar_instancia(clave);
		nodo_clave* nodo_ = malloc(sizeof(nodo_clave));
		nodo_->clave = clave;
		nodo_->nodo_instancia = *nodo_instancia;
		list_add(lista_claves, nodo_);
	}
	free(clave_buscada);
	return nodo_instancia;
}

nodo* seleccionar_instancia(char* clave){
	nodo* instancia_seleccionada;

	nodo* nodo_auxiliar;
	int minimo_LSU = 1000000;

	char* char_KE = malloc(sizeof(char));
	int cantidad_letras_KE, id_instancia;
	div_t resultado_KE;

	switch(info_coordinador.algoritmo_distribucion){

	case EL:
		instancia_seleccionada = list_get(lista_instancias, ultima_instancia_EL);
		if(ultima_instancia_EL++ == list_size(lista_instancias)-1){ultima_instancia_EL = 0;}
		break;

	case LSU:
		for(int contador = 0; contador < list_size(lista_instancias); contador++){
			nodo_auxiliar = list_get(lista_instancias, contador);
			socket_instancia_buscado = nodo_auxiliar->socket;
			if(list_count_satisfying(lista_claves, condicion_socket_clave) < minimo_LSU){
				minimo_LSU = list_count_satisfying(lista_claves, condicion_socket_clave);
				instancia_seleccionada = nodo_auxiliar;
			}
		}
		break;

	case KE:
		memcpy(char_KE, clave, sizeof(char));
		if( 25 % list_size(lista_instancias) == 0){
			cantidad_letras_KE = 26 / list_size(lista_instancias);
		} else {
			resultado_KE = div(26,list_size(lista_instancias));
			cantidad_letras_KE = resultado_KE.quot;
		}

		if((*char_KE - 97) % cantidad_letras_KE == 0){
			id_instancia = cantidad_letras_KE / *char_KE;
		} else {
			resultado_KE = div((*char_KE - 97), cantidad_letras_KE); //(int)'a' = 97
			id_instancia = resultado_KE.quot+1;
		}
		id_instancia_buscado = id_instancia;
		instancia_seleccionada = list_find(lista_instancias, condicion_id_instancia);
		break;
	}
	return instancia_seleccionada;
	free(char_KE);
}

int buscar_instancia_ficticia(char* clave){
	nodo* nodito = seleccionar_instancia(clave);
	if(info_coordinador.algoritmo_distribucion == EL){
		ultima_instancia_EL = ultima_instancia_EL - 1;
	}
	return nodito->id;
}

nodo* encontrar_esi(int socket){//verificar semaforos
	socket_esi_buscado = socket;
	nodo* el_nodo = list_find(lista_esis, condicion_socket_esi);
	return el_nodo;
}

void fin_instancia(void* datos){
	nodo* nodo_instancia = datos;
	int fin = 85;
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, fin);
	enviar(nodo_instancia->socket, buffer, sizeof(int), logger);
	free(buffer);
}

void cerrar_instancias(){
	list_iterate(lista_instancias, fin_instancia);
}

void compactar_intancias(){
	int mensaje = 3;
	void* buffer = malloc(sizeof(int));

	memcpy(buffer, &mensaje, sizeof(int));
	log_info(logger, "El tamanio de la lista es %d", list_size(lista_instancias));
	for(int i = 0; i < list_size(lista_instancias); i++){
		nodo* nodito = list_get(lista_instancias, i);
		enviar(nodito->socket, buffer, sizeof(int), logger);
		log_info(logger, "Le dije a la instancia %d de socket %d", nodito->id, nodito->socket);
	}
}




