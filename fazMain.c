#include "3dEngine.h"
#include "render.h"
#include "input.h"
#include "player_info.h"
#include "sprite.h"
#include "debug/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "serverClient/protocol.h"
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#define TO_RAD(deg) (deg * (M_PI / 180.0f))
#define TO_DEG(rad) (rad * (180.0f / M_PI))

#define PORT 23107

static volatile int running = 1;
static void on_sigint(int sig) {
    (void)sig;
    printf("killed client. guess who's angry now\n");
    printf("I... am dying....\n");
    running = 0;

}


    int map[20][20] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1}, 
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, 
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, 
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1}, 
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

int main(int argc, char** argv)
{
    size_t n = 0;
    size_t nOld = 0;
    signal(SIGINT, on_sigint);
    if(argc != 2) {
	fprintf(stderr, "Usage: sudo ./wolfensterminal serverIP\n");
	exit(-1);


    }
    char* hostIP = argv[1];
    int clientFd;
    struct sockaddr_in servaddr;
    clientFd = socket(AF_INET, SOCK_DGRAM, 0);
    // Create UDP socket
    if(clientFd == -1) {
	perror("bumass client failed");
	return -1;
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, hostIP, &servaddr.sin_addr) <= 0){
	perror("the ip was not in the correct format.");
	return -1;
	
    };
    socklen_t addrlen = sizeof(servaddr);
    Player myPlayer = {0};
    Public myPdata = {.id = -1, .x = 10, .y = 20, .angle = 0};

    myPlayer.pdata = myPdata;
    myPlayer.health = 100;
    myPlayer.speed = 1.0f;
    send_message(clientFd, &servaddr, addrlen, CLIENT_HELLO, -1, (uint8_t*)&myPlayer,sizeof(Player));

    MessageHeader header;
    uint8_t payload[65507];
    struct timeval tv; 
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));


    int dataSize = recv_message(clientFd, &header, payload, sizeof(payload), &servaddr, &addrlen);

    if(dataSize <= 0 || header.operation != SERVER_WELCOME) {
	fprintf(stderr, "this is not the correct server: go away\n");
	return 1;
    }
    WelcomePayload message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, payload, dataSize);
    myPlayer.pdata.id = message.assigned_id;
    printf("listen buddy, server gonna give you only one thing: %u\n", myPlayer.pdata.id);
    printf("oh yeah this is where you live: \n%.*s",(int)message.map_size, (const char*)message.map_bytes);
   Public others[MAX_PLAYERS];


    debug_init();
    int numSprites = 3;
    Sprite* sprites = malloc(sizeof(Sprite) * numSprites);
    Sprite* players;
    Sprite* allSprites;
    sprites[0].x = 5*64;
    sprites[0].y = 3*64;
    sprites[0].isAngled = false;
    sprites[0].isExist = true;
    sprites[0].spriteType = S_GUY;
    sprites[0].distanceToPlayer = 50000;
    sprites[1].x = 2*64;
    sprites[1].y = 2*64;
    sprites[1].isAngled = false;
    sprites[1].isExist = true;
    sprites[1].spriteType = S_GUY;
    sprites[1].distanceToPlayer = 50000;
    sprites[2].x = 3*64;
    sprites[2].y = 5*64;
    sprites[2].isAngled = false;
    sprites[2].isExist = true;
    sprites[2].spriteType = S_SGUN;
    sprites[2].distanceToPlayer = 50000;
    InputDeviceStuff iDS = open_devices();
    Inputs inputs = {.forward = false,
	.back = false,
	.left = false,
	.right = false,
	.tRight = false,
	.tLeft = false,
	.mouseX = 0};
    setup_screen();
    clear_screen();
    start_color();

    Player_info player = {.x = 5*64, .y = 5*64, .angle = -90, .curr_speed = 0, .max_speed = 5, .hasPistol = true, .hasShotgun = false, .equipped = 1};
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    while (running) {
	getmaxyx(stdscr, rows, cols);

	clear_screen();

	float FOV = 70.0f; // degrees
	float FOV_RAD = FOV * (M_PI / 180.0f);

	sprites[0].x += 5;

	for (int i = 0; i < numSprites; i++) {
	    sprites[i].distanceToPlayer = dist(player.x, player.y, sprites[i].x, sprites[i].y, 0);
	}

	allSprites = realloc(allSprites, sizeof(Sprite) * (numSprites + n));

	for (int i = 0; i < numSprites; i++) {
	    allSprites[i] = sprites[i];
	}
	for (size_t i = (size_t)numSprites; i < (size_t)numSprites + n; i++) {
	    allSprites[i] = players[i - (size_t)numSprites];
	}

	draw_all_stuff(map, &player, cols, rows, &allSprites, numSprites + (int)n);


    refresh();
    detect_input(&iDS, &inputs);
    player.angle += inputs.mouseX; //UNCOMMENT IF MOUSE IS WORKING AS INTENDED
    if (player.angle < -180) {
	player.angle += 360;
    } else if (player.angle > 180) {
	player.angle -= 360;
    }
    inputs.mouseX = 0;
    
    if (inputs.forward) {
	if (map[((player.y + (int)(20*sin(TO_RAD(player.angle)))) >> 6)][((player.x + (int)(20*cos(TO_RAD(player.angle)))) >> 6)] == 0) {
	    player.x += 5*cos(TO_RAD(player.angle));
	    player.y += 5*sin(TO_RAD(player.angle));
	}
    }
    if (inputs.back) {
	if (map[((player.y - (int)(20*sin(TO_RAD(player.angle)))) >> 6)][((player.x - (int)(20*cos(TO_RAD(player.angle)))) >> 6)] == 0) {
	    player.x -= 5*cos(TO_RAD(player.angle));
	    player.y -= 5*sin(TO_RAD(player.angle));
	}
    }
    if (inputs.left) {
	if (map[((player.y - (int)(20*cos(TO_RAD(player.angle)))) >> 6)][((player.x + (int)(20*sin(TO_RAD(player.angle)))) >> 6)] == 0) {
	    player.x += 5*sin(TO_RAD(player.angle));
	    player.y -= 5*cos(TO_RAD(player.angle));
	}
    }
    if (inputs.right) {
	if (map[((player.y + (int)(20*cos(TO_RAD(player.angle)))) >> 6)][((player.x - (int)(20*sin(TO_RAD(player.angle)))) >> 6)] == 0) {
	    player.x -= 5*sin(TO_RAD(player.angle));
	    player.y += 5*cos(TO_RAD(player.angle));
	}
    }
    
    

    if (inputs.tLeft) {
	player.angle--;
	if (player.angle < -180) {
	    player.angle = 180;
	}
    }
    if (inputs.tRight) {
	player.angle++;
	if (player.angle > 180) {
	    player.angle = -180;
	}
    }

    if (inputs.pistol && player.hasPistol) {
	player.equipped = 1;
    }

    if (inputs.shotgun && player.hasShotgun) {
	player.equipped = 2;
    }

    for (int i = 0; i < numSprites; i++) {
	if (sprites[i].isExist && sprites[i].spriteType == S_SGUN) {
	    if (sprites[i].distanceToPlayer < 20) {
		sprites[i].isExist = false;
		player.hasShotgun = true;
	    }
	}
    }


    memcpy(&myPlayer.pdata.x, &player.x, sizeof(int));
    memcpy(&myPlayer.pdata.y, &player.y, sizeof(int));
    memcpy(&myPlayer.pdata.angle, &player.angle, sizeof(int));

    send_message(clientFd, &servaddr, addrlen, CLIENT_UPDATE, myPlayer.pdata.id, &myPlayer, sizeof(Player));

    MessageHeader snapshot;
    struct sockaddr_in from;
    socklen_t flen = sizeof(from);
    int r = recv_message(clientFd, &snapshot, payload, sizeof(payload), &from, &flen);
    if(r < 0){
	if(errno == EWOULDBLOCK || errno == EAGAIN){
	    fprintf(stderr, "server thinks you're a bum. either:\n"
		"1. the ip you just inputted was bad\n"
		"2. server closed\n");
	    running = 0;
       }
   }
 
    if(r > 0 && snapshot.operation == SERVER_SNAPSHOT){
        uint16_t count = 0;
        memcpy(&count, payload, sizeof(count));
        n = count;
        if (n > MAX_PLAYERS){
            n = MAX_PLAYERS;
        }
	if (n > nOld) {
	    players = realloc(players, sizeof(Sprite) * n);
	}
	nOld = n;
        memcpy(others, payload + sizeof(count), n*sizeof(Public));

	for (size_t i = 0; i < n; i++) {
	    players[i].x = others[i].x;
	    players[i].y = others[i].y;
	    players[i].isExist = true;
	    players[i].spriteType = S_GUY;
	}

        }

    

    refresh();
    napms(16);
    }
    send_message(clientFd, &servaddr, addrlen, CLIENT_DISCONNECT, myPlayer.pdata.id, &myPlayer, sizeof(Player));
    close(clientFd);
    return 0;
}

