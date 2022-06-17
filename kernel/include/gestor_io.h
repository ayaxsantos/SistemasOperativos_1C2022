#ifndef KERNEL_GESTOR_IO_H
#define KERNEL_GESTOR_IO_H

#include "main.h"

//////////////////////////////////////////

void *gestor_io();
void *mediano_plazo();
void inicializar_gestor_io();

bool el_proceso_no_esta_suspendido(t_proceso *);
bool el_proceso_tiene_que_suspenderse(t_proceso *);

time_t tiempoF, tiempoI;

//////////////////////////////////////////

#endif //KERNEL_GESTOR_IO_H
