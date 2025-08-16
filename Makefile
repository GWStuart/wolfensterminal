CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 
LDFLAGS = -lm -lncurses

.PHONY: all 
.DEFAULT_GOAL:= all

all: main

main: player_info.o render.o main.o map_editor.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f playerinfo *.o
