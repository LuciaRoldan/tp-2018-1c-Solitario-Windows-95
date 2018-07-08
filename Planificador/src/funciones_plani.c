#include "funciones_plani.h"

//inicializando

sockets inicializar_planificador(){
	sockets sockets_planificador;//doble declaracion
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
	log_info(logger, "Se leyo el archivo de configuracion correctamente");

	//ver como levantar el resto, puerto escucha, algoritmo, etc

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

	enviar(socket_coordinador, buffer, sizeof(int)*3, 80, logger);

	log_info(logger, "Envie aHndshake al Coordinador");

	free(buffer);

	int id_protocolo;
	recibir(socket_coordinador, &id_protocolo, sizeof(int), logger);

	if(id_protocolo == 80){
		void* buffer = malloc(sizeof(int)*2);
		recibir(socket_coordinador, buffer, sizeof(int), logger);
		proceso_recibido = deserializar_handshake(buffer);
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
	t_handshake yo = {0, PLANIFICADOR};

	void* buffer = malloc(sizeof(int)*3);
	serializar_handshake(buffer, yo);
	log_info(logger, "Serialice el handshake de un Esi!");

	int protocolo;
	recibir(socket_esi, &protocolo, sizeof(int), logger);

	if(protocolo == 80){
	void* buffer = malloc(sizeof(int)*2);
		recibir(socket_esi, buffer, sizeof(int), logger);
		proceso_recibido = deserializar_handshake(buffer);
		if (proceso_recibido.proceso !=  0){ //validar que no tenía ya al ESI en otra PCB
			enviar(socket_esi, buffer, sizeof(int)*3, 80, logger);
			log_info(logger, "Se establecio la conexion con el Esi de id %d\n", proceso_recibido.id);
			return proceso_recibido.id;
		} else {
			log_info(logger, "Se recibio el Handshake el Esi de id %d\n pero fallo", proceso_recibido.id);
			return -1;
		}
	} else {
		log_info(logger, "Error en el protocolo del Handshake del Esi");
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
void recibir_esis(int socket_esis){
	log_info(logger, "Entre al hilo recibir_esis");
	t_list* pcbs = list_create();
	t_list* esis_ready = list_create();
	while(1);
	int socket_esi_nuevo;
	socket_esi_nuevo = aceptar_conexion(&socket_esis); //es como una funcion con un accept basicamente
	if(socket_esi_nuevo > 1){						//que devuelve lo que me devuelve el accept. Now
		int id_esi_nuevo;								//en las commons!
		id_esi_nuevo = handshake_esi(socket_esi_nuevo);
		if (id_esi_nuevo){
			pcb pcb_esi_nuevo;
			pcb_esi_nuevo = crear_pcb_esi(socket_esi_nuevo, id_esi_nuevo);
			list_add(pcbs, &pcb_esi_nuevo); //agrego PCB ESI a mi lista de ESIs
			list_add(esis_ready, &pcb_esi_nuevo); //agrego PCB ESI a cola ready
}}}/*



			pthread_t hilo_escucha_esi;
			if (pthread_create(&hilo_escucha_esi, 0 , manejar_esi, (void*) &pcb_esi_nuevo) < 0){
				perror("No se pudo crear el hilo");
			}
			pthread_join(hilo_escucha_esi , 0);
		log_info(logger, "Esi asignado");
		}
	}

		if (socket_esi_nuevo < 0){
	        perror("Fallo en el accept");
		}
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







void manejar_esi(pcb pcb_esi){
	//recibir el tipo y fijarme si es valido y seguir
	resultado_esi resultado = recibir_resultado_esi(pcb_esi.socket);
	switch (resultado){
				case (EXITO):
						registrar_exito_en_pcb(pcb_esi.id);
				break;
				case(FALLO):
				abortar(pcb_esi);
				break;
	}
	ejecutar_proximo_esi();
}

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
resultado_esi recibir_resultado_esi(int socket_esi){
	resultado_esi resultado;
	recibir(socket_esi, &resultado, sizeof(resultado), logger);
		//deserializar?

	return resultado;
}

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
