#include "../../include/general/carpisLib.h"

void habilitar_log(t_log *log, int habilitacion_log) {
	log->is_active_console = habilitacion_log;
}

void enviar_handshake(int *socket, modulo modulo_solicitante) {
    void *buffer = malloc(sizeof(int)*2);
    codigo_operacion handshake = HANDSHAKE;
    memcpy(buffer, &handshake, sizeof(int));
    memcpy(buffer + sizeof(int), &modulo_solicitante, sizeof(int));
    send(*socket, buffer, sizeof(int)*2, 0);
}

int esperar_handshake(int *socket, void(*mapeador)(int*, modulo)) {
    codigo_operacion cod_op = recibir_operacion(*socket);
    if(cod_op == HANDSHAKE) {
        modulo modulo_solicitante = recibir_operacion(*socket);
        mapeador(socket, modulo_solicitante);
        return 0;
    }
    return ERROR;
}

char* obtener_nombre_modulo(modulo un_modulo)
{
    switch (un_modulo)
    {
        case CPU:
            return "CPU";
        case MEMORIA:
            return "MEMORIA";
        default:
            return "ERROR";
    }
}

void enviar_datos_consola(int socket, t_consola *consola) {
    t_operacion *operacion = crear_operacion(T_CONSOLA);
    setear_operacion(operacion, consola);
    enviar_operacion(operacion, socket);
    eliminar_operacion(operacion);
}

t_consola *recibir_datos_consola(int socket) {
    //Size aca no me sirve pero para que no rompa lo dejo
    int size;
    void *buffer = recibir_buffer(&size, socket);
    t_consola *consola = deserializar_consola(buffer);
    free(buffer);
    return consola;
}

bool recibir_interrupcion(int socket)
{
    bool valor_interrupcion = false;
    int size;
    void *buffer = recibir_buffer(&size,socket);

    memcpy(&(valor_interrupcion),buffer,size);
    free(buffer);
    return valor_interrupcion;
}

void enviar_interrupcion(int socket)
{
    bool valor_interrupcion = true;

    t_operacion *operacion = crear_operacion(INTERRUPCION);
    setear_operacion(operacion, &valor_interrupcion);
    enviar_operacion(operacion, socket);
    eliminar_operacion(operacion);
}

void enviar_proceso_pcb(int socket, t_proceso_pcb *un_proceso_pcb, codigo_operacion un_codigo_operacion)
{
    t_operacion *operacion = crear_operacion(un_codigo_operacion);
    setear_operacion(operacion, un_proceso_pcb);
    enviar_operacion(operacion, socket);
    eliminar_operacion(operacion);
}

void *serializar_pcb(t_pcb *pcb, int *size_pcb) {
    int desplazamiento = 0;
    int size_consola = 0;
    void *consola_serializada = serializar_consola(pcb->consola, &size_consola);
    *size_pcb =  size_consola +sizeof(int)*4 + sizeof(int32_t);
    void *stream = malloc(*size_pcb);

    memcpy(stream + desplazamiento, &(pcb->pid), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &(pcb->program_counter), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &(pcb->un_estado), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &size_consola, sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, consola_serializada, size_consola);
    desplazamiento+= size_consola;
    memcpy(stream + desplazamiento, &(pcb->id_tabla_1n), sizeof(int32_t));
    return stream;
}

void serializar_proceso_pcb(t_proceso_pcb *proceso_bloqueo, t_operacion *operacion)
{
    int size_pcb;
    void *pcb_serializado = serializar_pcb(proceso_bloqueo->pcb,&size_pcb);
    operacion->buffer->size = sizeof(int) + size_pcb;
    operacion->buffer->stream = malloc(operacion->buffer->size);
    memcpy(operacion->buffer->stream, pcb_serializado, size_pcb);
    memcpy(operacion->buffer->stream + size_pcb,&(proceso_bloqueo->tiempo_bloqueo),sizeof(int));
    free(pcb_serializado);
}

void *serializar_consola(t_consola *consola, int *size) {
    int size_cola = 0, desplazamiento = 0;
    void *instrucciones_serializadas = serializar_instrucciones(consola->instrucciones, &size_cola);
    *size = size_cola + 2 * sizeof(int);
    void *stream = malloc(*size);
    memcpy(stream, &(consola->tamanio),  sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(stream + desplazamiento, &size_cola,  sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(stream + desplazamiento, instrucciones_serializadas, size_cola);
    free(instrucciones_serializadas);
    return stream;
}

void *serializar_instrucciones(t_queue *instrucciones, int *size_cola) {
    *size_cola = queue_size(instrucciones) * 3 * sizeof(int); //Ej: 2 instrucciones * 3 componentes * 4 tamanio int
    int desplazamiento = 0;
    void *stream = malloc(*size_cola);
    t_instruccion *instruccion;
    while (!queue_is_empty(instrucciones)) {
        instruccion = (t_instruccion *) queue_pop(instrucciones);
        memcpy(stream + desplazamiento, &(instruccion->instruc), sizeof(int));
        desplazamiento+= sizeof(int);
        memcpy(stream + desplazamiento, &(instruccion->parametro1), sizeof(int));
        desplazamiento+= sizeof(int);
        memcpy(stream + desplazamiento, &(instruccion->parametro2), sizeof(int));
        desplazamiento+= sizeof(int);
    }
    //*size_cola = desplazamiento;
    return stream;
}

t_pcb *recibir_pcb(int socket, void * buffer) {
    //Size aca no me sirve pero para que no rompa lo dejo
    int size;

    int desplazamiento = 0;
    t_pcb *pcb = malloc(sizeof(t_pcb));

    memcpy(&(pcb->pid), buffer, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&(pcb->program_counter), buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&(pcb->un_estado), buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&size, buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int); //Size consola
    pcb->consola = deserializar_consola(buffer+desplazamiento);
    desplazamiento+=size;
    memcpy(&(pcb->id_tabla_1n), buffer+desplazamiento, sizeof(int32_t));;
    return pcb;
}

t_proceso_pcb *deserializar_proceso_pcb(int socket)
{
    int size;
    void *buffer = recibir_buffer(&size,socket);

    t_proceso_pcb *un_proceso_pcb = malloc(sizeof(un_proceso_pcb));
    un_proceso_pcb->pcb = deserializar_pcb(buffer);

    memcpy(&(un_proceso_pcb->tiempo_bloqueo), buffer, sizeof(int));
    free(buffer);
    return un_proceso_pcb;
}

t_pcb *deserializar_pcb(void *buffer) {
    int size;
    int desplazamiento = 0;
    t_pcb *pcb = malloc(sizeof(t_pcb));

    memcpy(&(pcb->pid), buffer, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&(pcb->program_counter), buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&(pcb->un_estado), buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(&size, buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int); //Size consola
    pcb->consola = deserializar_consola(buffer+desplazamiento);
    desplazamiento+=size;
    memcpy(&(pcb->id_tabla_1n), buffer+desplazamiento, sizeof(int32_t));;

    return pcb;
}

t_consola *deserializar_consola(void *buffer) {
    int desplazamiento = 0;
    t_consola *consola = malloc(sizeof(t_consola));
    memcpy(&(consola->tamanio), buffer, sizeof(int));
    desplazamiento+=sizeof(int);
    int size_cola;
    memcpy(&(size_cola), buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int);
    consola->instrucciones = deserializar_instrucciones(buffer+desplazamiento, size_cola);

    return consola;
}

t_queue *deserializar_instrucciones(void *buffer, int size_cola) {
    t_queue *instrucciones = queue_create();
    int desplazamiento = 0;
    t_instruccion *instruccion;
    while(desplazamiento < size_cola) {
        instruccion = malloc(sizeof(t_instruccion));
        memcpy(&(instruccion->instruc), buffer + desplazamiento, sizeof(int));
        desplazamiento+=sizeof(int);
        memcpy(&(instruccion->parametro1), buffer+desplazamiento, sizeof(int));
        desplazamiento+=sizeof(int);
        memcpy(&(instruccion->parametro2), buffer+desplazamiento, sizeof(int));
        desplazamiento+=sizeof(int);
        queue_push(instrucciones,instruccion);
    }
    return instrucciones;
}

/*** CPU + MEMORIA ***/
t_solicitud *recibir_solicitud(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    t_solicitud *solicitud = malloc(sizeof(t_solicitud));
    memcpy(solicitud, buffer, size);
    free(buffer);
    return solicitud;
}

t_tercera_solicitud *recibir_tercera_solicitud(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    t_tercera_solicitud *solicitud = malloc(sizeof(t_tercera_solicitud));
    memcpy(solicitud, buffer, size);
    free(buffer);
    return solicitud;
}
/*** KERNEL + MEMORIA ***/
/*
 * @DESC: Recibe un entero en el buffer y lo retorna
 * Se puede usar para recibir un tamanio_proceso como el id_tabla_1n u otras cosas que
 * se te imaginen
 */
int32_t recibir_entero(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    uint32_t tamanio;
    memcpy(&tamanio, buffer, size);
    free(buffer);
    return tamanio;
}
/*
t_solicitud *recibir_solicitud(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    t_solicitud *solicitud = malloc(sizeof(t_solicitud));
    int desplazamiento = 0;
    memcpy(&(solicitud->id_tabla_1n), buffer, sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(&(solicitud->entrada_tabla), buffer + desplazamiento, sizeof(int32_t));
    desplazamiento += sizeof(int);
    memcpy(&(solicitud->id_tabla_2n), buffer + desplazamiento, sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(&(solicitud->nro_frame), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(solicitud->tamanio_direccion_base), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    solicitud->direccion_base = malloc(solicitud->tamanio_direccion_base);
    memcpy(solicitud->direccion_base, buffer + desplazamiento, solicitud->tamanio_direccion_base);
    free(buffer);
    return solicitud;
}

t_tercera_solicitud *recibir_tercera_solicitud(int socket) {
    int size;
    void *buffer = recibir_buffer(&size, socket);
    t_tercera_solicitud *solicitud = malloc(sizeof(t_tercera_solicitud));
    int desplazamiento = 0;
    memcpy(&(solicitud->nro_frame), buffer, sizeof(unsigned int));
    desplazamiento += sizeof(unsigned int);
    memcpy(&(solicitud->accion_solicitada), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(solicitud->estado_memo), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(solicitud->tamanio), buffer + desplazamiento, sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    if(solicitud->tamanio != UNDEFINED) {
        solicitud->dato = malloc(solicitud->tamanio);
        memcpy(solicitud->dato, buffer + desplazamiento, solicitud->tamanio);
        desplazamiento += solicitud->tamanio;
    }
    memcpy(&(solicitud->tamanio_direccion_fisica), buffer + desplazamiento, sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    solicitud->direccion_fisica = malloc(solicitud->tamanio_direccion_fisica);
    memcpy(solicitud->direccion_fisica, buffer + desplazamiento, solicitud->tamanio_direccion_fisica);
    free(buffer);
    return solicitud;
}

void *serializar_solicitud(t_solicitud *solicitud, int *tamanio) {
    *tamanio = sizeof(int32_t)*3 + sizeof(int)*2 + solicitud->tamanio_direccion_base;
    void *buffer = malloc(*tamanio);
    int desplazamiento = 0;
    memcpy(buffer, &(solicitud->id_tabla_1n), sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(buffer + desplazamiento, &(solicitud->entrada_tabla), sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(buffer + desplazamiento, &(solicitud->id_tabla_2n), sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(buffer + desplazamiento, &(solicitud->nro_frame), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(buffer + desplazamiento, &(solicitud->tamanio_direccion_base), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(buffer + desplazamiento, solicitud->direccion_base, solicitud->tamanio_direccion_base);
    return buffer;
}

void *serializar_tercera_solicitud(t_tercera_solicitud *solicitud, int *tamanio) {
    if(solicitud->tamanio != UNDEFINED) {
        *tamanio = sizeof(int)*3 + sizeof(int32_t)*2 + solicitud->tamanio_direccion_fisica + solicitud->tamanio;
    }
    else {
        *tamanio = sizeof(int)*3 + sizeof(int32_t)*2 + solicitud->tamanio_direccion_fisica;
    }
    void *buffer = malloc(*tamanio);
    int desplazamiento = 0;
    memcpy(buffer, &(solicitud->nro_frame), sizeof(unsigned int));
    desplazamiento += sizeof(unsigned int);
    memcpy(buffer + desplazamiento, &(solicitud->accion_solicitada), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(buffer + desplazamiento, &(solicitud->estado_memo), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(buffer + desplazamiento, &(solicitud->tamanio), sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    if(solicitud->tamanio != UNDEFINED) {
        memcpy(buffer + desplazamiento, solicitud->dato, solicitud->tamanio);
        desplazamiento += solicitud->tamanio;
    }
    memcpy(buffer + desplazamiento, &(solicitud->tamanio_direccion_fisica), sizeof(int32_t));
    desplazamiento += sizeof(int32_t);
    memcpy(buffer + desplazamiento, solicitud->direccion_fisica, solicitud->tamanio_direccion_fisica);
    return buffer;
}
*/