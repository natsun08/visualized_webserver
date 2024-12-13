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
void* io_thread(void* data);

typedef struct {
    size_t file_size; 
    const char* filename; 
    int connfd; 
} IoArgs;


void thread_based(const char* filename, int request_num, int connfd) {
    int* thread_status = malloc(request_num * sizeof(int));
    pthread_t* thread_ids = malloc(request_num * sizeof(pthread_t));
    
    if (thread_ids == NULL || thread_status == NULL) {
        printf("Memory allocation failed.\n");
        free(thread_ids);
        free(thread_status);
        return;
    }
    
    memset(thread_status, 0, request_num * sizeof(int)); // status array 

    for (int i = 0; i < request_num; i++) {
        IoArgs* io_args = malloc(sizeof(IoArgs));
        if (io_args == NULL) {
            printf("Memory allocation for IoArgs failed for thread %d\n", i);
            thread_status[i] = -1;
            continue;
        }
        
        io_args->file_size = 0;
        io_args->filename = filename;
        io_args->connfd = connfd;

        int create_status = pthread_create(&thread_ids[i], NULL, io_thread, (void*)io_args);
        if (create_status != 0) {
            printf("Error creating thread %d: Error code %d\n", i, create_status);
            thread_status[i] = create_status;
            free(io_args);
        }
    }

    // Check thread join results
    bool all_successful = true;
    for (int i = 0; i < request_num; i++) {
        void* thread_return;
        int join_status = pthread_join(thread_ids[i], &thread_return);
        
        if (join_status != 0) {
            printf("Error joining thread %d: Error code %d\n", i, join_status);
            all_successful = false;
        }
        
        if (thread_return != NULL) {
            printf("Thread %d reported an error\n", i);
            all_successful = false;
        }
    }

    if (all_successful) {
        printf("All %d threads completed successfully.\n", request_num);
    } else {
        printf("Some threads failed to complete correctly.\n");
    }

    free(thread_ids);
    free(thread_status);
}
