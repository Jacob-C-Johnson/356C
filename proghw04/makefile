CC=gcc
CFLAGS=-pthread
TARGET=war_networked

all: $(TARGET)

$(TARGET): war_networked.o
	$(CC) $(CFLAGS) -o $(TARGET) war_networked.o

war_networked.o: war_networked.c
	$(CC) $(CFLAGS) -c war_networked.c

clean:
	rm -f *.o $(TARGET)