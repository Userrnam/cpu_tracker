#include "analyzer.h"

#include "logger.h"
#include "watchdog.h"

void calc_cpu_times(const cpu_stat_array_t *stats, cpu_times_array_t *times) {
	for (int i = 0; i < stats->count; ++i) {
		times->elems[i].idle     = stats->elems[i].idle + stats->elems[i].iowait;
		times->elems[i].non_idle = stats->elems[i].user + stats->elems[i].system + stats->elems[i].irq
							     + stats->elems[i].softirq + stats->elems[i].steal;
	}
}

float_array_t *calc_cpu_usage(const cpu_times_array_t *prev, const cpu_times_array_t *cur) {
	float_array_t *cpu_usage = alloc_float_array(cur->count);
	for (int i = 0; i < cur->count; ++i) {
		int total_delta = (cur->elems[i].idle + cur->elems[i].non_idle) -
						  (prev->elems[i].idle + prev->elems[i].non_idle);
		if (total_delta == 0) {
			free(cpu_usage);
			return NULL;
		}
		int idle_delta  = cur->elems[i].idle - prev->elems[i].idle;

		cpu_usage->elems[i] = (float)(total_delta - idle_delta) / (float)total_delta;
	}
	return cpu_usage;
}

void *analyzer(const analyzer_params_t* params) {
	log_message(INFO, "analyzer started");

	ring_buffer_t *reader_analyzer_buffer  = params->reader_analyzer_buffer;
	ring_buffer_t *analyzer_printer_buffer = params->analyzer_printer_buffer;

	cpu_times_array_t *prev = NULL;
	cpu_times_array_t *cur  = NULL;

	while (*params->is_running) {
		inform_watchdog(params->thread_id);
		cpu_stat_array_t *cpu_stat_array = read_packet(reader_analyzer_buffer);
		// timeout
		if (!cpu_stat_array) {
			log_message(WARNING, "analyzer timeout");
			continue;
		}

		// if it's first packet, create cpu_times arrays and continue.
		if (!prev) {
			prev = alloc_cpu_times_array(cpu_stat_array->count);
			cur  = alloc_cpu_times_array(cpu_stat_array->count);
			calc_cpu_times(cpu_stat_array, prev);
			free(cpu_stat_array);
			continue;
		}

		calc_cpu_times(cpu_stat_array, cur);
		free(cpu_stat_array);

		float_array_t *cpu_usage = calc_cpu_usage(prev, cur);
		if (cpu_usage) {
			write_packet(analyzer_printer_buffer, cpu_usage);

			// swap cur and prev
			cpu_times_array_t *tmp = cur;
			cur = prev;
			prev = tmp;
		}
	}

	log_message(INFO, "analyzer stopped");

	free(prev);
	free(cur);

	return NULL;
}

