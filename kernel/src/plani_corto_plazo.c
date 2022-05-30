#include "../include/plani_corto_plazo.h"

////////////////////////////////////////////

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
        proceso_en_exec = list_get(procesos_en_ready, 0);
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
        //Aplicar algoritmo de ordenamiento
        pthread_mutex_lock(&mutex_procesos_en_ready);
        proceso_en_exec = list_get(procesos_en_ready, 0);
        pthread_mutex_unlock(&mutex_procesos_en_ready);
        return NULL;
    }
}

void pasar_proceso_a_bloqueado(t_proceso *un_proceso)
{
    //Esta funcion va a encargarse de pasar los procesos a bloqueado al recibir I/O
    un_proceso->un_pcb->un_estado = BLOCKED;
    pthread_mutex_lock(&mutex_procesos_en_bloq);
    list_add(procesos_en_bloq,(void*) un_proceso);
    pthread_mutex_unlock(&mutex_procesos_en_bloq);
    sem_post(&hay_procesos_en_blocked);
}

/////////////////////////////////////////////////

void enviar_pcb(int socket, t_pcb* un_pcb)
{
    t_operacion *operacion = crear_operacion(PCB);
    setear_operacion(operacion, un_pcb);
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

/////////////////////////////////////////
