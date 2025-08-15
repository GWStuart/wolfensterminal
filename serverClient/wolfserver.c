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


int listen_on_port(char* port){
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // listen on all IP addresses`
    int err;
    if ((err = getaddrinfo(NULL, port, &hints, &ai))) {
        freeaddrinfo(ai);
        return -1; // Could not determine address
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (listenfd < 0) {
        freeaddrinfo(ai);
        return -1;
    }
    int optVal = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int))
            < 0) {
        freeaddrinfo(ai);
        return -1;
    }
    // Bind socket to address
    if (bind(listenfd, ai->ai_addr, sizeof(struct sockaddr)) < 0) {

        freeaddrinfo(ai);
        return -1;
    }
    // Indicate willingness to listen on a socet - connections can now be queued
    if (listen(listenfd, 1) < 0) { // Up to 10 connection requests can queue
        freeaddrinfo(ai);
        return -1;
    }
    struct sockaddr_in portInfo;
    socklen_t len = sizeof(portInfo);
    getsockname(listenfd, (struct sockaddr*)&portInfo, &len);
    int actualPort = ntohs(portInfo.sin_port);
    fprintf(stderr, "%d\n", actualPort);
    fflush(stderr);
    freeaddrinfo(ai);
    // Have listening socket - return it
    return listenfd;
   }

void *handle_client(void *client_socket) {
    int clientfd = *(int *)client_socket;
    char buffer[BUFSIZ];

    while(1){
	memset(buffer, 0, sizeof(buffer));
	int bytes_recieved = recv(clientfd, buffer, sizeof(buffer), 0);
	if(bytes_recieved <= 0) {
	    printf("bumass client gone\n");
	    break;
	}
	printf("Client: %s\n", buffer);
	send(clientfd, "bumass client", strlen("bumass client"), 0);
    }
    close(clientfd);
    free(client_socket);
    return NULL;
}

void connect_everything(int server) {
    int client_fd;
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize;
    while (1) {
        fromAddrSize = sizeof(struct sockaddr_in);
        client_fd = accept(server, (struct sockaddr*)&fromAddr, &fromAddrSize);
        if (client_fd < 0) {
	    perror("something went wrong number 1\n");
	    exit(-1);
        }

        char hostname[NI_MAXHOST];
        int error = getnameinfo((struct sockaddr*)&fromAddr, fromAddrSize,
                hostname, NI_MAXHOST, NULL, 0, 0);
        if (error) {
	    perror("something went wrong\n");
	    exit(-1);
        }

	int *client_socket = malloc(sizeof(int));
	*client_socket = client_fd;
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, handle_client, client_socket);
        pthread_detach(thread_id);
    }



}



int main(){
    //if(argc != 3){
    //    fprintf(stderr, "./wolfserver port MAX_PLAYERS\n");
    //    fprintf(stderr, "pretty fricking self explanatory lmfao\n");
    //    return 1;
    //}

    //char* port = argv[1];
    // udp: sock_dgram 
    int server = listen_on_port("0");
    connect_everything(server);

}
    
