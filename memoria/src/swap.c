#include "../include/swap.h"

t_log *logger_swap = NULL;

void iniciar_swap() {
	arrancar_logger_swap();
	leer_configuracion_swap();
	habilitar_log(logger_swap, config_swap.log_habilitado);

    log_info(logger_swap,"Iniciando conexión con módulo Memoria ... \n");
    /*int conexion_swap = crear_conexion(config_swap.ip_swap,config_swap.puerto_swap);

    formatear_swap(conexion_swap);

    if(conexion_swap > 0)
    {
        log_info(logger_swap,"SWAP a la espera de conexiones...");
    	int carpincho_fd = esperar_cliente(conexion_swap);
        int codigo_funcion = recibir_operacion(lib_ref->conexion);
        while(true) {
        	// administrar_operacion(conexion_swap);
        }
    	exit(EXIT_SUCCESS);
    }
    else
    {
    	log_error(logger_swap,"No se pudo conectar SWAP con el módulo Memoria.");
    	exit(EXIT_FAILURE);
    }*/

}

void arrancar_logger_swap(){
	t_log_level nivel_log = LOG_LEVEL_INFO;
	logger_swap = log_create("swap.log","swap",true,nivel_log);

	if(logger_swap ==  NULL){
		write(0,"ERROR -> No se pudo crear el log \n",30);
		exit(EXIT_FAILURE);
	}

	log_info(logger_swap,"--------- Log modulo SWAP ---------");
}

void leer_configuracion_swap(){
    t_config *un_config;

    un_config = config_create("./swap.config");

    config_swap.ip_swap = strdup(config_get_string_value(un_config,"IP_SWAP"));
    config_swap.puerto_swap = strdup(config_get_string_value(un_config,"PUERTO_SWAP"));
    config_swap.tamanio_swap = config_get_int_value(un_config,"TAMANIO_SWAP");
    config_swap.tamanio_pag = config_get_int_value(un_config,"TAMANIO_PAGINA");
    config_swap.archivos_swap = config_get_array_value(un_config,"ARCHIVOS_SWAP");
    config_swap.marcos_maximos = config_get_int_value(un_config,"MARCOS_MAXIMOS");
    config_swap.retardo_swap = config_get_int_value(un_config,"RETARDO_SWAP");
    config_swap.log_habilitado = config_get_int_value(un_config,"LOG_HABILITADO");
    config_destroy(un_config);

}

void formatear_swap(){
    t_config_swap* config = &config_swap;
    int i, truncado = 0, cerrado = 0;

    // Defino estructuras administrativas
    int cant_archivos = string_array_size(config->archivos_swap);
    swap.header = malloc(sizeof(t_header)); //TODO:Liberar memoria
    swap.particiones = list_create();
    swap.header->tamanio_swap = config->tamanio_swap * cant_archivos;

    if ((swap.header->tamanio_swap % config->tamanio_pag) == 0){
    swap.header->cant_pags_por_particion = swap.header->tamanio_swap / config->tamanio_pag;

    // Defino particiones
        for (i = 0; i < cant_archivos; i++){
            t_fcb *fcb_aux = malloc(sizeof(t_fcb));
            fcb_aux->id_archivo = i;
            fcb_aux->nombre_archivo = config->archivos_swap[i];
            fcb_aux->pags_en_archivo = inicializar_particion();
            fcb_aux->procesos_en_archivo = list_create();

            t_particion *particion = malloc(sizeof(t_particion));
            particion->fcb = fcb_aux;
            particion->archivo = creat(config->archivos_swap[i], S_IRWXG);

            if (particion->archivo != -1) {
                truncado = ftruncate(particion->archivo, config->tamanio_swap);
                if (truncado != 0){ log_info(logger_swap,"No se pudo establecer el tamanio del archivo."); }

                cerrado = close (particion->archivo);
                if (cerrado != 0){ log_info(logger_swap,"No se pudo cerrar el archivo."); }

                list_add(swap.particiones, particion);
            }
            else {
                log_info(logger_swap,"No se pudo crear el archivo.");
            }
        }
    } else {
    	log_info(logger_swap,"No se pudo configurar las estructuras de SWAP por errores en el archivo config.");
    };
}

t_list* inicializar_particion(){
    t_list* carpis_aux = list_create();
    int i = 0;

    for (i = 0; i < swap.header->cant_pags_por_particion; i++){
        t_pagina_swap* pag_aux = malloc(sizeof(t_pagina_swap));
        pag_aux->id_memoria = -1;
        pag_aux->pid = -1;
        pag_aux->is_free = true;
        list_add(carpis_aux, pag_aux);
    }

    return carpis_aux;
};
