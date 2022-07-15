#include "plani_mediano_plazo.h"

////////////////////////////////////////////
void inicializar_plani_mediano_plazo()
{
    pthread_create(hilo_mediano_plazo, NULL, mediano_plazo_pri, NULL);
    pthread_detach(*hilo_mediano_plazo);
}

///////////////////////////////////////////

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
        sem_post(&grado_multiprog_lo_permite); //TODO Toda la logica con memoria --> ya esta no?

        t_operacion *operacion = crear_operacion(SUSPENSION_PROCESO);
        setear_operacion(operacion,&(un_proceso->un_pcb->id_tabla_1n));
        enviar_operacion(operacion,un_proceso->mi_socket_memoria);
        eliminar_operacion(operacion);

        codigo_operacion cod_op = recibir_operacion(un_proceso->mi_socket_memoria);
        if(cod_op != SUSPENSION_PROCESO) {
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"Error al recibir la suspension del proceso %d",un_proceso->un_pcb->pid);
            pthread_mutex_unlock(&mutex_log);
        }
        else {
            int id_tabla_1n = recibir_entero(un_proceso->mi_socket_memoria);
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Se suspende proceso con PID = %u con ID tabla = %d",un_proceso->un_pcb->pid, id_tabla_1n);
            pthread_mutex_unlock(&mutex_log);
        }

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"El proceso con PID: %u paso a suspendido bloqueado | Tiempo: %f",
                 un_proceso->un_pcb->pid,
                 difftime(un_proceso->tiempoF,un_proceso->tiempoI));
        pthread_mutex_unlock(&mutex_log);
    }
    //un_proceso->tiempo_acumulado = difftime(un_proceso->tiempoF,un_proceso->tiempoI);
    pthread_mutex_unlock(&un_proceso->mutex_proceso);
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

///////////////////////////////////////////
