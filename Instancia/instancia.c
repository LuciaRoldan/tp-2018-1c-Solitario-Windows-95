#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <Commons_propias/commons_propias.h>
#include "instancia.h"

char* ipCoordinador;
int puertoCoordinador;


tipo_algoritmo algoritmo_reemplazo;

//puntoDeMontaje; //path absoluto ???

char* nombreInstancia;
int intervaloDump;
int cantidad_entradas;
int tamano_entrada;
int matriz_memoria[cantidad_entradas][3];
int espacio_para_memoria;
char* memoria;
FILE *archivo;

//-------------structs------------------//

typedef struct {
	char instruccion[];
	uint32_t intruccion_long;
	int clave;
	uint32_t clave_long;
}__attribute__((packed)) Mensaje_tipo1;

typedef struct {
	char instruccion[];
	uint32_t instruccon_long;
	int clave;
	uint32_t clave_long;
	int value;
	uint32_t value_long;
}__attribute__((packed)) Mensaje_tipo2;

typedef struct {
	char instruccion[];
}__attribute__((packed)) Mensaje_tipo3;

//hay que hacer una variable char que es todo el espacio de memoria que le hago un malloc para guardar todo ese espacio para ella
//y que lo obtengo a partir del archivo de configuracion que me da el coordinador
int main() {

	void inicializar_instancia() {
		int socket = connect_to_server(ipCoordinador, puertoCoordinador);
		void *configuracion = wait_content(socket);
		leer_archivo_configuracion(configuracion);
		memoria = malloc(espacio_para_memoria);
	}

	//buscar como hacer el recv a header

	void leer_archivo_configuracion(configuracion) {
		fscanf(configuracion, "%s %s %s %d %d %d", &ipCoordinador,
				&puertoCoordinador, &algoritmo_reemplazo, &cantidad_entradas,
				&tamano_entrada, &espacio_para_memoria);
		fclose(configuracion);
	}

	void recibir_mensaje(){

	}

	ContentHeader header = malloc(sizeof(struct ContentHeader));

	int cantidad_bytes_header = recv(socket, header, sizeof(ContentHeader), 0); //tengo que hacer un recv para recibir el header y conocer el tipo de mensaje

	void procesar_mensaje(ContentHeader header) {
		char buffer = malloc(sizeof(buffer));
		int id = wait_content(socket, &buffer); //recibe el header y el mensaje queda guardado en el buffer
		char * instruccion;
		int * clave;
		Mensaje_tipo2 mensaje_tipo2 = malloc(sizeof(struct Mensaje_tipo2));

		if (id == 1) {
			Mensaje_tipo1 mensaje_tipo1 = malloc(sizeof(struct Mensaje_tipo1));
			mensaje_tipo1 = *buffer; //no se si esto esta bien

			instruccion = strcpy(mensaje_tipo1->instruccion, instruccion); //del mensaje obtenemos la instruccion
			clave = strcpy(mensaje_tipo1->clave, clave); //obtenemos la clave del mensaje

		} else {
			mensaje_tipo2 = malloc(sizeof(struct Mensaje_tipo2));
			mensaje_tipo2 = *buffer;

			instruccion = strcpy(mensaje_tipo2->instruccion, instruccion); //del mensaje obtenemos la instruccion
			clave = strcpy(mensaje_tipo2->clave, clave); //obtenemos la clave del mensaje
		}

		while (1) {

			switch (instruccion) {
			case ("GET"):

				void bloquear_clave(clave);

			    Mensaje_tipo3 buffer_info = malloc(sizeof(struct Mensaje_tipo3));
				int direccion = obtener_direccion(clave);
				memcpy(buffer_info, *direccion, sizeof(buffer_info)); //obtiene la info a leer

				void enviar_info_coordinador(buffer_info);

				break;
			case ("SET"):

				int * value = strcpy(mensaje_tipo2->value, value);
				void guardar_archivo( value);

				break;
			case ("STORE"):

				int direccion = obtener_direccion(clave);
				char informacion[];

				memcpy(informacion,*direccion, sizeof(informacion));

				archivo = fopen("informacion.txt","w");
				fwrite(&informacion, sizeof(informacion),1,archivo);
				fclose(archivo);

				void desbloquear_clave(clave);

				break;
			}
		}

	}

// se usa el sizeof (Mensaje) por que quiere saber todo el valor de la struct
//El recv guarda la cantidad de bytes no el mensaje por lo tanto este se va a guardar en el "buffer" mensaje que creamos

	int obtener_direccion(int clave) {
		int i, direccion;
		while (i != clave) {
			direccion = matriz_memoria[i][2];
			i++;
		}
		return direccion;
	}

	void bloquear_clave(int clave) {

		ContentHeader header = (1, sizeof(Mensaje_tipo1));
		send(socket, header, sizeof(header), 0);

		Mensaje_tipo1 mensaje = { "Bloquear_clave", clave };
		send_mensaje(socket, mensaje);
	}

	void desbloquear_clave(int clave) {

		ContentHeader header = (1, sizeof(Mensaje_tipo1));
		send(socket, header, sizeof(header), 0);

		Mensaje_tipo1 mensaje = { "Desbloquear_clave", clave };
		send_mensaje(socket, mensaje);
	}

	void enviar_coordinador(Mensaje_tipo3 informacion) {

		send_content(socket, *informacion, sizeof(Mensaje_tipo3));
	}


	void guardar_archivo(int clave, int * value) {

		int direccion = obtener_direccion(clave);
		memcopy(*direccion, value, sizeof(value));
	}


}
