#include "../include/cpu.h"


void iniciar() {
    //pcbs = list_create();

    dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
    interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    conectar_a_memoria_y_recibir_config();
    esperar_a_kernel();
}

void esperar_a_kernel() {
    log_info(logger_cpu,"CPU a la espera de Kernel");
    socket_kernel_dispatch = esperar_cliente(dispatch);
    esperar_handshake(socket_kernel_dispatch, enviar_confirmacion);
    pthread_t hilo_dispatch, hilo_interrupt;
    pthread_create(&hilo_dispatch, NULL, &ejecutar_pcb, NULL);
    pthread_create(&hilo_interrupt, NULL, &ejecutar_interrupcion, NULL);

    pthread_detach(hilo_dispatch);
    pthread_detach(hilo_interrupt);
}

void enviar_confirmacion(int *socket, modulo modulo_solicitante) {
    if(modulo_solicitante == KERNEL) {
        log_info(logger_cpu,"HANDSHAKE con Kernel realizado");
        void *buffer = malloc(sizeof(int)*2);
        modulo modulo_actual = CPU;
        codigo_operacion handshake = HANDSHAKE;
        memcpy(buffer, &handshake, sizeof(int));
        memcpy(buffer + sizeof(int), &modulo_actual, sizeof(int));
        send(*socket, buffer, sizeof(int)*2, 0);
    }
    else {
        log_error(logger_cpu, "Error al realizar el HANDSHAKE con Kernel");
    }
}

void *ejecutar_pcb(void *arg) {
    //WAIT(sem_ejecutar); inicia: 1
    int operacion = recibir_operacion(dispatch);
    switch (operacion) {
        case PCB:
            recibir_pcb();
            break;
        default:
            break;
    }
    /*
     * La cpu es una sola no?
     */
    //SIGNAL(sem_interrupt);
}

//void recibir_pcb() {
//    t_pcb *pcb = deserializar_pcb(socket_kernel_dispatch);
//    while(queue_is_empty(pcb->consola->instrucciones)) {
//        t_instruccion *instruccion = (t_instruccion *) queue_pop(pcb->consola->instrucciones);
//        switch (instruccion->instruc) {
//            case NO_OP:
//                break;
//            case IO:
//                break;
//        }
//    }
//}

void ciclo_de_instruccion() {
	t_pcb *pcb = deserializar_pcb(socket_kernel_dispatch);

	t_instruccion *instruccion = (t_instruccion *) queue_pop(pcb->consola->instrucciones); // FETCH

	if(necesita_fetch_operands(instruccion->instruc)) { // DECODE
		// Pedir direc logica a MMU
        void *espacio_a_asignar = obtener_dato_memoria(instruccion->parametro1, pcb);
	}

	ejecutar_instruccion(instruccion);
}

void *ejecutar_interrupcion(void *arg) {
    int socket_interrupt = esperar_cliente(interrupt);
    //guardar esa interrupcion
    //WAIT(sem_interrupt) -> inicia: 0
    //SIGNAL(sem_execute);
}

int necesita_fetch_operands(instruccion *instruction) {
	return instruction == COPY;
}

void ejecutar_instruccion(t_instruccion *instruccion) {
    int resultado;
    switch (instruccion->instruc) {
        case NO_OP:
            resultado = usleep(config_cpu.retardo_noop * 1000); // devuelve 0, todo ok o -1 si fallo, falta agarrar el error
            if(resultado == -1 )
                log_error(logger_cpu, "Error al realizar usleep");
           break;
        case IO:
           break;
    }
}
