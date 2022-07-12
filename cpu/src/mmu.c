#include "../include/mmu.h"

void iniciar_mmu() {
    
}

uint32_t obtener_dato_memoria(dir_logica dir) {
    return procesar_solicitud(dir,READ_ACCION,0);
}

int escribir_dato_memoria(dir_logica dir, uint32_t dato) {
    return procesar_solicitud(dir, WRITE_ACCION, dato);
}

int procesar_solicitud(dir_logica dir, accion accion_pedida, uint32_t dato) {
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
    if(num_pagina > config_cpu.tamanio_pagina) {
        return ERROR;
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
        unsigned int nro_frame = solicitar_registro_2nivel(id_tabla_2n, entrada_tabla_2do_nivel(num_pagina));
        actualizar_tlb(nro_frame, num_pagina, pcb->id_tabla_1n);
        if(accion_pedida == WRITE_ACCION) {
            estado_memoria res = enviar_dato_memoria(calcular_desplazamiento(dir, num_pagina), nro_frame, dato);
            return res;
        }
        else {
            uint32_t dato = solicitar_dato(calcular_desplazamiento(dir, num_pagina), nro_frame);
            return dato;
        }
    }
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