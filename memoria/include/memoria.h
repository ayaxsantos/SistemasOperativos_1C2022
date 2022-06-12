#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED

#include "main.h"

void iniciar_memoria();
void iniciar_proceso(int socket_cliente);
void terminar_proceso(int socket_cliente);

void gestionar_suspension(int socket_cliente);
void gestionar_acceso(int socket_cliente);
void gestionar_lectura(int cliente_fd);
void gestionar_escritura(int cliente_fd);

void leer_dato();
void escribir_dato();

#endif /* MEMORIA_H_INCLUDED */
