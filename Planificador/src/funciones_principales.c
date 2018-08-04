#include "planificador_punto_hache.h"

//--MANEJAR ESIS - PLANIFICAR--//
void manejar_esis(){

	while(terminar_todo){

		while(pausar_planificador >= 0 && se_cerro_todo != 1){

			if(list_size(esis_ready) > 0){

				sem_wait(&s_planificar);
				//log_info(logger, "Pase el semaforo de la muerte");

				if (pausar_planificador == -1 || terminar_todo == -1){ //cambie
					log_info(logger, "Break en manejar_esis por operacion PAUSAR");
					break;
					log_info(logger, "No hice el break un carajo");
				}

				pthread_mutex_lock(&m_lista_esis_ready);

				if (pausar_planificador == -1 || terminar_todo == -1){ //cambie
					log_info(logger, "Break en manejar_esis por operacion PAUSAR");
					pthread_mutex_unlock(&m_lista_esis_ready);
					break;
					log_info(logger, "No hice el break un carajo");
				}

				if(list_size(esis_ready) > 0){

				//pthread_mutex_lock(&m_lista_esis_ready);

				//log_info(logger, "La cantidad de ESIs ready desde manejar_esis es: %d", list_size(esis_ready));


				planificar_si_corresponde();

				void* esi_a_ejecutar = list_get(esis_ready, 0);
				pcb* pcb_esi;
				pcb_esi = esi_a_ejecutar;
				//log_info(logger, "El Esi que va a ejecutar es: %d", pcb_esi->id);

				actualizar_rafaga_si_hubo_desalojo(pcb_esi);

				pthread_mutex_lock(&m_id_esi_ejecutando);
				id_esi_ejecutando = pcb_esi->id;
				pthread_mutex_unlock(&m_id_esi_ejecutando);

				sumar_retardo_otros_ready();

				enviar_solicitud_ejecucion(pcb_esi);
			}
			pthread_mutex_unlock(&m_lista_esis_ready);

			}
		}
	}
	log_info(logger, "Saliendo del hilo manejar_esis");
}


//--RECIBIR ESIS--//
void recibir_esis(void* socket_esis){
	int int_socket_esis = *((int*) socket_esis);

	int socket_esi_nuevo;
	while(terminar_todo == 1){
		//log_info(logger, "Esperando un ESI adentro de recibir_esis");

		socket_esi_nuevo = aceptar_conexion(int_socket_esis);

		if (socket_esi_nuevo > 0){
			int id_esi_nuevo;
			id_esi_nuevo = handshake_esi(socket_esi_nuevo);

			if (id_esi_nuevo){
				pthread_t hilo_escucha_esi;
				pcb* pcb_esi_nuevo;
				pcb_esi_nuevo = crear_pcb_esi(socket_esi_nuevo, id_esi_nuevo, hilo_escucha_esi);

				pthread_mutex_lock(&m_lista_pcbs);
				list_add(pcbs, pcb_esi_nuevo);
				pthread_mutex_unlock(&m_lista_pcbs);

				pthread_mutex_lock(&m_lista_esis_ready);
				if(desalojo == 1 || desalojo == 0){
				calcular_estimacion(pcb_esi_nuevo);
				//log_info(logger, "La nueva estimacion del esi nuevo %d es: %f", pcb_esi_nuevo->id, pcb_esi_nuevo->ultimaEstimacion);
				pcb_esi_nuevo->ultimaRafaga = 0;
				}

				list_add(esis_ready, pcb_esi_nuevo);
				pthread_mutex_unlock(&m_lista_esis_ready);

				if (pthread_create(&hilo_escucha_esi, 0 , manejar_esi, (void*) pcb_esi_nuevo) < 0){
					perror("No se pudo crear el hilo");
				}
			}
		} else {
	        perror("Fallo en el accept");
		}
	}
	log_info(logger, "Saliendo del hilo recibir_esis");
}

//--MANEJAR UN ESI--//
void manejar_esi(void* la_pcb){
	if(list_size(esis_ready) == 1){
		sem_post(&s_planificar);
	}

	//log_info(logger, "Entre a manejar_esi");
	pcb pcb_esi = *((pcb*) la_pcb);
	int chau = 1;

	while(chau > 0 && esi_a_finalizar != pcb_esi.id){
		//log_info(logger, "En manejar_esi y el ID del ESI es: %d", pcb_esi.id);

		int resultado = recibir_un_int(pcb_esi.socket);
		log_info(logger, "El ESI %d me envio el resultado_esi %d:\n", pcb_esi.id, resultado);

		if (resultado >= 0){ //&terminar_todo != -1
			switch (resultado){
				case (84):
					if (terminar_todo != -1){
					sem_post(&s_planificar);
					}
				break;
				case (90):
					pthread_mutex_lock(&m_lista_esis_ready);
					mover_esi_a_bloqueados(clave_buscada, id_esi_ejecutando);
					if (terminar_todo != -1){
					sem_post(&s_planificar);
					}
				break;
				case (81):
					mover_esi_a_finalizados(id_esi_ejecutando);
					chau = 0;
				break;
				case (20): //exit de consola
					esi_a_finalizar = pcb_esi.id;
					chau = 0;
				break;
				default:
					abortar_esi(pcb_esi.id);
					procesar_motivo_aborto(resultado);
					chau = 0;
				break;
			}
		}
		if (terminar_todo == -1 && chau == 1){
			sem_wait(&s_planificar); //shady
			sem_post(&s_podes_cerrar_dice_el_esi);
			chau = 0;
		}
	}
	if(pcb_esi.id == esi_a_finalizar){
		mover_esi_a_finalizados(pcb_esi.id);
	}
	log_info(logger, "Saliendo del hilo manejar_esi %d", pcb_esi.id);
}

//--MANEJAR COORDINADOR--//
void manejar_coordinador(void* socket){
	int socket_coordinador = *((int*) socket);
	//log_info(logger, "Entre al hilo manejar_coordinador");
	int conexion_valida = 1;
	while(conexion_valida > 0){

		int id = recibir_un_int(sockets_planificador.socket_coordinador);
		log_info(logger, "Id recibido del Coordinador: %d", id);

		int tamanio;
		t_esi_operacion instruccion;
		void* buffer;
		switch(id){
		case (82): //nuevo pedido
			tamanio = recibir_un_int(socket_coordinador);

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

	if(terminar_todo != -1){
		cerrar_planificador();
	}
	log_info(logger, "Saliendo del hilo manejar_coordinador");
	//pthread_exit(NULL);
}
