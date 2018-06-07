#ifndef ESI_H_
#define ESI_H_
#include <Commons_propias/commons_propias.h>
#include <Commons/commons/config.h>


//structs
typedef struct {
	char* ip_coordinador;
	char* puerto_coordinador;
	char* ip_planificador;
	char* puerto_planificador;
} info_arch_config;

// Lectura de archivo
info_arch_config leer_arch_configuracion(FILE* arch_configuracion, t_conexion conexion_plani, t_conexion conexion_coordi);

// Handshake
int handshake(int* socket_servidor, t_log* logger);

// Acciones
int ejecutar_instruccion(FILE* script, int socket_Planificador, t_log* logger_esi);
int enviar_instruccion_sgte(int socket_destino, FILE* archivo);
int enviar_instruccion_a_ejecutar(int socket_Coordinador, t_log* logger_esi);

#endif /* ESI_H_ */
