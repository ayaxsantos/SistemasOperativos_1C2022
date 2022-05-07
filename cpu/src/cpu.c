#include "../include/cpu.h"

void iniciar() {
    int server_cpu = iniciar_servidor("127.0.0.1", "25657");
    conectar_a_memoria_y_recibir_config(server_cpu);
        /*
    esperar_a_kernel();
    recibir_procesos();*/
}

void conectar_a_memoria_y_recibir_config(int server_cpu) {
    log_info(logger_cpu,"Iniciando conexion con modulo MEMORIA ... \n");
    socket_memoria = crear_conexion(config_cpu.ip_memoria,config_cpu.puerto_memoria);
    enviar_handshake(&socket_memoria, CPU);
    int resultado = recibir_handshake(&socket_memoria, obtener_configuracion);
    if(!resultado) {
        log_error(logger_cpu,"No se pudo conectar con el modulo MEMORIA");
        exit(EXIT_FAILURE);
    }
    log_info(logger_cpu,"MEMORIA Conectada");
}

void obtener_configuracion(int *socket, modulo modulo) {
    /**
     * Deserializar configuracion de memoria y setearla
     */
}