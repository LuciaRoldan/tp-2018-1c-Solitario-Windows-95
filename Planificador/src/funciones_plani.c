#include "funciones_plani.h"

//////////----------COSAS POR HACER ANTES DE SEGUIR----------//////////
//1. TERMINAR DE LEER EL ARCHIVO DE CONFIGURACION Y VER QUE LEA BIEN = LISTA DE CLAVES INICIALMENTE BLOQUEADAS
//2. VERIFICAR QUE registrar_exito_en_pcb ANDE

//inicializando

sockets inicializar_planificador(){
	sockets sockets_planificador;
	leer_archivo_configuracion();
	sockets_planificador.socket_coordinador = connect_to_server(conexion_coordinador.ip, conexion_coordinador.puerto, logger);
	conectarse_al_coordinador(sockets_planificador.socket_coordinador);
	sockets_planificador.socket_esis = inicializar_servidor(atoi(conexion_planificador.puerto), logger); //pasar ip
	return sockets_planificador;
}

void leer_archivo_configuracion(){
	t_config* configuracion = config_create("config_planificador");
		conexion_planificador.ip = strdup(config_get_string_value(configuracion,"IP_PLANIFICADOR"));
		conexion_planificador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_PLANIFICADOR"));
		conexion_coordinador.ip = strdup(config_get_string_value(configuracion,"IP_COORDINADOR"));
		conexion_coordinador.puerto = strdup(config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
		algoritmo = strdup(config_get_string_value(configuracion, "ALGORITMO_PLANIFICACION")); //anda
		estimacion_inicial = atoi(strdup(config_get_string_value(configuracion, "ESTIMACION_INICIAL"))); //anda
		//falta leer las claves inicialmente bloqueadas
	log_info(logger, "Se leyo el archivo de configuracion correctamente");
}

void conectarse_al_coordinador(int socket_coordinador){
	handshake_coordinador(socket_coordinador);
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
		recibir(socket_esi, buffer2, sizeof(int), logger);
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

pcb crear_pcb_esi(int socket_esi_nuevo, int id_esi){
	pcb pcb_esi;
	pcb_esi.id = id_esi;
	pcb_esi.socket = socket_esi_nuevo;
	return pcb_esi;
}


//manejar esis
void recibir_esis(void* socket_esis){
	int int_socket_esis = *((int*) socket_esis);
	log_info(logger, "Entre al hilo recibir_esis y el socket es %d\n", int_socket_esis);
	pcbs = list_create();
	esis_ready = list_create();
	int socket_esi_nuevo;
	while(1){
		log_info(logger, "Esperando un ESI");
		sleep(1);
		socket_esi_nuevo = aceptar_conexion(int_socket_esis);
		if (socket_esi_nuevo > 0){
			int id_esi_nuevo;
			id_esi_nuevo = handshake_esi(socket_esi_nuevo);
			if (id_esi_nuevo){
				pcb pcb_esi_nuevo;
				pcb_esi_nuevo = crear_pcb_esi(socket_esi_nuevo, id_esi_nuevo);
				list_add(pcbs, &pcb_esi_nuevo); //agrego PCB ESI a mi lista de ESIs
				list_add(esis_ready, &pcb_esi_nuevo); //agrego PCB ESI a cola ready
				pthread_t hilo_escucha_esi;
				if (pthread_create(&hilo_escucha_esi, 0 , manejar_esi, (void*) &pcb_esi_nuevo) < 0){
					perror("No se pudo crear el hilo");
				}
				pthread_join(hilo_escucha_esi , 0);
			}
		} else { //socket_esi_nuevo < 0
	        perror("Fallo en el accept");
		}
	}
}

void manejar_esi(void* la_pcb){
	while(1){
		log_info(logger, "Entre a manjear ESI");
		pcb pcb_esi = *((pcb*) la_pcb);
		log_info(logger, "ID dentro de manejar esi es %d\n", pcb_esi.id);
		log_info(logger, "Socket dentro de manejar esi es %d\n", pcb_esi.socket);
		planificar();
		sleep(5);
		resultado_esi resultado = recibir_int(pcb_esi.socket, logger);
		log_info(logger, "El ESI me envio el resultado_esi %d\n", resultado);
		if (resultado >= 0){
			switch (resultado){
				case (EXITO):
						registrar_exito_en_pcb(pcb_esi.id);
				break;
				case (FALLO):
					//mover_esi_a_bloqueados(pcb_esi.id);
				break;
				default:
					//abortar_esi(pcb_esi.id);
				break;
			}
		}
	}
}

void planificar(){
	ordenar_pcbs();
	void* pcbb;
	pcbb = list_get(esis_ready, 1);
	pcb esi_a_ejecutar = *((pcb*) pcbb);

	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 61);
	enviar(esi_a_ejecutar.socket, buffer, sizeof(int), logger);
}

//--Registrar exito--// //VER QUE ESTO ANDE ANTES DE SEGUIR!!!!!
void registrar_exito_en_pcb(int id_esi){
	void* pcbb;
	id_buscado = id_esi;
	pcbb = list_find(pcbs, claves_iguales);
	pcb* esi_que_ejecuto = pcbb;
	esi_que_ejecuto->ultimaRafaga++;
	log_info(logger, "La ultima rafaga del ESI que acaba de ejecutar es: %d\n", esi_que_ejecuto->ultimaRafaga);
}

bool claves_iguales(void* pcbb){
	pcb* pcb_esi = pcbb;
	return pcb_esi->id == id_buscado;
}

//--Ordenar PCBs--//
void ordenar_pcbs(){
	if(strcmp(algoritmo, "SJF_CD")){
	planificacionSJF_CD();
	}
	else if(strcmp(algoritmo, "SJF_SD")){
	planificacionSJF_SD();
	}
	else if(strcmp(algoritmo, "HRRN")){
	planificacionHRRN();
	}
	else{
		log_info(logger, "Algoritmo invalido en ordenar_pcbs");
	}
}

void planificacionSJF_CD(){
	list_sort(esis_ready, algoritmo_SJF);
}

void planificacionSJF_SD(){
	list_sort(esis_ready, algoritmo_SJF);
}

void planificacionHRRN(){
	list_sort(esis_ready, algoritmo_HRRN);
}

bool algoritmo_SJF(void* pcb_1, void* pcb_2){

	pcb pcb1 = *((pcb*) pcb_1);
	pcb pcb2 = *((pcb*) pcb_2);

	float proxima_rafaga1;
	float proxima_rafaga2;

	int alfaSJF = 2;

	proxima_rafaga1 =  (alfaSJF/100) * (pcb1.ultimaRafaga) + (1 - alfaSJF/100)* (pcb1.ultimaEstimacion);
	proxima_rafaga2 =  (alfaSJF/100) * (pcb2.ultimaRafaga) + (1 - alfaSJF/100)* (pcb2.ultimaEstimacion);

	if ( proxima_rafaga1 <= proxima_rafaga2){
		return 1;
	} else {
		return 0;
	}
}

bool algoritmo_HRRN(void* pcb_1, void* pcb_2){

	pcb pcb1 = *((pcb*) pcb_1);
	pcb pcb2 = *((pcb*) pcb_2);

	int alfaHRRN = 2;

	int estimacion1 = (alfaHRRN/100) * (pcb1.ultimaRafaga) + (1 - alfaHRRN/100)* (pcb1.ultimaEstimacion);
	int tiempo_de_respuesta1 = (pcb1.retardo + estimacion1) / estimacion1;

	int estimacion2 = (alfaHRRN/100) * (pcb2.ultimaRafaga) + (1 - alfaHRRN/100)* (pcb2.ultimaEstimacion);
	int tiempo_de_respuesta2 = (pcb2.retardo + estimacion2) / estimacion2;

	if(tiempo_de_respuesta1 >= tiempo_de_respuesta2){
		return 1;
	}
	return 0;
}


/*
void manejar_coordinador(void* socket_coordinador){

	int id = recibir_int(sockets_planificador.socket_coordinador, logger);
	pedido_esi pedido;

	switch(id){
		case (10): //nuevo pedido
				pedido = recibir_pedido_coordinador(sockets_planificador.socket_coordinador);


				//responder_a_pedido_coordinador(socket_coordinador, pedido);
		break;
		case (11): //respuesta de un estado que pedi
				//int tamano_status_clave = malloc(sizeof(struct status_clave));
				//struct status_clave status = recibir_status_clave(socket_coordinador);
				//mostrar_status_clave(status);
		break;
		log_info(logger, "Pedido invalido del Coordinador");
}



	//comportamiento posterior


}


*/
/*







//consola

void pausar_planificador(){
	//syscall bloqueante ?????
}

void bloquear_esi(char* clave, int esi_id){
	//mover_esi_a_bloqueados(colaDeBloqueadoEsis, idEsi, clave); //encola al Esi en la lista de bloqueados con la clave
	//que corresponda
}

void desbloquear_esi(char* clave){
	mover_esi_a_ready(clave);
}


//ColaDeEsi listar(Clave clave){ //recurso == clave?
//	return "claveBloqueada.esisEnEspera"; //busca en su lista de Claves bloqueadas, la clave que se le pide,
//	//y devuelve la cola de esis bloqueados.
//}


void kill(int id_esi){
	mover_esi_a_finalizado(id_esi);
	//liberar los recursos. Fijarse que clave estaba ocupando y desocuparla.
}

int status(char* clave){
	//devuelve informacion de la instancia que se consulta
	//fijarse en la ClavesBloqueadas a ver si esta la clave
	return 1;
}

int* deadlock(){return 0;} //va a devolver los esis con deadlocks //no se jaja

void killEsis(ColaDeEsi esis){
//	for(){ //mata los esis uno por uno
//	kill(esi); }
}






*/
/*
void mover_esi_a_bloqueado(int idEsi){} //hay que hacer estas funciones de encolar
void mover_esi_a_ready(int idEsi){}
void mover_esi_a_finalizado(int idEsi){}

int clave_tomada_por_otro_esi(int clave){
	int tomada = ";";//buscar en la estructura que tenga las claves a ver si esta bloqueada
	return tomada;
}

//void asignar_esi_a_clave(Clave clave, int idEsi){
//	//buscar en donde sea que tenga las claves con los esis y asignarle el esi.
//}


void ejecutar_proximo_esi(){}

//
//void ejecutarEsi(esi){
//	solicitarEjecucion(esi); //le va a mandar al ESI un mensaje diciéndole que quiere
//							//que ejecute}



//hablar con coordinador
void manejar_coordinador(void* socket_coordinador){

	int tipo_mensaje = recibir_tipo_mensaje_coordinador(socket_coordinador);
	if (tipo_mensaje == 10){ //el 10 seria un nuevo pedido
		//int tamano_pedido_esi = malloc(sizeof(pedido_esi));
		//struct pedido_esi* pedido = recibir_pedido_coordinador(socket_coordinador);
		//responder_a_pedido_coordinador(socket_coordinador, pedido);
	}
	else if (tipo_mensaje == 11){ //el 11 es una respuesta a un estado que pedi
		//int tamano_status_clave = malloc(sizeof(struct status_clave));
		//struct status_clave status = recibir_status_clave(socket_coordinador);
		//mostrar_status_clave(status);
	}
	else {
		log_info(logger, "Pedido invalido del Coordinador");
	};
}


void responder_a_pedido_coordinador(int socket_coordinador, pedido_esi pedido){
	switch(pedido.instruccion.keyword){
	case(GET):
	case(STORE):
			if(clave_tomada_por_otro_esi(pedido.esi_id)){
				mover_esi_a_bloqueado(pedido.esi_id);
				actualizar_pcb_esi_bloqueado(pedido.esi_id); //hace falta ?
				informar_bloqueo_coordinador(socket_coordinador, pedido.esi_id);
			}
			else {
				asignar_clave_esi(pedido.instruccion.argumentos.GET.clave, pedido.esi_id);
				actualizar_pcb_esi_asignado(pedido.esi_id);
				informar_exito_coordinador(socket_coordinador, pedido.esi_id);
			}
			break;
	case(SET):
			if(clave_tomada_por_otro_esi(pedido.esi_id)){
				log_info(logger, "Pedido invalido! Clave tomada por otro ESI");
			}
			else {
				actualizar_pcb_esi_asignado(pedido.esi_id);
				informar_exito_coordinador(socket_coordinador, pedido.esi_id);
			}
			break;
	}
}

void mostrar_status_clave(status_clave clave){

}



//	switch (mensaje->clave){ //o sea me fijo en el header si la clave por la que
//				//me estan preguntando esta tomada o no
//				case (clave_tomada(mensaje->clave)): //aca ve que la clave que me pedian esta tomada. Es una funcion
//						//que retorna true o false
//						mover_esi_a_bloqueado(idEsi);
//						asignar_esi_a_clave(mensaje->clave, idEsi);
//						return -1;//le dice al coordinador que no le puede asignar la clave a un Esi.
//				break;
//				case (!clave_tomada(mensaje->clave)): //la clave no esta tomada por ningun Esi
//						asignar_esi_a_clave(mensaje->clave, idEsi);
//						return 1; //exito
//				break;
//				}
//				}
//}


//MENSAJES

*/
/*
pedido_esi recibir_pedido_coordinador(int socket_coordinador){

	int tamanio = recibir_int(sockets_planificador.socket_coordinador, logger);
	void* buffer = malloc(tamanio);
	recibir(sockets_planificador.socket_coordinador, buffer, tamanio, logger);
	pedido_esi* pedido;
	deserializar_pedido_coordinador(buffer, pedido);
	return *pedido;
}


/*
int recibir_tipo_mensaje_coordinador(int socket_coordinador){
	int tipo_mensaje;
	recibir(socket_coordinador, &tipo_mensaje, sizeof(tipo_mensaje), logger);
		//deserializar
	return tipo_mensaje;
}

status_clave recibir_status_clave(int socket_coordinador, status_clave status);
void informar_bloqueo_coordinador(int socket_coordinador, int id_esi){

}



void informar_exito_coordinador(int socket_coordinador, int id_esi){

}



//EN EL ESI

void enviar_resultado_esi(int socket_planificador, resultado_esi resultado, t_log* logger){
	void* buffer = malloc(sizeof(enum));
	memcpy(buffer, &resultado, sizeof(enum));
	enviar(socket_planificador, buffer, sizeof(enum), 41, logger);
	//enviar(socket_planificador, buffer, sizeof(enum), logger);
}


//void serializar_enum(void * buffer, int id, int resultado){
//	size_t offset =0;
//
//	memcpy(buffer + offset, id ,sizeof(int));
//
//	offset =+ sizeof(int);
//
//	memcpy(buffer + offset, resultado, sizeof(int));
}
*/
