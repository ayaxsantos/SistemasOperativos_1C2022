#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <general/carpisLib.h>
#include <conexion.h>

///////////////////////////////////////////

typedef struct config_kernel
{
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *puerto_escucha;
    char *algoritmo_planificacion;
    int estimacion_inicial;
    int alfa_plani;
    int grado_multiprogramacion;
    int tiempo_max_bloqueado;
} t_config_kernel;

///////////////////////////////////////////

t_log *un_logger;
t_config_kernel una_config_kernel;

///////////////////////////////////////////

void iniciar_logger();
void iniciar_config();

///////////////////////////////////////////

#endif