#include "Funciones_coordinador.h"


/////////////////////// INICIALIZACION ///////////////////////

void leer_archivo_configuracion(info_archivo_config* configuracion){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	FILE* archivo = fopen("Configuracion_coordinador.txt", "r");

	fscanf(archivo, "%s %d %d %d %d %d",
			configuracion->ip,
			&(configuracion->puerto_escucha),
			&(configuracion->algoritmo_distribucion),
			&(configuracion->cantidad_entradas),
			&(configuracion->tamano_entrada),
			&(configuracion->retardo));
	fclose(archivo);
}

void inicializar_coordinador(info_archivo_config configuracion){
	socket_escucha = inicializar_servidor(configuracion.puerto_escucha, logger);
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

/////////////////////// COMUNICACION ///////////////////////

int enviar_configuracion_instancia(int socket, info_archivo_config configuracion){
	datos_configuracion mensaje = {configuracion.tamano_entrada, configuracion.cantidad_entradas};
	//serializar
	int bytes_enviados = enviar(socket, &mensaje, sizeof(datos_configuracion), 00, logger);
	return bytes_enviados;
}

int enviar_pedido_esi(int esi_id, int socket, t_esi_operacion instruccion){
	pedido_esi pedido = {esi_id, instruccion};
	//serializar
	int bytes_enviados = enviar(socket, &pedido, sizeof(pedido), 43, logger);
	return bytes_enviados;
}

int enviar_status_clave(int socket, status_clave* status){
	//serializar
	int bytes_enviados = enviar(socket, status, sizeof(status_clave), 44, logger);
	return bytes_enviados;
}

int enviar_pedido_valor(int socket, char* clave){
	//serializar
	int bytes_enviados = enviar(socket, clave, sizeof(clave), 01, logger);
	return bytes_enviados;
}

int enviar_confirmacion(int socket, int* confirmacion, int id){
	//serializa
	int bytes_enviados = enviar(socket, confirmacion, sizeof(int), id, logger);
	return bytes_enviados;
}

int recibir_confirmacion(int socket){
	int confirmacion;
	recibir(socket, &confirmacion, sizeof(int), logger);
	//deserializar
	return confirmacion;
}

char* recibir_pedido_clave(int socket){
	char* clave;
	recibir(socket, clave, sizeof(clave), logger);
	//deserializar?
	return clave;
}

status_clave recibir_status(int socket){
	status_clave status;
	recibir(socket, &status, sizeof(status_clave), logger);
	//deserializar?
	return status;
}

t_esi_operacion recibir_instruccion(int socket){
	t_esi_operacion instruccion;
	recibir(socket, &instruccion, sizeof(t_esi_operacion), logger);
	//deserializar
	return instruccion;
}


/////////////////////// FUNCIONAMIENTO INTERNO ///////////////////////

int handshake(int socket_cliente){
	int conexion_hecha = 0;

	t_handshake proceso_recibido;
	t_handshake yo = {0, COORDINADOR};
	void* buffer_recepcion = malloc(sizeof(int)*2);
	void* buffer_envio = malloc(sizeof(int)*3);

	recibir(socket_cliente, buffer_recepcion, sizeof(int)*2, logger);
	proceso_recibido = deserializar_handshake(buffer_recepcion);

	printf("proceso recibido %d \n", proceso_recibido.proceso);
	printf("id proceso recibido %d \n", proceso_recibido.id);

	free(buffer_recepcion);

	serializar_handshake(buffer_envio, yo);
	enviar(socket_cliente, buffer_envio, sizeof(int)*3, 80, logger);

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
		//conectar_instancia(socket_cliente, proceso_recibido.id);
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
		log_info(logger, "Entre en el while de procesar conexion");
		printf("socket escucha %d \n", socket_escucha);
		int socket_cliente = aceptar_conexion(socket_escucha);
		printf("socket cliente %d \n", socket_cliente);
		recibir(socket_cliente, &id_mensaje, sizeof(int), logger);
		printf("protocolo recibido %d \n", id_mensaje);
		if(id_mensaje == 80){
			handshake(socket_cliente);
		}
	}
}

void atender_planificador(){
	int id_mensaje;
	while(1){
		log_info(logger, "Entre en el while de atender planificador");
		recibir(socket_planificador, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket_planificador);
	}
}

void atender_esi(void* datos_esi){
	log_info(logger, "Estoy en el hilo del esi!");
	hilo_proceso mis_datos = *((hilo_proceso*)datos_esi);
	int id_mensaje;
	while(1){
		recibir(mis_datos.socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, mis_datos.socket);
	}
}

void atender_instancia(void* datos_instancia){
	log_info(logger, "Estoy en el hilo de la instancia!");
	hilo_proceso mis_datos = *((hilo_proceso*)datos_instancia);
	int id_mensaje;
	while(1){
		recibir(mis_datos.socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, mis_datos.socket);
	}
}

void agregar_nuevo_esi(int socket_esi, int id_esi){
	hilo_proceso datos_esi = {socket_esi, id_esi};
	void* datazos = &datos_esi;
	pthread_t hilo_esi;
	pthread_create(&hilo_esi, 0, atender_esi, datazos);
}

void agregar_nueva_instancia(int socket_instancia, int id_instancia){
	hilo_proceso datos_instancia = {socket_instancia, id_instancia};
	void* datazos = &datos_instancia;
	pthread_t hilo_instanica;
	pthread_create(&hilo_instanica, 0, atender_instancia, datazos);
}

int procesar_mensaje(int id, int socket){
	int resultado;
	char* clave;
	int socket_instancia;
	t_esi_operacion instruccion;
	status_clave status;

	switch(id){
		case 20:
			resultado = recibir_confirmacion(socket);
			return resultado;
			break;
		case 21:
			clave = recibir_pedido_clave(socket);
			//socket_instancia = buscar_instancia(clave); //falta
			resultado = enviar_pedido_valor(socket_instancia, clave);
			return resultado;
			break;
		case 22:
			status = recibir_status(socket);
			resultado = enviar_status_clave(socket, &status);
			return resultado;
			break;
		case 23:
			desconectar_instancia(); //falta
			return 1;
			break;
		case 24:
			instruccion = recibir_instruccion(socket);
			resultado = procesar_instruccion(instruccion, socket); // falta
			return resultado;
			break;
		default:
			return -1;
			break;
	}
}

void desconectar_instancia(){
	//modificar tabla
	//terminar el hilo
}

int* buscar_instancia(char* clave){
	int y = 1;
	int* x = &y;
	return x;
}

int procesar_instruccion(t_esi_operacion instruccion, int socket){
	return 1;
}
