#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>
#include <parsi/parser.h>

// Configuracion ESI
sockets_conexiones leer_arch_configuracion(){
	sockets_conexiones conexion;
	configuracion_esi = config_create("ESI/Configuracion_ESI.cfg");
	conexion.socket_coordi = conectarse_al_Coordinador();
	conexion.socket_plani = conectarse_al_Planificador();
	return conexion;
}

// Conexiones
int conectarse_al_Coordinador(){
	int socket = connect_to_server(config_get_string_value(configuracion_esi, "IP_COORDINADOR"), config_get_string_value(configuracion_esi, "PORT_COORDINADOR"), logger_esi);
	return socket;
}

int conectarse_al_Planificador(){
	int socket = connect_to_server(config_get_string_value(configuracion_esi, "IP_PLANIFICADOR"), config_get_string_value(configuracion_esi, "PORT_PLANIFICADOR"), logger_esi);
	return socket;
}

// Handshake
int handshake_del_esi(int socket_servidor) {

	t_handshake yo = { idEsi, ESI };
	t_handshake proceso_recibido;
	void* buffer = malloc(sizeof(int)*3);
	void *hs_recibido = malloc(sizeof(int)*2);
	int protocolo;

	serializar_handshake(buffer, yo);
	enviar(socket_servidor, buffer, sizeof(int)*3, logger_esi);
	log_info(logger_esi, "Envie el Handshake");
	free(buffer);

	protocolo = recibir_int(socket_servidor, logger_esi);
	log_info(logger_esi, "Id de protocolo recibido: %d \n", protocolo);
	if (cumple_protocolo(protocolo, 80) == 0) {
		log_info(logger_esi, "Conexion invalida: error en protocolo.");
		return -1;
	}

	recibir(socket_servidor, hs_recibido, sizeof(int)*2, logger_esi);
	proceso_recibido = deserializar_handshake(hs_recibido);
	free(hs_recibido);

	switch(proceso_recibido.proceso){
		case PLANIFICADOR:
			log_info(logger_esi, "Soy ESI %d y me conecte con el Planificador", idEsi);
			return 1;
			break;
		case COORDINADOR:
			log_info(logger_esi, "Soy ESI %d y me conecte con el Coordinador", idEsi);
			return 1;
			break;
		default:
			log_info(logger_esi, "Conexion invalida: error de proceso no válido.");
			return -1;
			break;
	}
}

// Verificaciones
int cumple_protocolo(int mensaje, int nro){
	if(mensaje == nro){
		return 1;
	} else return 0;
}

// Acciones
t_esi_operacion parsear_linea(FILE* archivo){
	char* line = NULL;
	size_t len = 0;
	//ssize_t read;
	if(me_bloquearon > 0){
		fseek(archivo, -ultimo_read, SEEK_CUR);
		me_bloquearon = 0;
	}
	ultimo_read = getline(&line, &len, archivo);
	t_esi_operacion parsed = parse(line);
	return parsed;
}

int enviar_instruccion(t_esi_operacion instruccion, int socket_Coordinador){
	int tamanio_buffer = tamanio_buffer_instruccion(instruccion);
	void* buffer_instruccion = malloc(tamanio_buffer);
	serializar_instruccion(buffer_instruccion, instruccion);
	int exito = enviar(socket_Coordinador, buffer_instruccion, tamanio_buffer, logger_esi);
	log_info(logger_esi, "Instruccion enviada al COORDINADOR.");
	free(buffer_instruccion);
	return exito;
}

void mostrar_instruccion(t_esi_operacion instruccion){
	switch(instruccion.keyword){
		case GET:
			log_info(logger_esi, "La instruccion parseada es: GET %s", instruccion.argumentos.GET.clave);
			break;
		case SET:
			log_info(logger_esi, "La instruccion parseada es: SET %s %s", instruccion.argumentos.SET.clave, instruccion.argumentos.SET.valor);
			break;
		case STORE:
			log_info(logger_esi, "La instruccion parseada es: STORE %s", instruccion.argumentos.STORE.clave);
			break;
		}
}

bool excede_tamanio_clave(t_esi_operacion op){
	switch(op.keyword){
	case GET:
		return strlen(op.argumentos.GET.clave) > 40;
		break;
	case SET:
		return strlen(op.argumentos.SET.clave) > 40;
		break;
	case STORE:
		return strlen(op.argumentos.STORE.clave) > 40;
		break;
	default:
		return false;
	}
}

void error_clave_larga(sockets_conexiones las_conexiones){
	void *protocolo = malloc(sizeof(int));
	serializar_id(protocolo, 86);
	enviar(las_conexiones.socket_coordi, protocolo, sizeof(int), logger_esi);
	enviar(las_conexiones.socket_plani, protocolo, sizeof(int), logger_esi);
	free(protocolo);
}

void informar_fin_de_programa(sockets_conexiones conexiones, int aborto){
	if(aborto == 0){
		int envio;
		serializar_id(&envio, 81);
		log_info(logger_esi, "Fin de script.\n");
		enviar(conexiones.socket_coordi, &envio, sizeof(int), logger_esi);
		enviar(conexiones.socket_plani, &envio, sizeof(int), logger_esi);
	} else{
		log_info(logger_esi, "Abortando ESI.\n");
		int aborto;
		serializar_id(&aborto, 85);
		enviar(conexiones.socket_plani, &aborto, sizeof(int), logger_esi);
	}
}

int ejecutar_instruccion_sgte(FILE* archivo, int socket_Coordinador){
	t_esi_operacion operacion = parsear_linea(archivo);
	if(!operacion.valido) return -1;
	log_info(logger_esi,"Parseado de instruccion completo");
	mostrar_instruccion(operacion);
	if(enviar_instruccion(operacion, socket_Coordinador) > 0){
		liberar_instruccion(operacion);
		return 1;
	} else {
		liberar_instruccion(operacion);
		return 0;
	}
}

void liberar_instruccion(t_esi_operacion operacion){
	switch(operacion.keyword){
	case GET:
		free(operacion.argumentos.GET.clave);
		break;
	case SET:
		free(operacion.argumentos.SET.clave);
		free(operacion.argumentos.SET.valor);
		break;
	case STORE:
		free(operacion.argumentos.STORE.clave);
		break;
	}
}

int informar_confirmacion(int confirmacion, int socket_destino, t_log* logger_esi){
	int aborto = 0;
	switch(confirmacion){
		case 84:
			log_info(logger_esi, "Instruccion ejecutada satisfactoriamente.");
			break;
		case 85:
			log_info(logger_esi, "Fallo al ejecutar la instruccion.");
			aborto = 1;
			break;
		case 87:
			log_info(logger_esi, "Error de clave no identificada.");
			aborto = 1;
			break;
		case 88:
			log_info(logger_esi, "Error de clave inaccesible.");
			aborto = 1;
			break;
		case 89:
			log_info(logger_esi, "Error de clave no bloqueada.");
			aborto = 1;
			break;
		case 90:
			me_bloquearon = 1;
			log_info(logger_esi, "Bloquear ESI.");
			break;
		default:
			log_info(logger_esi, "Id fuera de protocolo");
			return 1;
			break;
	}
	int buffer_confirmacion;
	serializar_id(&buffer_confirmacion, confirmacion);
	enviar(socket_destino, &buffer_confirmacion, sizeof(int), logger_esi);
	return aborto;
}

int enviar_exit_coordi(int socket){
	int envio;
	serializar_id(&envio, 81);
	return enviar(socket, &envio, sizeof(int), logger_esi);
}

#endif /* ESI_FUNCIONES_H_ */
