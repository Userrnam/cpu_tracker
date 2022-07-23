#ifndef ANALYZER_H
#define ANALYZER_H

#include "cpu_stat.h"
#include "ring_buffer.h"
#include "array.h"

typedef struct analyzer_params {
	ring_buffer_t *reader_analyzer_buffer;
	ring_buffer_t *analyzer_printer_buffer;
	int thread_id;
	volatile int *is_running;
} analyzer_params_t;

typedef struct cpu_times {
	int idle;
	int non_idle;
} cpu_times_t;

ARRAY(cpu_times)
ARRAY_BASIC_TYPE(float)

// calc idle and non_idle cpu times.
void calc_cpu_times(const cpu_stat_array_t *stat, cpu_times_array_t *times);

// calculates cpu usage based on cpu times. If cpu times are the same returns NULL.
float_array_t *calc_cpu_usage(const cpu_times_array_t *prev, const cpu_times_array_t *cur);

// receives data from reader, calculates CPU usage for each CPU core and sends it to printer
void *analyzer(const analyzer_params_t*);

#endif // ANALYZER_H
