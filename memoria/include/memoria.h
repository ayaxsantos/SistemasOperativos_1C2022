#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED

#include "main.h"

void iniciar_memoria();
void iniciar_proceso(t_pcb *proceso, int socket_cliente);
void terminar(int socket_cliente);

void gestionar_suspension(int socket_cliente);
void gestionar_acceso(int socket_cliente);
void gestionar_lectura(int cliente_fd);
void gestionar_escritura(int cliente_fd);

bool buscar_por_pid(void *unCarpincho, unsigned int pid);
t_tabla_pagina *encontrar_tabla_de_pid(unsigned int pid);
bool hay_espacio_en_memoria_para();

void leer_dato();
void escribir_dato();

#endif /* MEMORIA_H_INCLUDED */
