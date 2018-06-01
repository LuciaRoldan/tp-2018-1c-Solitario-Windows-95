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

//INICIALIZACION
info_archivo_config leer_archivo_configuracion(FILE* archivo);
int inicializar_coordinador(info_archivo_config configuracion, t_log* logger);
void conectar_planificador(int* socket_escucha, int* socket_planificador, t_log* logger);

//COMUNICACION
int enviar_configuracion_instancia(int* socket, info_archivo_config configuracion, t_log* logger);
int enviar_pedido_esi(int esi_id, int* socket, t_esi_operacion instruccion, t_log* logger);
int enviar_status_clave(int* socket, status_clave* status, t_log* logger);
int enviar_pedido_valor(int* socket, char* clave, t_log* logger);
int enviar_confirmacion(int* socket, int* confirmacion, int id, t_log* logger);
int recibir_confirmacion(int* socket, t_log* logger);
char* recibir_pedido_clave(int* socket, t_log* logger);
status_clave recibir_status(int* socket, t_log* logger);
t_esi_operacion recibir_instruccion(int* socket, t_log* logger);

//FUNCIONAMIENTO INTERNO
int handshake(int* socket, t_log* logger);
int procesar_mensaje(int id, int* socket, t_log* logger);
void conectar_esi(int* socket, int id_recibido, t_log* logger);
void conectar_instancia(int* socket, int id_recibido, t_log* logger);
void desconectar_instancia();
int* buscar_instancia(char* clave);
int procesar_instruccion(t_esi_operacion instruccion, int* socket, t_log* logger);

#endif /* COORDINADOR_H_ */
