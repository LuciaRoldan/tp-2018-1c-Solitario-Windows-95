#include "planificador.h"
#include "planificador.c"

void inicializar_planificador(int* socketCoordinador, int* socketEsis){ //como hago para decir que recibo puntero?
	leer_archivo_configuracion();
	socketCoordinador = conectarse_al_coordinador();
	socketEsis = inicializar_servidor(conexion.ip, conexion.puerto, logger);
}

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: puertoEscucha, algoritmoPlanificacion, estimacionInicial, ipCoordinador, puertoCoordinador y clavesInicialmenteBloqueadas
	configuracion = fopen("archivo_configuracion_planificador.txt", "r");
	fscanf(configuracion, "%s %d %d %s %s %s", &puertoEscucha, &algoritmoPlanificacion, &estimacionInicial , &ipCoordinador, &puertoCoordinador, &clavesInicialmenteBloqueadas);
	fclose(configuracion);
}

int conectarse_al_coordinador(){
	int socketCoordinador;
	t_config* configuracion=config_create("/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Config");
	strcpy(conexion.ip,config_get_string_value(configuracion,"IP_COORDINADOR"));
	strcpy(conexion.puerto,config_get_string_value(configuracion,"PUERTO_COORDINADOR"));
	socketCoordinador = connect_to_server(conexion.ip, conexion.puerto, logger);
	return socketCoordinador;
}

void manejar_esi(){
	pcb *mensaje = malloc(sizeof(pcb)); //no entiendo. Problema con si es un puntero a void
	//y como lo paso a un struct pcb. Puedo guardar un puntero a void en un puntero a pcb ???
	mensaje = wait_content(atoi(*puertoEscucha)); //espera a que le llegue algo del puerto por el
												  //que le hablan los Esis
												  //el ESI le manda la PCB
	switch (mensaje->id){ //no se como obtener el id del esi que me llega. AYUDA
				case (!esi_existente()):
						pthread_t hilo_ESI; //creo hilo para el nuevo ESI
						agregar_a_cola_de_ready(mensaje->id); //saco el ID de la PCB
						pthread_create(&hilo_ESI, NULL, planificar_ESI(), NULL);
				break;
				case(esi_existente()):
						recibir_exito_o_error(mensaje);
						ejecutar_proximo_esi();
				break;
}
