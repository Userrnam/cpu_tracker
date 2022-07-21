#include "ring_buffer.h"

// reads raw data from /proc/stat and sends it to analyzer via curcular buffer
void *reader_thread(void*);

// receives data from reader, calculates CPU usage for each CPU core and sends it to printer
void *analyzer_thread(void*);

// receives data from analyzer and prints it to the console
void *printer_thread(void*);

// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog_thread(void*);

void *logger_thread(void*);

void *sigterm_handler(int signum);

int main() {

}
