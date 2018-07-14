#include "funciones_instancia.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
/////////////////////// INICIALIZACION ///////////////////////

void leer_configuracion_propia(configuracion_propia* configuracion, t_log* logger) {

	FILE* archivo = fopen("../Configuracion_instancia.txt", "r");

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

void recibir_configuracion(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(datos_configuracion));
	recibir(socket_coordinador, buffer, sizeof(datos_configuracion), logger);
	deserializar_configuracion(buffer);
	free(buffer);
}

void deserializar_configuracion(void* buffer) {
	memcpy(&(configuracion.cantidad_entradas), buffer, sizeof(int));
	memcpy(&(configuracion.tamano_entrada), (buffer + (sizeof(int))), sizeof(int));
}

void procesarID(int socket_coordinador, t_log* logger) {
	int id = recibir_int(socket_coordinador, logger);
	t_handshake handshake_coordi;
	t_esi_operacion instruccion;
	char* clave;

	switch (id) {
	case (00):
		recibir_configuracion(socket_coordinador, logger);
		break;
	case (01):
		clave =	recibe_pedido_status(socket_coordinador, logger);
//		enviar_status_clave(); //declarar
		break;
	case (02):
		instruccion = recibir_instruccion(socket_coordinador, logger);
		procesar_instruccion(socket_coordinador, instruccion, logger);
		break;
	case (80):
		recibir(socket_coordinador, (void*) &handshake_coordi,sizeof(t_handshake), logger);
		deserializar_handshake((void*) &handshake_coordi);
		break;
	}
}

char* recibe_pedido_status(int socket_coordinador, t_log* logger){
	int tamanio;
	char* clave;
	void* buffer_tamanio = malloc(sizeof(int));
	recibir(socket_coordinador,buffer_tamanio,sizeof(int),logger);
	tamanio = deserializar_id(buffer_tamanio);
	void* buffer = malloc(tamanio);
	recibir(socket_coordinador,buffer,tamanio,logger);
	clave = deserializar_string(buffer);
	free(buffer_tamanio);
	free(tamanio);
	return clave;
}

int enviar_status_clave(int socket_coordinador, char*clave, t_log* logger){
	char* valor;
	valor = dictionary_get(diccionario_memoria,clave);
//	int idInstancia = mi_configuracion.nombreInstancia;
	status_clave status = {clave,0,valor};
	int tamanio_buffer = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio_buffer);
	serializar_status_clave(buffer,status);
	int bytes_enviados = enviar(socket_coordinador,buffer,tamanio_buffer,logger);
	return bytes_enviados;

}



t_esi_operacion recibir_instruccion(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(t_esi_operacion));
	recibir(socket_coordinador,buffer,sizeof(t_esi_operacion), logger);
	t_esi_operacion instruccion = deserializar_instruccion(buffer,logger);
	free(buffer);
	return instruccion;
}

void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion, t_log* logger) { //REVISAR

	switch (instruccion.keyword) {
	case (GET):
		if((dictionary_has_key(diccionario_memoria,instruccion.argumentos.GET.clave))){ //devuelve true si la tiene
			void* buffer = malloc(sizeof(int));
			serializar_id(buffer,04);
			enviar(socket_coordinador,buffer,sizeof(int),logger);
		} else{
			dictionary_put(diccionario_memoria,instruccion.argumentos.GET.clave,""); //esta bien hacer esto para crear una key?
			void* buffer = malloc(sizeof(int));
			serializar_id(buffer,05);
			enviar(socket_coordinador,buffer, sizeof(int),logger);
		}
		break;
	case (SET):
		dictionary_put(diccionario_memoria,instruccion.argumentos.SET.clave,instruccion.argumentos.SET.valor);
		break;
	case (STORE):
		guardar_archivo(instruccion.argumentos.STORE.clave, logger);
		enviar_a_desbloquear_clave(socket_coordinador, instruccion.argumentos.STORE.clave, logger);
		break;
	}
}

void guardar_archivo(char* clave, t_log* logger){

	//		No se si es el path que hay que usar o donde guardariamos los archivos?
			char* path = "/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/Instancia";
			char* real_path = malloc(sizeof(path));
			memcpy(real_path,path,sizeof(path));

			int fd;
			char* puntero_memoria;

			char* valor = dictionary_get(diccionario_memoria,clave);
			int tamanio_valor = sizeof(valor);

			fd =  open(real_path, O_RDWR | O_CREAT, S_IRWXU);

			if (fd < 0) {
				log_info(logger, "No se pudo abrir el archivo");
			}

			lseek(fd,tamanio_valor,SEEK_SET);

			puntero_memoria = mmap(NULL,tamanio_valor,PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
			memcpy(puntero_memoria, valor, tamanio_valor);

			msync(puntero_memoria, tamanio_valor, MS_SYNC);

			munmap(puntero_memoria, tamanio_valor);
}

	void enviar_a_desbloquear_clave(int socket_coordinador, char* clave, t_log* logger) {
		void* buffer = malloc(sizeof(int)+ sizeof(clave));
		serializar_pedido_desbloqueo(buffer,clave);
		enviar(socket_coordinador, buffer, sizeof(int), logger);
		free(buffer);
	}

	void serializar_pedido_desbloqueo(void* buffer, char* clave){
		serializar_id(buffer,03);
		memcpy(buffer,clave,sizeof(clave));
	}


int handshake(int* socket_coordinador, t_log* logger, int id) {
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = { INSTANCIA, id };
		int id_recibido;
		void* buffer_envio = malloc(sizeof(int) * 3); //Es de 3 porque tambien se manda el protocolo

		serializar_handshake(buffer_envio, yo);
		enviar(*socket_coordinador, buffer_envio, sizeof(int) * 3, logger);

		free(buffer_envio);
		void* buffer_recepcion = malloc(sizeof(int) * 2);

		recibir(*socket_coordinador, &id_recibido, sizeof(int), logger);
		recibir(*socket_coordinador, buffer_recepcion, sizeof(int) * 2, logger);
		proceso_recibido = deserializar_handshake(buffer_recepcion);

		printf("%d\n", proceso_recibido.proceso);
		printf("%d\n", proceso_recibido.id);

		if (proceso_recibido.proceso != COORDINADOR) {

			log_info(logger, "Conexion invalida");
			exit(-1);
		}

		log_info(logger, "Conectado al COORDINADOR ", proceso_recibido.id);

		return 1;
}
	 /*algoritmo_distribucion(){

	}*/


//Falta hacer la funcion en donde se busque la direccion donde guardar la value es decir con la clave vamos buscando donde se
//encuentra y de ahi tomamos el lugar de la matriz en donde vamos a guardar la informacion (en el caso de que ya haya algo guardado
//esto se pisa)
//Si el coordinador pide un key que no existe lo va a identificar y me va a avisar mediante una instruccion que la agregue
//asique lo tengo que agregar al protocolo a esta nueva instruccion
//(ver issues puede ayudar)
