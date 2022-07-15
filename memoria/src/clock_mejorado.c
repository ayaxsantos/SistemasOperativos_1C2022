#include "../include/clock_mejorado.h"

t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {

	t_frame *frame_a_asignar = NULL;

	while(frame_a_asignar == NULL) {
		frame_a_asignar = realizar_primer_paso(tabla_1n, registro, accion_memoria, entrada_tabla_1n, entrada_tabla_2n);
		if(!frame_a_asignar) {
			frame_a_asignar = realizar_segundo_paso(tabla_1n, registro, accion_memoria, entrada_tabla_1n, entrada_tabla_2n);
		}
	}

}

t_frame *realizar_primer_paso(t_tabla_pagina *tabla_1n, t_col_pagina *registro, accion accion_memoria,
							  int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
	t_frame *frame_a_asignar = NULL;
	t_frame_asignado *posible_frame_a_reemplazar;

	for(int i = 0; i <= cantidad_maxima_frames - 1 ; i++) {
		posible_frame_a_reemplazar = (t_frame_asignado*)list_get(tabla_1n->frames_asignados, tabla_1n->puntero);
		frame_a_asignar = (t_frame*)list_get(memoria_principal->frames, posible_frame_a_reemplazar->nro_frame);

		if(!frame_a_asignar->usado && !frame_a_asignar->modificado) {
			t_tabla_pagina *tabla_2n_pagina_victima = (t_tabla_pagina*)dictionary_get(tabla_1n->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_1n));
			t_col_pagina *registro_pagina_victima = (t_col_pagina*)dictionary_get(tabla_2n_pagina_victima->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_2n));

            realizar_page_fault(tabla_1n->id_tabla, calcular_nro_pagina(entrada_tabla_1n, entrada_tabla_2n),
            					frame_a_asignar->base);

			registro_pagina_victima->presencia = false;
			registro->nro_frame = posible_frame_a_reemplazar->nro_frame;
			registro->presencia = true;

			posible_frame_a_reemplazar->entrada_tabla_1n = entrada_tabla_1n;
			posible_frame_a_reemplazar->entrada_tabla_2n = entrada_tabla_2n;

			frame_a_asignar->modificado = accion_memoria;
			frame_a_asignar->usado = true;

			incrementar_puntero(tabla_1n);

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

	for(int i = 0; i <= cantidad_maxima_frames - 1 ; i++) {
		posible_frame_a_reemplazar = (t_frame_asignado*)list_get(tabla_1n->frames_asignados, tabla_1n->puntero);
		frame_a_asignar = (t_frame*)list_get(memoria_principal->frames, posible_frame_a_reemplazar->nro_frame);

		if(!frame_a_asignar->usado && frame_a_asignar->modificado) {
			t_tabla_pagina *tabla_2n_pagina_victima = (t_tabla_pagina*)dictionary_get(tabla_1n->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_1n));
			t_col_pagina *registro_pagina_victima = (t_col_pagina*)dictionary_get(tabla_2n_pagina_victima->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_2n));

			escribir_pagina_en_swap(tabla_1n->id_tabla, calcular_nro_pagina(posible_frame_a_reemplazar->entrada_tabla_1n,
			                         posible_frame_a_reemplazar->entrada_tabla_2n), frame_a_asignar->base);

            realizar_page_fault(tabla_1n->id_tabla,
                                calcular_nro_pagina(entrada_tabla_1n, entrada_tabla_2n),frame_a_asignar->base);

			registro_pagina_victima->presencia = false;
			registro->nro_frame = posible_frame_a_reemplazar->nro_frame;
			registro->presencia = true;

			posible_frame_a_reemplazar->entrada_tabla_1n = entrada_tabla_1n;
			posible_frame_a_reemplazar->entrada_tabla_2n = entrada_tabla_2n;

			frame_a_asignar->modificado = accion_memoria;
			frame_a_asignar->usado = true;

			incrementar_puntero(tabla_1n);

			return frame_a_asignar;
		} else {
			frame_a_asignar->usado = true;
			incrementar_puntero(tabla_1n);
		}
	}
	return NULL;
}