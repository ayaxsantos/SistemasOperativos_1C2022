#ifndef KERNEL_GESTOR_IO_H
#define KERNEL_GESTOR_IO_H

#include "main.h"
#include "plani_mediano_plazo.h"

//////////////////////////////////////////

void *gestor_io();
void inicializar_gestor_io();
bool el_proceso_esta_susp_bloqueado(t_proceso *);

//////////////////////////////////////////

#endif //KERNEL_GESTOR_IO_H
