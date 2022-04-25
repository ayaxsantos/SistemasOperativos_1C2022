#include <main.h>

int main(void)
{
    iniciar_logger();
    iniciar_config();
    conexion();

    return EXIT_SUCCESS;
}

void iniciar_logger()
{
    t_log_level nivel_log = LOG_LEVEL_INFO;
    un_logger = log_create("../kernel.log","kernel",true,nivel_log);

    if(un_logger == NULL)
    {
        write(0,"ERROR -> No se pudo crear el log \n",30);
        exit(EXIT_FAILURE);
    }
}

void iniciar_config()
{
    t_config *una_config = config_create("../kernel.config");

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

    free(una_config);
}