#include "../include/main.h"

int main() {   

    arrancar_logger();
    leer_configuracion();
    //iniciar_semaforos();
    //iniciar();
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

    un_config = config_create("./memoria.config");
    
    config_cpu.ip_memoria = strdup(config_get_string_value(un_config,"IP_MEMORIA"));
    config_cpu.puerto_memoria = strdup(config_get_string_value(un_config,"PUERTO_MEMORIA"));
    config_cpu.cantidad_entradas_tlb = config_get_int_value(un_config,"CANTIDAD_ENTRADAS_TLB");
    config_cpu.algoritmo_reemplazo_tlb = strdup(config_get_string_value(un_config,"ALGORITMO_REEMPLAZO_TLB"));
    config_cpu.retardo_noop = config_get_int_value(un_config,"RETARDO_NOOP");
    config_cpu.puerto_escucha_dispatch = strdup(config_get_string_value(un_config,"PUERTO_ESCUCHA_DISPATCH"));
    config_cpu.puerto_escucha_interrupt = strdup(config_get_string_value(un_config,"PUERTO_ESCUCHA_INTERRUPT"));
    config_cpu.log_habilitado = config_get_int_value(un_config,"LOG_HABILITADO");
    habilitar_log(logger_cpu, config_cpu.log_habilitado);
    
    config_destroy(un_config);

}

void iniciar_semaforos() {

}

void setear_algoritmo_reemplazo_tlb() {
    if(!strcmp("FIFO",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = FIFO;
    }
    else if(!strcmp("LRU",config_cpu.algoritmo_reemplazo_tlb)) {
        tipo_algoritmo_tlb = LRU;
    }
    else {
        log_error(logger_cpu,"No se pudieron setear las estructuras de memoria. Error en el archivo config.");
    } 
}

void liberar_configuracion_y_log() {
    free(config_cpu.ip_memoria);
    free(config_cpu.puerto_memoria);
    free(config_cpu.algoritmo_reemplazo_tlb);
    free(config_cpu.puerto_escucha_dispatch);
    free(config_cpu.puerto_escucha_interrupt);
    log_destroy(logger_cpu);
}