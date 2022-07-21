#include "reader.h"
#include "analyzer.h"
#include "printer.h"

// receives some message from all threads, if at least 1 thread hasn't sent a message in more
// than 2s stops the application
void *watchdog(void*);

void *logger(void*);

void *sigterm_handler(int signum);

int main() {

	return 0;
}
