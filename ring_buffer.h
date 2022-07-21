#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <semaphore.h>

// used to send data packet from one thread to another
typedef struct ring_buffer {
	void **packets;
	int size;        // size of the buffer
	int next_read;   // next packet is read at this index
	int next_write;  // next packet is written at this index

	sem_t read_block;   // block read when the buffer is empty
	sem_t write_block;  // block write when the buffer is full
} ring_buffer_t;

void create(ring_buffer_t *ring_buffer, int size);
void destroy(ring_buffer_t *ring_buffer);

// writes packet to the buffer, blocks if current_size > buffer_size
void  write_packet(ring_buffer_t *ring_buffer, void *packet);

// reads packet from the buffer, waits if buffer is empty
void* read_packet(ring_buffer_t *ring_buffer);

#endif  // RING_BUFFER_H
