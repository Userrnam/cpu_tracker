#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "logger.h"
#include "watchdog.h"

static volatile int is_running;

static void signal_handler(int signum) {
	// in our case this is useless, but otherwise you get a warning
	if (signum == SIGINT) {
		log_message(INFO, "got SIGINT, stopping application");
		is_running = 0;
	}
}

int main() {
	const char *thread_names[] = { "reader", "analyzer", "printer" };

	is_running = 1;

	// make sure the logger turns off last
	int logger_is_running = 1;

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

	// create logger thread
	logger_params_t logger_params;
	logger_params.file_name = "log.txt";
	logger_params.ring_buffer_size = 100;
	logger_params.types_bitmask = INFO | WARNING | ERROR;
	logger_params.is_running = &logger_is_running;
	if (sem_init(&logger_params.started, 0, 0) != 0) {
		perror("sem_init");
	}
	pthread_t logger_thread;
	pthread_create(&logger_thread, NULL, (void*(*)(void*))logger, &logger_params);
	sem_wait(&logger_params.started);

	// create watchdog thread
	watchdog_params_t watchdog_params;
	watchdog_params.names = thread_names;
	watchdog_params.thread_count = 3;
	watchdog_params.is_running = &is_running;
	if (sem_init(&watchdog_params.started, 0, 0) != 0) {
		perror("sem_init");
	}
	pthread_t watchdog_thread;
	pthread_create(&watchdog_thread, NULL, (void*(*)(void*))watchdog, &watchdog_params);
	sem_wait(&watchdog_params.started);

	// create reader thread
	reader_params_t reader_params;
	reader_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	reader_params.thread_id = 0;
	reader_params.is_running = &is_running;
	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL, (void*(*)(void*))reader, &reader_params);

	// create analyzer thread
	analyzer_params_t analyzer_params;
	analyzer_params.thread_id = 1;
	analyzer_params.reader_analyzer_buffer = &reader_analyzer_buffer;
	analyzer_params.analyzer_printer_buffer = &analyzer_printer_buffer ;
	analyzer_params.is_running = &is_running;
	pthread_t analyzer_thread;
	pthread_create(&analyzer_thread, NULL, (void*(*)(void*))analyzer, &analyzer_params);

	// create printer thread
	printer_params_t printer_params;
	printer_params.thread_id = 2;
	printer_params.analyzer_printer_buffer = &analyzer_printer_buffer;
	printer_params.is_running = &is_running;
	pthread_t printer_thread;
	pthread_create(&printer_thread, NULL, (void*(*)(void*))printer, &printer_params);

	// wait for the threads
	pthread_join(reader_thread,   NULL);
	pthread_join(analyzer_thread, NULL);
	pthread_join(printer_thread,  NULL);
	pthread_join(watchdog_thread, NULL);

	// stop logger
	logger_is_running = 0;

	pthread_join(logger_thread,   NULL);

	destroy(&reader_analyzer_buffer);
	destroy(&analyzer_printer_buffer);

	return 0;
}
