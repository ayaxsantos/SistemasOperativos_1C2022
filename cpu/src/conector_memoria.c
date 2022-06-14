#include "../include/conector_memoria.h"
/*
int solicitar_registro_1nivel(int id_tabla_1n, int entrada_tabla_1n) {
    t_request_tabla *request = malloc(sizeof(t_request_tabla));
    request->id_tabla_1n = id_tabla_1n;
    request->tabla2n = UNDEFINED;
    request->entrada_tabla = entrada_tabla_1n;
    int32_t *id_tabla_2n = (int32_t *) gestionar_solicitud(request, RESPONSE_TABLA);
    return *id_tabla_2n;
}

t_columna_pagina *solicitar_registro_2nivel(int tabla2n, int entrada_tabla_2n) {

    t_request_tabla *request = malloc(sizeof(t_request_tabla));
    request->id_tabla_1n = pcb->id_tabla_1n;
    request->tabla2n = tabla2n;
    request->entrada_tabla = entrada_tabla_2n;
    t_columna_pagina *registro = (t_columna_pagina *)gestionar_solicitud(request, RESPONSE_REGISTRO);
    return registro;
}

void *gestionar_solicitud(t_request_tabla *request, codigo_operacion codigo_response) {
    t_operacion *operacion = crear_operacion(REQUEST_TABLA);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_memoria);
    eliminar_operacion(operacion);

    //Espera de retorno
    int codigo_operacion = recibir_operacion(socket_memoria);
    if(codigo_operacion == codigo_response && codigo_response == RESPONSE_TABLA) {
        int32_t *id_tabla_2n = recibir_registro_1nivel(socket_memoria);
        return id_tabla_2n;
    }
    else if (codigo_operacion == codigo_response && codigo_response == RESPONSE_REGISTRO) {
        t_columna_pagina *registro = recibir_registro_2nivel(socket_memoria);
        return registro;
    }
    log_error(logger_cpu,"Error al realizar %d", codigo_response);
    return NULL;
}

int32_t *recibir_registro_1nivel(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    return (int32_t *)buffer;
}

t_columna_pagina *recibir_registro_2nivel(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    return (t_columna_pagina *)buffer;
}
*/