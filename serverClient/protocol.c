#include <stdlib.h>
#include "protocol.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>


int send_message(int sockFD, struct sockaddr_in *to, socklen_t tolen,
                 uint8_t operation, uint8_t id, void *data, uint32_t dataSize)
{
    MessageHeader header;
    header.prefix = htole32(CORRECT_PREFIX);
    header.operation = operation;
    header.id = id;
    header.dataSize = htole32(dataSize);

    //this single thing saved me from depression, all hail libc
    //it basically does all the 2310 a4 stuff for you
    //I think this would've been super helpful
    struct iovec iov[2];
    iov[0].iov_base = &header;
    iov[0].iov_len  = sizeof(header);
    iov[1].iov_base = (void*)data;
    iov[1].iov_len  = data ? dataSize : 0;

    //this too, saved me from alot of sorrow.
    //basically, it makes it so that you can send fragmented data in one datagram
    //which is actually super usefull for UDP
    struct msghdr msg = {0};
    msg.msg_name = (void*)to;
    msg.msg_namelen = tolen;
    msg.msg_iov = iov;
    msg.msg_iovlen = data ? 2 : 1;

    ssize_t sent = sendmsg(sockFD, &msg, 0);
    return (int)sent;
}

int recv_message(int sockFD, MessageHeader *header, uint8_t *buffer, size_t bufferSize,
                 struct sockaddr_in *from, socklen_t *fromlen)
{
    uint8_t infoBytes[sizeof(MessageHeader)];
    ssize_t got = recvfrom(sockFD, infoBytes, sizeof(infoBytes), MSG_PEEK,
                           (struct sockaddr*)from, fromlen);
    if (got < (ssize_t)sizeof(MessageHeader)){
	perror("the header has a small member");
       	return -1;
    }

    memcpy(header, infoBytes, sizeof(MessageHeader));
    if (le32toh(header->prefix) != CORRECT_PREFIX){
	perror("bumass prefix is wrong");
       	return -1;
    }

    uint32_t payloadSize = le32toh(header->dataSize);
    if (payloadSize > bufferSize){
	perror("bumass server/client \"here, take more\" ahh");
       	return -1;
    }

    // total size
    size_t total = sizeof(MessageHeader) + payloadSize;
    uint8_t *fullMessage = (uint8_t*)malloc(total);
    //someone didn't tell me you can recieve in fragments... (12 hrs btw)
    ssize_t full = recvfrom(sockFD, fullMessage, total, 0, (struct sockaddr*)from, fromlen);
    if (full < (ssize_t)total) {
	perror("bummass wrong data \"here take less\" ahh");
       	free(fullMessage);
       	return -1;
    }

    //copy header and message into full message in order to send in one
    //BECAUSE YOU HAVE TO SEND IN ONE AND NOT THE OTHER WAY AROUND
    memcpy(header, fullMessage, sizeof(MessageHeader));
    if (buffer && payloadSize) memcpy(buffer, fullMessage + sizeof(MessageHeader), payloadSize);
    free(fullMessage);
    return (int)payloadSize; 
}

size_t build_public_snapshot(Player players[], size_t maxPlayers,
                             uint8_t excludeId, Public *out, size_t outCap)
{
    size_t outCount = 0;
    for (size_t i = 0; i < maxPlayers; ++i) {
        if (!players[i].in_use) continue;
        if (players[i].pdata.id == excludeId) continue;
        if (outCount < outCap) {
            out[outCount++] = players[i].pdata;
        }
    }
    return outCount;
}
