#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <commons/string.h>
#include <general/carpisLib.h>

////////////////////////////////////////////////
typedef struct t_config_consola
{
    char* ip_kernel;
    char* puerto_kernel;
    int log_habilitado;
} t_config_consola;

/*typedef struct instruccion{
	char* instruc;
	int parametro1;
	int parametro2;
}T_CONSOLA;*/

////////////////////////////////////////////////

void monitorear_colita(t_queue *colita);
instruccion devolver_enum_instruccion(char* instruccion);
void leer_configuracion();
void conectar_a_kernel();
void enviar_informacion(char *tamanio);
void esperar_mensaje_finalizacion();
void liberar_memoria_y_conexiones();

////////////////////////////////////////////////

t_queue *cola_instrucciones;
t_config_consola config_consola;
int conexion_kernel;


#endif
