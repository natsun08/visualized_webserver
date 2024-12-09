#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 100

typedef struct {
    int requests[MAX_SIZE];
    int front;
    int rear;
} Queue;

void initialize_queue(Queue* q) {
    q->front = -1;
    q->rear = 0;
}
bool is_empty(Queue* q) { 
    return (q->front == q->rear - 1); 
}
bool is_full(Queue* q) { 
    return (q->rear == MAX_SIZE); 
}

void enqueue(Queue* q, int value) {
    if (is_full(q)) {
        printf("Queue is full\n");
        return;
    }
    q->requests[q->rear] = value;
    q->rear++;
}

void dequeue(Queue* q) {
    if (is_empty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}

int peek(Queue* q) {
    if (is_empty(q)) {
        return -1; 
    }
    return q->requests[q->front + 1];
}

typedef struct {
    const char* filename;
    size_t* file_size;
} args;

// function to load data
void* load_data (const char* filename, size_t* file_size) { // add argument to identify the specific user 
    struct args* data = (struct args*) arg;  
    const char* filename = data->filename; 
    size_t* file_size = data->file_size;

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error while opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(*file_size);
    if (!buffer) {
        printf("Error allocating memory");
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, *file_size, file);
    fclose(file);

    // Add instructions to send response to requests 

}

// thread-based function to deliver data to users
void* thread_based_io(const char* filename, int request_num) {
    struct args* request = malloc(request_num * sizeof(struct args));
    pthread_t thread_ids[request_num] = malloc(request_num * sizeof(pthread_t));
    size_t file_size = 0;
    for (int i = 0; i < request_num; i++) {
        size_t file_size = 0;
        request[i].filename = filename;
        request[i].file_size = file_size;
    }
    
    for (int i = 0; i < request_num; i++) {
        pthread_create(&thread_ids[i], NULL, load_data, &request[i]);
    }

    for (int i = 0; i < request_num; i++) {
        pthread_join(thread_ids[i], NULL); 
    }
    
    printf("Successfully loaded the file. \n");
}

// event-base function to deliver data to users 
int event_based_io(const char* filename, Queue* requests) {
    size_t file_size = 0;
    while (!is_empty(requests)) {
        dequeue(requests);  
        load_data(filename, &file_size);  
    }
}

int main() {

}