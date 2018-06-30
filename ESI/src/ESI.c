#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

t_log * logger_esi;

int main(int argc, char* argv[]){
	//pasar x param a config argv[1]
	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Logger creado");
	t_config* datos_configuracion = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido");

	int socket_Coordinador = conectarse_al_Coordinador(datos_configuracion, logger_esi);
	log_info(logger_esi, "Conectado a COORDINADOR");
	int socket_Planificador = conectarse_al_Planificador(datos_configuracion, logger_esi);
	log_info(logger_esi, "Conectado a PLANIFICADOR");

	int codigo_plani;
	int abortoESI = 0;
	void* mensaje_coordi = malloc((sizeof(int)) + (sizeof(resultado_esi)));

	FILE* script_prueba = fopen(argv[0], "r");
	log_info(logger_esi, "Abriendo un script para ESI");
	handshake(socket_Coordinador, logger_esi);
	handshake(socket_Planificador, logger_esi);
	do {
		codigo_plani = recibir_int(socket_Planificador, logger_esi);
		switch(codigo_plani){
			case 61:
				ejecutar_instruccion(script_prueba, socket_Coordinador, logger_esi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR");
				recibir(socket_Coordinador, mensaje_coordi, sizeof(resultado_esi) + sizeof(int), logger_esi);
				informar_confirmacion(mensaje_coordi, socket_Planificador, logger_esi);
				break;
			case 62:
				abortoESI = 1;
				log_info(logger_esi, "Abortando ESI");
				break;
			default:
				break;
		}
	} while(!feof(script_prueba) || abortoESI == 0);
	fclose(script_prueba);
	free(mensaje_coordi);
	log_info(logger_esi, "Fin de programa");
	exit(1);
}



