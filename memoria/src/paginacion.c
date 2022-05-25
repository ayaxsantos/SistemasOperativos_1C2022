#include "../include/paginacion.h"

void crear_tabla_principal(){

} // tiene entradas_por_tabla filas que referencian a las entradas_por_tabla paginas de segundo nivel

void crear_tablas_segundo_nivel(){

}; // se crean memoria.cantidad_frames / entradas_por_tabla tablas de seg nivel


int get_cantidad_total_paginas() {
    return config_memoria.entradas_por_tabla*config_memoria.entradas_por_tabla;
}

int get_nro_pagina(uint32_t dir_logica) {
    return trunc((double) dir_logica / (double) config_memoria.tamanio_pagina);
}

int agregar_pag_a_tabla(t_tabla_pagina *tabla_pagina, char *nro_pag) {
    if(!hay_espacio_en_swap(tabla_pagina)) { //Es necesario el size??
        return -1;
    }
    if(dictionary_has_key(tabla_pagina->tabla, nro_pag)){
        return 0;
    }
    t_col_pagina *col = malloc(sizeof(t_col_pagina));
    t_frame *frame = obtener_frame_libre(tabla_pagina, col, atoi(nro_pag));
    dictionary_put(tabla_pagina->tabla, nro_pag, col);
    frame->nro_pagina_asignada = atoi(nro_pag);
    frame->pid_asignado = tabla_pagina->pid;
    frame->is_free = false;
    return 0;
}

int armar_tabla_paginas_fija(int size, t_tabla_pagina *tabla_pag_1n) {
    char *nro_pagina;
    int resultado;
    for(int i=0; i<cantidad_maxima_frames; i++) {
        nro_pagina = string_itoa(i);
        resultado = agregar_pag_a_tabla(tabla_pag_1n, nro_pagina);
        if(resultado == -1) {
            // pthread_mutex_lock(&mutex_logger);
            log_error(logger_memoria, "Espacio de memoria de llena del carpincho");
            // pthread_mutex_unlock(&mutex_logger);
            liberar_todas_las_paginas(tabla_pag_1n);
            return -1;
        }
    }
    int espacio_asignado = config_memoria.tamanio_pagina * cantidad_maxima_frames;
    if((espacio_asignado - size) < 0) {
        //Solicitar mas paginas con los mismo marcos
        resultado = armar_tabla_paginas_dinamicamente(size - espacio_asignado, espacio_asignado, tabla_pag_1n);
    }
    return resultado;
}

void modificar_bit_de_presencia_pagina(t_frame *frame, int value) {
    t_tabla_pagina *tabla_carpincho = encontrar_tabla_de_pid(frame->pid_asignado);
    t_col_pagina *registro = (t_col_pagina *) dictionary_get(tabla_carpincho->tabla, string_itoa(frame->nro_pagina_asignada));
    registro->presencia = value;
}

/*int liberar_paginas_si_es_posible(uint32_t dir_logica, t_tabla_pagina* tabla_carpincho) {
    int ultima_pagina = dictionary_size(tabla_carpincho->tabla);
    int nro_pagina_heap = get_nro_pagina(dir_logica) + 1;
    int size_a_liberar = (ultima_pagina * config_memoria.tamanio_pagina) - (dir_logica + sizeof(HeapMetadata));
    if(size_a_liberar > config_memoria.tamanio_pagina) {
        int dir_comienzo_pagina = (nro_pagina_heap * config_memoria.tamanio_pagina) - size_a_liberar;
        int nro_pag_a_liberar = get_nro_pagina(dir_comienzo_pagina);
        char *nro_pagina;
        t_col_pagina *pagina;
         t_frame *frame;
        while(nro_pag_a_liberar <= ultima_pagina - 1) {
            nro_pagina = string_itoa(nro_pag_a_liberar);
            pagina = (t_col_pagina *)dictionary_get(tabla_carpincho->tabla, nro_pagina);
            frame = list_get(memoria_principal->frames,pagina->nro_frame);
            frame->is_free = true;
            dictionary_remove(tabla_carpincho->tabla, nro_pagina);
            nro_pag_a_liberar++;
        }
        return 1;
    }
    return 0;
}*/

void liberar_todas_las_paginas(t_tabla_pagina* tabla_carpincho) {
    char *nro_pagina;
    t_col_pagina *pagina;
    t_frame *frame;
    int ultima_pagina = dictionary_size(tabla_carpincho->tabla);
    for(int i=0; i<ultima_pagina;i++) {
        nro_pagina = string_itoa(i);
        pagina = (t_col_pagina *)dictionary_remove(tabla_carpincho->tabla, nro_pagina);
        frame = list_get(memoria_principal->frames,pagina->nro_frame);
        frame->is_free = true;
        free(pagina);
    }
    //dictionary_clean_and_destroy_elements(tabla_carpincho->tabla, eliminar_columna_tabla);
    //TODO: Liberar en swamp
}
