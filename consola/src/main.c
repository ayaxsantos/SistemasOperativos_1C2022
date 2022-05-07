#include "../include/main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>
#include <general/carpisLib.h>

#define MAX_LEN 256

typedef struct instruccion{
	char* instruc;
	int parametro1;
	int parametro2;
}t_instruccion;

int es_un_identificador_valido(char* instruccion);
int cantidad_de_parametros_validos(char* instruccion);
int es_una_instruccion_valida(char* instruccion, int cantidad_de_parametros_recibidos);
void monitorear_colita(t_queue *colita);

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros\n");
        return EXIT_FAILURE;
    }
    FILE* archivo = malloc(sizeof(FILE));
    char *path = strdup(argv[1]);
    char *tamanio_proceso = argv[2];
    t_queue *cola_instrucciones = queue_create();
    int i = 0;
    archivo = fopen(path, "r");
    if (archivo == NULL) {
      printf("No se pudo leer el archivo\n");
      return 1;
    }
    t_instruccion *una_instruccion = NULL;
    char *buffer = malloc(sizeof(char));
    while (fgets(buffer, MAX_LEN, archivo))
    {
    	char **aux; //= malloc(sizeof(char*));
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        aux = string_split(buffer, " ");

        una_instruccion = malloc(sizeof(t_instruccion));
        una_instruccion->instruc = aux[0];
        if(strcmp(una_instruccion->instruc, "EXIT") != 0) {
            una_instruccion->parametro1 = atoi(aux[1]);
            //una_instruccion->parametro2 = atoi(aux[2]);
        }
        printf("Instruccion: %s\n", una_instruccion->instruc);
        printf("Operador 1: %d\n", una_instruccion->parametro1);
        printf("Operador 2: %d\n",una_instruccion->parametro2);
        if(strcmp(una_instruccion->instruc, "NO_OP") == 0){
        	//Aqui multiplicar por el numero de que diga una_instruccion->parametro1
        	i = 0;
            int j = una_instruccion->parametro1;
        	while(i < j){
        	    queue_push(cola_instrucciones, una_instruccion);
        	    i++;
        	 }
        }
        else {
            queue_push(cola_instrucciones, una_instruccion);
        }
        //Prueba para contar los parametros ingresados
    }
    monitorear_colita(cola_instrucciones);
    fclose(archivo);
    return 0;
}

int es_un_identificador_valido(char* instruccion) {
	if( strcmp(instruccion, "NO_OP") == 0 ||
		strcmp(instruccion, "I/O") == 0 ||
		strcmp(instruccion, "READ") == 0 ||
		strcmp(instruccion, "WRITE") == 0 ||
		strcmp(instruccion, "COPY") == 0 ||
		strcmp(instruccion, "EXIT") == 0) {
		return 1;
	} else {
		return 0;
	}
}

void monitorear_colita(t_queue *colita) {
    bool debug;
    debug = true;
    //debug = false;
    if(debug) {
        printf("\n*********************************************\n");
        t_instruccion *instruccion;
        int tam = queue_size(colita);
        for(int i=0; i<tam;i++) {
            instruccion = (t_instruccion *)queue_pop(colita);
            printf("INSTRUCCION: %s VALOR1: %d VALOR2: %d\n", instruccion->instruc, instruccion->parametro1, instruccion->parametro2);
        }
        printf("*********************************************\n");
    }
}