#ifndef LOGGER_H
#define LOGGER_H

#include <semaphore.h>

#define INFO    0b001
#define WARNING 0b010
#define ERROR   0b100

typedef struct logger_params {
	const char *file_name;
	int ring_buffer_size;
	int types_bitmask; // only messages with types specified in the bitmask will be written
	volatile int *is_running; 

	// used to signal when the logger has started, wait for it after the logger thread is created.
	sem_t started; 
} logger_params_t;

// logger thread. logs messages from it's ring buffer
void *logger(logger_params_t *params);

// puts message in logger's ring buffers
void log_message(int message_type, const char *s);

#endif // LOGGER_H

