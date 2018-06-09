#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

t_log * logger_esi, logger_conexion;
t_conexion conexion_plani, conexion_coordi;

int main(){
	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Logger creado");
	t_config* datos_configuracion = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido");

	int socket_Coordinador = conectarse_al_Coordinador(datos_configuracion, logger_esi);
	handshake(&socket_Coordinador, logger_esi);
	log_info(logger_esi, "Conectado a COORDINADOR");
	int socket_Planificador = conectarse_al_Planificador(datos_configuracion, logger_esi);
	handshake(&socket_Planificador, logger_esi);
	log_info(logger_esi, "Conectado a PLANIFICADOR");

	int codigo_plani, codigo_coordi, exito;
	resultados_esi resultado;

	FILE* ESI_1 = fopen("../Prubeas-ESI/ESI_1.txt", "r");
	log_info(logger_esi, "Abriendo archivo ESI_1");

	while(1){
		recibir(&socket_Planificador, &codigo_plani, sizeof(int), logger_esi);
		switch(codigo_plani){
			case 61:
				ejecutar_instruccion(ESI_1, &socket_Coordinador, logger_esi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR");
				recibir(&socket_Coordinador, &codigo_coordi, sizeof(int), logger_esi);
				break;
			/*case 40:
				recibir(&socket_Planificador, &mensaje, sizeof(mensaje), logger_esi);
				exito = enviar_instruccion_a_ejecutar(socket_Coordinador, &mensaje, logger_esi);
				if(!exito){
					enviar(&socket_Planificador, 60, sizeof(int), logger_esi);
				}
				break;*/
			default:
				fclose(ESI_1);
				log_info(logger_esi, "Fin de programa");
				exit(1);
				break;
		}
	}
}
