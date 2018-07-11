#ifndef FUNCIONES_ESI_H_
#define FUNCIONES_ESI_H_

#include <commons_propias/commons_propias.h>
#include <commons/config.h>

// Variables globales
int idEsi;
t_config* configuracion_esi;
t_log * logger_esi;
t_esi_operacion ultima_instruccion;

// Estructuras
typedef struct{
	int socket_coordi;
	int socket_plani;
} sockets_conexiones;

// Configuracion ESI
sockets_conexiones leer_arch_configuracion();

// Handshake
int handshake(int socket_servidor);

// Verificaciones
int cumple_protocolo(int mensaje, int nro);

// Conexiones
int conectarse_al_Coordinador(void);
int conectarse_al_Planificador(void);

// Acciones
void inicializar_buffer_segun_keyword(void* buffer, t_esi_operacion instruc);
int ejecutar_instruccion(FILE* script, int socket_Coordinador);
int enviar_instruccion_sgte(FILE* script, int socket_destino);
void informar_confirmacion(void* msj_recibido, int socket_destino, t_log* logger);
void informar_fin_de_programa(sockets_conexiones conexiones);

// Serializacion-Deserializacion
void serializar_confirmacion(void* buffer, resultado_esi *msj_confirmacion);
void deserializar_confirmacion(resultado_esi *la_confirmacion, void* buffer);
//int serializar_instruccion1(void* buffer, t_esi_operacion instruccion);
// ----------- nuevas funciones ---------------- //
t_esi_operacion deserializar_instruccion2(void* buffer);
int tamanio_instruccion2(t_esi_operacion instruccion);
void serializar_instruccion2(void* buffer, t_esi_operacion instruccion);


#endif /* FUNCIONES_ESI_H_ */
