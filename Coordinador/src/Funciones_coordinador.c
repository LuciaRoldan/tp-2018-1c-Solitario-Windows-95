#include "Funciones_coordinador.h"


/////////////////////// INICIALIZACION ///////////////////////

void leer_archivo_configuracion(info_archivo_config* configuracion){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	FILE* archivo = fopen("Configuracion_coordinador.txt", "r");

	if (archivo < 1) {
		log_info(logger, "No se puede abrir el archivo Configuracion_coordinador.txt");
		exit(1);
	}

	fscanf(archivo, "%s %d %d %d %d %d",
			configuracion->ip,
			&(configuracion->puerto_escucha),
			&(configuracion->algoritmo_distribucion),
			&(configuracion->cantidad_entradas),
			&(configuracion->tamano_entrada),
			&(configuracion->retardo));
	fclose(archivo);
}

int inicializar_coordinador(info_archivo_config configuracion){
	int socket_escucha = inicializar_servidor(configuracion.puerto_escucha, logger);
	return socket_escucha;
}

void conectar_planificador(int* socket_escucha){
	int socket_cliente = aceptar_conexion(socket_escucha);
	int resultado = handshake(&socket_cliente);
	if(resultado >= 0){
		socket_planificador = socket_cliente;
		log_info(logger, "Se establecio la conexion con el Planificdor");
	} else {
		log_info(logger, "Fallo en la conexion con el Planificdor");
	}
}

/////////////////////// COMUNICACION ///////////////////////

int enviar_configuracion_instancia(int* socket, info_archivo_config configuracion){
	datos_configuracion mensaje = {configuracion.tamano_entrada, configuracion.cantidad_entradas};
	//serializar
	int bytes_enviados = enviar(socket, &mensaje, sizeof(datos_configuracion), 00, logger);
	return bytes_enviados;
}

int enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion){
	pedido_esi pedido = {esi_id, instruccion};
	//serializar
	int bytes_enviados = enviar(socket, &pedido, sizeof(pedido), 43, logger);
	return bytes_enviados;
}

int enviar_status_clave(int* socket, status_clave* status){
	//serializar
	int bytes_enviados = enviar(socket, status, sizeof(status_clave), 44, logger);
	return bytes_enviados;
}

int enviar_pedido_valor(int* socket, char* clave){
	//serializar
	int bytes_enviados = enviar(socket, clave, sizeof(clave), 01, logger);
	return bytes_enviados;
}

int enviar_confirmacion(int* socket, int* confirmacion, int id){
	//serializa
	int bytes_enviados = enviar(socket, confirmacion, sizeof(int), id, logger);
	return bytes_enviados;
}

int recibir_confirmacion(int* socket){
	int confirmacion;
	recibir(socket, &confirmacion, sizeof(int), logger);
	//deserializar
	return confirmacion;
}

char* recibir_pedido_clave(int* socket){
	char* clave;
	recibir(socket, clave, sizeof(clave), logger);
	//deserializar?
	return clave;
}

status_clave recibir_status(int* socket){
	status_clave status;
	recibir(socket, &status, sizeof(status_clave), logger);
	//deserializar?
	return status;
}

t_esi_operacion recibir_instruccion(int* socket){
	t_esi_operacion instruccion;
	recibir(socket, &instruccion, sizeof(t_esi_operacion), logger);
	//deserializar
	return instruccion;
}


/////////////////////// FUNCIONAMIENTO INTERNO ///////////////////////

int handshake(int* socket_cliente){
	int conexion_hecha = 0;

	t_handshake proceso_recibido;
	t_handshake yo = {COORDINADOR, 0};
	void* buffer = malloc(sizeof(int)*2);
	serializar_handshake(buffer, yo);
	int leido;

	recibir(socket_cliente, &leido, sizeof(int), logger);
	proceso_recibido.proceso = leido;
	recibir(socket_cliente, &leido, sizeof(int), logger);
	proceso_recibido.id_proceso = leido;

	printf("%d\n", proceso_recibido.proceso);
	printf("%d\n", proceso_recibido.id_proceso);

	enviar(socket_cliente, buffer, sizeof(buffer), 80, logger);



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
		conectar_instancia(socket_cliente, proceso_recibido.id_proceso);
		return 1;
		break;

	case ESI:
		conectar_esi(socket_cliente, proceso_recibido.id_proceso);
		return 1;
		break;

	default:
		return -1;
		break;
	}

}

void procesar_conexion(int* socket_escucha){
	while(1){
		int socket_cliente = aceptar_conexion(socket_escucha);
		handshake(&socket_cliente);
	}
}

void atender_planificador(int* socket_planificador){
	int id_mensaje;
	while(1){
		recibir(socket_planificador, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket_planificador);
	}
}

void conectar_esi(int* socket, int id_recibido){
	int id_proceso = id_recibido;
	int id_mensaje;
	while(1){
		recibir(socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket);
	}
}

void conectar_instancia(int* socket, int id_recibido){

	log_info(logger, "Conectado a la INSTANCIA");
	int id_proceso = id_recibido;
	int id_mensaje;
	while(1){
		recibir(socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket);
	}
}

int procesar_mensaje(int id, int* socket){
	int resultado;
	char* clave;
	int* socket_instancia;
	t_esi_operacion instruccion;
	status_clave status;

	switch(id){
		case 20:
			resultado = recibir_confirmacion(socket);
			return resultado;
			break;
		case 21:
			clave = recibir_pedido_clave(socket);
			socket_instancia = buscar_instancia(clave); //falta
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

int procesar_instruccion(t_esi_operacion instruccion, int* socket){
	return 1;
}
