#ifndef INSTANCIA_FUNCIONES_H_
#define INSTANCIA_FUNCIONES_H_
#include "instancia.h"

/////////////////////// INICIALIZACION ///////////////////////

	void inicializar_instancia() {
		int socket = connect_to_server(ipCoordinador, puertoCoordinador);
		datos_configuracion configuracion = recibir_configuracion();
		leer_archivo_configuracion(configuracion);
		memoria = malloc(espacio_para_memoria);
	}

	datos_configuracion recibir_configuracion(){
		datos_configuracion configuracion;
		recibir(socket, &configuracion , sizeof(datos_configuracion), logger);
		return configuracion;
		//deserializar?
	}

	void leer_configuracion_propia(){
		fscanf(archivo_configuracion, "%d %s %d", &ipCoordinador,&puertoCoordinador,&nombreInstancia,&intervaloDump);
		fclose(archivo_configuracion);
	}

	void leer_archivo_configuracion() {
		fscanf(configuracion, "%d %d", &cantidad_entradas,
				&tamano_entrada);
		fclose(configuracion);
	}

	t_esi_operacion recibir_instruccion(){
		t_esi_operacion instruccion;
		recibir(socket, &instruccion , sizeof(t_esi_operacion), logger);
		return instruccion;
		//deserializar?
	}

void procesar_instruccion(t_esi_operacion instruccion) {

	switch (instruccion) {
	case (GET):
		break;
	case (SET):
		char* value = strcpy(instruccion->_raw, value); //no se si me devuelve solo el value
		void guardar_archivo(value);
		break;
	case (STORE):
		char* clave = strcpy(instruccion->_raw, clave);
		int direccion = obtener_direccion(clave);
		char informacion[];

		memcpy(informacion, *direccion, sizeof(informacion));

		archivo = fopen("informacion.txt", "w");
		fwrite(&informacion, sizeof(informacion), 1, archivo);
		fclose (archivo);

		void desbloquear_clave(clave);

		break;
	}
}



	void enviar_a_desbloquear_clave(int clave) {
		enviar(socket, clave, sizeof(clave), 03, logger);
		//serializar?
	}

	void guardar_archivo(int clave, int * value) {

		int direccion = obtener_direccion(clave);
		memcopy(*direccion, value, sizeof(value));
	}


#endif /* INSTANCIA_FUNCIONES_H_ */
