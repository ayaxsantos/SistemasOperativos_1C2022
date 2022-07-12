#ifndef UNIDADMMU_H_INCLUDED
#define UNIDADMMU_H_INCLUDED

#include "main.h"

void iniciar_mmu();

uint32_t obtener_dato_memoria(dir_logica dir);
int escribir_dato_memoria(dir_logica dir, uint32_t dato);
int procesar_solicitud(dir_logica dir, accion accion_pedida, uint32_t dato);

t_dictionary *solicitar_tabla_2nivel(int);

/////////////////////////////////////////////////////////////
int numero_pagina(dir_logica dir);
int32_t entrada_tabla_1er_nivel(int numero_pagina);
int entrada_tabla_2do_nivel(int numero_pagina);
int calcular_desplazamiento(dir_logica dir, int numero_pagina);

#endif /* MMU_H_INCLUDED */