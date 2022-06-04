#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "main.h"

void iniciar();
void esperar_a_kernel();
void enviar_confirmacion(int *socket, modulo modulo_solicitante);
void *ejecutar_pcb(void *arg);
void *ejecutar_interrupcion(void *arg);
void ciclo_de_instruccion();
int necesita_fetch_operands(instruccion instruction);
void ejecutar_instruccion(t_instruccion *instruccion, t_pcb *pcb);

void fin_de_proceso(int socket, t_pcb* un_pcb);

#endif /* CPU_H_INCLUDED */
