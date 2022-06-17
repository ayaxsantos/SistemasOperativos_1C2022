#ifndef CPU_TLB_H
#define CPU_TLB_H

#include "main.h"

void iniciar_tlb();
void actualizar_tlb(unsigned nro_frame, int nro_pagina, int id_tabla_1n);
void limpiar_entrada_tlb(int nro_pagina, int id_tabla_1n);

int obtener_nro_frame_de_tlb(int nro_pagina, int id_tabla_1n);
t_columna_tlb *buscar_registro_tlb(int nro_pagina, int id_tabla_1n);
bool buscar_por_proceso_id(void *arg, int nro_pagina, int id_tabla_1n);
void actualizar_tiempo_usado_tlb(t_columna_tlb *registro);
void realizar_algoritmo_reemplazo(int nro_frame, int nro_pagina, int id_tabla_1n);
bool ordenar_por_tiempo_desc_tlb(void *arg1, void *arg2);
#endif //CPU_TLB_H
