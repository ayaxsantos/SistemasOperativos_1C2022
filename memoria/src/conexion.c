#include "../include/conexion.h"

void *gestionar_conexion(void *arg) {
    int socket_cliente = *((int *)arg);
    mate_instance request_carpincho;
    while (true) {

		int cod_op = recibir_operacion(socket_cliente);

		switch (cod_op) {
            case MATE_INIT:
                request_carpincho = recibir_mate_instance(socket_cliente);
                iniciar_carpincho(&request_carpincho, socket_cliente);
                break;
            case MATE_CLOSE:
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria,"Me llego un mate close");
                pthread_mutex_unlock(&mutex_logger);
                gestionar_mate_close(socket_cliente);
                break;
            case MEM_ALLOC:
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria, "Me llego un mate alloc.");
                pthread_mutex_unlock(&mutex_logger);
                gestionar_mem_alloc(socket_cliente);
                break;
            case MEM_FREE:
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria, "Me llego un mate free.");
                pthread_mutex_unlock(&mutex_logger);
                gestionar_mem_free(socket_cliente);
                break;
            case MEM_READ:
            	pthread_mutex_lock(&mutex_logger);
            	log_info(logger_memoria,"Me llego un mem read.");
            	pthread_mutex_unlock(&mutex_logger);
            	gestionar_mem_read(socket_cliente);
                break;
            case MEM_WRITE:
            	pthread_mutex_lock(&mutex_logger);
            	log_info(logger_memoria,"Me llego un mem write.");
            	pthread_mutex_unlock(&mutex_logger);
            	gestionar_mem_write(socket_cliente);
                break;
            case -1:
                pthread_mutex_lock(&mutex_logger);
                log_error(logger_memoria, "El cliente se desconecto. Terminando Hilo.");
                pthread_mutex_unlock(&mutex_logger);
                return EXIT_FAILURE;
            default:
                pthread_mutex_lock(&mutex_logger);
                log_warning(logger_memoria, "Operacion desconocida.");
                pthread_mutex_unlock(&mutex_logger);
                break;
		}
    }
}

void esperar_handshake_kernel(int server) {
    log_info(logger_memoria,"Iniciando handshake con modulo KERNEL ... ");
    socket_cpu = esperar_cliente(server);
    int resultado = esperar_handshake(&socket_cpu, validar_modulo);
    if(resultado == -1) {
        log_error(logger_memoria,"No se pudo conectar con el modulo MEMORIA");
        exit(EXIT_FAILURE);
    }
    log_info(logger_memoria,"MEMORIA Conectada");
}

void esperar_handshake_cpu(int server) {
    log_info(logger_memoria,"Iniciando handshake con modulo CPU ... ");
    socket_cpu = esperar_cliente(server);
    int resultado = esperar_handshake(&socket_cpu, validar_modulo);
    if(resultado == -1) {
        log_error(logger_memoria,"No se pudo conectar con el modulo MEMORIA");
        exit(EXIT_FAILURE);
    }
    log_info(logger_memoria,"MEMORIA Conectada");
}

void validar_modulo(int *socket, modulo modulo_solicitante) {
    if(modulo_solicitante == CPU) {
        void *buffer = malloc(sizeof(int)*4);
        codigo_operacion handshake = HANDSHAKE;
        int modulo_actual = MEMORIA;
        int desplazamiento = 0;
        memcpy(buffer, &handshake, sizeof(int));
        desplazamiento += sizeof(int);
        memcpy(buffer + desplazamiento, &modulo_actual, sizeof(int));
        desplazamiento += sizeof(int);
        memcpy(buffer + desplazamiento, &(config_memoria.entradas_por_tabla), sizeof(int));
        desplazamiento += sizeof(int);
        memcpy(buffer + desplazamiento, &(config_memoria.tamanio_pagina), sizeof(int));

        send(*socket, buffer, sizeof(int)*4, 0);
        log_info(logger_memoria,"CPU Conectada");
        free(buffer);
        return;
    }
    log_error(logger_memoria,"Error de handshake con CPU");
}

void iniciar_proceso(int socket) {
	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal();
    list_add(tablas_primer_nivel, tabla_principal_del_proceso);
    int index_tabla = list_size(tablas_primer_nivel)-1;

    // TODO Agregar index al paquete que devuelvo
    // t_funcion *funcion = crear_funcion(INICIAR);
	// setear_funcion(funcion,index_tabla);
	// enviar_funcion(funcion,socket_cliente);
	// eliminar_funcion(funcion);

    crear_archivo(index_tabla, tabla_principal_del_proceso->tamanio_proceso);
}

void terminar_proceso(int socket) {
	int nada;
	void *buffer = recibir_buffer(&nada,socket);
	int index_tabla = socket;		// TODO Obtener id de tabla
	t_tabla_pagina* tabla_1n = list_get(tablas_primer_nivel, index_tabla);
	// liberar_todas_las_paginas(tabla_1n);
	free(buffer);
}

unsigned int get_thread_id() {
    return syscall(SYS_gettid);
}
