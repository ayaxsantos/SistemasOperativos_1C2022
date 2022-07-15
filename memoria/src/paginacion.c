#include "../include/paginacion.h"

/* ---------- Inicialización ----------*/

t_tabla_pagina *crear_tabla_principal(int tamanio){
	t_tabla_pagina *tabla_principal = inicializar_tabla(tamanio);
    tabla_principal->frames_asignados = list_create();
    tabla_principal->fue_suspendido = false;

    pthread_mutex_lock(&mutex_contador_tablas_1n);
    tabla_principal->id_tabla = cantidad_tablas_1n;
	cantidad_tablas_1n++;
    pthread_mutex_unlock(&mutex_contador_tablas_1n);

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
}

int agregar_pag_a_tabla_1n(t_tabla_pagina *tabla_proceso, char *nro_pag){
	t_tabla_pagina *tabla_2n_aux = inicializar_tabla(tabla_proceso->tamanio_proceso);
	tabla_2n_aux->id_tabla = atoi(nro_pag);
	int i;
	char *nro_pag_2n;

    for (i=0; i < config_memoria.entradas_por_tabla; i++){
		nro_pag_2n = string_itoa(i);
        agregar_pag_a_tabla_2n(tabla_2n_aux, nro_pag_2n);
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
		agregar_pag_a_tabla_2n(tabla_2n_aux, nro_pag_2n);
	}

	dictionary_put(tabla_proceso->tabla, string_itoa(nro_ultima_pag), tabla_2n_aux);
	return 0;
}

int agregar_pag_a_tabla_2n(t_tabla_pagina *tabla_2n, char *nro_pag) {
    t_col_pagina *col = malloc(sizeof(t_col_pagina));
    /*t_frame *frame;
    if(frames_asignados < config_memoria.marcos_por_proceso) {
        frame = recorrer_frames(tabla_2n);
    }
    else {
        frame = realizar_algoritmo(tabla_2n,col,READ_ACCION, atoi(nro_pag));
    }*/
    col->presencia = false;
    col->nro_frame = UNDEFINED;

    dictionary_put(tabla_2n->tabla, nro_pag, col);
    /*
    frame->usado = 1;
    frame->modificado = READ_ACCION;
    frame->nro_pagina_asignada = atoi(nro_pag);
    frame->is_free = false;*/
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
    entrada_tabla_1n_temporal = solicitud->entrada_tabla;

	solicitud->id_tabla_2n = tabla_2n->id_tabla;

	pthread_mutex_lock(&mutex_logger);
	log_info(logger_memoria, "Primera solicitud MMU recibida con exito.");
	pthread_mutex_unlock(&mutex_logger);
}

void segunda_solicitud_mmu(t_solicitud* solicitud){
	t_tabla_pagina *tabla_1n = obtener_tabla_1n_por_id(solicitud->id_tabla_1n);
	t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, string_itoa(entrada_tabla_1n_temporal));
	t_col_pagina *pagina = dictionary_get(tabla_2n->tabla, string_itoa(solicitud->entrada_tabla));

	if (!pagina->presencia){
        if(pagina->nro_frame != UNDEFINED) {
            t_frame *frame = realizar_algoritmo(tabla_2n, pagina, solicitud->accion_solicitada, entrada_tabla_1n_temporal,solicitud->entrada_tabla);
            solicitud->nro_frame = frame->nro_frame;
        }
        /*
         * Aca debo asignar el marco a la pagina
         * presencia: False && nro_frame: UNDEFINED
         */
        else{
            asignar_primer_marco_a_pagina(tabla_1n, tabla_2n, solicitud, pagina);
        }
	}
	else{
        solicitud->nro_frame = pagina->nro_frame;
	}

	pthread_mutex_lock(&mutex_logger);
	log_info(logger_memoria, "Segunda solicitud MMU recibida con exito.");
	pthread_mutex_unlock(&mutex_logger);
}

void tercera_solicitud_mmu(t_solicitud *solicitud){
	int respuesta;

	if (solicitud->accion_solicitada == READ_ACCION){
		respuesta = leer_dato_de_memoria(direccion_fisica(solicitud));
        if(respuesta < 0) {
            solicitud->estado_memo = READ_FAULT;
        }
        solicitud->dato = (uint32_t) respuesta;
	} else if (solicitud->accion_solicitada == WRITE_ACCION){
		respuesta = escribir_dato_en_memoria(direccion_fisica(solicitud), solicitud->dato);
		if (respuesta) {
			solicitud->estado_memo = WRITE_OK;
		} else {
			solicitud->estado_memo = WRITE_FAULT;
		}
	} else {
		log_info(logger_memoria,"Accion no valida.");
	}
    entrada_tabla_1n_temporal = UNDEFINED;
	pthread_mutex_lock(&mutex_logger);
	log_info(logger_memoria, "Tercera solicitud MMU recibida con exito.");
	pthread_mutex_unlock(&mutex_logger);
}

int leer_dato_de_memoria(char *dir_fisica){
    int dato;
    memcpy(&dato, dir_fisica, sizeof(int));
    return dato;
}

int escribir_dato_en_memoria(char *dir_fisica, uint32_t dato){
	int status = 0;

	memcpy(dir_fisica, &dato, sizeof(uint32_t));
	status = 1;

	return status;
}

char *direccion_fisica(t_solicitud *solicitud) {
    t_frame *frame = list_get(memoria_principal->frames, (int)solicitud->nro_frame);
    return frame->base + solicitud->desplazamiento;
}

void asignar_primer_marco_a_pagina(t_tabla_pagina *tabla_1n, t_tabla_pagina *tabla_2n, t_solicitud *solicitud,
                                   t_col_pagina *pagina) {
    t_frame *frame;
    int cantidad_frames_asignados = list_size(tabla_1n->frames_asignados);
    if(cantidad_frames_asignados < config_memoria.marcos_por_proceso) {
        frame = recorrer_frames();
        pagina->presencia = true;
        pagina->nro_frame = frame->nro_frame;
        if(tabla_1n->fue_suspendido) {
            realizar_page_fault(tabla_1n->id_tabla, calcular_nro_pagina(entrada_tabla_1n_temporal,solicitud->entrada_tabla),frame->base);
        }
        frame->usado = 1;
        frame->modificado = solicitud->accion_solicitada;
        frame->is_free = false;
        solicitud->nro_frame = frame->nro_frame;

        //Asigno el frame con los datos necesarios para realizar el algoritmo de reemplazo
        t_frame_asignado *frame_asignado = malloc(sizeof(t_frame_asignado));
        frame_asignado->nro_frame = frame->nro_frame;
        frame_asignado->entrada_tabla_2n = solicitud->entrada_tabla;
        frame_asignado->entrada_tabla_1n = entrada_tabla_1n_temporal;
        list_add(tabla_1n->frames_asignados, frame_asignado);
    }
    else {
        frame = realizar_algoritmo(tabla_2n, pagina, solicitud->accion_solicitada, entrada_tabla_1n_temporal,solicitud->entrada_tabla);
        solicitud->nro_frame = frame->nro_frame;
    }
}

/* ---------- Auxiliares ---------- */
bool buscar_por_id(void *una_tabla, unsigned int id) {
    t_tabla_pagina *tabla_proceso = (t_tabla_pagina *) una_tabla;
    return tabla_proceso->id_tabla == id;
}

t_tabla_pagina* obtener_tabla_1n_por_id(unsigned int id_buscado){
    pthread_mutex_lock(&mutex_lista_tablas_paginas);
    bool _buscar_por_id(void *una_tabla) {
        return buscar_por_id(una_tabla, id_buscado);
    }
    t_tabla_pagina *tabla_pagina = (t_tabla_pagina *)list_find(tablas_primer_nivel, _buscar_por_id);
    pthread_mutex_unlock(&mutex_lista_tablas_paginas);
    if(tabla_pagina == NULL) { return false; }		// ¿Se encontró una tabla asociada al ID? TODO: Debugear
    return tabla_pagina;
}

int calcular_nro_pagina(int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
    return entrada_tabla_1n * config_memoria.entradas_por_tabla + entrada_tabla_2n;
}

/* ---------- Cierre ----------*/

void liberar_todas_las_paginas_del_proceso(t_tabla_pagina* tabla_proceso){
    char *entrada_tabla_1n;
    char *entrada_tabla_2n;
    for (int i = 0; i < dictionary_size(tabla_proceso->tabla); ++i) {
        entrada_tabla_1n = string_itoa(i);
        t_tabla_pagina *tabla_2n = dictionary_get(tabla_proceso->tabla, entrada_tabla_1n);
        for (int j = 0; j < dictionary_size(tabla_2n->tabla); ++j) {
            entrada_tabla_2n = string_itoa(j);
            t_col_pagina *registro_pagina = dictionary_get(tabla_2n->tabla, entrada_tabla_2n);
            if(registro_pagina->presencia){
                registro_pagina->presencia = false;
                t_frame *frame = list_get(memoria_principal->frames,registro_pagina->nro_frame);
                frame->is_free = true;
            }
            registro_pagina->nro_frame = UNDEFINED;
            free(entrada_tabla_2n);
        }
        free(entrada_tabla_1n);
    }
    list_clean_and_destroy_elements(tabla_proceso->frames_asignados, liberar_frame_asignado);
    destruir_archivo(tabla_proceso->id_tabla);
    //dictionary_clean_and_destroy_elements(tabla_proceso->tabla, eliminar_columna_tabla);
}

void eliminar_columna_tabla(void *arg) {
    t_col_pagina *registro = (t_col_pagina*)arg;
    free(registro);
}
