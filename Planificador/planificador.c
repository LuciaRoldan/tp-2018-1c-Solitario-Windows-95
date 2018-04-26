//Aca hay que hacer lo que hizo Lu R en el coordinador que lee las coasa del archivo y las
//pone en variables
/*int puertoEscuchas;
//el algoritmo de planificacion con el que querramos que ejecute
algoritmoPlanificacion; //SJF CD/SD o HRRN
int estimacionInicial;
char* ipCoordinador[10]; //es una cadena de caracteres, es char*??????
int puertoCoordinador;
listaDeClaves clavesInicialmenteBloqueadas; //es una lista.
*/

typedef struct{
	int IDEsi;
	struct Esi* sgte;
} ColaDeEsi; //esto es una cola o un esi ???


struct ColaDeEsi* colaDeReadyEsis;
struct ColaDeEsi* colaDeBloqueadoEsis;
struct ColaDeEsi* colaDeFinalizadoEsis;

//hay que hacer un struct para que el planificador guarde las claves de las instancias bloqueadas
//y especifique que ESI la bloqueo. O sea, el Esi va a querer buscar las claves, y capaz que va a querer
//saber que ESI esta ocupando la clave. Entonces un struct capaz no es lo mas copado..


//estados en lo que puede estar el planificador
pausado;


void agregarEsiAColaDeReady(...){}

int main() {
	int puertoDeLlegada, PUERTO_ESI, PUERTO_COORDINADOR;
	int pausado = 0;
//	inicializar_servidor(IP_PLANIFICADOR, PUERTO_PLANIFICADOR); // Esto lo saca por arch de config

	while(1){
	struct mensaje mensaje = listen();
	switch (puertoDeLlegada){
		case (PUERTO_ESI):
			switch (mensaje->idEsi){
			case (!esi_existente()):
					agregarAColaDeReady(mensaje->idEsi);
			break;
			case(esi_existente()):
					recibir_exito_o_error();
			break;
		case (PUERTO_COORDINADOR):
			switch (mensaje->clave){ //o sea me fijo en el header si la clave por la que
			//me estan preguntando esta tomada o no
			case (clave_tomada(mensaje->clave)): //aca ve que la clave que me pedian esta tomada. Es una funcion
					//que retorna true o false
					return -1;//le dice al coordinador que no le puede asignar la clave a un Esi.
			break;
			case (!clave_tomada(mensaje->clave)): //la clave no esta tomada por ningun Esi
					asignar_clave_a_esi(mensaje->clave, mensaje->idEsi); //no se donde se guardara la info de los esis ?
					return 1; //exito
			break;
			}
			}
	}
	}

int clave_tomada(int clave){
	int tomada = ";";//buscar en la estructura que tenga las instancias a ver si esta bloqueada
	return tomada;
}

void asignar_clave_a_esi(int clave, int idEsi){
	//buscar en donde sea que tenga las claves con los esis y asignarles el esi.
}

//para que un usuario bloquee al planificador y no de mas ordenes a esis ni nada
}



//CONSOLA//
void pausarPlanificador(){
	//syscall bloqueante ?????
	}


void bloquearEsi(int clave, int idEsi){
	mover_esi_a_bloqueados(colaDeBloqueadoEsis, idEsi, clave); //encola al Esi en la lista de bloqueados con la clave
	//que corresponda
	moverEsiABloqueado(idEsi); //mueve al ESI a la cola de bloqueado del planificador
}

void encolar(ColaDeEsi cola, claveYesi){ //hacer el encolar como en Algoritmos.. y claveYesi es como
	//las dos cosas que se van a guardar en un nodo pero no se como seria la estructura
}





void desbloquearEsi(clave){
	esiDesbloquearse(clave);
	moverEsiAReady(clave);

}

//listaRecursos listar(recursos){ //lista procesos encolados en ese recurso
}

void kill(id){
//	moverEsiAFinalizado(clave);
	//liberar recursos que tenia en la instancia calculo
}

//int status(clave){
	//devuelve informacion de la instancia que se consulta
//}

//int* deadlock(){ //va a devolver los esis con deadlocks

//	matarEsis(); //consulta al usuario que ESIs quiere matar y los mata
//	}
}


void matarEsis(){
	//decime que esis
//	for(){ //mata los esis uno por uno
//	kill(esi); }
}

void moverEsiABloqueado(clave){}
void moverEsiAReady(clave){}
void moverEsiAFinalizado(clave){}

//void asignarEsi(esi);
	//

void ejecutarEsi(esi){
	solicitarEjecucion(esi); //le va a mandar al ESI un mensaje diciéndole que quiere
							//que ejecute
	//send(script); /mandarle el script al esi ??
}
