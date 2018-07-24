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
}

void enviar_exito(int socket_coordinador) {
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
	return !strcmp(entrada->clave, clave_buscada);
}

void recibir_configuracion(int socket_coordinador, t_log* logger) {
	void* buffer = malloc(sizeof(int)*2);
	recibir(socket_coordinador, buffer, sizeof(sizeof(int)*2), logger);
	configuracion_coordi = deserializar_configuracion_inicial_instancia(buffer);

	free(buffer);
}

void procesarID(int socket_coordinador, t_log* logger) {
	int tamanio_clave;
	void* buffer;
	int id = recibir_int(socket_coordinador, logger);
	t_esi_operacion instruccion;

	char* clave;

	switch (id) {
	case (03):
		compactar();
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
	log_info(logger, "llegue hasta enviar status");
	if(list_any_satisfy(tabla_entradas, condicion_clave_entrada)){
		estructura_clave* entrada_encontrada = list_find(tabla_entradas, condicion_clave_entrada);
		status_clave status = {clave,idInstancia, 0, entrada_encontrada->valor};
		int tamanio_buffer = tamanio_buffer_status(status);
		void* buffer = malloc(tamanio_buffer);
		serializar_status_clave(buffer,status);
		int bytes_enviados = enviar(socket_coordinador,buffer,tamanio_buffer,logger);
		free(buffer);
		return bytes_enviados;
	} else {
		return -1;
	}
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

void enviar_fallo(int socket_coordinador){
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
	char* valor;
	int tamanio_valor = 0;
	int tamanio_clave = 0;
	switch (instruccion.keyword) {

	case (GET):
		log_info(logger, "Se pidio operacion con GET");
		tamanio_clave = strlen(instruccion.argumentos.GET.clave) + 1;
		clave_buscada = malloc(tamanio_clave);
		memcpy(clave_buscada, instruccion.argumentos.GET.clave, tamanio_clave);
		estructura_clave* entrada_nueva = malloc(sizeof(estructura_clave));
		entrada_nueva->cantidad_operaciones = 0;
		entrada_nueva->clave = malloc(tamanio_clave); //guardo el espacio porque es un variable
		memcpy(entrada_nueva->clave, clave_buscada, tamanio_clave);
		entrada_nueva->valor = malloc(1); //hardcode por el plani
		memcpy(entrada_nueva->valor, "", sizeof(char)); //esta guardando una entrada del gran malloc
		bool validacion = list_any_satisfy(tabla_entradas,condicion_clave_entrada);
		if (!validacion) {
			printf("No existe la clave %s. Creando nueva. \n", clave_buscada);
			entrada_nueva->cantidad_operaciones = 0;
			entrada_nueva->tamanio_valor = 0;
			entrada_nueva->numero_entrada = list_size(tabla_entradas);
			list_add(tabla_entradas, entrada_nueva);
			log_info(logger, "Lo que guarda es: %s", entrada_nueva->clave);
		}
		free(clave_buscada);
		enviar_exito(socket_coordinador);
		break;

	case (SET):
		log_info(logger, "Se pidio operacion con SET");
		estructura_clave* entrada_encontrada;
		tamanio_valor = strlen(instruccion.argumentos.SET.valor) + 1;
		tamanio_clave = strlen(instruccion.argumentos.SET.clave) + 1;

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
		if(strcmp(entrada_encontrada->valor, "")==0){

			int resultado = asignar_memoria(*entrada_encontrada, cantidad_entradas, valor);
			log_info(logger, "se asigno memoria %d: ", resultado);

			if (resultado < 0) {
				asignar_memoria(*entrada_encontrada, cantidad_entradas, valor);
			}

			free(entrada_encontrada->valor); //No lo cambien de lugar

			entrada_encontrada->valor = (puntero_pagina - cantidad_entradas)* configuracion_coordi.tamano_entrada + inicio_memoria;

			log_info(logger, "mi direccion es: %d", entrada_encontrada->valor);

			memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);

			enviar_exito(socket_coordinador);

		} else {
			if(strlen(instruccion.argumentos.SET.valor) < entrada_encontrada->cantidad_entradas * configuracion_coordi.tamano_entrada){
						memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);
						enviar_exito(socket_coordinador);
						list_iterate(tabla_entradas, sumar_operacion);
						//free(valor);
						free(clave_buscada);
					} else {
						enviar_fallo(socket_coordinador);
					}
		}
		log_info(logger, "Termine el set");
		break;
		/*log_info(logger, "Se pidio operacion con SET");
		estructura_clave* entrada_encontrada;
		tamanio_valor = strlen(instruccion.argumentos.SET.valor) + 1;
		tamanio_clave = strlen(instruccion.argumentos.SET.clave) + 1;

		clave_buscada = malloc(tamanio_clave);
		memcpy(clave_buscada, instruccion.argumentos.SET.clave, tamanio_clave);

		entrada_encontrada = list_find(tabla_entradas, condicion_clave_entrada);

		cantidad_entradas = cantidad_entradas_ocupa(tamanio_valor);
		entrada_encontrada->cantidad_entradas = cantidad_entradas;
		entrada_encontrada->tamanio_valor = tamanio_valor;
		entrada_encontrada->cantidad_operaciones = 0;

		if (strcmp(entrada_encontrada->valor, "") == 0) {

			int resultado = asignar_memoria(*entrada_encontrada,
					cantidad_entradas, valor);
			log_info(logger, "se asigno memoria %d: ", resultado);

			if (resultado < 0) {
				asignar_memoria(*entrada_encontrada, cantidad_entradas, valor);
			}

			free(entrada_encontrada->valor); //No lo cambien de lugar
			entrada_encontrada->valor = (puntero_pagina - cantidad_entradas)* configuracion.cantidad_entradas + inicio_memoria;
			memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor,tamanio_valor);
			log_info(logger, "El valor que guarda es: %s ",
					entrada_encontrada->valor);

			enviar_exito(socket_coordinador);
			list_iterate(tabla_entradas, sumar_operacion);
			free(clave_buscada);*/
			break;

			case (STORE):
			log_info(logger, "Se pidio operacion con STORE");
			tamanio_clave = strlen(instruccion.argumentos.STORE.clave) + 1;
			guardar_archivo(instruccion.argumentos.STORE.clave, tamanio_clave,logger);
			log_info(logger, "Guarde en el archivo");
			enviar_exito(socket_coordinador);
			list_iterate(tabla_entradas, sumar_operacion);
			break;
	}
}

//me deja saber si aluna entrada de la tabla de entradas esta libre
int any_entrada_bitmap_libre() {
	int i = 0;
	while (i != configuracion_coordi.cantidad_entradas) {
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
	if(tamanio_valor % configuracion_coordi.tamano_entrada == 0){
		return tamanio_valor/configuracion_coordi.tamano_entrada;
	} else {
		div_t resultado = div(tamanio_valor,configuracion_coordi.tamano_entrada);
				return resultado.quot +1;
	}
}

int asignar_memoria(estructura_clave clave, int entradas_contiguas_necesarias, char* valor){
	int contador = 0;
	int espacios_libres = 0;
	int resultado = 0;
	puntero_pagina = 0;
	log_info(logger,"entradas_contiguas %d:", entradas_contiguas_necesarias);

	while(contador != entradas_contiguas_necesarias && puntero_pagina <= configuracion_coordi.cantidad_entradas -1){ //Muevo el puntero hasta que encuentre las entradas contiguas o me pase
		log_info(logger, "El bit map de en %d es %d", puntero_pagina, acceso_tabla[puntero_pagina]);
		if(acceso_tabla[puntero_pagina] == 0){
			espacios_libres += 1;
			puntero_pagina += 1;
			contador += 1;
		} else {
			puntero_pagina += 1;
			contador = 0;
		}
	}
	log_info(logger, "El contador de pargina quedo en: %d", puntero_pagina);

	if(contador == entradas_contiguas_necesarias){ //Si tengo las necesarias
		//salio todo bien, hay que poner los bitmap en 1
		log_info(logger, "Voy a actualizar el bitmap");
		for(int i = 0; i < entradas_contiguas_necesarias; i++){
			acceso_tabla[puntero_pagina - 1 - i] = 1;

		}
		return 1;
	} else {
		if(entradas_contiguas_necesarias <= espacios_libres){ //Si hay suficientes pero no estan juntas
//			compactar(); //Cuando termine tiene que volver a llamar a esta funcion
			puntero_pagina = 0;
			log_info(logger,"Entra a compactar");
			return 1; //solo caso positivo
		} else { //Si tengo que reemplazar
			log_info(logger,"Entra a buscar el algoritmo");
			resultado = implementar_algoritmo(&clave); //Los algoritmos tienen que dejar el puntero_pagina al final del espacio que va a usar
			if(resultado == 1){
				enviar_exito(socket_coordinador);
			} else {
				enviar_fallo(socket_coordinador);
			}
			return resultado;
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
//		compactar();
	}
}

int entradas_atomicas_contiguas(int puntero, int necesarias) {
	log_info(logger, "entra a buscar las entradas contiguas");
	int puntero_buscador = puntero;
	int contador = 0;
	while (contador != necesarias && puntero_buscador <= configuracion_coordi.cantidad_entradas) {
		estructura_clave* victima = list_get(tabla_entradas, puntero_buscador);
		if (victima->cantidad_entradas == 1) {
			puntero_buscador += 1;
			contador += 1;
		} else {
			puntero_buscador += 1;
			contador = 0;
		}
	}
	if(contador == necesarias){
		return puntero_buscador; //seria en la ultima entrada atomica que encuentra el puntero
	} else {
		return -1;
	}
}

int aplicar_algoritmo_circular(estructura_clave* entrada_nueva) {
	log_info(logger, "Entre al algoritmo circular");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	entrada_nueva = (estructura_clave*) malloc(sizeof(estructura_clave));
	log_info(logger, "Las entradas que necesito %d", entradas_necesarias);
	puntero_circular = entradas_atomicas_contiguas(puntero_circular,entradas_necesarias);
	if(puntero_circular == -1){
		log_error(logger,"No hay lugares atomicos para almacenar");
		return -1;
	} else{
		while(entradas_necesarias > 0){
			estructura_clave* victima = list_get(tabla_entradas,puntero_circular);
			log_info(logger,"la valor que tiene es: %s", victima->valor);
			entrada_nueva->numero_entrada = victima->numero_entrada;
			log_info(logger,"el numero de entrada de la victima es: %d", victima->numero_entrada);
			log_info(logger,"el numero de entrada de la entrada_nueva es: %d", entrada_nueva->numero_entrada);
			list_replace_and_destroy_element(tabla_entradas,victima->numero_entrada,entrada_nueva,borrar_entrada);
			log_info(logger,"reemplaza el elemento");
			puntero_circular ++;
			if(puntero_circular == configuracion_coordi.cantidad_entradas){
			puntero_circular = 0;
			}
		}
		puntero_pagina = puntero_circular;
		return 0;
	}
}

void borrar_entrada(void* entrada){
	estructura_clave* clave = entrada;
	free(clave->clave);
	free(clave->valor);
	free(clave);

}

int lru_atomicos_contiguos(int necesarias) {
	int maximo_LRU = 0;
	estructura_clave* entrada_LRU;
	estructura_clave* auxiliar;
	for (int i = 0; i < configuracion_coordi.cantidad_entradas; i++) {
		entrada_LRU = list_get(tabla_entradas, i);
		if (entrada_LRU->cantidad_operaciones > maximo_LRU) {
			maximo_LRU = entrada_LRU->cantidad_operaciones;
			auxiliar = entrada_LRU;
			int puntero = entradas_atomicas_contiguas(auxiliar->numero_entrada,
					necesarias);
			if (puntero != -1) {
				enviar_exito(socket_coordinador);
				return puntero;
			}
		}
	}
	return -1;
}



int aplicar_algoritmo_LRU(estructura_clave* entrada_nueva){
	log_info(logger, "Estoy en LRU");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	int puntero = lru_atomicos_contiguos(entradas_necesarias);

	if(puntero == -1){
			log_error(logger,"No hay lugares atomicos para almacenar");
			return -1;
	} else{
		while(entradas_necesarias > 0){
			estructura_clave* victima = list_get(tabla_entradas,puntero);
			entrada_nueva->numero_entrada = victima->numero_entrada;
			list_replace_and_destroy_element(tabla_entradas,victima->numero_entrada,entrada_nueva,borrar_entrada);
			puntero ++;
			if(puntero == configuracion_coordi.cantidad_entradas){
			puntero = 0;
			}
		}
		puntero_pagina = puntero;
		return 1; //solo caso de que haya salido bien
	}
}

int aplicar_algoritmo_BSU(estructura_clave* entrada_nueva) {
	log_info(logger, "Entro al algoritmo del BSU");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	int puntero = bsu_atomicos_contiguos(entradas_necesarias);

	if (puntero == -1) {
		log_error(logger, "No hay lugares atomicos para almacenar");
		return -1;
	} else {
		while (entradas_necesarias > 0) {
			estructura_clave* victima = list_get(tabla_entradas, puntero);
			entrada_nueva->numero_entrada = victima->numero_entrada;
			list_replace_and_destroy_element(tabla_entradas,victima->numero_entrada, entrada_nueva, borrar_entrada);
			puntero++;
			if (puntero == configuracion_coordi.cantidad_entradas) {
				puntero = 0;
			}
		}
		puntero_pagina = puntero;
		return 1; //solo caso de que haya salido bien
	}
}


int bsu_atomicos_contiguos(int necesarias) {
	int maximo_BSU = 0;
	estructura_clave* entrada_BSU;
	estructura_clave* auxiliar;
	for (int i = 0; i < configuracion_coordi.cantidad_entradas; i++) {
		entrada_BSU = list_get(tabla_entradas, i);
		if (entrada_BSU->cantidad_operaciones > maximo_BSU) {
			maximo_BSU = entrada_BSU->cantidad_operaciones;
			auxiliar = entrada_BSU;
			int puntero = entradas_atomicas_contiguas(auxiliar->numero_entrada,necesarias);
			if (puntero != -1) {
				enviar_exito(socket_coordinador);
				return puntero;
			}
		}
	}
	return -1;
}

int implementar_algoritmo(estructura_clave* entrada_nueva){
	int resultado = 0;;
	switch(mi_configuracion.algoritmoDeReemplazo){
	case(CIRC):
		resultado = aplicar_algoritmo_circular(entrada_nueva);
		break;
	case(LRU):
		resultado = aplicar_algoritmo_LRU(entrada_nueva);
		break;
	case(BSU):
		resultado = aplicar_algoritmo_BSU(entrada_nueva);
		break;
	}
	return resultado;
}

void compactar(){
	log_info(logger, "Quiso compactar");
	int* tabla_auxiliar;
	int proxima_entrada_ocupada;
	int puntero_auxiliar = 0;
	int tamanio_tabla = cantidad_entradas;
	char* valor_auxiliar;
	int tamanio_valor_auxiliar = 0;
	estructura_clave* elemento;
	tabla_auxiliar = (int*) malloc(cantidad_entradas * sizeof(int));
	char* memoria_nueva;
	memoria_nueva = (char*) malloc(memoria_total);
	while (tamanio_tabla != 0) {
		if (acceso_tabla[puntero_auxiliar] == 0) {
			proxima_entrada_ocupada = buscar_siguiente_entrada_ocupada(puntero_auxiliar);
			elemento = list_get(tabla_entradas, proxima_entrada_ocupada);
			tamanio_valor_auxiliar = elemento->tamanio_valor;
			valor_auxiliar = malloc(tamanio_valor_auxiliar);
			memcpy(valor_auxiliar, elemento->valor, tamanio_valor_auxiliar);

			if (elemento->cantidad_entradas > 1) {
				for (int i = 0; i < cantidad_entradas; i++) {
					tabla_auxiliar[puntero_auxiliar + i] = 1;
					acceso_tabla[puntero_auxiliar + i] = 0;
					list_replace_and_destroy_element(tabla_entradas, puntero_auxiliar + i, elemento, borrar_entrada);
					elemento->valor = (puntero_auxiliar - elemento->cantidad_entradas) * cantidad_entradas + memoria_nueva;
					memcpy(elemento->valor, valor_auxiliar, tamanio_valor_auxiliar);
				}
			} else {
				tabla_auxiliar[puntero_auxiliar] = 1;
				acceso_tabla[puntero_auxiliar] = 0;
				list_replace_and_destroy_element(tabla_entradas, puntero_auxiliar, elemento, borrar_entrada);
				elemento->valor = (puntero_auxiliar - elemento->cantidad_entradas) * cantidad_entradas + memoria_nueva;
				memcpy(elemento->valor, valor_auxiliar, tamanio_valor_auxiliar);
			}
		} else {
			tabla_auxiliar[puntero_auxiliar] = 1;
			acceso_tabla[puntero_auxiliar] = 0;
			elemento = list_get(tabla_entradas, puntero_auxiliar);
			tamanio_valor_auxiliar = elemento->tamanio_valor;
			valor_auxiliar = malloc(tamanio_valor_auxiliar);
			if (elemento->cantidad_entradas > 1) {
				for (int i = 0; i < cantidad_entradas; i++) {
					tabla_auxiliar[puntero_auxiliar + i] = 1;
					acceso_tabla[puntero_auxiliar + i] = 0;
					list_replace_and_destroy_element(tabla_entradas, puntero_auxiliar + i, elemento, borrar_entrada);
					elemento->valor = (puntero_auxiliar - elemento->cantidad_entradas) * cantidad_entradas + memoria_nueva;
					memcpy(elemento->valor, valor_auxiliar, tamanio_valor_auxiliar);
				}
			} else {
				tabla_auxiliar[puntero_auxiliar] = 1;
				acceso_tabla[puntero_auxiliar] = 0;
				list_replace_and_destroy_element(tabla_entradas, puntero_auxiliar, elemento, borrar_entrada);
				elemento->valor = (puntero_auxiliar	- elemento->cantidad_entradas) * cantidad_entradas + memoria_nueva;
				memcpy(elemento->valor, valor_auxiliar, tamanio_valor_auxiliar);
			}
		}
		free(valor_auxiliar);
		puntero_auxiliar++;
	}
	free(inicio_memoria);
	inicio_memoria = memoria_nueva;
	for (int i = 0; i < cantidad_entradas; i++) {
		acceso_tabla[i] = tabla_auxiliar[i];
	}
	free(tabla_auxiliar);
	free(memoria_nueva);

//	sem_init(&s_compactacion, 0, 0); No se si esta bien esto
//	for(int i = 0; i < cantidad_instancias; i++){
//		sem_post(&s_compactacion);//Habilita a todas las instancias a compactar
//	}
}

int buscar_siguiente_entrada_ocupada(int inicio_indice) {
	for (int i = 0; i < cantidad_entradas; i++) {
		if (acceso_tabla[i] == 1) {
			return i;
		}
	}
	return -1;
}

void enviar_pedido_compactacion(){
	void* buffer = malloc(sizeof(int));
	serializar_id(buffer,22);
	enviar(socket_coordinador,buffer,sizeof(int),logger);
	free(buffer);
}

int recibir_orden_compactacion(){
	int id;
	void* buffer = malloc(sizeof(int));
	id = deserializar_id(buffer);
	return id;;
}

//void hilo_compactar(){
//	sem_wait(&s_compactacion);
//	aca se hace la compactacion lol
//}


void guardar_archivo(char* clave, int tamanio_clave, t_log* logger){

			log_info(logger, "Entre a guardar");
			clave_buscada = malloc(tamanio_clave);
			memcpy(clave_buscada, clave, tamanio_clave);

			estructura_clave *entrada_encontrada = list_find(tabla_entradas,condicion_clave_entrada);
			entrada_encontrada->cantidad_operaciones = 0;

			if(strcmp(entrada_encontrada->valor, "") != 0){
				char* path;
				char* valor;
				int tamanio_path = strlen(mi_configuracion.puntoDeMontaje)+1;
				path = malloc(tamanio_path + tamanio_clave + sizeof(char)*5);
				strcpy(path, mi_configuracion.puntoDeMontaje);
				log_info(logger, "La clave es: %s", clave);
				strcat(path + tamanio_path -1, clave);
				strcat(path + tamanio_path + tamanio_clave -2, ".txt\0");

				int fd;
				char* puntero_memoria;

				int tamanio_valor = entrada_encontrada->tamanio_valor;
				valor = malloc(tamanio_valor);
				memcpy(valor,entrada_encontrada->valor,tamanio_valor);

				fd =  open(path, O_RDWR | O_CREAT, S_IRWXU);

				if (fd < 0) {
					log_info(logger, "No se pudo abrir el archivo");
					_exit_with_error(socket_coordinador, "Error al abrir el archivo", NULL, logger);
				}

							/*lseek(fd,lugar_de_memoria - 1,SEEK_CUR);
							lugar_de_memoria += tamanio_valor + 1;*/

				lseek(fd,0,SEEK_SET);

				write(fd, valor, tamanio_valor);

				puntero_memoria = mmap(NULL,tamanio_valor + sizeof(char),PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
				log_info(logger, "Voy a guardar algo de tamanio %d: %s", tamanio_valor, entrada_encontrada->valor);
				memcpy(puntero_memoria, valor, (tamanio_valor + sizeof(char)));
				msync(puntero_memoria, (tamanio_valor + sizeof(char)), MS_SYNC);
				munmap(puntero_memoria, (tamanio_valor + sizeof(char)));
				close(fd);
				free(clave_buscada);
				free(path);
			}

}


int handshake_instancia(int socket_coordinador, t_log* logger, int id) {
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
	estructura_clave* entrada = datos;
	guardar_archivo(entrada->clave, strlen(entrada->clave)+1, logger);
}


