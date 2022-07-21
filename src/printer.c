#include "printer.h"

#include <time.h>
#include <stdio.h>

#include "array.h"

ARRAY_BASIC_TYPE(float)

void *printer(printer_params_t *params) {
	ring_buffer_t *analyzer_printer_buffer = params->analyzer_printer_buffer;

	int count = 0;
	float_array_t *total = NULL;
	time_t prev_time = time(NULL);
	while (*params->is_running) {
		float_array_t *cpu_usage = read_packet(analyzer_printer_buffer);
		if (!total) {
			total = alloc_float_array(cpu_usage->count);
			memset(total->elems, 0, sizeof(float) * (size_t)total->count);
		}

		// update 'per frame' values
		count++;
		for (int i = 0; i < total->count; ++i) {
			total->elems[i] += cpu_usage->elems[i];
		}

		if (time(NULL) - prev_time >= 1) {
			// print usage, beginning is the escape sequence to clear the screen
			puts("\033[1;1H\033[2JCPU usage:");
			for (int i = 0; i < total->count; ++i) {
				printf("core %d: %f\n", i, (double)total->elems[i] / (double)count);
			}

			// reset 'per frame' values
			prev_time = time(NULL);
			count = 0;
			memset(total->elems, 0, sizeof(float) * (size_t)total->count);
		}

		free(cpu_usage);
	}

	return NULL;
}

