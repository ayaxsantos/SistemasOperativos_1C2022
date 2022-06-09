#include "../include/plani_corto_plazo.h"

////////////////////////////////////////////
// Generales, lanzamiento plani + funcion principal de este

void inicializar_plani_corto_plazo()
{
    if(strcmp("FIFO",una_config_kernel.algoritmo_planificacion) == 0)
    {
        lanzar_hilo_plani_corto_plazo_con(algoritmo_fifo);
    }
    else if(strcmp("SJF",una_config_kernel.algoritmo_planificacion) == 0)
    {
        lanzar_hilo_plani_corto_plazo_con(algoritmo_sjf_con_desalojo);
    }
    else
    {
        pthread_mutex_lock(&mutex_log);
        log_error(un_logger,"KERNEL PANIC -> Algoritmo de planificacion NO VALIDO");
        pthread_mutex_unlock(&mutex_log);

        //No se si ponerlo, es mucho quiza pero no deberia ni funcar en este caso...
        exit(EXIT_FAILURE);
    }
}

void lanzar_hilo_plani_corto_plazo_con(void* (*algoritmo_plani)(void*))
{
    pthread_create(hilo_corto_plazo, NULL, algoritmo_plani, NULL);
    pthread_detach(*hilo_corto_plazo);
}

void *algoritmo_fifo(void * args)
{
    while(true)
    {
        //Tomar el primer elemento de la lista
        sem_wait(&hay_procesos_en_ready);
        pthread_mutex_lock(&mutex_procesos_en_ready);
        proceso_en_exec = list_remove(procesos_en_ready, 0);
        pthread_mutex_unlock(&mutex_procesos_en_ready);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"Se pasa a EXEC el proceso PID = %u",proceso_en_exec->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);

        /*
        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_pcb(socket_dispatch, proceso_en_exec->un_pcb,PCB);
        pthread_mutex_unlock(&mutex_socket_dispatch);
         */

        gestionar_pcb_para_probar_sin_cpu();
        //gestionar_pcb();
    }
}

void *algoritmo_sjf_con_desalojo(void *args)
{
    t_proceso *proceso_candidato;

    while(true)
    {
        sem_wait(&hay_procesos_en_ready);

        //Por que este semaforo aca??
        //Para el primer orden, asi sabemos cual tenemos que mandar a ejecutar!!
        sem_wait(&hay_que_ordenar_cola_ready);

        //Ordenar lista
        organizacionPlani();

        //Tomar el primer elemento de la lista
        pthread_mutex_lock(&mutex_procesos_en_ready);
        proceso_en_exec = list_remove(procesos_en_ready, 0);
        pthread_mutex_unlock(&mutex_procesos_en_ready);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"Se pasa a EXEC el proceso PID = %u",proceso_en_exec->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_pcb(socket_dispatch, proceso_en_exec->un_pcb,PCB);
        pthread_mutex_unlock(&mutex_socket_dispatch);

        //Tomamos tiempo inicial APENAS lo pasamos a EXEC
        time(&tiempoI);

        while(true)
        {
            sem_wait(&hay_que_ordenar_cola_ready);

            //Tomamos le tiempo final, este se ira actualizando
            time(&tiempoF);

            organizacionPlani();
            proceso_candidato = list_get(procesos_en_ready,0);

            if(hay_que_desalojar(proceso_candidato))
            {
                pthread_mutex_lock(&mutex_log);
                log_info(un_logger, "Se debe desalojar al proceso con PID = %u",proceso_en_exec->un_pcb->pid);
                log_info(un_logger,"El proceso con PID = %u tiene una estimacion menor, de: %d",
                         proceso_candidato->un_pcb->pid,
                         proceso_candidato->un_pcb->una_estimacion);
                pthread_mutex_unlock(&mutex_log);

                solicitar_desalojo_a_cpu();
                break;
            }
        }
        gestionar_pcb();
    }
}

void solicitar_desalojo_a_cpu()
{
    return;
}

bool hay_que_desalojar(t_proceso *proceso_candidato)
{
    return proceso_candidato->un_pcb->una_estimacion < calcular_promedio_ponderado_exec();
}

double calcular_promedio_ponderado_exec()
{
    double tiempo_transcurrido_exec = difftime(tiempoF,tiempoI);
    double resultado = proceso_en_exec->un_pcb->una_estimacion - tiempo_transcurrido_exec;

    return round(resultado);
}

void pasar_proceso_a_bloqueado()
{
    //Esta funcion va a encargarse de pasar los procesos a bloqueado al recibir I/O

    t_proceso *proceso_a_bloquear = proceso_en_exec;

    proceso_en_exec = NULL;
    proceso_a_bloquear->un_pcb->un_estado = BLOCKED;

    pthread_mutex_lock(&mutex_procesos_en_bloq);
    queue_push(procesos_en_bloq,(void*) proceso_a_bloquear);
    pthread_mutex_unlock(&mutex_procesos_en_bloq);
    sem_post(&hay_procesos_en_blocked);
}

/////////////////////////////////////////////////

void realizar_envio_pcb(int socket, t_pcb *un_pcb)
{
    un_pcb->un_estado = EXEC;
    enviar_pcb(socket,un_pcb,PCB);

    //Se coloca tiempoI aqui
    time(&tiempoI);
}

t_pcb *obtener_pcb()
{
    int size;
    void *buffer = recibir_buffer(&size, socket_dispatch);
    t_pcb *un_pcb = deserializar_pcb(socket_dispatch);
    free(buffer);
    return un_pcb;
}

void gestionar_pcb()
{
    pthread_mutex_lock(&mutex_socket_dispatch);
    codigo_operacion un_codigo = (codigo_operacion)recibir_operacion(socket_dispatch);
    //Se coloca tiempoF aqui
    pthread_mutex_unlock(&mutex_socket_dispatch);
    switch(un_codigo)
    {
        case PCB:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB desaloja3!!");
            pthread_mutex_unlock(&mutex_log);
            pthread_mutex_lock(&mutex_socket_dispatch);
            proceso_en_exec->un_pcb = obtener_pcb();
            pthread_mutex_unlock(&mutex_socket_dispatch);
            time(&tiempoF);
            proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF,tiempoI,proceso_en_exec);
            devolver_proceso_a_ready(proceso_en_exec);
            break;
        case BLOQUEO:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB para bloquear!!");
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&mutex_socket_dispatch);
            t_proceso_bloqueo *proceso_para_bloquear = deserializar_proceso_bloqueo(socket_dispatch);
            pthread_mutex_unlock(&mutex_socket_dispatch);

            proceso_en_exec->un_pcb = proceso_para_bloquear->pcb;
            proceso_en_exec->tiempo_a_bloquear = proceso_para_bloquear->tiempo_bloqueo;

            time(&tiempoF);
            proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF,tiempoI,proceso_en_exec);
            pasar_proceso_a_bloqueado();
            //sem_post(&hay_procesos_en_ready);
            break;
        case FIN_PROCESO:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB para finalizar!!");
            pthread_mutex_unlock(&mutex_log);
            finalizar_proceso_ejecutando();
            break;
        default:
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"No se pudo recibir PCB!!");
            pthread_mutex_unlock(&mutex_log);
            break;
    }
}

void devolver_proceso_a_ready(t_proceso *un_proceso)
{
    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Se desaloja al proceso con PID: %u",un_proceso->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);


    pthread_mutex_lock(&mutex_procesos_en_ready);
    list_add(procesos_en_ready,un_proceso);
    pthread_mutex_lock(&mutex_procesos_en_ready);

    proceso_en_exec = NULL;

    sem_post(&hay_que_ordenar_cola_ready);
    sem_post(&hay_procesos_en_ready);
}

/////////////////////////////////////////////////
// Exclusivas de SJF con desalojo

int calcular_estimacion(time_t tiempoF, time_t tiempoI, t_proceso *un_proceso)
{
    double real_anterior = difftime(tiempoF,tiempoI);
    int alpha = una_config_kernel.alfa_plani;

    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Tiempito -> Real Anterior: %lf ms",real_anterior * 1E3);
    pthread_mutex_unlock(&mutex_log);

    int estimacion_anterior = un_proceso->un_pcb->una_estimacion;

    return estimacion_anterior * alpha + real_anterior * (1-alpha);
    //Cuenta...
    //Ti = Ti1 * α + Ri1 * (1 - α)
}

void organizacionPlani()
{
    pthread_mutex_lock(&mutex_procesos_en_ready);
    bool comparador_procesos_SJF(void* proceso_primero, void* proceso_segundo)
    {
        return comparador_de_procesos_SJF((t_proceso*)proceso_primero,(t_proceso*)proceso_segundo);
    }
    list_sort(procesos_en_ready, comparador_procesos_SJF);
    pthread_mutex_unlock(&mutex_procesos_en_ready);

}

bool comparador_de_procesos_SJF(t_proceso *un_proceso_primero, t_proceso *un_proceso_segundo)
{
    return un_proceso_primero->un_pcb->una_estimacion < un_proceso_segundo->un_pcb->una_estimacion;
}

/////////////////////////////////////////////////////
// FUNCION SOLO PARA PROBAR SIN NECESIDAD DE LEVANTAR CPU...
// SOLO PRUEBAS BASICAS DE PLANIFICACION CORTO PLAZO

void gestionar_pcb_para_probar_sin_cpu() {

    while (true) {
        t_instruccion *una_instruccion = queue_pop(proceso_en_exec->un_pcb->consola->instrucciones);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger, "La instruccion ejecutando es: %u", una_instruccion->instruc);
        pthread_mutex_unlock(&mutex_log);

        if (una_instruccion->instruc == IO) {
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger, "Volvio un PCB para bloquear!!");
            pthread_mutex_unlock(&mutex_log);

            proceso_en_exec->tiempo_a_bloquear = una_instruccion->parametro1;
            time(&tiempoF);
            proceso_en_exec->un_pcb->una_estimacion = 0;

            free(una_instruccion);
            pasar_proceso_a_bloqueado();
            break;
        } else if (una_instruccion->instruc == I_EXIT) {
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger, "Volvio un PCB para finalizar!!");
            pthread_mutex_unlock(&mutex_log);

            free(una_instruccion);
            finalizar_proceso_ejecutando(proceso_en_exec);
            break;
        }
        free(una_instruccion);
    }
}

//////////////////////////////////////////////////////
