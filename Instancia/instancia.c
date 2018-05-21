#include <Commons_propias/commons_propias.h>
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
	leer_configuracion_propia();
	int socket_coordinador = connect_to_server(ipCoordinador, puertoCoordinador);
	inicializar_instancia();

	return 0;
}
