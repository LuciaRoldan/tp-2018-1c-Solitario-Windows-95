#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char* argv[]){

	char **array_values = string_split(argv[1], "/");
	int i = 0;
	while (array_values[i] != NULL) {
		string_trim(&(array_values[i]));
		i++;
	}
	char* log = array_values[2];

	idEsi = atoi(argv[2]);
	sockets_conexiones conexiones;

	logger_esi = log_create("ESI/esi.log", log, true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Inicio de ESI %d ---> Logger creado.", idEsi);

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
	int flag_exit = 0;
	void* mensaje_coordi = malloc(sizeof(int));
	resultado_esi confirmacion;

	while((!feof(script_prueba)) && abortoESI == 0 && flag_exit == 0) {
		codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
		log_info(logger_esi, "Plani me dijo: %d", codigo_plani);
		//sleep(1);
		switch(codigo_plani){
			case 60: //desbloqueo ESI
				ejecutar_ultima_instruccion(conexiones.socket_coordi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR desde ESI %d", idEsi);
				confirmacion = recibir_int(conexiones.socket_coordi, logger_esi);
				log_info(logger_esi, "Recibi del coordinador: %d", confirmacion);
				abortoESI = informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);
				if(abortoESI) informar_fin_de_programa(conexiones, abortoESI);
				break;
			case 61: //solicitud de ejecucion
				if(ejecutar_instruccion_sgte(script_prueba, conexiones.socket_coordi) > 0){
					confirmacion = recibir_int(conexiones.socket_coordi, logger_esi);
					log_info(logger_esi, "Recibi del coordinador: %d", confirmacion);
					abortoESI = informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);
					/*if((confirmacion > 84) && (confirmacion < 90)){
						abortoESI = 1;
						informar_fin_de_programa(conexiones, abortoESI);
					} else informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);*/
					if(abortoESI) informar_fin_de_programa(conexiones, abortoESI);
				} else {
					log_info(logger_esi, "La clave no paso la prueba");
					error_clave_larga(conexiones);
					flag_exit = 1;
				}
				break;
			case 62: //abortar ESI
				abortoESI = 1;
				log_info(logger_esi, "Abortando ESI %d", idEsi);
				informar_fin_de_programa(conexiones, abortoESI);
				break;
			case 85: //exit por consola
				log_info(logger_esi, "Cerrando ESI %d por 'exit' de consola", idEsi);
				void* buffer_exit = malloc(sizeof(int));
				serializar_id(buffer_exit, 20);
				enviar(conexiones.socket_coordi, buffer_exit, sizeof(int), logger_esi);
				flag_exit = enviar_exit_coordi(conexiones.socket_coordi);
				break;
			default:
				log_info(logger_esi, "Mensaje fuera de protocolo: %d", codigo_plani);
				abortoESI = 1;
				informar_fin_de_programa(conexiones, abortoESI);
				break;
		}
		//sleep(2);
	}
	if(feof(script_prueba)) informar_fin_de_programa(conexiones, abortoESI);
	fclose(script_prueba);
	free(mensaje_coordi);
	free(configuracion_esi);
	log_info(logger_esi, "Holi");
	close(conexiones.socket_plani);
	close(conexiones.socket_coordi);
	log_info(logger_esi, "Fin de ejecucion de ESI %d\n", idEsi);
	log_info(logger_esi, "Termine wacho :7");
	free(logger_esi);
	return 0;
}
