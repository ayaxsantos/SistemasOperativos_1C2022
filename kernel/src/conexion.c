#include "../include/conexion.h"

void conexion(void)
{
    int socket_kernel_serv = iniciar_servidor("127.0.0.1", "25655");
    log_info(un_logger, "Kernel a la espera de conexiones ... \n");

    // Aca tendriamos que conectarnos con MEMORIA y CPU
    // En caso de no poder realizar la conexion, error!! Kernel Panic (?

    while(true)
    {
        int *socket_proceso = malloc(sizeof(int));
        *socket_proceso = esperar_cliente(socket_kernel_serv);

        pthread_t *hilo_proceso = malloc(sizeof(pthread_t));

        pthread_create(hilo_proceso, NULL, gestionar_comunicacion_con_proceso, (void*)&socket_proceso);
        pthread_detach(*hilo_proceso);
    }
}

void *gestionar_comunicacion_con_proceso(void* socket_proceso_param)
{
    int socket_proceso = *((int*) socket_proceso_param);                 //Casteo necesario por los argumentos, tambien necesario por pthread_create
    realizar_handshake(socket_proceso);

    func_code un_codigo = (func_code) recibir_operacion(socket_proceso);
    switch(un_codigo)
    {
        case INSTRUCCIONES:
            pthread_mutex_lock(&mutex_log);
            log_info(un_logger,"Recibi unas instrucciones!! 😁");
            pthread_mutex_unlock(&mutex_log);
            inicializar_proceso(socket_proceso);
            break;
        default:
            pthread_mutex_lock(&mutex_log);
            log_error(un_logger,"Mensaje no entendidoooo!!!");
            pthread_mutex_unlock(&mutex_log);
            break;
    }
    return NULL;
}

void realizar_handshake(int socket_proceso)
{
    func_code un_codigo = (func_code) recibir_operacion(socket_proceso);
    if(un_codigo == HANDSHAKE)
    {
        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"Se conecto un proceso!! 😂");
        pthread_mutex_unlock(&mutex_log);
        responder_handshake(socket_proceso);
    }
    else
    {
        pthread_mutex_lock(&mutex_log);
        log_info(un_logger,"No se pudo realizar el handshake :c");
        pthread_mutex_unlock(&mutex_log);
        pthread_exit(NULL);     //Preguntar si podemos usar esto... creo que si jeje
    }
}

void inicializar_proceso(int socket_proceso)
{
    t_proceso *un_proceso = malloc(sizeof (t_proceso));
    un_proceso->un_pcb = inicializar_pcb(socket_proceso);
    un_proceso->socket_proceso = socket_proceso;    //Cuando pasa exit, podemos responderle a la consola en particular
}

t_pcb *inicializar_pcb(int socket_proceso)
{
    t_pcb *un_pcb = malloc(sizeof(t_pcb));
    un_pcb->un_estado = NEW;
    un_pcb->pid = obtener_id_hilo();
    un_pcb->una_estimacion = una_config_kernel.estimacion_inicial;
    un_pcb->tamanio = obtener_tamanio(socket_proceso);
    un_pcb->unas_instrucciones = obtener_instrucciones(socket_proceso);

    //Tabla de paginas -> Proximas iteraciones

    return un_pcb;
}

unsigned int obtener_tamanio(int socket_proceso)
{
    // TODO: logica seralizacion...
    return 0;
}

t_list *obtener_instrucciones(int socket_proceso)
{
    // TODO: logica seralizacion...
    return NULL;
}

unsigned int obtener_id_hilo()
{
    //Resulta conveniente obtener el id del thread (el real!!), asegurando unicidad
    //Podria hacernos mas dependientes del SO en particular donde corre el sistema??
    return syscall(SYS_gettid);
}

////////////////////////////////////////////////

void responder_handshake(int socket_proceso)
{
    func_code un_codigo_funcion = HANDSHAKE;
    responder_un_func_code(socket_proceso,un_codigo_funcion);
}

void responder_fin_proceso(int socket_proceso)
{
    func_code un_codigo_funcion = FIN_PROCESO;
    responder_un_func_code(socket_proceso,un_codigo_funcion);
}

void responder_un_func_code(int socket_proceso, func_code un_codigo_funcion)
{
    t_funcion *una_funcion = crear_funcion(un_codigo_funcion);
    enviar_funcion(una_funcion,socket_proceso);

    eliminar_funcion(una_funcion);
}
