#include "../include/clock.h"
/*
 * Se apunta al próximo marco a reemplazar, pero:

 Si (Bit de uso == 0) ----> se reemplaza el marco.
 Si (Bit de uso == 1) ----> Bit de Uso = 0 y se apunta al siguiente marco y se vuelve a preguntar.
 */
t_frame *realizar_algoritmo_clock(t_tabla_pagina *tabla_1n, t_col_pagina *registro,
                                  accion accion_memoria, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {

	t_frame *frame_a_asignar = NULL;
	t_frame_asignado *posible_frame_a_reemplazar;

	int posicion_actual_puntero;

	while(frame_a_asignar == NULL) {
		posicion_actual_puntero = tabla_1n->puntero;
		posible_frame_a_reemplazar = (t_frame_asignado *)list_get(tabla_1n->frames_asignados, posicion_actual_puntero);
		frame_a_asignar = (t_frame*)list_get(memoria_principal->frames, posible_frame_a_reemplazar->nro_frame);

		if(frame_a_asignar->usado) { // BIT DE USO == 1
			frame_a_asignar->usado = false;
			frame_a_asignar = NULL;
			incrementar_puntero(tabla_1n);
		} else { 					// BIT DE USO == 0
			t_tabla_pagina *tabla_2n_pagina_victima = (t_tabla_pagina*)dictionary_get(tabla_1n->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_1n));
			t_col_pagina *registro_pagina_victima = (t_col_pagina*)dictionary_get(tabla_2n_pagina_victima->tabla, string_itoa(posible_frame_a_reemplazar->entrada_tabla_2n));

			if(frame_a_asignar->modificado) {
                escribir_pagina_en_swap(tabla_1n->id_tabla,
                                        calcular_nro_pagina(posible_frame_a_reemplazar->entrada_tabla_1n,
                                                            posible_frame_a_reemplazar->entrada_tabla_2n),
                                        frame_a_asignar->base);
			}
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
		}
	}
}

void incrementar_puntero(t_tabla_pagina *tabla_1n) {
	tabla_1n->puntero == (config_memoria.marcos_por_proceso - 1)? tabla_1n->puntero = 0 : tabla_1n->puntero++;
}
