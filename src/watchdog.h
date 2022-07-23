#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <semaphore.h>

typedef struct watchdog_params {
	const char **names; // names of the threads
	int thread_count;
	volatile int *is_running;

	// used to signal when the watchdog has started, wait for it after the watchdog thread is created.
	sem_t started;
} watchdog_params_t;

// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog(watchdog_params_t *params);

// inform watchdog that thread id is running correctly. Id of the thread is the index of
// this thread in watchdog_params.names
void inform_watchdog(int id);

#endif // WATCHDOG_H
