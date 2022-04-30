#ifndef KERNEL_CONEXION_H
#define KERNEL_CONEXION_H

#include <main.h>

///////////////////////////////////////////

void conexion(void);
void *gestionar_comunicacion_con_proceso(void*);

void responder_handshake(int);
void inicializar_proceso(int);
t_pcb *inicializar_pcb(int);

///////////////////////////////////////////

#endif //KERNEL_CONEXION_H
