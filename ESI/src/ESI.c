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

	int codigo_plani, codigo_coordi, resultado;
	int abortoESI = 0;
	int flag_exit = 0;
	me_bloquearon = 0;
	ultimo_read = 0;
	void* mensaje_coordi = malloc(sizeof(int));
	resultado_esi confirmacion;

	while((!feof(script_prueba)) && abortoESI == 0 && flag_exit == 0) {
		codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
		log_info(logger_esi, "Plani me dijo: %d", codigo_plani);
		//sleep(1);
		switch(codigo_plani){
			case 61: //solicitud de ejecucion
				resultado = ejecutar_instruccion_sgte(script_prueba, conexiones.socket_coordi);
				if(resultado > 0){
					confirmacion = recibir_int(conexiones.socket_coordi, logger_esi);
					log_info(logger_esi, "Recibi del coordinador: %d", confirmacion);
					abortoESI = informar_confirmacion(confirmacion, conexiones.socket_plani, logger_esi);
					if(abortoESI) informar_fin_de_programa(conexiones, abortoESI);
				} else {
					if(resultado < 0){
						log_info(logger_esi, "La clave es demasiado larga.");
						error_clave_larga(conexiones);
					} else{
						log_info(logger_esi, "No se pudo enviar la instruccion al Coordinador \n");
					}
					abortoESI = 1;
				}
				break;
			case 62: //abortar ESI
				abortoESI = 1;
				log_info(logger_esi, "Abortando ESI %d", idEsi);
				//informar_fin_de_programa(conexiones, abortoESI);
				break;
			case 85: //exit por consola
				log_info(logger_esi, "Cerrando ESI %d por 'exit' de consola", idEsi);
				void* buffer_exit = malloc(sizeof(int));
				serializar_id(buffer_exit, 20);
				enviar(conexiones.socket_plani, buffer_exit, sizeof(int), logger_esi);
				enviar(conexiones.socket_coordi, buffer_exit, sizeof(int), logger_esi);
				flag_exit = enviar_exit_coordi(conexiones.socket_coordi);
				break;
			case 91: //kill de consola
				codigo_coordi = recibir_int(conexiones.socket_coordi, logger_esi);
				if(codigo_coordi == 91){
					log_info(logger_esi, "ESI %d abortado por 'kill' de consola.", idEsi);
				} else{
					_exit_with_error(conexiones.socket_coordi, "Error al recibir mensaje del Coordinador", NULL, logger_esi);
				}
				/*void* buffer_int = malloc(sizeof(int));
				serializar_id(buffer_int, 81);
				enviar(conexiones.socket_coordi, buffer_int, sizeof(int), logger_esi);
				free(buffer_int);*/
				abortoESI = 1;
				break;
			default:
				log_info(logger_esi, "Mensaje fuera de protocolo: %d", codigo_plani);
				abortoESI = 1;
				informar_fin_de_programa(conexiones, abortoESI);
				break;
		}
		
	}
	if(feof(script_prueba)) informar_fin_de_programa(conexiones, abortoESI);
	sleep(0.2);
	fclose(script_prueba);
	free(mensaje_coordi);
	free(configuracion_esi);
	close(conexiones.socket_plani);
	close(conexiones.socket_coordi);
	log_info(logger_esi, "Fin de ejecucion de ESI %d\n", idEsi);
	log_destroy(logger_esi);

	return 0;
}
