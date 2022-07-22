#include "reader.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cpu_stat.h"

#define BUFFER_SIZE 2048

void *reader(reader_params_t *params) {
	FILE *fp = fopen("/proc/stat", "r");

	ring_buffer_t *reader_analyzer_buffer = params->reader_analyzer_buffer;

	// contents of file /proc/stat is constantly updating, so we copy should copy it into a
	// buffer before processing.
	char buf[BUFFER_SIZE];

	// 0 means uninitialized
	int cpu_count = 0;
	while (*params->is_running) {
		memset(buf, 0, BUFFER_SIZE);
		rewind(fp);
		fread(buf, 1, BUFFER_SIZE-1, fp);

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

		// sleep for 1ms otherwise our cpu tracker will use 100% of CPU!
		usleep(1000);
	}

	fclose(fp);

	return NULL;
}

