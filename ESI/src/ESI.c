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

	/*printf("El argv[0] es: %s\n", argv[0]);
	printf("El argv[1] es: %s\n", argv[1]);
	printf("El argv[2] es: %s\n", argv[2]);*/

	//char * path = string_from_format("%s", argv[0]);
	FILE* script_prueba = fopen(argv[1], "r");
	if (script_prueba == NULL){
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	} else
		log_info(logger_esi, "Script abierto para ESI %d \n", idEsi);

	fseek(script_prueba, 0, SEEK_SET);

	conexiones = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido para ESI %d.", idEsi);

	handshake_del_esi(conexiones.socket_coordi);
	handshake_del_esi(conexiones.socket_plani);

	int codigo_plani;
	int codigo_coordi;
	int abortoESI = 0;
	void* mensaje_coordi = malloc(sizeof(int));
	resultado_esi confirmacion;

	while((!feof(script_prueba)) && abortoESI == 0) {
		codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
		log_info(logger_esi, "Plani me dijo: %d", codigo_plani);
		sleep(2);
		switch(codigo_plani){
			case 60: //desbloqueo ESI
				ejecutar_ultima_instruccion(conexiones.socket_coordi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR desde ESI %d", idEsi);
				confirmacion = recibir_int(conexiones.socket_coordi, logger_esi);
				log_info(logger_esi, "Recibi del coordinador: %d", confirmacion);
				informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);
				break;
			case 61: //solicitud de ejecucion
				ejecutar_instruccion_sgte(script_prueba, conexiones.socket_coordi);
				confirmacion = recibir_int(conexiones.socket_coordi, logger_esi);
				log_info(logger_esi, "Recibi del coordinador: %d", confirmacion);
				if((confirmacion > 84) && (confirmacion < 90)){
					abortoESI = 1;
				} else informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);
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
	codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
	informar_fin_de_programa(conexiones, abortoESI);
	free(mensaje_coordi);
	close(conexiones.socket_plani);
	close(conexiones.socket_coordi);
	log_info(logger_esi, "Fin de ejecucion de ESI %d\n", idEsi);
	exit(1);
}
