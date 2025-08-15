CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lncurses

OBJS = render.o

all: main

main: main.o render.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

render.o: render.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f render.o main.o main
