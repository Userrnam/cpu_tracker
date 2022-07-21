#include "reader.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cpu_stat.h"


void *reader(reader_params_t *params) {
	FILE *fp = fopen("/proc/stat", "r");

	ring_buffer_t *reader_analyzer_buffer = params->reader_analyzer_buffer;

	// 2048 bytes should be enough
	char buf[2048];

	// 0 means uninitialized
	int cpu_count = 0;
	while (*params->is_running) {
		memset(buf, 0, 2048);
		rewind(fp);
		int count = fread(buf, 1, 2047, fp);
		printf("read: %d", count);

		if (cpu_count == 0) {
			// get cpu count
			char *p = buf;
			while (1) {
				p = strchr(p, '\n')+1;
				if (memcmp(p, "cpu", 3) == 0) {
					cpu_count++;
				} else {
					break;
				}
			}
		}
		cpu_stat_array_t *packet = alloc_cpu_stat_array(cpu_count);
		char *p = buf;
		for (int i = 0; i < cpu_count; ++i) {
			p = strchr(p, '\n')+1;
			sscanf(p, "%*s %d %d %d %d %d %d %d %d %d %d",
					&packet->elems[i].user,
					&packet->elems[i].nice,
					&packet->elems[i].system,
					&packet->elems[i].idle,
					&packet->elems[i].iowait,
					&packet->elems[i].irq,
					&packet->elems[i].softirq,
					&packet->elems[i].steal,
					&packet->elems[i].guest,
					&packet->elems[i].guest_nices);
		}

		write_packet(reader_analyzer_buffer, packet);
	}

	fclose(fp);

	return NULL;
}

