#ifndef INCLUDE_MEMORIA_CONTROLADOR_H_
#define INCLUDE_MEMORIA_CONTROLADOR_H_

#include "main.h"

/**
 * @DESC Obtiene un frame libre a partir de una tabla de pagina de 2do nivel, si hay uno
 * disponible (en free = true) lo retorna, sino, realiza el algoritmo de reemplazo asignado
 * @param t_tabla_pagina *tabla_2n
 * @param t_col_pagina *registro_a_llenar
 * @param int numero_de_pagina
 * @return t_frame*
 */
t_frame *obtener_frame_libre(t_tabla_pagina *tabla_pagina, t_col_pagina *registro, int nro_pagina);
t_frame *recorrer_frames(t_tabla_pagina *tabla_pagina);
t_frame *realizar_algoritmo(t_tabla_pagina *tabla_pagina, t_col_pagina *registro, accion accion, int nro_pagina);

#endif /* INCLUDE_MEMORIA_CONTROLADOR_H_ */
