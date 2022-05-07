#include <general/carpisLib.h>
#include <utils/serverutils.h>
#include <utils/utilslib.h>

typedef struct t_config_cpu
{
    char* ip_memoria;
    char* puerto_memoria;
    int cantidad_entradas_tlb;
    char *algoritmo_reemplazo_tlb;
    int retardo_noop;
    char* puerto_escucha_dispatch;
    char* puerto_escucha_interrupt;
    int log_habilitado;
} t_config_cpu;

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

t_algoritmo tipo_algoritmo_tlb;

t_config_cpu config_cpu;
t_log *logger_cpu;

int socket_memoria;

void leer_configuracion();
void arrancar_logger();
void setear_algoritmo_reemplazo_tlb();

void liberar_configuracion_y_log();

#include "mmu.h"
#include "cpu.h"