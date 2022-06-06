#ifndef KERNEL_CONEXION_H
#define KERNEL_CONEXION_H

#include <main.h>

///////////////////////////////////////////

void conexion(void);
void realizar_handshake(int);
void *gestionar_comunicacion_con_proceso(void*);

void inicializar_proceso(int);
t_pcb *inicializar_pcb(int);
void pasar_proceso_a_new(t_proceso *);
int conectar_con_cpu(int);
void confirmar_modulo(int *socket, modulo modulo);

unsigned int obtener_tamanio(int);
t_list *obtener_instrucciones(int socket_proceso);
void asignar_pid(t_pcb *);

void responder_fin_proceso(int);
void responder_handshake(int);

void mapeador(int *,modulo);

/////////////// Auxiliares ////////////////

void mostrar_en_pantalla(t_instruccion*);
void probar_comunicacion_instrucciones(t_pcb*);

///////////////////////////////////////////

#endif //KERNEL_CONEXION_H
