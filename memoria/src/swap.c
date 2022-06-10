#include "../include/swap.h"

t_log *logger_swap = NULL;

void iniciar_swap() {
	arrancar_logger_swap();
	habilitar_log(logger_swap, config_swap.log_habilitado);

    log_info(logger_swap,"Iniciando conexión con módulo Memoria ... \n");
    /*
    formatear_swap(conexion_swap);

	carpincho_fd = esperar_cliente(conexion_swap);

	codigo_funcion = recibir_operacion(lib_ref->conexion);
	administrar_operacion(conexion_swap);
    */
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

void formatear_swap(){
    swap.raiz = malloc(sizeof(t_swap)); //TODO: Liberar memoria
    swap.particiones = list_create();
}

void crear_archivo(int nro_proceso, int tamanio_proceso){
	int truncado = 0, cerrado = 0;
	char *nombre_archivo = "proceso" + nro_proceso + ".swap";

	t_fcb *fcb_aux = malloc(sizeof(t_fcb));
	fcb_aux->id_archivo = nro_proceso;
	fcb_aux->path_archivo = nombre_archivo;

	t_particion *particion = malloc(sizeof(t_particion));
	particion->fcb = fcb_aux;
	particion->archivo = creat(nombre_archivo, S_IRWXG);

	if (particion->archivo != -1) {
		truncado = ftruncate(particion->archivo, tamanio_proceso);
		if (truncado != 0){ log_info(logger_swap,"No se pudo establecer el tamanio del archivo."); }

		cerrado = close (particion->archivo);
		if (cerrado != 0){ log_info(logger_swap,"No se pudo cerrar el archivo."); }

		list_add(swap.particiones, particion);
	}
	else {
		log_info(logger_swap,"No se pudo crear el archivo.");
	}
}
