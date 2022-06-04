#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "main.h"

void iniciar_mmu();

void *obtener_dato_memoria(dir_logica, t_pcb *);
int escribir_dato_memoria(dir_logica, void *, t_pcb *);

t_dictionary *solicitar_tabla_2nivel(int);

/////////////////////////////////////////////////////////////
int numero_pagina(dir_logica dir);
int entrada_tabla_1er_nivel(int numero_pagina);
int entrada_tabla_2do_nivel(int numero_pagina);
int calcular_desplazamiento(dir_logica dir, int numero_pagina);

#endif /* MMU_H_INCLUDED */