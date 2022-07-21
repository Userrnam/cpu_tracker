#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

// C version of a template. Since arrays in this application are used as packets you
// can only use it by pointer, it was done this way to decrease number of allocations.
// This macro is used for complex types (end with _t), the parameter to this macro is
// name of complex type without _t at the end.
#define ARRAY(type) \
typedef struct type##_array { \
	int count; \
	type##_t elems[]; \
} type##_array_t; \
static inline type##_array_t *alloc_##type##_array(int count) { \
	type##_array_t *res = malloc(sizeof(int) + count * sizeof(cpu_stat_t)); \
	res->count = count; \
	return res; \
}

// same as above, but for basic types (no _t at the end of type).
#define ARRAY_BASIC_TYPE(type) \
typedef struct type##_array { \
	int count; \
	type elems[]; \
} type##_array_t; \
static inline type##_array_t *alloc_##type##_array(int count) { \
	type##_array_t *res = malloc(sizeof(int) + count * sizeof(cpu_stat_t)); \
	res->count = count; \
	return res; \
}

#endif

