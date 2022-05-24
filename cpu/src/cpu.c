#include "../include/cpu.h"

void iniciar() {
    pcbs = list_create();

    dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
    interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    conectar_a_memoria_y_recibir_config();
    esperar_a_kernel();
}

void esperar_a_kernel() {
    log_info(logger_cpu,"CPU a la espera de Kernel");

    pthread_t hilo_dispatch, hilo_interrupt;
    pthread_create(&hilo_dispatch, NULL, &ejecutar_pcb, NULL);
    pthread_create(&hilo_interrupt, NULL, &ejecutar_interrupcion, NULL);

    pthread_detach(hilo_dispatch);
    pthread_detach(hilo_interrupt);
}

void *ejecutar_pcb(void *arg) {
    int socket_dispatch = esperar_cliente(dispatch);
    /*
     * La cpu es una sola no?
     */
}

void *ejecutar_interrupcion(void *arg) {
    int socket_interrupt = esperar_cliente(interrupt);
}