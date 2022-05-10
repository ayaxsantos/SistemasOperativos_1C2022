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

int recibir_handshake(int *socket, void(*mapeador)(int*, modulo)) {
    codigo_operacion cod_op = recibir_operacion(*socket);
    if(cod_op == HANDSHAKE) {
        modulo modulo_solicitante = recibir_operacion(*socket);
        mapeador(socket, modulo_solicitante);
        return 0;
    }
    return ERROR;
}

void setear_operacion(t_operacion *operacion, void *valor) {
    codigo_operacion op_code = operacion->cod_op;

    switch (op_code) {
        case PCB:
            serializar_pcb((t_pcb *)valor, operacion);
            break;
    }
}

void serializar_pcb(t_pcb *pcb, t_operacion *operacion) {
    operacion->buffer->size =  + sizeof(int)*5;
    void *stream = malloc(operacion->buffer->size);
    memcpy(stream + desplazamiento, &(semaforo->mate_pid), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &(semaforo->codigo_error), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &(semaforo->valorInicial), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, &(semaforo->tamanio_nombre), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(stream + desplazamiento, semaforo->nombre, semaforo->tamanio_nombre);
    funcion->buffer->stream = stream;
}