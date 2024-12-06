CC=gcc
PROGS=scheduler
CFLAGS=-std=c99 -Wall -g
LFLAGS=-lm

# Object files needed
OBJS=scheduler.o my_queue.o

all: $(PROGS)

scheduler.o: scheduler.c my_queue.h
	$(CC) $(CFLAGS) -c scheduler.c
	
my_queue.o: my_queue.c my_queue.h
	$(CC) $(CFLAGS) -c my_queue.c

scheduler: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LFLAGS)
	
clean:
	rm -f *.o $(PROGS)
