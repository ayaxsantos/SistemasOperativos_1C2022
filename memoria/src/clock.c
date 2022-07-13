#include "../include/clock.h"
/*
 * Se apunta al próximo marco a reemplazar, pero:

 Si (Bit de uso == 0) ----> se reemplaza el marco.
 Si (Bit de uso == 1) ----> Bit de Uso = 0 y se apunta al siguiente marco y se vuelve a preguntar.
 */
t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
    char *entrada_tabla_1n_aux;
    char *entrada_tabla_2n_aux;
    //Comienza el verdadero algoritmo clock
    for (int i = 0; i < config_memoria.marcos_por_proceso; ++i) {
        t_frame_asignado *frame_asignado = list_get(tabla_1n->frames_asignados, tabla_1n->puntero);

    }
}