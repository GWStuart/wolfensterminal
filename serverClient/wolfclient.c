#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(){
    int clientFd;
    char buffer[BUFSIZ];
    struct sockaddr_in servaddr;
    clientFd = socket(AF_INET, SOCK_DGRAM, 0);
    // Create UDP socket
    if(clientFd == -1) {
	perror("bumass client failed");
	return -1;
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    const char* yuh = "HELLO SERVER :D";
    socklen_t len = sizeof(servaddr);
    sendto(clientFd, yuh, strlen(yuh), 0, (const struct sockaddr *)&servaddr, len);
    int n = recvfrom(clientFd, buffer, BUFSIZ, 0, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    printf("Server replied: %s\n", buffer);
    close(clientFd);
    return 0;



}
