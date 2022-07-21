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
