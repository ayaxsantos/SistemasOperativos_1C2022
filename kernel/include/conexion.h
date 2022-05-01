#ifndef KERNEL_CONEXION_H
#define KERNEL_CONEXION_H

#include <main.h>

///////////////////////////////////////////

void conexion(void);
void realizar_handshake(int);
void *gestionar_comunicacion_con_proceso(void*);

void inicializar_proceso(int);
t_pcb *inicializar_pcb(int);

unsigned int obtener_tamanio(int);
t_list *obtener_instrucciones(int socket_proceso);
unsigned int obtener_id_hilo();

void responder_un_func_code(int,func_code);
void responder_fin_proceso(int);
void responder_handshake(int);

///////////////////////////////////////////

#endif //KERNEL_CONEXION_H
