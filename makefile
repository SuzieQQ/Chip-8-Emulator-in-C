CC=gcc
CFLAGS=-I -std=c18 -W -g -c -Wextra
SOURCES = main.c chip8.c
LDFLAGS=  -lSDL2
OBJECTS=$(SOURCES:.c=.o)
TARGET=chip8

$(TARGET) : $(OBJECTS) $(LDFLAGS)
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) $(LDFLAGS)
