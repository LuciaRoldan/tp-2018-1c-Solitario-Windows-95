#include "funciones_plani.h"

//////////----------COSAS POR HACER ANTES DE SEGUIR----------//////////
//1. TERMINAR DE LEER EL ARCHIVO DE CONFIGURACION Y VER QUE LEA BIEN = LISTA DE CLAVES INICIALMENTE BLOQUEADAS
//export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug


/////-----INICIALIZANDO-----/////

sockets inicializar_planificador(char* path){
	sockets sockets_planificador;
	leer_archivo_configuracion(path);
	sockets_planificador.socket_coordinador = connect_to_server(conexion_coordinador.ip, conexion_coordinador.puerto, logger);
	conectarse_al_coordinador(sockets_planificador.socket_coordinador);
	sockets_planificador.socket_esis = inicializar_servidor(atoi(conexion_planificador.puerto), logger); //pasar ip
	inicializar_semaforos();
	return sockets_planificador;
}

void leer_archivo_configuracion(char* path){
	t_config* configuracion = config_create(path);

		conexion_planificador.ip = strdup(config_get_string_value(configuracion,"IP_PLANIFICADOR"));
		conexion_planificador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_PLANIFICADOR"));
		conexion_coordinador.ip = strdup(config_get_string_value(configuracion,"IP_COORDINADOR"));
		conexion_coordinador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
		algoritmo = strdup(config_get_string_value(configuracion, "ALGORITMO_PLANIFICACION"));
		estimacion_inicial = atoi(strdup(config_get_string_value(configuracion, "ESTIMACION_INICIAL")));
		log_info(logger, "Estimacion inicial es: %d", estimacion_inicial);
		alpha = strtof(strdup(config_get_string_value(configuracion, "ALPHA")),NULL);
		log_info(logger, "Alpha es: %f", alpha);
		char** las_claves_bloqueadas = config_get_array_value(configuracion, "CLAVES_BLOQUEADAS");
		log_info(logger, "Obtuve las claves bloqueadas");
		int indice_bloq = 0;
		//int tam_lista;
		while(las_claves_bloqueadas[indice_bloq] != NULL){
			clave_bloqueada* clave = malloc(sizeof(clave_bloqueada));
			clave->esi_que_la_usa = -1;
			clave->esis_en_espera = list_create();
			clave->clave = malloc(strlen(las_claves_bloqueadas[indice_bloq])+1);
			memcpy(clave->clave, las_claves_bloqueadas[indice_bloq], strlen(las_claves_bloqueadas[indice_bloq])+1);
			list_add(claves_bloqueadas, clave);
			indice_bloq += 1;
			log_info(logger, "Agregue clave: %s", clave->clave);
		}
		free(las_claves_bloqueadas);

	log_info(logger, "Se leyo el archivo de configuracion correctamente");
}

void conectarse_al_coordinador(int socket_coordinador){
	handshake_coordinador(socket_coordinador);
}

void inicializar_semaforos(){
	if (pthread_mutex_init(&m_recibir_resultado_esi, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_hilo_a_cerrar, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_pcbs, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_esis_ready, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_claves_bloqueadas, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_clave_buscada, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_id_esi_ejecutando, NULL) != 0){log_info(logger,"Fallo al inicializar mutex\n");}

	sem_init(&s_cerrar_un_hilo, 0, 0); //el segundo numerito es el valor inicial. el primero es 0.
	sem_init(&s_hilo_cerrado, 0, 0);
	sem_init(&s_eliminar_pcb, 0, 0);
	sem_init(&s_podes_procesar_una_respuesta, 0, 0);
	sem_init(&s_podes_procesar_un_pedido, 0, 0);
	sem_init(&s_planificar, 0, 0);
}

void handshake_coordinador(int socket_coordinador){

	t_handshake proceso_recibido;
	t_handshake yo = {0, PLANIFICADOR};

	void* buffer = malloc(sizeof(int)*3);
	serializar_handshake(buffer, yo);
	//log_info(logger, "Serialice el Handshake del Coordinador");

	enviar(socket_coordinador, buffer, sizeof(int)*3, logger);
	log_info(logger, "Envie Handshake al Coordinador");
	free(buffer);

	int id_protocolo;
	recibir(socket_coordinador, &id_protocolo, sizeof(int), logger);

	if(id_protocolo == 80){
		void* buffer = malloc(sizeof(int)*2);
		recibir(socket_coordinador, buffer, sizeof(int)*2, logger);
		proceso_recibido = deserializar_handshake(buffer);
		free(buffer);

		//printf("Proceso recibido: %d", proceso_recibido.proceso);
		//printf("ID proceso recibido: %d", proceso_recibido.id);

		if (proceso_recibido.proceso ==  COORDINADOR){
				log_info(logger, "Se establecio la conexion con el Coordinador");
			} else {
				log_info(logger, "Se recibio el Handshake del Coordinador pero fallo");
			}
	} else {
		log_info(logger, "Error en el protocolo del Handshake del Coordinador");
	}
}

int handshake_esi(int socket_esi){

	t_handshake proceso_recibido;
	t_handshake yo = {0, 0};

	void* buffer1 = malloc(sizeof(int)*3);
	serializar_handshake(buffer1, yo);

	//log_info(logger, "Por enviar en Handshake al ESI");
	int protocolo;
	recibir(socket_esi, &protocolo, sizeof(int), logger);

	if(protocolo == 80){
		void* buffer2 = malloc(sizeof(int)*2);
		recibir(socket_esi, buffer2, sizeof(int)*2, logger);
		proceso_recibido = deserializar_handshake(buffer2);
		if (proceso_recibido.proceso !=  0){ //validar que no tenía ya al ESI en otra PCB
			enviar(socket_esi, buffer1, sizeof(int)*3, logger);
			log_info(logger, "---------Se establecio la conexion con el Esi de id %d \n\n\n", proceso_recibido.id);
			free(buffer1);
			free(buffer2);
			return proceso_recibido.id;
		} else {
			log_info(logger, "Se recibio el Handshake el Esi de id %d\n pero fallo", proceso_recibido.id);
			free(buffer1);
			free(buffer2);
			return -1;
		}
	} else {
		log_info(logger, "Error en el protocolo del Handshake del Esi");
		free(buffer1);
		return -1;
	}
}


/////-----FUNCIONAMIENTO-----/////


//--MANEJAR ESIS--//
void manejar_esis(){

	while(terminar_todo){
	while(pausar_planificador>=0){
		sleep(1);
		if(list_size(esis_ready) > 0){
		
		sem_wait(&s_planificar);

		if(list_size(esis_ready) > 0){

		planificar();
		log_info(logger, "Pase planificar()");

		void* esi_a_ejecutar = list_get(esis_ready, 0);
		pcb* pcb_esi;
		pcb_esi = esi_a_ejecutar;
		log_info(logger, "El Esi que va a ejecutar es: %d", pcb_esi->id);

		if ((pcb_esi->id != id_esi_ejecutando) && (id_esi_ejecutando != -1)){
			pthread_mutex_lock(&m_id_esi_ejecutando);
			pthread_mutex_lock(&m_id_buscado);
			id_buscado = id_esi_ejecutando;
			void* un_esi = list_find(pcbs, ids_iguales_pcb);
			pthread_mutex_unlock(&m_id_buscado);
			pthread_mutex_unlock(&m_id_esi_ejecutando);
			pcb* esi_que_se_fue = un_esi;
			esi_que_se_fue->ultimaRafaga = rafaga_actual;
			rafaga_actual = 0;
			log_info(logger, "Actualice la ultimaRafaga del Esi %d a: %d", esi_que_se_fue->id, esi_que_se_fue->ultimaRafaga);
		}
		pthread_mutex_lock(&m_id_esi_ejecutando);
		id_esi_ejecutando = pcb_esi->id;
		pthread_mutex_unlock(&m_id_esi_ejecutando);

		sumar_retardo_otros_ready();

		void* buffer = malloc(sizeof(int));
		serializar_id(buffer, 61);
		log_info(logger, "Voy a enviar al ESI de id %d y socket %d", pcb_esi->id, pcb_esi->socket); //BORRAR
		enviar(pcb_esi->socket, buffer, sizeof(int), logger);
		log_info(logger, "Solicitud de ejecucion enviada al ESI: %d", pcb_esi->id);
		rafaga_actual += 1;
		free(buffer);
		}
		}
	}
	}
}

//--RECIBIR ESIS--//
void recibir_esis(void* socket_esis){
	int int_socket_esis = *((int*) socket_esis);
	//log_info(logger, "Dentro de recibir_esis y el socket es %d\n", int_socket_esis);

	int socket_esi_nuevo;
	while(terminar_todo == 1){
		log_info(logger, "Esperando un ESI adentro de recibir_esis");

		socket_esi_nuevo = aceptar_conexion(int_socket_esis);

		if (socket_esi_nuevo > 0){
			int id_esi_nuevo;
			id_esi_nuevo = handshake_esi(socket_esi_nuevo);

			if (id_esi_nuevo){
				pthread_t hilo_escucha_esi;
				pcb* pcb_esi_nuevo;
				pcb_esi_nuevo = crear_pcb_esi(socket_esi_nuevo, id_esi_nuevo, hilo_escucha_esi);

				pthread_mutex_lock(&m_lista_pcbs);
				list_add(pcbs, pcb_esi_nuevo); //agrego PCB ESI a mi lista de ESIs
				pthread_mutex_unlock(&m_lista_pcbs);

				pthread_mutex_lock(&m_lista_esis_ready);
				calcular_estimacion(pcb_esi_nuevo);
				list_add(esis_ready, pcb_esi_nuevo); //agrego PCB ESI a cola ready
				pthread_mutex_unlock(&m_lista_esis_ready);

				if (pthread_create(&hilo_escucha_esi, 0 , manejar_esi, (void*) pcb_esi_nuevo) < 0){
					perror("No se pudo crear el hilo");
				}
			}
		} else { //socket_esi_nuevo < 0
	        perror("Fallo en el accept");
		}
	}
}

//--MANEJAR UN ESI--//
void manejar_esi(void* la_pcb){
	if(list_size(esis_ready) == 1){
		sem_post(&s_planificar);
	}

	log_info(logger, "Entre a manejar_esi");
	pcb pcb_esi = *((pcb*) la_pcb);
	int chau = 1;

	while(chau > 0){
		log_info(logger, "En manejar_esi y el ID del ESI es: %d", pcb_esi.id);

		void* buffer_resultado = malloc(sizeof(int));
		recibir(pcb_esi.socket, buffer_resultado, sizeof(int), logger);
		int resultado = deserializar_id(buffer_resultado);
		free(buffer_resultado);

		log_info(logger, "El ESI %d me envio el resultado_esi %d:", pcb_esi.id, resultado);

		if (resultado >= 0){
			switch (resultado){
				case (84):
					sem_post(&s_planificar);
				break;
				case (90):
					pthread_mutex_lock(&m_lista_esis_ready);
					mover_esi_a_bloqueados(clave_buscada, id_esi_ejecutando);
					sem_post(&s_planificar);
				break;
				case (81):
					mover_esi_a_finalizados(id_esi_ejecutando);
					chau = 0;
				break;
				default:
					abortar_esi(pcb_esi.id);
					procesar_motivo_aborto(resultado);
					chau = 0;
				break;
			}

		}
		
	}
}

//--MANEJAR COORDINADOR--//
void manejar_coordinador(void* socket){
	int socket_coordinador = *((int*) socket);
	//log_info(logger, "Entre al hilo manejar_coordinador y el socket es %d\n", socket_coordinador);
	log_info(logger, "Entre al hilo manejar_coordinador");
	int conexion_valida = 1;
	while(conexion_valida > 0){

		int id;
		void* buffer_int = malloc(sizeof(int));
		recibir(socket_coordinador, buffer_int, sizeof(int), logger);
		id = deserializar_id(buffer_int);
		free(buffer_int);
		log_info(logger, "Id recibido del Coordinador: %d", id);

		int tamanio;
		t_esi_operacion instruccion;
		void* un_buffer_int;
		void* buffer;
		switch(id){
		case (82): //nuevo pedido

				un_buffer_int = malloc(sizeof(int));
				recibir(socket_coordinador, un_buffer_int, sizeof(int), logger);
				tamanio = deserializar_id(un_buffer_int);
				free(un_buffer_int);

				buffer = malloc(tamanio);
				recibir(socket_coordinador, buffer, tamanio, logger);
				instruccion = deserializar_instruccion(buffer);
				free(buffer);
				log_info(logger, "Recibi del Coordinador: %s, %d", instruccion.argumentos.GET.clave, instruccion.keyword);

				procesar_pedido(instruccion);

				//liberar_instruccion(instruccion);

				conexion_valida = 82;
		break;
		case (83):
			recibir_status_clave();
			conexion_valida = 83;
		break;
		case (44):
			conexion_valida = -99;
		break;
		default:
		log_info(logger, "Pedido invalido del Coordinador");
		conexion_valida = -1;
		}
	}
	if(conexion_valida == -99){
		log_info(logger, "El Coordinador termino de ejecutar");
	} else {
	log_error(logger, "Se rompio todo");
	}
	cerrar_planificador();
	pthread_exit(NULL);
}

void procesar_pedido(t_esi_operacion instruccion){

	clave_bloqueada* nodo_clave_buscada;
	pcb* pcb_pedido_esi;

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi_ejecutando;
	pcb_pedido_esi = list_find(pcbs, ids_iguales_pcb);
	pthread_mutex_unlock(&m_id_buscado);

	if (pcb_pedido_esi != NULL){ //VERIFICO QUE EL ESI SEA VALIDO

	switch(instruccion.keyword){
	case(GET):

		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.GET.clave;
		//memcpy(clave_buscada, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);

		if(nodo_clave_buscada == NULL){
			//No existe el elemento clave_bloqueada para esa clave en mi lista de claves bloqueadas, la creo
			clave_bloqueada* clave_nueva = malloc(sizeof(clave_bloqueada));

			clave_buscada = instruccion.argumentos.GET.clave;
			//memcpy(clave_buscada, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);

			clave_nueva->esi_que_la_usa = pcb_pedido_esi->id;

			char* clave_esi_nueva = malloc(strlen(instruccion.argumentos.GET.clave)+1);
			memcpy(clave_esi_nueva, instruccion.argumentos.GET.clave, strlen(instruccion.argumentos.GET.clave)+1);
			clave_nueva->clave = clave_esi_nueva;
			clave_nueva->esis_en_espera = list_create();

			pthread_mutex_lock(&m_lista_claves_bloqueadas);
			list_add(claves_bloqueadas, clave_nueva);
			pthread_mutex_unlock(&m_lista_claves_bloqueadas);
			log_info(logger, "Nueva clave_bloqueada creada %s para el ESI %d", clave_buscada, clave_nueva->esi_que_la_usa);
			informar_exito_coordinador();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					//Existe esa clave_bloqueada en mi lista de claves pero no esta asignada a ningun ESI
					pthread_mutex_lock(&m_lista_claves_bloqueadas);

					nodo_clave_buscada->esi_que_la_usa = id_esi_ejecutando;

					pthread_mutex_unlock(&m_lista_claves_bloqueadas);
					log_info(logger, "Clave %s asignada al ESI %d", clave_buscada, id_esi_ejecutando);
					informar_exito_coordinador();
				}

				else {
					if(nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
					//Me hacen un GET sobre una clave que ya tenia asignado ese ESI entonces no hago nada.
					log_info(logger, "GET realizado por el ESI %d sobre la clave %s que ya le pertenecia", id_esi_ejecutando, clave_buscada);
					informar_exito_coordinador();
					}

					else {
						//Me hacen un GET sobre una clave que estaba asignada a otro ESI entonces lo pongo en
						//la lista de espera de esa clave y lo saco de ready
						log_info(logger, "GET realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						//clave_buscada se mantiene igual para que la use la funcion de mover_esi_a_bloqueados cuando recibo respuesta del esi!!
						informar_bloqueo_coordinador();
					}
				}
			}
		//free(instruccion.argumentos.GET.clave);
			break;

	case(SET):

		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.SET.clave;
		//memcpy(clave_buscada, instruccion.argumentos.SET.clave, strlen(instruccion.argumentos.SET.clave)+1);
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);
		log_info(logger, "Nodo_clave_buscada en SET el id del esi que la usa es: %d", nodo_clave_buscada->esi_que_la_usa);

		if(nodo_clave_buscada == NULL){
				log_info(logger, "SET realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, id_esi_ejecutando);
				informar_aborto_coordinador_clave_no_id();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					log_info(logger, "SET realizado por el ESI %d sobre una clave %s sin ESIS asignados", id_esi_ejecutando, clave_buscada);
					informar_aborto_coordinador_clave_no_b();
				}

				else {
					if (nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
						log_info(logger, "SET realizado por el ESI %d sobre la clave que le pertenecia %s", id_esi_ejecutando, clave_buscada);
						informar_exito_coordinador();
					}

					else {
						log_info(logger, "SET realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						informar_aborto_coordinador_clave_no_b();
					}
				}
			}
		//free(instruccion.argumentos.SET.clave);
		//free(instruccion.argumentos.SET.valor);
			break;

	case(STORE):
		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.STORE.clave;
		//memcpy(clave_buscada, instruccion.argumentos.SET.clave, strlen(instruccion.argumentos.SET.clave)+1);
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);

			if(nodo_clave_buscada == NULL){
				log_info(logger, "STORE realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, id_esi_ejecutando);
				informar_aborto_coordinador_clave_no_id();
			}

			else {
				if (nodo_clave_buscada->esi_que_la_usa == 0){
					log_info(logger, "STORE realizado por el ESI %d sobre una clave %s sin ESIS asignados", id_esi_ejecutando, clave_buscada);
					informar_aborto_coordinador_clave_no_b();
				}

				else {
					if (nodo_clave_buscada->esi_que_la_usa == id_esi_ejecutando){
						log_info(logger, "STORE realizado por el ESI %d sobre la clave que le pertenecia %s", id_esi_ejecutando, clave_buscada);
						pthread_mutex_lock(&m_clave_buscada);
						liberar_clave(clave_buscada);
						pthread_mutex_unlock(&m_clave_buscada);
						informar_exito_coordinador();
					}

					else {
						log_info(logger, "STORE realizado por el ESI %d sobre una clave que no le pertenece %s", id_esi_ejecutando, clave_buscada);
						informar_aborto_coordinador_clave_no_b();
					}
				}
			}
	break;
	}
	//free(instruccion.argumentos.STORE.clave);
	} else {
		log_info(logger, "Pedido invalido. No conozco al ESI %d", id_esi_ejecutando);
	}
}

void liberar_instruccion(t_esi_operacion instruccion){
	switch(instruccion.keyword){
	case GET:
		free(instruccion.argumentos.GET.clave);
		break;
	case SET:
		free(instruccion.argumentos.SET.clave);
		free(instruccion.argumentos.SET.valor);
		break;
	case STORE:
		free(instruccion.argumentos.STORE.clave);
		break;
	}
}

//--RESULTADOS PEDIDOS PARA EL COORDINADOR--//
void informar_aborto_coordinador_clave_no_id(){
	int aborto = 87;
	enviar(sockets_planificador.socket_coordinador, &aborto, sizeof(int), logger);
}
void informar_aborto_coordinador_clave_no_b(){
	int aborto = 89;
	enviar(sockets_planificador.socket_coordinador, &aborto, sizeof(int), logger);
}
void informar_exito_coordinador(){
	int exito = 84;
	enviar(sockets_planificador.socket_coordinador, &exito, sizeof(int), logger);
}
void informar_bloqueo_coordinador(){
	int fallo = 90;
	enviar(sockets_planificador.socket_coordinador, &fallo, sizeof(int), logger);
}
void informar_coordi_kill(int id_Esi){
	void* buffer_envio = malloc(sizeof(int)*2);
	serializar_int(buffer_envio, id_Esi, 91);
	enviar(sockets_planificador.socket_coordinador, buffer_envio, sizeof(int)*2, logger);
	free(buffer_envio);
}


/////-----OPERACIONES SOBRE PCBS-----/////

//--Crear PCB--//
pcb* crear_pcb_esi(int socket_esi_nuevo, int id_esi, pthread_t hilo_esi){
	pcb* pcb_esi;
	pcb_esi = malloc(sizeof(pcb));
	pcb_esi->id = id_esi;
	pcb_esi->socket = socket_esi_nuevo;
	pcb_esi->ultimaEstimacion = estimacion_inicial;
	pcb_esi->ultimaRafaga = estimacion_inicial;
	pcb_esi->hilo = hilo_esi;
	return pcb_esi;
}

//--Planificar--//
void planificar(){
	log_info(logger, "Dentro de planificar");
	if(list_size(esis_ready) > 0){
		ordenar_pcbs();
	}
}

//FUNCIONES AUXILIARES PCB//

//--Encontrar el ID de un ESI en las PCBs--//
bool ids_iguales_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	return pcb_esi->id == id_buscado;
}

//--Sumar 1 al retardo de los demas ESIs ready--//
void sumar_retardo_otros_ready(){
	pthread_mutex_lock(&m_lista_esis_ready);
	list_iterate(esis_ready, sumar_retardo_menos_primero);
	pthread_mutex_unlock(&m_lista_esis_ready);

}

void sumar_retardo_menos_primero(void* pcbb){
	pcb* pcb_esi = pcbb;
	pcb* el_primer_esi = list_get(esis_ready, 0);
	if (el_primer_esi->id != pcb_esi->id){
		pcb_esi->retardo+=1;
	}
}

void sumar_retardo(void* pcbb){
	pcb* pcb = pcbb;
	pcb->retardo+=1;
}

bool es_el_primer_esi_ready(void* pcbb){
	pcb* pcb_esi_ready = pcbb;
	return pcb_esi_ready->id == id_esi_ejecutando;
}

bool no_es_el_primer_esi_ready(void *pcbb){
	pcb* pcb_esi_ready = pcbb;
	return pcb_esi_ready->id != id_esi_ejecutando;
}


//--FUNCIONES PLANIFICACION--//
void calcular_estimacion(pcb* una_pcb){
	if(strcmp(algoritmo, "SJF_CD") == 0){
		calcular_estimacion_SJF(una_pcb);
	}
		else if(strcmp(algoritmo, "SJF_SD") == 0){
		calcular_estimacion_SJF(una_pcb);
	}
		else if(strcmp(algoritmo, "HRRN") == 0){
		calcular_estimacion_HRRN(una_pcb);
	}
		else{
		log_info(logger, "Algoritmo invalido en ordenar_pcbs");
	}
}

void ordenar_pcbs(){
	pthread_mutex_lock(&m_lista_esis_ready);
	if(strcmp(algoritmo, "SJF_CD") == 0){
	planificacionSJF_CD();
	}
	else if(strcmp(algoritmo, "SJF_SD") == 0){
	planificacionSJF_SD();
	}
	else if(strcmp(algoritmo, "HRRN") == 0){
	planificacionHRRN();
	}
	else{
	log_info(logger, "Algoritmo invalido en ordenar_pcbs");
	}
	pthread_mutex_unlock(&m_lista_esis_ready);

}

void planificacionSJF_CD(){
	log_info(logger, "Estoy en SJF_CD. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_SJF_CD);
	}
}

void planificacionSJF_SD(){
	log_info(logger, "Estoy en SJF_SD. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_SJF_SD);
	}
}

void planificacionHRRN(){
	log_info(logger, "Estoy en HRRN. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_HRRN);
	}
}

void calcular_estimacion_SJF(pcb* pcb_esi){
	float proxima_rafaga =  (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	log_info(logger, "La ultimaRafaga del esi %d es: %d", pcb_esi->id, pcb_esi->ultimaRafaga);
	log_info(logger, "La ultima estimacion del esi %d es: %f", pcb_esi->id, pcb_esi->ultimaEstimacion);
	pcb_esi->ultimaEstimacion = proxima_rafaga;
	log_info(logger, "La estimacion del ESI %d es: %f", pcb_esi->id, proxima_rafaga);
}

void calcular_estimacion_HRRN(pcb* pcb_esi){
	float estimacion = (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	float tiempo_de_respuesta = (pcb_esi->retardo + estimacion) / estimacion;
	pcb_esi->ultimaEstimacion = tiempo_de_respuesta;
	log_info(logger, "La estimacion del ESI %d es: %f", pcb_esi->id, tiempo_de_respuesta);
}

bool algoritmo_SJF_CD(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	if ( pcb1->ultimaEstimacion <= pcb2->ultimaEstimacion){
		return true;
	} else {
		return false;
	}
}

bool algoritmo_SJF_SD(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	if(es_el_primer_esi_ready(pcb1)){
		return true;
	} else if (es_el_primer_esi_ready(pcb2)){
		return false;
	} else {

		if ( pcb1->ultimaEstimacion <= pcb2->ultimaEstimacion){
			return true;
		} else {
			return false;
		}
	}
}


bool algoritmo_HRRN(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	if(es_el_primer_esi_ready(pcb1)){
		return true;
	} else if (es_el_primer_esi_ready(pcb2)){
		return false;
	} else {

		if(pcb1->ultimaEstimacion >= pcb2->ultimaEstimacion){
			return true;
		} else {
			return false;
		}
	}
	
}

/////-----MOVIENDO ESIS-----/////

//--Mover ESI a cola de bloqueados de una clave--//
void mover_esi_a_bloqueados(char* clave, int esi_id){
	pthread_mutex_lock(&m_clave_buscada);
	clave_buscada = clave;

	clave_bloqueada* nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	pthread_mutex_unlock(&m_clave_buscada);
	int* id = malloc(sizeof(int));
	memcpy(id, &esi_id, sizeof(int));
	list_add(nodo_clave_buscada->esis_en_espera, id);

	//int tam_lista = list_size(nodo_clave_buscada->esis_en_espera);
	//log_info(logger, "El tam de la lista de los esis_en_espera es %d", tam_lista);
	pthread_mutex_lock(&m_id_buscado);
	id_buscado = esi_id;
	list_remove_by_condition(esis_ready, ids_iguales_cola_de_esis);
	log_info(logger, "Esi %d colocado en cola de espera de la clave %s", esi_id, clave_buscada);
	log_info(logger, "La cantidad de ESIs ready es: %d", list_size(esis_ready));

	pthread_mutex_unlock(&m_id_buscado);
	pthread_mutex_unlock(&m_lista_esis_ready); //arranca en mover a bloqueados
}

//--Abortar ESI--//
void abortar_esi(int id_esi){
	pcb* esi_abortado;

	//pcb* primer_elem = list_get(pcbs, 0);
	//log_info(logger, "El ID del primer ESI en la lista de PCBs es: %d", primer_elem->id);

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_abortado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_abortado = esi_abortado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	log_info(logger, "ESI abortado: %d", esi_abortado->id);

	//list_remove_by_condition(pcbs, ids_iguales_pcb);
	list_remove_by_condition(esis_ready, ids_iguales_pcb);

	pthread_mutex_unlock(&m_id_buscado);
	//free(esi_abortado); NO ME DEJA HACER FREE

	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_abortado, sizeof(int));
	list_add(esis_finalizados, id); //va o no?
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_abortado);

	cerrar_cosas_de_un_esi(esi_abortado);
	//sem_wait(&s_eliminar_pcb);
	//list_remove_and_destroy_by_condition(pcbs, ids_iguales_pcb, destruir_pcb); //Esto estaba comentado
}

//--Mover ESI a finalizados--//
void mover_esi_a_finalizados(int id_esi){
	pcb* esi_finalizado;

	//pcb* primer_elem = list_get(pcbs, 0);
	//log_info(logger, "El ID del primer ESI en la lista de PCBs es: %d", primer_elem->id);

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_finalizado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_finalizado = esi_finalizado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	
	log_info(logger, "ESI finalizado: %d", esi_finalizado->id);

	pthread_mutex_lock(&m_lista_esis_ready);
	list_remove_by_condition(esis_ready, ids_iguales_pcb);
	pthread_mutex_unlock(&m_lista_esis_ready);

	//list_remove_by_condition(pcbs, ids_iguales_pcb);

	pthread_mutex_unlock(&m_id_buscado);

	id_buscado = id_esi_finalizado;
	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_finalizado, sizeof(int));
	list_add(esis_finalizados, id);
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_finalizado);

	cerrar_cosas_de_un_esi(esi_finalizado);
	//sem_wait(&s_eliminar_pcb);
}

//--Sacar ESI de la cola de bloqueados de una clave--//
void quitar_esi_de_cola_bloqueados(void* clave_bloq){
	clave_bloqueada* clave = clave_bloq;
	if(!list_is_empty(clave->esis_en_espera)){
		if(list_find(clave->esis_en_espera, ids_iguales_cola_de_esis) != NULL){
			list_remove_by_condition(clave->esis_en_espera, ids_iguales_cola_de_esis);
		}
	}
}

//--Informar motivo aborto ESI--//
void procesar_motivo_aborto(int protocolo){
	switch (protocolo){
	case(86):
		log_info(logger, "ESI abortado por fallo en el tamanio de la clave");
		break;
	case(87):
		log_info(logger, "ESI abortado por clave no identificada");
		break;
	case(88):
		log_info(logger, "ESI abortado por clave inaccesible");
		break;
	case (89):
		log_info(logger, "ESI abortado por clave no bloqueada");
		break;
	default:
		log_info(logger, "ESI abortado por ESI desconocido");
		break;
	}
}


/////-----OPERACIONES SOBRE CLAVE_BLOQUEADA-----/////
void liberar_clave(char* clave){
	//hacer el lock ANTES de llamar a esta funcion
	//memcpy(clave_buscada, clave, strlen(clave)+1);
	clave_buscada = clave;
	clave_bloqueada* nodo_clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);

	log_info(logger, "El ESI que ocupaba la clave era: %d", nodo_clave->esi_que_la_usa);

	nodo_clave->esi_que_la_usa = 0;
	log_info(logger, "La clave %s se libero", clave);
	//log_info(logger, "Ahora es ocupada por (deberia ser 0): %d", nodo_clave->esi_que_la_usa);

	if(!list_is_empty(nodo_clave->esis_en_espera)){
		void* el_esi = list_remove(nodo_clave->esis_en_espera, 0);
		int* id_esi_ahora_ready = el_esi;
		nodo_clave->esi_que_la_usa = *id_esi_ahora_ready;
		id_buscado = *id_esi_ahora_ready;
		log_info(logger, "y es ahora ocupada por el ESI %d que es lo mismo que %d", nodo_clave->esi_que_la_usa, *id_esi_ahora_ready);
		void* un_esi = list_find(pcbs, ids_iguales_pcb);
		pcb* el_nuevo_esi_ready = un_esi;
		calcular_estimacion(el_nuevo_esi_ready);
		list_add(esis_ready, el_nuevo_esi_ready);
		if(list_size(esis_ready) == 1){
			sem_post(&s_planificar);
		}
	}
}

//FUNCIONES AUXILIARES CLAVE_BLOQUEADA//
bool claves_iguales_nodo_clave(void* nodo_clave){
	//int tamanio = list_size(claves_bloqueadas);
	//log_info(logger, "El tamanio de la lista de claves bloqueadas es %d", tamanio);

	clave_bloqueada* una_clave = (clave_bloqueada*) nodo_clave;
	//log_info(logger, "La clave_buscada es: %s", clave_buscada);
	//log_info(logger, "La clave que estoy comparando es: %s", una_clave->clave);

	if(strcmp(una_clave->clave, clave_buscada) == 0){
		return true;
	} else {
		return false;
	}
}

//--Buscar ESI en una cola de esis bloqueados--//
bool ids_iguales_cola_de_esis(void* id){
	int* id_esi = (int*) id;
	return *id_esi == id_buscado;
}

/////-----COSAS PARA LA CONSOLA-----/////

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

//--Cerrar cosas--//

void desencadenar_cerrar_planificador(){
	void* envio = malloc(sizeof(int));
	serializar_id(envio, 20);
	enviar(sockets_planificador.socket_coordinador, envio, sizeof(int), logger);
	free(envio);
}

void cerrar_planificador(){

	terminar_todo = -1;
	fin_de_programa = 1;
	log_info(logger, "Pase la parte del exit que no puede fallar");

	log_info(logger, "LA CANTIDAD DE PCBS ES %d", list_size(pcbs));
	pcb* lolol = list_get(pcbs, 0);
	log_info(logger, "EL SOCKET DEL PRIMERO ES %d", lolol->socket);
	list_iterate(pcbs, despedir_esi);
	log_info(logger, "Despedi esis");

	list_clean_and_destroy_elements(claves_bloqueadas, borrar_nodo_clave);
	log_info(logger, "el tam de claves_bloqueadas es %d", list_size(claves_bloqueadas));

	//free(claves_bloqueadas);
	free(conexion_planificador.ip);
	free(conexion_planificador.puerto);
	free(conexion_coordinador.ip);
	free(conexion_coordinador.puerto);
	free(algoritmo);
	free(clave_buscada);
	//free(esi_abortado);

	free(esis_ready);
	list_iterate(pcbs, cerrar_cosas_de_un_esi);
	list_clean_and_destroy_elements(esis_finalizados, free_esi_finalizado);
	log_info(logger, "el tam de esis_finalizados es %d", list_size(esis_finalizados));

	list_clean_and_destroy_elements(pcbs, destruir_pcb);
	//log_info(logger, "el tam de pcbs es %d", list_size(pcbs));
	free(hilo_a_cerrar);

}

void free_esi_finalizado(void* id){
	int* id_finalizado = id;
	free(id_finalizado);
}

void despedir_esi(void* esi){
	pcb* pcb_esi = esi;
	void* envio = malloc(sizeof(int));
	serializar_id(envio, 85);
	enviar(pcb_esi->socket, envio, sizeof(int), logger);
	free(envio);
}

void borrar_nodo_clave(void* clave){
	clave_bloqueada* clave_bloq = clave;
	free(clave_bloq->clave);
	list_iterate(clave_bloq->esis_en_espera, eliminar_esi_en_espera);
	free(clave_bloq);
}

void eliminar_esi_en_espera(void* esi){
	int* esi_en_espera = esi;
	free(esi_en_espera);
}

//--Destruir PCB y liberar memoria--//
void destruir_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	free(pcb_esi);
}

//--Cerrar hilo y socket ESI abortado--//
void cerrar_cosas_de_un_esi(void* esi){
	pcb* esi_a_cerrar = esi;
	close(esi_a_cerrar->socket);
	log_info(logger, "Entre en cerrar_cosas");
	//pthread_mutex_lock(&m_hilo_a_cerrar);
	hilo_a_cerrar = &esi_a_cerrar->hilo;
	log_info(logger, "hola");
	hay_hilos_por_cerrar = 1;

		sem_post(&s_planificar);
		log_info(logger, "Post a planificar desde cerrar cosas");
	if(list_size(esis_ready) == 0){ //no se por que pero hacen falta 2
		sem_post(&s_planificar);
		log_info(logger, "Post a planificar desde cerrar cosas");
	}
	log_info(logger, "Post a cerrar_un_hilo");
	sem_post(&s_cerrar_un_hilo);
	sem_wait(&s_hilo_cerrado);
}

/////-----FUNCIONES QUE NO SIRVEN?-----/////
//--Registrar exito ESIS--//
//void registrar_exito_en_pcb(int id_esi){
//	sumar_ultima_rafaga(id_esi);
//}

//--Sumar 1 a la ultima rafaga del ESI--//
//void sumar_ultima_rafaga(int id_esi){
//	void* pcbb;
//	pthread_mutex_lock(&m_id_buscado);
//	id_buscado = id_esi;
//	pcbb = list_find(pcbs, ids_iguales_pcb);
//	pthread_mutex_unlock(&m_id_buscado);
//	pcb* esi_que_ejecuto = pcbb;
//	esi_que_ejecuto->ultimaRafaga+=1;
//	//log_info(logger, "La ultima rafaga del ESI: %d que acaba de intentar ejecutar es: %d", esi_que_ejecuto->id, esi_que_ejecuto->ultimaRafaga);
//}

//void actualizar_ultima_estimacion_SJF(){
//	pthread_mutex_lock(&m_lista_pcbs);
//	list_iterate(esis_ready, calcular_estimacion_SJF);
//	pthread_mutex_unlock(&m_lista_pcbs);
//}

//void actualizar_ultima_estimacion_HRRN(){
//	pthread_mutex_lock(&m_lista_pcbs);
//	list_iterate(esis_ready, calcular_estimacion_HRRN);
//	pthread_mutex_unlock(&m_lista_pcbs);
//}

