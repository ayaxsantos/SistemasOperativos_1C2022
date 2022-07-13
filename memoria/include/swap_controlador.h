#ifndef INCLUDE_SWAP_CONTROLADOR_H_
#define INCLUDE_SWAP_CONTROLADOR_H_

#include "swap.h"

void realizar_page_fault(void *data, int nro_pagina, unsigned int pid);

void gestionar_page_request(unsigned int pid, int pagina);
void gestionar_page_write(unsigned int pid, int pagina, void* a_escribir);

void marcar_pag_ocupada(int pid, int nro_pagina_en_memoria);
void swapear_tabla_completa(unsigned int pid, t_tabla_pagina *tabla_1n);

/* ------ Auxiliares ------ */
t_particion* encontrar_particion_de(int tabla_1n);
int obtener_nro_pagina_libre(t_particion* particion);
int nro_pagina_en_swap(t_particion *particion, int nro_pag);
void asignar_pagina_a(t_particion* particion, int nro_pagina);
int calcular_pags_libres(t_particion* particion);

#endif /* INCLUDE_SWAP_CONTROLADOR_H_ */
