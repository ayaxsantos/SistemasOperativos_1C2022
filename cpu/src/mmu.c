#include "../include/mmu.h"

void iniciar_mmu() {
    
}

uint32_t *obtener_dato_memoria(dir_logica dir) {
    char *dir_fisica = dir_logica_a_fisica(dir,READ_ACCION,0);
    t_pag_incompleta pag_incompleta;
    if(!data_esta_en_una_pagina(dir, &pag_incompleta)) {
        void *buffer = malloc(size_dato);
        //pthread_mutex_lock(&mutex_mp);
        memcpy(buffer, dir_fisica, pag_incompleta.size_restante);
        int size = size_dato - pag_incompleta.size_restante;
        int desplazamiento = pag_incompleta.size_restante;
        while(size != 0) {
            dir_fisica = dir_logica_a_fisica(pag_incompleta.fin_pagina, READ_ACCION, 0);
            if(data_esta_en_una_pagina(pag_incompleta.fin_pagina, &pag_incompleta)) {
                memcpy(buffer + desplazamiento, dir_fisica, size);
                break;
            }
            memcpy(buffer, dir_fisica, pag_incompleta.size_restante);
            desplazamiento += pag_incompleta.size_restante;
            size = size - pag_incompleta.size_restante;
        }
        return (uint32_t *)buffer;
    }
    //pthread_mutex_unlock(&mutex_mp);
    return (uint32_t *)dir_fisica;
}

int escribir_dato_memoria(dir_logica dir, uint32_t dato) {
    return dir_logica_a_fisica(dir, WRITE_ACCION, dato);
}

char *dir_logica_a_fisica(dir_logica dir, accion accion_pedida, int32_t size) {
    /*
     * Si esta en TLB -> HIT Devolver Dato
     * Sino -> MISS realizar accesos
     */
    /*
    * 1- Un primer acceso para conocer en qué tabla de páginas de 2do nivel
    *      está direccionado el marco en que se encuentra la página a la que queremos acceder
    * 2- Un segundo acceso para conocer en qué marco está la misma
    * 3- Finalmente acceder a la porción de memoria correspondiente (la dirección física).
    */
    int num_pagina = numero_pagina(dir);
    //TODO: VALIDAR
    if(num_pagina > config_cpu.tamanio_pagina) {
        return "-100";
    }
    //int resultado = obtener_nro_frame_de_tlb(num_pagina,pcb->id_tabla_1n);
    int resultado = ERROR;
    if(resultado != ERROR) {
        if(accion_pedida == WRITE_ACCION) {
            estado_memoria res = enviar_dato_memoria(calcular_desplazamiento(dir, num_pagina), resultado, dato);
            return res;
        }
        else {
            uint32_t dato = solicitar_dato(calcular_desplazamiento(dir, num_pagina), resultado);
            return dato;
        }
    }
    else {
        int entrada_tabla_1n = entrada_tabla_1er_nivel(num_pagina);
        int32_t id_tabla_2n = solicitar_registro_1nivel(pcb->id_tabla_1n, entrada_tabla_1n);
        t_solicitud *response = solicitar_registro_2nivel(id_tabla_2n, entrada_tabla_2do_nivel(num_pagina));
        actualizar_tlb(response->nro_frame, num_pagina, pcb->id_tabla_1n);
        if(accion_pedida == WRITE_ACCION) {
            estado_memoria res = enviar_dato_memoria(calcular_desplazamiento(dir, num_pagina), nro_frame, dato);
            return &res;
        }
        else {
            char *direccion_fisica = response->direccion_base + calcular_desplazamiento(dir, num_pagina);
            t_tercera_solicitud *dato = solicitar_dato(direccion_fisica, response->nro_frame, size);
            return dato->direccion_fisica;
        }
    }
}

bool data_esta_en_una_pagina(dir_logica dir_logica, t_pag_incompleta *pag_incomplenta) {
    pag_incomplenta->nro_pagina = (double) dir_logica / (double) config_cpu.tamanio_pagina;
    pag_incomplenta->fin_pagina  = (pag_incomplenta->nro_pagina + 1) * config_cpu.tamanio_pagina;
    pag_incomplenta->size_restante = pag_incomplenta->fin_pagina - dir_logica;
    return pag_incomplenta->size_restante >= size_dato;
}

////////////////////////////////////////////////////////
int numero_pagina(dir_logica dir) {
    return floor(dir / config_cpu.tamanio_pagina);
}

int entrada_tabla_1er_nivel(int numero_pagina) {
    return floor(numero_pagina / config_cpu.entradas_por_tabla);
}

int entrada_tabla_2do_nivel(int numero_pagina) {
    return numero_pagina % config_cpu.entradas_por_tabla;
}

int calcular_desplazamiento(dir_logica dir, int numero_pagina) {
    return dir - numero_pagina * config_cpu.tamanio_pagina;
}
