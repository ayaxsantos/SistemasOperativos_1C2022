#include "../include/plani_mediano_plazo.h"

////////////////////////////////////////////
int duracion;
///////////////////////////////////////////



void inicializar_plani_mediano_plazo(){
    pthread_create(hilo_mediano_plazo, NULL, planificador_mediano_plazo, NULL);
    pthread_detach(*hilo_mediano_plazo);
    pthread_create(hilo_mediano_plazo, NULL, planificador_mediano_plazo_susp, NULL);
    pthread_detach(*hilo_mediano_plazo);
}

void *planificador_mediano_plazo(void * args)
{
    while(1)
    {
        sem_wait(&hay_procesos_en_blocked);

        //I/O(tiempo): Esta instrucción representa una syscall de I/O bloqueante.
        // Se deberá devolver el PCB actualizado al Kernel junto al tiempo de bloqueo en milisegundos.
        pthread_mutex_lock(&mutex_procesos_en_bloq);
        proceso_en_bloq = queue_pop(procesos_en_bloq);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);
        //duracion aun tenemos que definirlo como recibirlo y donde se guarde
        if(una_config_kernel.tiempo_max_bloqueado < duracion)
        {
            proceso_en_bloq->un_pcb->un_estado = SUSP_BLOCKED;
            pthread_mutex_lock(&mutex_procesos_en_bloq_susp);
            queue_push(procesos_en_bloq_susp,(void*) proceso_en_bloq);
            pthread_mutex_unlock(&mutex_procesos_en_bloq_susp);

            sem_post(&hay_procesos_en_blocked_susp);
        }
    }
}

void *planificador_mediano_plazo_susp(void * args)
{
    while(1)
    {
        sem_wait(&hay_procesos_en_blocked_susp);
        pthread_mutex_lock(&mutex_procesos_en_bloq_susp);
        proceso_en_bloq_susp = queue_pop(procesos_en_bloq_susp);
        pthread_mutex_unlock(&mutex_procesos_en_bloq_susp);
        if(1)  //condicion de si esta para pasar a ready
        {
            proceso_en_bloq->un_pcb->un_estado = SUSP_READY;
            pthread_mutex_lock(&mutex_procesos_en_bloq_susp);
            queue_push(procesos_en_ready_susp,(void*) proceso_en_bloq);
            pthread_mutex_unlock(&mutex_procesos_en_bloq_susp);
            //Estos procesos tienen prioridad sobre los new
        }
    }
}