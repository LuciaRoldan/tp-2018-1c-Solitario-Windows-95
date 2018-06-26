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
	handshake(socket_Coordinador, logger_esi);
	log_info(logger_esi, "Conectado a COORDINADOR");
	int socket_Planificador = conectarse_al_Planificador(datos_configuracion, logger_esi);
	handshake(socket_Planificador, logger_esi);
	log_info(logger_esi, "Conectado a PLANIFICADOR");

	int codigo_plani;
	void* mensaje_coordi = malloc((sizeof(int)) + (sizeof(resultado_esi)));

	for(int i=1; i<argc; i+=1){
		FILE* script_prueba = fopen(argv[i], "r");
		log_info(logger_esi, "Abriendo un script para ESI");
		while(!feof(script_prueba)){
			recibir(socket_Planificador, &codigo_plani, sizeof(int), logger_esi);
			switch(codigo_plani){
				case 61:
					ejecutar_instruccion(script_prueba, socket_Coordinador, logger_esi);
					log_info(logger_esi, "Instruccion enviada a COORDINADOR");
					recibir(socket_Coordinador, mensaje_coordi, sizeof(int), logger_esi);
					informar_confirmacion(mensaje_coordi, socket_Planificador, logger_esi);
					break;
				/*case 40:
					recibir(&socket_Planificador, &mensaje, sizeof(mensaje), logger_esi);
					exito = enviar_instruccion_a_ejecutar(socket_Coordinador, &mensaje, logger_esi);
					if(!exito){
						enviar(&socket_Planificador, 60, sizeof(int), logger_esi);
					}
					break;*/
				default:
					break;
			}
			fclose(script_prueba);
		}
	}
	log_info(logger_esi, "Fin de programa");
	exit(1);
}



