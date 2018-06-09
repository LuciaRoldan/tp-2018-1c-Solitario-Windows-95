#include "funciones_plani.h"

/* COSAS POR HACER
 *
 *
 * 1) VER ṔORQUE ROMPE EL PTHREAD
 *
 */

int main() {
	int* socket_coordinador;
	int* socket_esis;
	logger = log_create("planificador.log", "PLANIFICADOR", 1, LOG_LEVEL_INFO);
	inicializar_planificador(&socket_coordinador, &socket_esis, logger); //leyendo archivo configuracion

	pthread_t hilo_escucha_esis;
	pthread_t hilo_coordinador;
	pthread_t hilo_consola;


	pthread_create(&hilo_escucha_esis, 0, recibir_esis, (void*) &socket_esis);
	pthread_create(&hilo_coordinador, 0, manejar_coordinador, (void*) &socket_coordinador);
	pthread_create(&hilo_consola, 0, manejar_consola, (void*) 0);

	return 0;
}







/*	struct mensaje mensaje = listen(); //o sea, espera a que le llegue CUALQUIER cosa
	switch (puertoDeLlegada){
		case (PUERTO_ESI):
			switch (idEsi()){ //no se como obtener el id del esi que me llega. AYUDA
			case (!esi_existente()):
					agregar_a_cola_de_ready(idEsi()); //como puedo saber el id del Esi? fork?
											//idEsi NO va a ser una func, pero no se de dd sale
			break;
			case(esi_existente()):
					recibir_exito_o_error(mensaje);
					ejecutar_proximo_esi();
			break;
		case (PUERTO_COORDINADOR):
			switch (mensaje->clave){ //o sea me fijo en el header si la clave por la que
			//me estan preguntando esta tomada o no
			case (clave_tomada(mensaje->clave)): //aca ve que la clave que me pedian esta tomada. Es una funcion
					//que retorna true o false
					mover_esi_a_bloqueado(idEsi);
					asignar_esi_a_clave(mensaje->clave, idEsi);
					return -1;//le dice al coordinador que no le puede asignar la clave a un Esi.
			break;
			case (!clave_tomada(mensaje->clave)): //la clave no esta tomada por ningun Esi
					asignar_esi_a_clave(mensaje->clave, idEsi);
					return 1; //exito
			break;
			}
			}
	}
	}

*/




/*
   int ID;
    int arrival_time;
    int time_to_completion;
    int wait_time;
    int turn_around;
    int active;

void sjf (struct process array[MAX_PROCESS], int num_pid){
    printf("Shortest Job First\n");//for the output so we know what algorithm
    //create an array of pids that are valid to search.
    int num_valid_processes = 0, current_time=0, i,j, next_process, counter = 0;//declarations
    process to_sort[MAX_PROCESS];

    //we want to do this next loop for as many processes as we have, or num_pid
    for(j=0; j<num_pid; j++){
        //adds all the available processes to the to sort array to be sorted
        //available means that it has arrived, which means it is <= current_time
        //after it gets all the processes, it breaks out of the for loop
        for(i=counter; i<num_pid; i++){
            if(array[i].arrival_time<=current_time){
                to_sort[i]=array[i];
                num_valid_processes++;
                counter++;
            }
            else
                break;
        }
        //sort the to_sort array by the time_to_completion
        sort_by_time(to_sort,num_valid_processes);

        //set the wait time and turnaround time for the next process
        next_process = to_sort[0].ID;
        array[next_process].wait_time = current_time-array[next_process].arrival_time;
        array[next_process].turn_around = array[next_process].wait_time + array[next_process].time_to_completion;
        //change the current_time and continue
        //current time = current time + wait time + time to completion
        current_time= current_time + array[next_process].time_to_completion;

        //delete the process we just worked on so we don't get duplicates.
        num_valid_processes--;
        for(i=0;i<num_valid_processes;i++){
            to_sort[i]=to_sort[i+1];
        }
    }
    //loop back up to get available processes
    //now all the info in out first array is filled out, print it out.
    print_info(array, num_pid);
}

*/
