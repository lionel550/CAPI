CC = gcc
CFLAGS = -I include -Wall -Wextra -Wpedantic -g
TARGET = capi
BUILD_DIR = build
C_SRCS := $(wildcard src/*.c)

$(BUILD_DIR)/$(TARGET): $(C_SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(C_SRCS)

clean:
	rm -r $(BUILD_DIR)
