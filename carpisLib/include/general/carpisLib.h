#ifndef MATELIB_H_INCLUDED
#define MATELIB_H_INCLUDED

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
	FIN_PROCESO,
	MSJ
    /*A definir*/
}func_code;

/*** Generales ***/
typedef struct
{
	func_code codigo_funcion;
	t_buffer* buffer;
} t_funcion;

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

#include "../utils/utilslib.h"

#endif