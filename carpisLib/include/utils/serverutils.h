#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>

void* recibir_buffer(int*, int);

int iniciar_servidor(char *ip, char *puerto);
int esperar_cliente(int);
t_list* recibir_paquete(int);
char *recibir_mensaje(int);
int recibir_operacion(int);

#endif /* UTILS_H_ */

