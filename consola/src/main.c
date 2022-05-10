#include "../include/main.h"

#define MAX_LEN 256

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
        una_instruccion->instruc = aux[0];
        if(strcmp(una_instruccion->instruc, "NO_OP") == 0){
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
            if(strcmp(una_instruccion->instruc, "I/O") == 0 || strcmp(una_instruccion->instruc, "READ") == 0){
		        una_instruccion->parametro1 = atoi(aux[1]);
                una_instruccion->parametro2 = 0;
	        }
            if(strcmp(una_instruccion->instruc, "WRITE") == 0 || strcmp(una_instruccion->instruc, "COPY") == 0){
		        una_instruccion->parametro1 = atoi(aux[1]);
                una_instruccion->parametro2 = atoi(aux[2]);
	        }
            if(strcmp(una_instruccion->instruc, "EXIT") == 0) {
               una_instruccion->parametro1 = 0;
               una_instruccion->parametro2 = 0;
            }
            queue_push(cola_instrucciones, una_instruccion);
        }
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
            printf("INSTRUCCION: %s VALOR1: %d VALOR2: %d\n", instruccion->instruc, instruccion->parametro1, instruccion->parametro2);
        }
        printf("*********************************************\n");
    }
}