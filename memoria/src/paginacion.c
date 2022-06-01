#include "../include/paginacion.h"

t_tabla_pagina *crear_tabla_principal_para(int pid_proceso){
	t_tabla_pagina *tabla_principal = armar_tabla_paginas(pid_proceso);
	tabla_principal->tablas_segundo_nivel = list_create();
	crear_tablas_segundo_nivel(pid_proceso, tabla_principal);

	return tabla_principal;
}

void crear_tablas_segundo_nivel(int pid_proceso, t_tabla_pagina *tabla_principal){
	int cant_tablas_2n = memoria_principal->cantidad_frames / config_memoria.entradas_por_tabla;
	t_tabla_pagina *tabla_aux;
	int i = 0;

	for (i=0; i < cant_tablas_2n; i++){
		tabla_aux = armar_tabla_paginas(pid_proceso);
		tabla_aux->nro_tabla_2n = i;
		list_add(tabla_principal->tablas_segundo_nivel, tabla_aux);
		dictionary_put(tabla_principal->tabla, string_itoa(i), i);
	}

};

t_tabla_pagina *armar_tabla_paginas(int pid) {
	t_tabla_pagina *tabla_paginas = crear_tabla_inicializada(pid);

    int cant_frames = config_memoria.entradas_por_tabla;
    int resultado;
	char *nro_pagina;

    for(int i=0; i < cant_frames; i++) {
        nro_pagina = string_itoa(i);
        resultado = agregar_pag_a_tabla(tabla_paginas, nro_pagina);
        if(resultado == -1) {
            // pthread_mutex_lock(&mutex_logger);
            log_error(logger_memoria, "El espacio de memoria del proceso está lleno.");
            // pthread_mutex_unlock(&mutex_logger);
            liberar_todas_las_paginas(tabla_paginas);
            return -1;
        }
    }

    return resultado;
}

t_tabla_pagina *crear_tabla_inicializada(int pid){
	t_tabla_pagina* nueva_tabla = malloc(sizeof(t_tabla_pagina));
	nueva_tabla->tabla = dictionary_create();
	nueva_tabla->nro_tabla_2n = -1;
	nueva_tabla->pid = pid;
	nueva_tabla->puntero = 0;
	nueva_tabla->cantidad_hit = 0;
	nueva_tabla->cantidad_miss = 0;

	return nueva_tabla;
}

int get_cantidad_total_paginas() {
    return config_memoria.entradas_por_tabla*config_memoria.entradas_por_tabla;
}

// TODO: Chequear qué dirección lógica pasa CPU
int get_nro_pagina(uint32_t dir_logica) {
    return trunc((double) dir_logica / (double) config_memoria.tamanio_pagina);
}

int agregar_pag_a_tabla(t_tabla_pagina *tabla_proceso, char *nro_pag){
    t_col_pagina *col = malloc(sizeof(t_col_pagina));
    t_frame *frame = obtener_frame_libre(tabla_proceso, col, atoi(nro_pag));
    dictionary_put(tabla_proceso->tabla, nro_pag, col);
    frame->nro_pagina_asignada = atoi(nro_pag);
    frame->pid_asignado = tabla_proceso->pid;
    frame->is_free = false;
    return 0;
}

void modificar_bit_de_presencia_pagina(t_frame *frame, int valor){
    t_tabla_pagina *tabla_paginas = encontrar_tabla_de_pid(frame->pid_asignado);
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
    //TODO: Liberar en swamp
}

void eliminar_columna_tabla(void *arg) {
    t_columna_pagina *registro = (t_columna_pagina *)arg;
    free(registro);
}
