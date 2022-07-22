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
		free(hilo_cliente);
	}
}

void iniciar_proceso(int socket_cliente) {
    t_dato_inicio *inicio_proceso = recibir_dato_inicio(socket_cliente);
    pthread_mutex_lock(&mutex_logger);
    log_info(logger_memoria,"Llego un INICIO_PROCESO -> PID: %d", inicio_proceso->pid);
    pthread_mutex_unlock(&mutex_logger);
	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal((int )inicio_proceso->tamanio_proceso);

	pthread_mutex_lock(&mutex_lista_tablas_paginas);
	list_add(tablas_primer_nivel, tabla_principal_del_proceso);
	pthread_mutex_unlock(&mutex_lista_tablas_paginas);

	inicio_proceso->id_tabla_1n = tabla_principal_del_proceso->id_tabla;

    t_operacion *operacion = crear_operacion(INICIO_PROCESO);
	setear_operacion(operacion,inicio_proceso);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);

	crear_archivo((int)inicio_proceso->pid, inicio_proceso->id_tabla_1n,inicio_proceso->tamanio_proceso);

    pthread_mutex_lock(&mutex_logger);
    log_info(logger_memoria,"Respondo con el id tabla primer nivel %d del PID: %d", inicio_proceso->id_tabla_1n,inicio_proceso->pid);
    pthread_mutex_unlock(&mutex_logger);
    free(inicio_proceso);
}

void terminar_proceso(int socket_cliente) {
	uint32_t id_tabla = recibir_entero(socket_cliente);
    pthread_mutex_lock(&mutex_logger);
    log_info(logger_memoria,"Llego un FIN_PROCESO para el proceso con id tabla primer nivel %d", id_tabla);
    pthread_mutex_unlock(&mutex_logger);
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, (int)id_tabla);

	liberar_tabla_principal(tabla_1n);

	t_operacion *operacion = crear_operacion(FIN_PROCESO_MEMORIA);
	setear_operacion(operacion,&id_tabla);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);

	destruir_archivo((int)id_tabla);
}

void suspender_proceso(int socket_cliente) {
	int32_t id_tabla_1n = (int32_t)recibir_entero(socket_cliente);
    pthread_mutex_lock(&mutex_logger);
    log_warning(logger_memoria,"Llego un SUSPENSION_PROCESO para el proceso con id tabla primer nivel %d", id_tabla_1n);
    pthread_mutex_unlock(&mutex_logger);

	pthread_mutex_lock(&mutex_lista_tablas_paginas);
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, id_tabla_1n);
    pthread_mutex_unlock(&mutex_lista_tablas_paginas);
    swapear_proceso(tabla_1n);

	t_operacion *operacion = crear_operacion(SUSPENSION_PROCESO);
	setear_operacion(operacion,&id_tabla_1n);
	enviar_operacion(operacion,socket_cliente);
	eliminar_operacion(operacion);
}
