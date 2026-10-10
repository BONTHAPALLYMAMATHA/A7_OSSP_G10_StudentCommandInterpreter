CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = -pthread

SRC = src/main.c \
      src/input.c \
      src/parser.c \
      src/process.c \
      src/builtin.c \
      src/signals.c \
      src/pipes.c \
      src/redirect.c \
      src/thread.c \
      src/jobs.c \
      src/job_control.c

TARGET = bin/shellforge
DEADLOCK = bin/deadlock

.PHONY: all run clean asan

all: $(TARGET) $(DEADLOCK)

$(TARGET): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

$(DEADLOCK): src/deadlock.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/deadlock.c $(LDFLAGS) -o $(DEADLOCK)

asan:
	mkdir -p bin
	$(CC) $(CFLAGS) -fsanitize=address $(SRC) $(LDFLAGS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f bin/shellforge bin/deadlock
