#ifndef ANALYZER_H
#define ANALYZER_H

#include "ring_buffer.h"

typedef struct analyzer_params {
	ring_buffer_t *reader_analyzer_buffer;
	ring_buffer_t *analyzer_printer_buffer;
} analyzer_params_t;

// receives data from reader, calculates CPU usage for each CPU core and sends it to printer
void *analyzer_thread(void*);

#endif // ANALYZER_H
