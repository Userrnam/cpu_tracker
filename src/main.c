#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"


// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog(void*);

void *logger(void*);

static volatile int is_running;

static void signal_handler(int signum) {
	// in our case this is useless, but otherwise you get a warning
	if (signum == SIGINT) {
		is_running = 0;
	}
}

int main() {
	is_running = 1;

	// setup signale handler
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = signal_handler;
    sigaction(SIGINT, &action, NULL);

	// create ring buffers
	ring_buffer_t reader_analyzer_buffer;
	ring_buffer_t analyzer_printer_buffer;

	create(&reader_analyzer_buffer, 100);
	create(&analyzer_printer_buffer, 100);

	// create reader thread
	reader_params_t reader_params;
	reader_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	reader_params.is_running = &is_running;
	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL, (void*(*)(void*))reader, &reader_params);

	// create analyzer thread
	analyzer_params_t analyzer_params;
	analyzer_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	analyzer_params.analyzer_printer_buffer = &analyzer_printer_buffer ;
	analyzer_params.is_running = &is_running;
	pthread_t analyzer_thread;
	pthread_create(&analyzer_thread, NULL, (void*(*)(void*))analyzer, &analyzer_params);

	// create printer thread
	printer_params_t printer_params;
	printer_params.analyzer_printer_buffer = &analyzer_printer_buffer;
	printer_params.is_running = &is_running;
	pthread_t printer_thread;
	pthread_create(&printer_thread, NULL, (void*(*)(void*))printer, &printer_params);

	// wait for the threads
	pthread_join(reader_thread,   NULL);
	pthread_join(analyzer_thread, NULL);
	pthread_join(printer_thread,  NULL);

	destroy(&reader_analyzer_buffer);
	destroy(&analyzer_printer_buffer);

	return 0;
}
