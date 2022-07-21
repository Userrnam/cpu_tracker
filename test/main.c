#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ring_buffer.h>


void ring_buffer_test() {
	ring_buffer_t ring_buffer;

	create(&ring_buffer, 100);

	// create random packets
	int *packets[10];
	for (int i = 0; i < 10; ++i) {
		packets[i] = malloc(10 * sizeof(int));
		for (int j = 0; j < 10; ++j) {
			packets[i][j] = rand();
		}
	}

	// write packets
	for (int i = 0; i < 10; ++i) {
		write_packet(&ring_buffer, packets[i]);
	}

	// read pacets
	for (int i = 0; i < 10; ++i) {
		int *packet = read_packet(&ring_buffer);
		assert(memcmp(packet, packets[i], 10 * sizeof(int)) == 0);
		free(packet);
	}

	destroy(&ring_buffer);
}


int main() {
	srand(time(NULL));

	ring_buffer_test();

	puts("OK");

	return 0;
}

