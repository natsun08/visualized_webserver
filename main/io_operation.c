#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 1000
// #define EVENT_TYPE_IO 1
// #define EVENT_TYPE_Fibonacci 2
#define CHUNK_SIZE 4096


typedef struct {
    size_t file_size; 
    const char* filename; 
    int connfd; 
} IoArgs; 
typedef char (*EventHandler)(void*);
typedef struct {
    // int event_type; 
    void* event_data; 
    EventHandler handler; 
} IoEvent; 

// typedef struct {
//     // int event_type; 
//     IoEvent events[MAX_SIZE]; 
//     int front; 
//     int rear; 
// } EventQueue;

// void initialize_queue(EventQueue* q) {
//     q->front = 0;
//     q->rear = 0; 
// }

// bool is_empty(EventQueue* q) {
//     return q->front >= q->rear; 
// }

// void enqueue(EventQueue* q, IoEvent event) {
//     q->events[q->rear] = event;
//     q->rear++; 
// }
// IoEvent dequeue(EventQueue* q) {
//     if (q->front < q->rear) {
//         q->front++; 
//         return q->events[q->front - 1]; 
//     }
//     else {
//         perror("Empty event queue.");
//         exit(1); 
//     }
// }

char* load_data_chunked(void* args) {
    IoArgs* io_args = (IoArgs*) args;
    FILE* file = fopen(io_args->filename, "r");
    if (!file) return NULL;

    #define CHUNK_SIZE 4096
    char* buffer = malloc(io_args->file_size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = 0;
    size_t total_read = 0;
    char chunk[CHUNK_SIZE];

    while ((bytes_read = fread(chunk, 1, CHUNK_SIZE, file)) > 0) {
        memcpy(buffer + total_read, chunk, bytes_read);
        total_read += bytes_read;
    }

    buffer[total_read] = '\0';  // Null-terminate
    fclose(file);
    return buffer;
}

char* load_data(void* args) {
    IoArgs* io_args = (IoArgs*) args;
    const char* filename = io_args->filename;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error while opening file\n");
        return NULL;
    }

    // Check file size
    fseek(file, 0, SEEK_END);
    io_args->file_size = ftell(file);
    rewind(file);

    // Choose loading method based on file size
    if (io_args->file_size > 10 * 1024 * 1024) {  // > 10MB
        // Use chunked loading for large files
        return load_data_chunked(args);
    } else {
        // Use original method for smaller files
        char* file_data = malloc(io_args->file_size + 1);
        if (!file_data) {
            printf("Error allocating memory\n");
            fclose(file);
            return NULL;
        }

        fread(file_data, 1, io_args->file_size, file);
        file_data[io_args->file_size] = '\0';  // Null-terminate
        fclose(file);
        return file_data;
    }
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

void* io_thread(void* data) {
    IoArgs* args = (IoArgs*)data;
    if (!args) {
        printf("Invalid thread arguments\n");
        return NULL;
    }

    // Actual file loading logic
    FILE* file = fopen(args->filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", args->filename);
        return NULL;
    }

    // Read file contents
    char buffer[CHUNK_SIZE];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    if (bytes_read > 0) {
        // Send data back to client
        ssize_t sent = write(args->connfd, buffer, bytes_read);
        if (sent < 0) {
            perror("Failed to send data to client");
        }
    }

    free(args);
    return NULL;
}


