#include "../include/memoria.h"
#include "conexion.h"

void iniciar_memoria() {
    tablas_primer_nivel = list_create();
    int server = iniciar_servidor(config_memoria.ip_memoria,config_memoria.puerto_escucha);
    setear_estructuras_de_memoria();
	esperar_handshake_cpu(server);
    esperar_handshake_kernel(server);
    pthread_t *hilo_cpu = malloc(sizeof(pthread_t));
    pthread_create(hilo_cpu, NULL, &gestionar_conexion_cpu, NULL);
    pthread_detach(*hilo_cpu);
    int *socket_cliente;
    log_info(logger_memoria,"Memoria a la espera de conexiones con Kernel ...");
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
    t_dato_inicio *inicio_proceso = recibir_dato_inicio(socket_cliente);
	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal((int )inicio_proceso->tamanio_proceso);
    //Mutex
    list_add(tablas_primer_nivel, tabla_principal_del_proceso);
    int index_tabla = list_size(tablas_primer_nivel)-1;
    //UnMutex
    inicio_proceso->id_tabla_1n = index_tabla;

    t_operacion *operacion = crear_operacion(INICIO_PROCESO);
	setear_operacion(operacion,inicio_proceso);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);

	//crear_archivo(tabla_principal_del_proceso->id_tabla, inicio_proceso->tamanio_proceso);
}

void terminar_proceso(int socket_cliente) {
	uint32_t id_tabla = recibir_entero(socket_cliente);
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, id_tabla);

	liberar_todas_las_paginas_del_proceso(tabla_1n);

	t_operacion *operacion = crear_operacion(FIN_PROCESO);
	setear_operacion(operacion,&id_tabla);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);

	destruir_archivo(id_tabla);
}

void suspender_proceso(int socket_cliente) {
	uint32_t id = recibir_entero(socket_cliente);
	// t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, id);

	// swapear_tabla_completa(tabla_1n); TODO

	t_operacion *operacion = crear_operacion(SUSPENSION_PROCESO);
	setear_operacion(operacion,&id);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);
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
