#include "../include/plani_corto_plazo.h"

////////////////////////////////////////////

time_t tiempoF, tiempoI;

void inicializar_plani_corto_plazo(char *algoritmo_seleccionado)
{
    if(!strcmp("FIFO",algoritmo_seleccionado))
    {
        lanzar_hilo_plani_corto_plazo_con(algoritmo_fifo);
    }
    else if(!strcmp("SJF",algoritmo_seleccionado))
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
        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_pcb(socket_dispatch, proceso_en_exec->un_pcb);
        pthread_mutex_unlock(&mutex_socket_dispatch);
        gestionar_pcb();
    }
}

void *algoritmo_sjf_con_desalojo(void *args)
{
    while(true)
    {
        sem_wait(&hay_procesos_en_ready);
        //Ordenar lista
        organizacionPlani();
        //Tomar el primer elemento de la lista
        pthread_mutex_lock(&mutex_procesos_en_ready);
        proceso_en_exec = list_remove(procesos_en_ready, 0);
        pthread_mutex_unlock(&mutex_procesos_en_ready);
        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_pcb(socket_dispatch, proceso_en_exec->un_pcb);
        pthread_mutex_unlock(&mutex_socket_dispatch);
        gestionar_pcb();
    }
}

void pasar_proceso_a_bloqueado(t_proceso *un_proceso)
{
    //Esta funcion va a encargarse de pasar los procesos a bloqueado al recibir I/O
    un_proceso->un_pcb->un_estado = BLOCKED;
    pthread_mutex_lock(&mutex_procesos_en_bloq);
    queue_push(procesos_en_bloq,(void*) un_proceso);
    pthread_mutex_unlock(&mutex_procesos_en_bloq);
    sem_post(&hay_procesos_en_blocked);
}

/////////////////////////////////////////////////

void enviar_pcb(int socket, t_pcb* un_pcb)
{
    un_pcb->un_estado = EXEC;
    t_operacion *operacion = crear_operacion(PCB);
    setear_operacion(operacion, un_pcb);
    //Se coloca tiempoI aqui
    time(&tiempoI);
    enviar_operacion(operacion, socket);
    eliminar_operacion(operacion);
}

t_pcb *recibir_pcb()
{
    //Size aca no me sirve pero para que no rompa lo dejo
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
            log_info(un_logger,"Volvio un PCB para bloquear!!");
            pthread_mutex_unlock(&mutex_log);
            pthread_mutex_lock(&mutex_socket_dispatch);
            proceso_en_exec->un_pcb = recibir_pcb();
            pthread_mutex_unlock(&mutex_socket_dispatch);
            time(&tiempoF);
            proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF,tiempoI,proceso_en_exec);
            pasar_proceso_a_bloqueado(proceso_en_exec);
            break;
        case FIN_PROCESO:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB para finalizar!!");
            pthread_mutex_unlock(&mutex_log);
            break;
        default:
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"No se pudo recibir PCB!!");
            pthread_mutex_unlock(&mutex_log);
            break;
    }
}

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
    //Ti = Ti­1 * α + Ri­1 * (1 -­ α)
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

/////////////////////////////////////////
