#include "input.h"




//CODE MADE BY AI, BUT MODIFIED HEAVILY

InputDeviceStuff open_devices() {
    InputDeviceStuff iDS;
    iDS.epollfd = epoll_create1(0);
    if (iDS.epollfd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    printf("Searching for all keyboard and mouse devices...\n");

    // Iterate through a range of event files to find all devices
    for (iDS.i = 0; iDS.i < 32; (iDS.i)++) {
        snprintf(iDS.device_path, sizeof(iDS.device_path), "%s%d", EVENT_DEVICE_PATH, iDS.i);
        iDS.fd = open(iDS.device_path, O_RDONLY);
        if (iDS.fd >= 0) {
            unsigned long ev_bits[1];
            unsigned long key_bits[2];
            unsigned long rel_bits[1];
            char name[256];
            
            ioctl(iDS.fd, EVIOCGNAME(sizeof(name)), name);
            ioctl(iDS.fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits);

            // Check if it's a keyboard
            if (ev_bits[0] & (1 << EV_KEY)) {
                ioctl(iDS.fd, EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits);
                // Check for a broad range of keys, not just a few power buttons
                if (key_bits[0] > 0 && key_bits[1] > 0) {
                    printf("Keyboard found and added to epoll: %s (%s)\n", iDS.device_path, name);
                    iDS.ev.events = EPOLLIN;
                    iDS.ev.data.fd = iDS.fd;
                    if (epoll_ctl(iDS.epollfd, EPOLL_CTL_ADD, iDS.fd, &(iDS.ev)) == -1) {
                        perror("epoll_ctl: keyboard");
                        close(iDS.fd);
                    } else {
                        (iDS.total_devices_found)++;
                    }
                    continue;
                }
            }

            // Check if it's a mouse
            if (ev_bits[0] & (1 << EV_REL)) {
                printf("Mouse found and added to epoll: %s (%s)\n", iDS.device_path, name);
                iDS.ev.events = EPOLLIN;
                iDS.ev.data.fd = iDS.fd;
                if (epoll_ctl(iDS.epollfd, EPOLL_CTL_ADD, iDS.fd, &(iDS.ev)) == -1) {
                    perror("epoll_ctl: mouse");
                    close(iDS.fd);
                } else {
                    (iDS.total_devices_found)++;
                }
                continue;
            }
            
            close(iDS.fd);
        }
    }
    
    if (iDS.total_devices_found == 0) {
        fprintf(stderr, "Could not find any suitable input devices. Make sure you have permissions.\n");
        close(iDS.epollfd);
        exit(1);
    }
    return iDS;
}


void detect_input(InputDeviceStuff* iDS, Inputs* inputs)
{
    //printf("Monitoring all devices simultaneously. Press Ctrl+C to exit.\n");
    
    iDS->nfds = epoll_wait(iDS->epollfd, iDS->events, MAX_EVENTS, -1);
    if (iDS->nfds == -1) {
	if (errno == EINTR) return; // Handle interrupted syscall
	perror("epoll_wait");
	close(iDS->epollfd);
	exit(1);
    }

    for (iDS->i = 0; iDS->i < iDS->nfds; ++(iDS->i)) {
	struct input_event input_ev;
	if (read(iDS->events[iDS->i].data.fd, &input_ev, sizeof(struct input_event)) == -1) {
	    perror("read");
	    continue;
	}

	switch (input_ev.type) {
	    case EV_KEY:
		if (input_ev.value == 1) {
		    //printf("Key/Button pressed: code %d\n", input_ev.code);
		    switch (input_ev.code) {
			case K_W:
			    inputs->forward = true;
			    break;
			case K_S:
			    inputs->forward = true;
			    break;
			case K_A:
			    inputs->forward = true;
			    break;
			case K_D:
			    inputs->right = true;
			    break;
			default: 
			    break;
		    }
		} else if (input_ev.value == 0) {
		    //printf("Key/Button released: code %d\n", input_ev.code);
		    switch (input_ev.code) {
			case K_W:
			    inputs->forward = false;
			    break;
			case K_S:
			    inputs->forward = false;
			    break;
			case K_A:
			    inputs->forward = false;
			    break;
			case K_D:
			    inputs->right = false;
			    break;
			default: 
			    break;
		    }
		}
		break;
	    case EV_REL:
		if (input_ev.code == REL_X) {
		    //printf("Mouse moved horizontally by %d\n", input_ev.value);
		    inputs->mouseX = input_ev.value;
		} else if (input_ev.code == REL_Y) {
		    //printf("Mouse moved vertically by %d\n", input_ev.value);
		} else if (input_ev.code == REL_WHEEL) {
		    //printf("Mouse wheel scrolled by %d\n", input_ev.value);
		}
		break;
	}
    }

}

void close_input(InputDeviceStuff* iDS) {
    close(iDS->epollfd);
}
