#include "esi.h"

typedef enum {
	NUEVO,
	LISTO,
	BLOQUEANDO,
	EJECUTANDO
} estado;

typedef struct {
	estado estado;
	int retardo; //tiempo en ser atendido el esi la ultima vez que ejecuto
	int ultimaRafaga; //con el retardo y la ultimaRafaga se calcula la estimacion ?

} pcb;

int main(){

	int idEsi;

	iniciarEsi(); //conectarse con el coordinador y el planificador

	while(1){
		int mensaje = listen(); //escuchar los mensajes. El Panificador le pasa Ints para decirle
		//que ejecute, se bloquee o se desbloquee.
			switch (puertoLlegada()){
			case ("PUERTOPLANIFICADOR"): //chequeo que el puerto sea por el que me comunico con el Plani
				struct InstruccionESI accionESI = listen();
				switch (accionESI->intruccion){
				case ("EJECUTAR"): //deberian ser consts definidas en el protocoloo.
					traducirYEjecutar();
				break;
				case ("BLOQUEARSE"):
					bloquearse_esi();
				break;
				case ("DESBLOQUEARSE"):
				break;
				}
			}
			case ("PUERTOCOORDINADOR"):


	}

/*int traducirYEjecutar(){
	//variable instruccion
	int resultadoEjecucion;
	while(readln(script)){ //mientras haya cosas para leer en el script
		linea = readln();
		solicitud = parser(linea); //parsear la linea
		resultadoEjecucion = enviarSolicitud(solicitud);
		actualizarPCB(resultadoEjecucion);
		informarPlanificador(PCB); //le cuenta al planificador si pudo
												//ejecutar o no

	}
}*/

int iniciarEsi(){ //se conecta con el planificador y el coordinador y si puede
	//establecer conexion con ambos retorna 0.
	conectarEsiConPlanificador(); //implementar el establecer conexión
	conectarEsiConCoordinador(); //idem
	return 0;
}
/*void esperarSolicitudEjecucion(){
	listen(planificador);
	//recibir el script del planificador
	ejecutarPrograma(script);
}*/
//int enviarSolicitud(solicitud){
//	int resultadoEjecucion;
//	//mandar la instruccion al Coordinador
//	resultado = //recibir
//	return resultadoEjecucion;


void bloquearEsi(clave, id){
	//termina de ejecutar la linea que esatba ejecutando
	guardarCotexto(clave, id);


}

/*int parsearSentencia(sentencia){ //traduce cadena de caracteres en una operacion
	//entendible por el sistema de RD. Usamos el parser que nos dan.
	sentenciaParseada = parser(sentencia);
	return sentenciaParseada;
}*/

/*int pasarSentenciaCoordinador(clave){
	respuesta = atender_conexion_esi(clave); //si tuvo exito o no
	return respuesta;
}*/
