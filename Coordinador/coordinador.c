#include "coordinador.h"
#include "coordinador_funciones.h"

char* puerto_escucha;
tipo_algoritmo algoritmo_distribucion;//LSU EL o KE
int cantidad_entradas;
int tamano_entrada;
int retardo;
FILE* configuracion;
char* ip;
char* puerto;
t_conexion conexionInstancia;

void inicializar_coordinador(){
	leer_archivo_configuracion();
	inicializar_servidor(ip, puerto,logger);
}

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	configuracion = fopen("Configuracion coordinador.txt", "r");
	fscanf(configuracion, "%s %s %d %d %d %d", &ip, &puerto_escucha, &algoritmo_distribucion , &cantidad_entradas, &tamano_entrada, &retardo);
	fclose(configuracion);
}

void enviar_configuracion_instancia(){
	datos_configuracion mensaje = {tamano_entrada, cantidad_entradas};
	//send_content(int socket, void * content, 1);
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

t_config* configuracion=config_create("/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Config");

strcpy(conexion.ip,config_get_string_value(configuracion,"IP_COORDINADOR"));

strcpy(conexion.puerto,config_get_string_value(configuracion,"PUERTO_COORDINADOR"));

logger = log_create("coordinador.log", "COORDINADOR", true, LOG_LEVEL_INFO);



inicializar_servidor(conexion.ip,conexion.puerto,logger);
return 0;

}
