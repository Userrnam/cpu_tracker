#ifndef PRINTER_H
#define PRINTER_H

#include "ring_buffer.h"

typedef struct printer_params {
	ring_buffer_t *analyzer_printer_buffer;
	int *is_running;
} printer_params_t;

void *printer_thread(printer_params_t*);

#endif // PRINTER_H
