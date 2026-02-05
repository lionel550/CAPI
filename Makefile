CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
TARGET = flask-c
BUILD_DIR = out

$(BUILD_DIR)/$(TARGET): main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ main.c $(LIBS)

clean:
	rm -r $(BUILD_DIR)
