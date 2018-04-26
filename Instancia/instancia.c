#include "commons_propias.h"

char* ipCoordinador;
int puertoCoordinador;
//algoritmoReemplazo; CIRC LRU o BSU;
//puntoDeMontaje; //path absoluto ???
char* nombreInstancia;
int intervaloDump;

int main() {

	int puertoDeLlegada, PUERTO_ESI, PUERTO_COORDINADOR;

	int socket = connect_to_server("127.0.0.1", "8000"); //preguntar como hacer que lo traiga de nuestras commons
/*
    char* buffer = malloc(sizeof(Mensaje));
	struct Mensaje mensaje = recv(socket, buffer, strlen(buffer), 0); //recibe el mensaje del coordinador

	void procesar_mensaje(Mensaje mensaje) {

		while (1) {

			char * instruccion = strcpy(mensaje->intruccion,instruccion); //del mensaje obtenemos la instruccion
			int clave = mensaje->clave;
			int respuesta;
			acceder_a_entrada(clave); //ingresa a la tabla de entradas segun la clave que se manda por el mensaje
			switch (instruccion) { // la instruccion puede ser GET,SET o STORE
			case (GET): //con el get la instancia bloquea la clave de la entrada
				bloquearClave();
				return 1; //si todo sale bien retorna un 1 sino 0( no se como verifica que salga bien)
				break;
			case (SET): //guarda la info
				almacenar_infor(); //
				return 1; //exito
				break;
			case (STORE): //guarda la info y desbloquea clave de la entrada
				almacenar_info();
				desbloquearClave();
				return 1;
				break;
			}
			enviar_resultado_coordinador ();
		}
*/
	}

	void send_content;
	void contestar_solicitud;

	void recibir_configuracion_inicial;
	void inicializar_instancia;

	void bloquear_clave();

	void procesar_mensaje( mensaje);
	void enviar_resultado_coordinador();
}

