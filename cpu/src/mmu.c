#include "../include/mmu.h"

void iniciar_mmu() {
    
}

void *obtener_dato_memoria(dir_logica dir, t_pcb *pcb) {
    /*
     * 1- Un primer acceso para conocer en qué tabla de páginas de 2do nivel
     *      está direccionado el marco en que se encuentra la página a la que queremos acceder
     * 2- Un segundo acceso para conocer en qué marco está la misma
     * 3- Finalmente acceder a la porción de memoria correspondiente (la dirección física).
     
    int num_pagina = numero_pagina(dir);
    int entrada_tabla_1n = entrada_tabla_1er_nivel(num_pagina);
    int id_tabla_2n = solicitar_registro_1nivel(pcb->id_tabla_1n, entrada_tabla_1n);
    t_columna_pagina *pagina = solicitar_registro_2nivel(id_tabla_2n, entrada_tabla_2do_nivel(num_pagina));
    return pagina;*/
}
/*
char *dir_logica_a_fisica(dir_logica dir, t_pcb *pcb, t_accion accion) {
    int cantidad_paginas = dictionary_size(pcb->tabla_1n);
    int dir_nro_pagina = get_nro_pagina(dir);
    if(dir_nro_pagina >= cantidad_paginas) {
        //No se encontro la direccion logica;
        return "-100";
    }
    int desplazamiento = dir - config_memoria.tamanio_pagina * dir_nro_pagina;
    int resultado = obtener_nro_frame_de_tlb(dir_nro_pagina, tabla_carpincho->pid);
    t_frame *frame;
    if(resultado != -1) {
        frame = list_get(memoria_principal->frames, resultado);
        if(accion == WRITE)
            frame->modificado = accion;
        frame->usado = true;
        tabla_carpincho->cantidad_hit++;
        actualizar_tiempo_usado(frame);
        return frame->base + desplazamiento;
    }
    else {
        char *nro_pagina = string_itoa(dir_nro_pagina);
        tabla_carpincho->cantidad_miss++;
        t_columna_pagina *pagina = (t_columna_pagina *)dictionary_get(tabla_carpincho->tabla, nro_pagina);
        if(pagina->presencia) {
            frame = list_get(memoria_principal->frames, pagina->nro_frame);
            if(accion == WRITE)
                frame->modificado = accion;
            frame->usado = true;
            actualizar_tiempo_usado(frame);
            monitorear_tabla_carpincho(tabla_carpincho);
            actualizar_tlb(pagina->nro_frame, dir_nro_pagina, tabla_carpincho->pid);
            free(nro_pagina);
            return frame->base + desplazamiento;
        }
        frame = realizar_algoritmo(tabla_carpincho, pagina, accion, dir_nro_pagina);
        monitorear_tabla_carpincho(tabla_carpincho);
        actualizar_tlb(frame->nro_frame, dir_nro_pagina, tabla_carpincho->pid);
        free(nro_pagina);
        return frame->base + desplazamiento;
    }
}*/

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
