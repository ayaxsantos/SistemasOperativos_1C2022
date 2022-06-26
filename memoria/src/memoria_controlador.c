#include "../include/memoria_controlador.h"

t_frame *recorrer_frames(t_tabla_pagina *tabla_pagina) {
    int i;
    t_frame *frame = NULL;
    //pthread_mutex_lock(&mutex_mp);
    for(i=0;i<memoria_principal->cantidad_frames;i++) {
		frame = (t_frame*)list_get(memoria_principal->frames, i);
		if(frame->is_free) {
            return frame;
            //pthread_mutex_unlock(&mutex_mp);
		}
	}
    //pthread_mutex_unlock(&mutex_mp);
    return NULL;
}

t_frame *realizar_algoritmo(t_tabla_pagina *tabla_pagina, t_col_pagina *registro, accion accion, int nro_pagina) {
    t_frame *frame = NULL;
    switch (tipo_algoritmo) {
    case CLOCK:
            //sem_wait(&sem_algoritmo);
            //frame = realizar_algoritmo_clock(tabla_carpincho, registro, accion, nro_pagina);
            //sem_post(&sem_algoritmo);
        break;
    case CLOCKM:
            //sem_wait(&sem_algoritmo);
            //frame = realizar_algoritmo_lru(tabla_carpincho, registro, accion, nro_pagina);
            //sem_post(&sem_algoritmo);
        break;
    }
    return frame;
}
