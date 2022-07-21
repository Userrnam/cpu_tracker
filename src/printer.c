#include "printer.h"

#include <stdio.h>

#include "array.h"

ARRAY_BASIC_TYPE(float)

void *printer_thread(printer_params_t *params) {
	ring_buffer_t *analyzer_printer_buffer = params->analyzer_printer_buffer;

	while (*params->is_running) {
		float_array_t *cpu_usage = read_packet(analyzer_printer_buffer);

		// beginning is the escape sequence to clear the screen
		puts("\033[2JCPU usage:");
		for (int i = 0; i < cpu_usage->count; ++i) {
			printf("core %d: %f\n", i, (double)cpu_usage->elems[i]);
		}

		free(cpu_usage);
	}

	return NULL;
}

