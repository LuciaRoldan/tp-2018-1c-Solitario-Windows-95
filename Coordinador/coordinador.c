#include "commons_propias.h"

typedef enum {LSU, EL, KE}tipo_algoritmo; //Va en el .h


char* puerto_escucha;
tipo_algoritmo algoritmo_distribucion;//LSU EL o KE
int cantidad_entradas;
int tamano_entrada;
int retardo;
FILE* configuracion;
char* ip;
char* puerto;


void inicializar_coordinador(){
	leer_archivo_configuracion();
	inicializar_servidor(ip, puerto);
}

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	configuracion = fopen("Configuracion coordinador.txt", "r");
	fscanf(configuracion, "%s %s %d %d %d %d", &ip, &puerto_escucha, &algoritmo_distribucion , &cantidad_entradas, &tamano_entrada, &retardo);
	fclose(configuracion);
}

void enviar_configuracion_instancia(){
	//send_content(int socket, void * content)
}


/*int atender_conexion_esi(clave){ //recibe la solicitud del esi con la clave que quiere
	//acceder
	aceptar_solicitud_esi;
	procesar_solicitud_esi;
	enviar_solicitud_instancia;
	logear_respuesta;
	informar_respuesta_planificador;
	return respuesta;
}
mensaje procesar solicitud esi{
	solicitud = recibir(mensaje);

	if(!chequear_uso_de_recursos(key)) //chequea con el planificador si se esta usando la instancia
	{
		return fallo;
	}

	instancia = buscar_instancia(solicitud);
	resultadoInstancia = solicitar_uso(instancia);
	informarUsoInstancia(instancia, esi);
	return mensaje;
}
*/

/*
void inicializar_coordinador;
void leer_archivo_configuracion;
void enviar_configuracion_instancia;
void configurar_planificador;
void esperar_contenido;
void atender_conexion_esi;
*/
int main(){
	inicializar_cooridnador();


return 0;

}
