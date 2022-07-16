#include "../include/swap.h"

void iniciar_swap() {
    formatear_swap();
}

void formatear_swap(){
    swap.raiz = malloc(sizeof(t_swap));
    swap.particiones = list_create();
}

void crear_archivo(int id, int id_tabla_1n, int tamanio_proceso){
	int truncado = 0, cerrado = 0;
	char *id_s = string_itoa(id);

	char *nombre_archivo = string_new();
		string_append(&nombre_archivo,"/proceso_");
		string_append(&nombre_archivo, id_s);
		string_append(&nombre_archivo,".swap");

	t_fcb *fcb_aux = malloc(sizeof(t_fcb));
		fcb_aux->id_archivo = id;
		fcb_aux->id_tabla = id_tabla_1n;
		fcb_aux->path_archivo = string_duplicate(config_memoria.path_swap);
								string_append(&(fcb_aux->path_archivo), nombre_archivo);
		fcb_aux->pags_en_archivo = formatear_pags_en_archivo(tamanio_proceso);

	t_particion *particion = malloc(sizeof(t_particion));
		particion->fcb = fcb_aux;
		particion->archivo = creat(fcb_aux->path_archivo, S_IRWXG | S_IRWXO | S_IRWXU);
		particion->tamanio = tamanio_proceso;

	if (particion->archivo != -1) {
		truncado = ftruncate(particion->archivo, tamanio_proceso);
		if (truncado != 0){ log_error(logger_memoria,"No se pudo establecer el tamanio del archivo."); }

		cerrado = close (particion->archivo);
		if (cerrado != 0){ log_error(logger_memoria,"No se pudo cerrar el archivo."); }

		list_add(swap.particiones, particion);
        log_info(logger_memoria,"El archivo swap del proceso %d se creó con éxito.", id);
	}
	else {
        log_error(logger_memoria,"No se pudo crear el archivo.");
	}

	free(id_s);
	free(nombre_archivo);
}

t_list* formatear_pags_en_archivo(int tamanio_proceso){
    t_list* pags_en_archivo = list_create();

    int pags_por_archivo = tamanio_proceso / config_memoria.entradas_por_tabla;

    for (int i = 0; i < pags_por_archivo; i++){
        t_pagina_swap* pag_aux = malloc(sizeof(t_pagina_swap));
			pag_aux->id_memoria = -1;
			pag_aux->is_free = true;
        list_add(pags_en_archivo, pag_aux);
    }

    return pags_en_archivo;
}

void destruir_archivo(int id){
	t_particion *particion = encontrar_particion_de(id);
	
	if(particion != NULL){
		for (int i = 0; i < list_size(particion->fcb->pags_en_archivo); i++){
			liberar_pagina_swap(i, particion);
		}
		list_destroy(particion->fcb->pags_en_archivo);
        remove(particion->fcb->path_archivo);
		free(particion->fcb->path_archivo);
		free(particion->fcb);
	}
	else{
		log_error(logger_memoria,"El proceso que se intenta destruir no existe en swap.");
	}
	free(particion);
}

void liberar_pagina_swap(int nro_pag_swap, t_particion* particion){
    t_pagina_swap* pagina_swap = list_get(particion->fcb->pags_en_archivo, nro_pag_swap);
    pagina_swap->id_memoria = -1;
    pagina_swap->is_free = 1;

	free(pagina_swap);
}
