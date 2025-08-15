#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>



int main(){
    int client_fd;
    struct sockaddr_in server_addr;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1) {
	perror("bumass client failed");
	return -1;
    }
    server_addr.sin_family = AF_INET;
    return 0;



}
