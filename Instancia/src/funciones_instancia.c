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
	log_info(logger, "Le respondi por exito al coordinador");
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
	return strcmp(entrada->clave, clave_buscada) == 0;
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

	pthread_mutex_lock(&m_tabla);

	char* clave;

	switch (id) {
	case (03):
		compactar();
		break;
	case (82):
		log_info(logger, "Recibi una instruccion");
		instruccion = recibir_instruccion(socket_coordinador, logger);
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
	pthread_mutex_unlock(&m_tabla);
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
		status_clave status = {clave,idInstancia, 0, ""};
		int tamanio_buffer = tamanio_buffer_status(status);
		void* buffer = malloc(tamanio_buffer);
		serializar_status_clave(buffer,status);
		int bytes_enviados = enviar(socket_coordinador,buffer,tamanio_buffer,logger);
		free(buffer);
		return bytes_enviados;
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
	log_info(logger, "Le respondi por fallo al coordinador");
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
	int tamanio_valor = 0;
	int tamanio_clave = 0;
	switch (instruccion.keyword) {

	case (GET):
		log_info(logger, "Se pidio operacion con GET");
		list_iterate(tabla_entradas, sumar_operacion);
		enviar_exito(socket_coordinador);
	break;

	case (SET):
		log_info(logger, "Se pidio operacion con SET");
		log_info(logger, "La clave del set es: %s", instruccion.argumentos.SET.clave);
		estructura_clave* entrada_encontrada;
		tamanio_valor = strlen(instruccion.argumentos.SET.valor) + 1;
		tamanio_clave = strlen(instruccion.argumentos.SET.clave) + 1;
		list_iterate(tabla_entradas, sumar_operacion);

		clave_buscada = malloc(tamanio_clave);
		memcpy(clave_buscada, instruccion.argumentos.SET.clave, tamanio_clave);

		if(list_any_satisfy(tabla_entradas, condicion_clave_entrada)){ //En el caso de que se haga set de una clave que ya se habia hecho set

			log_info(logger,"Ya se hizo SET de esta clave");
			entrada_encontrada = list_find(tabla_entradas, condicion_clave_entrada);
			entrada_encontrada->cantidad_operaciones = 0;

			if(cantidad_entradas_ocupa(tamanio_valor) <= entrada_encontrada->cantidad_entradas){
					memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);
					entrada_encontrada->tamanio_valor = tamanio_valor;
					free(clave_buscada);
					log_info(logger, "Quedo guardado: %s", entrada_encontrada->valor);

			} else {

				log_info(logger, "El nuevo valor se pasa del tamanio que habia reservado");

				//Cambie que se borra la entrada anterior y agrego una nueva como si fuese la primera vez que se hace el set
				for(int i = 0; i < entrada_encontrada->cantidad_entradas; i++){
					acceso_tabla[i + entrada_encontrada->numero_pagina] = 0;
				}
				list_remove_and_destroy_by_condition(tabla_entradas, condicion_clave_entrada, borrar_entrada);

				entrada_encontrada = malloc(sizeof(estructura_clave));
				entrada_encontrada->clave = malloc(tamanio_clave);
				memcpy(entrada_encontrada->clave, instruccion.argumentos.SET.clave, tamanio_clave);

				cantidad_entradas = cantidad_entradas_ocupa(tamanio_valor);
				log_info(logger, "tiene %d entradas ", cantidad_entradas);
				entrada_encontrada->cantidad_entradas = cantidad_entradas;
				entrada_encontrada->tamanio_valor = tamanio_valor;


				int resultado = asignar_memoria(entrada_encontrada, cantidad_entradas, instruccion.argumentos.SET.valor);
				if(resultado < 0){
					log_info(logger, "Asigno memoria por segunda vez");
					asignar_memoria(entrada_encontrada, cantidad_entradas, instruccion.argumentos.SET.valor);}

				entrada_encontrada->valor = (puntero_pagina - cantidad_entradas)* configuracion_coordi.tamano_entrada + inicio_memoria;
				log_info(logger, "mi direccion es: %d", entrada_encontrada->valor);
				memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);
				entrada_encontrada->numero_pagina = puntero_pagina - cantidad_entradas;
				log_info(logger, "Quedo guardado: %s", entrada_encontrada->valor);
				log_info(logger, "Voy a guardar en %d", entrada_encontrada->numero_pagina);

				list_add(tabla_entradas, entrada_encontrada);
				/*puntero_entrada++; //Se agrega suma la entrada que se agrega

				if(cantidad_entradas > 1){
					for(int i = 0; i < configuracion_coordi.cantidad_entradas; i++){
						puntero_pagina += i;
						if(puntero_pagina == (configuracion_coordi.cantidad_entradas)){ //se cambia cuando es igual porque este todavia no se uso
							puntero_pagina = 0; // el ultimo usado es cantidad entradas - 1
						}
					}

				} else{
					puntero_pagina++;
					if(puntero_pagina == (configuracion_coordi.cantidad_entradas)){
						puntero_pagina = 0;
					}
				}*/

				//enviar_exito(socket_coordinador);

				}

		} else {

			log_info(logger,"Es la primera vez que se hace SET de esta clave");
			entrada_encontrada = malloc(sizeof(estructura_clave));
			entrada_encontrada->clave = malloc(tamanio_clave);
			memcpy(entrada_encontrada->clave, instruccion.argumentos.SET.clave, tamanio_clave);

			cantidad_entradas = cantidad_entradas_ocupa(tamanio_valor);
			log_info(logger, "tiene %d entradas ", cantidad_entradas);
			entrada_encontrada->cantidad_entradas = cantidad_entradas;
			entrada_encontrada->tamanio_valor = tamanio_valor;
			entrada_encontrada->cantidad_operaciones = 0;

			int resultado = asignar_memoria(entrada_encontrada, cantidad_entradas, instruccion.argumentos.SET.valor);
			if(resultado < 0){
				log_info(logger, "Asigno memoria por segunda vez");
				asignar_memoria(entrada_encontrada, cantidad_entradas, instruccion.argumentos.SET.valor);}

			entrada_encontrada->valor = (puntero_pagina - cantidad_entradas)* configuracion_coordi.tamano_entrada + inicio_memoria;
			log_info(logger, "mi direccion es: %d", entrada_encontrada->valor);
			memcpy(entrada_encontrada->valor, instruccion.argumentos.SET.valor, tamanio_valor);
			entrada_encontrada->numero_pagina = puntero_pagina - cantidad_entradas;
			log_info(logger, "Quedo guardado: %s", entrada_encontrada->valor);
			log_info(logger, "Voy a guardar en %d", entrada_encontrada->numero_pagina);

			list_add(tabla_entradas, entrada_encontrada);
			/*puntero_entrada++; //Se suma la entrada que se agrega
			log_info(logger, "EL PUNTERO AHORA ESTA EN %d:", puntero_entrada);

			if(cantidad_entradas > 1){
				for(int i = 0; i < configuracion_coordi.cantidad_entradas; i++){
					puntero_pagina += i;
					if(puntero_pagina == (configuracion_coordi.cantidad_entradas)){ //se cambia cuando es igual porque este todavia no se uso
						puntero_pagina = 0; // el ultimo usado es cantidad entradas - 1
					}
				}


			} else{
				puntero_pagina++;
				if(puntero_pagina == (configuracion_coordi.cantidad_entradas)){
					puntero_pagina = 0;
				}
			}*/
		}
			enviar_exito(socket_coordinador); //Revisar si esta bien aca porque siempre va a enviar exito

		log_info(logger, "------------------------------------------------------------");
		log_info(logger, "---------------------- TABLA ENTRADAS ----------------------");
		for(int i = 0; i < list_size(tabla_entradas); i++){
			estructura_clave* nodo = list_get(tabla_entradas, i);
			log_info(logger, "En la posicion %d esta la clave %s con el valor %s", i, nodo->clave, nodo->valor);
			log_info(logger, "El segmento ocupa %d paginas", nodo->cantidad_entradas);
		}
		log_info(logger, "-------------------------- BITMAP --------------------------");
		for(int i = 0; i < configuracion_coordi.cantidad_entradas; i++){
			log_info(logger, "En la posicion %d el bitmap es %d", i, acceso_tabla[i]);
		}
		log_info(logger, "------------------------------------------------------------");

		log_info(logger, "Termine el set");
		break;

			case (STORE):
			log_info(logger, "Se pidio operacion con STORE");
			tamanio_clave = strlen(instruccion.argumentos.STORE.clave) + 1;

			clave_buscada = malloc(tamanio_clave);
			memcpy(clave_buscada, instruccion.argumentos.SET.clave, tamanio_clave);

			if(list_any_satisfy(tabla_entradas, condicion_clave_entrada)){
				guardar_archivo(instruccion.argumentos.STORE.clave, tamanio_clave,logger);
				log_info(logger, "Guarde en el archivo");
				enviar_exito(socket_coordinador);
			} else {
				enviar_fallo(socket_coordinador);
			}

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
	//int valor_sin_barra_cero = tamanio_valor -1;
	if(tamanio_valor % configuracion_coordi.tamano_entrada == 0){
		return tamanio_valor/configuracion_coordi.tamano_entrada;
	} else {
		div_t resultado = div(tamanio_valor,configuracion_coordi.tamano_entrada);
				return resultado.quot +1;
	}
}

int asignar_memoria(estructura_clave* clave, int entradas_contiguas_necesarias, char* valor){
	int contador = 0;
	int espacios_libres = 0;
	int resultado = 0;
	puntero_pagina = 0;
	log_info(logger,"entradas_contiguas necesarias %d:", entradas_contiguas_necesarias);

	while(contador != entradas_contiguas_necesarias && puntero_pagina <= configuracion_coordi.cantidad_entradas -1){ //Muevo el puntero hasta que encuentre las entradas contiguas o me pase
//		log_info(logger, "El bit map de en %d es %d", puntero_pagina, acceso_tabla[puntero_pagina]);
//		siempre va a pasar por esta parte ya sea de una o despues de aplicar compactacion y/o algoritmo por eso esta bien que el puntero de pagina se mantenga en 0
		if(acceso_tabla[puntero_pagina] == 0){
			espacios_libres += 1;
			puntero_pagina += 1;
			contador += 1;
		} else {
			puntero_pagina += 1;
			contador = 0;
		}
	}
	log_info(logger, "El puntero de pagina quedo en: %d", puntero_pagina);

	if(contador == entradas_contiguas_necesarias){ //Si tengo las necesarias
		//salio todo bien, hay que poner los bitmap en 1
		clave->numero_pagina = puntero_pagina - entradas_contiguas_necesarias; // el puntero_pagina queda en el siguiente a rellenar
		log_info(logger, "Voy a actualizar el bitmap");
		for(int i = 0; i < entradas_contiguas_necesarias; i++){
			acceso_tabla[puntero_pagina - 1 - i] = 1;
		}
		resultado = 1;
	} else {
		if(entradas_contiguas_necesarias <= espacios_libres){ //Si hay suficientes pero no estan juntas
			compactar(); //Cuando termine tiene que volver a llamar a esta funcion
			puntero_pagina = 0;
			log_info(logger,"Termine de compactar");
			return -1; //para que vuelva a entrar a la funcion
		} else { //Si tengo que reemplazar
			log_info(logger,"Entra a buscar el algoritmo");
			resultado = implementar_algoritmo(clave, entradas_contiguas_necesarias); //Los algoritmos tienen que dejar el puntero_pagina al final del espacio que va a usar
			return -1;
		}
		//resultado = implementar_algoritmo(clave, entradas_contiguas_necesarias);
	}
	return resultado;
}

void sumar_operacion(void* entradas){
	estructura_clave* entrada = entradas;
	entrada->cantidad_operaciones += 1;
}


void almacenar_valor(char* valor, int tamanio_valor){ //creo que no la estamos usando
//	si alcanza la memoria lo guarda
	if((puntero_pagina + tamanio_valor) <= memoria_total){
		memcpy((inicio_memoria + puntero_pagina), valor,tamanio_valor);
		puntero_pagina += tamanio_valor;
	} else{
//	 si no compacta
//		compactar();
	}
}


int cantidad_entradas_atomicas() {
	int contador = 0;
	int puntero = 0;
	estructura_clave* entrada;
	while (puntero < list_size(tabla_entradas)) {
		entrada = list_get(tabla_entradas, puntero);
		if (entrada->cantidad_entradas == 1) {
			puntero += 1;
			contador += 1;
		} else {
			puntero += 1;
		}
	}
	log_info(logger, "Hay %d entradas atomicas", contador);
	return contador;
}



void reemplazar_y_destruir(int indice, estructura_clave* estructura_nueva){
	log_info(logger,"entra a reemplazar y destruir");
	estructura_clave* estructura_vieja = list_remove(tabla_entradas, indice);
	log_info(logger,"El tamanio de la lista: %d ", list_size(tabla_entradas));
	log_info(logger, "el lugar donde estoy %d ", indice);
	free(estructura_vieja->clave);
	free(estructura_vieja);
	estructura_nueva->numero_pagina = indice;
	list_add_in_index(tabla_entradas,indice,estructura_nueva);
}

void borrar_entrada(void* entrada){
	estructura_clave* clave = entrada;
//	free(clave->clave);
//	log_info(logger, "termino el borrar");
	free(clave);
}

void obtener_puntero_entrada(){
	estructura_clave* estructura;
	estructura = list_find(tabla_entradas, condicion_tiene_puntero_entrada);
	puntero_entrada = estructura->numero_pagina;
}


bool condicion_tiene_puntero_entrada(void* datos){
	estructura_clave entrada = *((estructura_clave*) datos);
	bool respuesta = (entrada.numero_pagina <= puntero_pagina_buscado) && (puntero_pagina_buscado <= (entrada.numero_pagina + entrada.cantidad_entradas));
	return respuesta;

}

int entradas_atomicas_contiguas(int puntero, int necesarias) {
	log_info(logger, "entra a buscar las entradas contiguas");
	int puntero_buscador = puntero; //puntero de una entrada
	int contador = 0;
	log_info(logger, "El puntero es %d", puntero_buscador);
	for (int i = 0; i < list_size(tabla_entradas); i++) {
		estructura_clave* victima = list_get(tabla_entradas, puntero_buscador);
		if (contador != necesarias) {
			if (victima->cantidad_entradas == 1) {
				log_info(logger, "la pagina %d es atomica", puntero_buscador);
				puntero_buscador += 1;
				contador += 1;
			} else {
				puntero_buscador += 1;
				contador = 0;
			}
			if (puntero_buscador == list_size(tabla_entradas)) {
				puntero_buscador = 0;
			}
		}
	}
	if (contador == necesarias) {
		return puntero; //devuelve el puntero a la primer entrada que hay que liberar
	} else {
		return -1;//si no encontro las entradas contiguas
	}
}

int entradas_atomicas_vacias_contiguas(int necesarias) {
	int puntero = 0;
	int contador = 0;
	int i = 0;

	while (contador != necesarias) {
		if (acceso_tabla[i] == 0) {
			contador++;
			i++;
		} else {
			contador = 0;
			i++;
		}
	}
	return puntero - necesarias;

}

int buscar_mayor_bsu_atomico(int necesarias) {
	log_info(logger,"Busca el mayor bsu");
	int maximo_BSU = 0;
	int segmento_seleccionado;
	estructura_clave* entrada_BSU;
	estructura_clave* auxiliar;
	for (int i = 0; i < list_size(tabla_entradas); i++) {
		entrada_BSU = list_get(tabla_entradas, i);
		if (entrada_BSU->tamanio_valor > maximo_BSU  && entrada_BSU->cantidad_entradas == 1) {
			maximo_BSU = entrada_BSU->tamanio_valor;
			auxiliar = entrada_BSU;
			segmento_seleccionado = i;
			log_info(logger, "La clave con mayor BSU es %s: ", entrada_BSU->clave);
		}
	}
	return segmento_seleccionado;
}

int buscar_mayor_lru_atomico(int necesarias) {
	log_info(logger,"Busca el mayor lru");
	int maximo_LRU = 0;
	int segmento_seleccionado;
	estructura_clave* entrada_LRU;
	estructura_clave* auxiliar;
	for (int i = 0; i < list_size(tabla_entradas); i++) {
		entrada_LRU = list_get(tabla_entradas, i);
		if (entrada_LRU->cantidad_operaciones > maximo_LRU && entrada_LRU->cantidad_entradas == 1) {
			maximo_LRU = entrada_LRU->cantidad_operaciones;
			auxiliar = entrada_LRU;
			segmento_seleccionado = i;
			log_info(logger, "La clave con mayor LRU es %s: ", entrada_LRU->clave);
			}
		}
	return segmento_seleccionado; //puntero al elegido para eliminar
}

int aplicar_algoritmo_circular(estructura_clave* entrada_nueva) {
	puntero_circular = 0;
	log_info(logger, "Entre al algoritmo circular");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	log_info(logger, "Las entradas que necesito %d", entradas_necesarias);
	int atomicas_borradas = 0;
	int resultado = entradas_atomicas_contiguas(puntero_circular, entradas_necesarias); //puntero a las paginas atomicas contiguas (en caso de no haber va a cero ya que habria que recorrer toda la lista
	//el resultado me devuelve el puntero a la primer entrada que deberia eliminar (si no hay que compactar)
	estructura_clave* victima;
	log_info(logger, "Recibi %d de entradas atomicas", resultado);
	if(resultado != (-1)){
		puntero_circular = resultado;
	}
	while (atomicas_borradas != entradas_necesarias) {
		victima = list_get(tabla_entradas, puntero_circular);
		if (victima->cantidad_entradas == 1) {
			acceso_tabla[victima->numero_pagina] = 0;
			list_remove_and_destroy_element(tabla_entradas, puntero_circular, borrar_entrada);
			puntero_circular++;
			atomicas_borradas++;

		} else {
			puntero_circular++;
		}
		if(puntero_circular >= (list_size(tabla_entradas))){
			puntero_circular = 0;
		}

	}
	return resultado; // si habia entradas contiguas me lo devuelve en el puntero a donde empezo a borrar sino en -1
}


int aplicar_algoritmo_LRU(estructura_clave* entrada_nueva) {
	log_info(logger, "Estoy en LRU");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	int puntero = 0;

	while (entradas_necesarias > 0) {
		puntero = buscar_mayor_lru_atomico(entradas_necesarias); //cada vez que borre el mayo lru, va a haber uno nuevo
		estructura_clave* auxiliar = list_get(tabla_entradas, puntero);
		for(int i = 0; i < auxiliar->cantidad_entradas; i++){
			acceso_tabla[auxiliar->numero_pagina + i] = 0;
		}
		list_remove_and_destroy_element(tabla_entradas, puntero, borrar_entrada);

		puntero++;
		entradas_necesarias--;
		if (puntero == configuracion_coordi.cantidad_entradas) {
			puntero = 0;
		}
	}
	if(entradas_libres_contiguas(entrada_nueva->cantidad_entradas)){
		puntero_pagina = puntero + entrada_nueva->cantidad_entradas;
		return puntero;
	} else {
		return -1; //para agregar los nuevos valores empieza a recorrer desde le cero (hay que compactar)
	}
}

bool entradas_libres_contiguas(int necesarias){
	int contador = 0;
	int espacios_libres = 0;
	int puntero_pag = 0;
	while(contador != necesarias && puntero_pag <= configuracion_coordi.cantidad_entradas -1){ //Muevo el puntero hasta que encuentre las entradas contiguas o me pase
			if(acceso_tabla[puntero_pag] == 0){
				espacios_libres += 1;
				puntero_pag += 1;
				contador += 1;
			} else {
				puntero_pag += 1;
				contador = 0;
			}
		}
	if(contador == necesarias){
		return 1;
	} else {
		return 0;
	}
}

int aplicar_algoritmo_BSU(estructura_clave* entrada_nueva) {
	log_info(logger, "Entro al algoritmo del BSU");
	int entradas_necesarias = entrada_nueva->cantidad_entradas;
	int puntero = 0;

	while (entradas_necesarias > 0) {
		puntero = buscar_mayor_bsu_atomico(entradas_necesarias);
		estructura_clave* auxiliar = list_get(tabla_entradas, puntero);
		for(int i = 0; i < auxiliar->cantidad_entradas; i++){
			acceso_tabla[auxiliar->numero_pagina + i] = 0;
		}
		list_remove_and_destroy_element(tabla_entradas,puntero,borrar_entrada);

		puntero++;
		entradas_necesarias--;
		if (puntero == configuracion_coordi.cantidad_entradas) {
			puntero = 0;
		}
	}
	if(entradas_libres_contiguas(entrada_nueva->cantidad_entradas)){
		puntero_pagina = puntero + entrada_nueva->cantidad_entradas;
		return puntero;
	} else {
		return -1; //para agregar los nuevos valores empieza a recorrer desde le cero (hay que compactar)
	}
}


int implementar_algoritmo(estructura_clave* entrada_nueva, int entradas_contiguas_necesarias) {
	int respuesta;
	if (cantidad_entradas_atomicas() < entradas_contiguas_necesarias) {
		log_error(logger, "No hay lugares atomicos para almacenar");
		return -1;
	} else {
		respuesta = usar_algoritmo(entrada_nueva);
		if (respuesta == -1) {
			enviar_pedido_compactacion();
			int resultado = recibir_orden_compactacion();
			while (resultado != 3) {
				resultado = recibir_orden_compactacion();
			}
			compactar();
			respuesta = entradas_atomicas_vacias_contiguas(entradas_contiguas_necesarias) + entradas_contiguas_necesarias;
		}
	  return -1;
	}
}

int usar_algoritmo(estructura_clave* entrada_nueva){
	int resultado;
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
	log_info(logger, "Compactareeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
	log_info(logger, "Entre a compactar");

	//Declaro nuevas estructuras
	char* memoria_nueva = malloc(configuracion_coordi.cantidad_entradas * configuracion_coordi.tamano_entrada);
	int* nuevo_acceso_tabla = malloc(configuracion_coordi.cantidad_entradas * sizeof(int));
	int proxima_entrada = 0;
	int proxima_pagina = 0;

	//Inicializo nuevo bitmap
	for(int h = 0; h < configuracion_coordi.cantidad_entradas; h++){
		nuevo_acceso_tabla[h] = 0;
	}

	/*log_info(logger, "*********************");
	for(int h = 0; h < configuracion_coordi.cantidad_entradas; h++){
			log_info(logger, "El nuevo bitmap en %d en %d", h, nuevo_acceso_tabla[h]);
		}
	log_info(logger, "*********************");*/

	//Si tengo algo en la lista entonces esta ocupado, lo paso a la memoria nueva
	for(int i = 0; i < list_size(tabla_entradas); i++){
		log_info(logger, "El tamanio dado por el coordi es: %d", configuracion_coordi.tamano_entrada);
		log_info(logger, "El comienzo de la nueva memoria es: %d y la proxima pagins es: %d", memoria_nueva, proxima_pagina);
		estructura_clave* nodo = list_get(tabla_entradas, i);
		char* nueva_direccion = memoria_nueva + (proxima_pagina * configuracion_coordi.tamano_entrada);
		log_info(logger, "La variable nueva direccion es: %d", nueva_direccion);
		memcpy(nueva_direccion, nodo->valor, nodo->tamanio_valor);
		nodo->valor = nueva_direccion;
		log_info(logger, "La nueva direccion es: %d", (proxima_entrada * configuracion_coordi.tamano_entrada) + memoria_nueva);
		nodo->numero_pagina = proxima_pagina;
		log_info(logger, "La entrada %d tiene %d paginas", i, nodo->cantidad_entradas);

		//Actualizo el nuevo bitmap
		for(int j = 0; j < nodo->cantidad_entradas; j++){
				nuevo_acceso_tabla[proxima_pagina + j] = 1;
			}

		proxima_pagina += nodo->cantidad_entradas;
	}

	log_info(logger, "*********************");
	for(int h = 0; h < configuracion_coordi.cantidad_entradas; h++){
			log_info(logger, "El nuevo bitmap en %d es %d", h, nuevo_acceso_tabla[h]);
		}
	log_info(logger, "*********************");

	//Libero espacio anterior e igualo al nuevo
	free(inicio_memoria);
	free(acceso_tabla);
	inicio_memoria = memoria_nueva;
	acceso_tabla = nuevo_acceso_tabla;



	/*log_info(logger, "Quiso compactar");
	int* tabla_auxiliar;
	int proxima_entrada_ocupada;
	int puntero_auxiliar = 0;
	cantidad_entradas = list_size(tabla_entradas);
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
	free(memoria_nueva);*/

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
	recibir(socket_coordinador, buffer, sizeof(int), logger);
	id = deserializar_id(buffer);
	return id;
}


void guardar_archivo(char* clave, int tamanio_clave, t_log* logger){

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
				//log_info(logger, "La clave es: %s", clave);
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
				//log_info(logger, "Voy a guardar algo de tamanio %d: %s", tamanio_valor, entrada_encontrada->valor);
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


