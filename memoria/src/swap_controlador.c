#include "../include/swap_controlador.h"

void realizar_page_fault(void *data, int32_t id_tabla_1n, int32_t entrada_tabla_1n, int32_t entrada_tabla_2n) {
    // TODO
}

void gestionar_page_request(unsigned int pid, int pagina){
	void * pag_leida;
    t_particion* particion = encontrar_particion_de(pid);

    if (!particion){
    	log_info(logger_swap,"El proceso no tiene páginas en swap que pueda pedir.");
    }
    else{
        int nro_pag_en_swap = nro_pagina_en_swap(particion, pagina);
        if(nro_pag_en_swap > -1) {
        	int inicio_pag = nro_pag_en_swap * config_memoria.tamanio_pagina;

			int file = open(particion->fcb->path_archivo, O_RDWR);
			char* ptro_archivo = (char*)mmap(0, config_memoria.tamanio_pagina, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);

			void* buffer = malloc(config_memoria.tamanio_pagina);
			char c;
			int i, desplazamiento = 0;

			if (ptro_archivo != 0){
				for (i = inicio_pag; i < config_memoria.tamanio_pagina + inicio_pag; i++){
					c = ptro_archivo[i];
					memcpy(buffer + desplazamiento, &c, sizeof(char));
					desplazamiento+= sizeof(char);
				}
				pag_leida = buffer;

				int cod = munmap(ptro_archivo, config_memoria.tamanio_pagina);

				int cerrado = close (particion->archivo);
				if (cerrado != 0){ log_info(logger_swap,"No se pudo cerrar el archivo."); }

				/*if (cod == 0){
					marcar_pag_ocupada(pid, pagina);

					t_paquete_memoria *paquete = crear_paquete_memoria(WRITE_PAGE);
					setear_paquete_memoria(paquete, solicitud);
					enviar_paquete_memoria(paquete, socket_memoria);
					eliminar_paquete_memoria(paquete);
				}*/
			}
			free(buffer);
			return;
        }
    	log_info(logger_swap,"PF: El proceso no tiene la pagina pedida en swap.");
    }
}

void gestionar_page_write(unsigned int pid, int pagina, void* a_escribir){
    t_particion* particion = encontrar_particion_de(pid);

    if (!particion){
    	log_info(logger_swap,"Error en la recuperación del archivo en swap para este proceso.");
    }

    int nro_pag_en_swap = nro_pagina_en_swap(particion, pagina);
    if (nro_pag_en_swap == -1){                                            // Página nueva
		int nro_pagina_libre = obtener_nro_pagina_libre(particion);
		if (nro_pagina_libre > -1){
			asignar_pagina_a(particion, nro_pagina_libre);
		}
		nro_pag_en_swap = nro_pagina_libre;
    }
	else{
		log_info(logger_swap,"El proceso alcanzó el máximo número de páginas que puede pedir.");
	}

    int inicio_pag = nro_pag_en_swap * config_memoria.tamanio_pagina;

    int file = open(particion->fcb->path_archivo, O_RDWR);
    char* ptro_archivo = (char*) mmap(0, config_memoria.tamanio_pagina, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);

    if (ptro_archivo != 0){
        ptro_archivo[inicio_pag] = a_escribir;
        int desplazamiento = inicio_pag;
        int desplazamiento_data = 0;
        for(int i=inicio_pag;i<config_memoria.tamanio_pagina + inicio_pag; i++){
            memcpy(ptro_archivo + desplazamiento, a_escribir + desplazamiento_data, sizeof(char));
            desplazamiento += sizeof(char);
            desplazamiento_data += sizeof(char);
        }

        int cod = munmap(ptro_archivo, config_memoria.tamanio_pagina);
        if (cod != 0){ log_info(logger_swap,"No se pudo 'desmapear' el archivo."); }

        int cerrado = close (particion->archivo);
        if (cerrado != 0){ log_info(logger_swap,"No se pudo cerrar el archivo."); }

        log_info(logger_swap,"La escritura en swap se realizó con éxito.");
    }
}

/*
 * Analizar distintos casos:
 * 1. Primera vez que se envía toda la data a swap (hay basura en el archivo)
 * 2. No es necesario enviar toda la data, sino sólo las páginas modificadas
 *
 * Para pensar: ¿Vale la pena un page_write por cada página en el caso 1?
 * 				¿No se puede hacer más eficiente abriendo el archivo y escribiendo todo de una?
 * */
void swapear_tabla_completa(t_tabla_pagina *tabla_1n){
    /*
	int i, j;
	t_tabla_pagina *tabla_2n_aux;
	t_col_pagina *col_aux;
	t_frame *frame_aux;

	for (i = 0; i < dictionary_size(tabla_1n->tabla); i++){
		tabla_2n_aux = dictionary_get(tabla_1n->tabla, string_itoa(i));
		for (j = 0; i < dictionary_size(tabla_2n_aux->tabla); j++){
			col_aux = dictionary_get(tabla_2n_aux->tabla, string_itoa(j));
			col_aux->presencia = false;
			pthread_mutex_lock(&mutex_mp);
			frame_aux = list_get(memoria_principal->frames, col_aux->nro_frame);
			pthread_mutex_unlock(&mutex_mp);
			gestionar_page_write(pid, j, frame_aux->base);
		}
	}*/
    char *entrada_tabla_1n;
    char *nro_pag;
    for (int i = 0; i < dictionary_size(tabla_1n->tabla); ++i) {
        entrada_tabla_1n = string_itoa(i);
        t_tabla_pagina *tabla_2n = dictionary_get(tabla_1n->tabla, entrada_tabla_1n);
        for (int j = 0; j < dictionary_size(tabla_2n->tabla); ++j) {
            nro_pag = string_itoa(j);
            t_col_pagina *registro_pagina = dictionary_get(tabla_2n->tabla, nro_pag);
            if(registro_pagina->presencia){
                registro_pagina->presencia = false;
                t_frame *frame = list_get(memoria_principal->frames,registro_pagina->nro_frame);
                //El numero de pagina esta formado por: entrada_tabla_1n:entrada_tabla_2n
                //gestionar_page_write(tabla_1n->id_tabla, i, j, frame->base);
                frame->is_free = true;
            }
            registro_pagina->nro_frame = UNDEFINED;
            free(nro_pag);
        }
        free(entrada_tabla_1n);
    }
    tabla_1n->suspendido = true;
    list_clean_and_destroy_elements(tabla_1n->frames_asignados, liberar_frame_asignado);
}

void marcar_pag_ocupada(int pid, int nro_pagina_en_memoria){
	t_particion* particion = encontrar_particion_de(pid);
    int nro_pag_en_swap = nro_pagina_en_swap(particion, nro_pagina_en_memoria);

    t_pagina_swap* pagina_swap = list_get(particion->fcb->pags_en_archivo, nro_pag_en_swap);
    pagina_swap->id_memoria = nro_pagina_en_memoria;
    pagina_swap->is_free = 0;
}


/* ------ Auxiliares ------ */
t_particion* encontrar_particion_de(int tabla_1n){
	t_list *particiones = swap.particiones;
    for (int i = 0; i < list_size(particiones); i++){
    	t_particion* particion_aux = list_get(particiones,i);
        int id_tabla_1n_aux = particion_aux->fcb->id_tabla;
        if (tabla_1n == id_tabla_1n_aux){
        	return list_get(particiones,i);
        }
    }

    return NULL;
}

int obtener_nro_pagina_libre(t_particion* particion){
    for (int i = 0; i < list_size(particion->fcb->pags_en_archivo); i++){
    	t_pagina_swap* pagina_aux = list_get(particion->fcb->pags_en_archivo,i);
        if (pagina_aux->is_free){
            return i;
        }
    }
    return -1;
}

int nro_pagina_en_swap(t_particion *particion, int nro_pag_memoria){
	int pags_por_archivo = particion->tamanio / config_memoria.entradas_por_tabla;
	for (int i = 0; i < pags_por_archivo; i++){
        t_pagina_swap* pag_aux = list_get(particion->fcb->pags_en_archivo,i);
        if (pag_aux->id_memoria == nro_pag_memoria){
            return i;
        }
    }
    return -1;
}

void asignar_pagina_a(t_particion *particion, int nro_pagina){
    t_list* paginas = particion->fcb->pags_en_archivo;
    int i;
    for (i = 0; i < list_size(paginas); i++){
        t_pagina_swap* pag_aux = list_get(paginas,i);
        if (pag_aux->is_free){
            pag_aux->id_memoria = nro_pagina;
            pag_aux->is_free = 0;
            return;
        }
    }
}

int calcular_pags_libres(t_particion* particion){
    t_list* paginas = particion->fcb->pags_en_archivo;
    int paginas_libres = 0, i;
        for (i = 0; i < list_size(paginas); i++){
            t_pagina_swap* pag_aux = list_get(paginas,i);
            if (pag_aux->is_free){
                paginas_libres++;
            }
        }

    return paginas_libres;
}
