#include "../include/memoria.h"

void iniciar_memoria() {
    tablas_primer_nivel = list_create();
    // int server = iniciar_servidor(config_memoria.puerto_escucha);
    setear_estructuras_de_memoria();

	/*log_info(logger_memoria,"Memoria a la espera de conexiones ...");

	int socket_cliente;
	while(true) {
		socket_cliente = esperar_cliente(server);
		pthread_t hilo;
		pthread_create(&hilo, NULL, &gestionar_conexion, &socket_cliente);
		pthread_detach(hilo);
	}*/
}

void iniciar_proceso(int socket_cliente) {
	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal();
    list_add(tablas_primer_nivel, tabla_principal_del_proceso);
    int index_tabla = list_size(tablas_primer_nivel)-1;

    // TODO Agregar index al paquete que devuelvo
    // t_funcion *funcion = crear_funcion(INICIAR);
	// setear_funcion(funcion,index_tabla);
	// enviar_funcion(funcion,socket_cliente);
	// eliminar_funcion(funcion);
}

void terminar_proceso(int socket_cliente) {
	int nada;
	void *buffer = recibir_buffer(&nada,socket_cliente);
	int index_tabla = socket_cliente;		// TODO Obtener id de tabla
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, index_tabla);
	liberar_todas_las_paginas(tabla_1n);
	free(buffer);
}

void gestionar_suspension(int socket_cliente) {
	// TODO Completar
}

void gestionar_acceso(int socket_cliente) {
	// TODO Completar
}

void gestionar_lectura(int socket_cliente){
	// TODO Completar
}

void gestionar_escritura(int socket_cliente){
	// TODO Completar
}

void leer_dato() {
	// TODO Completar
}

void escribir_dato() {
	// TODO Completar
}
