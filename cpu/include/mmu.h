#ifndef UNIDADMMU_H_INCLUDED
#define UNIDADMMU_H_INCLUDED

#include "main.h"

void iniciar_mmu();

void *obtener_dato_memoria(dir_logica, t_pcb *);
int escribir_dato_memoria(dir_logica, void *, t_pcb *);

char *dir_logica_a_fisica(dir_logica dir_logica, t_pcb *pcb, t_accion accion);

#endif /* MMU_H_INCLUDED */