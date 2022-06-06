#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <general/carpisLib.h>
#include <utils/serverutils.h>
#include <utils/utilslib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <sys/time.h>

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

typedef struct proceso
{
    int socket_proceso;
    t_pcb *un_pcb;
    int tiempo_a_bloquear;
} t_proceso;

//////////////////////////////////////////

t_log *un_logger;
t_config *una_config;
t_config_kernel una_config_kernel;

t_queue *procesos_en_new;
t_list *procesos_en_ready;
t_queue *procesos_en_bloq;
t_queue *procesos_en_bloq_susp;
t_queue *procesos_en_ready_susp;
t_list *procesos_en_exit;

t_proceso *proceso_en_exec;
t_proceso *proceso_en_bloq;
t_proceso *proceso_en_bloq_susp;

pthread_mutex_t mutex_log;
pthread_mutex_t mutex_procesos_en_new;
pthread_mutex_t mutex_procesos_en_ready;
pthread_mutex_t mutex_procesos_en_bloq;
pthread_mutex_t mutex_procesos_en_bloq_susp;
pthread_mutex_t mutex_socket_dispatch;

sem_t grado_multiprog_lo_permite;
sem_t llego_un_proceso;
sem_t hay_procesos_en_ready;
sem_t hay_procesos_en_blocked;
sem_t hay_procesos_en_blocked_susp;

pthread_t *hilo_corto_plazo;
pthread_t *hilo_largo_plazo;
pthread_t *hilo_mediano_plazo;
pthread_t *hilo_gestor_io;


///////////////////////////////////////////

void iniciar_logger();
void iniciar_config();
void iniciar_semaforos();
void iniciar_mutex();
void iniciar_hilos();
void iniciar_estructuras();

void inicializar_plani_largo_plazo();

void liberar_memoria();
void liberar_semaforos();
void liberar_mutex();

///////////////////////////////////////////

#include <conexion.h>
#include <plani_largo_plazo.h>
#include <plani_corto_plazo.h>
#include <plani_mediano_plazo.h>
#include <gestor_io.h>

///////////////////////////////////////////

int socket_dispatch;
int socket_interrupt;

///////////////////////////////////////////

#endif