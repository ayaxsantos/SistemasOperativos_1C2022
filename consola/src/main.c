#include "../include/main.h"

#define MAX_LEN 256

///*** Consola ***/
/*
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
typedef enum
{
    NO_OP, 0
    IO, 1
    READ, 2
    WRITE, 3
    COPY, 4
    I_EXIT 5
} instruccion;*/

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros\n");
        return EXIT_FAILURE;
    }
    FILE* archivo = malloc(sizeof(FILE));
    char *path = strdup(argv[1]);
    char *tamanio_proceso = argv[2];
    cola_instrucciones = queue_create();
    int i = 0;
    int j = 0;
    archivo = fopen(path, "r");
    if (archivo == NULL) {
      printf("No se pudo leer el archivo\n");
      return 1;
    }
    t_instruccion *una_instruccion = NULL;
    char *buffer = malloc(sizeof(char));
    while (fgets(buffer, MAX_LEN, archivo))
    {
    	char **aux;
        buffer[strcspn(buffer, "\n")] = 0;
        aux = string_split(buffer, " ");

        una_instruccion = malloc(sizeof(t_instruccion));
        una_instruccion->instruc = devolver_enum_instruccion(aux[0]);
        if(una_instruccion->instruc == NO_OP){
            i = 0;
            j = atoi(aux[1]);
            una_instruccion->parametro1 = 0;
            una_instruccion->parametro2 = 0;
        	while(i < j){
                queue_push(cola_instrucciones, una_instruccion);
        	    i++;
        	 }
        }
        else{
            if(una_instruccion->instruc == IO || una_instruccion->instruc == READ){
		        una_instruccion->parametro1 = atoi(aux[1]);
                una_instruccion->parametro2 = 0;
	        }
            if(una_instruccion->instruc == WRITE || una_instruccion->instruc == COPY){
		        una_instruccion->parametro1 = atoi(aux[1]);
                una_instruccion->parametro2 = atoi(aux[2]);
	        }
            if(una_instruccion->instruc == I_EXIT) {
               una_instruccion->parametro1 = 0;
               una_instruccion->parametro2 = 0;
            }
            queue_push(cola_instrucciones, una_instruccion);
        }
    //free(aux); ====> Hay que liberar aux??
    }
    //free(buffer); preguntar como liberar estos dos
    //free(una_instruccion);

    monitorear_colita(cola_instrucciones);
    fclose(archivo);
    return EXIT_SUCCESS;
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
            printf("INSTRUCCION: %d VALOR1: %d VALOR2: %d\n", instruccion->instruc, instruccion->parametro1, instruccion->parametro2);
        }
        printf("*********************************************\n");
    }
}

instruccion devolver_enum_instruccion(char* instruccion) {
    if(strcmp(instruccion, "NO_OP") == 0) return NO_OP;
    if(strcmp(instruccion, "I/O") == 0) return IO;
    if(strcmp(instruccion, "READ") == 0) return READ;
    if(strcmp(instruccion, "WRITE") == 0) return WRITE;
    if(strcmp(instruccion, "COPY") == 0) return COPY;
    if(strcmp(instruccion, "EXIT") == 0) return I_EXIT;
}
