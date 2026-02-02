.PHONY: all clean test
CC = gcc
CFLAGS = -O0 -Wall -Wextra -g -DDEBUG_A
INCLUDES = -Iaux/logger/
LIBS = -lncurses

all: editor

editor: main.c
	$(CC) $(CFLAGS) $(INCLUDES) main.c -o bin/editor $(LIBS)

test: editor
	./bin/editor aux/test_files/test_file1.txt

clean:
	rm -rf bin/* log/test/*

