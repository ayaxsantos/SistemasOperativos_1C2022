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

    }
}

void *algoritmo_sjf_con_desalojo(void *args)
{
    return NULL;
}
