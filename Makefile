CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LIBS = -lncurses

all: render

render: render.c
	$(CC) $(CFLAGS) render.c -o render $(LIBS)

clean:
	rm -f main

