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

typedef struct header{
    int tamanio_swap;
    void* raiz;
    int cant_pags_por_particion;
} t_header;

typedef struct pagina_swap{
    int id_memoria;         // id de la página en memoria
    int pid;
    bool is_free;
} t_pagina_swap;

typedef struct fcb{
    int id_archivo;
    char* nombre_archivo;
    t_list* pags_en_archivo;
    t_list* procesos_en_archivo;
} t_fcb;

typedef struct particion{
    t_fcb* fcb;
    int archivo;
} t_particion;

////////////////////////////////////////////////

typedef struct swap{
    t_header* header;
    t_list* particiones;
} t_swap;

////////////////////////////////////////////////

t_swap swap;

int max_marcos_por_proceso_en_swap;

////////////////////////////////////////////////

void iniciar_swap();
void arrancar_logger_swap();
void leer_configuracion_swap();

void formatear_swap();
t_list* inicializar_particion();

#endif /* INCLUDE_SWAP_H_ */
