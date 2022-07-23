#ifndef READER_H
#define READER_H

#include "ring_buffer.h"

#include "cpu_stat.h"

typedef struct reader_params {
    ring_buffer_t *reader_analyzer_buffer;
	int thread_id;
    volatile int *is_running;
} reader_params_t;

cpu_stat_array_t *parse_cpu_stats(const char *s);
void *reader(const reader_params_t *params);

#endif // READER_H
