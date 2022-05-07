#include "../include/main.h"

int main(){

    arrancar_logger();
    leer_configuracion();
    habilitar_log(logger_memoria, 1);
    iniciar_semaforos();
    // iniciar();
    return EXIT_SUCCESS;
}

void arrancar_logger() {
	t_log_level nivel_log = LOG_LEVEL_INFO;
	logger_memoria = log_create("memoria.log","memoria",true,nivel_log);

	if(logger_memoria ==  NULL)
	{
		write(0,"ERROR -> No se pudo crear el log \n",30);
		exit(EXIT_FAILURE);
	}

	log_info(logger_memoria,"--------- Log modulo MEMORIA ---------");
}

void leer_configuracion() {   
    t_config *un_config;

    un_config = config_create("./memoria.config");
    
    config_memoria.puerto_escucha = strdup(config_get_string_value(un_config,"PUERTO_ESCUCHA"));
    config_memoria.tamanio_memoria = config_get_int_value(un_config,"TAM_MEMORIA");
    config_memoria.tamanio_pagina = config_get_int_value(un_config,"TAM_PAGINA");
    config_memoria.entradas_por_tabla = config_get_int_value(un_config,"ENTRADAS_POR_TABLA");
    config_memoria.retardo_memoria = config_get_int_value(un_config,"RETARDO_MEMORIA");
    config_memoria.algoritmo_reemplazo = strdup(config_get_string_value(un_config,"ALGORITMO_REEMPLAZO"));
    config_memoria.marcos_por_proceso = config_get_int_value(un_config,"MARCOS_POR_PROCESO");
    config_memoria.retardo_swap = config_get_int_value(un_config,"RETARDO_SWAP");
    config_memoria.path_swap = strdup(config_get_string_value(un_config,"PATH_SWAP"));
    
    config_destroy(un_config);

}

void setear_estructuras_de_memoria() {
    memoria_principal = malloc(sizeof(t_memoria));
    if (config_memoria.tamanio_memoria % config_memoria.tamanio_pagina == 0){
		memoria_principal->memoria = malloc(config_memoria.tamanio_memoria);
		memoria_principal->tamanio_frame = config_memoria.tamanio_pagina;
		memoria_principal->cantidad_frames = config_memoria.tamanio_memoria / config_memoria.tamanio_pagina;
    
        iniciar_particionamiento_en_frames();
        setear_algoritmo_reemplazo();
    } else {
    	log_error(logger_memoria,"No se pudieron setear las estructuras de memoria."
    			"Error en el archivo config: Es necesario que el tamanio_pagina sea multiplo de tamanio_memoria.");
    };
}

void iniciar_particionamiento_en_frames() {
    int i, desplazamiento = 0;
    memoria_principal->frames = list_create();
    t_frame *frame = NULL;
    for(i=0; i<memoria_principal->cantidad_frames; i++) {
        frame = malloc(sizeof(t_frame));
        frame->base = memoria_principal->memoria + desplazamiento;
        frame->is_free = true;
        frame->nro_frame = i;
        list_add(memoria_principal->frames, frame);
        desplazamiento+= memoria_principal->tamanio_frame;
    }
}

void iniciar_semaforos() {
    // TO DO
}

void setear_algoritmo_reemplazo() {
    if(!strcmp("CLOCK-M",config_memoria.algoritmo_reemplazo)) {
        tipo_algoritmo = CLOCKM;
    }
    else if(!strcmp("CLOCK",config_memoria.algoritmo_reemplazo)) {
        tipo_algoritmo = CLOCK;
    }
    else {
        log_error(logger_memoria,"No se pudo setear el algoritmo de reemplazo. Error en el archivo config.");
    } 
}

void finalizar_memoria() {
    // list_destroy_and_destroy_elements(tablas_paginas, eliminar_paginas);
    liberar_configuracion_y_log();
    liberar_memoria();
    exit(EXIT_SUCCESS);
}

void liberar_configuracion_y_log() {
    free(config_memoria.puerto_escucha);
    free(config_memoria.algoritmo_reemplazo);
    free(config_memoria.path_swap);
    log_destroy(logger_memoria);
}

void liberar_memoria() {
    list_destroy_and_destroy_elements(memoria_principal->frames, eliminar_frames);
    free(memoria_principal->memoria);
    free(memoria_principal);
}

void eliminar_frames(void *arg) {
    t_frame *frame = (t_frame *)arg;
    free(frame);
}
