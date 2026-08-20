CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = src/main.c
TARGET = bin/student_shell

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
