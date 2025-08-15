CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 
LDFLAGS = -lm 

.PHONY: all 
.DEFAULT_GOAL:= all
all: playerinfo
playerinfo: player_info.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f playerinfo *.o