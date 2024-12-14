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
    int event_type; 
    IoEvent events[MAX_SIZE]; 
    int front; 
    int rear; 
} IoEventQueue;

void initialize_queue(IoEventQueue* q);
bool is_empty(IoEventQueue* q);
void enqueue(IoEventQueue* q, IoEvent event);
IoEvent dequeue(IoEventQueue* q);
char* io_handler(void* data);
void* load_file_thread(void* arg); 
char* load_data_chunked(void* args); 

void* event_based(IoEvent event, int event_num, int connfd, char* buff, int buff_size, int count) {
    IoEventQueue queue_instance;
    IoEventQueue* queue = &queue_instance;
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
       IoEvent current_event = dequeue(queue);
        if (current_event.handler) {
            char* file_data = current_event.handler(current_event.event_data);
            if (file_data == NULL) {
                printf("Error: handler returned NULL data for event %d\n", i);
                all_successful = false;
                break;
            }
            
            // Free the file_data to prevent memory leak
            free(file_data);
        } else {
            all_successful = false;
            break;
        }
    }
    // Send data to client if all events were successful
    if (all_successful) {
        printf("All events processed successfully\n");
    } else {
        printf("Not all events were successful\n");
    }

    // Free the combined buffer
    free(combined_buffer);

    return all_successful ? buff : NULL;
}
