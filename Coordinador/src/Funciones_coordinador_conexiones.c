#include "Funciones_coordinador.h"

//////////////////////////////////////////////////// CONEXION /////////////////////////////////////////////////////

int handshake(int socket_cliente){
	t_handshake proceso_recibido;
	t_handshake yo = {0, COORDINADOR};
	void* buffer_recepcion = malloc(sizeof(int)*2);
	void* buffer_envio = malloc(sizeof(int)*3);

	recibir(socket_cliente, buffer_recepcion, sizeof(int)*2, logger);
	proceso_recibido = deserializar_handshake(buffer_recepcion);

	free(buffer_recepcion);

	serializar_handshake(buffer_envio, yo);
	enviar(socket_cliente, buffer_envio, sizeof(int)*3, logger);

	free(buffer_envio);
	if(conexion_hecha){
		if(proceso_recibido.proceso == PLANIFICADOR){
			conexion_hecha = 0;
			return 1;
		} else {
			return -1;
		}
	}

	switch(proceso_recibido.proceso){
	case PLANIFICADOR:
		return -1;
		break;

	case INSTANCIA:
		log_info(logger, "Se establecio la conexion con la Instancia %d", proceso_recibido.id);
		agregar_nueva_instancia(socket_cliente, proceso_recibido.id);
		log_info(logger, "El socket de la instancia es: %d", socket_cliente);
		return 1;
		break;

	case ESI:
		log_info(logger, "Se establecio la conexion con el ESI %d", proceso_recibido.id);
		agregar_nuevo_esi(socket_cliente, proceso_recibido.id);
		return 1;
		break;

	default:
		log_info(logger, "Conexion de proceso desconocido");
		return -1;
		break;
	}
}

void procesar_conexion(){
	int id_mensaje;
	int resultado = 1;
	while(resultado > 0 && !terminar_programa){
		int socket_cliente = aceptar_conexion(socket_escucha);
		recibir(socket_cliente, &id_mensaje, sizeof(int), logger);
		if(id_mensaje == 80){
			resultado = handshake(socket_cliente);
		} else {
			log_info(logger, "Se recibio un protocolo desconocido: %d", id_mensaje);
			resultado = -1;
		}
	}
	//pthread_exit(NULL);
}

void atender_planificador(){
	log_info(logger, "Hilo del planificador creado");
	int resultado = 1;
	while(resultado > 0){
		resultado = procesar_mensaje(socket_planificador);
	}
	if(resultado == -99){
		log_info(logger, "Recibi la finalizacion del Planificador");
	} else{
		log_error(logger, "Fallo en la conexion con el Planificador");
	}
	terminar_programa = true;
	close(socket_planificador);
	sem_post(&s_cerrar_hilo);
}

void atender_esi(void* datos_esi){
	int resultado = 1;
	hilo_proceso mis_datos = deserializar_hilo_proceso(datos_esi);
	free(datos_esi);
	log_info(logger, "Hilo del ESI %d creado", mis_datos.id);
	while(resultado > 0 && !terminar_programa){
		resultado = procesar_mensaje(mis_datos.socket);
	}
}

void atender_instancia(void* datos_instancia){
	hilo_proceso mis_datos = deserializar_hilo_proceso(datos_instancia);
	free(datos_instancia);
	log_info(logger, "Hilo de la Instancia %d creado", mis_datos.id);
	enviar_configuracion_instancia(mis_datos.socket);
	enviar_claves(mis_datos);//LO AGREGO IVI
	int resultado = 1;
	while(resultado > 0 && !terminar_programa){
		resultado = procesar_mensaje(mis_datos.socket);
	}
	socket_instancia_buscado = mis_datos.socket; //Agregar semaforos

	log_info(logger, "RIP instancia");
	desconectar_instancia(mis_datos.socket);
	//pthread_exit(NULL);
}

void enviar_claves(hilo_proceso mis_datos){ //AGREGO IVI
	t_list* claves_instancia;
	nodo_clave* nodo;
	void* buffer = malloc(sizeof(int));
	if(list_size(lista_claves)>0){
		id_instancia_buscado = mis_datos.id;
		serializar_id(buffer, 04);
		enviar(mis_datos.socket, buffer, sizeof(int), logger);
		//free(buffer);
		log_info(logger, "la lista de claves tiene %d", list_size(lista_claves));
		claves_instancia = list_filter(lista_claves, condicion_id_instancia);
		log_info(logger, "el tamanio es %d", list_size(claves_instancia));
		for(int i = 0; i < list_size(claves_instancia); i++){
			log_info(logger, "entra al for");
			nodo = list_get(claves_instancia, i);
			enviar_clave(nodo->clave,mis_datos.socket);
			log_info(logger, "envio %s en %d", nodo->clave, i);
		}
		serializar_id(buffer, 05);
		enviar(mis_datos.socket,buffer,sizeof(int),logger);
	} else {
		serializar_id(buffer, 05);
		enviar(mis_datos.socket,buffer,sizeof(int),logger);
	}

}

void enviar_clave(char* clave, int socket){ //AGREGO IVI
	log_info(logger, "llega a castear el nodo");
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer = malloc(tamanio_buffer);
	log_info(logger,"serialice el tamanio %d",tamanio_buffer);
	serializar_string_log(buffer,clave, 06,logger);
	log_info(logger,"serialice");
	int bytes = enviar(socket, buffer, tamanio_buffer, logger);
	log_info(logger, "pude enviar %d bytes", bytes);
//	free(buffer);
	/*	int tamanio = tamanio_buffer_string(clave);
	void* buffer = malloc(tamanio);
	serializar_string(buffer, clave, id);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return bytes_enviados;*/
}

void desconectar_instancia(int socket){
	socket_instancia_buscado = socket;
	nodo* el_nodo = list_find(lista_instancias, condicion_socket_instancia);
	close(socket);
	hilo_a_cerrar = &el_nodo->hilo;
	list_remove_and_destroy_by_condition(lista_instancias, condicion_socket_instancia, eliminar_nodo);
	sem_post(&s_cerrar_hilo);
}

