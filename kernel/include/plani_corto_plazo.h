#ifndef KERNEL_PLANI_CORTO_PLAZO_H
#define KERNEL_PLANI_CORTO_PLAZO_H

#include "main.h"
#include <math.h>

//////////////////////////////////////

void inicializar_plani_corto_plazo();
void lanzar_hilo_plani_corto_plazo_con(void* (*)(void*));
void *algoritmo_fifo(void*);
void *algoritmo_sjf_con_desalojo(void*);
void pasar_proceso_a_bloqueado();
void organizacionPlani();
int calcular_estimacion(time_t tiempoF, time_t tiempoI, t_proceso *un_proceso);
bool comparador_de_procesos_SJF(t_proceso *un_proceso_primero, t_proceso *un_proceso_segundo);
void devolver_proceso_a_ready(t_proceso *);
void gestionar_pcb_para_probar_sin_cpu();

//////////////////////////////////////

t_pcb *obtener_pcb();
void gestionar_pcb();

//////////////////////////////////////

bool hay_que_desalojar(t_proceso *);
void solicitar_desalojo_a_cpu();
double calcular_tiempo_ejecutando();
void *rutina_monitoreo_desalojo(void *);

time_t tiempoF, tiempoI;

//TEMPORAL, SOLO PARA FUNCION DE TEST
int flag_interrupt;
pthread_mutex_t mutex_flag_interrupt;

bool esta_el_flag_interrupt_en_alto();

//////////////////////////////////////

#endif //KERNEL_PLANI_CORTO_PLAZO_H
