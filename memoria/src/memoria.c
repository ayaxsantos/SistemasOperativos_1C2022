#include "../include/memoria.h"
#include "conexion.h"

void iniciar_memoria() {
    tablas_primer_nivel = list_create();
    int server = iniciar_servidor(config_memoria.ip_memoria,config_memoria.puerto_escucha);
    setear_estructuras_de_memoria();
	esperar_handshake_cpu(server);
    esperar_handshake_kernel(server);
	log_info(logger_memoria,"Memoria a la espera de conexiones ...");
    pthread_t *hilo_cpu = malloc(sizeof(pthread_t));
    pthread_create(hilo_cpu, NULL, &gestionar_conexion_cpu, NULL);
    pthread_detach(*hilo_cpu);
    int *socket_cliente;
	while(true) {
        socket_cliente = malloc(sizeof(int));
        *socket_cliente = esperar_cliente(server);
		pthread_t *hilo_cliente =  malloc(sizeof(pthread_t));
        // Esto es para kernel
		pthread_create(hilo_cliente, NULL, &gestionar_conexion_kernel, socket_cliente);

		pthread_detach(*hilo_cliente);
	}
}

void iniciar_proceso(int socket_cliente) {
    uint32_t tamanio_proceso = recibir_entero(socket_cliente);
	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal((int )tamanio_proceso);
    //Mutex
    list_add(tablas_primer_nivel, tabla_principal_del_proceso);
    int index_tabla = list_size(tablas_primer_nivel)-1;
    //UnMutex

     t_operacion *operacion = crear_operacion(INICIO_PROCESO);
	 setear_operacion(operacion,&index_tabla);
	 enviar_operacion(operacion,socket_cliente);
	 eliminar_operacion(operacion);

     //TODO: Ver despues SWAP
     //crear_archivo(index_tabla, tabla_principal_del_proceso->tamanio_proceso);
}

void terminar_proceso(int socket_cliente) {
	int nada;
	void *buffer = recibir_buffer(&nada,socket_cliente);
	int index_tabla = socket_cliente;		// TODO Obtener id de tabla
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, index_tabla);
	// liberar_todas_las_paginas(tabla_1n);
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
