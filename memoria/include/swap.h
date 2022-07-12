#ifndef INCLUDE_SWAP_H_
#define INCLUDE_SWAP_H_

#include <sys/types.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "main.h"

#define ERROR_SWAP_WRITE -2

////////////////////////////////////////////////

typedef struct config_swap{
    char* ip_swap;
    char* puerto_swap;
    int tamanio_swap;
    int tamanio_pag;
    char** archivos_swap;
    int marcos_maximos;
    int retardo_swap;
    int log_habilitado;
} t_config_swap;

////////////////////////////////////////////////

t_config_swap config_swap;
t_log *logger_swap;

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
t_log *logger_swap;

////////////////////////////////////////////////

void iniciar_swap();
void arrancar_logger_swap();

void formatear_swap();
void crear_archivo(int id, int tamanio_proceso);
t_list* formatear_pags_en_archivo(int tamanio_proceso);

void destruir_archivo(int id_tabla);
void liberar_pagina(int nro_pag_swap, t_particion* particion);

#endif /* INCLUDE_SWAP_H_ */
