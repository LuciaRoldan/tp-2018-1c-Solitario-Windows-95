#include "planificador_funciones.h"

char* puertoEscucha;
AlgoritmoPlanificacion algoritmoPlanificacion;
int estimacionInicial;
char* ipCoordinador[10]; //es una cadena de caracteres, es char*??????
char* puertoCoordinador;
char* clavesInicialmenteBloqueadas; //es una lista //va a haber que parsearlo. paja.
FILE* configuracion;
t_conexion conexion;
logger = log_create("planificador.log", "PLANIFICADOR", TRUE, LOG_LEVEL_INFO);

struct ColaDeEsi *colaDeReadyEsis;
struct ColaDeEsi *colaDeBloqueadoEsis;
struct ColaDeEsi *colaDeFinalizadoEsis;

struct ClavesBloqueadas *clavesBloqueadas;

//estados en lo que puede estar el planificador
//pausado;

//void agregarEsiAColaDeReady(...){}

int main() {
	int socketCoordinador;
	int socketEsis;
	logger = log_create("planificador.log", "PLANIFICADOR", true, LOG_LEVEL_INFO);
	inicializar_planificador(*socketCoordinador, *socketEsis); //leyendo archivo configuracion

	pthread_t hiloEscuchaEsis;
	pthread_t hiloCoordinador;
	pthread_t hiloConsola;


	pthread_create(&hiloEscuchaEsis, NULL, recibir_esis, (void*) socketEsis);
	pthread_create(&hiloCoordinador, NULL, manejar_coordinador, ((void*) &socketCoordinador));

	return 0;
}

void recibir_esis(int socketEsis){
	int conexionEsi;
	while(conexionEsi = aceptar_conexion(socketEsis)){ //es como una funcion con un accept basicamente
													//que devuelve lo que me devuelve el accept. Now
													//en las commons!

		log_info(logger, "Conexion aceptada del Esi: "); //imprimir el id del Esi que se me conecto
		pthread_t hiloEscuchaEsi;
		if( pthread_create(&hiloEscuchaEsi, NULL ,  manejar_esi, (void*) &conexionEsi) < 0){
			perror("No se pudo crear el hilo");
		}
		    //Now join the thread , so that we dont terminate before the thread
		pthread_join(hiloEscuchaEsi , NULL);
		log_info(logger, "Esi asignado");
	}

		if (conexionEsi < 0){
	        perror("accept failed");
		}
}



	//pthread_create(&hiloEscuchaCoordinador, NULL, escucharCoordinador(), NULL);




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

//CONSOLA//
void pausarPlanificador(){
	//syscall bloqueante ?????
}


//para que un usuario bloquee al planificador y no de mas ordenes a esis ni nada
void bloquearEsi(int clave, int idEsi){
	mover_esi_a_bloqueados(colaDeBloqueadoEsis, idEsi, clave); //encola al Esi en la lista de bloqueados con la clave
	//que corresponda
}

void encolar(ColaDeEsi cola, claveYesi){ //hacer el encolar como en Algoritmos.. y claveYesi es como
	//las dos cosas que se van a guardar en un nodo pero no se como seria la estructura
}

void desbloquearEsi(clave){
	mover_esi_a_ready(clave);
}

ColaDeEsi listar(Clave clave){ //recurso == clave?
	return "claveBloqueada.esisEnEspera"; //busca en su lista de Claves bloqueadas, la clave que se le pide,
	//y devuelve la cola de esis bloqueados.
}

void kill(idEsi){
	mover_esi_a_finalizado(idEsi);
	//liberar los recursos. Fijarse que clave estaba ocupando y desocuparla.
}

int status(clave){
	//devuelve informacion de la instancia que se consulta
	//fijarse en la ClavesBloqueadas a ver si esta la clave
	return 1;
}

int* deadlock(){return 0;} //va a devolver los esis con deadlocks //no se jaja

void matarEsis(ColaDeEsi esis){
//	for(){ //mata los esis uno por uno
//	kill(esi); }
}



// OTRAS FUNCIONES ESI-PLANI //

void mover_esi_a_bloqueado(int idEsi){} //hay que hacer estas funciones de encolar
void mover_esi_a_ready(int idEsi){}
void mover_esi_a_finalizado(int idEsi){}

int esiExistente(int idEsi){return 1;} //bueno aca, o me puedo fijar en mis colas si ya tengo al Esi, o puedo
//fijarme que me esta mandando. Si me manda un 1 o un -1 es pq es existente.


int clave_tomada(int clave){
	int tomada = ";";//buscar en la estructura que tenga las claves a ver si esta bloqueada
	return tomada;
}

void asignar_esi_a_clave(Clave clave, int idEsi){
	//buscar en donde sea que tenga las claves con los esis y asignarle el esi.
}

void ejecutar_proximo_esi(){}

void ejecutarEsi(esi){
	solicitarEjecucion(esi); //le va a mandar al ESI un mensaje diciéndole que quiere
							//que ejecute
	//send(script); /mandarle el script al esi ??
}
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
