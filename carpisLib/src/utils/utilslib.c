#include "../../include/utils/utilslib.h"

void enviar_operacion(t_operacion *operacion, int socket_cliente) {
	int bytes;
	void* a_enviar;
	bytes = operacion->buffer->size + 2*sizeof(int);
	a_enviar = serializar_operacion(operacion, bytes);
	send(socket_cliente, a_enviar, bytes, 0);
	free(a_enviar);
}

void *serializar_operacion(t_operacion *operacion, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(operacion->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(operacion->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, operacion->buffer->stream, operacion->buffer->size);

	return magic;
}

void setear_operacion(t_operacion *operacion, void *valor) {
    int op_code = operacion->cod_op;
    int size;
    void *stream;

    switch (op_code) {
		case T_CONSOLA:
            stream = serializar_consola((t_consola *)valor, &size);
            operacion->buffer->size = size;
            operacion->buffer->stream = stream;
            break;
		case INICIO_PROCESO:
            size = sizeof(int32_t) + sizeof(unsigned int) + sizeof(int);
            stream = malloc(size);
            memcpy(stream, valor, size);
            operacion->buffer->size = size;
            operacion->buffer->stream = stream;
			break;
        case PCB:
		case BLOQUEO:
		case FIN_PROCESO:
			serializar_proceso_pcb((t_proceso_pcb *)valor,operacion);
			break;
		case INTERRUPCION:
		case FIN_PROCESO_MEMORIA:
		case SUSPENSION_PROCESO:
			size = sizeof(int);
			stream = malloc(size);
			memcpy(stream, valor, size);
			operacion->buffer->size = size;
            operacion->buffer->stream = stream;
			break;
        case PRIMERA_SOLICITUD:
        case SEGUNDA_SOLICITUD:
        case TERCERA_SOLICITUD:
            size = sizeof(int32_t) * 3 + sizeof(unsigned int)*2 + sizeof(int)*2 + sizeof(uint32_t);
            stream = malloc(size);
            memcpy(stream, valor, size);
            operacion->buffer->size = size;
            operacion->buffer->stream = stream;
            break;
    }
    return;
}

t_operacion *crear_operacion(codigo_operacion cod_op) {
	t_operacion *operacion = malloc(sizeof(t_operacion));
    operacion->cod_op = cod_op;
    crear_buffer_operacion(operacion);
	return operacion;
}

void crear_buffer_operacion(t_operacion *operacion) {
    operacion->buffer = malloc(sizeof(t_buffer));
    operacion->buffer->size = 0;
    operacion->buffer->stream = NULL;
}

/*********************************************/
//-----------------Liberaciones de memoria---------------------/

void eliminar_operacion(t_operacion *operacion) {
	free(operacion->buffer->stream);
	free(operacion->buffer);
	free(operacion);
}


