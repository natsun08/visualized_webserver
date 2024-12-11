#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 200
#define PORT 8080
#define SA struct sockaddr
void func(int sockfd) 
{
    char buff[MAX];
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the command (e.g., 'request <number>', 'set mode single', 'set mode multi', 'set mode event', 'metrics', or 'exit'): ");
        
        // Get input from the user
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = 0; 

        write(sockfd, buff, sizeof(buff));

        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server: %s\n", buff);
        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main() 
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("socket successfully created...\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else {
        printf("connected to the server...\n");
    }

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}