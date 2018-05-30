#include "esi_funciones.h"

	int idEsi;
	/*int cantidad_entradas;
	int tamano_entrada;
	int retardo;*/
	FILE* arch_configuracion;
	t_conexion conexion_plani, conexion_coordi;
	t_log * logger_esi;


	int main(){
		logger_esi = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
		info_arch_config arch_configuracion = leer_arch_configuracion(arch_configuracion, &conexion_plani, &conexion_coordi);


		int socket_Planificador = connect_to_server(conexion_plani.ip, conexion_plani.puerto, logger_esi);
		int socket_Coordinador = connect_to_server(conexion_coordi.ip, conexion_coordi.puerto, logger_esi);

		int codigo_plani, exito;

		char* line;
		t_esi_operacion mensaje = parse(line); //Parsea y devuelve instrucción de ESI

		while(1){
				recibir(&socket_Planificador, &codigo_plani, sizeof(int), logger_esi);
				switch(codigo_plani){
					case 43:
						exito = buscar_instruccion(socket_Planificador, logger_esi);
						if(!exito){
							enviar(&socket_Planificador, 60, sizeof(int), logger_esi);
						}
						break;
					case 40:
						recibir(&socket_Planificador, &mensaje, sizeof(mensaje), logger_esi);
						exito = enviar_instruccion_a_ejecutar(socket_Coordinador, &mensaje, logger_esi);
						if(!exito){
							enviar(&socket_Planificador, 60, sizeof(int), logger_esi);
						}
						break;
					default:
						break;
				}


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
