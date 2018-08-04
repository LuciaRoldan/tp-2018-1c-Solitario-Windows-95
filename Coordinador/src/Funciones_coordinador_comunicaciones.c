#include "Funciones_coordinador.h"

///////////////////////////////////////////////// COMUNICACION /////////////////////////////////////////////////

int enviar_configuracion_instancia(int socket){
	int tamanio = info_coordinador.tamano_entrada;
	int cantidad = info_coordinador.cantidad_entradas;
	datos_configuracion mensaje = {tamanio, cantidad};
	void* buffer = malloc(sizeof(int)*3);
	serializar_configuracion_inicial_instancia(buffer, mensaje);
	int bytes_enviados = enviar(socket, buffer, sizeof(int)*3, logger);
	free(buffer);
	return bytes_enviados;
}

int enviar_status_clave(int socket, status_clave status){
	int tamanio = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio);
	serializar_status_clave(buffer, status);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return bytes_enviados;
}

int enviar_pedido_valor(int socket, char* clave, int id){
	int tamanio = tamanio_buffer_string(clave);
	log_info(logger, "El tamanio de lo que mando es: %d", tamanio);
	void* buffer = malloc(tamanio);
	serializar_string_log(buffer, clave, id, logger);
	int bytes_enviados = enviar(socket, buffer, tamanio, logger);
	log_info(logger, "Pedido_valor enviado a la instancia");
	free(buffer);
	return bytes_enviados;
}

int enviar_confirmacion(int socket, int confirmacion, int id){
	void* buffer = malloc(sizeof(int)*2);
	serializar_int(buffer, confirmacion, id);
	int bytes_enviados = enviar(socket, buffer, sizeof(int)*2, logger);
	free(buffer);
	return bytes_enviados;
}

int recibir_confirmacion(int socket){
	int confirmacion;
	void* buffer = malloc(sizeof(int));
	recibir(socket, buffer, sizeof(int), logger);
	confirmacion = deserializar_id(buffer);
	free(buffer);
	return confirmacion;
}

char* recibir_pedido_clave(int socket){
	char* clave;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	void* buffer = malloc(tamanio);
	clave = malloc(tamanio - sizeof(int));
	log_info(logger, "El tamanio es %d", tamanio);
	recibir(socket, buffer, tamanio, logger);
	deserializar_string(buffer,clave);
	free(buffer);
	return clave;
}

status_clave recibir_status(int socket){
	log_info(logger, "Adetro de recibir_status");
	status_clave status;
	int tamanio;
	recibir(socket, &tamanio, sizeof(int), logger);
	log_info(logger, "Recibi el tamanio %d",tamanio);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	log_info(logger, "Lo recibi");
	status = deserializar_status_clave_log(buffer, logger);
	log_info(logger, "Deserialice");
	free(buffer);
	return status;
}

t_esi_operacion recibir_instruccion(int socket){
	t_esi_operacion instruccion;
	int tamanio;
	void* buffercito = malloc(sizeof(int));
	recibir(socket, buffercito, sizeof(int), logger);
	tamanio = deserializar_id(buffercito);
	void* buffer = malloc(tamanio);
	recibir(socket, buffer, tamanio, logger);
	instruccion = deserializar_instruccion(buffer);
	free(buffer);
	free(buffercito);
	return instruccion;
}

int enviar_operacion(int socket, t_esi_operacion instruccion){
	int tamanio = tamanio_buffer_instruccion(instruccion);
	void* buffer = malloc(tamanio);
	serializar_instruccion(buffer, instruccion);
	int resultado = enviar(socket, buffer, tamanio, logger);
	free(buffer);
	return resultado;
}

void enviar_clave(char* clave, int socket){
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer = malloc(tamanio_buffer);
	serializar_string_log(buffer,clave, 06,logger);
	int bytes = enviar(socket, buffer, tamanio_buffer, logger);
	free(buffer);
}


