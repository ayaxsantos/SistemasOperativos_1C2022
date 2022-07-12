#include "../include/paginacion.h"

/* ---------- Inicialización ----------*/

t_tabla_pagina *crear_tabla_principal(int tamanio){
	t_tabla_pagina *tabla_principal = inicializar_tabla(tamanio);
	tabla_principal->id_tabla = cantidad_tablas_1n;
	cantidad_tablas_1n++;

	crear_tablas_segundo_nivel(tabla_principal);

	return tabla_principal;
}

void crear_tablas_segundo_nivel(t_tabla_pagina *tabla_principal){
	char *nro_pag;
	int i;

	div_t tablas_2n_necesarias = div(tabla_principal->pags_necesarias, config_memoria.entradas_por_tabla);
	int direccionamiento_max = config_memoria.entradas_por_tabla * config_memoria.entradas_por_tabla;

	if (tabla_principal->pags_necesarias <= direccionamiento_max){
		for (i=0; i < tablas_2n_necesarias.quot; i++){
			nro_pag = string_itoa(i);
			agregar_pag_a_tabla_1n(tabla_principal, nro_pag);
		}

		if (tablas_2n_necesarias.rem > 0){
			agregar_ultima_pag_a_tabla_1n(tabla_principal, atoi(nro_pag) + 1);
		}
	}
	else
	{
		pthread_mutex_lock(&mutex_logger);
		log_error(logger_memoria, "El proceso que se intenta cargar en memoria es demasiado grande para la configuracion ingresada");
		pthread_mutex_unlock(&mutex_logger);
	}
};

int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag){
	t_tabla_pagina *tabla_2n_aux = inicializar_tabla(tabla_proceso->tamanio_proceso);
	tabla_2n_aux->id_tabla = atoi(nro_pag);
	int i;
	char *nro_pag_2n;

    for (i=0; i < config_memoria.entradas_por_tabla; i++){
		nro_pag_2n = string_itoa(i);
        agregar_pag_a_tabla_2n(tabla_2n_aux, nro_pag_2n, tabla_proceso->frames_asignados);
    }

    dictionary_put(tabla_proceso->tabla, nro_pag, tabla_2n_aux);
    return 0;
}

int agregar_ultima_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, int nro_ultima_pag){
	t_tabla_pagina *tabla_2n_aux = inicializar_tabla(tabla_proceso->tamanio_proceso);
	tabla_2n_aux->id_tabla = nro_ultima_pag;
	int i;
	char *nro_pag_2n;

	int pags_necesarias_ultima_tabla = tabla_proceso->pags_necesarias - config_memoria.entradas_por_tabla * (nro_ultima_pag-1);

	for (i=0; i < pags_necesarias_ultima_tabla; i++){
		nro_pag_2n = string_itoa(i);
		agregar_pag_a_tabla_2n(tabla_2n_aux, nro_pag_2n, tabla_proceso->frames_asignados);
	}

	dictionary_put(tabla_proceso->tabla, string_itoa(nro_ultima_pag), tabla_2n_aux);
	return 0;
}

int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_2n, char *nro_pag, int frames_asignados) {
    t_col_pagina *col = malloc(sizeof(t_col_pagina));
    t_frame *frame;
    if(frames_asignados < config_memoria.marcos_por_proceso) {
        frame = recorrer_frames(tabla_2n);
    }
    else {
        frame = realizar_algoritmo(tabla_2n,col,READ_ACCION, atoi(nro_pag));
    }
    col->presencia = true;
    col->nro_frame = frame->nro_frame;

    dictionary_put(tabla_2n->tabla, nro_pag, col);

    frame->usado = 1;
    frame->modificado = READ_ACCION;
    frame->nro_pagina_asignada = atoi(nro_pag);
    frame->is_free = false;
    return 0;
}

t_tabla_pagina *inicializar_tabla(int tamanio){
	t_tabla_pagina* nueva_tabla = malloc(sizeof(t_tabla_pagina));
	nueva_tabla->id_tabla = 0;
	nueva_tabla->tabla = dictionary_create();
	nueva_tabla->tamanio_proceso = tamanio;
	nueva_tabla->pags_necesarias = tamanio / config_memoria.tamanio_pagina;
	nueva_tabla->puntero = 0;

	return nueva_tabla;
}

/* ---------- Utilización ---------- */

int iniciar_proceso_en_memoria(int tamanio){
	t_tabla_pagina * tabla_proceso = crear_tabla_principal(tamanio);
	return tabla_proceso->id_tabla;
}

void primera_solicitud_mmu(t_solicitud* solicitud){
	t_tabla_pagina *tabla_1n = obtener_tabla_1n_por_id(solicitud->id_tabla_1n);
	t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, string_itoa(solicitud->entrada_tabla));

	solicitud->id_tabla_2n = tabla_2n->id_tabla;
}

void segunda_solicitud_mmu(t_solicitud* solicitud){
	t_tabla_pagina *tabla_1n = obtener_tabla_1n_por_id(solicitud->id_tabla_1n);
	t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, string_itoa(solicitud->id_tabla_2n));
	t_col_pagina *pagina = dictionary_get(tabla_2n->tabla, string_itoa(solicitud->entrada_tabla));

	if (pagina->presencia){
		solicitud->nro_frame = pagina->nro_frame;
	}
	else{
		t_frame *frame = realizar_algoritmo(tabla_2n, pagina, READ_ACCION, solicitud->entrada_tabla);
		solicitud->nro_frame = frame->nro_frame;
	}
}

void tercera_solicitud_mmu(t_tercera_solicitud *solicitud){
	int respuesta;

	if (solicitud->accion_solicitada == READ_ACCION){
        //TODO: Calcular la dir fisica a partir del nro de frame y desplazamiento
		//respuesta = leer_dato_de_memoria(solicitud->direccion_fisica);
		solicitud->dato = string_itoa(respuesta);
	} else if (solicitud->accion_solicitada == WRITE_ACCION){
		//respuesta = escribir_dato_en_memoria(solicitud->direccion_fisica, solicitud->dato);
		if (respuesta) {
			solicitud->estado_memo = WRITE_OK;
		} else {
			solicitud->estado_memo = WRITE_FAULT;
		}
	} else {
		log_info(logger_memoria,"Accion no valida.");
	}
}

int leer_dato_de_memoria(char *dir_fisica){
    char* dato = NULL;

    memcpy(dato, dir_fisica, sizeof(int));

    return atoi(dato);
}

int escribir_dato_en_memoria(char *dir_fisica, int dato){
	int status = 0;

	// Chequear que se pueda escribir ahi TODO
	memcpy(dir_fisica, string_itoa(dato), sizeof(int));
	status = 1;

	return status;
}

/* ---------- Auxiliares ---------- */
bool buscar_por_id(void *una_tabla, unsigned int id) {
    t_tabla_pagina *tabla_proceso = (t_tabla_pagina *) una_tabla;
    return tabla_proceso->id_tabla == id;
}

t_tabla_pagina* obtener_tabla_1n_por_id(unsigned int id_buscado){
    pthread_mutex_lock(&mutex_lista_tablas_paginas);
    bool _buscar_por_id(void *una_tabla) {
        return buscar_por_id(buscar_por_id, id_buscado);
    }
    t_tabla_pagina *tabla_pagina = (t_tabla_pagina *)list_find(tablas_primer_nivel, _buscar_por_id);
    pthread_mutex_unlock(&mutex_lista_tablas_paginas);
    if(tabla_pagina == NULL) { return false; }		// ¿Se encontró una tabla asociada al ID? TODO: Debugear
    return tabla_pagina;
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
}

void eliminar_columna_tabla(void *arg) {
    t_col_pagina *registro = (t_col_pagina*)arg;
    free(registro);
}