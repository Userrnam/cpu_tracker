

// used to send data from one thread to another. Works with packets
typedef struct circular_buffer {
	void **packets;
	int start;		  // current position of the first packet in circular buffer.
	int current_size; // how many packets buffer currently stores
	int buffer_size;  // how many packets buffer can store
	// FIXME: need some sync primitives
} circular_buffer_t;

// writes packet to circular buffer, if current_size > buffer_size overwrites the first packet
// in buffer and moves start.
void  write_packet(circular_buffer_t *circular_buffer, void *packet);

// reads packet from circular buffer, returns NULL if buffer has no packets
void* read_packet(circular_buffer_t *circular_buffer);


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
