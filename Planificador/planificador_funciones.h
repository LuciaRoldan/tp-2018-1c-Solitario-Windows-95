#include "planificador.h"

#ifndef PLANIFICADOR_FUNCIONES_H_
#define PLANIFICADOR_FUNCIONES_H_

void inicializar_planificador(int* socketCoordinador, int* socketEsis);
void leer_archivo_configuracion();
int conectarse_al_coordinador();
void manejar_esi();
void procesar_exito_o_error(int resultado_esi);
void manejar_coordinador();



t_conexion conexion_planificador;
t_conexion conexion_coordinador;

#endif /* PLANIFICADOR_FUNCIONES_H_ */
