#include "../include/clock.h"
/*
 * Se apunta al próximo marco a reemplazar, pero:

 Si (Bit de uso == 0) ----> se reemplaza el marco.
 Si (Bit de uso == 1) ----> Bit de Uso = 0 y se apunta al siguiente marco y se vuelve a preguntar.
 */
t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t id_tabla_2n, int32_t entrada_tabla_2n) {
    t_frame *marco_de_proceso;
    char *entrada_tabla_1n;
    char *entrada_tabla_2n_aux;
    for (int i = 0; i < dictionary_size(tabla_1n->tabla); ++i) {
        entrada_tabla_1n = string_itoa(i);
        t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, entrada_tabla_1n);
        for (int j = 0; j < dictionary_size(tabla_2n->tabla); ++j) {
            entrada_tabla_2n_aux = string_itoa(tabla_1n->puntero);
            t_col_pagina *registro_pagina = dictionary_get(tabla_2n->tabla, entrada_tabla_2n_aux);
            if(registro_pagina->presencia){
                //marco_de_proceso = list_get(memoria_principal->frames,registro_pagina->nro_frame);
                tabla_1n->puntero = registro_pagina->nro_frame;
            }
            else {
                continue;
            }
        }
    }
}