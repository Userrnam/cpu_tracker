#ifndef CPU_STAT_H
#define CPU_STAT_H

#include "array.h"

typedef struct cpu_stat {
	int user;
	int nice;
	int system;
	int idle;
	int iowait;
	int irq;
	int softirq;
	int steal;
	int guest;
	int guest_nices;
} cpu_stat_t;

ARRAY(cpu_stat)


#endif // CPU_STAT_H
