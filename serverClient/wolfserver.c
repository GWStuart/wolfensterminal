#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

void* handle_data(int sockfd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFSIZ];
    printf("gurttt");

    while (1) {
	int n = recvfrom(sockfd, buffer, BUFSIZ, 0, (struct sockaddr*)&client_addr, &addr_len);
	if(n>0) {
	    buffer[n] = '\0';
	    printf("bumass client sent: %s", buffer);
	    fflush(stdout);
	} 

	const char *msg = "deez nuts bumass client";
	sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&client_addr, 
		addr_len);


    }
    close(sockfd);
    return NULL;
}



#define PORT 8080


int listen_on_port(int port){
    int listenfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (listenfd < 0) {
	perror("fd is gone to sleep");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    // Bind socket to address
    if (bind(listenfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	perror("fass bind failed\n");
	close(listenfd);
        exit(-1);
    }
    printf("UDP connection on Port 8080\n");
    handle_data(listenfd);
    return listenfd;
   }



int main(){
    //if(argc != 3){
    //    fprintf(stderr, "./wolfserver port MAX_PLAYERS\n");
    //    fprintf(stderr, "pretty fricking self explanatory lmfao\n");
    //    return 1;
    //}

    //char* port = argv[1];
    // udp: sock_dgram 
    listen_on_port(PORT);
    printf("gurt");
    return 0;

}
    
