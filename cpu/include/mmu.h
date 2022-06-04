#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "main.h"

void iniciar_mmu();

void *obtener_dato_memoria(dir_logica, t_pcb *);
int escribir_dato_memoria(dir_logica, void *, t_pcb *);

#endif /* MMU_H_INCLUDED */