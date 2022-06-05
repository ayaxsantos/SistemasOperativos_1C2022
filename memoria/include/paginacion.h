#ifndef PAGINACION_H_INCLUDED
#define PAGINACION_H_INCLUDED

#include "main.h"
#include "swap.h"
#include "swap_controlador.h"
#include "memoria_controlador.h"

t_tabla_pagina * crear_tabla_principal_para(int pid_proceso);
t_tabla_pagina *inicializar_tabla(int pid);
int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag);
void crear_tablas_segundo_nivel(t_tabla_pagina *tabla_principal);
int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_proceso, char *nro_pag);

int get_cantidad_total_paginas();
int get_nro_pagina(uint32_t dir_logica);

void modificar_bit_de_presencia_pagina(t_frame *frame, int valor);

void liberar_todas_las_paginas_del_proceso(t_tabla_pagina* tabla_proceso);
void eliminar_columna_tabla(void *arg);

#endif /* PAGINACION_H_INCLUDED */
