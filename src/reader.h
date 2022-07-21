#ifndef READER_H
#define READER_H

#include "ring_buffer.h"

void *reader_thread(ring_buffer_t *reader_analyzer_buffer);

#endif // READER_H
