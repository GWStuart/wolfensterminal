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
    int x;
    int y;
    float angle;
    float speed;
    int health;
    struct sockaddr_in connection;
} Player;

typedef struct {
    Player* playerList;
    int listSize;
} connectionList;

void* handle_data(int sockfd) {
    struct sockaddr_in clientaddr;
    socklen_t addr_len = sizeof(clientaddr);
    connectionList players;
    players.listSize = 0;

    while (1) {
	char buffer[BUFSIZ];
	int n = recvfrom(sockfd, buffer, BUFSIZ, 0, (struct sockaddr*)&clientaddr, &addr_len);
	if(n>0) {
	    int playerID = -1;
	    for(int i = 0; i<players.listSize; i++){
		if(!memcmp(&players.playerList[i].connection, &clientaddr, sizeof(clientaddr))){
			playerID = i;
			break;
		}
	    }
	    if(playerID == -1){
		players.listSize++;
		players.playerList = realloc(players.playerList, sizeof(Player)*players.listSize);
		Player newPlayer = { .id = players.listSize-1, .x = 0, .y = 0, .angle = 0.6, .speed = 6.9, .health = 100, .connection = clientaddr};
		players.playerList[players.listSize-1] = newPlayer;
		const char *mapData = "deez nuts bumass client temp mapdata pretend I sent it to you";
		sendto(sockfd, mapData, strlen(mapData), 0, (struct sockaddr *)&clientaddr, 
		    addr_len);

	    } else if(!strncmp(buffer, "it all over now", 15)){ //player leave game
		int playerID;  
		sscanf(buffer, "%d", &playerID);
		if(playerID != players.listSize-1){
    		    for(int i = playerID; i<players.listSize-1; i++){
			players.playerList[i] = players.playerList[i+1];
		    }
		}
		players.listSize--;
		players.playerList = realloc(players.playerList, sizeof(Player)*players.listSize);
	    } else {
		Player updatePlayer = players.playerList[playerID];
		sscanf(buffer, "%d %d %d %f %f %d", 
			&updatePlayer.id, &updatePlayer.x, &updatePlayer.y, &updatePlayer.angle, &updatePlayer.speed, &updatePlayer.health);
	    }
	    char states[BUFSIZ];
	    int offset = 0;
	    for(int i = 0; i < players.listSize; i++){
		Player currPlayer = players.playerList[i];
		offset += sprintf(states + offset,
			"%d %d %d %f",
			currPlayer.id, currPlayer.x, currPlayer.y, currPlayer.angle);
	    }
	    for(int i = 0; i< players.listSize; i++){
		Player currPlayer = players.playerList[i];
		sendto(sockfd, states, strlen(states), 0,
                           (struct sockaddr *)&currPlayer.connection,
                           sizeof(currPlayer.connection));
	    }
	}
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
    
