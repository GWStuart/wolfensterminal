
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>

//macroes: this can change to fit your needs
#define MAX_PLAYERS 20
#define MAX_MAP_BYTES 2048
#define CORRECT_PREFIX 0x56561111

typedef enum {
    CLIENT_HELLO      = 1,  
    CLIENT_UPDATE     = 2,  
    CLIENT_DISCONNECT = 3,  
    SERVER_WELCOME    = 10, 
    SERVER_SNAPSHOT   = 11  
} OpCode;

//apparently this makes the stuff all close together, idk thank stackoverflow
typedef struct __attribute__((packed)) { 
    uint8_t id;   
    int x;
    int y;
    float angle;
} Public;

typedef struct {
    Public pdata;
    float  speed;
    int32_t health;
    struct sockaddr_in addr; 
    int in_use;            
} Player;

//I use it literally everywhere because it makes things easier to send as structs
typedef struct __attribute__((packed)) {
    uint32_t prefix;
    uint8_t  operation;
    uint8_t  id;       
    uint32_t dataSize; 
} MessageHeader;

//I literally never not use it (do not look at player struct)
typedef struct __attribute__((packed)) {
    uint8_t assigned_id;
    uint16_t map_size;
    uint8_t map_bytes[MAX_MAP_BYTES]; 
} WelcomePayload;


int recv_message(int sockFD, MessageHeader *header, uint8_t *buf, size_t buf_cap,
                 struct sockaddr_in *from, socklen_t *fromlen);

int send_message(int sockFD, struct sockaddr_in *to, socklen_t tolen,
                 uint8_t operation, uint8_t id, void *data, uint32_t dataSize);

// Utility to fill a vector of Public from server state, excluding "exclude_id".
// Returns the number of entries written.
size_t build_public_snapshot(Player players[], size_t maxPlayers,
                             uint8_t exclude_id, Public *out, size_t outCap);

#endif 
