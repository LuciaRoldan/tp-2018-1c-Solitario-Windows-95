#ifndef INSTANCIA_FUNCIONES_H_
#define INSTANCIA_FUNCIONES_H_
#include "instancia.h"

/////////////////////// INICIALIZACION ///////////////////////

	void inicializar_instancia() {
		datos_configuracion configuracion = recibir_configuracion();
		leer_archivo_configuracion(configuracion);
		memoria = malloc(espacio_para_memoria);
	}

	datos_configuracion recibir_configuracion(int* socket_coordinador) {
		datos_configuracion configuracion;
		recibir(socket_coordinador, &configuracion, sizeof(datos_configuracion), logger);
		return configuracion;
		//deserializar?
	}

	void leer_configuracion_propia() {
		fscanf(archivo_configuracion, "%d %s %d", &ipCoordinador,
				&puertoCoordinador, &nombreInstancia, &intervaloDump);
		fclose (archivo_configuracion);
	}

	void leer_archivo_configuracion() {
		fscanf(configuracion, "%d %d", &cantidad_entradas, &tamano_entrada);
		fclose (configuracion);
	}

	t_esi_operacion recibir_instruccion(int* socket_coordinador) {
		t_esi_operacion instruccion;
		recibir(socket_coordinador, &instruccion, sizeof(t_esi_operacion), logger);
		return instruccion;
		//deserializar?
	}

	void procesar_instruccion(int* socket_coordinador, t_esi_operacion instruccion) {

		switch (instruccion) {
		case (GET):
			break;
		case (SET):
			char* value = strcpy(instruccion->_raw, value); //no se si me esta devolviendo lo que quiero
			char* clave = strcpy(instruccion->_raw, value);
 			void guardar_archivo(clave,value);
			break;
		case (STORE):
			char* clave = strcpy(instruccion->_raw, clave);
			int direccion = obtener_direccion(clave);
			char informacion[];

			memcpy(informacion, *direccion, sizeof(informacion));

			archivo = fopen("informacion.txt", "w");
			fwrite(&informacion, sizeof(informacion), 1, archivo);
			fclose (archivo);

			void enviar_a_desbloquear_clave(socket_coordinador, clave);

			break;
		}
	}

	void enviar_a_desbloquear_clave(int* socket_coordinador, int clave) {
		enviar(socket_coordinador, clave, sizeof(clave), 03, logger);
		//serializar?
	}

	void guardar_archivo(int clave, int * value) {

		int direccion = obtener_direccion(clave);
		memcopy(*direccion, value, sizeof(value));
	}

	int handshake(int* socket_coordinador, t_log* logger) {
		int conexion_hecha = 0;

		t_handshake proceso_recibido;
		t_handshake yo = { INSTANCIA, 0 };
		int id_recibido;

		enviar(socket_coordinador, &yo, sizeof(t_handshake), 80, logger);
		recibir(socket_coordinador, &id_recibido, sizeof(int), logger);

		if (id_recibido != 80) {
			log_info(logger, "Conexion invalida");
			return -1;
		}

		recibir(socket_coordinador, &proceso_recibido, sizeof(t_handshake), logger);

		if (!conexion_hecha) {
			conexion_hecha = 1;
			return 1;
		} else {
			return -1;
		}

	}

#endif /* INSTANCIA_FUNCIONES_H_ */
