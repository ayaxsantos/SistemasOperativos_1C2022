#ifndef CARPISLIB_H_INCLUDED
#define CARPISLIB_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include "../utils/clienteutils.h"
#include "../utils/serverutils.h"

#define ERROR -1
#define UNDEFINED -100

/*** ENUMS ***/
typedef enum
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    SUSP_BLOCKED,
    SUSP_READY,
    EXIT,
} estado;

typedef enum
{   
	T_CONSOLA,
    HANDSHAKE,
    PCB,
    BLOQUEO,
    INTERRUPCION,
	FIN_PROCESO,
	MSJ
    /*A definir*/
}codigo_operacion;

typedef enum
{   
	KERNEL,
    CPU,
    MEMORIA
} modulo;

typedef enum
{
    NO_OP,
    IO,
    READ,
    WRITE,
    COPY,
    I_EXIT
} instruccion;

/*** Generales ***/
typedef struct
{
	codigo_operacion cod_op;
	t_buffer* buffer;
} t_operacion;

/*** Consola ***/

typedef struct t_instruccion{
    instruccion instruc;
    int parametro1;
    int parametro2;
} t_instruccion;

typedef struct
{
    t_queue *instrucciones;
    int tamanio;
} t_consola;

/*** CPU + KERNEL ***/

typedef struct pcb
{
    unsigned int pid;
    unsigned int program_counter;
    double una_estimacion;
    estado un_estado;
    t_consola *consola;

    //Tabla de paginas -> Proximas iteraciones
    t_dictionary *tabla_1n;
} t_pcb;


typedef struct proceso_pcb
{
    int tiempo_bloqueo;
    t_pcb *pcb;
} t_proceso_pcb;

/**
 * @name habilitar_log
 * 
 * @param t_log logger 
 * @param bool habilitacion_log 
 */
void habilitar_log(t_log *log, int habilitacion_log);

void enviar_handshake(int *socket, modulo modulo_solicitante);

/**
 * @DESC: recibe el handshake, se debe realizar una funcion que obtenga los datos solicitados al
 * servidor, GUARDA, es bloqueante
 * @param int* socket 
 * @param funcion mapeador 
 * @return int 
 */
int esperar_handshake(int *socket, void(*mapeador)(int*, modulo));

void enviar_datos_consola(int socket, t_consola *consola);
t_consola *recibir_datos_consola(int socket);

void enviar_interrupcion(int socket);
bool recibir_interrupcion(int socket);

void *serializar_consola(t_consola *consola, int *size);
t_pcb *deserializar_pcb_enviar(int socket, void * buffer);
void serializar_pcb(t_pcb *pcb, t_operacion *operacion);
void *serializar_instrucciones(t_queue *instrucciones, int *size_cola);
void *serializar_tabla1n(t_dictionary *tabla1n, int *size);
void enviar_proceso_pcb(int socket, t_proceso_pcb *un_proceso_pcb, codigo_operacion un_codigo_operacion);
void serializar_proceso_pcb(t_proceso_pcb *proceso_bloqueo, t_operacion *operacion);

t_pcb *deserializar_pcb(int socket);
t_consola *deserializar_consola(void *buffer);
t_queue *deserializar_instrucciones(void *buffer, int size_cola);
t_dictionary *deserializar_tabla1n(void *buffer, int size_tabla);
t_proceso_pcb *deserializar_proceso_pcb(int socket);

#include "../utils/utilslib.h"

#endif