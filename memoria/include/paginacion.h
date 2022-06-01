#ifndef PAGINACION_H_INCLUDED
#define PAGINACION_H_INCLUDED

#include "main.h"

t_tabla_pagina * crear_tabla_principal_para(int pid_proceso);
void crear_tablas_segundo_nivel(int pid_proceso, t_tabla_pagina *tabla_principal);
t_tabla_pagina * armar_tabla_paginas(int pid);
t_tabla_pagina *crear_tabla_inicializada(int pid);

int get_cantidad_total_paginas();
int get_nro_pagina(uint32_t dir_logica);
int agregar_pag_a_tabla(t_tabla_pagina *tabla_proceso, char *nro_pag);

void modificar_bit_de_presencia_pagina(t_frame *frame, int valor);

void liberar_todas_las_paginas_del_proceso(t_tabla_pagina* tabla_proceso);
void eliminar_columna_tabla(void *arg);

#endif /* PAGINACION_H_INCLUDED */
