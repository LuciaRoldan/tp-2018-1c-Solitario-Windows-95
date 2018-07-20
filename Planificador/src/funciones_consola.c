#include "funciones_plani.h"

// CONSOLA
void ejecutar_consola(){
	char * linea;
	char * clave;
	char* recurso;
	int el_id;

	while(1) {
		linea = readline("> Ingrese un comando: ");
		if(linea){
			add_history(linea);
			printf("---> La linea ingresada fue: %s\n", linea);
		}
		string_to_lower(linea);

		if(!strncmp(linea, "exit", 4)) {
			printf("Cerrando Consola. Hasta luego. \n");
			free(linea);
			cerrar_planificador();
			exit(1);
			break;
		}

		op_consola operacion = analizar_linea(linea);
		char **palabras = string_to_array(linea);

			switch(operacion){
			case BLOQUEAR:
				if(palabras[1] != NULL && palabras[2] != NULL){
					printf("Usted quiere bloquear un ESI.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("Clave: %s ---- ", palabras[1]);
					printf("ID: %s ---- ", palabras[2]);
					if(palabras[3]==NULL)  {printf("Bloque vacio\n");}
						else {
							palabras[3] = NULL;
							printf("Bloque vacio\n");
						}
					clave = palabras[1];
					el_id = atoi(palabras[2]);
					bloquear(clave, el_id);
				} else {
					printf("Faltan argumentos para la operacion: <clave> <id>\n");
				}
				break;
			case DESBLOQUEAR:
				if(palabras[1] != NULL){
					printf("Usted quiere desbloquear un ESI de para una clave.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("Clave: %s ---- ", palabras[1]);
					if(palabras[2]==NULL) {printf("Bloque vacio\n");}
						else {
							palabras[2] = NULL;
							printf("Bloque vacio\n");
						}
					clave = palabras[1];
					desbloquear(clave);
				} else {
					printf("Faltan argumentos para la operacion: <clave> \n");
				}
				break;
			case KILL:
				if(palabras[1] != NULL){
					printf("Usted quiere finalizar un proceso.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("ID: %s ---- ", palabras[1]);
					if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[2] = NULL;
						printf("Bloque vacio\n");
					}
					el_id = atoi(palabras[1]);
					kill(el_id);
				} else {
					printf("Faltan argumentos para la operacion: <id>\n");
				}
				break;
			case PAUSAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere pausar la planificacion.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[1] = NULL;
						printf("Bloque vacio\n");
					}
					pausar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de pausar.\n");
				}
				break;
			case CONTINUAR:
				if(palabras[1] == NULL) {
					printf("Usted quiere continuar la planificacion.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[1] = NULL;
						printf("Bloque vacio\n");
					}
					continuar_planificacion();
				} else {
					printf("Demasiados argumentos para la operacion de continuar.\n");
				}
				break;
			case LISTAR:
				if(palabras[1] != NULL) {
					printf("Usted quiere listar los procesos en cola de espera para un recurso.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("Recurso: %s ---- ", palabras[1]);
					if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[2] = NULL;
						printf("Bloque vacio\n");
					}
					recurso = palabras[1];
					listar_procesos_encolados(recurso);
				} else {
					printf("Faltan argumentos para la operacion: <recurso> \n");
				}
				break;
			case STATUS:
				if(palabras[1] != NULL) {
					printf("Usted quiere ver el estado de una clave.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					printf("Clave: %s ---- ", palabras[1]);
					if(palabras[2]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[2] = NULL;
						printf("Bloque vacio\n");
					}
					clave = palabras[1];
					pedir_status(clave);
				} else {
					printf("Faltan argumentos para la operacion: <clave>\n");
				}
				break;
			case DEADLOCK:
				if(palabras[1] == NULL) {
					printf("Usted quiere analizar deadlocks.\n");
					printf("Operacion: %s ---- ", palabras[0]);
					if(palabras[1]==NULL)  {printf("Bloque vacio\n");}
					else {
						palabras[1] = NULL;
						printf("Bloque vacio\n");
					}
					deadlock();
				} else {
					printf("Demasiados argumentos para la operacion de deadlock. \n");
				}
				break;
		   default:
			   printf("Comando no reconocido. Ingrese nuevamente. \n");
			   break;
			}
  }
  free(linea);
  exit(1);
}

op_consola analizar_linea(char* linea){

	if(string_contains(linea, "bloquear")) {
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
	return INVALIDO;
}

char**  string_to_array(char* text) {
    char **array_values = string_split(text, " ");
    int i = 0;
    while (array_values[i] != NULL) {
	    string_trim(&(array_values[i]));
	    i++;
    }
    return array_values;
}

// COMPLETAR SUBRUTINAS QUE VIENEN DE CONSOLA:
void pausar_planificacion(){

}
void continuar_planificacion(){

}
void bloquear(char * clave, int id){

}
void desbloquear(char * clave){

}
void listar_procesos_encolados(char* recurso){

}
void kill(int id){
	id_buscado = id;
	pcb* pcb_esi = (pcb*) list_find(pcbs, ids_iguales_pcb);
	log_info(logger, "ESI %d abortado por funcion 'kill' de consola.", pcb_esi->id);
	abortar_esi(pcb_esi->id);
}
void pedir_status(char* clave){
	int tamanio_buffer = tamanio_buffer_string(clave);
	void* buffer_pedido_clave = malloc(tamanio_buffer);
	serializar_string(buffer_pedido_clave, clave, 21);
	enviar(sockets_planificador.socket_coordinador, buffer_pedido_clave, tamanio_buffer, logger);
	sleep(2);
	//se recibe la respuesta por otro hilo
}
void deadlock(){

}
