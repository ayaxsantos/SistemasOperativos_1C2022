#include "../include/cpu.h"


void iniciar() {
    //pcbs = list_create();

    cpu_dispatch = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_dispatch);
    cpu_interrupt = iniciar_servidor(config_cpu.ip_cpu, config_cpu.puerto_escucha_interrupt);
    conectar_a_memoria_y_recibir_config();
    esperar_a_kernel();
}

void esperar_a_kernel() {
    log_info(logger_cpu,"CPU a la espera de Kernel");
    socket_kernel_dispatch = esperar_cliente(cpu_dispatch);
    esperar_handshake(&socket_kernel_dispatch, enviar_confirmacion);
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
	while(true) {
		sem_wait(&sem_execute);
		int operacion = recibir_operacion(cpu_dispatch);
		switch (operacion) {
			case PCB:
				ciclo_de_instruccion();
				break;
			default:
				break;
		}
		/*
		 * La cpu es una sola no?
		 */

	}
}


void ciclo_de_instruccion() {
	pcb = deserializar_pcb(socket_kernel_dispatch);

	t_instruccion *instruccion = (t_instruccion *) queue_pop(pcb->consola->instrucciones); // FETCH

	if(necesita_fetch_operands(instruccion->instruc)) { // DECODE
		// TODO
        void *espacio_a_asignar = obtener_dato_memoria(instruccion->parametro1, pcb);
	}
	pcb->program_counter ++; // Incremento acá pc porque si no, si tengo que enviar pcb a kernel, se va a mandar pc desactualizado
	ejecutar_instruccion(instruccion); // EXECUTE

	sem_post(&sem_execute); // Comience el ciclo de instrucccion devuelta
}

void *ejecutar_interrupcion(void *arg) {
	while(true) {
		int socket_interrupt = esperar_cliente(cpu_interrupt);

		sem_wait(&sem_interrupt);
		int operacion = recibir_operacion(cpu_dispatch);

		if(operacion == INTERRUPCION) {
			bool hay_interrupcion =  recibir_interrupcion(socket_interrupt);

			if(hay_interrupcion) {
				t_proceso_pcb *proceso_a_enviar = malloc(sizeof(t_proceso_pcb));

				proceso_a_enviar->tiempo_bloqueo = UNDEFINED;
				proceso_a_enviar->pcb = pcb;

				enviar_proceso_pcb(socket_kernel_dispatch, proceso_a_enviar, INTERRUPCION);

				free(proceso_a_enviar); // Preguntar si está OK el free acá
			}
		}
	}
}

int necesita_fetch_operands(instruccion instruction) {
	return instruction == COPY;
}

void ejecutar_instruccion(t_instruccion *instruccion) {
	int resultado;
	t_proceso_pcb *proceso_a_enviar = malloc(sizeof(t_proceso_pcb));
    switch (instruccion->instruc) {
        case NO_OP:

            resultado = usleep(config_cpu.retardo_noop * 1000);
            if(resultado == -1 )
                log_error(logger_cpu, "Error al realizar usleep");
            sem_post(&sem_interrupt);
           break;
        case IO: // Pedir a Kernel que bloque el proceso el tiempo que viene indicado en el param1
        	proceso_a_enviar->pcb = pcb;
        	proceso_a_enviar->tiempo_bloqueo = instruccion->parametro1;
        	enviar_proceso_pcb(socket_kernel_dispatch, proceso_a_enviar, BLOQUEO);
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

        	//fin_de_proceso(socket_kernel_dispatch, pcb);
        	sem_post(&sem_execute);
           break;
    }
    free(proceso_a_enviar);
}

// Inspirado en Kernel 😂
//  TODO: tal vez esta funcion deberia ir a carpisLib, se usa en cpu y kernel, para evitar repeticion de lógica
void fin_de_proceso(int socket, t_pcb* un_pcb)
{
    t_operacion *operacion = crear_operacion(FIN_PROCESO);
    setear_operacion(operacion, un_pcb);
    enviar_operacion(operacion, socket);
    eliminar_operacion(operacion);
}
