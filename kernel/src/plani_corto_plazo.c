#include "../include/plani_corto_plazo.h"

////////////////////////////////////////////
// Generales, lanzamiento plani + funcion principal de este

void inicializar_plani_corto_plazo()
{
    pthread_mutex_init(&mutex_flag_interrupt,NULL);

    if(strcmp("FIFO",una_config_kernel.algoritmo_planificacion) == 0)
    {
        lanzar_hilo_plani_corto_plazo_con(algoritmo_fifo);
    }
    else if(strcmp("SRT",una_config_kernel.algoritmo_planificacion) == 0)
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

        proceso_en_exec->un_pcb->un_estado = EXEC;

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"Se pasa a EXEC el proceso PID = %u",proceso_en_exec->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);


        t_proceso_pcb *un_proceso_pcb = malloc(sizeof(t_proceso_pcb));
        un_proceso_pcb->pcb = proceso_en_exec->un_pcb;
        un_proceso_pcb->tiempo_bloqueo = proceso_en_exec->tiempo_bloqueo;

        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_proceso_pcb(socket_dispatch, un_proceso_pcb,PCB);
        pthread_mutex_unlock(&mutex_socket_dispatch);

        //gestionar_pcb_para_probar_sin_cpu();
        gestionar_pcb();

        free(un_proceso_pcb);
    }
}

void *algoritmo_sjf_con_desalojo(void *args)
{
    pthread_t *hilo_monitoreo_tiempos = malloc(sizeof(pthread_t));

    //Para el primer orden, asi sabemos cual tenemos que mandar a ejecutar!!
    //sem_wait(&hay_que_ordenar_cola_ready); //semaforo que avisa que hay procesos en ready para el monitoreo de desalojo

    pthread_create(hilo_monitoreo_tiempos, NULL, rutina_monitoreo_desalojo, NULL);
    pthread_detach(*hilo_monitoreo_tiempos);

    while(true)
    {
        sem_wait(&hay_procesos_en_ready);

        //Tomar el primer elemento de la lista
        pthread_mutex_lock(&mutex_procesos_en_ready);
        proceso_en_exec = list_remove(procesos_en_ready, 0);
        //sem_post(&hay_proceso_ejecutando);
        pthread_mutex_unlock( &mutex_procesos_en_ready);

        proceso_en_exec->un_pcb->un_estado = EXEC;

        //Tomamos tiempo inicial APENAS lo pasamos a EXEC
        time(&tiempoI);

        pthread_mutex_lock(&mutex_log);
        log_warning(un_logger,"Se pasa a EXEC el proceso PID = %u",proceso_en_exec->un_pcb->pid);
        pthread_mutex_unlock(&mutex_log);

        t_proceso_pcb *un_proceso_pcb = malloc(sizeof(t_proceso_pcb));
        un_proceso_pcb->pcb = proceso_en_exec->un_pcb;
        un_proceso_pcb->tiempo_bloqueo = proceso_en_exec->tiempo_bloqueo;

        pthread_mutex_lock(&mutex_socket_dispatch);
        enviar_proceso_pcb(socket_dispatch, un_proceso_pcb,PCB);
        pthread_mutex_unlock(&mutex_socket_dispatch);

        //gestionar_pcb_para_probar_sin_cpu();
        gestionar_pcb();
    }
}

void *rutina_monitoreo_desalojo(void *args)
{
    t_proceso *proceso_candidato;
    while(true)
    {
        //sem_wait(&hay_proceso_ejecutando);
        sem_wait(&hay_que_ordenar_cola_ready);

        //Tomamos le tiempo final, este se ira actualizando
        organizacionPlani();

        pthread_mutex_lock(&mutex_procesos_en_ready);
        if(!list_is_empty(procesos_en_ready))
        {
            proceso_candidato = list_get(procesos_en_ready,0);
            time(&tiempoF);

            if(proceso_en_exec != NULL && hay_que_desalojar(proceso_candidato))
            {
                pthread_mutex_lock(&mutex_log);
                log_info(un_logger, "Se debe desalojar al proceso con PID = %u",proceso_en_exec->un_pcb->pid);
                log_info(un_logger,"El proceso con PID = %u tiene una estimacion menor, de: %f",
                         proceso_candidato->un_pcb->pid,
                         proceso_candidato->un_pcb->una_estimacion);
                pthread_mutex_unlock(&mutex_log);

                solicitar_desalojo_a_cpu();
                sem_wait(&se_produjo_desalojo);
            }
        }
        pthread_mutex_unlock(&mutex_procesos_en_ready);
    }
}

void solicitar_desalojo_a_cpu()
{
    pthread_mutex_lock(&mutex_flag_interrupt);
    enviar_interrupcion(socket_interrupt);
    pthread_mutex_unlock(&mutex_flag_interrupt);
}

bool hay_que_desalojar(t_proceso *proceso_candidato)
{
    double tiempo_que_lleva = calcular_tiempo_ejecutando() * 1000;

    pthread_mutex_lock(&mutex_log);
    log_error(un_logger,"El tiempo que lleva es: %f",tiempo_que_lleva);
    pthread_mutex_unlock(&mutex_log);

    double tiempo_total = proceso_en_exec->un_pcb->una_estimacion - tiempo_que_lleva;

    if(tiempo_que_lleva <= 0)
        return false;
    else if (proceso_candidato->un_pcb->una_estimacion < tiempo_total)
        return true;
    else
        return false;
}

double calcular_tiempo_ejecutando()
{
    double tiempo_transcurrido_exec = difftime(tiempoF,tiempoI);
    return round(tiempo_transcurrido_exec);
}

void pasar_proceso_a_bloqueado()
{
    //Esta funcion va a encargarse de pasar los procesos a bloqueado al recibir I/O

    t_proceso *proceso_a_bloquear = proceso_en_exec;

    proceso_en_exec = NULL;
    proceso_a_bloquear->un_pcb->un_estado = BLOCKED;
    time(&proceso_a_bloquear->tiempoI);

    pthread_mutex_lock(&mutex_procesos_en_bloq);
    list_add(procesos_en_bloq,(void*) proceso_a_bloquear);
    pthread_mutex_unlock(&mutex_procesos_en_bloq);
    sem_post(&hay_procesos_en_blocked);
}

/////////////////////////////////////////////////

void realizar_envio_pcb(int socket, t_pcb *un_pcb)
{
    t_proceso_pcb *un_proceso_pcb = malloc(sizeof(t_proceso_pcb));

    un_proceso_pcb->tiempo_bloqueo = UNDEFINED;
    un_proceso_pcb->pcb = un_pcb;
    enviar_proceso_pcb(socket,un_proceso_pcb,PCB);
}

void gestionar_pcb()
{
    t_proceso_pcb *un_proceso_pcb;
    pthread_mutex_lock(&mutex_socket_dispatch);
    codigo_operacion un_codigo = (codigo_operacion)recibir_operacion(socket_dispatch);
    pthread_mutex_unlock(&mutex_socket_dispatch);

    switch(un_codigo)
    {
        case PCB:
            //todo este logger se podria eliminar o no?
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB desaloja!!");
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&mutex_socket_dispatch);
            un_proceso_pcb = deserializar_proceso_pcb(socket_dispatch);
            pthread_mutex_unlock(&mutex_socket_dispatch);

            free(proceso_en_exec->un_pcb);

            proceso_en_exec->un_pcb = un_proceso_pcb->pcb;
            proceso_en_exec->tiempo_bloqueo = un_proceso_pcb->tiempo_bloqueo;

            free(un_proceso_pcb);

            // CUANDO SE DESALOJA NO SE VUELVE A ESTIMAR!!
            //proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF,tiempoI,proceso_en_exec);

            devolver_proceso_a_ready(proceso_en_exec);
            break;
        case BLOQUEO:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB para bloquear!!");
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&mutex_socket_dispatch);
            t_proceso_pcb *proceso_para_bloquear = deserializar_proceso_pcb(socket_dispatch);
            pthread_mutex_unlock(&mutex_socket_dispatch);

            free(proceso_en_exec->un_pcb);

            proceso_en_exec->un_pcb = proceso_para_bloquear->pcb;
            proceso_en_exec->tiempo_bloqueo = proceso_para_bloquear->tiempo_bloqueo;

            free(proceso_para_bloquear);

            // Esto solo deberia ejecutarse con SJF, por ahora aqui pues no afecta!!
            proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF,tiempoI,proceso_en_exec);
            pasar_proceso_a_bloqueado();
            break;
        case FIN_PROCESO:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Volvio un PCB para finalizar!!");
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&mutex_socket_dispatch);
            un_proceso_pcb = deserializar_proceso_pcb(socket_dispatch);
            pthread_mutex_unlock(&mutex_socket_dispatch);

            free(proceso_en_exec->un_pcb);

            proceso_en_exec->un_pcb = un_proceso_pcb->pcb;
            proceso_en_exec->tiempo_bloqueo = un_proceso_pcb->tiempo_bloqueo;

            free(un_proceso_pcb);

            finalizar_proceso_ejecutando();
            break;
        default:
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"No se pudo recibir PCB!!");
            pthread_mutex_unlock(&mutex_log);
            break;
    }
}

void devolver_proceso_a_ready(t_proceso *un_proceso)
{
    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Se desaloja al proceso con PID: %u",un_proceso->un_pcb->pid);
    pthread_mutex_unlock(&mutex_log);


    //pthread_mutex_lock(&mutex_procesos_en_ready);
    list_add(procesos_en_ready,un_proceso);
    //pthread_mutex_unlock(&mutex_procesos_en_ready);

    proceso_en_exec = NULL;
    sem_post(&se_produjo_desalojo);

    //if(proceso_en_exec != NULL)
    //    sem_post(&hay_que_ordenar_cola_ready);

    //sem_post(&hay_que_ordenar_cola_ready);
    sem_post(&hay_procesos_en_ready);
}

/////////////////////////////////////////////////
// Exclusivas de SJF con desalojo

double calcular_estimacion(time_t tiempoF, time_t tiempoI, t_proceso *un_proceso)
{
    time(&tiempoF);
    double real_anterior = difftime(tiempoF,tiempoI);
    double alpha = una_config_kernel.alfa_plani;

    pthread_mutex_lock(&mutex_log);
    log_info(un_logger,"Tiempito -> Real Anterior: %lf ms",real_anterior * 1E3);
    pthread_mutex_unlock(&mutex_log);

    double estimacion_anterior = un_proceso->un_pcb->una_estimacion;

    return (estimacion_anterior * alpha) + real_anterior * (1-alpha);
    //Cuenta...
    //Ti = Ti1 * α + Ri1 * (1 - α)
}

void organizacionPlani()
{
    pthread_mutex_lock(&mutex_procesos_en_ready);
    bool comparador_procesos_SJF(void* proceso_primero, void* proceso_segundo)
    {
        return comparador_de_procesos_SJF((t_proceso*)proceso_primero,(t_proceso*)proceso_segundo);
    }
    list_sort(procesos_en_ready, comparador_procesos_SJF);

    //todo todo esto se va no?
    //SI, es solo para probar xd
    void mostrar_procesos_en_ready_inner(void *un_proceso)
    {
        mostrar_procesos_en_ready((t_proceso*)un_proceso);
    }
    list_iterate(procesos_en_ready,mostrar_procesos_en_ready_inner);
    pthread_mutex_unlock(&mutex_procesos_en_ready);
}

void mostrar_procesos_en_ready(t_proceso *un_proceso)
{
    pthread_mutex_lock(&mutex_log);
    log_warning(un_logger,"HEL PROZEZO EZ: pid: %d | Eztimazion: %f",
                un_proceso->un_pcb->pid,
                un_proceso->un_pcb->una_estimacion);
    pthread_mutex_unlock(&mutex_log);
}

bool comparador_de_procesos_SJF(t_proceso *un_proceso_primero, t_proceso *un_proceso_segundo)
{
    if(un_proceso_primero->un_pcb->una_estimacion == un_proceso_segundo->un_pcb->una_estimacion){
        return un_proceso_primero->un_pcb->pid < un_proceso_segundo->un_pcb->pid;
    }
    else return (un_proceso_primero->un_pcb->una_estimacion < un_proceso_segundo->un_pcb->una_estimacion);
}

/////////////////////////////////////////////////////
// FUNCION SOLO PARA PROBAR SIN NECESIDAD DE LEVANTAR CPU (FIFO ONLY)...
// SOLO PRUEBAS BASICAS DE PLANIFICACION CORTO PLAZO

void gestionar_pcb_para_probar_sin_cpu()
{
    while(true)
    {
        t_instruccion *una_instruccion = queue_pop(proceso_en_exec->un_pcb->consola->instrucciones);

        pthread_mutex_lock(&mutex_log);
        log_info(un_logger, "La instruccion ejecutando es: %u", una_instruccion->instruc);
        pthread_mutex_unlock(&mutex_log);

        if (una_instruccion->instruc == IO)
        {
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger, "Volvio un PCB para bloquear!!");
            pthread_mutex_unlock(&mutex_log);

            proceso_en_exec->tiempo_bloqueo = una_instruccion->parametro1;
            //proceso_en_exec->un_pcb->una_estimacion = calcular_estimacion(tiempoF, tiempoI, proceso_en_exec);

            free(una_instruccion);
            pasar_proceso_a_bloqueado();
            break;
        }
        else if (una_instruccion->instruc == I_EXIT)
        {
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger, "Volvio un PCB para finalizar!!");
            pthread_mutex_unlock(&mutex_log);

            free(una_instruccion);
            finalizar_proceso_ejecutando();
            break;
        }
        else if (esta_el_flag_interrupt_en_alto())
        {
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger, "HAY QUE DESALOJAR!!");
            pthread_mutex_unlock(&mutex_log);

            pthread_mutex_lock(&mutex_flag_interrupt);
            flag_interrupt = false;
            pthread_mutex_unlock(&mutex_flag_interrupt);
        }
        free(una_instruccion);

    }
}

bool esta_el_flag_interrupt_en_alto()
{
    bool valor;
    pthread_mutex_lock(&mutex_flag_interrupt);
    valor = flag_interrupt;
    pthread_mutex_unlock(&mutex_flag_interrupt);
    return valor;
}

//////////////////////////////////////////////////////
