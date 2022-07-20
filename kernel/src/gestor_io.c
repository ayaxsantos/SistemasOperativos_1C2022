#include "../include/gestor_io.h"

//////////////////////////////////////////
// Aun sin suspencion de procesos!! OJO que mediano
// plazo va a tener que ver aca...

void inicializar_gestor_io()
{
    pthread_create(hilo_gestor_io, NULL, gestor_io, NULL);
    pthread_detach(*hilo_corto_plazo);
}

void *gestor_io(void)
{
    int tiempo_a_bloquear = 0;
    double tiempo_bloq_mostrar = 0;

    while(true)
    {
        sem_wait(&hay_procesos_en_blocked);
        pthread_mutex_lock(&mutex_procesos_en_bloq);
        t_proceso *un_proceso = list_get(procesos_en_bloq,0);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);

        tiempo_bloq_mostrar = ((double) un_proceso->tiempo_bloqueo)/1000;

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"El proceso con PID: %u hace su I/O de %f segundos",
                 un_proceso->un_pcb->pid,
                 tiempo_bloq_mostrar);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&un_proceso->mutex_proceso);
        tiempo_a_bloquear = un_proceso->tiempo_bloqueo * 1000;
        pthread_mutex_unlock(&un_proceso->mutex_proceso);

        usleep(tiempo_a_bloquear);
        pthread_mutex_lock(&mutex_log);

        log_warning(un_logger,"El proceso con PID: %u salio del I/O",un_proceso->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&mutex_procesos_en_bloq);
        un_proceso = list_remove(procesos_en_bloq,0);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);

        if(el_proceso_esta_susp_bloqueado(un_proceso))
        {
            //Pasar a susp ready
            pthread_mutex_lock(&mutex_log);
            log_warning(un_logger,"El proceso con PID: %u pasa a SUSPENDIDO READY",un_proceso->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&un_proceso->mutex_proceso);
            un_proceso->un_pcb->un_estado = SUSP_READY;
            pthread_mutex_unlock(&un_proceso->mutex_proceso);

            pthread_mutex_lock(&mutex_procesos_en_susp_ready);
            queue_push(procesos_en_susp_ready,un_proceso);
            pthread_mutex_unlock(&mutex_procesos_en_susp_ready);

            sem_post(&llego_un_proceso);
        }
        else
        {
            //Pasar a ready
            pthread_mutex_lock(&mutex_log);
            log_warning(un_logger,"El proceso con PID: %u pasa a READY",un_proceso->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&un_proceso->mutex_proceso);
            un_proceso->un_pcb->un_estado = READY;
            pthread_mutex_unlock(&un_proceso->mutex_proceso);

            pthread_mutex_lock(&mutex_procesos_en_ready);
            list_add(procesos_en_ready,un_proceso);
            pthread_mutex_unlock(&mutex_procesos_en_ready);

            sem_post(&hay_que_ordenar_cola_ready);
            sem_post(&hay_procesos_en_ready);
        }
    }
}


bool el_proceso_esta_susp_bloqueado(t_proceso *un_proceso)
{
    pthread_mutex_lock(&un_proceso->mutex_proceso);
    bool condicion = un_proceso->un_pcb->un_estado == SUSP_BLOCKED;
    pthread_mutex_unlock(&un_proceso->mutex_proceso);
    return condicion;
}

//////////////////////////////////////////
