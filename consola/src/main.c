#include "../include/main.h"
#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros");
        return EXIT_FAILURE;
    }
    FILE* archivo = malloc(sizeof(FILE));
    archivo = fopen("file.txt", "r");
    char *path = *argv[1];
    char *tamanio_proceso = *argv[2];
    t_list *lista_instrucciones = list_create();
    archivo = fopen(path, "r");
    if (archivo == NULL) {
      printf("No se pudo leer el archivo");
      return 1;
    }

    char buffer[MAX_LEN] = malloc(sizeof(char));
    while (fgets(buffer, MAX_LEN, archivo))
    {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
        list_add(lista_instrucciones, buffer);
    }

    fclose(archivo);
    return 0;














    /*
    //char* ptro_archivo = (char*)mmap(0, config_swamp.tamanio_pagina, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    while(archivo != -1) {
        /**
         * 1- Leer instruccion hasta "\n" 
         * 2- Parsear ins (ver que sea valido)
         * 3- Agregar a la lista instrucciones
         */
    //}

    /**
     * Conectar a kernel
     * Enviar instrucciones
     * Esperar finalizacion
     */

	//return 0;
}
