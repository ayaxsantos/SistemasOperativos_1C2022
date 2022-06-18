#include "../include/cpu.h"


void iniciar() {
    cpu_dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
    cpu_interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    //conectar_a_memoria_y_recibir_config();
    esperar_a_kernel();
}

void esperar_a_kernel() {
    log_info(logger_cpu,"CPU a la espera de Kernel");
    pthread_t hilo_dispatch, hilo_interrupt;
    pthread_t hilo_ciclo_de_instruccion;
    pthread_create(&hilo_dispatch, NULL, &ejecutar_pcb, NULL);
    pthread_create(&hilo_interrupt, NULL, &ejecutar_interrupcion, NULL);
    pthread_create(&hilo_ciclo_de_instruccion, NULL, &ciclo_de_instruccion, NULL);

    pthread_detach(hilo_dispatch);
    pthread_detach(hilo_interrupt);
    pthread_detach(hilo_ciclo_de_instruccion);
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
    socket_kernel_dispatch = esperar_cliente(cpu_dispatch);
    esperar_handshake(&socket_kernel_dispatch, enviar_confirmacion);

	while(true) {

		sem_wait(&sem_busqueda_proceso_nuevo);
		int operacion = recibir_operacion(socket_kernel_dispatch);
        t_proceso_pcb *proceso_pcb;
        switch (operacion) {
			case PCB:
                proceso_pcb = deserializar_proceso_pcb(socket_kernel_dispatch);
                pcb = proceso_pcb->pcb;
                free(proceso_pcb);

                sem_post(&sem_ciclo_de_instruccion);
				break;
			default:
				break;
		}
	}
}


void *ciclo_de_instruccion(void *arg) {

	sem_wait(&sem_ciclo_de_instruccion);

	t_instruccion *instruccion = (t_instruccion *) queue_pop(pcb->consola->instrucciones); // FETCH
	operacion_a_enviar = UNDEFINED;
	proceso_a_enviar = malloc(sizeof(t_proceso_pcb));

	if(necesita_fetch_operands(instruccion->instruc)) { // DECODE
		// TODO
        uint32_t espacio_a_asignar = obtener_dato_memoria(instruccion->parametro1);
	}

	ejecutar_instruccion(instruccion); // EXECUTE
	pcb->program_counter ++;

	if(hay_que_desalojar_cpu()) {
		desalojar_cpu();
	} else {
		free(proceso_a_enviar);// Preguntar si está OK el free acá
		sem_post(&sem_ciclo_de_instruccion);
	}
}

void *ejecutar_interrupcion(void *arg) {
    //socket_kernel_interrupt = esperar_cliente(cpu_interrupt);
	while(true) {
		sem_wait(&sem_interrupt);
		//int operacion = recibir_operacion(socket_kernel_interrupt);
		int operacion = INTERRUPCION; // BORRAR

		if(operacion == INTERRUPCION) {
			//bool hay_interrupcion =  recibir_interrupcion(socket_kernel_interrupt);
			bool hay_interrupcion = false; // BORRAR
			if(hay_interrupcion) {
				proceso_a_enviar->tiempo_bloqueo = UNDEFINED;
				operacion_a_enviar = INTERRUPCION;

			}
		}
	}
}

int necesita_fetch_operands(instruccion instruction) {
	return instruction == COPY;
}

void ejecutar_instruccion(t_instruccion *instruccion) {
	int resultado;

    switch (instruccion->instruc) {
        case NO_OP:
            resultado = usleep(config_cpu.retardo_noop * 1000);
            if(resultado == -1 )
                log_error(logger_cpu, "Error al realizar usleep");
            sem_post(&sem_interrupt);
           break;

        case IO:
        	proceso_a_enviar->tiempo_bloqueo = instruccion->parametro1;
        	operacion_a_enviar = BLOQUEO;
           break;

        case READ:
        	sem_post(&sem_interrupt);
           break;

        case WRITE:
        	sem_post(&sem_interrupt);
           break;

        case COPY:
        	sem_post(&sem_interrupt);
           break;

        case I_EXIT:
			proceso_a_enviar->tiempo_bloqueo = UNDEFINED;
			operacion_a_enviar = FIN_PROCESO;
           break;
    }
}

void desalojar_cpu() {
		proceso_a_enviar->pcb = pcb;
		enviar_proceso_pcb(socket_kernel_dispatch, proceso_a_enviar, operacion_a_enviar);
		free(proceso_a_enviar);// Preguntar si está OK el free acá

		sem_post(&sem_busqueda_proceso_nuevo);
}

bool hay_que_desalojar_cpu() {
	return operacion_a_enviar != UNDEFINED;
}

