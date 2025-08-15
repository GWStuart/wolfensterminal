#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PLAYERS 5

typedef struct { 
    int id;
    int x, y;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    int maxPlayers;
} gameState;


int listen_on_port(char* port){
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    int err;
    if((err = getaddrinfo(NULL, port, &hints, &ai))){
	freeaddrinfo(ai);
	perror("port was messed up or something retry if unstable connection");
	return -1;
    }

    int server = socket(AF_INET, SOCK_DGRAM, 0);
    if (server < 0) {
	freeaddrinfo(ai);
	perror("the socket was messed up, try again with a new port or smthin");
	return -1;
    }

    int optVal = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int)) < 0) {
	freeaddrinfo(ai);
	perror("IP is weird asf normally you shouldn't see ts");
	return -1;
    }
    if(bind(server, ai->ai_addr, sizeof(struct sockaddr)) < 0) {
	freeaddrinfo(ai);
	perror("binding messed up, you've gotten this far and it failed, kinda like real life");
	return -1;
    }
    struct sockaddr_in portInfo;
    socklen_t len = sizeof(portInfo);
    getsockname(server, (struct sockaddr*)&portInfo, &len);
    int actualPort = ntohs(portInfo.sin_port);
    fprintf(stderr, "%d\n", actualPort);
    fflush(stderr);
    freeaddrinfo(ai);
    return server;





}


int main(int argc, char** argv){
    //if(argc != 3){
    //    fprintf(stderr, "./wolfserver port MAX_PLAYERS\n");
    //    fprintf(stderr, "pretty fricking self explanatory lmfao\n");
    //    return 1;
    //}
    struct sockaddr_in server_addr, client_addr;

    //char* port = argv[1];
    // udp: sock_dgram 
    int server = listen_on_port("0");
    printf("Socket created successfully\n");
    return 0;


}
