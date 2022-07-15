#ifndef MEMORIA_CLOCK_H
#define MEMORIA_CLOCK_H

#include "main.h"

t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t id_tabla_2n, int32_t entrada_tabla_2n);

#endif //MEMORIA_CLOCK_H
