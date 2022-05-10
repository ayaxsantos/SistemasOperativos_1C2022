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
	INSTRUCCIONES,
    HANDSHAKE,
    PCB,
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

/*** Generales ***/
typedef struct
{
	codigo_operacion cod_op;
	t_buffer* buffer;
} t_operacion;

/*** CPU + KERNEL ***/

typedef struct
{
	unsigned int pid;
	// TODO: Sera necesario este struct o se puede generalizar con la de segundo nivel?
} t_tabla_1nivel;

typedef struct pcb
{
    unsigned int pid;
    unsigned int tamanio;
    t_list *unas_instrucciones;
    unsigned int program_counter;

    //Tabla de paginas -> Proximas iteraciones
	t_tabla_1nivel tabla;
	
    int una_estimacion;
    estado un_estado;
} t_pcb;

/*** Consola ***/
typedef struct
{
	t_list *instrucciones;
	int tamanio;
} t_consola;



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
 * servidor
 * @param int* socket 
 * @param funcion mapeador 
 * @return int 
 */
int recibir_handshake(int *socket, void(*mapeador)(int*, modulo));
void serializar_pcb(t_pcb *pcb, t_operacion *operacion);
#include "../utils/utilslib.h"

#endif