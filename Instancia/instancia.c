#include "instancia_funciones.h"

char* ipCoordinador;
int puertoCoordinador;
tipo_algoritmo_reemplazo algoritmo_reemplazo;
char* nombreInstancia;
int intervaloDump;
int cantidad_entradas;
int tamano_entrada;
int matriz_memoria[cantidad_entradas][3];
int espacio_para_memoria = cantidad_entradas * tamano_entrada;
char* memoria;
FILE* archivo_configuracion;
FILE*archivo;
t_log * logger;
int socket_coordinador;


int main() {

	logger = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_INFO);

	configuracion_propia configuracion_propia = leer_configuracion_propia(archivo);
	strcpy(configuracion_propia.ipCoordinador,ipCoordinador);
	strcpy(configuracion_propia.puertoCoordinador,puertoCoordinador);
	strcpy(configuracion_propia.nombreInstancia,nombreInstancia);
	strcpy(configuracion_propia.intervaloDump,intervaloDump);

	socket_coordinador = connect_to_server(ipCoordinador, puertoCoordinador, logger);

	datos_configuracion configuracion = recibir_configuracion(socket_coordinador,logger);
	strcpy(configuracion.cantidad_entradas,cantidad_entradas);
	strcpy(configuracion.tamano_entrada,tamano_entrada);
	int espacio_para_memoria = cantidad_entradas * tamano_entrada;
	memoria = malloc(espacio_para_memoria);
	int matriz_memoria[cantidad_entradas][3];

	return 0;
}

