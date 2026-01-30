.PHONY: all clean test
CC = gcc
CFLAGS = -O0 -Wall -Wextra -g -DDEBUG
INCLUDES = -Iaux/logger/
LIBS = -lncurses

all: editor

editor: main.c
	$(CC) $(CFLAGS) $(INCLUDES) main.c -o bin/editor $(LIBS)

test: editor
	./bin/editor

clean:
	rm -rf bin/*

