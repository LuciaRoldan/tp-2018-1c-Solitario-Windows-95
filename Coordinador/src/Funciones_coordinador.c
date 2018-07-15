#include "Funciones_coordinador.h"


////////////////////////////////////////////// PROCESAMIENTO ///////////////////////////////////////////////

int procesar_mensaje(int socket){
	int resultado, id;
	char* clave;
	nodo* nodo_instancia;
	nodo* el_nodo;
	int protocolo_extra = 1; //Sacar inicializacion
	t_esi_operacion instruccion;
	status_clave status;
	void* buffer_int = malloc(sizeof(int));
	recibir(socket, buffer_int, sizeof(int), logger);
	id = deserializar_id(buffer_int);
	log_info(logger, "Protocolo recibido: %d", id);

	switch(id){
		case 20://salio tod o ok
			log_info(logger, "El plani respondio");
			return 1;
		case 21:
			clave = recibir_pedido_clave(socket);
			nodo_instancia = buscar_instancia(clave);
			protocolo_extra = 1;
			resultado = enviar_pedido_valor(nodo_instancia->socket, clave, protocolo_extra);
			return resultado;
			break;
		case 23:
			resultado = desconectar_instancia(socket);
			return 1;
			break;
		case 81:
			el_nodo = encontrar_esi(socket);
			hilo_a_cerrar = &el_nodo->hilo;
			socket_esi_buscado = el_nodo->socket;
			log_info(logger, "voy a eliminar");
			list_remove_by_condition(lista_esis, condicion_socket_esi);
			sem_post(&s_cerrar_hilo);
			return -1;
			break;
		case 82:
			instruccion = recibir_instruccion(socket);
			procesar_instruccion(instruccion, socket);
			return 1;
			break;
		case 83:
			status = recibir_status(socket);
			resultado = enviar_status_clave(socket_planificador, status);
			return resultado;
			break;
		default:
			return -1;
			break;
	}
}

int procesar_instruccion(t_esi_operacion instruccion, int socket){
	char* clave;
	switch(instruccion.keyword){
	case GET:
		clave = malloc(sizeof(instruccion.argumentos.GET.clave));
		memcpy(clave, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
		break;
	case SET:
		clave = malloc(sizeof(instruccion.argumentos.SET.clave));
		memcpy(clave, instruccion.argumentos.SET.clave, strlen(instruccion.argumentos.SET.clave)+1);
		break;
	case STORE:
		clave = malloc(sizeof(instruccion.argumentos.STORE.clave));
		memcpy(clave, instruccion.argumentos.STORE.clave, strlen(instruccion.argumentos.STORE.clave)+1);
		break;
	}
	nodo* esi = encontrar_esi(socket);
	log_info(logger, "Id esi: %d", esi->id);
	nodo* instancia = buscar_instancia(clave);
	log_info(logger, "Id instancia: %d", instancia->id);

	enviar_pedido_esi(esi->id, instruccion);
	/*void* buffercito = malloc(sizeof(int));
	recibir(socket_planificador, buffercito, sizeof(int), logger);
	int protocolo_plani = deserializar_id(buffercito);
	int protocolo_instancia;
	log_info(logger, "Plani dijo: %d", protocolo_plani);
	switch(protocolo_plani){
	case 24:
		return FALLO_ESI;
		break;
	case 20:
		enviar_operacion(instancia->socket, instruccion);
		recibir(instancia->socket, buffercito, sizeof(int), logger);
		protocolo_instancia = deserializar_id(buffercito);
		if(protocolo_instancia == 20){
			return EXITO_ESI;
		}
		return FALLO_ESI;
	default:
		return ABORTO_ESI;
		break;
	}*/
	return 1;
}

///////////////////////////////////////////////// CONEXION ////////////////////////////////////////////////

int handshake(int socket_cliente){
	int conexion_hecha = 0;

	t_handshake proceso_recibido;
	t_handshake yo = {0, COORDINADOR};
	void* buffer_recepcion = malloc(sizeof(int)*2);
	void* buffer_envio = malloc(sizeof(int)*3);

	recibir(socket_cliente, buffer_recepcion, sizeof(int)*2, logger);
	proceso_recibido = deserializar_handshake(buffer_recepcion);

	log_info(logger, "Proceso recibido %d", proceso_recibido.proceso);
	log_info(logger, "Id proceso recibido %d", proceso_recibido.id);

	free(buffer_recepcion);

	serializar_handshake(buffer_envio, yo);
	enviar(socket_cliente, buffer_envio, sizeof(int)*3, logger);

	free(buffer_envio);

	switch(proceso_recibido.proceso){
	case PLANIFICADOR:
		if(!conexion_hecha){
			conexion_hecha = 1;
			return 1;
		}else{
			return -1;
		}
		break;

	case INSTANCIA:
		log_info(logger, "Se establecio la conexion con una Instancia ");
		agregar_nueva_instancia(socket_cliente, proceso_recibido.id);
		return 1;
		break;

	case ESI:
		log_info(logger, "Se establecio la conexion con un ESI ");
		agregar_nuevo_esi(socket_cliente, proceso_recibido.id);
		return 1;
		break;

		default:
		return -1;
		break;
	}
}

void procesar_conexion(){
	int id_mensaje;
	while(1){
		int socket_cliente = aceptar_conexion(socket_escucha);
		recibir(socket_cliente, &id_mensaje, sizeof(int), logger);
		if(id_mensaje == 80){
			handshake(socket_cliente);
		}
	}
}

void atender_planificador(){
	log_info(logger, "Entre en el hilo del planificador");
	while(1){
		procesar_mensaje(socket_planificador);
	}
}

void atender_esi(void* datos_esi){
	int resultado = 1;
	log_info(logger, "Estoy en el hilo del esi!");
	hilo_proceso mis_datos = deserializar_hilo_proceso(datos_esi);
	while(resultado > 0){
		resultado = procesar_mensaje(mis_datos.socket);
	}
}

void atender_instancia(void* datos_instancia){
	log_info(logger, "Estoy en el hilo de la instancia!");
	hilo_proceso mis_datos = deserializar_hilo_proceso(datos_instancia);
	while(1){
		procesar_mensaje(mis_datos.socket);
	}
}

int desconectar_instancia(int socket){
	socket_instancia_buscado = socket;
	nodo* el_nodo = list_find(lista_instancias, condicion_socket_instancia);
	int resultado = pthread_join(el_nodo->hilo, NULL);
	log_info(logger, "ya tire el join: %d", resultado);
	return resultado;
}

/////////////////////////////////////////// FUNCIONES DE LISTAS ///////////////////////////////////////////

bool condicion_socket_esi(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.socket == socket_esi_buscado;
}

bool condicion_socket_instancia(void* datos){
	nodo un_nodo = *((nodo*) datos);
	return un_nodo.socket == socket_instancia_buscado;
}

bool verificar_existencia_instancia(nodo nodo){
	return list_count_satisfying(lista_instancias, condicion_socket_instancia) > 0;
}

void reemplazar_instancia(nodo un_nodo){
	list_remove_by_condition(lista_instancias, condicion_socket_instancia);
	list_add(lista_instancias, &un_nodo);
}

nodo* buscar_instancia(char* clave){
	nodo* nodo_instancia;
	if(dictionary_has_key(diccionario_claves, clave)){
		nodo_instancia = dictionary_get(diccionario_claves, clave);
	} else {
		nodo_instancia = seleccionar_instancia(clave);
	}

	return nodo_instancia;
}

nodo* seleccionar_instancia(char* clave){
	nodo* instancia_seleccionada;
	switch(info_coordinador.algoritmo_distribucion){
	case EL:
		log_info(logger, "Tamanio: %d", list_size(lista_instancias));
		//instancia_seleccionada = list_get(lista_instancias, ultima_instancia_EL);
		instancia_seleccionada = list_get(lista_instancias, 0);
		if(ultima_instancia_EL++ == list_size(lista_instancias)){ultima_instancia_EL = 0;}
		break;
	case LSU:
		break;
	case KE:
		break;
	}
	return instancia_seleccionada;
}

nodo* encontrar_esi(int socket){
	socket_esi_buscado = socket;
	nodo* el_nodo = list_find(lista_esis, condicion_socket_esi);
	log_info(logger, "Socket encontrado: %d, y su id: %d", el_nodo->socket, el_nodo->id);
	return el_nodo;
}

/////////////////////////////////////////// FUNCIONES DE HILOS ///////////////////////////////////////////

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
	void* buffer = malloc(sizeof(int)*2);
	serializar_hilo_proceso(buffer, datos_esi);
	pthread_t hilo_esi;
	pthread_create(&hilo_esi, 0, atender_esi, buffer); //(void*) &
	nodo* el_nodo = malloc(sizeof(nodo));
	el_nodo->socket = socket_esi;
	el_nodo->id = id_esi;
	el_nodo->hilo = hilo_esi;
	list_add(lista_esis, el_nodo);
}

void agregar_nueva_instancia(int socket_instancia, int id_instancia){
	hilo_proceso datos_instancia = {socket_instancia, id_instancia};
	void* buffer = malloc(sizeof(int)*2);
	serializar_hilo_proceso(buffer, datos_instancia);
	pthread_t hilo_instancia;
	pthread_create(&hilo_instancia, 0, atender_instancia, buffer);
	nodo* el_nodo = malloc(sizeof(nodo));
	el_nodo->socket = socket_instancia;
	el_nodo->id = id_instancia;
	el_nodo->hilo = hilo_instancia;
	list_add(lista_instancias, el_nodo);
	/*socket_instancia_buscado = nodo.socket;
	bool precencia_instancia = verificar_existencia_instancia(nodo);
	if(!precencia_instancia){
		list_add(lista_instancias, &nodo);
	}else{
		reemplazar_instancia(nodo);
	}*/
}

///////////////////////////////////////////////// INICIALIZACION /////////////////////////////////////////////////

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	FILE* archivo = fopen("Configuracion_coordinador.txt", "r");

	fscanf(archivo, "%s %d %d %d %d %d",
			info_coordinador.ip,
			&(info_coordinador.puerto_escucha),
			&(info_coordinador.algoritmo_distribucion),
			&(info_coordinador.cantidad_entradas),
			&(info_coordinador.tamano_entrada),
			&(info_coordinador.retardo));
	fclose(archivo);
}

void inicializar_coordinador(){
	socket_escucha = inicializar_servidor(info_coordinador.puerto_escucha, logger);
	lista_esis = list_create();
	lista_instancias = list_create();
	diccionario_claves = dictionary_create();
	ultima_instancia_EL = 0;
	inicializar_semaforos();
}

void inicializar_semaforos(){
	if (pthread_mutex_init(&m_cerrar_hilo, NULL) != 0) {printf("\n mutex init failed\n");}
	sem_init(&s_cerrar_hilo, 0, 0); //El primer 0 es para compartir solamente con mis hilos y el segundo es el valor
}

void conectar_planificador(){
	int socket_cliente = aceptar_conexion(socket_escucha);
	int protocolo;

	recibir(socket_cliente, &protocolo, sizeof(int), logger);
	if(protocolo == 80){
		int resultado = handshake(socket_cliente);
		if(resultado >= 0){
			socket_planificador = socket_cliente;
			log_info(logger, "Se establecio la conexion con el Planificdor");
		} else{
			log_info(logger, "Fallo en la conexion con el Planificdor");
		}
	} else{
		log_info(logger, "Fallo en la conexion con el Planificdor");
	}
}

///////////////////////////////////////////////// COMUNICACION /////////////////////////////////////////////////

int enviar_configuracion_instancia(int socket){
	datos_configuracion mensaje = {info_coordinador.tamano_entrada, info_coordinador.cantidad_entradas};
	void* buffer = malloc(sizeof(int)*3);
	serializar_configuracion_inicial_instancia(buffer, mensaje);
	int bytes_enviados = enviar(socket, buffer, sizeof(int)*3, logger);
	return bytes_enviados;
}

int enviar_pedido_esi(int esi_id, t_esi_operacion instruccion){
	pedido_esi pedido = {esi_id, instruccion};
	void* buffer = malloc(tamanio_buffer_instruccion(instruccion) + sizeof(int));
	serializar_pedido_esi(buffer, pedido);
	log_info(logger, "Serialice");
	int bytes_enviados = enviar(socket_planificador, buffer, sizeof(pedido), logger);
	return bytes_enviados;
}

int enviar_status_clave(int socket, status_clave status){
	int tamanio = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio);
	serializar_status_clave(buffer, status);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	return bytes_enviados;
}

int enviar_pedido_valor(int socket, char* clave, int id){
	int tamanio = tamanio_buffer_string(clave);
	void* buffer = malloc(tamanio);
	serializar_string(buffer, clave, id);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	return bytes_enviados;
}

int enviar_confirmacion(int socket, int confirmacion, int id){
	void* buffer = malloc(sizeof(int)*2);
	serializar_int(buffer, confirmacion, id);
	int bytes_enviados = enviar(socket, buffer, sizeof(int)*2, logger);
	return bytes_enviados;
}

int recibir_confirmacion(int socket){
	int confirmacion;
	void* buffer = malloc(sizeof(int));
	recibir(socket, buffer, sizeof(int), logger);
	confirmacion = deserializar_id(buffer);
	return confirmacion;
}

char* recibir_pedido_clave(int socket){
	char* clave;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	deserializar_string(buffer,clave);
	return clave;
}

status_clave recibir_status(int socket){
	status_clave status;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	status = deserializar_status_clave(buffer);
	return status;
}

t_esi_operacion recibir_instruccion(int socket){
	t_esi_operacion instruccion;
	int tamanio;
	void* buffercito = malloc(sizeof(int));
	recibir(socket, buffercito, sizeof(int), logger);
	tamanio = deserializar_id(buffercito);
	void* buffer = malloc(tamanio);
	int resultado = recibir(socket, buffer, tamanio, logger);
	instruccion = deserializar_instruccion(buffer);
	return instruccion;
}

void enviar_operacion(int socket, t_esi_operacion instruccion){
	int tamanio = tamanio_buffer_instruccion(instruccion);
	void* buffer = malloc(tamanio);
	serializar_instruccion(buffer, instruccion);
	enviar(socket, buffer, tamanio, logger);
}

