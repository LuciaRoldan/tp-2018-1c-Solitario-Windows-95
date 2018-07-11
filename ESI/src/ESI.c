#include "funciones_ESI.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <parsi/parser.h>

int main(int argc, char* argv[]){

// PRUEBA DE SERIALIZACION
	/*char** el_raw = "raw?";
	t_esi_operacion instruccion1 = {1, SET, "hola", "chau", el_raw};*/
	FILE* archivo = fopen(argv[1], "r");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	printf("El argv[0] es: %s \n El argv[1] es: %s \n El argv[2] es: %s \n", argv[0], argv[1], argv[2]);
	if (archivo == NULL){
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, archivo)) != -1) {
		printf("La linea leida es: %s \n", line);
		t_esi_operacion parsed = parse(line);

		if(parsed.valido){
			switch(parsed.keyword){
				case GET:
					printf("GET\tclave: <%s>\n", parsed.argumentos.GET.clave);
					break;
				case SET:
					printf("SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
					break;
				case STORE:
					printf("STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
					break;
				default:
					fprintf(stderr, "No pude interpretar <%s>\n", line);
					exit(EXIT_FAILURE);
			}

			destruir_operacion(parsed);
		} else {
			fprintf(stderr, "La linea <%s> no es valida\n", line);
			exit(EXIT_FAILURE);
		}
	}

	fclose(archivo);
	if (line)
		free(line);

	/*t_esi_operacion instruccion = parse(line); //Parsea y devuelve instrucción de ESI
	free(line);
	int tamanio_instruc = tamanio_instruccion2(instruccion);
	int tamanio_buffer = tamanio_instruc + sizeof(int);
	void* buffer = malloc(tamanio_buffer);
	serializar_instruccion2(buffer, instruccion);
	int protocolo;
	memcpy(&protocolo, buffer, sizeof(int));
	printf("protocolo: %d \n", protocolo);
	memcpy(&tamanio_buffer, buffer+ sizeof(int), sizeof(int));
	printf("tamanio_buffer: %d \n", tamanio_buffer);
	void* buffer_recibido = malloc(tamanio_buffer);
	memcpy(buffer_recibido, buffer+ sizeof(int)*2, tamanio_buffer);
	t_esi_operacion recibido = deserializar_instruccion2(buffer_recibido);
	printf("Recibido: %s, %s, %d \n", recibido.argumentos.SET.clave, recibido.argumentos.SET.valor, recibido.keyword);



// CODIGO ESI

	/*idEsi = atoi(argv[2]);
	sockets_conexiones conexiones;

	logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger_esi,"Inicio de ESI %d. Logger creado.", idEsi);

	conexiones = leer_arch_configuracion();
	log_info(logger_esi, "Archivo de configuracion leido para ESI %d.", idEsi);

	handshake(conexiones.socket_coordi);
	handshake(conexiones.socket_plani);

	int codigo_plani, codigo_coordi;
	int abortoESI = 0;
	void* mensaje_coordi = malloc(sizeof(resultado_esi));

	FILE* script_prueba = fopen(argv[1], "r");

	while(!feof(script_prueba) || abortoESI == 0) {
		codigo_plani = recibir_int(conexiones.socket_plani, logger_esi);
		switch(codigo_plani){
			case 45: //desbloqueo ESI
				enviar_ultima_instruccion(conexiones.socket_coordi);
				codigo_coordi = recibir_int(conexiones.socket_coordi, logger_esi);
				if(cumple_protocolo(codigo_coordi, 20)){
					recibir(conexiones.socket_coordi, mensaje_coordi, sizeof(resultado_esi), logger_esi);
					informar_confirmacion(mensaje_coordi, conexiones.socket_plani, logger_esi);
				} else{
					_exit_with_error(conexiones.socket_coordi, "Error en el intercambio de mensajes", mensaje_coordi, logger_esi);
				}
				break;
			case 61:
				ejecutar_instruccion(script_prueba, conexiones.socket_coordi);
				log_info(logger_esi, "Instruccion enviada a COORDINADOR desde ESI %d", idEsi);
				codigo_coordi = recibir_int(conexiones.socket_coordi, logger_esi);
				if(cumple_protocolo(codigo_coordi, 20)){
					recibir(conexiones.socket_coordi, mensaje_coordi, sizeof(resultado_esi), logger_esi);
					informar_confirmacion(mensaje_coordi, conexiones.socket_plani, logger_esi);
				} else{
					_exit_with_error(conexiones.socket_coordi, "Error en el intercambio de mensajes", mensaje_coordi, logger_esi);
				}
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
	informar_fin_de_programa(conexiones);
	free(mensaje_coordi);
	close(conexiones.socket_plani);
	close(conexiones.socket_coordi);
	log_info(logger_esi, "Fin de ejecucion de ESI %d\n", idEsi);*/
	exit(1);
}
