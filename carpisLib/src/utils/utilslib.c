#include "../../include/utils/utilslib.h"

void mostrar_error(char *exmsj, t_log *logger) {
	log_error(logger, exmsj);
}

/*********************************************/
void enviar_funcion(t_funcion* funcion, int socket_cliente) {
	int bytes;
	void* a_enviar;
	bytes = funcion->buffer->size + 2*sizeof(int);
	a_enviar = serializar_funcion(funcion, bytes);
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

t_operacion * crear_operacion(codigo_operacion cod_op)
{
	t_operacion *operacion = malloc(sizeof(t_operacion));
    operacion->cod_op = cod_op;
	crear_buffer_funcion(operacion);
	return operacion;
}

void setear_funcion(t_operacion * funcion, void* valor)
{
	int desplazamiento = 0;
	void *stream;
	//TO DO 

	//AUX
	int *pid;

	int f_code = funcion->cod_op;
	switch(f_code) {
		default:break;
	}
	return;
}


void crear_buffer_operacion(t_operacion *operacion) {
    operacion->buffer = malloc(sizeof(t_buffer));
    operacion->buffer->size = 0;
    operacion->buffer->stream = NULL;
}
/*Deserializar segun los tipos*/
/*********************************************/
/*EJEMPLO
t_carpincho_sem* recibir_semaforo(int socket_cliente)
{
	//Size aca no me sirve pero para que no rompa lo dejo
	int size;

	int desplazamiento = 0;
	void * buffer;
	t_carpincho_sem* semaforo = malloc(sizeof(t_carpincho_sem));

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&(semaforo->mate_pid), buffer, sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(&(semaforo->valor), buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(&(semaforo->tamanio_nombre), buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	semaforo->nombre = malloc(semaforo->tamanio_nombre);
	memcpy(semaforo->nombre, buffer+desplazamiento, semaforo->tamanio_nombre);
	
	free(buffer);
	return semaforo;
}*/

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

void eliminar_funcion(t_funcion* funcion) {
	free(funcion->buffer->stream);
	free(funcion->buffer);
	free(funcion);
}


