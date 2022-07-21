CC = gcc
CFLAGS = -g -Wall -Wextra

TARGET_EXEC := cpu_tracker
BUILD_DIR   := ./build

# Find all the C and C++ files we want to compile
SRCS := $(shell find . -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

