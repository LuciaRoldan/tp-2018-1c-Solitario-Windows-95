#include "funciones_plani.h"
#include <commons/string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//////////----------COSAS POR HACER ANTES DE SEGUIR----------//////////
//1. TERMINAR DE LEER EL ARCHIVO DE CONFIGURACION Y VER QUE LEA BIEN = LISTA DE CLAVES INICIALMENTE BLOQUEADAS
//export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug


/////-----INICIALIZANDO-----/////

sockets inicializar_planificador(){
	sockets sockets_planificador;
	leer_archivo_configuracion();
	sockets_planificador.socket_coordinador = connect_to_server(conexion_coordinador.ip, conexion_coordinador.puerto, logger);
	conectarse_al_coordinador(sockets_planificador.socket_coordinador);
	sockets_planificador.socket_esis = inicializar_servidor(atoi(conexion_planificador.puerto), logger); //pasar ip
	inicializar_semaforos();
	return sockets_planificador;
}

void leer_archivo_configuracion(){
	t_config* configuracion = config_create("config_planificador");

		conexion_planificador.ip = strdup(config_get_string_value(configuracion,"IP_PLANIFICADOR"));
		conexion_planificador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_PLANIFICADOR"));
		conexion_coordinador.ip = strdup(config_get_string_value(configuracion,"IP_COORDINADOR"));
		conexion_coordinador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
		algoritmo = strdup(config_get_string_value(configuracion, "ALGORITMO_PLANIFICACION"));
		estimacion_inicial = atoi(strdup(config_get_string_value(configuracion, "ESTIMACION_INICIAL")));
		log_info(logger, "Estimacion inicial es: %d", estimacion_inicial);
		alpha = strtof(strdup(config_get_string_value(configuracion, "ALPHA")),NULL);
		log_info(logger, "Alpha es: %f", alpha);
		//falta leer las claves inicialmente bloqueadas

	log_info(logger, "Se leyo el archivo de configuracion correctamente");
}

void conectarse_al_coordinador(int socket_coordinador){
	handshake_coordinador(socket_coordinador);
}

void inicializar_semaforos(){
	if (pthread_mutex_init(&m_recibir_resultado_esi, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_hilo_a_cerrar, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_pcbs, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_esis_ready, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_claves_bloqueadas, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_clave_buscada, NULL) != 0){printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_id_esi_ejecutando, NULL) != 0){printf("Fallo al inicializar mutex\n");}

	sem_init(&s_cerrar_un_hilo, 0, 0); //el segundo numerito es el valor inicial. el primero es 0.
	sem_init(&s_hilo_cerrado, 0, 0);
	sem_init(&s_eliminar_pcb, 0, 0);
	sem_init(&s_podes_procesar_una_respuesta, 0, 0);
	sem_init(&s_podes_procesar_un_pedido, 0, 0);
	sem_init(&s_planificar, 0, 1);
	//sem_init(&s_fijate_si_hay_esis, 0, 1);
	//sem_init(&s_procesa_un_pedido, 0, 0);
	//sem_init(&s_coordi_manejado, 0, 0);
}

void handshake_coordinador(int socket_coordinador){

	t_handshake proceso_recibido;
	t_handshake yo = {0, PLANIFICADOR};

	void* buffer = malloc(sizeof(int)*3);
	serializar_handshake(buffer, yo);
	log_info(logger, "Serialice el Handshake del Coordinador");

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

		printf("proceso recibido %d \n", proceso_recibido.proceso);
		printf("id proceso recibido %d \n", proceso_recibido.id);

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

	log_info(logger, "Por enviar en Handshake al ESI");
	int protocolo;
	recibir(socket_esi, &protocolo, sizeof(int), logger);

	if(protocolo == 80){
		void* buffer2 = malloc(sizeof(int)*2);
		recibir(socket_esi, buffer2, sizeof(int)*2, logger);
		proceso_recibido = deserializar_handshake(buffer2);
		if (proceso_recibido.proceso !=  0){ //validar que no tenía ya al ESI en otra PCB
			enviar(socket_esi, buffer1, sizeof(int)*3, logger);
			log_info(logger, "Se establecio la conexion con el Esi de id %d\n", proceso_recibido.id);
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

	while(1){
		if(list_size(esis_ready) > 0){
		log_info(logger, "En manejar ESI");

			sem_wait(&s_planificar);
			planificar();
			//sem_post(&s_podes_procesar_un_pedido);
			log_info(logger, "Podes procesar un mensaje desde manejar_esis");

			sleep(1);
		}
	}
}

//--RECIBIR ESIS--//
void recibir_esis(void* socket_esis){
	int int_socket_esis = *((int*) socket_esis);
	log_info(logger, "Entre al hilo recibir_esis y el socket es %d\n", int_socket_esis);

	int socket_esi_nuevo;
	while(1){
		//log_info(logger, "holaaaaaaaaa");
		//sem_wait(&s_fijate_si_hay_esis);
		log_info(logger, "Esperando un ESI. Adentro de recibir_esis");

		//sleep(1);
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
				list_add(esis_ready, pcb_esi_nuevo); //agrego PCB ESI a cola ready
				pthread_mutex_unlock(&m_lista_esis_ready);

				if (pthread_create(&hilo_escucha_esi, 0 , manejar_esi, (void*) pcb_esi_nuevo) < 0){
					perror("No se pudo crear el hilo");
				}
			}
		} else { //socket_esi_nuevo < 0
	        perror("Fallo en el accept");
		}
		//sem_post(&s_podes_procesar_un_mensaje);
		//log_info(logger, "Podes procesar un mensaje");
		//sem_post(&s_planificar);
	}
}

//--MANEJAR UN ESI--//
void manejar_esi(void* la_pcb){
	log_info(logger, "Entre a manejar ESI");
	pcb pcb_esi = *((pcb*) la_pcb);
	int chau = 1;

	while(chau){
		log_info(logger, "En manejar ESI: %d", pcb_esi.id);
		//sem_wait(&s_podes_procesar_una_respuesta);

		//int cant_ready;
		//cant_ready = list_size(esis_ready);

		//if(cant_ready>1){

		//planificar();
		//sem_post(&s_procesa_un_pedido);
		//log_info(logger, "Procesa un pedido del Cordi");
		//sem_wait(&s_coordi_manejado);
		sleep(1);

		void* buffer_resultado = malloc(sizeof(int));
		recibir(pcb_esi.socket, buffer_resultado, sizeof(int), logger);
		int resultado = deserializar_id(buffer_resultado);
		free(buffer_resultado);//PELIGRO

		log_info(logger, "El ESI me envio el resultado_esi %d", resultado);

		if (resultado >= 0){
			switch (resultado){
				case (84):
					pthread_mutex_lock(&m_lista_pcbs);
					registrar_exito_en_pcb(pcb_esi.id);
					pthread_mutex_unlock(&m_lista_pcbs);
				break;
				case (90):
					mover_esi_a_bloqueados(clave_buscada, id_esi_ejecutando);
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
		sem_post(&s_planificar);
		//log_info(logger, "Fijate si hay esis desde manejar_esis");
	}
}

//--MANEJAR COORDINADOR--//
void manejar_coordinador(void* socket){
	int socket_coordinador = *((int*) socket);
	log_info(logger, "Entre al hilo manejar_coordinador y el socket es %d\n", socket_coordinador);
	bool conexion_valida = true;
	while(conexion_valida){

		//sem_wait(&s_procesa_un_pedido);

		int id;
		void* buffer_int = malloc(sizeof(int));
		recibir(socket_coordinador, buffer_int, sizeof(int), logger);
		id = deserializar_id(buffer_int);
		free(buffer_int); //PELIGRO
		log_info(logger, "Id recibido del Coordinador: %d", id);

		int tamanio;
		t_esi_operacion instruccion;
		void* un_buffer_int;
		void* buffer;
		switch(id){
		case (82): //nuevo pedido
				//sem_wait(&s_podes_procesar_un_pedido);
				un_buffer_int = malloc(sizeof(int));
				recibir(socket_coordinador, un_buffer_int, sizeof(int), logger);
				tamanio = deserializar_id(un_buffer_int);
				free(un_buffer_int); //PELIGRO

				buffer = malloc(tamanio);
				recibir(socket_coordinador, buffer, tamanio, logger);
				instruccion = deserializar_instruccion(buffer);
				free(buffer);
				log_info(logger, "Recibido: %s, %d \n", instruccion.argumentos.GET.clave, instruccion.keyword);

				procesar_pedido(instruccion);
				//sem_post(&s_podes_procesar_una_respuesta);
				//sem_post(&s_coordi_manejado);
				//log_info(logger, "Podes procesar un mensaje desde manejar_coordinador");
				log_info(logger, "Procese pedido");
		break;
		case (83):
			recibir_status_clave();
		break;
		default:
		log_info(logger, "Pedido invalido del Coordinador");
		conexion_valida = false;
		}
	}
	log_error(logger, "Se rompio todo");
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
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		pthread_mutex_unlock(&m_clave_buscada);

		if(nodo_clave_buscada == NULL){
			//No existe el elemento clave_bloqueada para esa clave en mi lista de claves bloqueadas, la creo
			clave_bloqueada* clave_nueva = malloc(sizeof(clave_bloqueada));
			clave_buscada = instruccion.argumentos.GET.clave;
			clave_nueva->esi_que_la_usa = id_esi_ejecutando;
			clave_nueva->clave = clave_buscada;
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
			break;

	case(SET):

		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.SET.clave;
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
			break;

	case(STORE):
		pthread_mutex_lock(&m_clave_buscada);
		clave_buscada = instruccion.argumentos.STORE.clave;
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
						log_info(logger, "STORE realizado por el ESI %d sobre una clave que no le pertenece %d", id_esi_ejecutando, clave_buscada);
						informar_aborto_coordinador_clave_no_b();
					}
				}
			}
	break;
	}
		//pthread_mutex_unlock(&m_recibir_resultado_esi);
		//log_info(logger, "Lock al m_recibir_resultado_esi");
	} else {
		log_info(logger, "Pedido invalido. No conozco al ESI %d", id_esi_ejecutando);
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


/////-----OPERACIONES SOBRE PCBS-----/////

//--Crear PCB--//
pcb* crear_pcb_esi(int socket_esi_nuevo, int id_esi, pthread_t hilo_esi){
	pcb* pcb_esi;
	pcb_esi = malloc(sizeof(pcb));
	pcb_esi->id = id_esi;
	pcb_esi->socket = socket_esi_nuevo;
	pcb_esi->ultimaEstimacion = estimacion_inicial;
	pcb_esi->ultimaRafaga = 0;
	pcb_esi->hilo = hilo_esi;
	return pcb_esi;
}

//--Registrar exito ESIS--//
void registrar_exito_en_pcb(int id_esi){
	sumar_ultima_rafaga(id_esi);
}

//--Planificar--//
void planificar(){
	//export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug
	log_info(logger, "Planificando");
	ordenar_pcbs();
	sumar_retardo_otros_ready();
	pcb* pcb_esi;
	void* esi_a_ejecutar = list_get(esis_ready, 0);
	pcb_esi = esi_a_ejecutar;
	pthread_mutex_lock(&m_id_esi_ejecutando);
	id_esi_ejecutando = pcb_esi->id;
	pthread_mutex_unlock(&m_id_esi_ejecutando);
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 61);
	enviar(pcb_esi->socket, buffer, sizeof(int), logger);
	log_info(logger, "Solicitud de ejecucion enviada al ESI: %d", pcb_esi->id);
	free(buffer); //PELIGRO
}

//FUNCIONES AUXILIARES PCB//

//--Encontrar el ID de un ESI en las PCBs--//
bool ids_iguales_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	return pcb_esi->id == id_buscado;
}

//--Sumar 1 a la ultima rafaga del ESI--//
void sumar_ultima_rafaga(int id_esi){
	void* pcbb;
	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	pcbb = list_find(pcbs, ids_iguales_pcb);
	pthread_mutex_unlock(&m_id_buscado);
	pcb* esi_que_ejecuto = pcbb;
	esi_que_ejecuto->ultimaRafaga+=1;
	log_info(logger, "La ultima rafaga del ESI: %d que acaba de intentar ejecutar es: %d", esi_que_ejecuto->id, esi_que_ejecuto->ultimaRafaga);
}

//--Sumar 1 al retardo de los demas ESIs ready--//
void sumar_retardo_otros_ready(){
	pthread_mutex_lock(&m_lista_esis_ready);
	t_list* esis_ready_menos_primero = list_filter(esis_ready, no_es_el_primer_esi_ready);
	pthread_mutex_unlock(&m_lista_esis_ready);
	list_iterate(esis_ready_menos_primero, sumar_retardo);
	free(esis_ready_menos_primero); //PELIGRO
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

void ordenar_pcbs(){
	pthread_mutex_lock(&m_lista_esis_ready);
	log_info(logger, "El algoritmo es: %s", algoritmo);
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
		log_info(logger, "Hay mas de un ESI ready");
		list_sort(esis_ready, algoritmo_SJF_CD);
		actualizar_ultima_estimacion_SJF();
	}
}

void planificacionSJF_SD(){
	log_info(logger, "Estoy en SJF_SD. La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_SJF_SD);
		actualizar_ultima_estimacion_SJF();
	}
}

void planificacionHRRN(){
	if(list_size(esis_ready) > 1){
		log_info(logger, "Hay mas de un ESI ready");
		list_sort(esis_ready, algoritmo_HRRN);
		actualizar_ultima_estimacion_HRRN();
	}
}

void actualizar_ultima_estimacion_SJF(){
	pthread_mutex_lock(&m_lista_pcbs);
	list_iterate(pcbs, calcular_estimacion_SJF);
	pthread_mutex_unlock(&m_lista_pcbs);
}

void calcular_estimacion_SJF(void* pcbb){
	pcb* pcb_esi = pcbb;
	float proxima_rafaga =  (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	log_info(logger, "Alpha es: %f", alpha);
	log_info(logger, "La ultimaRafaga del esi es: %d", pcb_esi->ultimaRafaga);
	log_info(logger, "La ultima estimacion del esi es: %f", pcb_esi->ultimaEstimacion);
	log_info(logger, "Vine a calcular ultimasEstimaciones y es: %f", proxima_rafaga);
	pcb_esi->ultimaEstimacion = proxima_rafaga;
}

void actualizar_ultima_estimacion_HRRN(){
	pthread_mutex_lock(&m_lista_pcbs);
	list_iterate(pcbs, calcular_estimacion_HRRN);
	pthread_mutex_unlock(&m_lista_pcbs);
}

void calcular_estimacion_HRRN(void* pcbb){
	pcb* pcb_esi = pcbb;
	float estimacion = (alpha/100) * (pcb_esi->ultimaRafaga) + (1 - alpha/100)* (pcb_esi->ultimaEstimacion);
	float tiempo_de_respuesta = (pcb_esi->retardo + estimacion) / estimacion;
	log_info(logger, "Vine a calcular ultimasEstimaciones");
	pcb_esi->ultimaEstimacion = tiempo_de_respuesta;
}

bool algoritmo_SJF_CD(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	log_info(logger, "ultimaRafaga ESI1: %d", pcb1->ultimaRafaga);
	log_info(logger, "ultimaRafaga ESI2: %d", pcb2->ultimaRafaga);
	log_info(logger, "alpha es: %f", alpha);
	log_info(logger, "ultimaEstimacion ESI1: %f", pcb1->ultimaEstimacion);
	log_info(logger, "ultimaEstimacion ESI2: %f", pcb2->ultimaEstimacion);

	float proxima_rafaga1;
	float proxima_rafaga2;

	proxima_rafaga1 =  (alpha/100) * (pcb1->ultimaRafaga) + (1 - alpha/100)* (pcb1->ultimaEstimacion);
	proxima_rafaga2 =  (alpha/100) * (pcb2->ultimaRafaga) + (1 - alpha/100)* (pcb2->ultimaEstimacion);

	log_info(logger, "La proxima_rafaga del ESI1 es %f y la del ESI2 es %f", proxima_rafaga1, proxima_rafaga2);


	if ( proxima_rafaga1 <= proxima_rafaga2){
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

		log_info(logger, "ultimaRafaga ESI1: %d", pcb1->ultimaRafaga);
		log_info(logger, "ultimaRafaga ESI2: %d", pcb2->ultimaRafaga);
		log_info(logger, "alpha es: %f", alpha);
		log_info(logger, "ultimaEstimacion ESI1: %f", pcb1->ultimaEstimacion);
		log_info(logger, "ultimaEstimacion ESI2: %f", pcb2->ultimaEstimacion);

		float proxima_rafaga1;
		float proxima_rafaga2;

		proxima_rafaga1 =  (alpha/100) * (pcb1->ultimaRafaga) + (1 - alpha/100)* (pcb1->ultimaEstimacion);
		proxima_rafaga2 =  (alpha/100) * (pcb2->ultimaRafaga) + (1 - alpha/100)* (pcb2->ultimaEstimacion);

		log_info(logger, "La proxima_rafaga del ESI1 es %f y la del ESI2 es %f", proxima_rafaga1, proxima_rafaga2);

		if ( proxima_rafaga1 <= proxima_rafaga2){
			return true;
		} else {
			return false;
		}
	}
}


bool algoritmo_HRRN(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	float estimacion1 = (alpha/100) * (pcb1->ultimaRafaga) + (1 - alpha/100)* (pcb1->ultimaEstimacion);
	float tiempo_de_respuesta1 = (pcb1->retardo + estimacion1) / estimacion1;

	float estimacion2 = (alpha/100) * (pcb2->ultimaRafaga) + (1 - alpha/100)* (pcb2->ultimaEstimacion);
	float tiempo_de_respuesta2 = (pcb2->retardo + estimacion2) / estimacion2;

	log_info(logger, "El tiempo_respuesta del ESI1 es %f y del ESI2 es %f", tiempo_de_respuesta1, tiempo_de_respuesta2);

	if(tiempo_de_respuesta1 >= tiempo_de_respuesta2){
		return 1;
	}
	return 0;
}

/////-----MOVIENDO ESIS-----/////

//--Mover ESI a cola de bloqueados de una clave--//
void mover_esi_a_bloqueados(char* clave, int esi_id){
	pthread_mutex_lock(&m_clave_buscada);
	clave_buscada = clave;
	clave_bloqueada* nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	pthread_mutex_unlock(&m_clave_buscada);
	int* id = //aRREGLAAAAAR
	list_add(nodo_clave_buscada->esis_en_espera, &esi_id);
	log_info(logger, "El id que acabo de poner en la cola desde la cola es: %d", list_get(nodo_clave_buscada->esis_en_espera, 0));
	pthread_mutex_lock(&m_id_buscado);
	id_buscado = esi_id;
	list_remove_by_condition(esis_ready, ids_iguales_cola_de_esis);
	log_info(logger, "Esi %d colocado en cola de espera de la clave %s", esi_id, clave_buscada);
	pthread_mutex_unlock(&m_id_buscado);
}

//--Abortar ESI--//
void abortar_esi(int id_esi){
	pcb* esi_abortado;

	pcb* primer_elem = list_get(pcbs, 0);
	log_info(logger, "El ID del primer ESI en la lista de PCBs es: %d", primer_elem->id);

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_abortado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_abortado = esi_abortado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	log_info(logger, "ESI abortado: %d", esi_abortado->id);

	list_remove_by_condition(pcbs, ids_iguales_pcb);
	list_remove_by_condition(esis_ready, ids_iguales_pcb);

	pthread_mutex_unlock(&m_id_buscado);
	//free(esi_abortado); NO ME DEJA HACER FREE

	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_abortado, sizeof(int));
	list_add(esis_finalizados, &id_esi_abortado); //va o no?
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_abortado);

	cerrar_cosas_de_un_esi(esi_abortado);
	sem_wait(&s_eliminar_pcb);
	//list_remove_and_destroy_by_condition(pcbs, ids_iguales_pcb, destruir_pcb);
}

//--Mover ESI a finalizados--//
void mover_esi_a_finalizados(int id_esi){
	pcb* esi_finalizado;

	pcb* primer_elem = list_get(pcbs, 0);
	log_info(logger, "El ID del primer ESI en la lista de PCBs es: %d", primer_elem->id);

	pthread_mutex_lock(&m_id_buscado);
	id_buscado = id_esi;
	esi_finalizado = list_find(pcbs, ids_iguales_pcb);
	int id_esi_finalizado = esi_finalizado->id;

	pthread_mutex_lock(&m_lista_claves_bloqueadas);
	list_iterate(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	pthread_mutex_unlock(&m_lista_claves_bloqueadas);

	log_info(logger, "ESI finalizado: %d", esi_finalizado->id);

	//list_remove_and_destroy_by_condition(pcbs, ids_iguales_pcb, destruir_pcb);
	//log_info(logger, "El ID del ESI de remove_and_destroy_by_condition es: %d", id_esi_finalizado);

	pthread_mutex_lock(&m_lista_esis_ready);
	list_remove_by_condition(esis_ready, ids_iguales_pcb);
	pthread_mutex_lock(&m_lista_esis_ready);

	list_remove_by_condition(pcbs, ids_iguales_pcb);

	pthread_mutex_unlock(&m_id_buscado);
	//free(esi_finalizado); //NO ME DEJA HACER FREE

	id_buscado = id_esi_finalizado;
	int* id = malloc(sizeof(int));
	memcpy(id, &id_esi_finalizado, sizeof(int));
	list_add(esis_finalizados, &id_esi_finalizado);
	log_info(logger, "Esi %d agregado a esis_finalizados", id_esi_finalizado);

	cerrar_cosas_de_un_esi(esi_finalizado);
	sem_wait(&s_eliminar_pcb);
}

//--Destruir PCB y liberar memoria--// NO SIRVE
void destruir_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	free(pcb_esi);
}

//--Cerrar hilo y socket ESI abortado--//
void cerrar_cosas_de_un_esi(pcb* esi_a_cerrar){
	close(esi_a_cerrar->socket);
	log_info(logger, "Entre en cerrar_cosas");
	//sem_post(&s_podes_procesar_un_mensaje);
	pthread_mutex_lock(&m_hilo_a_cerrar);
	hilo_a_cerrar = &esi_a_cerrar->hilo;
	hay_hilos_por_cerrar = 1;
	sem_post(&s_cerrar_un_hilo);
	sem_wait(&s_hilo_cerrado);
}

//--Sacar ESI de la cola de bloqueados de una clave--//
void quitar_esi_de_cola_bloqueados(void* clave_bloq){
	clave_bloqueada* clave = clave_bloq;
	if(!list_is_empty(clave->esis_en_espera)){
		if(list_find(clave->esis_en_espera, ids_iguales_cola_de_esis) != NULL){
			log_info(logger, "Voy a remover de esis_en_espera");
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
	}
}


/////-----OPERACIONES SOBRE CLAVE_BLOQUEADA-----/////
void liberar_clave(char* clave){
	//hacer el lock ANTES de llamar a esta funcion
	clave_buscada = clave;
	clave_bloqueada* nodo_clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);

	log_info(logger, "El ESI que ocupaba la clave era: %d", nodo_clave->esi_que_la_usa);

	nodo_clave->esi_que_la_usa = 0;
	log_info(logger, "La clave %s se libero", clave);
	log_info(logger, "Ahora es ocupada por (deberia ser 0): %d", nodo_clave->esi_que_la_usa);

	if(!list_is_empty(nodo_clave->esis_en_espera)){
		int* esi_que_ahora_va_a_tener_la_clave = list_remove(nodo_clave->esis_en_espera, 0);
		nodo_clave->esi_que_la_usa = *esi_que_ahora_va_a_tener_la_clave;
		log_info(logger, "y es ahora ocupada por el ESI %d", esi_que_ahora_va_a_tener_la_clave);
	}
}

//FUNCIONES AUXILIARES CLAVE_BLOQUEADA//
bool claves_iguales_nodo_clave(void* nodo_clave){
	int tamanio = list_size(claves_bloqueadas);
	log_info(logger, "El tamanio de la lista de claves bloqueadas es %d", tamanio);

	clave_bloqueada* una_clave = (clave_bloqueada*) nodo_clave;
	log_info(logger, "La clave_buscada es: %s", clave_buscada);
	log_info(logger, "La clave que estoy comparando es: %s", una_clave->clave);

	if(strcmp(una_clave->clave, clave_buscada) == 0){
		return true;
	} else {
		return false;
	}
}

//--Buscar ESI en una cola de esis bloqueados--//
bool ids_iguales_cola_de_esis(void* id){
	int* id_esi = id;
	return *id_esi == id_buscado;
}

/////-----COSAS PARA LA CONSOLA-----/////

void mostrar_status_clave(status_clave status){
	if (status.contenido != NULL){
		//mostrar valor? "El valor de la clave es: /s", status.contenido
	} else {
		//decir "La clave existe pero esta vacia"
	}
	if (status.id_instancia_actual != 0){
		//decir "La clave se encuentra actualmente es la instancia: %s", status.id_instancia_actual
	}
	//decir "La clave se guardaria en la instancia: %s", status.id_instancia_nueva
	//decir "Los ESIs a la espera de la clave son:
	clave_buscada = status.clave;
	clave_bloqueada* clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
	list_iterate(clave->esis_en_espera, imprimir_id_esi);
}

//FUNCIONES AUXILIARES CONSOLA//

//--Recibir status del Coordinador--//
void recibir_status_clave(){
	status_clave status;
	int tamanio = recibir_int(sockets_planificador.socket_coordinador, logger);
	log_info(logger, "Tamanio recibido del coordinador: %d", tamanio);
	void* buffer = malloc(tamanio);
	recibir(sockets_planificador.socket_coordinador, buffer, tamanio, logger);
	status = deserializar_status_clave(buffer);
	printf("Recibido el status_clave de la clave: %s", status.clave);
	mostrar_status_clave(status);
	free(buffer);
}

void imprimir_id_esi(void* esi){
	int* id_esi = esi;
	//mostrar "%d", id_esi;
}


// CONSOLA
void ejecutar_consola(){
	char* linea;
	char* clave;
	char* recurso;
	int el_id;

	while(1) {
		linea = readline("> Ingrese un comando: ");
		if(linea){
			add_history(linea);
			printf("---> La linea ingresada fue: %s\n", linea);
		}
		string_to_lower(linea);

		if(!strncmp(linea, "exit", 4)) {
			printf("Cerrando Consola. Hasta luego. \n");
			free(linea);
			cerrar_planificador();
			exit(1);
			break;
		}

		op_consola operacion = analizar_linea(linea);

		if(operacion != INVALIDO){

			char **palabras = string_to_array(linea);

			switch(operacion){
			case BLOQUEAR:
				printf("Usted quiere bloquear un ESI.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				printf("Clave: %s ---- ", palabras[1]);
				printf("ID: %s ---- ", palabras[2]);
				if(palabras[3]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[3] = NULL;
						printf("Bloque vacio\n");
					}
				clave = palabras[1];
				el_id = atoi(palabras[2]);
				bloquear(clave, el_id);
				break;
			case DESBLOQUEAR:
				printf("Usted quiere desbloquear un ESI de para una clave.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				printf("Clave: %s ---- ", palabras[1]);
				if(palabras[2]==NULL) {printf("Bloque vacio\n");}
					else {
						palabras[2] = NULL;
						printf("Bloque vacio\n");
					}
				clave = palabras[1];
				desbloquear(clave);
				break;
			case KILL:
				printf("Usted quiere finalizar un proceso.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				printf("ID: %s ---- ", palabras[1]);
				if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[2] = NULL;
					printf("Bloque vacio\n");
				}
				el_id = atoi(palabras[1]);
				kill(el_id);
				break;
			case PAUSAR:
				printf("Usted quiere pausar la planificacion.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[1] = NULL;
					printf("Bloque vacio\n");
				}
				pausar_planificacion();
				break;
			case CONTINUAR:
				printf("Usted quiere continuar la planificacion.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[1] = NULL;
					printf("Bloque vacio\n");
				}
				continuar_planificacion();
				break;
			case LISTAR:
				printf("Usted quiere listar los procesos en cola de espera para un recurso.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				printf("Recurso: %s ---- ", palabras[1]);
				if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[2] = NULL;
					printf("Bloque vacio\n");
				}
				recurso = palabras[1];
				listar_procesos_encolados(recurso);
				break;
			case STATUS:
				printf("Usted quiere ver el estado de una clave.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				printf("Clave: %s ---- ", palabras[1]);
				if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[2] = NULL;
					printf("Bloque vacio\n");
				}
				clave = palabras[1];
				pedir_status(clave);
				break;
			case DEADLOCK:
				printf("Usted quiere analizar deadlocks.\n");
				printf("Operacion: %s ---- ", palabras[0]);
				if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
				else {
					palabras[1] = NULL;
					printf("Bloque vacio\n");
				}
				deadlock();
				break;
		   default:
			   break;
			}
		} else{
			printf("Comando no reconocido. Ingrese nuevamente. \n");
		}
  }
  free(linea);
  exit(1);
}

op_consola analizar_linea(char* linea){

	if(string_contains(linea, "bloquear")) {
		return BLOQUEAR;
	}
	if(string_starts_with(linea, "desbloquear")) {
		return DESBLOQUEAR;
	}
	if(string_starts_with(linea, "kill")){
		return KILL;
	}
	if(string_starts_with(linea, "pausar")){
		return PAUSAR;
	}
	if(string_starts_with(linea, "continuar")){
		return CONTINUAR;
	}
	if(string_starts_with(linea, "listar")){
		return LISTAR;
	}
	if(string_starts_with(linea, "status")){
		return STATUS;
	}
	if(string_starts_with(linea, "deadlock")){
		return DEADLOCK;
	}
	return INVALIDO;
}

char**  string_to_array(char* text) {
    char **array_values = string_split(text, " ");
    int i = 0;
    while (array_values[i] != NULL) {
	    string_trim(&(array_values[i]));
	    i++;
    }
    return array_values;
}


// COMPLETAR SUBRUTINAS QUE VIENEN DE CONSOLA:
void pausar_planificacion(){

}
void continuar_planificacion(){

}
void bloquear(char * clave, int id){

}
void desbloquear(char * clave){

}
void listar_procesos_encolados(char* recurso){

}
void kill(int id){

}
void pedir_status(char* clave){
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer_pedido_clave = malloc(tamanio_buffer);
	serializar_string(buffer_pedido_clave, clave, 21);
	enviar(sockets_planificador.socket_coordinador, buffer_pedido_clave, tamanio_buffer, logger);
	sleep(2);
	//se recibe la respuesta por otro hilo
}
void deadlock(){

}


// Rutinas de cierre
void cerrar_sockets(){
	close(sockets_planificador.socket_coordinador);
	//cerrar los sockets de la lista de pcbs
}
void cerrar_hilos(){
	//joinear los hilos (?
}
void cerrar_planificador(){
	void* envio = malloc(sizeof(int));
	serializar_id(envio, 20);
	enviar(sockets_planificador.socket_coordinador, envio, sizeof(int), logger);
	cerrar_sockets();
	cerrar_hilos();
	//liberar variables globales
}


//MUTEX

//pthread_mutex_lock(&un_mutex); //WAIT
//pthread_mutex_unlock(&un_mutex); //SIGNAL

//SEM
//sem_wait(&un_semaforo); //WAIT
//sem_post(&un_semaforo); //SIGNAL


