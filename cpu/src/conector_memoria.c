#include "../include/conector_memoria.h"

/*
 * typedef struct t_solicitud
{
    int32_t id_tabla_1n;
    int32_t entrada_tabla;
    int32_t id_tabla_2n;
    unsigned int nro_frame;
    unsigned int desplazamiento;
    accion accion_solicitada;
    uint32_t dato; //Suponemos un entero no signado de 4bytes
    estado_memoria estado_memo;
} __attribute__((packed)) t_solicitud;
 */
int32_t solicitar_registro_1nivel(int id_tabla_1n, int entrada_tabla_1n) {
    t_solicitud *request = malloc(sizeof(t_solicitud));
    request->id_tabla_1n = id_tabla_1n;
    request->entrada_tabla = entrada_tabla_1n;

    request->id_tabla_2n = UNDEFINED;
    request->nro_frame = UNDEFINED;
    request->desplazamiento = UNDEFINED;
    request->dato = UNDEFINED;

    request = (t_solicitud *)gestionar_solicitud(request, PRIMERA_SOLICITUD);
    int32_t  id_tabla_2n = request->id_tabla_2n;
    free(request);
    return id_tabla_2n;
}

unsigned int solicitar_registro_2nivel(int tabla2n, int entrada_tabla_2n) {
    t_solicitud *request = malloc(sizeof(t_solicitud));
    request->id_tabla_1n = pcb->id_tabla_1n;
    request->id_tabla_2n = tabla2n;
    request->entrada_tabla = entrada_tabla_2n;

    request->nro_frame = UNDEFINED;
    request->desplazamiento = UNDEFINED;
    request->dato = UNDEFINED;

    request = (t_solicitud *)gestionar_solicitud(request, SEGUNDA_SOLICITUD);
    unsigned int nro_frame = request->nro_frame;
    free(request);
    return nro_frame;
}

uint32_t solicitar_dato(int desplazamiento, unsigned int nro_frame) {
    t_solicitud *request = malloc(sizeof(t_solicitud));
    request->desplazamiento = desplazamiento;
    request->nro_frame = nro_frame;
    request->accion_solicitada = READ_ACCION;
    request->estado_memo = READ_OK;

    request->dato = UNDEFINED;

    request = (t_solicitud *)gestionar_solicitud(request, TERCERA_SOLICITUD);
    uint32_t dato = request->dato;
    free(request);
    return dato;
}


int enviar_dato_memoria(int desplazamiento, unsigned int nro_frame, uint32_t dato) {
    t_tercera_solicitud *request = malloc(sizeof(t_tercera_solicitud));
    request->desplazamiento = desplazamiento;
    request->nro_frame = nro_frame;
    request->accion_solicitada = WRITE_ACCION;
    request->dato = dato;
    request->estado_memo = WRITE_OK;

    request = (t_tercera_solicitud *)gestionar_solicitud(request, TERCERA_SOLICITUD);
    estado_memoria estado_memo = request->estado_memo;
    free(request);
    return estado_memo;
}

void *gestionar_solicitud(void *request, codigo_operacion codigo) {
    t_operacion *operacion = crear_operacion(codigo);
    setear_operacion(operacion, request);
    enviar_operacion(operacion, socket_memoria);
    eliminar_operacion(operacion);
    free(request);

    //Espera de retorno
    int codigo_retorno = recibir_operacion(socket_memoria);
    switch (codigo_retorno) {
        case PRIMERA_SOLICITUD:
        case SEGUNDA_SOLICITUD:
        case TERCERA_SOLICITUD:
            request = recibir_solicitud(socket_memoria);
            return request;
    }
    log_error(logger_cpu,"Error al realizar %d", codigo);
    return NULL;
}