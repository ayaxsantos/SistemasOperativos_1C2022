#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../carpisLib/include/general/carpisLib.h"
#include <string.h>
#include <commons/string.h>
#include <general/carpisLib.h>

////////////////////////////////////////////////

/*typedef struct instruccion{
	char* instruc;
	int parametro1;
	int parametro2;
}T_CONSOLA;*/

////////////////////////////////////////////////

void monitorear_colita(t_queue *colita);
instruccion devolver_enum_instruccion(char* instruccion);

////////////////////////////////////////////////

t_queue *cola_instrucciones;




#endif
