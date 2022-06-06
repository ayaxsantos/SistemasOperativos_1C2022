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
    while(true)
    {
        sem_wait(&hay_procesos_en_blocked);

        pthread_mutex_lock(&mutex_procesos_en_bloq);
        t_proceso *un_proceso = queue_pop(procesos_en_bloq);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"El prceso con PID: %u hace su I/O de %d segundos",
                 un_proceso->un_pcb->pid,
                 un_proceso->tiempo_a_bloquear);
        pthread_mutex_unlock(&mutex_log);

        usleep(un_proceso->tiempo_a_bloquear * 1000);

        pthread_mutex_lock(&mutex_procesos_en_ready);
        list_add(procesos_en_ready,un_proceso);
        pthread_mutex_unlock(&mutex_procesos_en_ready);
        sem_post(&hay_procesos_en_ready);
    }
}

//////////////////////////////////////////
