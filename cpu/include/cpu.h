#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "main.h"

void iniciar();
void esperar_a_kernel();
void enviar_confirmacion(int *socket, modulo modulo_solicitante);
void *ejecutar_pcb(void *arg);
void *ejecutar_interrupcion(void *arg);
void recibir_pcb();

#endif /* CPU_H_INCLUDED */