#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"
#include <signal.h>
#include <errno.h>

#define PORT 23107
#define LOCALHOST "127.0.0.1"

static volatile int running = 1;
static void on_sigint(int sig) {
    (void)sig;
    printf("killed client. guess who's angry now\n");
    printf("I... am dying....\n");
    running = 0;

}

int main(int argc, char** argv){
    signal(SIGINT, on_sigint);
    if(argc != 2){
	fprintf(stderr, "Usage: /wolfclient serverIP\n");
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
    }
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
    tv.tv_sec = 2;
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
   int funnyCheck = 0;
   while(running == 1){
       if(myPlayer.pdata.y%2){
	   myPlayer.pdata.y = 3*myPlayer.pdata.y +1;
       } else{
	   myPlayer.pdata.y = myPlayer.pdata.y/2;
       }
       if(myPlayer.pdata.x%2){
	   myPlayer.pdata.x = 3*myPlayer.pdata.x +1;
       } else{
	   myPlayer.pdata.x = myPlayer.pdata.x/2;
       }
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
	   size_t n = count;
	   if (n > MAX_PLAYERS){
	       n = MAX_PLAYERS;
	   }
	   memcpy(others, payload + sizeof(count), n*sizeof(Public));

	   for(size_t i = 0; i<n; i++) {
	       printf(" [id=%u x=%d y=%d a=%.2f]", others[i].id, others[i].x, others[i].y, others[i].angle);
	   }
	   if(n != 0){

	       printf("\n");

	   }
	    if(n == 0 && !funnyCheck) {
	       printf("lonely ahh bum ahh client\n");
	       funnyCheck++;
	       }
	   } 

       usleep(200*1000);
    }
      send_message(clientFd, &servaddr, addrlen, CLIENT_DISCONNECT, myPlayer.pdata.id, &myPlayer, sizeof(Player));
      close(clientFd);
      return 0;
}
