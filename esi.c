int iniciarEsi(){ //se conecta con el planificador y el coordinador y si puede
	//establecer conexion con ambos retorna 0.
	conectarEsiConPlanificador();
	conectarEsiConCoordinador();
	return 0;
}}
void esperarSolicitudEjecucion(){
	listen(planificador);
	//recibir el script del planificador
	ejecutarPrograma(script);
}
void ejecutarPrograma(script){ //ejecuta el script del programa cuando el planificador
	//se lo indica, pasándole el programa como parámetro
	for() {//ejecuta sentencias una por una y se las pasa al coordinador. Ejec atomica.
		clave = parsearSentencia(sentencia); //obtiene la clave que le tiene que pedir al coordinador
		resultado = pasarSentenciaCoordinador(clave); //el coordinador le retorna
		//el resultado de la ejecucion de la sentencia
		informarResultadoAlPlanificador(resultado);
	}
}

void bloquearEsi(clave, id){
	//termina de ejecutar la linea que esatba ejecutando
	guardarCotexto(clave, id);


}

int parsearSentencia(sentencia){ //traduce cadena de caracteres en una operacion
	//entendible por el sistema de RD. Usamos el parser que nos dan.
	sentenciaParseada = parser(sentencia);
	return sentenciaParseada;
}

int pasarSentenciaCoordinador(clave){
	respuesta = atender_conexion_esi(clave); //si tuvo exito o no
	return respuesta;
}
