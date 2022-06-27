#include "../include/tlb.h"

void iniciar_tlb() {
    tlb = list_create();
    for (int i = 0; i < config_cpu.cantidad_entradas_tlb; i++) {
        t_columna_tlb *registro = malloc(sizeof(t_columna_tlb));
        registro->is_free = true;
        registro->id_tabla_1n = UNDEFINED;
        registro->pagina = UNDEFINED;
        registro->entrada = i;
        list_add(tlb, registro);
    }
}

void actualizar_tlb(unsigned int nro_frame, int nro_pagina, int id_tabla_1n) {
    //pthread_mutex_lock(&mutex_tlb);
    t_columna_tlb *registro;
    //monitorear_registros_tlb();
    for(int i=0; i<config_cpu.cantidad_entradas_tlb; i++) {
        registro = (t_columna_tlb *)list_get(tlb, i);
        if(registro->is_free) {
            registro->id_tabla_1n = id_tabla_1n;
            registro->is_free = false;
            registro->nro_marco = nro_frame;
            registro->pagina = nro_pagina;
            long tiempo;
            time_t tiempo_actual;
            tiempo = (long) time(&tiempo_actual);
            registro->tiempo_usado = tiempo;
            //pthread_mutex_unlock(&mutex_tlb);
            return;
        }
    }
    realizar_algoritmo_reemplazo(nro_frame, nro_pagina, id_tabla_1n);
    //monitorear_registros_tlb();
    //pthread_mutex_unlock(&mutex_tlb);
}

void limpiar_entrada_tlb(int nro_pagina, int id_tabla_1n) {
    //pthread_mutex_lock(&mutex_tlb);
    t_columna_tlb *registro = buscar_registro_tlb(nro_pagina, id_tabla_1n);
    if(!registro) {
        //pthread_mutex_unlock(&mutex_tlb);
        return;
    }
    registro->is_free = true;
    registro->id_tabla_1n = UNDEFINED;
    registro->pagina = UNDEFINED;
    //pthread_mutex_unlock(&mutex_tlb);
}

int obtener_nro_frame_de_tlb(int nro_pagina, int id_tabla_1n) {
    //pthread_mutex_lock(&mutex_tlb);
    t_columna_tlb *registro = buscar_registro_tlb(nro_pagina, id_tabla_1n);
    //MISS
    if(!registro) {
        //tlb_miss++;
        //pthread_mutex_lock(&mutex_logger);
        log_info(logger_cpu,"TLB Miss -> PID: %d  Numero de Pagina: %d", id_tabla_1n, nro_pagina);
        //usleep(config_cpu.retardo_fallo_tlb);
        //pthread_mutex_unlock(&mutex_logger);
        //pthread_mutex_unlock(&mutex_tlb);
        return ERROR;
    }
    actualizar_tiempo_usado_tlb(registro);
    //tlb_hit++;
    //pthread_mutex_lock(&mutex_logger);
    log_info(logger_cpu,"TLB Hit -> ID TABLA PRIMER NIVEL: %d  Numero de Pagina: %d  Marco: %d", id_tabla_1n, nro_pagina, registro->nro_marco);
    //usleep(config_memoria.retardo_acierto_tlb);
    //pthread_mutex_unlock(&mutex_logger);
    //pthread_mutex_unlock(&mutex_tlb);
    return registro->nro_marco;
}

t_columna_tlb *buscar_registro_tlb(int nro_pagina, int id_tabla_1n) {
    bool _buscar_por_proceso_id(void *arg) {
        return buscar_por_proceso_id(arg, nro_pagina, id_tabla_1n);
    }
    t_columna_tlb *registro = NULL;
    registro = (t_columna_tlb *)list_find(tlb, _buscar_por_proceso_id);
    return registro;
}

bool buscar_por_proceso_id(void *arg, int nro_pagina, int id_tabla_1n) {
    t_columna_tlb *registro = (t_columna_tlb *) arg;
    return (registro->id_tabla_1n == id_tabla_1n) && (registro->pagina == nro_pagina);
}

void actualizar_tiempo_usado_tlb(t_columna_tlb *registro) {
    if(tipo_algoritmo_tlb == LRU) {
        long tiempo;
        time_t tiempo_actual;
        tiempo = (long) time(&tiempo_actual);
        registro->tiempo_usado = tiempo;
    }
    return;
}

void realizar_algoritmo_reemplazo(int nro_frame, int nro_pagina, int id_tabla_1n) {
    t_list *registros_candidatos;
    registros_candidatos = list_sorted(tlb, ordenar_por_tiempo_desc_tlb);
    t_columna_tlb *registro = list_get(registros_candidatos, 0);
    //loggear_resultados(registro, nro_frame, nro_pagina, pid);
    registro->id_tabla_1n = id_tabla_1n;
    registro->nro_marco = nro_frame;
    registro->pagina = nro_pagina;
    long tiempo;
    time_t tiempo_actual;
    tiempo = (long) time(&tiempo_actual);
    registro->tiempo_usado = tiempo;
    list_destroy(registros_candidatos);
}

bool ordenar_por_tiempo_desc_tlb(void *arg1, void *arg2) {
    t_columna_tlb *registro1 = (t_columna_tlb *)arg1;
    t_columna_tlb *registro2 = (t_columna_tlb *)arg2;
    return registro1->tiempo_usado < registro2->tiempo_usado;
}
