#ifndef KERNEL_PLANI_MEDIANO_PLAZO_H
#define KERNEL_PLANI_MEDIANO_PLAZO_H

#include "main.h"

//////////////////////////////////////

void inicializar_plani_mediano_plazo();
void *mediano_plazo_pri(void*);
void monitorear_estado_y_tiempo(t_proceso *);
void *mediano_plazo();
bool el_proceso_no_esta_suspendido(t_proceso *);
bool el_proceso_tiene_que_suspenderse(t_proceso *);

//////////////////////////////////////

#endif //KERNEL_PLANI_MEDIANO_PLAZO_H
