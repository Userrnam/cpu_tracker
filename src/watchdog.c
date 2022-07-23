#include "watchdog.h"

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "array.h"
#include "logger.h"

// contains time of last message from the threads
ARRAY_BASIC_TYPE(int);
static int_array_t *inform_time;

void *watchdog(watchdog_params_t *params) {
	log_message(INFO, "watchdog started");

	inform_time = alloc_int_array(params->thread_count);

	// initialize inform_time array to current time
	for (int i = 0; i < inform_time->count; ++i) {
		inform_time->elems[i] = time(NULL);
	}

	sem_post(&params->started);

	char buf[512];
	while (*params->is_running) {
		time_t current_time = time(NULL);
		for (int i = 0; i < inform_time->count; ++i) {
			if (current_time - inform_time->elems[i] >= 2) {
				sprintf(buf, "Watchdog: no message from %s in 2s", params->names[i]);
				fprintf(stderr, "%s\n", buf);
				log_message(ERROR, buf);

				// FIXME: I'm testing with SIGINT, the task says to use SIGTERM
				raise(SIGINT);
				break;
			}
		}
		usleep(1000);
	}

	free(inform_time);

	log_message(INFO, "watchdog stopped");

	return NULL;
}

void inform_watchdog(int id) {
	if (!inform_time) {
		log_message(ERROR, "inform_watchdog: watchdog is not initialized");
		return;
	}
	if (id < 0 || id >= inform_time->count) {
		log_message(ERROR, "inform_watchdog: wrong thread id");
		return;
	}

	inform_time->elems[id] = time(NULL);
}

