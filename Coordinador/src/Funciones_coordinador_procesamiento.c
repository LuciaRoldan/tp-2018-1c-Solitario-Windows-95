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
	//log_info(logger, "Protocolo recibido: %d", id);
	free(buffer_int);
	int rta_esi;

	switch(id){
		case 20://Exit
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

		case 22: //Compactacion de las instancias
			compactar_intancias();
			log_info(logger, "Les aviso a las Instancias que tienen que compactar");
			return 1;
			break;

		case 24:
			log_info(logger, "Fallo por clave inaccesible, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 88;
			void* buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			pthread_exit(NULL);
			return -1;
			break;

		case 25: //Exito instancia
			log_info(logger, "Recibi confirmacion de la Instancia %d", instancia_seleccionada->id);
			rta_esi = 84;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			log_info(logger, "Le digo al ESI %d que la ejecucion fue exitosa", esi_ejecutando->id);
			free(buffer_int);
			return resultado;
			break;

		case 81: //Fin de ejecucion del ESI
			el_nodo = encontrar_esi(socket);
			hilo_a_cerrar = &el_nodo->hilo;
			socket_esi_buscado = el_nodo->socket;
			log_info(logger, "El ESI %d termino", el_nodo->id);
			log_info(log_operaciones, "El ESI %d termino", el_nodo->id);
			close(socket);
			sem_post(&s_cerrar_hilo);
			sem_wait(&s_borrar_elemento);
			list_remove_and_destroy_by_condition(lista_esis, condicion_socket_esi, eliminar_nodo); //No gusta
			pthread_exit(NULL);
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
			log_info(logger, "Envie la instruccion a la instancia %d", instancia_seleccionada->id);
			return 1;

		case 85: //Fallo
			log_info(logger, "Fallo, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 25;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			liberar_instruccion();
			return 1;
			break;

		case 86: //ESI avisa calve muy larga
			socket_esi_buscado = socket;
			nodo* el_nodo = list_find(lista_esis, condicion_socket_esi);
			log_info(logger, "Fallo de clave muy larga, ID ESI: %d", el_nodo->id);
			hilo_a_cerrar = &el_nodo->hilo;
			list_remove_and_destroy_by_condition(lista_instancias, condicion_socket_esi, eliminar_nodo);
			sem_post(&s_cerrar_hilo);
			close(socket);
			pthread_exit(NULL);
			return -1;
			break;

		case 87: //Fallo clave no identificada
			log_info(logger, "Fallo por clave no identificada, ID ESI: %d", esi_ejecutando->id);
			rta_esi = 87;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			liberar_instruccion();
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			close(socket);
			pthread_exit(NULL);
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
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			close(socket);
			pthread_exit(NULL);
			return resultado;
			break;

		case 90://Hay que bloquear al ESI
			log_info(logger, "Le digo al ESI %d que esta bloqueado", esi_ejecutando->id);
			rta_esi = 90;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
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
			hilo_a_cerrar = &esi_ejecutando->hilo;
			sem_post(&s_cerrar_hilo);
			close(socket);
			pthread_exit(NULL);
			return resultado;
			break;
		default:
			//log_error(logger, "Protocolo desconocido");
			return -1;
			break;
	}
}

int procesar_instruccion(t_esi_operacion instruccion, int socket){
	char* clave;
	esi_ejecutando = encontrar_esi(socket);
	int rta_esi;

	switch(instruccion.keyword){
	case GET:
		log_info(log_operaciones, "ESI %d GET %s", esi_ejecutando->id, instruccion.argumentos.GET.clave);
		clave = malloc(strlen(instruccion.argumentos.GET.clave)+1);
		memcpy(clave, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
		break;
	case SET:
		log_info(log_operaciones, "ESI %d SET %s %s", esi_ejecutando->id, instruccion.argumentos.SET.clave, instruccion.argumentos.SET.valor);
		clave = malloc(strlen(instruccion.argumentos.SET.clave)+1);
		memcpy(clave, instruccion.argumentos.SET.clave, strlen(instruccion.argumentos.SET.clave)+1);
		break;
	case STORE:
		log_info(log_operaciones, "ESI %d STORE %s", esi_ejecutando->id, instruccion.argumentos.STORE.clave);
		clave = malloc(strlen(instruccion.argumentos.STORE.clave)+1);
		memcpy(clave, instruccion.argumentos.STORE.clave, strlen(instruccion.argumentos.STORE.clave)+1);
		break;
	}
	if(strlen(clave) > 40){																																		//Metete en tu propio codigo
		log_info(logger, "Fallo por clave muy larga, ID ESI: %d", esi_ejecutando->id);
		rta_esi = 86;
		void* buffer_int = malloc(sizeof(int));
		serializar_id(buffer_int, rta_esi);
		enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
		hilo_a_cerrar = &esi_ejecutando->hilo;
		sem_post(&s_cerrar_hilo);
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
				hilo_a_cerrar = &esi_ejecutando->hilo;
				sem_post(&s_cerrar_hilo);
				return -1;
		} else {
			log_info(logger, "El pedido es valido");
			instancia_seleccionada = buscar_instancia(clave);
			if(instruccion.keyword == GET){
				nodo_clave* nodito = malloc(sizeof(nodo_clave));
				nodito->clave = malloc(strlen(clave));
				memcpy(nodito->clave, clave, strlen(clave));
				nodito->nodo_instancia = *instancia_seleccionada;
				list_add(lista_claves, nodito);
			}
			if(instruccion.keyword == STORE){
				clave_buscada = malloc(strlen(instruccion.argumentos.STORE.clave)+1);
				memcpy(clave_buscada, instruccion.argumentos.STORE.clave, strlen(instruccion.argumentos.STORE.clave)+1);
				nodo_clave* nodito = list_remove_by_condition(lista_claves, condicion_clave);
				free(nodito->clave);
				free(clave_buscada);
			}
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
