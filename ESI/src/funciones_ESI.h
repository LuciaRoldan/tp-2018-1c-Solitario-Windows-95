#ifndef FUNCIONES_ESI_H_
#define FUNCIONES_ESI_H_

#include <commons_propias/commons_propias.h>
#include <commons/config.h>

// Estructuras
typedef struct{
	int socket_coordi;
	int socket_plani;
} sockets_conexiones;

// Variables globales
int idEsi;
t_config* configuracion_esi;
t_log * logger_esi;
t_esi_operacion ultima_instruccion;

// Configuracion ESI
sockets_conexiones leer_arch_configuracion();

// Handshake
int handshake_del_esi(int socket_servidor);

// Verificaciones
int cumple_protocolo(int mensaje, int nro);

// Conexiones
int conectarse_al_Coordinador(void);
int conectarse_al_Planificador(void);

// Acciones
t_esi_operacion parsear_linea(FILE* archivo);
void mostrar_instruccion(t_esi_operacion instruccion);
void informar_fin_de_programa(sockets_conexiones conexiones, int flag);
int ejecutar_instruccion_sgte(FILE* archivo, int socket_Coordinador);
void error_clave_larga(sockets_conexiones las_conexiones);
int enviar_instruccion(t_esi_operacion operacion, int socket_destino);
void ejecutar_ultima_instruccion(int socket_destino);
void liberar_instruccion(t_esi_operacion instruccion);
void procesar_confirmacion(int confirmacion, int socket_destino);
void informar_confirmacion(int confirmacion, int socket_destino, t_log* logger_esi);


// Serializacion-Deserializacion
void serializar_confirmacion(void* buffer, resultado_esi *msj_confirmacion);
resultado_esi deserializar_confirmacion(void* buffer);

#endif /* FUNCIONES_ESI_H_ */
