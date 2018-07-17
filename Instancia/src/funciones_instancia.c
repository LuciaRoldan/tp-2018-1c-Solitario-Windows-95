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

void enviar_exito(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 25);
	enviar(socket_coordinador, buffer, sizeof(int), logger);
	log_info(logger, "Le respondi al coordinador");
	free(buffer);
}

bool condicion_clave_entrada(void* datos){
	estructura_clave entrada = *((estructura_clave*) datos);
	return strcmp(entrada.clave, clave_buscada);
}

int cantidad_entradas_ocupa(int tamanio_valor){
	if(tamanio_valor%configuracion.tamano_entrada == 0){
		return tamanio_valor/configuracion.tamano_entrada;
	} else {
		div_t resultado = div(tamanio_valor,configuracion.tamano_entrada);
				return resultado.quot +1;
	}

}

datos_configuracion recibir_configuracion(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(datos_configuracion));
	recibir(socket_coordinador, buffer, sizeof(datos_configuracion), logger);
	datos_configuracion configuracion = deserializar_configuracion_inicial_instancia(buffer);
	free(buffer);
	return configuracion;
}

void procesarID(int socket_coordinador, t_log* logger) {
	int id = recibir_int(socket_coordinador, logger);
	t_esi_operacion instruccion;
	// t_handshake handshake_coordi;
	char* clave;

	switch (id) {
	/*case (80):
		recibir(socket_coordinador, (void*) &handshake_coordi,sizeof(t_handshake), logger);
		deserializar_handshake((void*) &handshake_coordi);
		log_info(logger, "Hice el handshake");
		break;*/
	case (82):
		log_info(logger, "Recibi una instruccion");
		instruccion = recibir_instruccion(socket_coordinador, logger);
		log_info(logger, "La deserialicé");
		procesar_instruccion(socket_coordinador, instruccion, logger);
		break;
	case (83):
		clave =	recibe_pedido_status(socket_coordinador, logger);
//		enviar_status_clave(); //declarar
		break;
	}
}

char* recibe_pedido_status(int socket_coordinador, t_log* logger){
	int tamanio;
	char* clave = malloc(tamanio);
	void* buffer_tamanio = malloc(sizeof(int));
	int num = recibir(socket_coordinador,buffer_tamanio,sizeof(int),logger);
	printf("me llegaron %d bytes ", num);
	tamanio = deserializar_id(buffer_tamanio);
	void* buffer = malloc(tamanio + sizeof(int));
	int bytes_recibidos = recibir(socket_coordinador,buffer,(tamanio + sizeof(int)),logger);
	printf("me llegaron %d bytes ", bytes_recibidos);
	log_info(logger,"recibi %d bytes", bytes_recibidos);
	deserializar_string(buffer, clave);
	log_info(logger,"recibi la clave %s: ",clave);
	free(buffer_tamanio);
	free(buffer);
//	hay que agregar un free de la clave
	return clave;
}

int enviar_status_clave(int socket_coordinador, char*clave, t_log* logger){
	char* valor;
//	valor = dictionary_get(diccionario_memoria,clave);
//	int idInstancia = mi_configuracion.nombreInstancia;
	status_clave status = {clave,0,valor};
	int tamanio_buffer = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio_buffer);
	serializar_status_clave(buffer,status);
	int bytes_enviados = enviar(socket_coordinador,buffer,tamanio_buffer,logger);
	return bytes_enviados;

}


t_esi_operacion recibir_instruccion(int socket_coordinador, t_log* logger) {
	int tamanio_operacion = recibir_int(socket_coordinador, logger);
	void* buffer = malloc(tamanio_operacion);
	recibir(socket_coordinador, buffer, tamanio_operacion, logger);
	t_esi_operacion instruccion = deserializar_instruccion(buffer);
	free(buffer);
	log_info(logger, "Recibi instrucccion del COORDINADOR");
	return instruccion;
}

void enviar_fallo(int socket_coordinador, t_log* logger){
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 24);
	enviar(socket_coordinador,buffer,sizeof(int),logger);
	log_info(logger, "Le respondi al coordinador");
	free(buffer);
}

int existe_clave(char* clave) {
	estructura_clave* clave_encontrada = malloc(sizeof(estructura_clave));
	for (int i = 0; i < configuracion.cantidad_entradas; i++) {
		clave_encontrada = (estructura_clave*) list_get(tabla_entradas, i);
		if (clave_encontrada->clave == clave) {
			return 1;
		}
	}
	return 0;
}

void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion, t_log* logger) {
	char* clave;
	char* valor;
	int tamanio_valor = 0;
	int tamanio_clave = 0;
	switch (instruccion.keyword) {
	case (GET):
		log_info(logger, "Se pidio operacion con GET");
		clave = instruccion.argumentos.GET.clave;
		int tamanio_clave = sizeof(clave);
		printf("La clave es: %s\n", clave);
		if (existe_clave(clave) == 0){ // REVISAR PORQUE ESTO ROMPE. CAUSA UN SEGMENTATION FAULT.
			printf("No existe la clave %s. Creando nueva. \n", clave);
			estructura_clave entrada_nueva;
			entrada_nueva.clave = (char*) malloc(sizeof(char)*40); // 40 porque es el tamaño máximo de la clave y guardo el espacio porque es un variable
			memcpy(entrada_nueva.clave,clave,tamanio_clave);
			list_add_in_index(tabla_entradas, indice, &entrada_nueva);
			acceso_tabla[indice] = 1; //quiere decir que esta ocupada esa entrada
			indice ++;
		}
		enviar_exito(socket_coordinador,logger);
		break;
	case (SET):

		log_info(logger, "Se pidio operacion con SET");
		tamanio_valor = strlen(instruccion.argumentos.SET.valor)+1;
		tamanio_clave = strlen(instruccion.argumentos.SET.clave)+1;
		valor = malloc(tamanio_valor);
		clave_buscada = malloc(tamanio_clave);
		memcpy(valor, instruccion.argumentos.SET.valor, tamanio_valor);
		memcpy(clave_buscada, instruccion.argumentos.SET.clave, tamanio_clave);
		estructura_clave *entrada_encontrada = list_find(tabla_entradas,condicion_clave_entrada);
		int cantidad_entradas = cantidad_entradas_ocupa(tamanio_valor);
		entrada_encontrada->tamanio_valor = tamanio_valor;
		memcpy(entrada_encontrada->valor, valor, tamanio_valor);
		asignar_memoria(*entrada_encontrada, cantidad_entradas);
		enviar_exito(socket_coordinador,logger);
		break;
	case (STORE):
		log_info(logger, "Se pidio operacion con STORE");
		tamanio_clave = strlen(instruccion.argumentos.STORE.clave) + 1;
		guardar_archivo(instruccion.argumentos.STORE.clave, tamanio_clave, logger);
		enviar_a_desbloquear_clave(socket_coordinador, instruccion.argumentos.STORE.clave, logger);
		enviar_exito(socket_coordinador,logger);
		break;
	}
}

void asignar_memoria(estructura_clave clave, int entradas_contiguas_necesarias){
	int contador = 0;
	int posicion_actual = 0;
	//busca entradas vacias contiguas hasta que no haya mas
	while(contador == entradas_contiguas_necesarias){
		if (acceso_tabla[clave.cantidad_entradas + posicion_actual] == 0){
			posicion_actual += 1;
			contador += 1;
//			encaso de que no haya mas entradas aplica algoritmo de reemplazo
			if(clave.cantidad_entradas + posicion_actual == configuracion.cantidad_entradas){
				implementar_algoritmo();
			}
		else{
			posicion_actual += 1;
			contador == 0;
			if(clave.cantidad_entradas + posicion_actual == configuracion.cantidad_entradas){
							implementar_algoritmo();
				}
			}
		}
	}
//	cuando encontro las entradas contiguas almacena el valor
	for(int i = 0; i < entradas_contiguas_necesarias; i++){
		int numero_entrada = clave.cantidad_entradas + posicion_actual;
		acceso_tabla[numero_entrada - i] = 1;
		clave.numero_entrada = numero_entrada;
		almacenar_valor(clave.valor, clave.tamanio_valor);

	}
}

void implementar_algoritmo(){

}

void compactar(){

}

void almacenar_valor(char* valor, int tamanio_valor){
//	 si no alcanza el espacio en memoria lo compacta
	if(memoria_usada + tamanio_valor > memoria_total){
		compactar();
	} else{
//	si no lo guarda directamente
	memcpy(inicio_memoria + memoria_usada, valor,tamanio_valor);
	memoria_usada += tamanio_valor;
	}
}



void guardar_archivo(char* clave,int tamanio_clave, t_log* logger){

			char* path;
			char* valor;
			int tamanio_path = strlen(mi_configuracion.puntoDeMontaje)+1;
			path = malloc(tamanio_path);
			memcpy(path,mi_configuracion.puntoDeMontaje,sizeof(path));

			int fd;
			char* puntero_memoria;

			memcpy(clave_buscada, clave,tamanio_clave);
			estructura_clave *entrada_encontrada = list_find(tabla_entradas,condicion_clave_entrada);
			int tamanio_valor = entrada_encontrada->tamanio_valor;
			valor = malloc(tamanio_valor);
			memcpy(valor,entrada_encontrada->valor,tamanio_valor);

			fd =  open(path, O_RDWR | O_CREAT, S_IRWXU);

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
		serializar_id(buffer,83);
		memcpy(buffer + sizeof(int),clave, strlen(clave));
	}


int handshake_instancia(int socket_coordinador, t_log* logger, int id) {
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = { id, INSTANCIA };
		int id_recibido;
		void* buffer_envio = malloc(sizeof(int) * 3); //Es de 3 porque tambien se manda el protocolo

		serializar_handshake(buffer_envio, yo);
		enviar(socket_coordinador, buffer_envio, sizeof(int) * 3, logger);

		free(buffer_envio);
		void* buffer_recepcion = malloc(sizeof(int) * 2);

		recibir(socket_coordinador, &id_recibido, sizeof(int), logger);
		recibir(socket_coordinador, buffer_recepcion, sizeof(int) * 2, logger);
		proceso_recibido = deserializar_handshake(buffer_recepcion);

		printf("Proceso: %d\n", proceso_recibido.proceso);
		printf("Id del proceso: %d\n", proceso_recibido.id);

		if (proceso_recibido.proceso != COORDINADOR) {

			log_info(logger, "Conexion invalida");
			exit(-1);
		}

		log_info(logger, "Conectado al COORDINADOR ", proceso_recibido.id);

		return 1;
}

void aplicar_algoritmo_circular(){

}

	 /*algoritmo_distribucion(){

	}*/


//Falta hacer la funcion en donde se busque la direccion donde guardar la value es decir con la clave vamos buscando donde se
//encuentra y de ahi tomamos el lugar de la matriz en donde vamos a guardar la informacion (en el caso de que ya haya algo guardado
//esto se pisa)
//Si el coordinador pide un key que no existe lo va a identificar y me va a avisar mediante una instruccion que la agregue
//asique lo tengo que agregar al protocolo a esta nueva instruccion
//(ver issues puede ayudar)
