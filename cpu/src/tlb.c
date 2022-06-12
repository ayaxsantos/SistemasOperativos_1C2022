#include "../include/tlb.h"

int obtener_nro_frame_de_tlb(int nro_pagina, int pid) {
    pthread_mutex_lock(&mutex_tlb);
    t_columna_tlb *registro = buscar_registro_tlb(nro_pagina, pid);
    //MISS
    if(!registro) {
        tlb_miss++;
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_memoria,"TLB Miss -> PID: %d  Numero de Pagina: %d", pid, nro_pagina);
        usleep(config_memoria.retardo_fallo_tlb);
        pthread_mutex_unlock(&mutex_logger);
        pthread_mutex_unlock(&mutex_tlb);
        return -1;
    }
    actualizar_tiempo_usado_tlb(registro);
    tlb_hit++;
    pthread_mutex_lock(&mutex_logger);
    log_info(logger_memoria,"TLB Hit -> PID: %d  Numero de Pagina: %d  Marco: %d", pid, nro_pagina, registro->nro_marco);
    usleep(config_memoria.retardo_acierto_tlb);
    pthread_mutex_unlock(&mutex_logger);
    pthread_mutex_unlock(&mutex_tlb);
    return registro->nro_marco;
}