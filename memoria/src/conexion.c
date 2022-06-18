#include "../include/conexion.h"

void *gestionar_conexion(void *arg) {
    int socket_cliente = *((int *)arg);
    while (true) {
		codigo_operacion cod_op = recibir_operacion(socket_cliente);
		switch (cod_op) {
            case READY:
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
    }*/
}

void *gestionar_conexion_cpu(void *arg) {
    while (true) {
        codigo_operacion cod_op = recibir_operacion(socket_cpu);
        switch (cod_op) {
            case PRIMERA_SOLICITUD:
                gestionar_primera_solicitud();
                break;
            case SEGUNDA_SOLICITUD:
                gestionar_segunda_solicitud();
                break;
            case TERCERA_SOLICITUD:
                gestionar_tercera_solicitud();
                break;
            case -1:
                pthread_mutex_lock(&mutex_logger);
                log_error(logger_memoria, "CPU se desconecto. Terminando Hilo.");
                pthread_mutex_unlock(&mutex_logger);
                return EXIT_FAILURE;
            default:
                pthread_mutex_lock(&mutex_logger);
                log_warning(logger_memoria, "Operacion desconocida de CPU.");
                pthread_mutex_unlock(&mutex_logger);
                break;
        }
    }
}
/*
 * Logica repetida, se puede mejorar @Jume, Vos podes
 */
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
    log_info(logger_memoria,"Esperando handshake con modulo CPU ... ");
    socket_cpu = esperar_cliente(server);
    int resultado = esperar_handshake(&socket_cpu, validar_modulo);
    if(resultado == -1) {
        log_error(logger_memoria,"No se pudo conectar con el modulo CPU");
        exit(EXIT_FAILURE);
    }
    log_info(logger_memoria,"CPU Conectada");
}

/*
 * Hasta aca
 */
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

void gestionar_primera_solicitud() {
    t_solicitud *request = recibir_solicitud(socket_cpu);
    //primera_solicitud_mmu(request);
    request->id_tabla_2n = 3;

    t_operacion *operacion = crear_operacion(PRIMERA_SOLICITUD);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_cpu);
    eliminar_operacion(operacion);
    free(request);
}

void gestionar_segunda_solicitud() {
    t_solicitud *request = recibir_solicitud(socket_cpu);
    //segunda_solicitud_mmu(request);
    request->nro_frame = 3;

    t_operacion *operacion = crear_operacion(SEGUNDA_SOLICITUD);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_cpu);
    eliminar_operacion(operacion);
    free(request);
}

void gestionar_tercera_solicitud() {
    t_tercera_solicitud *request = recibir_tercera_solicitud(socket_cpu);
    tercera_solicitud_mmu(request);
}
