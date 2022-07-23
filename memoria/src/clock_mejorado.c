#include "../include/clock_mejorado.h"

t_frame *realizar_algoritmo_clock_mejorado(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {

	t_frame *frame_a_asignar = NULL;

	while(frame_a_asignar == NULL) {
		frame_a_asignar = realizar_primer_paso(tabla_1n, registro, accion_memoria, entrada_tabla_1n, entrada_tabla_2n);
		if(!frame_a_asignar) {
			frame_a_asignar = realizar_segundo_paso(tabla_1n, registro, accion_memoria, entrada_tabla_1n, entrada_tabla_2n);
		}
	}
    return frame_a_asignar;

}

t_frame *realizar_primer_paso(t_tabla_pagina *tabla_1n, t_col_pagina *registro, accion accion_memoria,
							  int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
	t_frame *frame_a_asignar = NULL;
	t_frame_asignado *posible_frame_a_reemplazar;
    char *entrada_tabla_1n_s;
    char *entrada_tabla_2n_s;

	for(int i = 0; i <= config_memoria.marcos_por_proceso - 1 ; i++) {
		posible_frame_a_reemplazar = (t_frame_asignado*)list_get(tabla_1n->frames_asignados, tabla_1n->puntero);
		frame_a_asignar = (t_frame*)list_get(memoria_principal->frames, posible_frame_a_reemplazar->nro_frame);

		if(!frame_a_asignar->usado && !frame_a_asignar->modificado) {
            entrada_tabla_1n_s = string_itoa(posible_frame_a_reemplazar->entrada_tabla_1n);
            entrada_tabla_2n_s = string_itoa(posible_frame_a_reemplazar->entrada_tabla_2n);
			t_tabla_pagina *tabla_2n_pagina_victima = (t_tabla_pagina*)dictionary_get(tabla_1n->tabla, entrada_tabla_1n_s);
			t_col_pagina *registro_pagina_victima = (t_col_pagina*)dictionary_get(tabla_2n_pagina_victima->tabla, entrada_tabla_2n_s);
            pthread_mutex_lock(&mutex_logger);
            log_warning(logger_memoria,"Pagina victima = %d ", calcular_nro_pagina(posible_frame_a_reemplazar->entrada_tabla_1n,
                                                                                   posible_frame_a_reemplazar->entrada_tabla_2n));
            pthread_mutex_unlock(&mutex_logger);
            pthread_mutex_lock(&mutex_swap);
            realizar_page_fault(tabla_1n->id_tabla, calcular_nro_pagina(entrada_tabla_1n, entrada_tabla_2n),
            					frame_a_asignar->base);
            pthread_mutex_unlock(&mutex_swap);

			registro_pagina_victima->presencia = false;
			registro->nro_frame = posible_frame_a_reemplazar->nro_frame;
			registro->presencia = true;

			posible_frame_a_reemplazar->entrada_tabla_1n = entrada_tabla_1n;
			posible_frame_a_reemplazar->entrada_tabla_2n = entrada_tabla_2n;

			frame_a_asignar->modificado = accion_memoria;
			frame_a_asignar->usado = true;

			incrementar_puntero(tabla_1n);

            free(entrada_tabla_1n_s);
            free(entrada_tabla_2n_s);

			return frame_a_asignar;
		} else {
			incrementar_puntero(tabla_1n);
		}
	}
	return NULL;
}

t_frame *realizar_segundo_paso(t_tabla_pagina *tabla_1n, t_col_pagina *registro, accion accion_memoria,
							  int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
	t_frame *frame_a_asignar = NULL;
	t_frame_asignado *posible_frame_a_reemplazar;
    char *entrada_tabla_1n_s;
    char *entrada_tabla_2n_s;

    for(int i = 0; i <= config_memoria.marcos_por_proceso  - 1 ; i++) {
		posible_frame_a_reemplazar = (t_frame_asignado*)list_get(tabla_1n->frames_asignados, tabla_1n->puntero);
		frame_a_asignar = (t_frame*)list_get(memoria_principal->frames, posible_frame_a_reemplazar->nro_frame);

		if(!frame_a_asignar->usado && frame_a_asignar->modificado) {
            entrada_tabla_1n_s = string_itoa(posible_frame_a_reemplazar->entrada_tabla_1n);
            entrada_tabla_2n_s = string_itoa(posible_frame_a_reemplazar->entrada_tabla_2n);
			t_tabla_pagina *tabla_2n_pagina_victima = (t_tabla_pagina*)dictionary_get(tabla_1n->tabla, entrada_tabla_1n_s);
			t_col_pagina *registro_pagina_victima = (t_col_pagina*)dictionary_get(tabla_2n_pagina_victima->tabla, entrada_tabla_2n_s);
            pthread_mutex_lock(&mutex_logger);
            log_warning(logger_memoria,"Pagina victima = %d ", calcular_nro_pagina(posible_frame_a_reemplazar->entrada_tabla_1n,
                                                                                   posible_frame_a_reemplazar->entrada_tabla_2n));
            pthread_mutex_unlock(&mutex_logger);
            pthread_mutex_lock(&mutex_swap);
			escribir_pagina_en_swap(tabla_1n->id_tabla, calcular_nro_pagina(posible_frame_a_reemplazar->entrada_tabla_1n,
			                         posible_frame_a_reemplazar->entrada_tabla_2n), frame_a_asignar->base);
            pthread_mutex_unlock(&mutex_swap);
            pthread_mutex_lock(&mutex_swap);
            realizar_page_fault(tabla_1n->id_tabla,
                                calcular_nro_pagina(entrada_tabla_1n, entrada_tabla_2n),frame_a_asignar->base);
            pthread_mutex_unlock(&mutex_swap);
			registro_pagina_victima->presencia = false;
			registro->nro_frame = posible_frame_a_reemplazar->nro_frame;
			registro->presencia = true;

			posible_frame_a_reemplazar->entrada_tabla_1n = entrada_tabla_1n;
			posible_frame_a_reemplazar->entrada_tabla_2n = entrada_tabla_2n;

			frame_a_asignar->modificado = accion_memoria;
			frame_a_asignar->usado = true;

			incrementar_puntero(tabla_1n);

            free(entrada_tabla_1n_s);
            free(entrada_tabla_2n_s);

			return frame_a_asignar;
		} else {
			frame_a_asignar->usado = false;
			incrementar_puntero(tabla_1n);
		}
	}
	return NULL;
}