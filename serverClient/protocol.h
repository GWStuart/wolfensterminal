
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
#define CORRECT_PREFIX 0x56561111
#define NEW_PLAYER 1 //server sending new player information
#define UPDATE_PLAYERS 2 //server sending updated players
#define CLIENT_UPDATE 3 //client updating server
#define DISCONNECT 4 // client telling server they are disconnecting

typedef struct {
    uint32_t prefix;
    uint8_t operation;
    uint32_t dataSize;
    uint8_t* data;
    // come back here, you need to add the part where the id returns when the player is new
    uint8_t id;
} Message;

typedef struct {
    int id;
    int x;
    int y;
    float angle;
} Public;


typedef struct {                                                                 
    Public pdata;                                                                
    float speed;                                                                 
    int health;                                                                  
    struct sockaddr_in connection;                                               
} Player;  


int recieve_message(int sockfd, Message *message, struct sockaddr_in *from,
                    socklen_t *fromlen);

int send_message(int sockfd, struct sockaddr_in *to, socklen_t tolen, uint8_t operation,
                 uint32_t dataSize, uint8_t* data, uint8_t id);
