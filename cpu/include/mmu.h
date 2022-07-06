#ifndef UNIDADMMU_H_INCLUDED
#define UNIDADMMU_H_INCLUDED

#include "main.h"

void iniciar_mmu();

uint32_t *obtener_dato_memoria(dir_logica);
int escribir_dato_memoria(dir_logica dir, uint32_t dato);
char *dir_logica_a_fisica(dir_logica dir, accion accion_pedida, int32_t size);
bool data_esta_en_una_pagina(dir_logica dir_logica, t_pag_incompleta *pag_incomplenta);

t_dictionary *solicitar_tabla_2nivel(int);

/////////////////////////////////////////////////////////////
int numero_pagina(dir_logica dir);
int32_t entrada_tabla_1er_nivel(int numero_pagina);
int entrada_tabla_2do_nivel(int numero_pagina);
int calcular_desplazamiento(dir_logica dir, int numero_pagina);

#endif /* MMU_H_INCLUDED */