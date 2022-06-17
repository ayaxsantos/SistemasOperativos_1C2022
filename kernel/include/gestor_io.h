#ifndef KERNEL_GESTOR_IO_H
#define KERNEL_GESTOR_IO_H

#include "main.h"

//////////////////////////////////////////

void *gestor_io();
void *mediano_plazo();
void *mediano_plazo_pri(void*);
void inicializar_gestor_io();

bool el_proceso_no_esta_suspendido(t_proceso *);
bool el_proceso_tiene_que_suspenderse(t_proceso *);
void monitorear_estado_y_tiempo(t_proceso *);


//////////////////////////////////////////

#endif //KERNEL_GESTOR_IO_H
