#ifndef KERNEL_PLANI_LARGO_PLAZO_H
#define KERNEL_PLANI_LARGO_PLAZO_H

#include "main.h"

////////////////////////////////////////////

void* planificador_largo_plazo();
t_proceso *obtener_proceso_en_new();
void transicionar_proceso_a_ready(t_proceso *);
void transicionar_proceso_a_exit();

////////////////////////////////////////////

#endif //KERNEL_PLANI_LARGO_PLAZO_H
