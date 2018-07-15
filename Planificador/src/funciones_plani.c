#include "funciones_plani.h"

//////////----------COSAS POR HACER ANTES DE SEGUIR----------//////////
//1. TERMINAR DE LEER EL ARCHIVO DE CONFIGURACION Y VER QUE LEA BIEN = LISTA DE CLAVES INICIALMENTE BLOQUEADAS
//2. VERIFICAR QUE abortar_esi ANDE
//3. HABLAR CON EL COORDINADOR PARA QUE ME PIDA UNA INSTRUCCION Y YO ME CREE UNA CLAVE CON SUS COSAS
//4. ARREGLAR LEER FLOAT DE ARCHIVO CONFIG

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
		log_info(logger, "Estimacion inicial es: %d", estimacion_inicial);
		alpha = strtof(strdup(config_get_string_value(configuracion, "ALPHA")),NULL);
		log_info(logger, "Alpha es: %f", alpha);
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

pcb crear_pcb_esi(int socket_esi_nuevo, int id_esi){
	pcb pcb_esi;
	pcb_esi.id = id_esi;
	pcb_esi.socket = socket_esi_nuevo;
	pcb_esi.ultimaEstimacion = estimacion_inicial;
	pcb_esi.ultimaRafaga = 0;
	return pcb_esi;
}


//manejar esis
void recibir_esis(void* socket_esis){
	int int_socket_esis = *((int*) socket_esis);
	log_info(logger, "Entre al hilo recibir_esis y el socket es %d\n", int_socket_esis);
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
				//pthread_join(hilo_escucha_esi , 0);
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
		log_info(logger, "ID dentro de manejar esi es %d", pcb_esi.id);
		log_info(logger, "Socket dentro de manejar esi es %d", pcb_esi.socket);
		planificar();
		sleep(5);

		resultado_esi resultado = recibir_int(pcb_esi.socket, logger);
		log_info(logger, "El ESI me envio el resultado_esi %d", resultado);
		if (resultado >= 0){
			switch (resultado){
				case (EXITO):
					registrar_exito_en_pcb(pcb_esi.id);//anda
				break;
				case (FALLO):
					//id_buscado = pcb_esi.id;
					//list_remove_by_condition(esis_ready, id_buscado);
					//ya lo encole en la cola de esis_bloqueados que corresponde en el Coordi
				break;
				default:
					abortar_esi(pcb_esi.id);
				break;
			}
		}
	}
}

void manejar_coordinador(void* socket){
	int socket_coordinador = *((int*) socket);
	log_info(logger, "Entre al hilo manejar_coordinador y el socket es %d\n", socket_coordinador);

	while(1){

			//PROBANDO LEER COSAS CREADAS DESDE OTRO HILO
			/*if(list_size(pcbs)>1){
			void* primer_esi = list_get(pcbs, 1);
			pcb* pcb_esi = primer_esi;
			int id_primer_esi = pcb_esi->id;
			log_info(logger, "El id del segundo ESI en las pcbs es: %d", id_primer_esi);
			}*/
			//FIN DE LA PRUEBA


		int id;
		void* buffer_int = malloc(sizeof(int));
		recibir(socket_coordinador, buffer_int, sizeof(int), logger);
		id = deserializar_id(buffer_int);
		log_info(logger, "Id recibido del Coordinador: %d", id);

		int tamanio;
		pedido_esi pedido;
		switch(id){
		case (83): //nuevo pedido
				tamanio = recibir_int(socket_coordinador, logger);
				log_info(logger, "Tamanio recibido del coordinador: %d", tamanio);
				void* buffer = malloc(tamanio);
				recibir(socket_coordinador, buffer, tamanio, logger);
				pedido = deserializar_pedido_esi(buffer);
				log_info(logger, "Recibido: %s, %d \n", pedido.instruccion.argumentos.GET.clave, pedido.instruccion.keyword);
				procesar_pedido(pedido);
				free(buffer);
		break;
		case (0):
				recibir_status_clave();
		break;
		default:
		log_info(logger, "Pedido invalido del Coordinador");
	}
	}
}

void procesar_pedido(pedido_esi pedido){
	clave_bloqueada* nodo_clave_buscada;
	pcb* pcb_pedido_esi;
	id_buscado = pedido.esi_id;
	pcb_pedido_esi = list_find(pcbs, ids_iguales_pcb);
	if (pcb_pedido_esi != NULL){ //VERIFICO QUE EL ESI SEA VALIDO
	switch(pedido.instruccion.keyword){
	case(GET):
		clave_buscada = pedido.instruccion.argumentos.GET.clave;
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		if(nodo_clave_buscada == NULL){
			//No existe el elemento clave_bloqueada para esa clave en mi lista de claves bloqueadas, la creo
			clave_bloqueada* clave_nueva = malloc(sizeof(clave_bloqueada));
			clave_buscada = pedido.instruccion.argumentos.GET.clave;
			clave_nueva->esi_que_la_usa = pedido.esi_id;
			clave_nueva->clave = clave_buscada;
			clave_nueva->esis_en_espera = list_create();
			list_add(claves_bloqueadas, clave_nueva);
			log_info(logger, "Nueva clave_bloqueada creada %s para el ESI %d", clave_buscada, clave_nueva->esi_que_la_usa);
			informar_exito_coordinador();
			} else {
				if (&nodo_clave_buscada->esi_que_la_usa == NULL){
					//Existe esa clave_bloqueada en mi lista de claves pero no esta asignada a ningun ESI
					nodo_clave_buscada->esi_que_la_usa = pedido.esi_id;
					log_info(logger, "Clave %s asignada al ESI %d", clave_buscada, pedido.esi_id);
					informar_exito_coordinador();
				} else {
					if(nodo_clave_buscada->esi_que_la_usa == pedido.esi_id){
					//Me hacen un GET sobre una clave que ya tenia asignado ese ESI entonces no hago nada.
					log_info(logger, "GET realizado por el ESI %d sobre la clave %s que ya le pertenecia", pedido.esi_id, clave_buscada);
					informar_exito_coordinador();
					} else {
						//Me hacen un GET sobre una clave que estaba asignada a otro ESI entonces lo pongo en
						//la lista de espera de esa clave y lo saco de ready
						log_info(logger, "GET realizado por el ESI %d sobre una clave que no le pertenece %d", pedido.esi_id, clave_buscada);
						mover_esi_a_bloqueados(clave_buscada, nodo_clave_buscada, pedido.esi_id);
						informar_fallo_coordinador();
					}
				}
			}
			break;

	case(SET):
		clave_buscada = pedido.instruccion.argumentos.SET.clave;
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
		log_info(logger, "Nodo_clave_buscada en SET el id del esi que la usa es: %d", nodo_clave_buscada->esi_que_la_usa);
			if(nodo_clave_buscada == NULL){
				log_info(logger, "SET realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, pedido.esi_id);
				abortar_esi(pedido.esi_id);
				informar_aborto_coordinador();
			} else {
				if (&nodo_clave_buscada->esi_que_la_usa == NULL){
					log_info(logger, "SET realizado por el ESI %d sobre una clave %s sin ESIS asignados", pedido.esi_id, clave_buscada);
					abortar_esi(pedido.esi_id);
					informar_aborto_coordinador();
				} else {
					if (nodo_clave_buscada->esi_que_la_usa == pedido.esi_id){
						log_info(logger, "SET realizado por el ESI %d sobre la clave que le pertenecia %s", pedido.esi_id, clave_buscada);
						informar_exito_coordinador();
					} else {
						log_info(logger, "SET realizado por el ESI %d sobre una clave que no le pertenece %s", pedido.esi_id, clave_buscada);
						abortar_esi(pedido.esi_id);
						informar_aborto_coordinador();
					}
				}
			}
			break;

	case(STORE):
		clave_buscada = pedido.instruccion.argumentos.STORE.clave;
		nodo_clave_buscada = list_find(claves_bloqueadas, claves_iguales_nodo_clave);
			if(nodo_clave_buscada == NULL){
				log_info(logger, "STORE realizado sobre una clave inexistente %s por el ESI %d", clave_buscada, pedido.esi_id);
				abortar_esi(pedido.esi_id);
				informar_aborto_coordinador();
			} else {
				if (&nodo_clave_buscada->esi_que_la_usa == NULL){
					log_info(logger, "STORE realizado por el ESI %d sobre una clave %s sin ESIS asignados", pedido.esi_id, clave_buscada);
					abortar_esi(pedido.esi_id);
					informar_aborto_coordinador();
				} else {
					if (nodo_clave_buscada->esi_que_la_usa == pedido.esi_id){
						log_info(logger, "STORE realizado por el ESI %d sobre la clave que le pertenecia %s", pedido.esi_id, clave_buscada);
						liberar_clave(clave_buscada);
						informar_exito_coordinador();
					} else {
						log_info(logger, "STORE realizado por el ESI %d sobre una clave que no le pertenece %d", pedido.esi_id, clave_buscada);
						abortar_esi(pedido.esi_id);
						informar_aborto_coordinador();
					}
				}
			}
	break;
	}
	} else {
		log_info(logger, "Pedido invalido. No conozco al ESI %d", pedido.esi_id);
	}
}


/////-----RESULTADOS PEDIDOS-----/////
void informar_aborto_coordinador(){
	int aborto = 22;
	enviar(sockets_planificador.socket_coordinador, &aborto, sizeof(int), logger);
}
void informar_exito_coordinador(){
	int exito = 20;
	enviar(sockets_planificador.socket_coordinador, &exito, sizeof(int), logger);
}
void informar_fallo_coordinador(){
	int fallo = 24;
	enviar(sockets_planificador.socket_coordinador, &fallo, sizeof(int), logger);
}


/////-----OPERACIONES SOBRE PCBS-----/////
//--Registrar exito ESIS--// ANDA!
void registrar_exito_en_pcb(int id_esi){
	void* pcbb;
	id_buscado = id_esi;
	pcbb = list_find(pcbs, ids_iguales_pcb);
	pcb* esi_que_ejecuto = pcbb;
	esi_que_ejecuto->ultimaRafaga++;
	log_info(logger, "La ultima rafaga del ESI que acaba de ejecutar es: %d\n", esi_que_ejecuto->ultimaRafaga);
}

//--Mover ESI a cola de bloqueados de una clave--// //TERMINAR
void mover_esi_a_bloqueados(char* clave, clave_bloqueada* nodo_clave_buscada, int esi_id){
	list_add(nodo_clave_buscada->esis_en_espera, &esi_id);
	id_buscado = esi_id;
	list_remove_by_condition(esis_ready, ids_iguales_cola_de_esis);
	log_info(logger, "Esi %d colocado en cola de espera de la clave %s", esi_id, clave_buscada);
}


//--Abortar ESIS--//    //VER QUE ANDE
void abortar_esi(int id_esi){
	pcb* esi_abortado;
	id_buscado = id_esi;

	pcb* primer_elem = list_get(pcbs, 0);
	log_info(logger, "El ID del primer ESI en la lista es: %d", primer_elem->id);

	esi_abortado = list_find(pcbs, ids_iguales_pcb);
	list_remove_by_condition(pcbs, ids_iguales_pcb);

	log_info(logger, "El ID del ESI de remove_by_condition es: %d", esi_abortado->id);
	list_add(esis_finalizados, esi_abortado);
	list_map(claves_bloqueadas, quitar_esi_de_cola_bloqueados);
	log_info(logger, "ESI abortado: %d", esi_abortado->id);

	pcb* esi_finalizado = list_get(esis_finalizados, 0);
	log_info(logger, "El ID del primer ESI finalizado es: %d", esi_finalizado->id);


}

void* quitar_esi_de_cola_bloqueados(void* clave_bloq){
	clave_bloqueada* clave = clave_bloq;
	list_remove_by_condition(clave->esis_en_espera, ids_iguales_cola_de_esis); //tiene que ser t_list pero todavia no tengo claves
	return clave;
}

//FUNCIONES AUXILIARES PCB//
bool ids_iguales_pcb(void* pcbb){
	pcb* pcb_esi = pcbb;
	return pcb_esi->id == id_buscado;
}


/////-----OPERACIONES SOBRE CLAVE_BLOQUEADA-----/////
void liberar_clave(char* clave){
	clave_buscada = clave;
	clave_bloqueada* nodo_clave = list_find(claves_bloqueadas, claves_iguales_nodo_clave);

	log_info(logger, "El ESI que ocupaba la clave era: %d", nodo_clave->esi_que_la_usa);

	nodo_clave->esi_que_la_usa = NULL;
	log_info(logger, "La clave %s se libero", clave);

	log_info(logger, "Ahora es ocupada por: %d", nodo_clave->esi_que_la_usa);

	if(!list_is_empty(nodo_clave->esis_en_espera)){
		int* esi_que_ahora_va_a_tener_la_clave = list_remove(nodo_clave->esis_en_espera, 1);
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

bool ids_iguales_cola_de_esis(void* id){ //cuando creo una clave, la creo con una list de ids bloqueados esperando.
	int* id_esi = id;
	return *id_esi == id_buscado;
}






//--Ordenar PCBs--//

void planificar(){
	//export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug
	log_info(logger, "Planificando");
	ordenar_pcbs();
	pcb* pcb_esi;
	void* esi_a_ejecutar = list_get(esis_ready, 0);
	pcb_esi = esi_a_ejecutar;
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 61);
	enviar(pcb_esi->socket, buffer, sizeof(int), logger);
	log_info(logger, "Solicitud de ejecucion enviada al ESI: %d", pcb_esi->id);
}

void ordenar_pcbs(){
	log_info(logger, "El algoritmo es: %s", algoritmo);
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
	log_info(logger, "Entre a planificacionSJF_CD");
	log_info(logger, "La cantidad de esis ready es %d", list_size(esis_ready));
	if(list_size(esis_ready) > 1){
		log_info(logger, "Hay mas de un ESI ready");
		list_sort(esis_ready, algoritmo_SJF);
	}
}

void planificacionSJF_SD(){
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_SJF);
	}
}

void planificacionHRRN(){
	if(list_size(esis_ready) > 1){
		list_sort(esis_ready, algoritmo_HRRN);
	}
}

bool algoritmo_SJF(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	log_info(logger, "ultimaRafaga ESI1: %d", pcb1->ultimaRafaga);
	log_info(logger, "ultimaRafaga ESI2: %d", pcb2->ultimaRafaga);
	log_info(logger, "alpha es: %f", alpha);
	log_info(logger, "ultimaEstimacion ESI1: %d", pcb1->ultimaEstimacion);
	log_info(logger, "ultimaEstimacion ESI2: %d", pcb2->ultimaEstimacion);

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

bool algoritmo_HRRN(void* pcb_1, void* pcb_2){

	pcb* pcb1 = pcb_1;
	pcb* pcb2 = pcb_2;

	int estimacion1 = (alpha/100) * (pcb1->ultimaRafaga) + (1 - alpha/100)* (pcb1->ultimaEstimacion);
	int tiempo_de_respuesta1 = (pcb1->retardo + estimacion1) / estimacion1;

	int estimacion2 = (alpha/100) * (pcb2->ultimaRafaga) + (1 - alpha/100)* (pcb2->ultimaEstimacion);
	int tiempo_de_respuesta2 = (pcb2->retardo + estimacion2) / estimacion2;

	log_info(logger, "El tiempo_respuesta del ESI1 es %f y del ESI2 es %f", tiempo_de_respuesta1, tiempo_de_respuesta2);

	if(tiempo_de_respuesta1 >= tiempo_de_respuesta2){
		return 1;
	}
	return 0;
}


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

void imprimir_id_esi(void* esi){
	int* id_esi = esi;
	//mostrar "%d", id_esi;
}

/*



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


int* deadlock(){return 0;} //va a devolver los esis con deadlocks //no se jaja

void killEsis(ColaDeEsi esis){
//	for(){ //mata los esis uno por uno
//	kill(esi); }
}






*/



