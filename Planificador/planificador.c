//atributos? variables?

int puertoEscuchas;
//el algoritmo de planificacion con el que querramos que ejecute
algoritmoPlanificacion; //SJF CD/SD o HRRN
int estimacionInicial;
char* ipCoordinador[10]; //es una cadena de caracteres, es char*??????
int puertoCoordinador;
listaDeClaves clavesInicialmenteBloqueadas; //es una lista.

//si alguno sabe como hacer colas y tiene ganas de hacer colas haga colas
colaDeReadyEsis
colaDeEjecucionEsis
colaDeBloqueadoEsis
colaDeFinalizadoEsis


//estados en lo que puede estar el planificador
bool pausado;



void pausarPlanificador(){
	pausado = true;
} //para que un usuario bloquee al planificador y no de mas ordenes a esis ni nada

void bloquearEsi(int clave, int id){ //No entiendo bien como se bloquea un ESI.
	//con clave accedo a la cola del recurso, con el id accedo al esi
	esiBloquearse();
	moverEsiABloqueado(id); //mueve al ESI a la cola de bloqueado del planificador
}

void desbloquearEsi(clave){
	esiDesbloquearse(clave);
	moverEsiAReady(clave);

}

listaRecursos listar(recursos){ //lista procesos encolados en ese recurso
}

void kill(id){
	moverEsiAFinalizado(clave);
	//liberar recursos que tenia en la instancia calculo
}

int? status(clave){
	//devuelve informacion de la instancia que se consulta
}

int* deadlock(){ //va a devolver los esis con deadlocks

	matarEsis(); //consulta al usuario que ESIs quiere matar y los mata
	}
}


void matarEsis(){
	//decime que esis
	for(){ //mata los esis uno por uno
	kill(esi); }
}

void moverEsiABloqueado(clave){}
void moverEsiAReady(clave){}
void moverEsiAFinalizado(clave){}

void asignarEsi(esi);
	//

void ejecutarEsi(esi){
	solicitarEjecucion(esi); //le va a mandar al ESI un mensaje diciéndole que quiere
							//que ejecute
	//send(script); /mandarle el script al esi ??
}
