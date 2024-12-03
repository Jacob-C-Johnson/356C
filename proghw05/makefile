CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = scheduler

all: $(TARGET)

$(TARGET): scheduler.o my_queue.o
	$(CC) $(CFLAGS) -o $(TARGET) scheduler.o my_queue.o

scheduler.o: scheduler.c my_queue.h
	$(CC) $(CFLAGS) -c scheduler.c

my_queue.o: my_queue.c my_queue.h
	$(CC) $(CFLAGS) -c my_queue.c

clean:
	rm -f *.o $(TARGET)
