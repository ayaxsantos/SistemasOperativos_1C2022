#ifndef INCLUDE_SWAP_CONTROLADOR_H_
#define INCLUDE_SWAP_CONTROLADOR_H_

#include "swap.h"

bool buscar_en_swap_por_pid(void *unaPagina, int pid);
t_particion* encontrar_particion_de(int pid);

int obtener_nro_pagina_libre(t_particion* particion);
int nro_pagina_en_swap(t_particion *particion, int nro_pag_memoria);
void liberar_pagina(int pid, int nro_pag_swap, t_particion* particion);
int calcular_pags_libres(t_particion* particion);

#endif /* INCLUDE_SWAP_CONTROLADOR_H_ */
