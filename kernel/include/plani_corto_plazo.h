#ifndef KERNEL_PLANI_CORTO_PLAZO_H
#define KERNEL_PLANI_CORTO_PLAZO_H

#include "main.h"

//////////////////////////////////////

void inicializar_plani_corto_plazo();
void lanzar_hilo_plani_corto_plazo_con(void* (*)(void*));
void *algoritmo_fifo(void*);
void *algoritmo_sjf_con_desalojo(void*);

void pasar_proceso_a_bloqueado(t_proceso *un_proceso);

//////////////////////////////////////

void enviar_pcb(int socket, t_pcb* un_pcb);
t_pcb *recibir_pcb();
void gestionar_pcb();

//////////////////////////////////////

#endif //KERNEL_PLANI_CORTO_PLAZO_H
