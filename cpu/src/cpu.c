#include "../include/cpu.h"

void iniciar() {
    pcbs = list_create();

    int dispatch = iniciar_servidor("127.0.0.1", "25657");
    int interrupt = iniciar_servidor("127.0.0.1", "25656");
    conectar_a_memoria_y_recibir_config();
    esperar_a_kernel(dispatch, interrupt);
    /*
    recibir_procesos();*/
}

void esperar_a_kernel(int dispatch, int interrupt) {
    log_info(logger_cpu,"CPU a la espera de Kernel");

    pthread_t hilo_dispatch, hilo_interrupt;
    pthread_create(&hilo_dispatch, NULL, &ejecutar_pcb, &dispatch);
    pthread_create(&hilo_interrupt, NULL, &ejecutar_interrupcion, &interrupt);

    pthread_detach(hilo_dispatch);
    pthread_detach(hilo_interrupt);
}

void *ejecutar_pcb(void *arg) {
    int dispatch = *((int *)arg);
    int socket_dispatch = esperar_cliente(dispatch);
    /*
     * La cpu es una sola no?
     */
}

void *ejecutar_interrupcion(void *arg) {
    int interrupt = *((int *)arg);
    int socket_interrupt = esperar_cliente(interrupt);
}