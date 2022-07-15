#ifndef MEMORIA_CLOCK_MEJORADO_H
#define MEMORIA_CLOCK_MEJORADO_H

#include "main.h"

t_frame *realizar_algoritmo_clock_mejorado(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t id_tabla_2n, int32_t entrada_tabla_2n);
t_frame *realizar_primer_paso(t_tabla_pagina *tabla_1n, t_col_pagina *registro, accion accion_memoria,
							  int32_t id_tabla_2n, int32_t entrada_tabla_2n);
t_frame *realizar_segundo_paso(t_tabla_pagina *tabla_1n, t_col_pagina *registro, accion accion_memoria,
							  int32_t id_tabla_2n, int32_t entrada_tabla_2n);
void incrementar_puntero(t_tabla_pagina *tabla_1n);

#endif MEMORIA_CLOCK_MEJORADO_H
