
#include "funciones_instancia.h"

configuracion_propia mi_configuracion;
char* ipCoordinador;
int puertoCoordinador;
tipo_algoritmo_reemplazo algoritmo_reemplazo;
int idInstancia;
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


	leer_configuracion_propia(&mi_configuracion,logger);

	socket_coordinador = connect_to_server(mi_configuracion.ipCoordinador, mi_configuracion.puertoCoordinador, logger);

	handshake(&socket_coordinador,logger, mi_configuracion.nombreInstancia);
	char* clave;

	int id = recibir_int(socket_coordinador, logger);
	log_info(logger,"Recibi %d bytes", sizeof(id));
	log_info(logger,"Recibi el id: %d", id);

	clave = recibe_pedido_status(socket_coordinador,logger);
	log_info(logger,"Recibi esta clave: %s", clave);



	int espacio_para_memoria = cantidad_entradas * tamano_entrada;

	diccionario_memoria = dictionary_create();
	diccionario_memoria->table_max_size = espacio_para_memoria;
	memoria = malloc(espacio_para_memoria);

	free(espacio_para_memoria);
	sleep(5);
	return 0;
}

