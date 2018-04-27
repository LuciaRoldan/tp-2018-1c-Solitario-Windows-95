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

typedef struct{ //estructura que va a contener todas las claves y el estado de cada una
	Clave clave; //deberia llegarle de las commons
	int esiQueLaUsa;
	ColaDeEsi esisEnEspera;
} ClaveBloqueada;

typedef struct{
	ClaveBloqueada clave;
	ClaveBloqueada* sgte;
} ClavesBloqueadas;

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
	struct mensaje mensaje = listen(); //o sea, espera a que le llegue CUALQUIER cosa
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

void recibir_exito_o_fallo(int mensaje){ //esto deberia llegar de las commons
	//recibiria un 1 o un -1
}

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

