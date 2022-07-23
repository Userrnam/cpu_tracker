#include "logger.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "ring_buffer.h"

typedef struct message {
	int type;
	char *msg;
} message_t;

static ring_buffer_t messages;
static int types_bitmask;
static int initialized;

// writes message to file
void write_message(FILE *fp, int type, const char *msg) {
	char time_buffer[64];

	time_t raw_time = time(NULL);
	struct tm *time_info = localtime(&raw_time);

	strftime(time_buffer, 64, "%Y-%m-%d %H:%M:%S", time_info);

	const char *type_name = "WRONG_LOG_LEVEL";
	switch (type) {
	case INFO:    type_name = "INFO"; break;
	case WARNING: type_name = "WARNING"; break;
	case ERROR:   type_name = "ERROR"; break;
	}

	fprintf(fp, "%s [%s]: %s\n", time_buffer, type_name, msg);
}

void *logger(logger_params_t *params) {
	if (params->types_bitmask < 0 || params->types_bitmask > 0b111) {
		fprintf(stderr, "logger: wrong bitmask\n");
		sem_post(&params->started);
		return NULL;
	}
	types_bitmask = params->types_bitmask;
	if (params->ring_buffer_size < 1 || params->ring_buffer_size >= 400) {
		fprintf(stderr, "logger: ring_buffer_size must be at least 1 and less than 400\n");
		sem_post(&params->started);
		return NULL;
	}
	create(&messages, params->ring_buffer_size);

	initialized = 1;

	sem_post(&params->started);
	FILE *fp = fopen(params->file_name, "a");
	if (!fp) {
		fprintf(stderr, "logger: failed to open file \"%s\"\n", params->file_name);
		return NULL;
	}

	write_message(fp, INFO, "logger started");

	while (1) {
		message_t *msg = (message_t *)read_packet(&messages);
		if (!msg) {
			// logger must process all messages, so we do this check here
			if (!*params->is_running) {
				break;
			}
			continue;
		}

		write_message(fp, msg->type, msg->msg);

		free(msg->msg);
		free(msg);
	}

	write_message(fp, INFO, "logger stopped");
	fprintf(fp, "\n");

	fclose(fp);

	destroy(&messages);

	return NULL;
}

void log_message(int type, const char *s) {
	if (!initialized || (type & types_bitmask) == 0) {
		return;
	}
	message_t *msg = (message_t *)malloc(sizeof(message_t));
	msg->type = type;
	msg->msg = strdup(s);
	write_packet(&messages, msg);
}

