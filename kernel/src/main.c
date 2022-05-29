#include <main.h>

int main(void)
{
    iniciar_logger();
    iniciar_config();
    iniciar_semaforos();
    iniciar_mutex();
    iniciar_estructuras();
    iniciar_hilos();

    inicializar_plani_largo_plazo();
    inicializar_plani_corto_plazo(una_config_kernel.algoritmo_planificacion);

    conexion();

    liberar_memoria();
    return EXIT_SUCCESS;
}

void iniciar_logger()
{
    t_log_level nivel_log = LOG_LEVEL_INFO;
    un_logger = log_create("kernel.log","kernel",true,nivel_log);

    if(un_logger == NULL)
    {
        write(0,"ERROR -> No se pudo crear el log \n",30);
        exit(EXIT_FAILURE);
    }
    log_info(un_logger,"---------------- Log modulo KERNEL ----------------");
}

void iniciar_config()
{
    t_config *una_config = config_create("./kernel.config");

    una_config_kernel.ip_memoria = config_get_string_value(una_config,"IP_MEMORIA");
    una_config_kernel.puerto_memoria = config_get_string_value(una_config,"PUERTO_MEMORIA");
    una_config_kernel.ip_cpu = config_get_string_value(una_config,"IP_CPU");
    una_config_kernel.puerto_cpu_dispatch = config_get_string_value(una_config,"PUERTO_CPU_DISPATCH");
    una_config_kernel.puerto_cpu_interrupt = config_get_string_value(una_config,"PUERTO_CPU_INTERRUPT");
    una_config_kernel.puerto_escucha = config_get_string_value(una_config,"PUERTO_ESCUCHA");
    una_config_kernel.algoritmo_planificacion = config_get_string_value(una_config,"ALGORITMO_PLANIFICACION");
    una_config_kernel.estimacion_inicial = config_get_int_value(una_config,"ESTIMACION_INICIAL");
    una_config_kernel.alfa_plani = config_get_int_value(una_config,"ALFA");
    una_config_kernel.grado_multiprogramacion = config_get_int_value(una_config,"GRADO_MULTIPROGRAMACION");
    una_config_kernel.tiempo_max_bloqueado = config_get_int_value(una_config,"TIEMPO_MAXIMO_BLOQUEADO");

    config_destroy(una_config);
}

void iniciar_estructuras()
{
    procesos_en_new = queue_create();
    procesos_en_ready = list_create();
    procesos_en_bloq = list_create();
    procesos_en_exit = list_create();
}

void iniciar_semaforos()
{
    sem_init(&grado_multiprog_lo_permite,0,una_config_kernel.grado_multiprogramacion);
    sem_init(&llego_un_proceso,0,0);
}

void iniciar_mutex()
{
    pthread_mutex_init(&mutex_log,NULL);
    pthread_mutex_init(&mutex_procesos_en_new,NULL);
    pthread_mutex_init(&mutex_procesos_en_ready,NULL);
}

void iniciar_hilos()
{
    hilo_corto_plazo = malloc(sizeof(pthread_t));
    hilo_largo_plazo = malloc(sizeof(pthread_t));
}

void liberar_memoria()
{
    //Ir agregando mas adelante los free/destroy necesarios!

    queue_destroy(procesos_en_new);
    list_destroy(procesos_en_ready);
    list_destroy(procesos_en_bloq);
    list_destroy(procesos_en_exit);

    log_destroy(un_logger);

    liberar_semaforos();
    liberar_mutex();
}

void liberar_mutex()
{
    pthread_mutex_destroy(&mutex_log);
    pthread_mutex_destroy(&mutex_procesos_en_new);
    pthread_mutex_destroy(&mutex_procesos_en_ready);
}

void liberar_semaforos()
{
    sem_destroy(&grado_multiprog_lo_permite);
    sem_destroy(&llego_un_proceso);
}

void liberar_hilos()
{
    free(hilo_corto_plazo);
    free(hilo_largo_plazo);
}