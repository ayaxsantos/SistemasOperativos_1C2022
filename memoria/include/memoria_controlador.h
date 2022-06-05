#ifndef INCLUDE_MEMORIA_CONTROLADOR_H_
#define INCLUDE_MEMORIA_CONTROLADOR_H_

#include "main.h"

t_frame *obtener_frame_libre(t_tabla_pagina *tabla_pagina, t_col_pagina *registro, int nro_pagina);
t_frame *recorrer_frames(t_tabla_pagina *tabla_pagina);
t_frame *realizar_algoritmo(t_tabla_pagina *tabla_carpincho, t_col_pagina *registro, accion accion, int nro_pagina);

#endif /* INCLUDE_MEMORIA_CONTROLADOR_H_ */
