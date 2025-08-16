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
#include "protocol.h"

#define MAX_PLAYERS 5
#define CORRECT_PREFIX 0x56561111
#define NEW_PLAYER 1 //server sending new player information
#define UPDATE_PLAYERS 2 //server sending updated players
#define CLIENT_UPDATE 3 //client updating server
#define DISCONNECT 4 // client telling server they are disconnecting


typedef struct {
    Player* playerList;
    int listSize;
} connectionList;


void* handle_data(int sockfd) {
    struct sockaddr_in clientaddr;
    socklen_t addr_len = sizeof(clientaddr);
    connectionList players = {0};
    players.listSize = 0;
    int index = 0;

    while (1) {
	index++;
	Message msg = {0};
	if(recieve_message(sockfd, &msg, &clientaddr, &addr_len) == 0){
	    int playerID = -1;
	    for(int i = 0; i<players.listSize; i++){
		if(!memcmp(&players.playerList[i].connection, &clientaddr, sizeof(clientaddr))){
		    playerID = i;
		    break;
		}
	    }
	    switch(msg.operation) {
		case CLIENT_UPDATE: 
		
		if(playerID == -1){
		    //perform sending map data, probably from a file of some sort
		    //this is temp code.
		    printf("shuf\n");
		    players.listSize++;
		    players.playerList = realloc(players.playerList, sizeof(Player)*players.listSize);
		    Player newPlayer = {0};
	            memcpy(&newPlayer, msg.data, sizeof(Player));
		    newPlayer.pdata.id = players.listSize;
		    players.playerList[players.listSize-1] = newPlayer;
		    players.playerList[players.listSize-1].connection = clientaddr;


		    const char *mapData = "deez nuts bumass client temp mapdata pretend I sent it to you";
		    send_message(sockfd, &clientaddr, addr_len, 1, sizeof(mapData), (uint8_t*)mapData, newPlayer.pdata.id);
		    //additionally, need to send the id as well

		} else { //the message sent was an existing player
		    Player newPlayer = {0};
	            memcpy(&newPlayer, msg.data, sizeof(Player));
		    players.playerList[playerID] = newPlayer;
		    players.playerList[playerID].connection = clientaddr;
		}
		break;
				    
		case DISCONNECT: 
		    if(playerID != players.listSize-1){
			for(int i = playerID; i<players.listSize-1; i++){
			    players.playerList[i] = players.playerList[i+1];
			}
		    }
		    players.playerList = realloc(players.playerList, sizeof(Player)*players.listSize);
		    break;
	    }

	    //sending updates to everysingle player, using only relevant information.

	    fflush(stdout);
	    if(players.listSize != 0){
		Public* pdatas = malloc(sizeof(Public)*players.listSize);
		for(int i = 0; i<players.listSize; i++){
		    pdatas[i] = players.playerList[i].pdata;
		}
		for(int i = 0; i<players.listSize; i++){
			send_message(sockfd, &players.playerList[i].connection, addr_len, UPDATE_PLAYERS, sizeof(Public)*players.listSize, (uint8_t*)pdatas, players.listSize);
		}

		free(pdatas);
	    }


	    if(!index%1000){
		index = 0;
		if(players.listSize == 0){
		    printf("bum ass\n");
		}

		for(int i = 0; i<players.listSize; i++) {
		    Player curPlayer = players.playerList[i];
		    printf("ID: %d, x: %d, y: %d, angle: %f, health: %d, speed: %f\n",
			    curPlayer.pdata.id, curPlayer.pdata.x, curPlayer.pdata.y, curPlayer.pdata.angle, curPlayer.health, curPlayer.speed);
		}
		fflush(stdout);

	    }
	    
	    free(msg.data);
	}
    }
    free(players.playerList);
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
    
