#include "../include/memoria.h"

void iniciar() {
    tablas_primer_nivel = list_create();
    setear_estructuras_de_memoria();

    // TODO Completar
}

void iniciar_proceso(t_pcb *proceso, int socket_cliente) {
	proceso->pid = get_thread_id();

	t_tabla_pagina* tabla_principal_del_proceso = crear_tabla_principal_para(proceso->pid);

    list_add(tablas_primer_nivel, tabla_principal_del_proceso);

    // TODO Completar
}

void terminar(int socket_cliente) {
	// TODO Completar
}

void gestionar_suspension(int socket_cliente) {
	// TODO Completar
}

void gestionar_acceso(int socket_cliente) {
	// TODO Completar
}

void gestionar_lectura(int socket_cliente){
	// TODO Completar
}

void gestionar_escritura(int socket_cliente){
	// TODO Completar
}


bool buscar_por_pid(void *una_tabla_de_proceso, unsigned int pid) {
    t_tabla_pagina *tabla = (t_tabla_pagina *) una_tabla_de_proceso;
    return tabla->pid == pid;
}

// TODO: Debugear
t_tabla_pagina* encontrar_tabla_de_pid(unsigned int pid){
    //pthread_mutex_lock(&mutex_lista_tablas_paginas);
    bool _buscar_por_id(void *un_proceso) {
        return buscar_por_pid(un_proceso, pid);
    }
    t_tabla_pagina *tabla_pagina = (t_tabla_pagina *)list_find(tablas_primer_nivel, _buscar_por_id);
    //pthread_mutex_unlock(&mutex_lista_tablas_paginas);
    if(tabla_pagina == NULL) { return false; }		// ¿Se encontró una tabla asociada al PID?
    return tabla_pagina;
}


bool hay_espacio_en_memoria_para(){
	// TODO Completar
	return false;
}

void leer_dato() {
	// TODO Completar
}

void escribir_dato() {
	// TODO Completar
}
