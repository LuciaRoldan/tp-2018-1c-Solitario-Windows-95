#include "Funciones_coordinador.h"

///////////////////////////////////////////////// INICIALIZACION /////////////////////////////////////////////////

void leer_archivo_configuracion(){
	//Supongo que en el archivo el orden es: ip, puerto, algoritmo, entradas, tamaño y retardo
	FILE* archivo = fopen("Configuracion_coordinador.txt", "r");

	fscanf(archivo, "%s %d %d %d %d %d",
			info_coordinador.ip,
			&(info_coordinador.puerto_escucha),
			&(info_coordinador.algoritmo_distribucion),
			&(info_coordinador.cantidad_entradas),
			&(info_coordinador.tamano_entrada),
			&(info_coordinador.retardo));
	fclose(archivo);
}

void inicializar_coordinador(){
	socket_escucha = inicializar_servidor(info_coordinador.puerto_escucha, logger);
	lista_esis = list_create();
	lista_instancias = list_create();
	lista_claves = list_create();
	ultima_instancia_EL = 0;
	terminar_programa = 0;
	conexion_hecha = 1;
	inicializar_semaforos();
}

void inicializar_semaforos(){
	if (pthread_mutex_init(&m_operacion_ejecutando, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_esi_ejecutando, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_instancia_seleccionada, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_hilo_a_cerrar, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_ultima_instancia_EL, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_claves, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_socket_instancia_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_socket_esi_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_instancias, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_lista_esis, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_log_operaciones, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_clave_buscada, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	if (pthread_mutex_init(&m_id_instancia_buscado, NULL) != 0) {printf("Fallo al inicializar mutex\n");}
	sem_init(&s_cerrar_hilo, 0, 0); //El primer 0 es para compartir solamente con mis hilos y el segundo es el valor
}

void conectar_planificador(){
	int socket_cliente = aceptar_conexion(socket_escucha);
	int protocolo;

	recibir(socket_cliente, &protocolo, sizeof(int), logger);
	if(protocolo == 80){
		int resultado = handshake(socket_cliente);
		if(resultado >= 0){
			socket_planificador = socket_cliente;
			log_info(logger, "Se establecio la conexion con el Planificdor");
		} else{
			log_error(logger, "Fallo en la conexion con el Planificdor");
		}
	} else{
		log_error(logger, "Fallo en la conexion con el Planificdor");
	}
}


