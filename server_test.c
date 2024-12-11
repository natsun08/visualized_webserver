#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
// #include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SIZE 1000
#define EVENT_TYPE_IO 1
#define EVENT_TYPE_Fibonacci 2
#define MAX 15000
#define PORT 8080
#define SA struct sockaddr

// Event and Queue Structure
typedef struct {
    size_t file_size; 
    const char* filename; 
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

// Function Prototypes
// long calculate_fibonacci(int n);
char* load_data(void* args);
void initialize_queue(EventQueue* q);
bool is_empty(EventQueue* q);
void enqueue(EventQueue* q, Event event);
Event dequeue(EventQueue* q);
char* io_handler(void* data);
void func(int connfd);
void* load_file_thread(void* arg); 


void* event_based(Event event, int event_num, int connfd, char buff[MAX], int n, int count) {
    EventQueue queue_instance;
    EventQueue* queue = &queue_instance;
    initialize_queue(queue);
    for (int i = 0; i < event_num; i++) {
        enqueue(queue, event);
    }
    bool all_successful = true;
    for (int i = 0; i < event_num; i++) {
        Event current_event = dequeue(queue);
        if (current_event.handler) {
            char* file_data = current_event.handler(current_event.event_data); // Call the handler function
            if (file_data != NULL) {
                snprintf(buff, MAX, "%s", file_data); // Store the result in buff
                write(connfd, buff, strlen(buff)); // Write the data back to the client
            } else {
                printf("Error: handler returned NULL data.\n");
                all_successful = false;
            }
            write(connfd, buff, sizeof(buff));
        } else {
            printf("Unknown event type: %d\n", current_event.event_type);
            all_successful = false;
        }
    }
    if (all_successful) {
        printf("all_successful!\n");
    } else {
        printf("Not all events were successful.\n");
    }
    return NULL; 
}

void thread_based(const char* filename, int request_num) {
    pthread_t* thread_ids = malloc(request_num * sizeof(pthread_t)); // Array to hold thread IDs

    // Check if malloc succeeded
    if (thread_ids == NULL) {
        printf("Memory allocation for thread IDs failed.\n");
        return;
    }

    // Create 'request_num' threads, each to load a file
    for (int i = 0; i < request_num; i++) {
        IoArgs* io_args = malloc(sizeof(IoArgs)); // Allocate memory for IoArgs per thread
        if (io_args == NULL) {
            printf("Memory allocation for IoArgs failed.\n");
            free(thread_ids); // Free the allocated thread_ids memory
            return;
        }
        
        io_args->file_size = 0;  // Can be updated based on actual file size if needed
        io_args->filename = filename;  // Set filename

        // Create thread and pass IoArgs as argument
        if (pthread_create(&thread_ids[i], NULL, load_file_thread, (void*)io_args) != 0) {
            printf("Error creating thread %d\n", i);
            free(io_args); // Free memory in case of error
            continue;
        }
    }

    // Join all threads to ensure they finish execution
    for (int i = 0; i < request_num; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Free memory after all threads are joined
    free(thread_ids);
}


void func(int connfd) {
    char buff[MAX];
    int n;
    int count;

    for (;;) {
        bzero(buff, MAX);
        read(connfd, buff, sizeof(buff));

        printf("From client: %s\t To client : ", buff);

        if (strncmp("IO events", buff, 9) == 0) { // run event here 
            IoArgs io_args = {0, "10mb.txt"};  // Replace with your file
            Event event = {EVENT_TYPE_IO, (void*)&io_args, io_handler};
            event_based(event, 300, connfd, buff, MAX, 1);
        }

        // if (strncmp("IO threads", buff, 9) == 0) { // run event here 
        //     IoArgs io_args = {0, "2mb-examplefile-com.txt"};  // Replace with your file
        //     Event event = {EVENT_TYPE_IO, (void*)&io_args, io_handler};
        //     thread_based(event, 1, connfd, buff, MAX, 1);
        // }

        // add thread-based and event-based for Fibonacci 

        bzero(buff, MAX);

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
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
