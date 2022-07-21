#ifndef READER_H
#define READER_H

#include "ring_buffer.h"

typedef struct reader_params {
    ring_buffer_t *reader_analyzer_buffer;
    int *is_running;
} reader_params_t;

void *reader_thread(reader_params_t *params);

#endif // READER_H
