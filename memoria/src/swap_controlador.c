#include "../include/swap_controlador.h"

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

void liberar_pagina(int pid, int nro_pag_swap, t_particion* particion){
    t_pagina_swap* pagina_swap = list_get(particion->fcb->pags_en_archivo, nro_pag_swap);
    pagina_swap->id_memoria = -1;
    pagina_swap->is_free = 1;
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
