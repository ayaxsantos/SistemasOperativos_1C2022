#include "../../include/utils/utilslib.h"

void mostrar_error(char *exmsj, t_log *logger) {
	log_error(logger, exmsj);
}

/*********************************************/
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
    codigo_operacion op_code = operacion->cod_op;
    int desplazamiento = 0, size;
    t_consola *consola;
    void *stream;

    switch (op_code) {
        case PCB:
            serializar_pcb((t_pcb *)valor, operacion);
            break;
        case T_CONSOLA:
            stream = serializar_consola((t_consola *)valor, &size);
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

int32_t recibir_direccion_memoria(int socket_cliente) {
	int32_t cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int32_t), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

/*********************************************/
//-----------------Liberaciones de memoria---------------------/

void eliminar_operacion(t_operacion *operacion) {
	free(operacion->buffer->stream);
	free(operacion->buffer);
	free(operacion);
}


