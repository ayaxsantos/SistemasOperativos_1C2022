#include "../include/conexion.h"

int conexion(void)
{
    int socket_kernel_serv = iniciar_servidor("127.0.0.1","25655");
    log_info(un_logger,"Kernel a la espera de conexiones ... \n");

    // Aca tendriamos que conectarnos con MEMORIA y CPU
    // En caso de no poder realizar la conexion, error!! Kernel Panic (?

    int socket_proceso = esperar_cliente(socket_kernel_serv);
    gestionar_comunicacion_con_proceso(socket_proceso);

    return EXIT_SUCCESS;
}

void gestionar_comunicacion_con_proceso(int socket_proceso)
{
    func_code un_codigo = (func_code) recibir_operacion(socket_proceso);
    switch(un_codigo)
    {
        case HANDSHAKE:
            log_info(un_logger,"Se conecto un proceso!! 😂");
            responder_handshake(socket_proceso);
            break;
        case INSTRUCCIONES:
            log_info(un_logger,"Recibi unas instrucciones!! 😁");
            inicializar_proceso(socket_proceso);
            break;
        default:
            log_error(un_logger,"Mensaje no entendidoooo!!!");
            break;
    }
}

void inicializar_proceso(int socket_proceso)
{
    return;
}

void responder_handshake(int socket_proceso)
{
    return;
}