#ifndef INCLUDE_SWAP_H_
#define INCLUDE_SWAP_H_

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "main.h"

#define ERROR_SWAP_WRITE -2

////////////////////////////////////////////////

typedef struct pagina_swap{
    int id_memoria;         // id de la página en memoria
    bool is_free;
} t_pagina_swap;

typedef struct fcb{
    int id_archivo;
    int id_tabla;
    char* path_archivo;
    t_list* pags_en_archivo;
} t_fcb;

typedef struct particion{
    t_fcb* fcb;
    int archivo;
    int tamanio;
} t_particion;

////////////////////////////////////////////////

typedef struct swap{
	void* raiz;
    t_list* particiones;
} t_swap;

////////////////////////////////////////////////

t_swap swap;

////////////////////////////////////////////////

void iniciar_swap();

void formatear_swap();
void crear_archivo(int id, int id_tabla_1n ,int tamanio_proceso);
t_list* formatear_pags_en_archivo(int tamanio_proceso);

void destruir_archivo(int id_tabla);
void liberar_pagina(int nro_pag_swap, t_particion* particion);

#include "swap_controlador.h"

#endif /* INCLUDE_SWAP_H_ */
