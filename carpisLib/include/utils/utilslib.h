#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include "../general/carpisLib.h"

t_operacion *crear_operacion(codigo_operacion cod_op);
void crear_buffer_operacion(t_operacion *operacion);
void setear_operacion(t_operacion *operacion, void *valor);
void *serializar_operacion(t_operacion *operacion, int bytes);
void enviar_operacion(t_operacion *operacion, int socket_cliente);
void eliminar_operacion(t_operacion *operacion) ;

int32_t recibir_direccion_memoria(int socket_cliente);

void mostrar_error(char *exmsj, t_log *logger);

#endif