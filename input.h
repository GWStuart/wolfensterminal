#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <linux/input.h>

#define EVENT_DEVICE_PATH "/dev/input/event"
#define MAX_EVENTS 64

#define K_W 17
#define K_S 31
#define K_A 32
#define K_D 30
#define K_Q 16
#define K_E 18

typedef struct {
    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd;
    int nfds;
    int fd;
    char device_path[256];
    int i;
    int total_devices_found;
} InputDeviceStuff;

typedef struct {
    bool forward;
    bool back;
    bool left;
    bool right;
    bool tRight;
    bool tLeft;
    int mouseX;
} Inputs;


InputDeviceStuff open_devices();

void detect_input(InputDeviceStuff* iDS, Inputs* inputs);

void close_input(InputDeviceStuff* iDS);


#endif
