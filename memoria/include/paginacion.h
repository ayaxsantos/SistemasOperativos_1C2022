#ifndef PAGINACION_H_INCLUDED
#define PAGINACION_H_INCLUDED

#include "main.h"

void crear_tabla_principal(); // tiene entradas_por_tabla filas que referencian a las tablas de segundo nivel
void crear_tablas_segundo_nivel(); // se crean entradas_por_tabla tablas de seg nivel

int get_cantidad_total_paginas();
int get_nro_pagina(uint32_t dir_logica);
int agregar_pag_a_tabla(t_tabla_pagina *tabla_pagina, char *nro_pag);

/**
 * @PARAMS: mate_pointer, t_tabla_pagina *
 * @DESC: Libera las paginas en memoria y swamp a partir de una direccion logica, tiene en cuenta
 * el tamaño de HeapMetadata para sumar a la direccion, ya que es un slot
 * @RETURN: 0 si no pudo liberar la pagina, 1 si libero la/s pagina/s
 */
int liberar_paginas_si_es_posible(uint32_t dir_logica, t_tabla_pagina* tabla_carpincho);
void liberar_todas_las_paginas(t_tabla_pagina* tabla_carpincho);

int armar_tabla_paginas(int size_total, int dir_logica_desde, t_tabla_pagina *tabla_carpi);
void modificar_bit_de_presencia_pagina(t_frame *frame, bool value);

int armar_tabla_paginas_dinamicamente(int size_total, int dir_logica_desde, t_tabla_pagina *tabla_carpi);
int armar_tabla_paginas_fija(int size, t_tabla_pagina *tabla_carpi);

#endif /* PAGINACION_H_INCLUDED */
