#ifndef INCLUDE_MEMORIA_CONTROLADOR_H_
#define INCLUDE_MEMORIA_CONTROLADOR_H_

#include "main.h"

t_frame *recorrer_frames(t_tabla_pagina *tabla_pagina);
t_frame *realizar_algoritmo(t_tabla_pagina *tabla_pagina, t_col_pagina *pagina,
                            accion accion, int32_t id_tabla_2n, int32_t entrada_tabla_2n);

#endif /* INCLUDE_MEMORIA_CONTROLADOR_H_ */
