#include "../../include/general/carpisLib.h"

void habilitar_log(t_log *log, int habilitacion_log) {
	log->is_active_console = habilitacion_log;
}