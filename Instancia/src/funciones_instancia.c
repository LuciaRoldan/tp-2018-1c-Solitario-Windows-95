#include "funciones_instancia.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
/////////////////////// INICIALIZACION ///////////////////////

void leer_configuracion_propia(char* path, configuracion_propia* configuracion) {

	FILE* archivo = fopen(path, "r");

	fscanf(archivo, "%s %s %d %s %s %d",
			mi_configuracion.ipCoordinador,
			mi_configuracion.puertoCoordinador,
			&(mi_configuracion.algoritmoDeReemplazo),
			mi_configuracion.puntoDeMontaje,
			mi_configuracion.nombreInstancia ,
			&(mi_configuracion.intervaloDump));
	fclose(archivo);

	printf("+++ %s +++", configuracion->puntoDeMontaje);
}

void enviar_exito(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 25);
	enviar(socket_coordinador, buffer, sizeof(int), logger);
	log_info(logger, "Le respondi al coordinador");
	free(buffer);
}

void enviar_confirmacion_cierre(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer, 20);
	enviar(socket_coordinador, buffer, sizeof(int), logger);
	log_info(logger, "Me estoy cerrando");
	free(buffer);
}

bool condicion_clave_entrada(void* datos){
	estructura_clave* entrada = (estructura_clave*) datos;
	log_info(logger, "Clave buscada: %s y encontrada: %s", clave_buscada, entrada->clave);
	return !strcmp(entrada->clave, clave_buscada);
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

	configuracion.cantidad_entradas = 3; //BORRAR ES PARA TESTEAR

	free(buffer);
	return configuracion;
}

void procesarID(int socket_coordinador, t_log* logger) {
	int tamanio_clave;
	void* buffer;
	int id = recibir_int(socket_coordinador, logger);
	t_esi_operacion instruccion;

	char* clave;

	switch (id) {
	case (82):
		log_info(logger, "Recibi una instruccion");
		instruccion = recibir_instruccion(socket_coordinador, logger);
		log_info(logger, "La deserialicé");
		procesar_instruccion(socket_coordinador, instruccion, logger);
		break;
	case (83):
		buffer = malloc(sizeof(int));
		recibir(socket_coordinador, buffer, sizeof(int), logger);
		tamanio_clave = deserializar_id(buffer);
		free(buffer);
		clave =	malloc(tamanio_clave);
		clave = recibe_pedido_status();
		enviar_status_clave(clave); //declarar
		free(clave);
		break;
	case (85): //INDICA QUE DEBE SALIR DEL WHILE
		enviar_confirmacion_cierre(socket_coordinador,logger);
		activa = false;
		break;
	}
}

char* recibe_pedido_status() {
	char* la_clave;
	int tamanio;
	void* buffer_tamanio = malloc(sizeof(int));
	int num = recibir(socket_coordinador,buffer_tamanio,sizeof(int),logger);
	printf("me llegaron %d bytes \n", num);
	tamanio = deserializar_id(buffer_tamanio);
	void* buffer = malloc(tamanio);
	int bytes_recibidos = recibir(socket_coordinador,buffer, tamanio,logger);
	printf("me llegaron %d bytes \n", bytes_recibidos);
	log_info(logger,"recibi %d bytes", bytes_recibidos);
	deserializar_string(buffer, la_clave);
	log_info(logger,"recibi la clave %s: ", la_clave);
	free(buffer_tamanio);
	free(buffer);
//	hay que agregar un free de la clave
	return la_clave;
}

int enviar_status_clave(char* clave){
	clave_buscada = clave;
	status_clave statuss;
	log_info(logger, "llegue hasta enviar status");
	if(list_any_satisfy(tabla_entradas, condicion_clave_entrada)){
		log_info(logger, "aguno satisdae busqueda status_clave");
	estructura_clave* entrada_encontrada = list_find(tabla_entradas, condicion_clave_entrada);
	status_clave status = {clave,idInstancia, 0, entrada_encontrada->valor};
	int tamanio_buffer = tamanio_buffer_status(status);
	void* buffer = malloc(tamanio_buffer);
	serializar_status_clave(buffer,status);
	int bytes_enviados = enviar(socket_coordinador,buffer,tamanio_buffer,logger);
	free(buffer);
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

bool existe_clave(char* clave) {
	clave_buscada = malloc(strlen(clave)+1);
	memcpy(clave_buscada, clave, strlen(clave)+1);

	bool resultado = list_any_satisfy(tabla_entradas, condicion_clave_entrada);
	free(clave_buscada);
	return resultado;
}

void procesar_instruccion(int socket_coordinador, t_esi_operacion instruccion, t_log* logger) {
	char* clave;
	char* valor;
	int tamanio_valor = 0;
	int tamanio_clave = 0;
	switch (instruccion.keyword) {

	case (GET):
		log_info(logger, "Se pidio operacion con GET");
		int entrada_libre = 0;
		tamanio_clave = strlen(instruccion.argumentos.GET.clave)+1;
		clave_buscada = malloc(tamanio_clave);
		memcpy(clave_buscada,instruccion.argumentos.GET.clave, tamanio_clave);
		estructura_clave* entrada_nueva = malloc(sizeof(estructura_clave));
		entrada_nueva->cantidad_operaciones = 0;
		entrada_nueva->clave = malloc(tamanio_clave); //guardo el espacio porque es un variable
		memcpy(entrada_nueva->clave, clave_buscada, tamanio_clave);
		entrada_nueva->valor = malloc(1);//hardcode por el plani
		memcpy(entrada_nueva->valor,"", sizeof(char));//esta guardando una entrada del gran malloc
		bool validacion = list_any_satisfy(tabla_entradas,condicion_clave_entrada);
		log_info(logger,"entrada nueva %d", entrada_nueva->cantidad_operaciones);
		if (!validacion) {
			printf("No existe la clave %s. Creando nueva. \n", clave_buscada);
				entrada_nueva->cantidad_operaciones = 0;
				entrada_nueva->tamanio_valor = 0;
				entrada_nueva->numero_entrada = list_size(tabla_entradas);
				list_add(tabla_entradas, entrada_nueva);
				log_info(logger,"Lo que guarda es: %s", entrada_nueva->clave);
				log_info(logger,"la entrada_libre ahora es: %d ", entrada_libre);
				log_info(logger, "El contenido del bitmap es: %d", acceso_tabla[1]);
		}
		free(clave_buscada);
		enviar_exito(socket_coordinador,logger);
		break;

	case (SET):
		log_info(logger, "Se pidio operacion con SET");
		estructura_clave* entrada_encontrada;
		tamanio_valor = strlen(instruccion.argumentos.SET.valor)+1;
		tamanio_clave = strlen(instruccion.argumentos.SET.clave)+1;

		log_info(logger, "Se guardaron los tamaños");

		clave_buscada = malloc(tamanio_clave);
		memcpy(clave_buscada, instruccion.argumentos.SET.clave, tamanio_clave);

		entrada_encontrada = list_find(tabla_entradas, condicion_clave_entrada);
		log_info(logger, "Paso el find %d", entrada_encontrada->cantidad_operaciones);

		cantidad_entradas = cantidad_entradas_ocupa(tamanio_valor);
		log_info(logger, "tiene %d entradas ", cantidad_entradas);
		entrada_encontrada->cantidad_entradas = cantidad_entradas;
		entrada_encontrada->tamanio_valor = tamanio_valor;
		entrada_encontrada->cantidad_operaciones = 0;

		log_info(logger, "Antes de asignar memoria");

		int resultado = asignar_memoria(*entrada_encontrada, cantidad_entradas, valor);
		log_info(logger, "se asigno memoria %d: ", resultado);

		if(resultado < 0){
			asignar_memoria(*entrada_encontrada, cantidad_entradas, valor);
		}

		free(entrada_encontrada->valor); //No lo cambien de lugar
		entrada_encontrada->valor = (puntero_pagina - cantidad_entradas)*configuracion.cantidad_entradas + inicio_memoria;
		memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);


		enviar_exito(socket_coordinador,logger);
		list_iterate(tabla_entradas, sumar_operacion);
//		free(valor);
		free(clave_buscada);
		break;

	case (STORE):
		log_info(logger, "Se pidio operacion con STORE");
		tamanio_clave = strlen(instruccion.argumentos.STORE.clave) + 1;
		guardar_archivo(instruccion.argumentos.STORE.clave, tamanio_clave, logger);
		log_info(logger, "Guarde en el archivo");
		enviar_exito(socket_coordinador,logger);
		list_iterate(tabla_entradas, sumar_operacion);
		break;
	}
}

//me deja saber si aluna entrada de la tabla de entradas esta libre
int any_entrada_bitmap_libre() {
	int i = 0;
	while (i != configuracion.cantidad_entradas) {
		log_info(logger, "El numero del bitmap es: %d", i);
		if (acceso_tabla[i] == 0) {
			log_info(logger, "El contenido del bitmap es: %d", acceso_tabla[i]);
			return i;
		}
		log_info(logger, "El contenido del bitmap es: %d", acceso_tabla[i]);
		i++;
	}
 	return -1;
}

int cantidad_entradas_ocupa(int tamanio_valor){
	if(tamanio_valor % configuracion.tamano_entrada == 0){
		return tamanio_valor/configuracion.tamano_entrada;
	} else {
		div_t resultado = div(tamanio_valor,configuracion.tamano_entrada);
				return resultado.quot +1;
	}
}

int asignar_memoria(estructura_clave clave, int entradas_contiguas_necesarias, char* valor){
	int contador = 0;
	int posicion_siguiente = 1; //la primera que quiero reservar
	int espacios_libres = 0;
	log_info(logger,"entro a asignar memoria");
	log_info(logger,"entradas_contiguas %d:", entradas_contiguas_necesarias);

	while(contador != entradas_contiguas_necesarias && puntero_pagina <= configuracion.cantidad_entradas){ //Muevo el puntero hasta que encuentre las entradas contiguas o me pase
		log_info(logger, "Entra al while");
		if(acceso_tabla[puntero_pagina] == 0){
			log_info(logger, "Entra al if");
			espacios_libres += 1;
			puntero_pagina += 1;
			contador += 1;
			log_info(logger, "El contador quedo en: %d ", contador);
		} else {
			puntero_pagina += 1;
			contador = 0;
		}
	}

	if(contador == entradas_contiguas_necesarias){ //Si tengo las necesarias
		//salio todo bien, hay que poner los bitmap en 1
		for(int i = 0; i < entradas_contiguas_necesarias; i++){
			acceso_tabla[puntero_pagina - i] = 1;

		}
		return 1;
	} else {
		if(entradas_contiguas_necesarias <= espacios_libres){ //Si hay suficientes pero no estan juntas
			compactar(); //Cuando termine tiene que volver a llamar a esta funcion
			puntero_pagina = 0;
			return -1;
		} else { //Si tengo que reemplazar
			implementar_algoritmo(&clave, logger); //Los algoritmos tienen que dejar el puntero_pagina al final del espacio que va a usar
			puntero_pagina = 0;
			return 1;
		}
	}

}

void sumar_operacion(void* entradas){
	estructura_clave* entrada = entradas;
	entrada->cantidad_operaciones += 1;
}


void almacenar_valor(char* valor, int tamanio_valor){
//	si alcanza la memoria lo guarda
	if((puntero_pagina + tamanio_valor) <= memoria_total){
		memcpy((inicio_memoria + puntero_pagina), valor,tamanio_valor);
		puntero_pagina += tamanio_valor;
	} else{
//	 si no compacta
		compactar();
	}
}

//void entradas_atomicas_contiguas(int necesarias){
//
//	int contador = 0;
//	while(contador != necesarias){
//
//	}
//}

void aplicar_algoritmo_circular(estructura_clave* entrada_nueva, t_log* logger) {
	log_info(logger, "Entre al algoritmo circular");
	int atomica = 1;
	log_info(logger, "variable atomica");
	estructura_clave* entrada_original;
	log_info(logger, "armo la estructura original");
	while (atomica) {
		log_info(logger,"Entre en el while");
		entrada_original = list_get(tabla_entradas, puntero_circular);
		log_info(logger, "Obtengo la entrada original");
		if (entrada_original->cantidad_entradas == 1) {
			log_info(logger, "Es atomica");
			atomica = 0;
		}
		log_info(logger, "Aumento el puntero circular");
		puntero_circular += 1;
		if(puntero_circular == configuracion.cantidad_entradas){
			puntero_circular = 0;
		}
	}
	log_info(logger, "Sale del while");
	entrada_nueva->numero_entrada = entrada_original->numero_entrada;
	list_replace_and_destroy_element(tabla_entradas,entrada_original->numero_entrada,entrada_nueva,borrar_entrada);
	log_info(logger, "Deberia reemplazar el elemento");
}

void borrar_entrada(void* entrada){
	estructura_clave* clave = entrada;
	free(clave->clave);
	free(clave->valor);
	free(clave);

}

void aplicar_algoritmo_LRU(estructura_clave* entrada_nueva, t_log* logger){
	log_info(logger, "Estoy en LRU");
	estructura_clave* entrada_LRU;
	estructura_clave* auxiliar;
	int maximo_LRU = 0;

	for(int i = 0; i < configuracion.cantidad_entradas; i++){
		entrada_LRU = list_get(tabla_entradas,i);
		if(entrada_LRU->cantidad_operaciones > maximo_LRU){
			maximo_LRU = entrada_LRU->cantidad_operaciones;
			auxiliar = entrada_LRU;
		}
	}
	entrada_nueva->numero_entrada = entrada_LRU->numero_entrada;
	list_replace_and_destroy_element(tabla_entradas,auxiliar->numero_entrada,entrada_nueva,borrar_entrada);
	log_info(logger, "La entrada elegida es la %d", auxiliar->numero_entrada);
}

void aplicar_algoritmo_BSU(estructura_clave* entrada_nueva, t_log* logger){
	log_info(logger,"Entro al algoritmo del BSU");
	estructura_clave* entrada_BSU;
	estructura_clave* auxiliar;
	int maximo_BSU = 0;

	for(int i = 0; i < configuracion.cantidad_entradas; i++){
		entrada_BSU = list_get(tabla_entradas,i);
		if(entrada_BSU->tamanio_valor > maximo_BSU){
			log_info(logger, "El tamanio visto es %d", entrada_BSU->tamanio_valor);
			maximo_BSU = entrada_BSU->tamanio_valor;
			log_info(logger, "El maximo es: %d", maximo_BSU);
			auxiliar = entrada_BSU;
		}
	}
	entrada_nueva->numero_entrada = auxiliar->numero_entrada;
	list_replace_and_destroy_element(tabla_entradas,auxiliar->numero_entrada,entrada_nueva,borrar_entrada);
	log_info(logger, "La entrada elegida es la %d", auxiliar->numero_entrada);
}


void implementar_algoritmo(estructura_clave* entrada_nueva, t_log* logger){
	switch(mi_configuracion.algoritmoDeReemplazo){
	case(CIRC):
		aplicar_algoritmo_circular(entrada_nueva, logger);
		break;
	case(LRU):
		aplicar_algoritmo_LRU(entrada_nueva, logger);
		break;
	case(BSU):
		aplicar_algoritmo_BSU(entrada_nueva, logger);
		break;
	}
}

void compactar(){
	printf("Quiso compactar");
	int cantidad_instancias;
	int mensaje = 22;
	void* buffercito = malloc(sizeof(int));
	serializar_id(buffercito, mensaje);
	enviar(socket_coordinador, buffercito, sizeof(int), logger);//Envia al coordinador el pedido de cantidad instancias
	free(buffercito);
	void* buffer = malloc(sizeof(int));
	recibir(socket_coordinador, buffer, sizeof(int), logger);//Recibe el protocolo que deberia ser 03
	mensaje = deserializar_id(buffer);
	recibir(socket_coordinador, buffer, sizeof(int), logger);//Recibe la cantidad
	cantidad_instancias = deserializar_id(buffer);
	log_info(logger, "Voy a hacer %d posts", cantidad_instancias);
	for(int i = 0; i < cantidad_instancias; i++){
		sem_post(&s_compactacion);//Habilita a todas las instancias a compactar
	}
}

void hilo_compactar(){
	sem_wait(&s_compactacion);
	//aca se hace la compactacion lol
}


void guardar_archivo(char* clave, int tamanio_clave, t_log* logger){

			log_info(logger, "Entre a guardar");
			char* path;
			char* valor;
			int tamanio_path = strlen(mi_configuracion.puntoDeMontaje)+1;
			path = malloc(tamanio_path + tamanio_clave + sizeof(char)*5);
			strcpy(path, mi_configuracion.puntoDeMontaje);
			log_info(logger, "La clave es: %s", clave);
			strcat(path + tamanio_path -1, clave);
			strcat(path + tamanio_path + tamanio_clave -2, ".txt\0");

			log_info(logger, "Tengo el path piola: %s", path);

			int fd;
			char* puntero_memoria;

			clave_buscada = malloc(tamanio_clave);
			memcpy(clave_buscada, clave, tamanio_clave);

			estructura_clave *entrada_encontrada = list_find(tabla_entradas,condicion_clave_entrada);
			entrada_encontrada->cantidad_operaciones = 0;
			int tamanio_valor = entrada_encontrada->tamanio_valor;
			valor = malloc(tamanio_valor + sizeof(char));
			memcpy(valor," ",sizeof(char));
			memcpy(valor,entrada_encontrada->valor,tamanio_valor);

			log_info(logger, "Clave encontrada: %s, valor: %s", entrada_encontrada->clave, valor);

			fd =  open(path, O_RDWR | O_CREAT, S_IRWXU);

			if (fd < 0) {
				log_info(logger, "No se pudo abrir el archivo");
			}

			lseek(fd,lugar_de_memoria - 1,SEEK_CUR);
			lugar_de_memoria += tamanio_valor + 1;

			write(fd, valor, (tamanio_valor + sizeof(char)));

			puntero_memoria = mmap(NULL,tamanio_valor + sizeof(char),PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
			log_info(logger, "Voy a guardar algo de tamanio %d", tamanio_valor);
			memcpy(puntero_memoria, valor, (tamanio_valor + sizeof(char)));
			msync(puntero_memoria, (tamanio_valor + sizeof(char)), MS_SYNC);
			munmap(puntero_memoria, (tamanio_valor + sizeof(char)));
			close(fd);
			free(clave_buscada);
			free(path);
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
		free(buffer_recepcion);
		return 1;
}

void dump(){
	while(activa){
		sleep(mi_configuracion.intervaloDump);
		pthread_mutex_lock(&m_tabla);
		list_iterate(tabla_entradas, dumpear);
		log_info(logger, "Hice el dump");
		pthread_mutex_unlock(&m_tabla);
	}
}

void dumpear(void* datos){
	log_info(logger, "Entre a dumpear");
	estructura_clave* entrada = datos;
	guardar_archivo(entrada->clave, strlen(entrada->clave)+1, logger);
}


	 /*algoritmo_distribucion(){

	}*/


//Falta hacer la funcion en donde se busque la direccion donde guardar la value es decir con la clave vamos buscando donde se
//encuentra y de ahi tomamos el lugar de la matriz en donde vamos a guardar la informacion (en el caso de que ya haya algo guardado
//esto se pisa)
//Si el coordinador pide un key que no existe lo va a identificar y me va a avisar mediante una instruccion que la agregue
//asique lo tengo que agregar al protocolo a esta nueva instruccion
//(ver issues puede ayudar)
