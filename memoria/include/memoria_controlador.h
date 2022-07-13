#ifndef INCLUDE_MEMORIA_CONTROLADOR_H_
#define INCLUDE_MEMORIA_CONTROLADOR_H_

#include "main.h"

t_frame *recorrer_frames();
t_frame *realizar_algoritmo(t_tabla_pagina *tabla_pagina, t_col_pagina *pagina,
                            accion accion, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n);

void liberar_frame_asignado(void *frame_asignado);

#endif /* INCLUDE_MEMORIA_CONTROLADOR_H_ */
