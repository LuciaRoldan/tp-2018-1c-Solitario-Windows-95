#ifndef ESI_FUNCIONES_H_
#define ESI_FUNCIONES_H_

#include "funciones_ESI.h"
#include <commons/bitarray.h>

t_config* leer_arch_configuracion(){
	t_config* configuracion_esi = config_create("Configuracion_ESI.cfg");
	idEsi = config_get_int_value(configuracion_esi, "ID");

	return configuracion_esi;
}

int conectarse_al_Coordinador(t_config * arch_config, t_log* logger){
	int socket = connect_to_server(config_get_string_value(arch_config, "IP_COORDINADOR"), config_get_string_value(arch_config, "PORT_COORDINADOR"), logger);
	return socket;
}

int conectarse_al_Planificador(t_config * arch_config, t_log* logger){
	int socket = connect_to_server(config_get_string_value(arch_config, "IP_PLANIFICADOR"), config_get_string_value(arch_config, "PORT_PLANIFICADOR"), logger);
	return socket;
}

int handshake(int socket_servidor, t_log* logger) {

	t_handshake yo = { ESI };
	void* buffer = malloc(sizeof(int));
	void *hs_recibido = malloc(sizeof(int)*2);

	serializar_handshake(buffer, yo);

	enviar(socket_servidor, buffer, sizeof(int), 80, logger);
	free(buffer);
	recibir(socket_servidor, hs_recibido, sizeof(int)*2, logger);
	int id_recibido = deserializar_id(hs_recibido);
	free(hs_recibido);

	if (id_recibido != 80) {
		log_info(logger, "Conexion invalida");
		return -1;
	} else return 1;
}

int enviar_instruccion_sgte(FILE* archivo, int socket_Coordinador, t_log* logger_esi){
	char* line = "";
	fgets(line, 0, archivo);
	t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	free(line);
	int exito = enviar(socket_Coordinador,&instruccion, sizeof(t_esi_operacion), 24, logger_esi);
	return exito;
}

int ejecutar_instruccion(FILE* script, int socket_Coordinador, t_log* logger){
	if(!feof(script)){
		if(enviar_instruccion_sgte(script, socket_Coordinador, logger) > 0){
			log_info(logger, "Se ha enviado correctamente a instruccion al Planificador \n ");
			return 1;
			} else {
				log_info(logger, "No se pudo enviar la instruccion al Planificador \n");
				return 0;
			}
		} else {
			log_info(logger, "No hay instrucciones disponibles. \n");
			return 0;
		}
}

resultado_esi deserializar_confirmacion(void* buffer_receptor){
	resultado_esi confirmacion;
	memcpy(&confirmacion, (buffer_receptor + (sizeof(int))), sizeof(resultado_esi));
	return confirmacion;
}

void informar_confirmacion(void* msj_recibido, int socket_destino, t_log* logger_esi){
	resultado_esi confirmacion = deserializar_confirmacion(msj_recibido);
	switch(confirmacion){
		case EXITO:
			log_info(logger_esi, "Instruccion ejecutada satisfactoriamente.");
			break;
		case FALLO:
			log_info(logger_esi, "Fallo al ejecutar la instruccion.");
			break;
		case PEDIUNACLAVEMUYLARGA:
			log_info(logger_esi, "Error de clave muy larga.");
			break;
		case PEDIUNACLAVENOID:
			log_info(logger_esi, "Error de clave no identificada.");
			break;
		case PEDIUNACLAVEINACC:
			log_info(logger_esi, "Error de clave inaccesible.");
			break;
	}
	enviar(socket_destino, &confirmacion, sizeof(resultado_esi), 41, logger_esi);
}

/*esi_bloqueado salvar_info_bloqueado(FILE* script){
	esi_bloqueado nuevo_bloqueado;
	nuevo_bloqueado.idESI = idEsi;
	nuevo_bloqueado.script = script;
	nuevo_bloqueado.ultima_posicion = ftell(script);
	return nuevo_bloqueado;
}

nodo_bloqueado *buscar_segun_id(int id, nodo_bloqueado *raiz){
	nodo_bloqueado *aux = raiz;
	while (aux->esi->idESI != id){
		aux = aux->sgte;
	}
	return aux;
}

nodo_bloqueado *buscar_fin_de_lista(nodo_bloqueado * raiz){
	nodo_bloqueado *aux = raiz;
	while (aux->sgte != NULL){
		aux = aux->sgte;
	}
	return aux;
}

void agregar_a_lista(nodo_bloqueado *raiz, esi_bloqueado info){
	nodo_bloqueado *ptr = buscar_fin_de_lista(raiz);
	ptr->sgte = malloc(sizeof(nodo_bloqueado));
	*ptr->sgte->esi = info;
	*ptr->sgte->sgte = NULL;
}

void eliminar_lista_bloqueados(nodo_bloqueado *raiz){
	nodo_bloqueado *aux1 = raiz;
	nodo_bloqueado *aux2 = raiz->sgte;
	while (aux1->sgte != NULL){
		free(aux1->sgte);
		aux1 = aux2;
		aux2 = aux1->sgte;
	}
	free(aux1);
	free(aux2);
	free(raiz);
}

esi_bloqueado desbloquearse(int id, nodo_bloqueado *lista_bloq){
	nodo_bloqueado *ptr = buscar_segun_id(id, lista_bloq);
	return ptr->esi;
}*/

#endif /* ESI_FUNCIONES_H_ */
