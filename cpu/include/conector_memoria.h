#ifndef CPU_CONECTOR_MEMORIA_H
#define CPU_CONECTOR_MEMORIA_H

#include "main.h"

t_columna_pagina *solicitar_registro_2nivel(int tabla2_n,int);
int solicitar_registro_1nivel(int id_tabla_1n, int entrada_tabla_1n);
void *gestionar_solicitud(t_request_tabla *request, codigo_operacion codigo_response);

int32_t *recibir_registro_1nivel(int socket);
t_columna_pagina *recibir_registro_2nivel(int socket);

#endif //CPU_CONECTOR_MEMORIA_H
