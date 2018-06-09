#include "funciones_instancia.h"


/////////////////////// INICIALIZACION ///////////////////////

	datos_configuracion recibir_configuracion(int* socket_coordinador,t_log* logger) {
		datos_configuracion configuracion;
		recibir(socket_coordinador, &configuracion, sizeof(datos_configuracion), logger);
		return configuracion;
		//deserializar?
	}

void leer_configuracion_propia(configuracion_propia* configuracion, t_log* logger) {

	FILE* archivo = fopen("Configuracion_instancia.txt", "r");

	if (archivo < 1) {
		log_info(logger,"No se puede abrir el archivo Configuracion_instancia.txt");
		exit(1);
	}

	fscanf(archivo, "%s %s %s %s %s %d",
			configuracion->ipCoordinador,
			configuracion->puertoCoordinador,
			configuracion->algoritmoDeReemplazo,
			configuracion->puntoDeMontaje,
			configuracion->nombreInstancia,
			&(configuracion->intervaloDump));
	fclose(archivo);
}


//	t_esi_operacion recibir_instruccion(int* socket_coordinador, t_log* logger) {
//		t_esi_operacion instruccion;
//		recibir(socket_coordinador, &instruccion, sizeof(t_esi_operacion), logger);
//		return instruccion;
//		//deserializar?
//	}
//
//	void enviar_a_desbloquear_clave(int* socket_coordinador, int clave, t_log* logger) {
//		enviar(socket_coordinador, clave, sizeof(clave), 03, logger);
//		//serializar?
//	}

	/*void guardar_archivo(char* clave, char* valor) {

		int* direccion = obtener_direccion(clave);
		memcopy(direccion, valor, sizeof(valor));
	}*/

/*	void procesar_instruccion(int* socket_coordinador, t_esi_operacion instruccion) {

		switch (instruccion.keyword) {
		case (GET):
//fijarme si existe en memoria, en caso de que no exista tengo que guardar una de las entradas
			break;
//		case (SET):
//			char* clave = malloc(40);
//			char* valor = malloc(char);
//			*strcpy(clave, instruccion.argumentos.SET.clave);
//			*strcpy(valor, instruccion.argumentos.SET.valor);
// 			guardar_archivo(clave,valor);
//			break;
//		case (STORE):
			char* clave = malloc(40);
			*strcpy(clave, instruccion.argumentos.STORE.clave);
			int direccion = &clave;
			char informacion[];

			memcpy(informacion, *direccion, sizeof(informacion));

			//archivo = fopen("informacion.txt", "w");
			fwrite(&informacion, sizeof(informacion), 1, archivo);
			fclose (archivo);

			void enviar_a_desbloquear_clave(socket_coordinador, clave);
			break;
		}
	}
*/

	int handshake(int* socket_coordinador, t_log* logger) {
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = { INSTANCIA, 0 };
		int id_recibido;

		enviar(socket_coordinador, &yo, sizeof(t_handshake), 80, logger);
		recibir(socket_coordinador, &proceso_recibido, sizeof(t_handshake), logger);

		if (proceso_recibido.proceso != COORDINADOR) {

			log_info(logger, "Conexion invalida");
			exit(-1);
		}

		log_info(logger, "Conectado al COORDINADOR ", proceso_recibido.id_proceso);

		return 1;

	 /*algoritmo_distribucion(){

	}*/

}

//Falta hacer la funcion en donde se busque la direccion donde guardar la value es decir con la clave vamos buscando donde se
//encuentra y de ahi tomamos el lugar de la matriz en donde vamos a guardar la informacion (en el caso de que ya haya algo guardado
//esto se pisa)
//Si el coordinador pide un key que no existe lo va a identificar y me va a avisar mediante una instruccion que la agregue
//asique lo tengo que agregar al protocolo a esta nueva instruccion
//(ver issues puede ayudar)
