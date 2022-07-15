#include "../include/tlb.h"

void iniciar_tlb() {
    tlb = list_create();
    for (int i = 0; i < config_cpu.cantidad_entradas_tlb; i++) {
        t_columna_tlb *registro = malloc(sizeof(t_columna_tlb));
        registro->is_free = true;
        registro->pagina = UNDEFINED;
        registro->nro_marco = UNDEFINED;
        registro->entrada = i;
        list_add(tlb, registro);
    }
}

void actualizar_tlb(unsigned int nro_frame, int nro_pagina) {
    t_columna_tlb *registro;
    validar_marco_unico(nro_frame);
    //monitorear_registros_tlb();
    for(int i=0; i<config_cpu.cantidad_entradas_tlb; i++) {
        registro = (t_columna_tlb *)list_get(tlb, i);
        if(registro->is_free) {
            registro->is_free = false;
            registro->nro_marco = nro_frame;
            registro->pagina = nro_pagina;
            long tiempo;
            time_t tiempo_actual;
            tiempo = (long) time(&tiempo_actual);
            registro->tiempo_usado = tiempo;
            return;
        }
    }
    realizar_algoritmo_reemplazo(nro_frame, nro_pagina);
    //monitorear_registros_tlb();
}

void limpiar_tlb() {
    for (int i = 0; i < config_cpu.cantidad_entradas_tlb; i++) {
        t_columna_tlb *registro = list_get(tlb, i);
        registro->is_free = true;
        registro->pagina = UNDEFINED;
        registro->nro_marco = UNDEFINED;
    }
}

void limpiar_entrada_tlb(int nro_pagina) {
    t_columna_tlb *registro = buscar_registro_tlb(nro_pagina);
    if(!registro) {
        return;
    }
    registro->is_free = true;
    registro->pagina = UNDEFINED;
}

int obtener_nro_frame_de_tlb(int nro_pagina, unsigned int *nro_frame) {
    t_columna_tlb *registro = buscar_registro_tlb(nro_pagina);
    //MISS
    if(!registro) {
        //tlb_miss++;
        log_info(logger_cpu,"TLB Miss -> Numero de Pagina: %d", nro_pagina);
        return ERROR;
    }
    *nro_frame = registro->nro_marco;
    actualizar_tiempo_usado_tlb(registro);
    //tlb_hit++;
    log_info(logger_cpu,"TLB Hit -> Numero de Pagina: %d  Marco: %d", nro_pagina, registro->nro_marco);
    return 1;
}

t_columna_tlb *buscar_registro_tlb(int nro_pagina) {
    bool _buscar_por_numero_pagina(void *arg) {
        return buscar_por_numero_pagina(arg, nro_pagina);
    }
    t_columna_tlb *registro = NULL;
    registro = (t_columna_tlb *)list_find(tlb, _buscar_por_numero_pagina);
    return registro;
}

bool buscar_por_numero_pagina(void *arg, int nro_pagina) {
    t_columna_tlb *registro = (t_columna_tlb *) arg;
    return registro->pagina == nro_pagina;
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

void validar_marco_unico(unsigned int nro_frame) {
    bool _buscar_por_numero_marco(void *arg) {
        return buscar_por_numero_marco(arg, nro_frame);
    }
    t_columna_tlb *registro = NULL;
    registro = (t_columna_tlb *)list_find(tlb, _buscar_por_numero_marco);
    if(registro) {
        registro->is_free = true;
        registro->pagina = UNDEFINED;
    }
}

bool buscar_por_numero_marco(void *arg, unsigned int nro_frame) {
    t_columna_tlb *registro = (t_columna_tlb *) arg;
    return registro->nro_marco == nro_frame;
}

void realizar_algoritmo_reemplazo(unsigned int nro_frame, int nro_pagina) {
    log_info(logger_cpu,"Realizando algoritmo de reemplazo de entrada en TLB");
    t_list *registros_candidatos;
    registros_candidatos = list_sorted(tlb, ordenar_por_tiempo_desc_tlb);
    t_columna_tlb *registro = list_get(registros_candidatos, 0);
    //loggear_resultados(registro, nro_frame, nro_pagina, pid);
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

void setear_algoritmo_reemplazo_tlb() {
    if(!strcmp("FIFO",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = FIFO;
    }
    else if(!strcmp("LRU",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = LRU;
    }
    else {
        log_error(logger_cpu,"No se pudo setear el algoritmo de TLB. Error en el archivo config.");
    }
}
