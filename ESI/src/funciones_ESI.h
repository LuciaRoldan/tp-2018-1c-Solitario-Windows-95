#ifndef FUNCIONES_ESI_H_
#define FUNCIONES_ESI_H_

#include <commons_propias/commons_propias.h>
#include <commons/config.h>

// Variables globales
int idEsi; // ESTO LO DEBERIA MANEJAR EL PLANI CON LOS HILOS!!
estado estado_ESI;

/*/ Estructuras
typedef struct{
	int idESI;
	FILE* script;
	int ultima_posicion;
} esi_bloqueado;

typedef struct{
	esi_bloqueado esi;
	nodo_bloqueado *sgte;
} nodo_bloqueado;*/

// Lectura de archivo
t_config* leer_arch_configuracion();

// Handshake
int handshake(int socket_servidor, t_log* logger);

//Conexiones
int conectarse_al_Coordinador(t_config* arch_config, t_log* logger);
int conectarse_al_Planificador(t_config * arch_config, t_log* logger);

/*/ Manejo de listas
nodo_bloqueado *buscar_fin_de_lista(nodo_bloqueado * raiz);
void agregar_a_lista(nodo_bloqueado *raiz, esi_bloqueado nodo_nuevo);
nodo_bloqueado *buscar_segun_id(int id, nodo_bloqueado *raiz);
void eliminar_lista_bloqueados(nodo_bloqueado *raiz);*/

// Acciones
int ejecutar_instruccion(FILE* script, int socket_Coordinador, t_log* logger_esi);
int enviar_instruccion_sgte(FILE* script, int socket_destino, t_log* logger_esi);
void informar_confirmacion(void* msj_recibido, int socket_destino, t_log* logger);

// Serializacion-Deserializacion
resultado_esi deserializar_confirmacion(void* buffer);

/*/ Bloqueos
esi_bloqueado desbloquearse(int id, nodo_bloqueado *lista_bloq);*/


#endif /* FUNCIONES_ESI_H_ */
