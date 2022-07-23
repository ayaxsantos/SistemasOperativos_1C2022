#include "plani_mediano_plazo.h"

////////////////////////////////////////////////////

void *monitorear_estado_y_tiempo_pri(void *un_proceso_param)
{
    t_proceso *un_proceso = (t_proceso*) un_proceso_param;
    //Nos habia quedado en segundos...
    usleep(una_config_kernel.tiempo_max_bloqueado * 1000000);

    pthread_mutex_lock(&(un_proceso->mutex_proceso));
    int un_pid = un_proceso->un_pcb->pid;
    pthread_mutex_unlock(&(un_proceso->mutex_proceso));

    if(el_proceso_esta_bloqueado(un_proceso))
    {
        pthread_mutex_lock(&un_proceso->mutex_proceso);
        un_proceso->un_pcb->un_estado = SUSP_BLOCKED;
        pthread_mutex_unlock(&un_proceso->mutex_proceso);
        sem_post(&grado_multiprog_lo_permite);

        pthread_mutex_lock(&un_proceso->mutex_proceso);
        time(&un_proceso->tiempoF);
        pthread_mutex_unlock(&un_proceso->mutex_proceso);

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"El proceso con PID: %u SE LO VA A SUSPENDER | Tiempo: %f",
                    un_pid,
                    difftime(un_proceso->tiempoF,un_proceso->tiempoI));
        pthread_mutex_unlock(&mutex_log);

        t_operacion *operacion = crear_operacion(SUSPENSION_PROCESO);
        pthread_mutex_lock(&un_proceso->mutex_proceso);
        setear_operacion(operacion,&(un_proceso->un_pcb->id_tabla_1n));
        enviar_operacion(operacion,un_proceso->mi_socket_memoria);
        pthread_mutex_unlock(&un_proceso->mutex_proceso);
        eliminar_operacion(operacion);

        pthread_mutex_lock(&mutex_log);
        log_error(un_logger,"Envie peticion a memoria, pid %d",un_pid);
        pthread_mutex_unlock(&mutex_log);

        pthread_mutex_lock(&un_proceso->mutex_proceso);

        pthread_mutex_lock(&mutex_log);
        log_error(un_logger,"Entre a recibir datos de memoria, pid %d",un_pid);
        pthread_mutex_unlock(&mutex_log);

        codigo_operacion cod_op = recibir_operacion(un_proceso->mi_socket_memoria);
        pthread_mutex_unlock(&un_proceso->mutex_proceso);

        if(cod_op != SUSPENSION_PROCESO) {
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"Error al recibir la suspension del proceso %d",un_pid);
            pthread_mutex_unlock(&mutex_log);
        }
        else {
            int id_tabla_1n = recibir_entero(un_proceso->mi_socket_memoria);
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Se suspende proceso con PID = %u con ID tabla = %d",un_pid, id_tabla_1n);
            pthread_mutex_unlock(&mutex_log);
        }

        pthread_mutex_lock(&un_proceso->mutex_proceso);
        time(&un_proceso->tiempoF);
        pthread_mutex_unlock(&un_proceso->mutex_proceso);

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"El proceso con PID: %u paso a SUSPENDIDO BLOQUEADO | Tiempo: %f",
                    un_pid,
                    difftime(un_proceso->tiempoF,un_proceso->tiempoI));
        pthread_mutex_unlock(&mutex_log);
    }
    return NULL;
}

bool el_proceso_esta_bloqueado(t_proceso *un_proceso)
{
    pthread_mutex_lock(&un_proceso->mutex_proceso);
    bool una_condicion = un_proceso->un_pcb->un_estado == BLOCKED;
    pthread_mutex_unlock(&un_proceso->mutex_proceso);

    return una_condicion;
}

///////////////////////////////////////////

