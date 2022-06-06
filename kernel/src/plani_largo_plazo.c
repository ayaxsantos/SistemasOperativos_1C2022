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

        un_proceso = obtener_proceso_en_new();
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
    //Agrego que se le coloque una estimacion inicial al proceso -> Carlos
    un_proceso->un_pcb->una_estimacion = una_config_kernel.estimacion_inicial;
    pthread_mutex_lock(&mutex_procesos_en_ready);
    list_add(procesos_en_ready,(void*) un_proceso);
    pthread_mutex_unlock(&mutex_procesos_en_ready);

    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Se pasa proceso a READY -> PID = %u",un_proceso->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);

    sem_post(&hay_procesos_en_ready);
}

////////////////////////////////////////////

void finalizar_proceso_ejecutando()
{
    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Se finaliza el proceso con PID = %d",proceso_en_exec->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);

    transicionar_proceso_a_exit();

    responder_fin_proceso(proceso_en_exec->socket_proceso);
    proceso_en_exec = NULL;

    sem_post(&grado_multiprog_lo_permite);
}

void transicionar_proceso_a_exit()
{
    list_add(procesos_en_exit,proceso_en_exec);
}
