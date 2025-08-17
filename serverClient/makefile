CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -Wextra -g
DEBUG = -g
LDFLAGS = -pthread




all: wolfserver wolfclient


.DEFAULT_GOAL:= all

wolfserver: wolfserver.o protocol.o

wolfclient: wolfclient.o protocol.o


