#ifndef INCLUDE_SWAP_CONTROLADOR_H_
#define INCLUDE_SWAP_CONTROLADOR_H_

#include "swap.h"

void realizar_page_fault(int32_t id_tabla_1n, int nro_pagina, void *a_leer);
void escribir_pagina_en_swap(int32_t id_tabla_1n, int nro_pagina, void *a_escribir);

void marcar_pag_ocupada(int pid, int nro_pagina_en_memoria);
void swapear_proceso(t_tabla_pagina *tabla_1n);

/* ------ Auxiliares ------ */
t_particion* encontrar_particion_de(int tabla_1n);
int obtener_nro_pagina_libre(t_particion* particion);
int nro_pagina_en_swap(t_particion *particion, int nro_pag);
void asignar_pagina_a(t_particion* particion, int nro_pagina);
int calcular_pags_libres(t_particion* particion);

#endif /* INCLUDE_SWAP_CONTROLADOR_H_ */
