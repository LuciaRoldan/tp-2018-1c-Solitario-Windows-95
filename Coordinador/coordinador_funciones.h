#ifndef COORDINADOR_FUNCIONES_H_
#define COORDINADOR_FUNCIONES_H_
#include "coordinador.h"


/////////////////////// INICIALIZACION ///////////////////////

info_archivo_config leer_archivo_configuracion(FILE* archivo){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	info_archivo_config datos_configuracion;
	archivo = fopen("Configuracion coordinador.txt", "r");
	fscanf(archivo, "%s %s %d %d %d %d", datos_configuracion.ip,
			datos_configuracion.puerto_escucha, datos_configuracion.algoritmo_distribucion ,
			datos_configuracion.cantidad_entradas, datos_configuracion.tamano_entrada,
			datos_configuracion.retardo);
	fclose(archivo);
	return datos_configuracion;
}

int inicializar_coordinador(info_archivo_config configuracion, t_log* logger){
	int socket_escucha = inicializar_servidor(configuracion.ip, configuracion.puerto_escucha,logger);
	return socket_escucha;
}

void conectar_planificador(int* socket_escucha, int* socket_planificador, t_log* logger){
	int socket_cliente = aceptar_conexion(socket_escucha);
	int resultado = handshake(&socket_cliente, logger);
	if(resultado >= 0){
		*socket_planificador = socket_cliente;
		log_info(logger, "Se establecio la conexion con el Planificdor");
	} else {
		log_info(logger, "Fallo en la conexion con el Planificdor");
	}
}

/////////////////////// COMUNICACION ///////////////////////

int enviar_configuracion_instancia(int* socket, info_archivo_config configuracion, t_log* logger){
	datos_configuracion mensaje = {*configuracion.tamano_entrada, *configuracion.cantidad_entradas};
	//serializar
	int bytes_enviados = enviar(socket, &mensaje, sizeof(datos_configuracion), 00, logger);
	return bytes_enviados;
}

int enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion, t_log* logger){
	pedido_esi pedido = {esi_id, instruccion};
	//serializar
	int bytes_enviados = enviar(socket, &pedido, sizeof(pedido), 43, logger);
	return bytes_enviados;
}

int enviar_status_clave(int* socket, status_clave* status, t_log* logger){
	//serializar
	int bytes_enviados = enviar(socket, status, sizeof(status_clave), 44, logger);
	return bytes_enviados;
}

int enviar_pedido_valor(int* socket, char* clave, t_log* logger){
	//serializar
	int bytes_enviados = enviar(socket, clave, sizeof(clave), 01, logger);
	return bytes_enviados;
}

int enviar_confirmacion(int* socket, int* confirmacion, int id, t_log* logger){
	//serializa
	int bytes_enviados = enviar(socket, confirmacion, sizeof(int), id, logger);
	return bytes_enviados;
}

int recibir_confirmacion(int* socket, t_log* logger){
	int confirmacion;
	recibir(socket, &confirmacion, sizeof(int), logger);
	//deserializar
	return confirmacion;
}

char* recibir_pedido_clave(int* socket, t_log* logger){
	char* clave;
	recibir(socket, clave, sizeof(clave), logger);
	//deserializar?
	return clave;
}

status_clave recibir_status(int* socket, t_log* logger){
	status_clave status;
	recibir(socket, &status, sizeof(status_clave), logger);
	//deserializar?
	return status;
}

t_esi_operacion recibir_instruccion(int* socket, t_log* logger){
	t_esi_operacion instruccion;
	recibir(socket, &instruccion, sizeof(t_esi_operacion), logger);
	//deserializar
	return instruccion;
}


/////////////////////// FUNCIONAMIENTO INTERNO ///////////////////////

int handshake(int* socket_cliente, t_log* logger){
	int conexion_hecha = 0;

	t_handshake proceso_recibido;
	t_handshake yo = {COORDINADOR, 0};

	recibir(socket_cliente, &proceso_recibido, sizeof(t_handshake), logger);
	enviar(socket_cliente, &yo, sizeof(t_handshake), 80, logger);

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
		conectar_instancia(socket_cliente, proceso_recibido.id_proceso, logger);
		return 1;
		break;

	case ESI:
		conectar_esi(socket_cliente, proceso_recibido.id_proceso, logger);
		return 1;
		break;

	default:
		return -1;
		break;
	}

}

void procesar_conexion(int* socket_escucha, t_log* logger){
	while(1){
		int socket_cliente = aceptar_conexion(socket_escucha, logger);
		handshake(&socket_cliente, logger);
	}
}

void atender_planificador(int* socket_planificador, t_log* logger){
	int id_mensaje;
	while(1){
		recibir(socket_planificador, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket, logger);
	}
}

void conectar_esi(int* socket, int id_recibido, t_log* logger){
	int id_proceso = id_recibido;
	int id_mensaje;
	while(1){
		recibir(socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket, logger);
	}
}

void conectar_instancia(int* socket, int id_recibido, t_log* logger){
	int id_proceso = id_recibido;
	int id_mensaje;
	while(1){
		recibir(socket, &id_mensaje, sizeof(int), logger);
		procesar_mensaje(id_mensaje, socket, logger);
	}
}

int procesar_mensaje(int id, int* socket, t_log* logger){
	int resultado;
	char* clave;
	int* socket_instancia;
	t_esi_operacion instruccion;
	status_clave status;

	switch(id){
		case 20:
			resultado = recibir_confirmacion(socket, logger);
			return resultado;
			break;
		case 21:
			clave = recibir_pedido_clave(socket, logger);
			socket_instancia = buscar_instancia(clave); //falta
			resultado = enviar_pedido_valor(socket_instancia, clave, logger);
			return resultado;
			break;
		case 22:
			status = recibir_status(socket, logger);
			resultado = enviar_status_clave(socket, &status, logger);
			return resultado;
			break;
		case 23:
			desconectar_instancia(); //falta
			return 1;
			break;
		case 24:
			instruccion = recibir_instruccion(socket, logger);
			resultado = procesar_instruccion(instruccion, socket, logger); // falta
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

int procesar_instruccion(t_esi_operacion instruccion, int* socket, t_log* logger){
	return 1;
}

/*int atender_conexion_esi(clave){ //recibe la solicitud del esi con la clave que quiere
	//acceder
	aceptar_solicitud_esi;
	procesar_solicitud_esi;
	enviar_solicitud_instancia;
	logear_respuesta;
	informar_respuesta_planificador;
	return respuesta;
}
mensaje procesar solicitud esi{
	solicitud = recibir(mensaje);

	if(!chequear_uso_de_recursos(key)) //chequea con el planificador si se esta usando la instancia
	{
		return fallo;
	}

	instancia = buscar_instancia(solicitud);
	resultadoInstancia = solicitar_uso(instancia);
	informarUsoInstancia(instancia, esi);
	return mensaje;
}
*/

#endif /* COORDINADOR_FUNCIONES_H_ */
