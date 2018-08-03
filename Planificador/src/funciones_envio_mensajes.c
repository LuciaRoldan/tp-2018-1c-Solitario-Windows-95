#include "planificador_punto_hache.h"

/////-----ENVIAR-----/////

void enviar_solicitud_ejecucion(pcb* pcb_esi){
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 61);
	log_info(logger, "Solicitud de ejecution enviada al ESI de id %d y socket %d", pcb_esi->id, pcb_esi->socket);
	rafaga_actual += 1;
	enviar(pcb_esi->socket, buffer, sizeof(int), logger);
	//log_info(logger, "Solicitud de ejecucion enviada al ESI: %d", pcb_esi->id);
	free(buffer);
}

//--Informar cosas al Coordinador--//
void informar_aborto_coordinador_clave_no_id(){
	int aborto = 87;
	enviar(sockets_planificador.socket_coordinador, &aborto, sizeof(int), logger);
}
void informar_aborto_coordinador_clave_no_b(){
	int aborto = 89;
	enviar(sockets_planificador.socket_coordinador, &aborto, sizeof(int), logger);
}
void informar_exito_coordinador(){
	int exito = 84;
	enviar(sockets_planificador.socket_coordinador, &exito, sizeof(int), logger);
}
void informar_bloqueo_coordinador(){
	int fallo = 90;
	enviar(sockets_planificador.socket_coordinador, &fallo, sizeof(int), logger);
}
void informar_coordi_kill(int id_Esi){
	void* buffer_envio = malloc(sizeof(int)*2);
	serializar_int(buffer_envio, id_Esi, 91);
	enviar(sockets_planificador.socket_coordinador, buffer_envio, sizeof(int)*2, logger);
	free(buffer_envio);
}



/////-----RECIBIR-----/////
int recibir_un_int(int un_socket){
	void* buffer_resultado = malloc(sizeof(int));
	recibir(un_socket, buffer_resultado, sizeof(int), logger);
	int resultado = deserializar_id(buffer_resultado);
	free(buffer_resultado);
	return resultado;
}
