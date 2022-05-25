#include "../include/mmu.h"

void iniciar_mmu() {
    
}

char *dir_logica_a_fisica(dir_logica dir_logica, t_pcb *pcb, t_accion accion) {
    int cantidad_paginas = dictionary_size(pcb->tabla_1n);
    int dir_nro_pagina = get_nro_pagina(dir_logica);
    if(dir_nro_pagina >= cantidad_paginas) {
        //No se encontro la direccion logica;
        return "-100";
    }
    int desplazamiento = dir_logica  - config_memoria.tamanio_pagina * dir_nro_pagina;
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
}