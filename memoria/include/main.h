#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <general/carpisLib.h>
#include <utils/serverutils.h>
#include <utils/utilslib.h>

////////////////////////////////////////////////

typedef struct config_memoria
{
    char* puerto_escucha;
    char* ip_memoria;
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
    int tabla_1n_asignada;
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
	int id_tabla;
    int tamanio_proceso;
    t_dictionary *tabla;
    int puntero;
    int cantidad_hit;
    int cantidad_miss;
} t_tabla_pagina;

typedef enum
{   
    CLOCK,
    CLOCKM
} algoritmo;

////////////////////////////////////////////////

t_config_memoria config_memoria;
t_log *logger_memoria;
algoritmo tipo_algoritmo;
int cantidad_maxima_frames;
int cantidad_tablas_1n;

int socket_cpu, socket_kernel;

/************ESTRUCTURAS DE MEMORIA*******/

t_memoria *memoria_principal;
t_list *tablas_primer_nivel;
/************SEMAFOROS*******************/
pthread_mutex_t mutex_logger;

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

void validar_modulo(int *socket, modulo modulo);
void esperar_handshake_cpu(int server);
void esperar_handshake_kernel(int server);

#include "memoria.h"
#include "swap.h"
#include "conexion.h"
#include "paginacion.h"

#endif /* MAIN_H_INCLUDED */
