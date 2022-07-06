#include "../include/conexion.h"

void *gestionar_conexion_kernel(void *arg) {
    int socket_cliente = *((int *)arg);
    while (true) {
		codigo_operacion cod_op = recibir_operacion(socket_cliente);
		switch (cod_op) {
            case INICIO_PROCESO:
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria,"Llego un INICIO_PROCESO");
                pthread_mutex_unlock(&mutex_logger);
                iniciar_proceso(socket_cliente);
                break;
            case FIN_PROCESO:
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria,"Llego un FIN_PROCESO");
                pthread_mutex_unlock(&mutex_logger);
                terminar_proceso(socket_cliente);
                break;
            case SUSPENSION_PROCESO:
				pthread_mutex_lock(&mutex_logger);
				log_info(logger_memoria,"Llego un SUSPENSION_PROCESO");
				pthread_mutex_unlock(&mutex_logger);
				suspender_proceso(socket_cliente);
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

void validar_modulo(int *socket, modulo modulo_solicitante) {
    modulo modulo_actual = MEMORIA;

    if(modulo_solicitante == CPU) {
        void *buffer = malloc(sizeof(int)*4);
        codigo_operacion handshake = HANDSHAKE;
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
    else if(modulo_solicitante == KERNEL)
    {
        log_info(logger_memoria,"KERNEL Conectado");
        responder_handshake(socket,modulo_actual);
    }
    else
        log_error(logger_memoria,"Error de handshake con %s",obtener_nombre_modulo(modulo_solicitante));
}

void responder_handshake(int *socket,modulo modulo_actual)
{
    void *buffer = malloc(sizeof(int)*2);
    codigo_operacion handshake = HANDSHAKE;
    memcpy(buffer, &handshake, sizeof(int));
    memcpy(buffer + sizeof(int), &modulo_actual, sizeof(int));
    send(*socket, buffer, sizeof(int)*2, 0);
}

void gestionar_primera_solicitud() {
    t_solicitud *request = recibir_solicitud(socket_cpu);

    primera_solicitud_mmu(request);

    t_operacion *operacion = crear_operacion(PRIMERA_SOLICITUD);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_cpu);
    eliminar_operacion(operacion);
    free(request);
}

void gestionar_segunda_solicitud() {
    t_solicitud *request = recibir_solicitud(socket_cpu);

    segunda_solicitud_mmu(request);

    t_operacion *operacion = crear_operacion(SEGUNDA_SOLICITUD);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_cpu);
    eliminar_operacion(operacion);
    free(request);
}

void gestionar_tercera_solicitud() {
    t_tercera_solicitud *request = recibir_tercera_solicitud(socket_cpu);

    tercera_solicitud_mmu(request);

    t_operacion *operacion = crear_operacion(TERCERA_SOLICITUD);
	setear_operacion(operacion, request);
	enviar_operacion(operacion, socket_cpu);
	eliminar_operacion(operacion);
	free(request);
}
