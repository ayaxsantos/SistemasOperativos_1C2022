#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "main.h"

void iniciar();
void esperar_a_kernel();
void enviar_confirmacion(int *socket, modulo modulo_solicitante);

void *ejecutar_pcb(void *arg);
void *ejecutar_interrupcion(void *arg);
void *ciclo_de_instruccion(void *arg);

int necesita_fetch_operands(instruccion instruction);
void ejecutar_instruccion(t_instruccion *instruccion);
void desalojar_cpu();
bool hay_que_desalojar_cpu();

#endif /* CPU_H_INCLUDED */
