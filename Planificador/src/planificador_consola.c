#include "planificador_punto_hache.h"

// CONSOLA
void ejecutar_consola(){
	char * linea;
	char * clave;
	char* recurso;
	int el_id;

	while(1) {
		linea = readline("\n > Ingrese un comando: \n");
		if(!string_is_empty(linea)){
			add_history(linea);
			printf("*** La linea ingresada fue: %s\n", linea);
			string_to_lower(linea);

			if(!strncmp(linea, "exit", 4)) {
				printf("*** Cerrando Consola. Hasta luego. \n");
				free(linea);
				pausar_planificacion();
				usleep(100000);
				desencadenar_cerrar_planificador();
				exit(1);
				break;
			}

			op_consola operacion = analizar_linea(linea);
			char **palabras = string_to_array(linea, " ");

			switch(operacion){
			case BLOQUEAR:
				if(palabras[1] != NULL && palabras[2] != NULL){
					if(palabras[3]==NULL){
						printf("Usted quiere bloquear al ESI de ID <%s> para la clave <%s>.\n",palabras[2], palabras[1]);
						clave = malloc(strlen(palabras[1]));
						strcpy(clave, palabras[1]+1);
						el_id = atoi(palabras[2]);
						bloquear(clave, el_id);
					} else {
						printf("Demasiados argumentos para la operacion 'bloquear <clave> <id>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave> <id>\n");
				}
				break;
			case DESBLOQUEAR:
				if(palabras[1] != NULL){
					if(palabras[2]==NULL){
					printf("Usted quiere desbloquear la clave <%s>.\n", palabras[1]);
					clave = malloc(strlen(palabras[1])+1);
					strcpy(clave, palabras[1]);
					desbloquear(clave);
					} else {
						printf("Demasiados argumentos para la operacion 'desbloquear <clave>' \n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave> \n");
				}
				break;
			case KILL:
				if(palabras[1] != NULL){
					if(palabras[2]==NULL){
						printf("Usted quiere finalizar el proceso %s.\n", palabras[1]);
						el_id = atoi(palabras[1]);
						kill_esi(el_id);
					} else {
						printf("Demasiados argumentos para la operacion 'kill <id>' \n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <id>\n");
				}
				break;
			case PAUSAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere pausar la planificacion.\n");
					pausar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de pausar.\n");
				}
				break;
			case CONTINUAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere continuar la planificacion.\n");
					continuar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de continuar.\n");
				}
				break;
			case LISTAR:
				if(palabras[1] != NULL) {
					if(palabras[2]==NULL){
						printf("Usted quiere listar los procesos en cola de espera para el recurso <%s>.\n", palabras[1]);
						recurso = palabras[1];
						listar_procesos_encolados(recurso);
					} else {
						printf("Demasiados argumentos para la operacion de 'listar <recurso>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <recurso> \n");
				}
				break;
			case STATUS:
				if(palabras[1] != NULL) {
					if(palabras[2]==NULL){
						printf("Usted quiere ver el estado de la clave <%s>.\n", palabras[1]);
						clave = palabras[1];
						pedir_status(clave);
					} else {
						printf("Demasiados argumentos para la operacion de 'status <clave>'.\n");
					}
				} else {
					printf("Faltan argumentos para la operacion: <clave>\n");
				}
				break;
			case DEADLOCK:
				if(palabras[1] == NULL) {
					printf("Usted quiere analizar deadlocks.\n");
					deadlock();
				} else {
					printf("Demasiados argumentos para la operacion de deadlock. \n");
				}
				break;
			case PLANIFICAR:
				if(palabras[1] == NULL) {
					printf("Planificar.\n");
					post_a_planificar();
				} else {
					printf("Demasiados argumentos para la operacion planificar. \n");
				}
				break;
		   default:
			   printf("Comando no reconocido. Ingrese nuevamente. \n");
			   break;
			}
		}
		if(se_cerro_todo == 1){
			break;
		}
  }
  free(linea);
  exit(1);
}

op_consola analizar_linea(char* linea){

	if(string_starts_with(linea, "bloquear")) {
		return BLOQUEAR;
	}
	if(string_starts_with(linea, "desbloquear")) {
		return DESBLOQUEAR;
	}
	if(string_starts_with(linea, "kill")){
		return KILL;
	}
	if(string_starts_with(linea, "pausar")){
		return PAUSAR;
	}
	if(string_starts_with(linea, "continuar")){
		return CONTINUAR;
	}
	if(string_starts_with(linea, "listar")){
		return LISTAR;
	}
	if(string_starts_with(linea, "status")){
		return STATUS;
	}
	if(string_starts_with(linea, "deadlock")){
		return DEADLOCK;
	}
	if(string_starts_with(linea, "planificar")){
		return PLANIFICAR;
	}
	return INVALIDO;
}

char**  string_to_array(char* text, char* separator) {
    char **array_values = string_split(text, separator);
    int i = 0;
    while (array_values[i] != NULL) {
	    string_trim(&(array_values[i]));
	    i++;
    }
    return array_values;
}
