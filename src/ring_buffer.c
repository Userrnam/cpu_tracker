#include "ring_buffer.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static int wrap(int index, int size) {
	return index >= size ? index - size : index;
}

int create(ring_buffer_t *buffer, int size) {
	buffer->packets = (void *)calloc((size_t)size, sizeof(void *));
	if (!buffer->packets) {
		return -1;
	}

	buffer->size  = size;
	buffer->next_read  = 0;
	buffer->next_write = 0;

	// this should not fail.
	sem_init(&buffer->packet_count, 0, 0);

	// this can fail if size > SEM_VALUE_MAX
	return sem_init(&buffer->free_position_count, 0, (unsigned)size);
}

void destroy(ring_buffer_t *buffer) {
	for (int i = buffer->next_read;; ++i) {
		int index = wrap(i, buffer->size);
		if (index == buffer->next_write) {
			break;
		}
		free(buffer->packets[index]);
	}
	free(buffer->packets);

	sem_destroy(&buffer->packet_count);
	sem_destroy(&buffer->free_position_count);
}

void write_packet(ring_buffer_t *buffer, void *packet) {
	sem_wait(&buffer->free_position_count);

	buffer->packets[buffer->next_write++] = packet;
	buffer->next_write = wrap(buffer->next_write, buffer->size);

	sem_post(&buffer->packet_count);
}

void* read_packet(ring_buffer_t *buffer) {
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    ts.tv_sec += 1;

	if (sem_timedwait(&buffer->packet_count, &ts) == -1) {
		return NULL;
	}

	void *packet = buffer->packets[buffer->next_read];
	buffer->packets[buffer->next_read++] = NULL;
	buffer->next_read = wrap(buffer->next_read, buffer->size);

	sem_post(&buffer->free_position_count);

	return packet;
}

