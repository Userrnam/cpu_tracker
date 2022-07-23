#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ring_buffer.h>
#include <reader.h>
#include <analyzer.h>


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

	// read packets
	for (int i = 0; i < 10; ++i) {
		int *packet = read_packet(&ring_buffer);
		assert(packet);
		assert(memcmp(packet, packets[i], 10 * sizeof(int)) == 0);
		free(packet);
	}

	destroy(&ring_buffer);
}

void reader_test() {
	const char *s = "cpu  7839 311 10013 6053105 1118 33862 55077 0 0 0\n"
		"cpu0 3527 31 6287 3073484 193 2110 689 0 0 0\n"
		"cpu1 4311 280 3726 2979621 924 31751 54388 0 0 0\n"
		"intr 15983349 35 17321 0 0 0 0 0 0 0 0 0 0 158 0 0 29082 0 0 156117 29673 267922\n";

	cpu_stat_array_t *got = parse_cpu_stats(s);
	cpu_stat_array_t *expected = alloc_cpu_stat_array(2);
	expected->elems[0].user = 3527;
	expected->elems[0].nice = 31;
	expected->elems[0].system = 6287;
	expected->elems[0].idle = 3073484;
	expected->elems[0].iowait = 193;
	expected->elems[0].irq = 2110;
	expected->elems[0].softirq = 689;
	expected->elems[0].steal = 0;
	expected->elems[0].guest = 0;
	expected->elems[0].guest_nices = 0;

	expected->elems[1].user = 4311;
	expected->elems[1].nice = 280;
	expected->elems[1].system = 3726;
	expected->elems[1].idle = 2979621;
	expected->elems[1].iowait = 924;
	expected->elems[1].irq = 31751;
	expected->elems[1].softirq = 54388;
	expected->elems[1].steal = 0;
	expected->elems[1].guest = 0;
	expected->elems[1].guest_nices = 0;

	assert(memcmp(got->elems, expected->elems, 2 * sizeof(cpu_stat_t)) == 0);

	free(got);
	free(expected);
}

void analyzer_test() {
	cpu_stat_array_t *cpu_stats = alloc_cpu_stat_array(2);
	cpu_stats->elems[0].user = 3527;
	cpu_stats->elems[0].nice = 31;
	cpu_stats->elems[0].system = 6287;
	cpu_stats->elems[0].idle = 3073484;
	cpu_stats->elems[0].iowait = 193;
	cpu_stats->elems[0].irq = 2110;
	cpu_stats->elems[0].softirq = 689;
	cpu_stats->elems[0].steal = 0;
	cpu_stats->elems[0].guest = 0;
	cpu_stats->elems[0].guest_nices = 0;

	cpu_stats->elems[1].user = 4311;
	cpu_stats->elems[1].nice = 280;
	cpu_stats->elems[1].system = 3726;
	cpu_stats->elems[1].idle = 2979621;
	cpu_stats->elems[1].iowait = 924;
	cpu_stats->elems[1].irq = 31751;
	cpu_stats->elems[1].softirq = 54388;
	cpu_stats->elems[1].steal = 0;
	cpu_stats->elems[1].guest = 0;
	cpu_stats->elems[1].guest_nices = 0;

	cpu_times_array_t *cpu_times = alloc_cpu_times_array(2);
	calc_cpu_times(cpu_stats, cpu_times);

	cpu_times_array_t *expected_cpu_times = alloc_cpu_times_array(2);
	expected_cpu_times->elems[0].idle = 3073677;
	expected_cpu_times->elems[0].non_idle = 12613;
	expected_cpu_times->elems[1].idle = 2980545;
	expected_cpu_times->elems[1].non_idle = 94176;

	assert(memcmp(cpu_times->elems, expected_cpu_times->elems, 2 * sizeof(cpu_times_t)) == 0);

	free(cpu_times);
	free(expected_cpu_times);
	free(cpu_stats);
}

int main() {
	srand(time(NULL));

	ring_buffer_test();
	reader_test();
	analyzer_test();

	puts("OK");

	return 0;
}

