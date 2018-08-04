#include "planificador_punto_hache.h"

//////////----------COSAS POR HACER ANTES DE SEGUIR----------//////////

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
		//log_info(logger, "Estimacion inicial es: %d", estimacion_inicial);
		alpha = strtof(strdup(config_get_string_value(configuracion, "ALPHA")),NULL);
		//log_info(logger, "Alpha es: %f", alpha);
		char** las_claves_bloqueadas = config_get_array_value(configuracion, "CLAVES_BLOQUEADAS");
		//log_info(logger, "Obtuve las claves bloqueadas");
		int indice_bloq = 0;
		while(las_claves_bloqueadas[indice_bloq] != NULL){
			clave_bloqueada* clave = malloc(sizeof(clave_bloqueada));
			clave->esi_que_la_usa = -1;
			clave->esis_en_espera = list_create();
			clave->clave = malloc(strlen(las_claves_bloqueadas[indice_bloq])+1);
			memcpy(clave->clave, las_claves_bloqueadas[indice_bloq], strlen(las_claves_bloqueadas[indice_bloq])+1);
			list_add(claves_bloqueadas, clave);
			indice_bloq += 1;
			//log_info(logger, "Agregue a las claves_bloqueadas la clave: %s", clave->clave);
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
	sem_init(&s_planificar, 0, 0);
	sem_init(&s_podes_cerrar_dice_el_esi, 0, 0);
	sem_init(&s_esi_despedido, 0, 0);
}

void handshake_coordinador(int socket_coordinador){

	t_handshake proceso_recibido;
	t_handshake yo = {0, PLANIFICADOR};

	void* buffer = malloc(sizeof(int)*3);
	serializar_handshake(buffer, yo);
	//log_info(logger, "Serialice el Handshake del Coordinador");

	enviar(socket_coordinador, buffer, sizeof(int)*3, logger);
	//log_info(logger, "Envie Handshake al Coordinador");
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
			log_info(logger, "\n\n Se establecio la conexion con el Esi de id %d \n\n", proceso_recibido.id);
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


/////-----CERRAR-----/////
void desencadenar_cerrar_planificador(){
	void* envio = malloc(sizeof(int));
	serializar_id(envio, 20);
	enviar(sockets_planificador.socket_coordinador, envio, sizeof(int), logger);
	free(envio);
	cerrar_planificador();
}

void cerrar_planificador(){

	terminar_todo = -1;
	fin_de_programa = 1;

	list_iterate(pcbs, despedir_esi_vivo);
	log_info(logger, "Todos los ESISs finalizados");

	list_clean_and_destroy_elements(claves_bloqueadas, borrar_nodo_clave);
	log_info(logger, "Todas las claves borradas");
	log_info(logger, "El tamanio de la lista de claves_bloqueadas es %d", list_size(claves_bloqueadas));
	free(claves_bloqueadas);

	free(conexion_planificador.ip);
	free(conexion_planificador.puerto);
	free(conexion_coordinador.ip);
	free(conexion_coordinador.puerto);
	free(algoritmo);
	free(esis_ready);

	list_clean_and_destroy_elements(esis_finalizados, free_esi_finalizado);
	log_info(logger, "El tamanio de la lista de esis_finalizados es %d", list_size(esis_finalizados));
	free(esis_finalizados);

	list_clean_and_destroy_elements(pcbs, destruir_pcb);
	log_info(logger, "El tamanio de la lista de PCBs es %d", list_size(pcbs));
	free(pcbs);

	se_cerro_todo = 1;

	printf("\t El Planificador termino su ejecucion\n");
}

//--Cuando termina un ESI--//
void cerrar_cosas_de_un_esi(void* esi){
	pcb* esi_a_cerrar = esi;

	close(esi_a_cerrar->socket);
	hilo_a_cerrar = &esi_a_cerrar->hilo;
	hay_hilos_por_cerrar = 1;

	if(terminar_todo == 1){
		sem_post(&s_planificar);
	}
	sem_post(&s_cerrar_un_hilo);

	//log_info(logger, "Post a planificar desde cerrar cosas");
	//if(list_size(esis_ready) == 0 && terminar_todo == 1 && esi_a_abortar == -1){ //no se por que pero hacen falta 2
	//	sem_post(&s_planificar);
	//	log_info(logger, "Post a planificar desde cerrar cosas");
	//}
}
