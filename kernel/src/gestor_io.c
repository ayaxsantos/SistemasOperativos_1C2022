#include "../include/gestor_io.h"

//////////////////////////////////////////
// Aun sin suspencion de procesos!! OJO que mediano
// plazo va a tener que ver aca...

void inicializar_gestor_io()
{
    pthread_create(hilo_gestor_io, NULL, gestor_io, NULL);
    pthread_detach(*hilo_corto_plazo);
    pthread_create(hilo_mediano_plazo, NULL, mediano_plazo_pri, NULL);
    pthread_detach(*hilo_mediano_plazo);
}


void *gestor_io(void)
{
    while(true)
    {
        sem_wait(&hay_procesos_en_blocked);
        pthread_mutex_lock(&mutex_procesos_en_bloq);
        t_proceso *un_proceso = list_get(procesos_en_bloq,0);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);
        int pepito = 0;
        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"El proceso con PID: %u hace su I/O de %d milisegundos",
                 un_proceso->un_pcb->pid,
                 un_proceso->tiempo_bloqueo);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&un_proceso->mutex_proceso);
        pepito = un_proceso->tiempo_bloqueo * 1000;
        pthread_mutex_unlock(&un_proceso->mutex_proceso);

        usleep(pepito);
        pthread_mutex_lock(&mutex_log);

        log_error(un_logger,"El proceso con PID: %u salio del I/O",un_proceso->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&mutex_procesos_en_bloq);
        un_proceso = list_remove(procesos_en_bloq,0);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);

        pthread_mutex_lock(&mutex_log);
        log_error(un_logger,"El proceso con PID: %u va a comparar y su tiempo acumulado es: %f ",
                  un_proceso->un_pcb->pid,
                  un_proceso->tiempo_acumulado);
        pthread_mutex_unlock(&mutex_log);

        if((un_proceso->un_pcb->un_estado) == SUSP_BLOCKED)
        {
            //Pasar a susp ready
            //Se deberia pasar a mediano plazo
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"El proceso con PID: %u pasa a suspendido_ready",un_proceso->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);
            un_proceso->un_pcb->un_estado = SUSP_READY; // Faltan semaforos
            pthread_mutex_lock(&mutex_procesos_en_ready);
            list_add(procesos_en_ready,un_proceso);
            pthread_mutex_unlock(&mutex_procesos_en_ready);
        }
        else
        {
            //Pasar a ready
            pthread_mutex_lock(&mutex_log);
            log_warning(un_logger,"El proceso con PID: %u pasa a ready",un_proceso->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);
            un_proceso->un_pcb->un_estado = READY; //Faltan semaforos
            pthread_mutex_lock(&mutex_procesos_en_ready);
            list_add(procesos_en_ready,un_proceso);
            pthread_mutex_unlock(&mutex_procesos_en_ready);
        }
        sem_post(&hay_que_ordenar_cola_ready);
        sem_post(&hay_procesos_en_ready);
    }
}

//////////////////////////////////////////

void *mediano_plazo_pri(void* args)
{
    while(true)
    {
        pthread_mutex_lock(&mutex_procesos_en_bloq);
        void monitorear_estado_y_tiempo_inner(void *un_proceso)
        {
            monitorear_estado_y_tiempo((t_proceso*)un_proceso);
        }
        list_iterate(procesos_en_bloq,monitorear_estado_y_tiempo_inner);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);
        usleep(100);
    }
}

void monitorear_estado_y_tiempo(t_proceso *un_proceso)
{
    pthread_mutex_lock(&un_proceso->mutex_proceso);
    if(el_proceso_no_esta_suspendido(un_proceso))
    {
        un_proceso->un_pcb->un_estado = SUSP_BLOCKED;
        sem_post(&grado_multiprog_lo_permite); //TODO Toda la logica con memoria
        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"El proceso con PID: %u paso a suspendido bloqueado | Tiempo: %f",
                 un_proceso->un_pcb->pid,
                 difftime(un_proceso->tiempoF,un_proceso->tiempoI));
        pthread_mutex_unlock(&mutex_log);
    }
    //un_proceso->tiempo_acumulado = difftime(un_proceso->tiempoF,un_proceso->tiempoI);
    pthread_mutex_unlock(&un_proceso->mutex_proceso);
}

void *mediano_plazo(void)
{
    t_list *lista_auxiliar = list_create();
    t_proceso *proceso_auxiliar;

    time(&tiempoI);
    while(true) {
        usleep(100);
        pthread_mutex_lock(&mutex_log);

        pthread_mutex_unlock(&mutex_log);
        bool el_proceso_no_esta_suspendido_inner(void *un_proceso) {
            return el_proceso_no_esta_suspendido((t_proceso *) un_proceso);
        }

        pthread_mutex_lock(&mutex_procesos_en_bloq);
        lista_auxiliar = list_filter(procesos_en_bloq, el_proceso_no_esta_suspendido_inner);
        pthread_mutex_unlock(&mutex_procesos_en_bloq);

        while (!list_is_empty(lista_auxiliar)) {
            pthread_mutex_lock(&proceso_auxiliar->mutex_proceso);
            proceso_auxiliar = list_remove(lista_auxiliar, 0);
            proceso_auxiliar->un_pcb->un_estado = SUSP_BLOCKED;
            pthread_mutex_unlock(&proceso_auxiliar->mutex_proceso);
            sem_post(&grado_multiprog_lo_permite); //TODO Toda la logica con memoria
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"El proceso con PID: %u paso a suspendido bloqueado",proceso_auxiliar->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);
        }

        pthread_mutex_lock(&mutex_procesos_en_bloq);
        while (!list_is_empty(procesos_en_bloq)) {
            proceso_auxiliar = list_remove(procesos_en_bloq, 0);
            time(&tiempoF);
            proceso_auxiliar->tiempo_acumulado += difftime(tiempoF,tiempoI);
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Tiempito: %f",proceso_auxiliar->tiempo_acumulado);
            pthread_mutex_unlock(&mutex_log);
            list_add(lista_auxiliar, proceso_auxiliar);
        }
        time(&tiempoI);
        procesos_en_bloq = lista_auxiliar;
        pthread_mutex_unlock(&mutex_procesos_en_bloq);
        list_destroy(lista_auxiliar);
    }
}

bool el_proceso_no_esta_suspendido(t_proceso *un_proceso)
{
    return un_proceso->un_pcb->un_estado == BLOCKED && el_proceso_tiene_que_suspenderse(un_proceso);
}

bool el_proceso_tiene_que_suspenderse(t_proceso *un_proceso)
{
    time(&un_proceso->tiempoF);
    double tiempo_delta = difftime(un_proceso->tiempoF,un_proceso->tiempoI);
    //pthread_mutex_lock(&mutex_log);
    //log_info(un_logger,"Tiempo DELTA: %f",tiempo_delta);
    //pthread_mutex_unlock(&mutex_log);
    return tiempo_delta >= una_config_kernel.tiempo_max_bloqueado;
}

//////////////////////////////////////////
