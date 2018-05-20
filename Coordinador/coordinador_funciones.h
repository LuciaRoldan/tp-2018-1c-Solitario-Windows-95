#ifndef COORDINADOR_FUNCIONES_H_
#define COORDINADOR_FUNCIONES_H_
#include "coordinador.h"



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


info_archivo_config inicializar_coordinador(FILE* archivo, t_log logger){
	info_archivo_config datos_configuracion = leer_archivo_configuracion(archivo);
	inicializar_servidor(datos_configuracion.ip, datos_configuracion.puerto_escucha,logger);
	return datos_configuracion;
}


void enviar_configuracion_instancia(info_archivo_config configuracion, t_log logger){
	datos_configuracion mensaje = {*configuracion.tamano_entrada, *configuracion.cantidad_entradas};
	send_content(4, &mensaje, 1, logger); //Que socket le pongo??
}

void enviar_pedido_esi(int esi_id, int socket, t_esi_operacion instruccion, t_log logger){
	pedido_esi pedido = {esi_id, instruccion};
	enviar(socket, pedido, sizeof(pedido), logger);
}

void handshake(int socket){
	int proceso_recibido;
	send(socket, COORDINADOR , sizeof(proceso),0);
	recv(socket, &proceso_recibido , sizeof(proceso),0);
	switch(proceso_recibido){
	case PLANIFICADOR:
		//conectar_planificador();
		break;
	case INSTANCIA:
		//manejar_instancia();
		break;
	case ESI:
		//manejar_esi();
		break;
	case COORDINADOR:
		//rompo_todo();
		break;
	}
}

void procesar_conexiones(int socket){
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
