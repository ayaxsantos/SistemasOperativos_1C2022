#ifndef CPU_CONECTOR_MEMORIA_H
#define CPU_CONECTOR_MEMORIA_H

#include "main.h"

int32_t solicitar_registro_1nivel(int id_tabla_1n, int entrada_tabla_1n);
unsigned int solicitar_registro_2nivel(int tabla2_n,int);
uint32_t solicitar_dato(int desplazamiento, unsigned int nro_frame);
int enviar_dato_memoria(int desplazamiento, unsigned int nro_frame, uint32_t dato);

void *gestionar_solicitud(void *request, codigo_operacion);

#endif //CPU_CONECTOR_MEMORIA_H
