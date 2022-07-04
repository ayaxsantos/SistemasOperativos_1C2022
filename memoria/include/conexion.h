#ifndef INCLUDE_CONEXION_H_
#define INCLUDE_CONEXION_H_

#include "main.h"

void *gestionar_conexion_kernel(void *arg);
void *gestionar_conexion_cpu(void *arg);

void validar_modulo(int *socket, modulo modulo_solicitante);
void responder_handshake(int *socket,modulo modulo_actual);

void gestionar_primera_solicitud();
void gestionar_segunda_solicitud();
void gestionar_tercera_solicitud();

void iniciar_proceso(int socket);
void terminar_proceso(int socket);

unsigned int get_thread_id();

#endif /* INCLUDE_CONEXION_H_ */
