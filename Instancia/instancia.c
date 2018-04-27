#include <Commons_propias/commons_propias.h>

char* ipCoordinador;
int puertoCoordinador;
//algoritmoReemplazo; CIRC LRU o BSU;
//puntoDeMontaje; //path absoluto ???
char* nombreInstancia;
int intervaloDump;

int main() {


//seria como inicializar la instancia
	int socket = connect_to_server("127.0.0.1", "8081");

	Mensaje mensaje = malloc(sizeof(struct Mensaje));
	int cantidad_bytes = recv(socket, mensaje, sizeof(Mensaje), 0); //recibe el mensaje del coordinador
// se usa el sizeof (Mensaje) por que quiere saber todo el valor de la struct
//El recv guarda la cantidad de bytes no el mensaje por lo tanto este se va a guardar en el "buffer" mensaje que creamos
//probar que la cant de bytes sea mayor a cero

	void procesar_mensaje(Mensaje mensaje) {

		while (1) {

			char * instruccion = strcpy(mensaje->instruccion, instruccion); //del mensaje obtenemos la instruccion
			//como sacamos la clavee de la instruccion???? vamos a tener que armar otro tipo de struct que separe la instruccion de la clave
			/*
			 acceder_a_entrada(clave); //ingresa a la tabla de entradas segun la clave que se manda por el mensaje
			 int respuesta;
			 //no vamos a necesitar un switch porque la instancia solo lee o guarda es decir GET y SET

			 if (instruccion == "GET") {
			 respuesta = read_archivo();
			 } else {
			 respuesta = guardar_archivo();
			 }
			 send_mensaje(socket, respuesta);
			 //aca supuestamente le avisa al coordinador como salio pero no se si la respuesta va como el numero directo o habria que armarla en un protocolo
			 */
		}
	}
