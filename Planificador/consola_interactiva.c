#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

void main() {
  char * linea;
  while(1) {
    linea = readline(">");
    if(linea)
      add_history(linea);
    if(!strncmp(linea, "exit", 4)) {
       free(linea);
       break;
    }
    printf("%s\n", linea);
    if(!strncmp(linea, "bloquear", 8)) {
    	printf("Usted quiere ploquear una instancia.\n");
    }
    else {
    	if(!strncmp(linea, "desbloquear", 11)) {
    	printf("Usted quiere desbloquear una instancia.\n");
    	}
    	else {
    		if(!strncmp(linea, "kill", 4)){
    			printf("Usted quiere finalizar un proceso.\n");
    		}
    		else {
    			if(!strncmp(linea, "pausar", 6)){
    				printf("Usted quiere pausar un proceso.\n");
    			}
    			else{
    				if(!strncmp(linea, "continuar", 9)){
    					printf("Usted quiere continuar un proceso.\n");
    				}
    				else{
    					if(!strncmp(linea, "listar", 6)){
    						printf("Usted quiere listar los procesos en cola de espera para un recurso.\n");
    					}
    					else{
    						if(!strncmp(linea, "status", 6)){
    							printf("Usted quiere ver el estado de un proceso.\n");
    						}
    						else{
    							if(!strncmp(linea, "deadlock", 8)){
    								printf("Usted quiere analizar deadlocks.\n");
    							}
    							else{
    								printf("Comando no reconocido. Ingrese nuevamente. \n");
    							}
    						}
    					}
    				}
    			}

    		}
    	}

    }
    free(linea);
  }
}

