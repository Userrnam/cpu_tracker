#include <pthread.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"

// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog(void*);

void *logger(void*);

void *sigterm_handler(int signum);

int main() {
	ring_buffer_t reader_analyzer_buffer;
	ring_buffer_t analyzer_printer_buffer;

	int is_running = 1;

	create(&reader_analyzer_buffer, 100);
	create(&analyzer_printer_buffer, 100);

	pthread_t reader_thread;
	pthread_t analyzer_thread;
	pthread_t printer_thread;

	reader_params_t reader_params;
	reader_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	reader_params.is_running = &is_running;
	pthread_create(&reader_thread, NULL, (void*(*)(void*))reader, &reader_params);

	analyzer_params_t analyzer_params;
	analyzer_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	analyzer_params.analyzer_printer_buffer = &analyzer_printer_buffer ;
	analyzer_params.is_running = &is_running;
	pthread_create(&analyzer_thread, NULL, (void*(*)(void*))analyzer, &analyzer_params);

	printer_params_t printer_params;
	printer_params.analyzer_printer_buffer = &analyzer_printer_buffer;
	printer_params.is_running = &is_running;
	pthread_create(&printer_thread, NULL, (void*(*)(void*))printer, &printer_params);

	pthread_join(reader_thread,   NULL);
	pthread_join(analyzer_thread, NULL);
	pthread_join(printer_thread,  NULL);

	destroy(&reader_analyzer_buffer);
	destroy(&analyzer_printer_buffer);

	return 0;
}
