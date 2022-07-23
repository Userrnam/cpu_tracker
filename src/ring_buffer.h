#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <semaphore.h>
#include <pthread.h>

// used to send data packet from one thread to another. Packet is a void *, so it can point
// to any data structure, when reading the packet the user must cast it to appropriate type.
typedef struct ring_buffer {
	void **packets;
	int size;        // size of the buffer
	int next_read;   // next packet is read at this index
	int next_write;  // next packet is written at this index

	pthread_mutex_t write_mtx;
	pthread_mutex_t read_mtx;
	sem_t packet_count;        // number of packets in the buffer
	sem_t free_position_count; // number of free positions in the buffer (size - packet_count)
} ring_buffer_t;

// returns 0 on success, -1 on error
int create(ring_buffer_t *ring_buffer, int size);
void destroy(ring_buffer_t *ring_buffer);

// writes packet to the buffer, blocks if current_size > buffer_size
void  write_packet(ring_buffer_t *ring_buffer, void *packet);

// reads packet from the buffer, waits if buffer is empty
void* read_packet(ring_buffer_t *ring_buffer);

#endif  // RING_BUFFER_H
