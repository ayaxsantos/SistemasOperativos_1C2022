#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <general/carpisLib.h>
#include <utils/serverutils.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

typedef int32_t dir_logica;


/**
 * @DESC Quizas este en carpisLib
 *
 */
typedef enum
{
    CLOCK,
    LRU,
    FIFO
}t_algoritmo;

typedef struct
{
    char* ip_memoria;
    char* puerto_memoria;
    int cantidad_entradas_tlb;
    char *algoritmo_reemplazo_tlb;
    int retardo_noop;
    char* puerto_escucha_dispatch;
    char* puerto_escucha_interrupt;
    int entradas_por_tabla;
    int tamanio_pagina;
    char* ip_cpu;
    int log_habilitado;
} t_config_cpu;

/////////////////////////////////////////////////////////////
t_pcb *pcb;

t_algoritmo tipo_algoritmo_tlb;

t_config_cpu config_cpu;
t_log *logger_cpu;

int socket_memoria, cpu_dispatch, cpu_interrupt, socket_kernel_dispatch, socket_kernel_interrupt;
sem_t sem_interrupt, sem_execute;

t_pcb *pcb;
////////////////////////////////////////////////////////////
void leer_configuracion();
void arrancar_logger();
void conectar_a_memoria_y_recibir_config();
void obtener_configuracion(int *socket, modulo modulo);
void iniciar_semaforos();

void setear_algoritmo_reemplazo_tlb();

void liberar_semaforos();
void liberar_configuracion_y_log();

#include "mmu.h"
#include "cpu.h"
//#include "conector_memoria.h"

#endif
