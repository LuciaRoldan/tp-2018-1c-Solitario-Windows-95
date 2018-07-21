#include "Funciones_coordinador.h"


////////////////////////////////////////////////// PROCESAMIENTO ///////////////////////////////////////////////////

int procesar_mensaje(int socket){
	int resultado, id, id_esi;
	char* clave;
	nodo* nodo_instancia;
	nodo* el_nodo;
	int protocolo_extra = 1; //Sacar inicializacion
	t_esi_operacion instruccion;
	status_clave status;
	void* buffer_int = malloc(sizeof(int));
	id = recibir_int(socket, logger);
	log_info(logger, "Protocolo recibido: %d", id);
	free(buffer_int);
	int rta_esi;

	switch(id){
		case 20:
			rta_esi = 44;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			enviar(socket, buffer_int, sizeof(int), logger);
			return -99;
		case 21: //Recibo una clave
			clave = recibir_pedido_clave(socket);
			log_info(logger, "Me llego el pedido de la clave: %d", clave);
			nodo_instancia = buscar_instancia(clave);
			log_info(logger, "Le voy a pedir a la instancia: %d", nodo_instancia->id);
			protocolo_extra = 83;
			resultado = enviar_pedido_valor(nodo_instancia->socket, clave, protocolo_extra);
			return resultado;
			break;

		case 23:
			rta_esi = list_size(lista_instancias);
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(socket, buffer_int, sizeof(int), logger);
			log_info(logger, "Le digo a la Instancia cuantas Instancias hay");
			return resultado;
			break;

		case 25: //Exito instancia
			log_info(logger, "Recibi confirmacion de la Instancia %d", instancia_seleccionada->id);
			pthread_mutex_unlock(&m_instancia_seleccionada);
			rta_esi = 84;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			log_info(logger, "Le digo al ESI %d que la ejecucion fue exitosa", esi_ejecutando->id);
			free(buffer_int);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return resultado;
			break;

		case 81: //Fin de ejecucion del ESI
			el_nodo = encontrar_esi(socket);
			pthread_mutex_lock(&m_hilo_a_cerrar);
			hilo_a_cerrar = &el_nodo->hilo;
			pthread_mutex_lock(&m_socket_esi_buscado);
			socket_esi_buscado = el_nodo->socket;
			log_info(logger, "Voy a eliminar");
			pthread_mutex_lock(&m_lista_esis);
			list_remove_and_destroy_by_condition(lista_esis, condicion_socket_esi, eliminar_nodo);
			pthread_mutex_unlock(&m_lista_esis);
			pthread_mutex_unlock(&m_socket_esi_buscado);
			close(socket);
			sem_post(&s_cerrar_hilo);
			return -1;
			break;

		case 82: //Instruccion
			instruccion = recibir_instruccion(socket);
			procesar_instruccion(instruccion, socket);
			return 1;
			break;

		case 83: //Status clave
			status = recibir_status(socket);
			status.id_instancia_nueva = buscar_instancia_ficticia(status.clave);
			resultado = enviar_status_clave(socket_planificador, status);
			return resultado;
			break;

		case 84: //Exito Planificador
			log_info(logger, "Recibi confirmacion del Planificador");
			enviar_operacion(instancia_seleccionada->socket, operacion_ejecutando);
			liberar_instruccion();
			pthread_mutex_unlock(&m_operacion_ejecutando);
			log_info(logger, "Envie la instruccion a la instancia %d", instancia_seleccionada->id);
			return 1;

		case 85: //Fallo
			log_info(logger, "Fallo por clave no identificada, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 25;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			liberar_instruccion();
			pthread_mutex_unlock(&m_operacion_ejecutando);
			pthread_mutex_unlock(&m_instancia_seleccionada);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return 1;
			break;

		case 87: //Fallo clave no identificada
			log_info(logger, "Fallo por clave no identificada, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 87;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			liberar_instruccion();
			pthread_mutex_unlock(&m_operacion_ejecutando);
			pthread_mutex_lock(&m_hilo_a_cerrar);
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			pthread_mutex_unlock(&m_instancia_seleccionada);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return resultado;
			break;

		case 89://Fallo clave no bloqueada
			log_info(logger, "Fallo por clave no bloqueada, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 89;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			liberar_instruccion();
			pthread_mutex_unlock(&m_operacion_ejecutando);
			pthread_mutex_lock(&m_hilo_a_cerrar);
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			pthread_mutex_unlock(&m_instancia_seleccionada);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return resultado;
			break;
		case 90://Hay que bloquear al ESI
			log_info(logger, "Le digo al ESI %d que esta bloqueado", esi_ejecutando->id);
			rta_esi = 90;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			pthread_mutex_unlock(&m_instancia_seleccionada);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return resultado;
			break;
		case 91:
			id_esi = recibir_int(socket_planificador, logger);
			id_esi_buscado = id_esi;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, id);
			nodo* el_esi_buscado = list_find(lista_esis, condicion_id_esi);
			log_info(logger, "Abortando ESI %d por operacion 'kill' de consola", el_esi_buscado->id);
			resultado = enviar(el_esi_buscado->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			pthread_mutex_lock(&m_hilo_a_cerrar);
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			pthread_mutex_unlock(&m_esi_ejecutando); // Esto lo tengo que borrar?
			return resultado;
			break;
		default:
			return -1;
			break;
	}
}

int procesar_instruccion(t_esi_operacion instruccion, int socket){
	char* clave;
	pthread_mutex_lock(&m_esi_ejecutando);
	esi_ejecutando = encontrar_esi(socket);
	int rta_esi;

	pthread_mutex_lock(&m_log_operaciones);
	switch(instruccion.keyword){
	case GET:
		log_info(log_operaciones, "ESI %d GET %s", esi_ejecutando->id, instruccion.argumentos.GET.clave);
		clave = malloc(strlen(instruccion.argumentos.GET.clave)+1);
		memcpy(clave, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
		break;
	case SET:
		log_info(log_operaciones, "ESI %d GET %s %s", esi_ejecutando->id, instruccion.argumentos.SET.clave, instruccion.argumentos.SET.valor);
		clave = malloc(strlen(instruccion.argumentos.SET.clave)+1);
		memcpy(clave, instruccion.argumentos.SET.clave, strlen(instruccion.argumentos.SET.clave)+1);
		break;
	case STORE:
		log_info(log_operaciones, "ESI %d STORE %s", esi_ejecutando->id, instruccion.argumentos.STORE.clave);
		clave = malloc(strlen(instruccion.argumentos.STORE.clave)+1);
		memcpy(clave, instruccion.argumentos.STORE.clave, strlen(instruccion.argumentos.STORE.clave)+1);
		break;
	}
	if(strlen(clave) > 40){
		log_info(logger, "Fallo por clave muy larga, ID ESI: %d", esi_ejecutando->id);
		rta_esi = 86;
		void* buffer_int = malloc(sizeof(int));
		serializar_id(buffer_int, rta_esi);
		enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
		pthread_mutex_lock(&m_hilo_a_cerrar);
		hilo_a_cerrar = &esi_ejecutando->hilo;
		sem_post(&s_cerrar_hilo);
		pthread_mutex_unlock(&m_log_operaciones);
		pthread_mutex_unlock(&m_esi_ejecutando);
		free(buffer_int);
		return -1;
	} else {
		if(!clave_accesible(clave)) {
				log_info(logger, "Fallo por clave inaccesible, ID ESI: %d", esi_ejecutando->id);
				rta_esi = 88;
				void* buffer_int = malloc(sizeof(int));
				serializar_id(buffer_int, rta_esi);
				enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
				free(buffer_int);
				pthread_mutex_lock(&m_hilo_a_cerrar);
				log_info(logger, "Pase el lock");
				hilo_a_cerrar = &esi_ejecutando->hilo;
				sem_post(&s_cerrar_hilo);
				pthread_mutex_unlock(&m_instancia_seleccionada);
				pthread_mutex_unlock(&m_log_operaciones);
				pthread_mutex_unlock(&m_esi_ejecutando);
				return -1;
		} else {
			log_info(logger, "El pedido es valido");
			pthread_mutex_unlock(&m_log_operaciones);
			pthread_mutex_lock(&m_instancia_seleccionada);
			instancia_seleccionada = buscar_instancia(clave);
					/*if(instruccion.keyword == GET){
						nodo_clave* nodito = malloc(sizeof(nodo_clave));
						nodito->clave = malloc(strlen(clave));
						memcpy(nodito->clave, clave, strlen(clave));
						nodito->nodo_instancia = *instancia_seleccionada;
						list_add(lista_claves, nodito);
						log_info(logger, "lo agregue %d", list_size(lista_claves));
					}*/
			pthread_mutex_unlock(&m_operacion_ejecutando); //verificar
			pthread_mutex_lock(&m_operacion_ejecutando);
			operacion_ejecutando = instruccion;
			enviar_operacion(socket_planificador, instruccion);
		}
	}
	return 1;
	free(clave);
}

void liberar_instruccion(){
	switch(operacion_ejecutando.keyword){
	case GET:
		free(operacion_ejecutando.argumentos.GET.clave);
		break;
	case SET:
		free(operacion_ejecutando.argumentos.SET.clave);
		free(operacion_ejecutando.argumentos.SET.valor);
		break;
	case STORE:
		free(operacion_ejecutando.argumentos.STORE.clave);
		break;
	}
}
