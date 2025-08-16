#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define PORT 8080

int main(){
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
    servaddr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addrlen = sizeof(servaddr);
    Player myPlayer = {0};
    Public myPdata = {.id = -1, .x = 10, .y = 20, .angle = 0};

    myPlayer.pdata = myPdata;
    myPlayer.health = 100;
    myPlayer.speed = 1.0;
    send_message(clientFd, &servaddr, addrlen, CLIENT_UPDATE, sizeof(Player), (uint8_t*)&myPlayer, 0);

    Message msg;
    if(recieve_message(clientFd, &msg, &servaddr, &addrlen) == 0){
	if (msg.operation == NEW_PLAYER) {
	    printf("Got map data, here it is: %s", msg.data);
	    myPlayer.pdata.id = msg.id;
	    fflush(stdout);

	}
	free(msg.data);
    }

    for(int i = 0; i<5; i++) {
	myPlayer.pdata.x += 1;
	send_message(clientFd, &servaddr, addrlen,
                     CLIENT_UPDATE, sizeof(Player),
                     (uint8_t*)&myPlayer, myPlayer.pdata.id);
	if (recieve_message(clientFd, &msg, &servaddr, &addrlen) == 0) {
	    Public *players = (Public*)msg.data;
	    int count = msg.dataSize/sizeof(Public);
	    printf("Update: %d players online\n", count);
	    for(int i = 0; i<count; i++){
		printf("Player %d at (%d, %d)\n", players[i].id, players[i].x, players[i].y);
	    }
	}
	free(msg.data);
	sleep(1);
    }

    send_message(clientFd, &servaddr, addrlen,
                 DISCONNECT, sizeof(Player),
                 (uint8_t*)&myPlayer, myPlayer.pdata.id);
    close(clientFd);
    return 0;



}
