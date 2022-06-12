#include "../include/plani_largo_plazo.h"

////////////////////////////////////////////

void inicializar_plani_largo_plazo()
{
    pthread_create(hilo_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_detach(*hilo_largo_plazo);
}

void* planificador_largo_plazo(void)
{
    int valor_semaforo_multiprog = 0;
    t_proceso *un_proceso;

    while(true)
    {
        sem_wait(&grado_multiprog_lo_permite);
        sem_wait(&llego_un_proceso);
        sem_getvalue(&grado_multiprog_lo_permite,&valor_semaforo_multiprog);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"Se pasa proceso a READY, grado multiprogramacion: %d",valor_semaforo_multiprog);
        pthread_mutex_unlock(&mutex_log);

        if(false/*hay_susp_bloq*/)
        {
            //Paso a ready el susp bloq que haya primero en su lista
        }
        else
        {
            un_proceso = obtener_proceso_en_new();
        }
        transicionar_proceso_a_ready(un_proceso);
    }
}

////////////////////////////////////////////

t_proceso *obtener_proceso_en_new()
{
    t_proceso *un_proceso;

    pthread_mutex_lock(&mutex_procesos_en_new);
    un_proceso = (t_proceso*) queue_pop(procesos_en_new);
    pthread_mutex_unlock(&mutex_procesos_en_new);

    return un_proceso;
}

void transicionar_proceso_a_ready(t_proceso *un_proceso)
{


    pthread_mutex_lock(&mutex_procesos_en_ready);
    list_add(procesos_en_ready,(void*) un_proceso);
    pthread_mutex_unlock(&mutex_procesos_en_ready);

    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Se pasa proceso a READY -> PID = %u",un_proceso->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);

    sem_post(&hay_que_ordenar_cola_ready);
    sem_post(&hay_procesos_en_ready);
}

////////////////////////////////////////////

void finalizar_proceso_ejecutando()
{
    pthread_mutex_lock(&mutex_log);
    log_warning(un_logger,"Se finaliza el proceso con PID = %u",proceso_en_exec->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);

    responder_fin_proceso(proceso_en_exec->comunicacion_proceso->socket_proceso);
    transicionar_proceso_a_exit();
    proceso_en_exec = NULL;

    sem_post(&grado_multiprog_lo_permite);
}

void transicionar_proceso_a_exit()
{
    // No se si va a ser necesaria la lista!
    //list_add(procesos_en_exit,proceso_en_exec);

    queue_destroy(proceso_en_exec->un_pcb->consola->instrucciones);
    free(proceso_en_exec->un_pcb->consola);
    free(proceso_en_exec->un_pcb);
    pthread_mutex_destroy(&proceso_en_exec->comunicacion_proceso->mutex_socket_proceso);
    free(proceso_en_exec->comunicacion_proceso->hilo_com_proceso);
    free(proceso_en_exec->comunicacion_proceso);

    free(proceso_en_exec);
}

/////////////////////////////////////////////////
