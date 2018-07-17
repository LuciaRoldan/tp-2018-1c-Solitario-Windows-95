
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

	log_info(logger,"Hay socket con el Coordinador");

	handshake_instancia(socket_coordinador,logger, mi_configuracion.nombreInstancia);
	log_info(logger,"Recibi el handshake del cordi");
	printf("Mi nombre es: %d ", mi_configuracion.nombreInstancia);

	int id = recibir_int(socket_coordinador,logger);
	log_info(logger,"recibo un int ");
	printf("El int es: %d ", id);

	while(id != 00){
		log_info(logger,"Pero no es el de configuracion :( ");
		id = recibir_int(socket_coordinador,logger);
	}
	configuracion = recibir_configuracion(socket_coordinador,logger);
	log_info(logger,"Recibi la configuracion! ");

	const int cantidad_entradas = configuracion.cantidad_entradas;
	acceso_tabla = (int*) malloc(cantidad_entradas*sizeof(int));
	for(int i = 0; i< cantidad_entradas; i++){
		acceso_tabla[i]=0;
	}
	log_info(logger,"Creo un array para saber las entradas ocupadas y la vacio para que la tabla de entradas comience vacia");
	tabla_entradas = list_create();
	log_info(logger,"Creo la tabla de entradas");
	int tamanio_memoria = configuracion.cantidad_entradas * configuracion.tamano_entrada;
	char* inicio_memoria = malloc(tamanio_memoria);
	log_info(logger,"Guardo la memoria para los valores");
//
//	while(1){
//		log_info(logger, "Entré al while");
//		procesarID(socket_coordinador,logger);
//		log_info(logger, "Procesé. Vuelvo a entrar al while");
//	}

	//free(espacio_para_memoria);
	sleep(5);
	free(acceso_tabla);
	return 0;
}

