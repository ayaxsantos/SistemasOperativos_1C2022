#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include "../general/carpisLib.h"

void crear_buffer_funcion(t_funcion *funcion);
void setear_funcion(t_funcion* funcion, void *valor);
void *serializar_funcion(t_funcion* funcion, int bytes);
void enviar_funcion(t_funcion* funcion, int socket_cliente);
t_funcion* crear_funcion(func_code);
void eliminar_funcion(t_funcion *funcion);

int32_t recibir_direccion_memoria(int socket_cliente);

void mostrar_error(char *exmsj, t_log *logger);

#endif