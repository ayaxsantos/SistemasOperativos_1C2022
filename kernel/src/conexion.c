#include "../include/conexion.h"

void conexion(void)
{
    int socket_kernel_serv = iniciar_servidor("127.0.0.1", "25655");
    int socket_proceso = 0;
    log_info(un_logger, "Kernel a la espera de conexiones ... \n");

    // Aca tendriamos que conectarnos con MEMORIA y CPU
    // En caso de no poder realizar la conexion, error!! Kernel Panic (?

    while(true)
    {
        socket_proceso = esperar_cliente(socket_kernel_serv);

        pthread_t *hilo_proceso = malloc(sizeof(pthread_t));

        pthread_create(hilo_proceso, NULL, gestionar_comunicacion_con_proceso, (void*)&socket_proceso);
        pthread_detach(*hilo_proceso);
    }
}

void *gestionar_comunicacion_con_proceso(void* socket_proceso_param)
{
    int *socket_proceso = (int*) socket_proceso_param;
    func_code un_codigo = (func_code) recibir_operacion(*socket_proceso);
    switch(un_codigo)
    {
        case HANDSHAKE:
            log_info(un_logger,"Se conecto un proceso!! 😂");
            responder_handshake(*socket_proceso);
            break;
        case INSTRUCCIONES:
            log_info(un_logger,"Recibi unas instrucciones!! 😁");
            inicializar_proceso(*socket_proceso);
            break;
        default:
            log_error(un_logger,"Mensaje no entendidoooo!!!");
            break;
    }
    return NULL;
}

void inicializar_proceso(int socket_proceso)
{
    t_proceso *un_proceso = malloc(sizeof (t_proceso));
    un_proceso->un_pcb = inicializar_pcb(socket_proceso);
}

t_pcb *inicializar_pcb(int socket_proceso)
{
    t_pcb *un_pcb = malloc(sizeof(t_pcb));
    un_pcb->un_estado = NEW;

    // TODO: logica seralizacion...

    return un_pcb;
}

void responder_handshake(int socket_proceso)
{
    return;
}
