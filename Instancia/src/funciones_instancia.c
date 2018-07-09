#include "funciones_instancia.h"


/////////////////////// INICIALIZACION ///////////////////////


void recibir_configuracion(int socket_coordinador,datos_configuracion configuracion, t_log* logger) {
			recibir(socket_coordinador,(void*)&configuracion, sizeof(datos_configuracion), logger);
		}

void leer_configuracion_propia(configuracion_propia* configuracion, t_log* logger) {

	FILE* archivo = fopen("Configuracion_instancia.txt", "r");

	if (archivo < 1) {
		log_info(logger,"No se puede abrir el archivo Configuracion_instancia.txt");
		exit(1);
	}

	fscanf(archivo, "%s %s %s %s %d %d",
			configuracion->ipCoordinador,
			configuracion->puertoCoordinador,
			configuracion->algoritmoDeReemplazo,
			configuracion->puntoDeMontaje,
			&(configuracion->nombreInstancia),
			&(configuracion->intervaloDump));
	fclose(archivo);
}


	void procesarID(int socket_coordinador, t_log* logger){
		int id = recibir_int(socket_coordinador,logger);
		t_handshake handshake_coordi;
		t_esi_operacion instruccion;

		switch(id){
			case(00):
					recibir_configuracion(socket_coordinador, configuracion, logger);
			break;
			case(01):
	//				me pueden pedir clave sin una instruccion??
			break;
			case(02):
					recibir_instruccion(socket_coordinador, instruccion, logger);
					procesar_instruccion(socket_coordinador,instruccion);
			break;
			case(80):
					recibir(socket_coordinador,(void*)&handshake_coordi,sizeof(t_handshake),logger);
					deserializar_handshake((void*)&handshake_coordi);
			break;
		}
	}

	void deserializar_configuracion(void* buffer){
			memcpy(&(configuracion.cantidad_entradas),buffer,sizeof(int));
			memcpy(&(configuracion.tamano_entrada), (buffer + (sizeof(int))),sizeof(int));
		}

		void recibir_instruccion(int socket_coordinador, t_esi_operacion instruccion, t_log* logger) {
			recibir(socket_coordinador, (void*)&instruccion, sizeof(t_esi_operacion), logger);
		}
	//
	//	void enviar_a_desbloquear_clave(int* socket_coordinador, int clave, t_log* logger) {
	//		enviar(socket_coordinador, clave, sizeof(clave), 03, logger);
	//		//serializar?
	//	}

		/*void guardar_archivo(char* clave, char* valor) {

			int* direccion = obtener_direccion(clave);
			memcopy(direccion, valor, sizeof(valor));
		}*/


			void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion) { //REVISAR

//				switch (instruccion.keyword) {
//				case (GET):
//		//fijarme si existe en memoria, en caso de que no exista tengo que guardar una de las entradas
//					break;
//				case (SET):
//					char* clave = malloc(40);
//					char* valor = malloc(char);
//					*strcpy(clave, instruccion.argumentos.SET.clave);
//					*strcpy(valor, instruccion.argumentos.SET.valor);
//		 			guardar_archivo(clave,valor);
//					break;
//				case (STORE):
//					char* clave = malloc(40);
//					*strcpy(clave, instruccion.argumentos.STORE.clave);
//					int direccion = &clave;
//					char informacion[];
//
//					memcpy(informacion, *direccion, sizeof(informacion));
//
//					//archivo = fopen("informacion.txt", "w");
//					fwrite(&informacion, sizeof(informacion), 1, archivo);
//					fclose (archivo);
//
//					void enviar_a_desbloquear_clave(socket_coordinador, clave);
//					break;
//				}
			}

	int handshake(int* socket_coordinador, t_log* logger, int id) {
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = {id, INSTANCIA};
		int id_recibido;
		void* buffer_envio = malloc(sizeof(int)*3); //Es de 3 porque tambien se manda el protocolo

		serializar_handshake(buffer_envio, yo);
		enviar(*socket_coordinador, buffer_envio, sizeof(int)*3, 80, logger);

		free(buffer_envio);
		void* buffer_recepcion = malloc(sizeof(int)*2);

		recibir(*socket_coordinador, &id_recibido, sizeof(int), logger);
		recibir(*socket_coordinador, buffer_recepcion, sizeof(int)*2, logger);
		proceso_recibido = deserializar_handshake(buffer_recepcion);

		printf("%d\n", proceso_recibido.proceso);
		printf("%d\n", proceso_recibido.id);

		if (proceso_recibido.proceso != COORDINADOR) {

			log_info(logger, "Conexion invalida");
			exit(-1);
		}

		log_info(logger, "Conectado al COORDINADOR ", proceso_recibido.id);

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
