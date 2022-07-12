#include "../include/clock.h"
/*
 * Se apunta al próximo marco a reemplazar, pero:

 Si (Bit de uso == 0) ----> se reemplaza el marco.
 Si (Bit de uso == 1) ----> Bit de Uso = 0 y se apunta al siguiente marco y se vuelve a preguntar.
 */
t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t id_tabla_2n, int32_t entrada_tabla_2n) {
    char *entrada_tabla_1n;
    char *entrada_tabla_2n_aux;
    if(tabla_1n->puntero == UNDEFINED) {
        for (int i = 0; i < dictionary_size(tabla_1n->tabla); ++i) {
            entrada_tabla_1n = string_itoa(i);
            t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, entrada_tabla_1n);
            //Deberia llenar la lista de frames con los asignados al proceso en el orden FIFO
            for (int j = 0; j < dictionary_size(tabla_2n->tabla); ++j) {
                entrada_tabla_2n_aux = string_itoa(j);
                t_col_pagina *registro_pagina = dictionary_get(tabla_2n->tabla, entrada_tabla_2n_aux);
                if(registro_pagina->presencia){
                    if(tabla_1n->puntero == UNDEFINED) {
                        tabla_1n->puntero = registro_pagina->nro_frame;
                    }
                    t_frame_asignado *marco = malloc(sizeof(t_frame_asignado));
                    marco->nro_frame = registro_pagina->nro_frame;
                    marco->id_tabla_2n = tabla_2n->id_tabla;
                    marco->entrada_tabla_2n = j;
                    list_add(tabla_1n->frames_asignados, marco);
                }
            }
        }
    }
    //Comienza el verdadero algoritmo clock
    for (int i = 0; i < config_memoria.marcos_por_proceso; ++i) {
        t_frame_asignado *frame_asignado = list_get(tabla_1n->frames_asignados, tabla_1n->puntero);

    }
}