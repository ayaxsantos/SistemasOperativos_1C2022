#ifndef PAGINACION_H_INCLUDED
#define PAGINACION_H_INCLUDED

#include "main.h"
#include "swap.h"
#include "swap_controlador.h"
#include "memoria_controlador.h"
#include <general/carpisLib.h>

/* ---------- Inicialización ---------- */
t_tabla_pagina * crear_tabla_principal(int tamanio);
t_tabla_pagina *inicializar_tabla();
int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag);
void crear_tablas_segundo_nivel(t_tabla_pagina *tabla_principal);
int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_proceso, char *nro_pag);

/* ---------- Utilización ---------- */
int iniciar_proceso_en_memoria(int tamanio);
void primera_solicitud_mmu(t_solicitud* solicitud);
void segunda_solicitud_mmu(t_solicitud* solicitud);
void tercera_solicitud_mmu(t_tercera_solicitud* solicitud);

int leer_dato_de_memoria(char *dir_fisica);
int escribir_dato_en_memoria(char *dir_fisica, int dato);

/* ---------- Auxiliares ---------- */
bool buscar_por_id(void *una_tabla, unsigned int id);
t_tabla_pagina* obtener_tabla_1n_por_id(unsigned int id_buscado);
void modificar_bit_de_presencia_pagina(t_frame *frame, int valor);

/* ---------- Cierre ---------- */
void liberar_todas_las_paginas_del_proceso(t_tabla_pagina* tabla_proceso);
void eliminar_columna_tabla(void *arg);

#endif /* PAGINACION_H_INCLUDED */
