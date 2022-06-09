#include "../include/main.h"

#define MAX_LEN 256

///*** Consola ***/
int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Faltan parametros\n");
        return EXIT_FAILURE;
    }
    FILE* archivo;
    char *path = strdup(argv[1]);
    char *tamanio_proceso = argv[2];
    cola_instrucciones = queue_create();
    char **aux;
    int i = 0;
    int j = 0;
    archivo = fopen(path, "r");
    if (archivo == NULL) {
      printf("No se pudo leer el archivo\n");
      return 1;
    }
    t_instruccion *una_instruccion = NULL;
    char *buffer = malloc(sizeof(MAX_LEN) + 90);
    while (fgets(buffer, MAX_LEN, archivo))
    {
    	
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
        
        for (int i = 0;i<2;i++)
            free(aux[i]);

        free(aux);
    }

    monitorear_colita(cola_instrucciones);
    fclose(archivo);
    free(buffer);
    free(path);

    leer_configuracion();
    conectar_a_kernel();
    enviar_informacion(tamanio_proceso);
    esperar_mensaje_finalizacion();
    liberar_memoria_y_conexiones();
    
    return EXIT_SUCCESS;
}

void monitorear_colita(t_queue *colita) {
    bool debug;
    //debug = true;
    debug = false;
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
    return -1;
}

void conectar_a_kernel() {
    printf("Iniciando conexión con módulo Kernel ... \n");
    conexion_kernel = crear_conexion(config_consola.ip_kernel,config_consola.puerto_kernel);
    if(conexion_kernel > 0) {
        printf("Conectado a Kernel \n");
    }
    else {
        printf("No se pudo conectar Consola con el módulo Kernel \n");
        exit(EXIT_FAILURE);
    }
}

void enviar_informacion(char *tamanio) {
    t_consola *consola = malloc(sizeof(t_consola));
    consola->instrucciones = cola_instrucciones;
    consola->tamanio = atoi(tamanio);

    enviar_datos_consola(conexion_kernel, consola);
    free(consola);
}

void esperar_mensaje_finalizacion() {
    int estado_finalizacion; // 1: FINALIZO BIEN, 0: FINALIZO MAL
    recv(conexion_kernel, &estado_finalizacion, sizeof(int), MSG_WAITALL); //Se bloquea
    estado_finalizacion == 1 ? liberar_memoria_y_conexiones() : printf("Error al finalizar consola \n");
}

void leer_configuracion() {
    t_config *un_config;

    un_config = config_create("./consola.config");

    config_consola.ip_kernel = strdup(config_get_string_value(un_config,"IP_KERNEL"));
    config_consola.puerto_kernel = strdup(config_get_string_value(un_config,"PUERTO_KERNEL"));
    //habilitar_log(logger_consola, t_config_consola.log_habilitado); Tienen logger? Lo que deseen los chicos

    config_destroy(un_config);
}

void liberar_memoria_y_conexiones() 
{
    printf("Finalizando consola ... \n");
    queue_destroy(cola_instrucciones);
}
