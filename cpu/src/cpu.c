#include "../include/cpu.h"


void iniciar() {
   /* int cpu_dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
    int cpu_interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    socket_kernel_dispatch = esperar_cliente(cpu_dispatch);
    socket_kernel_interrupt = esperar_cliente(cpu_interrupt);*/
    //conectar_a_memoria_y_recibir_config();
    esperar_a_kernel(0, 0); // TODO ¿
}

void esperar_a_kernel(int cpu_dispatch, int cpu_interrupt) {
    log_info(logger_cpu,"CPU a la espera de Kernel");

    pthread_t *hilo_dispatch = malloc(sizeof(pthread_t));
    pthread_t *hilo_interrupt = malloc(sizeof(pthread_t));
    pthread_t *hilo_ciclo_de_instruccion = malloc(sizeof(pthread_t));

    pthread_create(hilo_dispatch, NULL, &ejecutar_pcb, &cpu_dispatch);
    pthread_create(hilo_interrupt, NULL, &ejecutar_interrupcion, &cpu_interrupt);
    pthread_create(hilo_ciclo_de_instruccion, NULL, &ciclo_de_instruccion, NULL);

    pthread_join(*hilo_interrupt, NULL);
    pthread_join(*hilo_dispatch, NULL);
    pthread_join(*hilo_ciclo_de_instruccion, NULL);
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
    } else {
        log_error(logger_cpu, "Error al realizar el HANDSHAKE con Kernel");
    }
}

void *ejecutar_pcb(void *arg) {
    int cpu_dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
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
    while (true) {
        sem_wait(&sem_ciclo_de_instruccion);

        t_instruccion *instruccion = (t_instruccion *) queue_pop(pcb->consola->instrucciones); // FETCH

        hay_interrupcion = false;
        operacion_a_enviar = UNDEFINED;
        proceso_a_enviar = malloc(sizeof(t_proceso_pcb));
        uint32_t valor_a_copiar;

        if(necesita_fetch_operands(instruccion->instruc)) { // DECODE
            // TODO
        	valor_a_copiar = obtener_dato_memoria(instruccion->parametro2); // VER, si está OK el parametro!!
        }

        ejecutar_instruccion(instruccion, valor_a_copiar); // EXECUTE
        pcb->program_counter ++;

        if(hay_que_desalojar_cpu()) {
            desalojar_cpu();
        } else {
            free(proceso_a_enviar);// Preguntar si está OK el free acá
            sem_post(&sem_ciclo_de_instruccion);
        }
    }
}

void *ejecutar_interrupcion(void *arg) {
    //int *cpu_interrupt = (int *)arg;
    int cpu_interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    socket_kernel_interrupt = esperar_cliente(cpu_interrupt);
	while(true) {

		int operacion = recibir_operacion(socket_kernel_interrupt);

		if(operacion == INTERRUPCION) {
			hay_interrupcion =  recibir_interrupcion(socket_kernel_interrupt);
            sem_wait(&sem_interrupt);

			if(hay_interrupcion) { // Creeria que este booleano cuando ejecute esta linea SIEMPRE va a ser true
				proceso_a_enviar->tiempo_bloqueo = UNDEFINED;
				operacion_a_enviar = INTERRUPCION;
			}
		}
/*        else if(operacion == UNDEFINED) {
            printf("Hola");
            //sem_post(&sem_ciclo_de_instruccion);
        }*/
	}
}

int necesita_fetch_operands(instruccion instruction) {
	return instruction == COPY;
}

void ejecutar_instruccion(t_instruccion *instruccion, uint32_t valor_a_copiar) {
	int resultado;

    switch (instruccion->instruc) {
        case NO_OP:
            resultado = usleep(config_cpu.retardo_noop * 1000);
            if(resultado == -1 )
                log_error(logger_cpu, "Error al realizar usleep");
            chequear_si_hay_interrupcion();
           break;

        case IO:
        	proceso_a_enviar->tiempo_bloqueo = instruccion->parametro1;
        	operacion_a_enviar = BLOQUEO;
           break;

        case READ:
        	log_info(logger_cpu, "Valor leido de memoria: %i ", obtener_dato_memoria(instruccion->parametro1));
        	//printf("Valor leido de memoria: %i \n", obtener_dato_memoria(instruccion->parametro1));
        	chequear_si_hay_interrupcion();
           break;

        case WRITE:
        	escribir_dato_memoria(instruccion->parametro1, instruccion->parametro2);
        	chequear_si_hay_interrupcion();
           break;

        case COPY:
        	escribir_dato_memoria(instruccion->parametro1, valor_a_copiar);
        	chequear_si_hay_interrupcion();
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

void chequear_si_hay_interrupcion() {

	if(hay_interrupcion) {
		sem_post(&sem_interrupt);
	} else {
		sem_post(&sem_ciclo_de_instruccion);
	}
}

