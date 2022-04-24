#include "../include/main.h"

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros");
        return EXIT_FAILURE;
    }

    char *path = *argv[1];
    char *tamanio_proceso = *argv[2];


    int archivo = open(path, O_RDONLY);
    //char* ptro_archivo = (char*)mmap(0, config_swamp.tamanio_pagina, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    while(archivo != -1) {
        /**
         * 1- Leer instruccion hasta "\n" 
         * 2- Parsear ins (ver que sea valido)
         * 3- Agregar a la lista instrucciones
         */
    }

    /**
     * Conectar a kernel
     * Enviar instrucciones
     * Esperar finalizacion
     */

	return 0;
}