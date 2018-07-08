#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

t_log * logger_esi;

int main(int argc, char* argv[]){
	int idEsi = atoi(argv[2]);
	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Logger creado para ESI %d", idEsi);
	t_config* datos_configuracion = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido para ESI %d", idEsi);

	int socket_Coordinador = conectarse_al_Coordinador(datos_configuracion, logger_esi);
	handshake(socket_Coordinador, idEsi, logger_esi);
	log_info(logger_esi, "ESI %d conectado a COORDINADOR", idEsi);

	int socket_Planificador = conectarse_al_Planificador(datos_configuracion, logger_esi);
	handshake(socket_Planificador, idEsi, logger_esi);
	log_info(logger_esi, "ESI %d conectado a PLANIFICADOR", idEsi);

	int codigo_plani;
	int abortoESI = 0;
	void* mensaje_coordi = malloc((sizeof(int)) + (sizeof(resultado_esi)));

	FILE* script_prueba = fopen(argv[1], "r");
	while(!feof(script_prueba) || abortoESI == 0) {
		codigo_plani = recibir_int(socket_Planificador, logger_esi);
		switch(codigo_plani){
			case 61:
				ejecutar_instruccion(script_prueba, socket_Coordinador, logger_esi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR desde ESI %d", idEsi);
				recibir(socket_Coordinador, mensaje_coordi, sizeof(resultado_esi) + sizeof(int), logger_esi);
				informar_confirmacion(mensaje_coordi, socket_Planificador, logger_esi);
				break;
			case 62:
				abortoESI = 1;
				log_info(logger_esi, "Abortando ESI %d", idEsi);
				break;
			default:
				break;
		}
	}
	fclose(script_prueba);
	free(mensaje_coordi);
	close(socket_Planificador);
	close(socket_Coordinador);
	log_info(logger_esi, "Fin de ejecucion de ESI %d", idEsi);
	exit(1);
}
