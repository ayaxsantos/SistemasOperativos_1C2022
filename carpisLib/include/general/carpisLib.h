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
#include "utils/clienteutils.h"
#include "utils/serverutils.h"

#define ERROR -1
#define UNDEFINED -100

typedef enum
{   
    /*A definir*/
}func_code;

typedef struct
{
	func_code codigo_funcion;
	t_buffer* buffer;
} t_funcion;

/**
 * @name habilitar_log
 * 
 * @param t_log logger 
 * @param bool habilitacion_log 
 */
void habilitar_log(t_log *log, int habilitacion_log);

#include "utils/utilslib.h"

#endif