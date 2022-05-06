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

typedef struct instruccion{
	char* instruc;
	int parametro1;
	int parametro2;
}t_instruccion;

////////////////////////////////////////////////

int es_un_identificador_valido(char* instruccion);
int cantidad_de_parametros_validos(char* instruccion);
int es_una_instruccion_valida(char* instruccion, int cantidad_de_parametros_recibidos);
void monitorear_colita(t_queue *colita);

////////////////////////////////////////////////

t_queue *cola_instrucciones;




#endif