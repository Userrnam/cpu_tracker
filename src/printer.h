#ifndef PRINTER_H
#define PRINTER_H

#include "ring_buffer.h"

typedef struct printer_params {
	ring_buffer_t *analyzer_printer_buffer;
	int *is_running;
} printer_params_t;

// reads raw data from /proc/stat and sends it to analyzer via curcular buffer
// receives data from analyzer and prints it to the console
void *printer(printer_params_t*);

#endif // PRINTER_H
