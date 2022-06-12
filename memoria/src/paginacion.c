#include "../include/paginacion.h"

/* ---------- Inicialización ----------*/

t_tabla_pagina *crear_tabla_principal(int tamanio){
	t_tabla_pagina *tabla_principal = inicializar_tabla(tamanio);
	tabla_principal = cantidad_tablas_1n;
	cantidad_tablas_1n++;

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
	nueva_tabla->id_tabla = 0;
	nueva_tabla->tabla = dictionary_create();
	nueva_tabla->tamanio_proceso = tamanio;
	nueva_tabla->puntero = 0;
	nueva_tabla->cantidad_hit = 0;
	nueva_tabla->cantidad_miss = 0;

	return nueva_tabla;
}

int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag){
	t_tabla_pagina *tabla_2n_aux = inicializar_tabla(tabla_proceso->tamanio_proceso);
	tabla_2n_aux->id_tabla = nro_pag;
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

/* ---------- Utilización ---------- */

int iniciar_proceso_en_memoria(int tamanio){
	t_tabla_pagina * tabla_proceso = crear_tabla_principal(tamanio);
	return tabla_proceso->id_tabla;
}

int primera_solicitud_mmu(int id_tabla_1n, int entrada_tabla_1n){
	t_tabla_pagina *tabla_1n = obtener_tabla_1n_por_id(id_tabla_1n);
	t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, string_itoa(entrada_tabla_1n));
	return tabla_2n->id_tabla;
}

bool buscar_por_id(void *una_tabla, unsigned int id) {
    t_tabla_pagina *tabla_proceso = (t_tabla_pagina *) una_tabla;
    return tabla_proceso->id_tabla == id;
}

t_tabla_pagina* obtener_tabla_1n_por_id(unsigned int id_buscado){
    //pthread_mutex_lock(&mutex_lista_tablas_paginas);
    bool _buscar_por_id(void *una_tabla) {
        return buscar_por_id(buscar_por_id, id);
    }
    t_tabla_pagina *tabla_pagina = (t_tabla_pagina *)list_find(tablas_primer_nivel, _buscar_por_id);
    //pthread_mutex_unlock(&mutex_lista_tablas_paginas);
    if(tabla_pagina == NULL) { return false; }		// ¿Se encontró una tabla asociada al ID? TODO: Debugear
    return tabla_pagina;
}

t_frame* get_frame(uint32_t dir_logica) {
	/*  */
}

void modificar_bit_de_presencia_pagina(t_frame *frame, int valor){
    t_tabla_pagina *tabla_paginas = list_get(tablas_primer_nivel,frame->tabla_1n_asignada);
    t_col_pagina *registro = (t_col_pagina *) dictionary_get(tabla_paginas->tabla, string_itoa(frame->nro_pagina_asignada));
    registro->presencia = valor;
}

/* ---------- Cierre ----------*/

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
