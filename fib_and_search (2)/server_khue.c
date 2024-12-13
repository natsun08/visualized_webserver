#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>


#define MAX_SIZE 4000
#define EVENT_TYPE_IO 1
#define EVENT_TYPE_Fibonacci 2
#define MAX 8000
#define PORT 8080
#define SA struct sockaddr

typedef struct {
    size_t file_size; 
    const char* filename; 
    int connfd; 
} IoArgs;

typedef char* (*EventHandler)(void*);
typedef struct {
    int event_type; 
    void* event_data; 
    EventHandler handler; 
} Event; 

typedef struct {
    int event_type; 
    Event events[MAX_SIZE]; 
    int front; 
    int rear; 
} EventQueue;

void* io_thread(void* data);
char* load_data(void* args);
void initialize_queue(EventQueue* q);
bool is_empty(EventQueue* q);
void enqueue(EventQueue* q, Event event);
Event dequeue(EventQueue* q);
char* io_handler(void* data);
void func(int connfd);
char* load_data_chunked(void* args); 
void thread_based(const char* filename, int request_num, int connfd);
void* event_based(Event event, int event_num, int connfd, char* buff, int buff_size, int count); 





void func(int connfd) {
    char buff[MAX];
    int n;
    int count;
    // printf("Buffer content: '%s'\n", buff);

    for (;;) {
        bzero(buff, MAX);
        read(connfd, buff, sizeof(buff));
        // printf("Buffer content: '%s'\n", buff);
        printf("From client: %s\t To client : ", buff);
        
        if (strncmp("IO events", buff, 9) == 0) { // run event here 
            bzero(buff, MAX);
            IoArgs io_args = {0, "2mb-examplefile-com.txt"};  // Replace with your file
            Event event = {EVENT_TYPE_IO, (void*)&io_args, io_handler};
            event_based(event, 10, connfd, buff, MAX, 1);
        }

        else if (strncmp("IO threads", buff, n) == 0) { // run event here 
            printf("Thread-based executed: "); 
            IoArgs io_args = {0, "2mb-examplefile-com.txt", connfd};  // Replace with your file
            thread_based("2mb-examplefile-com.txt", 10, connfd);
        }
    }
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }

    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else {
        printf("Server listening..\n");
    }

    len = sizeof(cli);

    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    } else {
        printf("server accept the client...\n");
    }
    
    func(connfd);

    close(sockfd);
    
    return 0; 
}
