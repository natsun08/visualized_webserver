#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 1000
#define EVENT_TYPE_IO 1
#define EVENT_TYPE_Fibonacci 2

typedef struct {
    size_t file_size; 
    const char* filename; 
} IoArgs; 
typedef char (*EventHandler)(void*);
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

void initialize_queue(EventQueue* q) {
    q->front = 0;
    q->rear = 0; 
}

bool is_empty(EventQueue* q) {
    return q->front >= q->rear; 
}

void enqueue(EventQueue* q, Event event) {
    q->events[q->rear] = event;
    q->rear++; 
}
Event dequeue(EventQueue* q) {
    if (q->front < q->rear) {
        q->front++; 
        return q->events[q->front - 1]; 
    }
    else {
        perror("Empty event queue.");
        exit(1); 
    }
}

char* load_data (void* args) {  
    IoArgs* io_args = (IoArgs*) args;
    const char* filename = io_args->filename; 

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error while opening file\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    io_args->file_size = ftell(file);  // Update file_size directly
    rewind(file);

    char* file_data = (char*)malloc(io_args->file_size);
    if (!file_data) {
        printf("Error allocating memory\n");
        fclose(file);
        return NULL;
    }

    fread(file_data, 1, io_args->file_size, file);
    fclose(file);
    return file_data; 
}

char* io_handler(void* data) {
    IoArgs* io_args = (IoArgs*)data;
    char* file_data = load_data(io_args);
    if (file_data) {
        return file_data; 
    } else {
        printf("Error while loading data from file: %s\n", io_args->filename);
        return NULL; 
    }
}


void* load_file_thread(void* arg) {
    IoArgs* io_args = (IoArgs*)arg;  // Get the argument data
    char* file_data = load_data(io_args);  // Load file content

    if (file_data) {
        printf("File %s loaded successfully:\n%s\n", io_args->filename, file_data);
        free(file_data); // Free memory after use
    } else {
        printf("Failed to load file: %s\n", io_args->filename);
    }

    return NULL;
}

