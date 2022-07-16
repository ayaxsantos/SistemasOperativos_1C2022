#ifndef PAGINACION_H_INCLUDED
#define PAGINACION_H_INCLUDED

#include "main.h"

/* ---------- Inicialización ---------- */
t_tabla_pagina * crear_tabla_principal(int tamanio);
void crear_tablas_segundo_nivel(t_tabla_pagina *tabla_principal);
int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag);
int agregar_ultima_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, int nro_ultima_pag);
int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_2n, char *nro_pag);
t_tabla_pagina *inicializar_tabla();

/* ---------- Utilización ---------- */
int iniciar_proceso_en_memoria(int tamanio);
void primera_solicitud_mmu(t_solicitud* solicitud);
void segunda_solicitud_mmu(t_solicitud* solicitud);
void tercera_solicitud_mmu(t_solicitud *solicitud);

char *direccion_fisica(t_solicitud *solicitud);
int leer_dato_de_memoria(char *dir_fisica);
int escribir_dato_en_memoria(char *dir_fisica, uint32_t dato);
void asignar_primer_marco_a_pagina(t_tabla_pagina *,t_tabla_pagina *tabla_2n, t_solicitud *solicitud,
                                   t_col_pagina *pagina);

/* ---------- Auxiliares ---------- */
bool buscar_por_id(void *una_tabla, unsigned int id);
bool _buscar_por_id(void *una_tabla);
t_tabla_pagina* obtener_tabla_1n_por_id(unsigned int id_buscado);
int calcular_nro_pagina(int32_t entrada_tabla_1n, int32_t entrada_tabla_2n);
void incrementar_puntero(t_tabla_pagina *tabla_1n);

/* ---------- Cierre ---------- */
void liberar_tabla_principal(t_tabla_pagina* tabla_principal);
void liberar_tablas_2n(t_dictionary* tabla_principal);
void eliminar_columna_tabla(void *arg);

#endif /* PAGINACION_H_INCLUDED */
