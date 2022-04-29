#include "../include/main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 256

typedef struct instruccion{
	char* instruc;

};

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
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
    }
    fclose(archivo);
    return 0;
}
