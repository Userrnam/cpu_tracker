#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "ring_buffer.h"

typedef struct cpu_stat {
	int user;
	int nice;
	int system;
	int idle;
	int iowait;
	int irq;
	int softirq;
	int steal;
	int guest;
	int guest_nices;
} cpu_stat_t;

typedef struct cpu_stat_array {
	int count;
	cpu_stat_t elems[];
} cpu_stat_array_t;

cpu_stat_array_t *alloc_cpu_stat_array(int count) {
	cpu_stat_array_t *res = malloc(sizeof(int) + count * sizeof(cpu_stat_t));
	res->count = count;
	return res;
}

ring_buffer_t reader_analyzer_buffer;

// reads raw data from /proc/stat and sends it to analyzer via curcular buffer
void *reader_thread(void*) {
	int fd = open("/proc/stat", O_RDONLY);

	// 2048 bytes should be enough
	char buf[2048];

	// 0 means un initialized
	int cpu_count = 0;
	while (1) {
		memset(buf, 0, 2048);
		read(fd, buf, 2047);

		if (cpu_count == 0) {
			// get cpu count
			while (1) {
				char *p = strchr(buf, '\n')+1;
				if (memcmp(p, "cpu", 3) == 0) {
					cpu_count++;
				} else {
					break;
				}
			}
		}
		cpu_stat_array_t *packet = alloc_cpu_stat_array(cpu_count);
		for (int i = 0; i < cpu_count; ++i) {
			char *p = strchr(buf, '\n')+1;
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
		write_packet(&reader_analyzer_buffer, packet);
	}

	close(fd);
}

// receives data from reader, calculates CPU usage for each CPU core and sends it to printer
void *analyzer_thread(void*);

// receives data from analyzer and prints it to the console
void *printer_thread(void*);

// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog_thread(void*);

void *logger_thread(void*);

void *sigterm_handler(int signum);

int main() {

	return 0;
}
