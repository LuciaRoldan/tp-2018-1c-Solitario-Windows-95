#include <Commons_propias/commons_propias.h>

char* ipCoordinador;
int puertoCoordinador;
typedef enum {
	LRU, CIRC, BSU
} tipo_algoritmo;
tipo_algoritmo algoritmo_reemplazo;
//puntoDeMontaje; //path absoluto ???
char* nombreInstancia;
int intervaloDump;
int cantidad_entradas;
int tamano_entrada;
int matriz_memoria[cantidad_entradas][tamano_entrada];

typedef struct {
	int clave_fila;
	int clave_columna;
}__attribute__((packed)) clave;

typedef struct {
	char instruccion[];
	int clave;
}__attribute__((packed)) Mensaje_Instancia;

typedef struct {
	char instruccion[];
}__attribute__((packed))Mensaje_a_Coordinador;

int main() {

	void leer_archivo_configuracion() {
		configuracion = fopen("Configuracion instancia.txt", "r");
		fscanf(configuracion, "%s %s %s %d %d", &ipCoordinador,
				&puertoCoordinador, &algoritmo_reemplazo, &cantidad_entradas,
				&tamano_entrada);
		fclose(configuracion);
	}

	void inicializar_planificador() {
		leer_archivo_configuracion();
		int socket = connect_to_server(ipCoordinador, puertoCoordinador);
	}

	Mensaje mensaje = malloc(sizeof(struct Mensaje));
	int cantidad_bytes = recv(socket, mensaje, sizeof(Mensaje), 0); //recibe el mensaje del coordinador
// se usa el sizeof (Mensaje) por que quiere saber todo el valor de la struct
//El recv guarda la cantidad de bytes no el mensaje por lo tanto este se va a guardar en el "buffer" mensaje que creamos
//probar que la cant de bytes sea mayor a cero

	void procesar_mensaje(Mensaje_Instancia mensaje) {

		while (1) {

			char * instruccion = strcpy(mensaje->instruccion, instruccion); //del mensaje obtenemos la instruccion
			int * clave = strcpy(mensaje->clave, clave); //obtenemos la clave del mensaje

			acceder_entrada(clave); //ingresa a la tabla de entradas segun la clave que se manda por el mensaje
			int respuesta;
			//no vamos a necesitar un switch porque la instancia solo lee o guarda es decir GET y SET
			switch (instruccion) {
			case (instruccion == "GET"):
				respuesta = read_archivo(clave);
				break;
			case (instruccion == "SET"):
				respuesta = guardar_archivo();
				break;
			case (instruccion == "STORE"):
				respuesta = guardar_archivo_y_desbloquar();
				if(respuesta){
				respuesta = 2;
				}
				break;
			}

			enviar_mesnaje_coordinador(respuesta);
			//aca supuestamente le avisa al coordinador como salio pero no se si la respuesta va como el numero directo o habria que armarla en un protocolo
		}
	}

	int acceder_entrada(clave clave) {
		int * clave_fila = strcpy(clave->clave_fila, clave_fila);
		int * clave_columna = strcpy(clave->clave_columna, clave_columna);
		return matriz_memoria[clave_fila][clave_columna];
	}

	int read_archivo() {
		int entrada = acceder_entrada();
		return fopen(entrada); // si sale bien retorna 1 y si mal 0??
		fclose();
	}

	int guardar_archivo(int archivo) {
		int entrada = acceder_entrada();
		return entrada = archivo;
	}
	//donde viene el archivo??


	void enviar_mensaje_coordinador(int respuesta){

		switch(respuesta){
		case(0):
				Mensaje_a_Coordinador mensaje = {"Algo salio mal"};
		break;
		case(1):
				Mensaje_a_Coordinador mensaje = {"Todo OK"};
		break;
		case (2):
				Mensaje_a_Coordinador mensaje = {"desbloquearESI"};
		break;
		}
		send_mensaje(socket,mensaje);
	}

}

