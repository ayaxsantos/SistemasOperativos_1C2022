#ifndef INCLUDE_SWAP_CONTROLADOR_H_
#define INCLUDE_SWAP_CONTROLADOR_H_

#include "swap.h"

t_particion* encontrar_particion_de_pid(int pid);
t_particion* asignar_particion_a(int pid);

int obtener_nro_pagina_libre(t_particion* particion);
int nro_pagina_en_swap(t_list* pags, int pid, int nro_pag_memoria);
int proceso_puede_pedir_mas_paginas(t_particion* particion, int pid);
void marcar_pag_ocupada_por(int pid, int nro_pag_memoria, t_particion* particion);
void liberar_pagina(int pid, int nro_pag_swap, t_particion* particion);
void asignar_pagina_a(int pid, t_particion* particion);
int calcular_pags_libres(t_particion* particion);
int hay_espacio_en_swap_para(int pid);
int hay_espacio_en_swap();

void gestionar_page_read();
void gestionar_page_write();
void gestionar_process_close();

#endif /* INCLUDE_SWAP_CONTROLADOR_H_ */
