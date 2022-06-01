#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>

////////////////////////////////////////////////

typedef struct config_memoria
{
    char* puerto_escucha;
    int tamanio_memoria;
    int tamanio_pagina;
    int entradas_por_tabla;
    int retardo_memoria;
    char *algoritmo_reemplazo;
    int marcos_por_proceso;
    int retardo_swap;
    char *path_swap;
} t_config_memoria;

typedef struct t_frame
{
    char *base;
    int nro_frame;
    bool usado;
    int modificado;
    bool is_free;
    int nro_pagina_asignada;
    int pid_asignado;
    long tiempo_usado;
} t_frame;

typedef struct t_memoria{
    void *memoria;
    int tamanio_frame;
    int cantidad_frames;
    t_list *frames;
} t_memoria;

typedef struct t_col_pagina
{
    bool presencia;
    int nro_frame;
} t_col_pagina;

typedef struct t_tabla_pagina
{
    unsigned int pid;
    t_dictionary *tabla;
    int nro_tabla_2n;
    int puntero;
    int cantidad_hit;
    int cantidad_miss;
    t_list* tablas_segundo_nivel;
} t_tabla_pagina;

typedef enum
{   
    READ,
    WRITE
}t_accion;

typedef enum
{   
    CLOCK,
    CLOCKM
}t_algoritmo;

////////////////////////////////////////////////

t_config_memoria config_memoria;
t_log *logger_memoria;
t_algoritmo tipo_algoritmo;

/************ESTRUCTURAS DE MEMORIA*******/

t_memoria *memoria_principal;
t_list *tablas_primer_nivel;

////////////////////////////////////////////////

void leer_configuracion();
void arrancar_logger();
void setear_estructuras_de_memoria();
void iniciar_particionamiento_en_frames();
void setear_algoritmo_reemplazo();
void iniciar_semaforos();

void finalizar_memoria();
void eliminar_frames(void *arg);
void liberar_memoria();
void liberar_configuracion_y_log();

#endif /* MAIN_H_INCLUDED */
