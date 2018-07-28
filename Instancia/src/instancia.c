#include <stdio.h>
#include <stdlib.h>

#include "funciones_instancia.h"


int main(int argc, char* argv[]){

	id_instancia = atoi(argv[2]);

	indice = 0;
	activa = true;
	puntero_circular = 0;
	lugar_de_memoria = 0;
	puntero_pagina = 0;
	puntero_entrada = 0;
	char log[10] = "INSTANCIA";
	memcpy(&log[9], argv[2], 1);
	leer_configuracion_propia(argv[1], &mi_configuracion);

	logger = log_create("instancia.log", mi_configuracion.nombreInstancia, true, LOG_LEVEL_INFO);

	if (pthread_mutex_init(&m_tabla, NULL) != 0) {printf("Fallo al inicializar mutex\n");}

	socket_coordinador = connect_to_server(mi_configuracion.ipCoordinador, mi_configuracion.puertoCoordinador, logger);

	log_info(logger,"Hay socket con el Coordinador");

	handshake_instancia(socket_coordinador,logger, id_instancia);
	log_info(logger,"Recibi el handshake del cordi");
	log_info(logger, "Mi algoritmo de reemplazo es: %d ", mi_configuracion.algoritmoDeReemplazo);

	int id = recibir_int(socket_coordinador,logger);
	log_info(logger,"recibo un int \n");

	while(id != 00){
		log_info(logger,"Pero no es el de configuracion :( ");
		id = recibir_int(socket_coordinador,logger);
	}

//	recibir_configuracion(socket_coordinador,logger);
	configuracion_coordi.tamano_entrada = recibir_int(socket_coordinador,logger) + 1;
	configuracion_coordi.cantidad_entradas = recibir_int(socket_coordinador,logger);

	log_info(logger,"Recibi la configuracion! ");
	cantidad_entradas = configuracion_coordi.cantidad_entradas;
	log_info(logger,"Mi cantidad de entradas es: %d ", cantidad_entradas);
	log_info(logger,"Mi tamanio de entrada es : %d ", configuracion_coordi.tamano_entrada);


	/*char* espacio_bitmap = malloc(cantidad_entradas);
	precencia = bitarray_create_with_mode(espacio_bitmap, cantidad_entradas, MSB_FIRST);*/ //Lo de bitarray

	acceso_tabla = (int*) malloc(cantidad_entradas*sizeof(int));
	for(int i = 0; i< cantidad_entradas; i++){
		acceso_tabla[i]=0;
	}
	log_info(logger, "La cantidad de entradas es: %d", cantidad_entradas);

	for(int i = 0; i< cantidad_entradas; i++){
		printf("%d, ", acceso_tabla[i]);
	}

	memoria_usada = 0;
	indice = 0;
	log_info(logger,"Creo un array para saber las entradas ocupadas y la vacio para que la tabla de entradas comience vacia");
	tabla_entradas = list_create();
	log_info(logger,"Creo la tabla de entradas");
	memoria_total = configuracion_coordi.cantidad_entradas * configuracion_coordi.tamano_entrada;
	inicio_memoria = malloc(memoria_total);
	log_info(logger,"Guardo la memoria para los valores");

	int reincorporacion = recibir_int(socket_coordinador,logger);

	while(reincorporacion != 04){
//		siempre va a recibir la reinco aunque sea vacio
		reincorporacion = recibir_int(socket_coordinador,logger);
	}
	reincorporarInstancia();

//
	pthread_t hilo_dump;
	pthread_create(&hilo_dump, 0, dump, NULL);

	while(activa){
		procesarID(socket_coordinador,logger);
	}

	//free(espacio_para_memoria);
	pthread_join(&hilo_dump, NULL);
	free(acceso_tabla);
	close(socket_coordinador);
	return 0;
}
