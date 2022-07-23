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

	if (pthread_mutex_init(&buffer->write_mtx, NULL) != 0) {
		perror("write mutex init");
		return -1;
	}

	if (pthread_mutex_init(&buffer->read_mtx, NULL) != 0) {
		perror("read mutex init");
		return -1;
	}

	if (sem_init(&buffer->packet_count, 0, 0) != 0) {
		perror("packet_count sem_init");
		return -1;
	}

	if (sem_init(&buffer->free_position_count, 0, (unsigned)size) != 0) {
		perror("free_position_count sem_init");
		return -1;
	}

	return 0;
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

	pthread_mutex_destroy(&buffer->write_mtx);
	pthread_mutex_destroy(&buffer->read_mtx);

	sem_destroy(&buffer->packet_count);
	sem_destroy(&buffer->free_position_count);
}

void write_packet(ring_buffer_t *buffer, void *packet) {
	pthread_mutex_lock(&buffer->write_mtx);
	sem_wait(&buffer->free_position_count);

	buffer->packets[buffer->next_write++] = packet;
	buffer->next_write = wrap(buffer->next_write, buffer->size);

	sem_post(&buffer->packet_count);
	pthread_mutex_unlock(&buffer->write_mtx);
}

void* read_packet(ring_buffer_t *buffer) {
	pthread_mutex_lock(&buffer->read_mtx);
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
	pthread_mutex_unlock(&buffer->read_mtx);

	return packet;
}

