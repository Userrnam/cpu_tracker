CC = clang
CFLAGS = -g -Weverything -std=c99 -I src
LDFLAGS = -lpthread

TARGET_EXEC := cpu_tracker
TEST_EXEC   := tests
BUILD_DIR   := ./build

SRCS := $(shell find src/ -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
HEADERS := $(shell find src/ -name '*.c')

SRCS_NO_MAIN := $(foreach S,$(SRCS),$(if $(findstring main.c,$(S)),,$(S)))
OBJS_NO_MAIN := $(SRCS_NO_MAIN:%=$(BUILD_DIR)/%.o)

TEST_SRCS := $(shell find test/ -name '*.c')
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

test: $(BUILD_DIR)/$(TEST_EXEC)

$(BUILD_DIR)/$(TEST_EXEC): $(TEST_OBJS) $(OBJS_NO_MAIN)
	$(CC) $(TEST_OBJS) $(OBJS_NO_MAIN) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

