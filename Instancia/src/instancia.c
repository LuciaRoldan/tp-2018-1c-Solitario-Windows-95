#include <stdio.h>
#include <stdlib.h>

#include "funciones_instancia.h"


int main() {
	indice = 0;

	logger = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_INFO);


	leer_configuracion_propia(&mi_configuracion,logger);

	socket_coordinador = connect_to_server(mi_configuracion.ipCoordinador, mi_configuracion.puertoCoordinador, logger);

	log_info(logger,"Hay socket con el Coordinador");

	handshake_instancia(socket_coordinador,logger, mi_configuracion.nombreInstancia);
	log_info(logger,"Recibi el handshake del cordi");
	printf("Mi nombre es: %d ", mi_configuracion.nombreInstancia);

	int id = recibir_int(socket_coordinador,logger);
	log_info(logger,"recibo un int \n");
	printf("El int es: %d \n", id);

	while(id != 00){
		log_info(logger,"Pero no es el de configuracion :( ");
		id = recibir_int(socket_coordinador,logger);
	}
	configuracion = recibir_configuracion(socket_coordinador,logger);
	log_info(logger,"Recibi la configuracion! ");
	log_info(logger,"Mi cantidad de entradas es: %d ", configuracion.cantidad_entradas);
	log_info(logger,"Mi tamanio de entrada es : %d ", configuracion.tamano_entrada);

	const int cantidad_entradas = configuracion.cantidad_entradas;
	acceso_tabla = (int*) malloc(cantidad_entradas*sizeof(int));
	for(int i = 0; i< cantidad_entradas; i++){
		acceso_tabla[i]=0;
	}
	memoria_usada = 0;
	indice = 0;
	log_info(logger,"Creo un array para saber las entradas ocupadas y la vacio para que la tabla de entradas comience vacia");
	tabla_entradas = list_create();
	log_info(logger,"Creo la tabla de entradas");
	memoria_total = configuracion.cantidad_entradas * configuracion.tamano_entrada;
	inicio_memoria = malloc(memoria_total);
	log_info(logger,"Guardo la memoria para los valores");
//
	while(1){
		log_info(logger, "Entré al while");
		procesarID(socket_coordinador,logger);
		log_info(logger, "Procesé. Vuelvo a entrar al while");
	}

	//free(espacio_para_memoria);
	sleep(5);
	free(acceso_tabla);
	return 0;
}

