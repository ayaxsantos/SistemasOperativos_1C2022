#include <main.h>

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    if(argc < 1) {
        printf("Faltan parameters\n");
        return EXIT_FAILURE;
    }
    char *path = strdup(argv[1]);
    iniciar_logger();
    iniciar_config(path);
    iniciar_semaforos();
    iniciar_mutex();
    iniciar_estructuras();
    iniciar_hilos();

    contador_pid = 1;

    socket_dispatch = 0;
    socket_interrupt = 0;
    socket_memoria = 0;

    inicializar_plani_largo_plazo();
    inicializar_plani_corto_plazo();
    inicializar_gestor_io();
    //inicializar_plani_mediano_plazo();

    conexion();

    liberar_memoria();
    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////

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

void iniciar_config(char *path_config)
{
    una_config = config_create(path_config);

    una_config_kernel.ip_kernel = config_get_string_value(una_config,"IP_KERNEL");
    una_config_kernel.ip_memoria = config_get_string_value(una_config,"IP_MEMORIA");
    una_config_kernel.puerto_memoria = config_get_string_value(una_config,"PUERTO_MEMORIA");
    una_config_kernel.ip_cpu = config_get_string_value(una_config,"IP_CPU");
    una_config_kernel.puerto_cpu_dispatch = config_get_string_value(una_config,"PUERTO_CPU_DISPATCH");
    una_config_kernel.puerto_cpu_interrupt = config_get_string_value(una_config,"PUERTO_CPU_INTERRUPT");
    una_config_kernel.puerto_escucha = config_get_string_value(una_config,"PUERTO_ESCUCHA");
    una_config_kernel.algoritmo_planificacion = config_get_string_value(una_config,"ALGORITMO_PLANIFICACION");
    una_config_kernel.estimacion_inicial = config_get_int_value(una_config,"ESTIMACION_INICIAL");
    una_config_kernel.alfa_plani = config_get_double_value(una_config,"ALFA");
    una_config_kernel.grado_multiprogramacion = config_get_int_value(una_config,"GRADO_MULTIPROGRAMACION");
    una_config_kernel.tiempo_max_bloqueado = config_get_int_value(una_config,"TIEMPO_MAXIMO_BLOQUEADO") / 1000;
}

void iniciar_estructuras()
{
    procesos_en_new = queue_create();
    procesos_en_ready = list_create();
    procesos_en_susp_ready = queue_create();
    procesos_en_bloq = list_create();
    procesos_en_exit = list_create();
}

void iniciar_semaforos()
{
    sem_init(&grado_multiprog_lo_permite,0,una_config_kernel.grado_multiprogramacion);
    sem_init(&llego_un_proceso,0,0);
    sem_init(&hay_procesos_en_ready,0,0);
    sem_init(&hay_procesos_en_blocked,0,0);
    sem_init(&hay_que_ordenar_cola_ready,0,0);
    sem_init(&hay_proceso_ejecutando,0,0);

    sem_init(&se_produjo_desalojo,0,0);
}

void iniciar_mutex()
{
    pthread_mutex_init(&mutex_log,NULL);
    pthread_mutex_init(&mutex_procesos_en_new,NULL);
    pthread_mutex_init(&mutex_procesos_en_ready,NULL);
    pthread_mutex_init(&mutex_procesos_en_susp_ready,NULL);
    pthread_mutex_init(&mutex_procesos_en_bloq,NULL);
    pthread_mutex_init(&mutex_contador_pid,NULL);

    //Estos pueden estar demas...
    pthread_mutex_init(&mutex_socket_dispatch,NULL);
}

void iniciar_hilos()
{
    hilo_corto_plazo = malloc(sizeof(pthread_t));
    hilo_largo_plazo = malloc(sizeof(pthread_t));
    hilo_mediano_plazo = malloc(sizeof(pthread_t));
    hilo_gestor_io = malloc(sizeof(pthread_t));
    hilo_monitoreo_tiempos = malloc(sizeof(pthread_t));
}

/////////////////////////////////////////////////

void liberar_memoria()
{
    //Ir agregando mas adelante los free/destroy necesarios!

    queue_destroy(procesos_en_new);
    list_destroy(procesos_en_ready);
    queue_destroy(procesos_en_susp_ready);
    list_destroy(procesos_en_bloq);
    list_destroy(procesos_en_exit);

    log_destroy(un_logger);
    config_destroy(una_config);

    liberar_semaforos();
    liberar_mutex();
    liberar_hilos();
}

void liberar_mutex()
{
    pthread_mutex_destroy(&mutex_log);
    pthread_mutex_destroy(&mutex_procesos_en_new);
    pthread_mutex_destroy(&mutex_procesos_en_ready);
    pthread_mutex_destroy(&mutex_procesos_en_susp_ready);
}

void liberar_semaforos()
{
    sem_destroy(&grado_multiprog_lo_permite);
    sem_destroy(&llego_un_proceso);
    sem_destroy(&hay_procesos_en_ready);
    sem_destroy(&hay_procesos_en_blocked);
    sem_destroy(&hay_que_ordenar_cola_ready);
    sem_destroy(&hay_proceso_ejecutando);
}

void liberar_hilos()
{
    free(hilo_corto_plazo);
    free(hilo_largo_plazo);
    free(hilo_mediano_plazo);
    free(hilo_gestor_io);
    free(hilo_monitoreo_tiempos);
}

/////////////////////////////////////////////////
