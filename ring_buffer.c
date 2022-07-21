#include "ring_buffer.h"

#include <stdlib.h>


static int wrap(int index, int size) {
	return index >= size ? index - size : index;
}

int create(ring_buffer_t *buffer, int size) {
	buffer->packets = (void *)calloc(size, sizeof(void *));
	if (!buffer->packets) {
		return -1;
	}

	buffer->size  = size;
	buffer->next_read  = 0;
	buffer->next_write = 0;

	// this should not fail.
	sem_init(&buffer->packet_count, 0, 0);

	// this can fail if size > SEM_VALUE_MAX
	return sem_init(&buffer->free_position_count, 0, size);
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
