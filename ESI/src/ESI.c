#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char* argv[]){

	idEsi = atoi(argv[2]);
	sockets_conexiones conexiones;

	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Inicio de ESI %d ---> Logger creado.", idEsi);

	FILE* script_prueba = fopen(argv[1], "r");
	if (script_prueba == NULL){
	perror("Error al abrir el archivo: ");
	exit(EXIT_FAILURE);
	}
	nodo* lista_sentencias = parsear(script_prueba);
	if(lista_sentencias != NULL)
		log_info(logger_esi, "Script parseado para %d", idEsi);
	else
		log_info(logger_esi, "Error parseando el script para %d", idEsi);
	sentencia_actual = lista_sentencias;

	conexiones = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido para ESI %d.", idEsi);

	handshake(conexiones.socket_coordi);
	handshake(conexiones.socket_plani);

	int codigo_plani, codigo_coordi;
	int abortoESI = 0;
	void* mensaje_coordi = malloc(sizeof(resultado_esi));

	while((sentencia_actual!=NULL) || abortoESI == 0) {
		codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
		switch(codigo_plani){
			case 45: //desbloqueo ESI
				ejecutar_ultima_instruccion(conexiones.socket_coordi);
				codigo_coordi = recibir_int(conexiones.socket_coordi, logger_esi);
				if(cumple_protocolo(codigo_coordi, 20)){
					recibir(conexiones.socket_coordi, mensaje_coordi, sizeof(resultado_esi), logger_esi);
					informar_confirmacion(mensaje_coordi, conexiones.socket_plani, logger_esi);
				} else{
					_exit_with_error(conexiones.socket_coordi, "Error en el intercambio de mensajes", mensaje_coordi, logger_esi);
				}
				break;
			case 61: //solicitud de ejecucion
				ejecutar_instruccion_sgte(conexiones.socket_coordi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR desde ESI %d", idEsi);
				codigo_coordi = recibir_int(conexiones.socket_coordi, logger_esi);
				if(cumple_protocolo(codigo_coordi, 20)){
					recibir(conexiones.socket_coordi, mensaje_coordi, sizeof(resultado_esi), logger_esi);
					informar_confirmacion(mensaje_coordi, conexiones.socket_plani, logger_esi);
				} else{
					_exit_with_error(conexiones.socket_coordi, "Error en el intercambio de mensajes", mensaje_coordi, logger_esi);
				}
				break;
			case 62: //abortar ESI
				abortoESI = 1;
				log_info(logger_esi, "Abortando ESI %d", idEsi);
				break;
			default:
				_exit_with_error(conexiones.socket_plani, "Mensaje fuera de protocolo", NULL, logger_esi);
				break;
		}
	}
	fclose(script_prueba);
	informar_fin_de_programa(conexiones);
	free(mensaje_coordi);
	liberar_lista(lista_sentencias);
	free(sentencia_actual);
	close(conexiones.socket_plani);
	close(conexiones.socket_coordi);
	log_info(logger_esi, "Fin de ejecucion de ESI %d\n", idEsi);
	exit(1);
}
