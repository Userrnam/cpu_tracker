#include "analyzer.h"

#include "array.h"
#include "cpu_stat.h"

typedef struct cpu_times {
	int idle;
	int non_idle;
} cpu_times_t;

void calc_cpu_times(cpu_stat_t *stat, cpu_times_t *times) {
	times->idle     = stat->idle + stat->iowait;
	times->non_idle = stat->user + stat->system + stat->irq + stat->softirq + stat->steal;
}

ARRAY(cpu_times)
ARRAY_BASIC_TYPE(float)

void *analyzer_thread(void* _params) {
	analyzer_params_t *params = _params;

	ring_buffer_t *reader_analyzer_buffer  = params->reader_analyzer_buffer;
	ring_buffer_t *analyzer_printer_buffer = params->analyzer_printer_buffer;

	cpu_times_array_t *prev = NULL;
	cpu_times_array_t *cur  = NULL;

	while (1) {
		cpu_stat_array_t *cpu_stat_array = read_packet(reader_analyzer_buffer);

		// if it's first packet, create cpu_times arrays and continue.
		if (!prev) {
			prev = alloc_cpu_times_array(cpu_stat_array->count);
			cur  = alloc_cpu_times_array(cpu_stat_array->count);
			for (int i = 0; i < prev->count; ++i) {
				calc_cpu_times(&cpu_stat_array->elems[i], &prev->elems[i]);
			}
			continue;
		}
		for (int i = 0; i < prev->count; ++i) {
			calc_cpu_times(&cpu_stat_array->elems[i], &cur->elems[i]);
		}
		free(cpu_stat_array);

		float_array_t *cpu_usage = alloc_float_array(cur->count);
		for (int i = 0; i < cur->count; ++i) {
			int total_delta = (cur->elems[i].idle + cur->elems[i].non_idle) -
							  (prev->elems[i].idle + prev->elems[i].non_idle);
			int idle_delta  = cur->elems[i].idle - prev->elems[i].idle;

			cpu_usage->elems[i] = (float)(total_delta - idle_delta) / (float)total_delta;
		}
		write_packet(analyzer_printer_buffer, cpu_usage);
	}

	free(prev);
	free(cur);
}

