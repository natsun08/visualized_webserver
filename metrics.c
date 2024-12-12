#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

long calculate_fibonacci(int n);
long calculate_fibonacci_multithreaded(int n);                
void add_event(int id, int n);                               
void event_loop();

long long get_time_in_nanoseconds() {
    struct timespec ts;
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// Function designed for chat between client and server. 
void func(int connfd) {
    char buff[MAX];
    int index;
    
    for (;;) {
        bzero(buff, MAX);
        int bytes_read = read(connfd, buff, sizeof(buff));
        
        if (bytes_read <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }
        if (strlen(buff) == 0) {
            // Skip empty inputs
            continue;
        }
        
        printf("From client: %s\n", buff);
        
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }

        long long start_time = get_time_in_nanoseconds();
        
        char *token = strtok(buff, " ");
        char response[MAX] = {0};  // Zero-initialize entire buffer
        
        while (token != NULL) {
            // Add error checking for conversion
            char *endptr;
            long index = strtol(token, &endptr, 10);
            
            // Check for conversion errors
            if (token == endptr) {
                printf("Conversion error for token: %s\n", token);
                continue;
            }
            
            long res = calculate_fibonacci_multithreaded(index);
            
            // Use snprintf for safer concatenation
            size_t remaining = sizeof(response) - strlen(response) - 1;
            snprintf(response + strlen(response), remaining, "%ld ", res);
            
            token = strtok(NULL, " ");
        }

        // End time measurement
        long long end_time = get_time_in_nanoseconds();
        
        // Calculate execution time
        long long execution_time = end_time - start_time;
        
        // Ensure null-termination and trim trailing space
        response[sizeof(response) - 1] = '\0';
        size_t len = strlen(response);
        if (len > 0 && response[len-1] == ' ') {
            response[len-1] = '\0';
        }
        
        if (strlen(response) > 0) {
            write(connfd, response, strlen(response));
            printf("From Server: %s\n", response);
            printf("Execution Time: %lld ns\n", execution_time);
        }
    }
}


void main(){
 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server accept the client...\n"); 
  
    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 



