#include "esi.h"
#include "esi_funciones.h"

int main(){

	int idEsi;

	char* puerto_escucha;
	/*int cantidad_entradas;
	int tamano_entrada;
	int retardo;*/
	FILE* configuracion;
	char* ip;
	char* puerto;
	t_conexion conexion;



	int main(){
		t_log* log_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
		int socket_Planificador = connect_to_server("IP_PLANIFICADOR", "PUERTO_PLANIFICADOR", log_esi);
		char* line;
		Mensaje mensaje;
		mensaje.instruccion = parse(line); //Parsea y devuelve instrucción de ESI

		while(1){
				int mensaje = wait_content(socket_Planificador); //el planificador devuelve la proxima linea a ejecutar
					switch (mensaje){
					case ("PUERTO_PLANIFICADOR"): //chequeo que el puerto sea por el que me comunico con el Plani
						struct InstruccionESI accionESI = listen();
						switch (accionESI->intruccion){
						case ("EJECUTAR"): //deberian ser consts definidas en el protocolo.
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
