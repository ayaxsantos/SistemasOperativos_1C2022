#include "../include/paginacion.h"

t_tabla_pagina *crear_tabla_principal(int tamanio){
	t_tabla_pagina *tabla_principal = inicializar_tabla(tamanio);
	crear_tablas_segundo_nivel(tabla_principal);

	return tabla_principal;
}

void crear_tablas_segundo_nivel(t_tabla_pagina *tabla_principal){
	char *nro_pag;
	int i;

	for (i=0; i < config_memoria.entradas_por_tabla; i++){
		nro_pag = string_itoa(i);
		agregar_pag_a_tabla_1n(tabla_principal, nro_pag);
	}
};

t_tabla_pagina *inicializar_tabla(int tamanio){
	t_tabla_pagina* nueva_tabla = malloc(sizeof(t_tabla_pagina));
	nueva_tabla->tabla = dictionary_create();
	nueva_tabla->tamanio_proceso = tamanio;
	nueva_tabla->puntero = 0;
	nueva_tabla->cantidad_hit = 0;
	nueva_tabla->cantidad_miss = 0;

	return nueva_tabla;
}

int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag){
	t_tabla_pagina *tabla_2n_aux = inicializar_tabla(tabla_proceso->tamanio_proceso);
	int i, resultado;

    for (i=0; i < config_memoria.entradas_por_tabla; i++){
    		nro_pag = string_itoa(i);
    		resultado = agregar_pag_a_tabla_2n(tabla_proceso, nro_pag);

    		if(resultado == -1) {
    			// pthread_mutex_lock(&mutex_logger);
    			log_error(logger_memoria, "El espacio de memoria del proceso está lleno.");
    			// pthread_mutex_unlock(&mutex_logger);
    			liberar_todas_las_paginas_del_proceso(tabla_proceso);
    			return -1;
    		}
    	}

    dictionary_put(tabla_proceso->tabla, nro_pag, tabla_2n_aux);
    return 0;
}

int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_2n, char *nro_pag){
    t_col_pagina *col = malloc(sizeof(t_col_pagina));
    t_frame *frame = obtener_frame_libre(tabla_2n, col, atoi(nro_pag));
    dictionary_put(tabla_2n->tabla, nro_pag, col);
    frame->nro_pagina_asignada = atoi(nro_pag);
    frame->is_free = false;
    return 0;
}

int get_frame(uint32_t dir_logica) {
	// TODO: Chequear qué dirección lógica pasa CPU
	return 0;
}

void modificar_bit_de_presencia_pagina(t_frame *frame, int valor){
    t_tabla_pagina *tabla_paginas = list_get(tablas_primer_nivel,frame->tabla_1n_asignada);
    t_col_pagina *registro = (t_col_pagina *) dictionary_get(tabla_paginas->tabla, string_itoa(frame->nro_pagina_asignada));
    registro->presencia = valor;
}

void liberar_todas_las_paginas_del_proceso(t_tabla_pagina* tabla_proceso){
    char *nro_pagina;
    t_col_pagina *pagina;
    t_frame *frame;
    int ultima_pagina = dictionary_size(tabla_proceso->tabla);
    for(int i=0; i<ultima_pagina;i++) {
        nro_pagina = string_itoa(i);
        pagina = (t_col_pagina *)dictionary_remove(tabla_proceso->tabla, nro_pagina);
        frame = list_get(memoria_principal->frames,pagina->nro_frame);
        frame->is_free = true;
        free(pagina);
    }
    dictionary_clean_and_destroy_elements(tabla_proceso->tabla, eliminar_columna_tabla);
    //TODO: Liberar en swap
}

void eliminar_columna_tabla(void *arg) {
    t_col_pagina *registro = (t_col_pagina*)arg;
    free(registro);
}
