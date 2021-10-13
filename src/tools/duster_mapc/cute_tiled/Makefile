BINARY=libcute_tiled.a

CC=gcc
AR=ar
CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=

SOURCES=$(wildcard *.c )
OBJECTS=$(SOURCES:.c=.o)

all: $(BINARY)

# $(BINARY): $(OBJECTS)
# 	$(CC) $(CFLAGS) $(OBJECTS) -o $(BINARY)

$(BINARY): $(OBJECTS)
	$(AR) -r -o $(BINARY) $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(BINARY) $(OBJECTS)