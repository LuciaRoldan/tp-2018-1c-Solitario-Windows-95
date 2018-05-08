#ifndef COORDINADOR_FUNCIONES_H_
#define COORDINADOR_FUNCIONES_H_
#include "coordinador.h"



info_archivo_config leer_archivo_configuracion(FILE* archivo){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	info_archivo_config datos_configuracion;
	archivo = fopen("Configuracion coordinador.txt", "r");
	fscanf(archivo, "%s %s %d %d %d %d", datos_configuracion.ip,
			datos_configuracion.puerto_escucha, datos_configuracion.algoritmo_distribucion ,
			datos_configuracion.cantidad_entradas, datos_configuracion.tamano_entrada,
			datos_configuracion.retardo);
	fclose(archivo);
	return datos_configuracion;
}


info_archivo_config inicializar_coordinador(FILE* archivo){
	info_archivo_config datos_configuracion = leer_archivo_configuracion(archivo);
	inicializar_servidor(datos_configuracion.ip, datos_configuracion.puerto_escucha,logger);
	return datos_configuracion;
}


void enviar_configuracion_instancia(info_archivo_config configuracion){
	datos_configuracion mensaje = {*configuracion.tamano_entrada, *configuracion.cantidad_entradas};
	send_content(4, &mensaje, 1); //Que socket le pongo??
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

#endif /* COORDINADOR_FUNCIONES_H_ */
