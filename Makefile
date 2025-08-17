CC = gcc
CFLAGS = -std=gnu99 
LDFLAGS = -lm -lncurses

.PHONY: all 
.DEFAULT_GOAL:= all

all: wolfensterminal

wolfensterminal: player_info.c render.c fazMain.c input.c 3dEngine.c debug/debug.c serverClient/protocol.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f main fazMain *.o
