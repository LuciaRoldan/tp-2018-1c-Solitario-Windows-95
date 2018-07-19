#include "Funciones_coordinador.h"

/////////////////////////////////////////////// FUNCIONES DE HILOS ////////////////////////////////////////////////

void serializar_hilo_proceso(void* buffer, hilo_proceso hilo){
	hilo_proceso* info_hilo_proceso = malloc(sizeof(hilo_proceso));
	*info_hilo_proceso = hilo;
	memcpy(buffer, info_hilo_proceso, sizeof(hilo_proceso));
	free(info_hilo_proceso);
}

hilo_proceso deserializar_hilo_proceso(void *buffer_recepcion){
	hilo_proceso hilo_proceso_recibido;
	memcpy(&hilo_proceso_recibido.socket, buffer_recepcion, sizeof(int));
	memcpy(&hilo_proceso_recibido.id, buffer_recepcion + sizeof(int), sizeof(int));
	return hilo_proceso_recibido;
}

void agregar_nuevo_esi(int socket_esi, int id_esi){
	hilo_proceso datos_esi = {socket_esi, id_esi};
	void* buffer = malloc(sizeof(int)*2); //liberado en atender_esi
	serializar_hilo_proceso(buffer, datos_esi);
	pthread_t hilo_esi;
	pthread_create(&hilo_esi, 0, atender_esi, buffer);
	nodo* el_nodo = malloc(sizeof(nodo));
	el_nodo->socket = socket_esi;
	el_nodo->id = id_esi;
	el_nodo->hilo = hilo_esi;
	pthread_mutex_lock(&m_lista_esis);
	list_add(lista_esis, el_nodo);
	pthread_mutex_unlock(&m_lista_esis);
}

void agregar_nueva_instancia(int socket_instancia, int id_instancia){
	hilo_proceso datos_instancia = {socket_instancia, id_instancia};
	void* buffer = malloc(sizeof(int)*2); //liberado en atender_instancia
	serializar_hilo_proceso(buffer, datos_instancia);
	pthread_t hilo_instancia;
	pthread_create(&hilo_instancia, 0, atender_instancia, buffer);
	nodo* el_nodo = malloc(sizeof(nodo));
	el_nodo->socket = socket_instancia;
	el_nodo->id = id_instancia;
	el_nodo->hilo = hilo_instancia;
	pthread_mutex_lock(&m_lista_instancias);
	list_add(lista_instancias, el_nodo);
	pthread_mutex_unlock(&m_lista_instancias);
}
