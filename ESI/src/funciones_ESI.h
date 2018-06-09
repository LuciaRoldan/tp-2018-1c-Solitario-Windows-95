#ifndef FUNCIONES_ESI_H_
#define FUNCIONES_ESI_H_

#include <commons_propias/commons_propias.h>
#include <commons/config.h>

//variables globales
int idEsi;

// Lectura de archivo
t_config* leer_arch_configuracion();

// Handshake
int handshake(int* socket_servidor, t_log* logger);

//Conexiones
int conectarse_al_Coordinador(t_config* arch_config, t_log* logger);
int conectarse_al_Planificador(t_config * arch_config, t_log* logger);

// Acciones
int ejecutar_instruccion(FILE* script, int* socket_Coordinador, t_log* logger_esi);
int enviar_instruccion_sgte(int* socket_destino, FILE* script, t_log* logger_esi);
//int enviar_instruccion_a_ejecutar(int socket_Coordinador, t_log* logger_esi);


#endif /* FUNCIONES_ESI_H_ */
