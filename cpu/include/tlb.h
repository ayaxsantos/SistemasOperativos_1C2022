#ifndef CPU_TLB_H
#define CPU_TLB_H

#include "main.h"

void iniciar_tlb();
void actualizar_tlb(unsigned nro_frame, int nro_pagina);
void limpiar_entrada_tlb(int nro_pagina);
void limpiar_tlb();

int obtener_nro_frame_de_tlb(int nro_pagina, unsigned int *nro_frame);
t_columna_tlb *buscar_registro_tlb(int nro_pagina);
bool buscar_por_numero_pagina(void *arg, int nro_pagina);
void actualizar_tiempo_usado_tlb(t_columna_tlb *registro);

void validar_marco_unico(unsigned int nro_frame);
bool buscar_por_numero_marco(void *arg, unsigned int nro_frame);

void realizar_algoritmo_reemplazo(unsigned int nro_frame, int nro_pagina);
bool ordenar_por_tiempo_desc_tlb(void *arg1, void *arg2);

void setear_algoritmo_reemplazo_tlb();
#endif //CPU_TLB_H
