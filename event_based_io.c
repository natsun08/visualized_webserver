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
    // int event_type; 
    void* event_data; 
    EventHandler handler; 
} IoEvent; 

typedef struct {
    // int event_type; 
    Event events[MAX_SIZE]; 
    int front; 
    int rear; 
} EventQueue;

void initialize_queue(EventQueue* q);
bool is_empty(EventQueue* q);
void enqueue(EventQueue* q, Event event);
Event dequeue(EventQueue* q);
char* io_handler(void* data);
void* load_file_thread(void* arg); 
char* load_data_chunked(void* args); 

void* event_based(IoEvent event, int event_num, int connfd, char* buff, int buff_size, int count) {
    EventQueue queue_instance;
    EventQueue* queue = &queue_instance;
    initialize_queue(queue);

    for (int i = 0; i < event_num; i++) {
        enqueue(queue, event);
        printf("Successfully enqueued event %d\n", i);
    }

    bool all_successful = true;
    char* combined_buffer = malloc(buff_size * event_num);  
    if (combined_buffer == NULL) {
        printf("Memory allocation failed for combined buffer\n");
        return NULL;
    }
    memset(combined_buffer, 0, buff_size * event_num);

    // Process events
    for (int i = 0; i < event_num; i++) {
        Event current_event = dequeue(queue);
        if (current_event.handler) {
            char* file_data = current_event.handler(current_event.event_data);
            if (file_data == NULL) {
                printf("Error: handler returned NULL data for event %d\n", i);
                all_successful = false;
                break;
            }
            
            // Concatenate or append file data
            strncat(combined_buffer, file_data, buff_size - strlen(combined_buffer) - 1);
            
            // Free the file_data to prevent memory leak
            free(file_data);
        } else {
            printf("Unknown event type: %d\n", current_event.event_type);
            all_successful = false;
            break;
        }
    }
    // Send data to client if all events were successful
    if (all_successful) {
        printf("All events processed successfully\n");
        write(connfd, combined_buffer, strlen(combined_buffer));
        
        // Copy to original buffer if needed
        strncpy(buff, combined_buffer, buff_size - 1);
        buff[buff_size - 1] = '\0';
    } else {
        printf("Not all events were successful\n");
    }

    // Free the combined buffer
    free(combined_buffer);

    return all_successful ? buff : NULL;
}
