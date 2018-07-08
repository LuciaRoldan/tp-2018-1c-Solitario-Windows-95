
#include "funciones_instancia.h"

char* ipCoordinador;
int puertoCoordinador;
tipo_algoritmo_reemplazo algoritmo_reemplazo;
char* nombreInstancia;
int intervaloDump;
int cantidad_entradas;
int tamano_entrada;
char* memoria;
FILE* archivo_configuracion;
FILE*archivo;
t_log * logger;
int socket_coordinador;

int main() {

	logger = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_INFO);
	configuracion_propia mi_configuracion; //la podria hacer variable global

	leer_configuracion_propia(&mi_configuracion,logger);

	socket_coordinador = connect_to_server(mi_configuracion.ipCoordinador, mi_configuracion.puertoCoordinador, logger);

	handshake(&socket_coordinador,logger, 1);
	// Ivi estaba asi handshake(&socket_coordinador,logger, mi_configuracion.nombreInstancia);
	// lo cambie para testear

//	datos_configuracion configuracion = recibir_configuracion(socket_coordinador,logger);
//	strcpy(configuracion.cantidad_entradas,cantidad_entradas);
//	strcpy(configuracion.tamano_entrada,tamano_entrada);
//	int espacio_para_memoria = cantidad_entradas * tamano_entrada;
//	memoria = malloc(espacio_para_memoria);
//	int matriz_memoria[cantidad_entradas][3];

	return 0;
}

