#include "reader.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "watchdog.h"

#define BUFFER_SIZE 2048

cpu_stat_array_t *parse_cpu_stats(const char *s) {
	// get cpu count.
	int core_count = 0;
	const char *p = s;
	while (1) {
		p = strchr(p, '\n')+1;
		if (memcmp(p, "cpu", 3) == 0) {
			core_count++;
		} else {
			break;
		}
	}

	// parse cpu stats
	cpu_stat_array_t *cpu_stats = alloc_cpu_stat_array(core_count);
	p = s;
	for (int i = 0; i < core_count; ++i) {
		p = strchr(p, '\n')+1;
		sscanf(p, "%*s %d %d %d %d %d %d %d %d %d %d",
				&cpu_stats->elems[i].user,
				&cpu_stats->elems[i].nice,
				&cpu_stats->elems[i].system,
				&cpu_stats->elems[i].idle,
				&cpu_stats->elems[i].iowait,
				&cpu_stats->elems[i].irq,
				&cpu_stats->elems[i].softirq,
				&cpu_stats->elems[i].steal,
				&cpu_stats->elems[i].guest,
				&cpu_stats->elems[i].guest_nices);
	}
	return cpu_stats;
}

void *reader(const reader_params_t *params) {
	log_message(INFO, "reader started");

	FILE *fp = fopen("/proc/stat", "r");

	ring_buffer_t *reader_analyzer_buffer = params->reader_analyzer_buffer;

	// contents of file /proc/stat is constantly updating, so we copy it into a
	// buffer before processing.
	char buf[BUFFER_SIZE];

	while (*params->is_running) {
		inform_watchdog(params->thread_id);
		memset(buf, 0, BUFFER_SIZE);
		rewind(fp);
		fread(buf, 1, BUFFER_SIZE-1, fp);

		cpu_stat_array_t *packet = parse_cpu_stats(buf);

		write_packet(reader_analyzer_buffer, packet);

		// sleep for 1ms otherwise our cpu tracker will use 100% of CPU!
		usleep(1000);
	}

	fclose(fp);

	log_message(INFO, "reader stopped");

	return NULL;
}

