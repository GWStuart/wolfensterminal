
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
    uint8_t dataSize;
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
                    socklen_t *fromlen){
    uint8_t buffer[BUFSIZ];
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)from,
                     fromlen);
    if(n < (int)(sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t))){
        perror("this message has a small member");
        return -1;
    }
    uint32_t prefix = le32toh(*(uint32_t*)(buffer));
    if(prefix != 0x56561111){
        perror("the prefix is wrong, this guy is a faker");
        return -1;
    }
    printf("%X\n", prefix);
    fflush(stdout);

    message->prefix = prefix;
    message->operation = *(uint8_t*)(buffer + sizeof(uint32_t));
    printf("%d\n", message->operation);
    fflush(stdout);
    message->dataSize = (*(uint8_t*)(buffer + sizeof(uint32_t) + sizeof(uint8_t)));
    printf("%X\n", message->dataSize);
    fflush(stdout);
    if(message->dataSize > n-(2*sizeof(uint8_t)+sizeof(uint32_t))){
        perror("bumass client/server tried to send a file that was too large");
        return -1;
    }
    message->data = malloc(sizeof(uint8_t)*message->dataSize);
    memcpy(message->data, buffer +sizeof(uint8_t)+2*sizeof(uint32_t), message->dataSize);
    if(message->operation == 1 || message->operation == 3) { //new player gets ID, returning gets list size.
        message->id = *(uint8_t*)(buffer+sizeof(uint8_t)+2*sizeof(uint32_t) +message->dataSize);
    }
    return 0;

}

int send_message(int sockfd, struct sockaddr_in *to, socklen_t tolen, uint8_t operation,
                 uint8_t dataSize, uint8_t* data, uint8_t id){
    printf("%X\n", dataSize);
    Message message = {0};
    message.prefix = htole32(CORRECT_PREFIX);
    message.operation = operation;
    message.data = malloc(sizeof(uint8_t)*message.dataSize);
    memcpy(message.data, data, dataSize);

    size_t memory = sizeof(message.prefix) + sizeof(message.operation) +
    sizeof(message.dataSize) + dataSize;
    if(operation == 1 || operation == 3){
        message.id = id;
        memory += sizeof(message.id);

    }

    message.dataSize = htole32(dataSize);
    printf("%X\n", dataSize);
    fflush(stdout);
    int ret = sendto(sockfd, &message, memory, 0, (struct sockaddr*)to, tolen);
    free(message.data);
    return ret;

}
