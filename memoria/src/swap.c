#include "../include/swap.h"

t_log *logger_swap = NULL;

void iniciar_swap() {
	arrancar_logger_swap();
	habilitar_log(logger_swap, config_swap.log_habilitado);
    formatear_swap();
}

void arrancar_logger_swap(){
	t_log_level nivel_log = LOG_LEVEL_INFO;
	logger_swap = log_create("swap.log","swap",true,nivel_log);

	if(logger_swap ==  NULL){
		write(0,"ERROR -> No se pudo crear el log \n",30);
		exit(EXIT_FAILURE);
	}

	log_info(logger_swap,"--------- Log modulo SWAP ---------");
}

void formatear_swap(){
    swap.raiz = malloc(sizeof(t_swap)); //TODO: Liberar memoria
    swap.particiones = list_create();
}

void crear_archivo(int id, int tamanio_proceso){
	int truncado = 0, cerrado = 0;
	char *nombre_archivo = string_new();
		string_append(&nombre_archivo,"proceso_");
		string_append(&nombre_archivo, string_itoa(id));
		string_append(&nombre_archivo,".swap");

	t_fcb *fcb_aux = malloc(sizeof(t_fcb));
	fcb_aux->id_archivo = id;
	fcb_aux->path_archivo = nombre_archivo;
	fcb_aux->pags_en_archivo = formatear_pags_en_archivo(tamanio_proceso);

	t_particion *particion = malloc(sizeof(t_particion));
	particion->fcb = fcb_aux;
	particion->archivo = creat(nombre_archivo, S_IRWXG);

	if (particion->archivo != -1) {
		truncado = ftruncate(particion->archivo, tamanio_proceso);
		if (truncado != 0){ log_info(logger_swap,"No se pudo establecer el tamanio del archivo."); }

		cerrado = close (particion->archivo);
		if (cerrado != 0){ log_info(logger_swap,"No se pudo cerrar el archivo."); }

		list_add(swap.particiones, particion);
	}
	else {
		log_info(logger_swap,"No se pudo crear el archivo.");
	}
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
	t_particion* particion = encontrar_particion_de(id);

	if(particion != NULL){
		for (int i = 0; i < list_size(particion->fcb->pags_en_archivo); i++){
			liberar_pagina(i, particion);
			list_remove(particion->fcb->pags_en_archivo, i);
		}
	}
	else{
		log_info(logger_swap,"ERROR: El proceso que se intenta cerrar no existe en swap.");
	}
}
