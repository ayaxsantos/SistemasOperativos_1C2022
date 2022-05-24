#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "main.h"

void iniciar();
void esperar_a_kernel();
void *ejecutar_pcb(void *arg);
void *ejecutar_interrupcion(void *arg);

#endif /* CPU_H_INCLUDED */