CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 
LDFLAGS = -lm -lncurses

.PHONY: all 
.DEFAULT_GOAL:= all

all: wolfensterminal

wolfensterminal: player_info.c render.c fazMain.c input.c 3dEngine.c debug/debug.c serverClient/protocol.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#wolfensterminal: player_info.o render.o fazMain.o input.o 3dEngine.o 
#	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

#clean:
#	rm -f main fazMain *.o
