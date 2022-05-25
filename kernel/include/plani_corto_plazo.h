#ifndef KERNEL_PLANI_CORTO_PLAZO_H
#define KERNEL_PLANI_CORTO_PLAZO_H

#include "main.h"

//////////////////////////////////////

void inicializar_plani_corto_plazo();
void lanzar_hilo_plani_corto_plazo(void* (*)(void*));
void *algoritmo_fifo(void*);
void *algoritmo_sjf_con_desalojo(void*);

//////////////////////////////////////

#endif //KERNEL_PLANI_CORTO_PLAZO_H
