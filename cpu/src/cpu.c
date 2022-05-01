#include "../include/mmu.h"

void iniciar() {
    int server_cpu = iniciar_servidor("127.0.0.1", "25657");
    /*
    conectar_a_memoria_y_recibir_config();
    esperar_a_kernel();
    recibir_procesos();*/
}