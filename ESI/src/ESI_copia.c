#include "funciones_ESI.c"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int idEsi;
FILE* arch_configuracion;
info_arch_config datos_configuracion;
t_conexion conexion_plani, conexion_coordi;
t_log * logger_esi, logger_conexion;


int main(){
	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	datos_configuracion = leer_arch_configuracion(&conexion_plani, &conexion_coordi);

	int socket_Planificador = connect_to_server(conexion_plani.ip, conexion_plani.puerto, logger_esi);
	handshake(socket_Planificador, logger_esi);
	int socket_Coordinador = connect_to_server(conexion_coordi.ip, conexion_coordi.puerto, logger_esi);
	handshake(socket_Coordinador, logger_esi);

	int codigo_plani, codigo_coordi, exito;
	resultados_esi resultado;

	FILE* ESI_1 = fopen("../Prubeas-ESI/ESI_1.txt", "r");

	while(1){
		recibir(&socket_Planificador, &codigo_plani, sizeof(int), logger_esi);
		switch(codigo_plani){
			case 61:
				ejecutar_instruccion(ESI_1, socket_Coordinador, logger_esi);
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
				break;
		}
	}
}
