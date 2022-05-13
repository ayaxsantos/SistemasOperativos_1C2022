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

void serializar_pcb(t_pcb *pcb, t_operacion *operacion) {
    int desplazamiento = 0;
    int size_consola = 0, size_tabla = 0;
    void *consola_serializada = serializar_consola(pcb->consola, &size_consola);
    void *tabla_serializada = serializar_tabla1n(pcb->tabla_1n,&size_tabla);
    operacion->buffer->size =  size_consola + size_tabla +sizeof(int)*3;
    void *stream = malloc(operacion->buffer->size);

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
    memcpy(stream + desplazamiento, &size_tabla, sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, tabla_serializada, size_tabla);
    operacion->buffer->stream = stream;
}

void *serializar_consola(t_consola *consola, int *size) {
    int size_cola, desplazamiento = 0;
    void *instrucciones_serializadas = serializar_instrucciones(consola->instrucciones, &size_cola);
    *size = size_cola + sizeof(int);
    void *stream = malloc(*size);
    memcpy(stream, &(consola->tamanio),  sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(stream + desplazamiento, &size_cola,  sizeof(int));
    desplazamiento+=sizeof(int);
    memcpy(stream + desplazamiento, stream, size_cola);
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
        free(instruccion);
    }
    //*size_cola = desplazamiento;
    return stream;
}

void *serializar_tabla1n(t_dictionary *tabla1n, int *size) {
    *size = dictionary_size(tabla1n) * sizeof(int); //Ej: 2 registros * Tamanio int
    int desplazamiento = 0;
    void *stream = malloc(*size);
    int nro_tabla;
    int i = 0;
    char *index;
    while (!dictionary_is_empty(tabla1n)) {
        index = string_itoa(i);
        nro_tabla = *((int *) dictionary_get(tabla1n,index));
        memcpy(stream + desplazamiento, &nro_tabla, sizeof(int));
        desplazamiento+= sizeof(int);
        free(index);
        i++;
    }
    //*size = desplazamiento;
    return stream;
}

t_pcb *deserializar_pcb(int socket) {
    //Size aca no me sirve pero para que no rompa lo dejo
    int size;

    int desplazamiento = 0;
    void * buffer;
    t_pcb *pcb = malloc(sizeof(t_pcb));

    buffer = recibir_buffer(&size, socket);

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
    memcpy(&size, buffer+desplazamiento, sizeof(int));
    desplazamiento+=sizeof(int); //Size tabla
    pcb->tabla_1n = deserializar_tabla1n(buffer + desplazamiento, size);
    free(buffer);
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

t_dictionary *deserializar_tabla1n(void *buffer, int size_tabla) {
    t_dictionary *tabla = dictionary_create();
    int desplazamiento = 0;
    int nro_tabla;
    while(desplazamiento < size_tabla) {
        memcpy(&nro_tabla, buffer + desplazamiento, sizeof(int));
        desplazamiento+=sizeof(int);
    }
    return tabla;
}