#include <Commons_propias/commons_propias.h>

char* ipCoordinador;
int puertoCoordinador;
//algoritmoReemplazo; CIRC LRU o BSU;
//puntoDeMontaje; //path absoluto ???
char* nombreInstancia;
int intervaloDump;

int main() {

connect_to_server("127.0.0.1","8081");

return 0;
	/*
	int puertoDeLlegada, PUERTO_ESI, PUERTO_COORDINADOR;

	//seria como inicializar la instancia
	int socket = connect_to_server("127.0.0.1", "8000"); //preguntar como hacer que lo traiga de nuestras commons

	char* buffer = malloc(sizeof(struct Mensaje));
	struct Mensaje mensaje = recv(socket, buffer, strlen(buffer), 0); //recibe el mensaje del coordinador

	void procesar_mensaje(Mensaje mensaje) {

		while (1) {

			char * instruccion = strcpy(mensaje->instruccion, instruccion); //del mensaje obtenemos la instruccion
			//como sacamos la clavee de la instruccion????
			int respuesta;
			acceder_a_entrada(instruccion); //ingresa a la tabla de entradas segun la clave que se manda por el mensaje
			switch (instruccion) { // la instruccion puede ser GET,SET o STORE
			case (1): //con el 1 = GET la instancia bloquea la clave de la entrada
				bloquearClave();
				break;
			case (2): // 2 = SET guarda la info
				almacenar_infor(); //
				break;
			case (3): // 3 = STORE guarda la info y desbloquea clave de la entrada
				almacenar_info();
				desbloquearClave();
				break;
			}
			enviar_resultado_coordinador();
		}
	}

	/*	void send_content;
	 void contestar_solicitud;

	 void recibir_configuracion_inicial;
	 void inicializar_instancia;

	 void bloquear_clave();

	 void enviar_resultado_coordinador();
	 */

}
