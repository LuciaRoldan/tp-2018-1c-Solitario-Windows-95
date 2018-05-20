#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <Commons_propias/commons_propias.h>

///////////////////////// STRUCTS /////////////////////////

typedef enum {LSU, EL, KE} tipo_algoritmo_distribucion;

typedef struct{
	char* ip;
	char* puerto_escucha;
	int* algoritmo_distribucion;
	int* cantidad_entradas;
	int* tamano_entrada;
	int* retardo;
}info_archivo_config;

///////////////////////// FUNCIONES /////////////////////////

info_archivo_config leer_archivo_configuracion(FILE* archivo);
int inicializar_coordinador(info_archivo_config configuracion, t_log* logger);
void conectar_planificador(int* socket_escucha, int* socket_planificador, t_log* logger);
void enviar_configuracion_instancia(info_archivo_config configuracion, t_log* logger);
void enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion, t_log* logger);
int handshake(int* socket, t_log* logger);

#endif /* COORDINADOR_H_ */
