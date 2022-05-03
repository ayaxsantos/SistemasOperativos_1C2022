#include "../include/main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>

#define MAX_LEN 256

typedef struct instruccion{
	char* instruc;

};

int es_un_identificador_valido(char* instruccion);
int cantidad_de_parametros_validos(char* instruccion);
int es_una_instruccion_valida(char* instruccion, int cantidad_de_parametros_recibidos);

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros\n");
        return EXIT_FAILURE;
    }
    FILE* archivo = malloc(sizeof(FILE));
    char *path = strdup(argv[1]);
    char *tamanio_proceso = argv[2];
    archivo = fopen(path, "r");
    if (archivo == NULL) {
      printf("No se pudo leer el archivo\n");
      return 1;
    }

    char *buffer = malloc(sizeof(char));
    while (fgets(buffer, MAX_LEN, archivo))
    {
    	char **aux = malloc(sizeof(char*));
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        aux = string_split(buffer, " ");

        printf("Instruccion: %s\n", aux[0]);
        printf("Operador 1: %s\n", aux[1]);
        printf("Operador 2: %s\n",aux[2]);

        //Prueba para contar los parametros ingresados
        int i = 0;

        while(aux[i+1] != NULL) {
        	i++;
        }

        printf("Cantidad de operadores ingresados: %d\n",i);
        printf("La instruccion es valida? %d\n", es_una_instruccion_valida(aux[0], i));

        while(i >= 0) {
        	free(aux[i]);
        	i--;
        }
        printf("\n");
        //printf("%s\n", buffer);
    }
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

int cantidad_de_parametros_validos(char* instruccion) {
	if( strcmp(instruccion, "NO_OP") == 0 ||
		strcmp(instruccion, "I/O") == 0 ||
		strcmp(instruccion, "READ") == 0) {
		return 1;
	} else if(strcmp(instruccion, "WRITE") == 0 ||
			strcmp(instruccion, "COPY") == 0) {
		return 2;
	} else if(strcmp(instruccion, "EXIT") == 0) {
		return 0;
	} else {
		return 0;
	}
}

int es_una_instruccion_valida(char* instruccion, int cantidad_de_parametros_recibidos) {
	if(es_un_identificador_valido(instruccion) &&
			cantidad_de_parametros_validos(instruccion) == cantidad_de_parametros_recibidos) {
		return 1;
	} else {
		return 0;
	}
}
