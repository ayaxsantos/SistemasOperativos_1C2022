#include "../include/main.h"

int main() {   

    arrancar_logger();
    leer_configuracion();
    iniciar_semaforos();
    iniciar();
    return EXIT_SUCCESS;

    /*
    TODO:Los semaforos tambien deben destruirse
    */
}

void arrancar_logger() {
    t_log_level nivel_log = LOG_LEVEL_INFO;
	logger_cpu = log_create("cpu.log","cpu",true,nivel_log);

	if(logger_cpu ==  NULL) {
		write(0,"ERROR -> No se pudo crear el log \n",30);
		exit(EXIT_FAILURE);
	}

	log_info(logger_cpu,"--------- Log modulo CPU ---------");
}

void leer_configuracion() {   
    t_config *un_config;

    un_config = config_create("./cpu.config");
    
    config_cpu.ip_memoria = strdup(config_get_string_value(un_config,"IP_MEMORIA"));
    config_cpu.puerto_memoria = strdup(config_get_string_value(un_config,"PUERTO_MEMORIA"));
    config_cpu.cantidad_entradas_tlb = config_get_int_value(un_config,"ENTRADAS_TLB");
    config_cpu.algoritmo_reemplazo_tlb = strdup(config_get_string_value(un_config,"REEMPLAZO_TLB"));
    config_cpu.retardo_noop = config_get_int_value(un_config,"RETARDO_NOOP");
    config_cpu.puerto_escucha_dispatch = strdup(config_get_string_value(un_config,"PUERTO_ESCUCHA_DISPATCH"));
    config_cpu.puerto_escucha_interrupt = strdup(config_get_string_value(un_config,"PUERTO_ESCUCHA_INTERRUPT"));
    config_cpu.log_habilitado = config_get_int_value(un_config,"LOG_HABILITADO");
    config_cpu.ip_cpu = strdup(config_get_string_value(un_config,"IP_CPU"));
    habilitar_log(logger_cpu, config_cpu.log_habilitado);
    
    config_destroy(un_config);

}

void conectar_a_memoria_y_recibir_config() {
    log_info(logger_cpu,"Iniciando conexion con modulo MEMORIA ... ");
    socket_memoria = crear_conexion(config_cpu.ip_memoria,config_cpu.puerto_memoria);
    enviar_handshake(&socket_memoria, CPU);
    int resultado = esperar_handshake(&socket_memoria, obtener_configuracion);
    if(resultado == -1) {
        log_error(logger_cpu,"No se pudo conectar con el modulo MEMORIA");
        exit(EXIT_FAILURE);
    }
    log_info(logger_cpu,"MEMORIA Conectada");
}

void obtener_configuracion(int *socket, modulo modulo) {
    if(modulo == MEMORIA) {
        int size;
        int desplazamiento = 0;
        void * buffer;
        buffer = recibir_buffer(&size, *socket);

        memcpy(&(config_cpu.entradas_por_tabla), buffer, sizeof(int));
        desplazamiento+=sizeof(int);
        memcpy(&(config_cpu.tamanio_pagina), buffer+desplazamiento, sizeof(int));
        free(buffer);
    }
}

void iniciar_semaforos() {
	sem_init(&sem_execute, 0, 1);  // Preguntar si el 2do parametro está bien(en ambos)
	sem_init(&sem_interrupt, 0, 0);
}

void setear_algoritmo_reemplazo_tlb() {
    if(!strcmp("FIFO",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = FIFO;
    }
    else if(!strcmp("LRU",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = LRU;
    }
    else {
        log_error(logger_cpu,"No se pudieron setear las estructuras de CPU. Error en el archivo config.");
    } 
}

void liberar_semaforos()
{
    sem_destroy(&sem_execute);
    sem_destroy(&sem_interrupt);
}

void liberar_configuracion_y_log() {
    free(config_cpu.ip_memoria);
    free(config_cpu.puerto_memoria);
    free(config_cpu.algoritmo_reemplazo_tlb);
    free(config_cpu.puerto_escucha_dispatch);
    free(config_cpu.puerto_escucha_interrupt);
    log_destroy(logger_cpu);
}
