#include "../include/swap_controlador.h"

bool buscar_en_swap_por_pid(void *unaPagina, int pid) {
    t_pagina_swap *pagina = (t_pagina_swap *) unaPagina;
    return pagina->pid == pid;
}

t_particion* encontrar_particion_de(int pid){
    int i, j;
    for (i = 0; i < string_array_size(config_swap.archivos_swap); i++){
    	t_particion* particion_aux = list_get(swap.particiones,i);
        t_list* carpinchos_aux = particion_aux->fcb->procesos_en_archivo;
        for (j = 0; j < list_size(carpinchos_aux); j++){
            int *pid_aux = list_get(carpinchos_aux,j);
        	if (*pid_aux == pid){
                return list_get(swap.particiones,i);
            }
        }
    }

    return NULL;
}

int obtener_nro_pagina_libre(t_particion* particion){
    int i;
    for (i = 0; i < swap.header->cant_pags_por_particion; i++){
    	t_pagina_swap* pagina_aux = list_get(particion->fcb->pags_en_archivo,i);
        if (pagina_aux->is_free){
            return i;
        }
    }
    return -1;
}

int nro_pagina_en_swap(t_list* pags, int pid, int nro_pag_memoria){
    int i;
    for (i = 0; i < swap.header->cant_pags_por_particion; i++){
        t_pagina_swap* pag_aux = list_get(pags,i);
        if (pag_aux->id_memoria == nro_pag_memoria && pag_aux->pid == pid){
            return i;
        }
    }
    return -1;
}

int proceso_puede_pedir_mas_paginas(t_particion* particion, int pid){
    bool _buscar_por_pid(void *unaPagina) {
        return buscar_en_swap_por_pid(unaPagina, pid);
    }

    int paginas_pedidas = list_count_satisfying(particion->fcb->pags_en_archivo, _buscar_por_pid);

    return 0;
}

void marcar_pag_ocupada_por(int pid, int nro_pag_memoria, t_particion* particion){
    list_add(particion->fcb->procesos_en_archivo, &pid);

    int nro_pag_en_swap = nro_pagina_en_swap(particion->fcb->pags_en_archivo, pid, nro_pag_memoria);

    t_pagina_swap* pagina_swap = list_get(particion->fcb->pags_en_archivo, nro_pag_en_swap);
    pagina_swap->id_memoria = nro_pag_memoria;
    pagina_swap->pid = pid;
    pagina_swap->is_free = 0;
}

void liberar_pagina(int pid, int nro_pag_swap, t_particion* particion){
    t_pagina_swap* pagina_swap = list_get(particion->fcb->pags_en_archivo, nro_pag_swap);
    pagina_swap->id_memoria = -1;
    pagina_swap->pid = -1;
    pagina_swap->is_free = 1;
}

t_particion* asignar_particion_a(int pid){
    t_particion* particion_con_mas_lugar = list_get(swap.particiones, 0);
    int max_lugar_libre = 0, lugar_libre_de_particion_aux, i;

    for (i = 0; i < string_array_size(config_swap.archivos_swap); i++){
    	t_particion* particion_aux = list_get(swap.particiones,i);
        lugar_libre_de_particion_aux = calcular_pags_libres(particion_aux);

        if (lugar_libre_de_particion_aux > max_lugar_libre){
            particion_con_mas_lugar = particion_aux;
        }
    }
    int *id = malloc(sizeof(int));
    *id = pid;
    list_add(particion_con_mas_lugar->fcb->procesos_en_archivo, id);
    return particion_con_mas_lugar;
}

void asignar_pagina_a(int pid, t_particion* particion){
    // TODO Arreglar
	t_pagina_swap* solicitud;
	t_list* paginas = particion->fcb->pags_en_archivo;
    int i;
    for (i = 0; i < list_size(paginas); i++){
        t_pagina_swap* pag_aux = list_get(paginas,i);
        if (pag_aux->is_free){
            pag_aux->pid = solicitud->pid;
            pag_aux->id_memoria = solicitud->id_memoria;
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

int hay_espacio_en_swap_para(int pid){
	t_particion* particion = encontrar_particion_de(pid);
    if (!particion) {       // Busca lugar para pedir su primera página
        return hay_espacio_en_swap();
    }

	if (proceso_puede_pedir_mas_paginas(particion, pid) && obtener_nro_pagina_libre(particion) != -1){
		return 1;
	}
	return 0;
}

int hay_espacio_en_swap(){
    int i;
    for (i = 0; i < list_size(swap.particiones); i++){
    	t_particion* particion = list_get(swap.particiones,i);
        if (calcular_pags_libres(particion) > 0){
            return 1;
        }
    }

    return 0;
}
