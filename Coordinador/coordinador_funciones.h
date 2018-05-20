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
	int socket_cliente = aceptar_conexion(socket_escucha, logger);
	int resultado = handshake(&socket_cliente, logger);
	if(resultado >= 0){
		*socket_planificador = socket_cliente;
		log_info(logger, "Se establecio la conexion con el Planificdor");
	} else {
		log_info(logger, "Fallo en la conexion con el Planificdor");
	}
}

/////////////////////// COMUNICACION ///////////////////////

void enviar_configuracion_instancia(int* socket, info_archivo_config configuracion, t_log* logger){
	datos_configuracion mensaje = {*configuracion.tamano_entrada, *configuracion.cantidad_entradas};
	enviar(socket, &mensaje, sizeof(datos_configuracion), 00, logger);
}

void enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion, t_log* logger){
	pedido_esi pedido = {esi_id, instruccion};
	enviar(socket, &pedido, sizeof(pedido), 43, logger);
}

void enviar_status_clase(int* socket, status_clave* status, t_log* logger){
	enviar(socket, status, sizeof(status_clave), 44, logger);
}

void enviar_pedido_valor(int* socket, char* clave, t_log* logger){
	enviar(socket, clave, sizeof(clave), 01, logger);
}

int recibir_confirmacion(int* socket, t_log* logger){
	int confirmacion;
	recibir(socket, &confirmacion, sizeof(int), logger);
	return confirmacion;
}

char* recibir_pedido_clave(int* socket, t_log* logger){
	char* clave;
	recibir(socket, clave, sizeof(clave), logger);
	return clave;
}

char* recibir_valor(int* socket, t_log* logger){
	char* valor;
	recibir(socket, valor, sizeof(valor), logger);
	return valor;
}


/////////////////////// FUNCIONAMIENTO INTERNO ///////////////////////

int handshake(int* socket, t_log* logger){
	int conexion_hecha = 0;

	t_handshake proceso_recibido;
	t_handshake yo = {COORDINADOR, 0};
	int id_recibido;

	enviar(socket, &yo, sizeof(t_handshake), 80, logger);
	recibir(socket, &id_recibido, sizeof(int), logger);

	if(id_recibido != 80){
		return -1;
	}

	recibir(socket, &proceso_recibido, sizeof(t_handshake), logger);

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
		//manejar_instancia();
		return 1;
		break;
	case ESI:
		//manejar_esi();
		return 1;
		break;
	case COORDINADOR:
		return -1;
		break;
	default:
			return -1;
			break;
	}

}

/*void procesar_conexiones(int socket){
	while(1){
		//int cliente = aceptar_conexion();
		int cliente;
		handshake(cliente);
	}
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
