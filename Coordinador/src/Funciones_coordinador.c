#include "Funciones_coordinador.h"


////////////////////////////////////////////////// PROCESAMIENTO ///////////////////////////////////////////////////

int procesar_mensaje(int socket){
	int resultado, id;
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
		case 21: //Recibo una clave
			clave = recibir_pedido_clave(socket);
			nodo_instancia = buscar_instancia(clave);
			protocolo_extra = 1;
			resultado = enviar_pedido_valor(nodo_instancia->socket, clave, protocolo_extra);
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
			//Falta implementar, minimizar uso
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

		case 90://Hay que bloquear al ESI
			log_info(logger, "Le digo al ESI %d que esta bloqueado", esi_ejecutando->id);
			rta_esi = 90;
			buffer_int = malloc(sizeof(int));
			serializar_id(buffer_int, rta_esi);
			resultado = enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
			free(buffer_int);
			pthread_mutex_lock(&m_hilo_a_cerrar);
			hilo_a_cerrar = &esi_ejecutando->hilo;
			pthread_mutex_unlock(&m_instancia_seleccionada);
			pthread_mutex_unlock(&m_esi_ejecutando);
			return resultado;

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
		pthread_mutex_unlock(&m_esi_ejecutando);
		free(buffer_int);
		return -1;
	} else {
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
		pthread_mutex_lock(&m_operacion_ejecutando);
		operacion_ejecutando = instruccion;
		enviar_operacion(socket_planificador, instruccion);
	}
	if(!clave_accesible(clave)) {
		log_info(logger, "Fallo por clave inaccesible, ID ESI: %d", esi_ejecutando->id);
		rta_esi = 88;
		void* buffer_int = malloc(sizeof(int));
		serializar_id(buffer_int, rta_esi);
		enviar(esi_ejecutando->socket, buffer_int, sizeof(int), logger);
		free(buffer_int);
		pthread_mutex_lock(&m_hilo_a_cerrar);
		hilo_a_cerrar = &esi_ejecutando->hilo;
		sem_post(&s_cerrar_hilo);
		pthread_mutex_unlock(&m_instancia_seleccionada);
		pthread_mutex_unlock(&m_esi_ejecutando);
		return -1;
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

//////////////////////////////////////////////////// CONEXION /////////////////////////////////////////////////////

int handshake(int socket_cliente){
	int conexion_hecha = 0;

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
		log_info(logger, "Se establecio la conexion con la Instancia %d", proceso_recibido.id);
		agregar_nueva_instancia(socket_cliente, proceso_recibido.id);
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
	log_error(logger, "Fallo en la conexion con el Planificador");
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
	//pthread_exit(NULL);
}

void atender_instancia(void* datos_instancia){
	hilo_proceso mis_datos = deserializar_hilo_proceso(datos_instancia);
	free(datos_instancia);
	log_info(logger, "Hilo de la Instancia %d creado", mis_datos.id);
	enviar_configuracion_instancia(mis_datos.socket);
	int resultado = 1;
	while(resultado > 0 && !terminar_programa){
		resultado = procesar_mensaje(mis_datos.socket);
	}
	log_info(logger, "RIP instancia");
	desconectar_instancia(mis_datos.socket);
	//pthread_exit(NULL);
}

void desconectar_instancia(int socket){
	pthread_mutex_lock(&m_socket_instancia_buscado);
	socket_instancia_buscado = socket;
	pthread_mutex_lock(&m_lista_instancias);
	nodo* el_nodo = list_find(lista_instancias, condicion_socket_instancia);
	pthread_mutex_unlock(&m_lista_instancias);
	pthread_mutex_unlock(&m_socket_instancia_buscado);
	close(socket);
	hilo_a_cerrar = &el_nodo->hilo;
	sem_post(&s_cerrar_hilo);
}

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
	lista_claves = list_create();
	ultima_instancia_EL = 0;
	terminar_programa = 0;
	inicializar_semaforos();
}

void inicializar_semaforos(){
	if (pthread_mutex_init(&m_operacion_ejecutando, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_esi_ejecutando, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_instancia_seleccionada, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_hilo_a_cerrar, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_ultima_instancia_EL, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_claves, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_socket_instancia_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_socket_esi_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_instancias, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_esis, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_log_operaciones, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_clave_buscada, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_id_instancia_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
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
	free(buffer);
	return bytes_enviados;
}

int enviar_status_clave(int socket, status_clave status){
	int tamanio = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio);
	serializar_status_clave(buffer, status);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return bytes_enviados;
}

int enviar_pedido_valor(int socket, char* clave, int id){
	int tamanio = tamanio_buffer_string(clave);
	void* buffer = malloc(tamanio);
	serializar_string(buffer, clave, id);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return bytes_enviados;
}

int enviar_confirmacion(int socket, int confirmacion, int id){
	void* buffer = malloc(sizeof(int)*2);
	serializar_int(buffer, confirmacion, id);
	int bytes_enviados = enviar(socket, buffer, sizeof(int)*2, logger);
	free(buffer);
	return bytes_enviados;
}

int recibir_confirmacion(int socket){
	int confirmacion;
	void* buffer = malloc(sizeof(int));
	recibir(socket, buffer, sizeof(int), logger);
	confirmacion = deserializar_id(buffer);
	free(buffer);
	return confirmacion;
}

char* recibir_pedido_clave(int socket){
	char* clave;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	deserializar_string(buffer,clave);
	free(buffer);
	return clave;
}

status_clave recibir_status(int socket){
	status_clave status;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	status = deserializar_status_clave(buffer);
	free(buffer);
	return status;
}

t_esi_operacion recibir_instruccion(int socket){
	t_esi_operacion instruccion;
	int tamanio;
	void* buffercito = malloc(sizeof(int));
	recibir(socket, buffercito, sizeof(int), logger);
	tamanio = deserializar_id(buffercito);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	instruccion = deserializar_instruccion(buffer);
	free(buffer);
	free(buffercito);
	return instruccion;
}

int enviar_operacion(int socket, t_esi_operacion instruccion){
	int tamanio = tamanio_buffer_instruccion(instruccion);
	void* buffer = malloc(tamanio);
	serializar_instruccion(buffer, instruccion);
	int resultado = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return resultado;
}

