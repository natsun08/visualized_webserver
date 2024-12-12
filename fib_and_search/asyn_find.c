#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "asyn_find.h"
#include "common.h"

TaskQueue taskQueue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
// Function to enqueue a task
void Enqueue(SearchInput task) {
    TaskNode* newNode = (TaskNode*)malloc(sizeof(TaskNode));
    newNode->task = task;
    newNode->next = NULL;

    pthread_mutex_lock(&taskQueue.lock);
    if (taskQueue.rear == NULL) {
        taskQueue.front = taskQueue.rear = newNode;
    } else {
        taskQueue.rear->next = newNode;
        taskQueue.rear = newNode;
    }
    pthread_cond_signal(&taskQueue.cond);
    pthread_mutex_unlock(&taskQueue.lock);
}

// Function to dequeue a task
SearchInput Dequeue() {
    pthread_mutex_lock(&taskQueue.lock);
    while (taskQueue.front == NULL) {  // Wait for tasks if queue is empty
        pthread_cond_wait(&taskQueue.cond, &taskQueue.lock);
    }

    TaskNode* temp = taskQueue.front;
    SearchInput task = temp->task;
    taskQueue.front = taskQueue.front->next;
    if (taskQueue.front == NULL) {
        taskQueue.rear = NULL;
    }
    free(temp);
    pthread_mutex_unlock(&taskQueue.lock);
    return task;
}

// Function to perform the search
void* AsyncSearch(void* arg) {
    SearchInput* input = (SearchInput*)arg;
    bool found = false;

    for (int i = 0; i < input->size; i++) {
        if (input->array[i] == input->key) {
            found = true;
            break;
        }
    }

    // Print the result
    if (found) {
        printf("Key %d found in the array.\n", input->key);
    } else {
        printf("Key %d not present in the array.\n", input->key);
    }

    free(input);  // Free the allocated task input
    return NULL;
}

// Worker thread function
void* WorkerThread(void* arg) {
    while (true) {
        SearchInput task = Dequeue();
        SearchInput* taskCopy = (SearchInput*)malloc(sizeof(SearchInput));
        *taskCopy = task;  // Create a copy to pass to the thread
        pthread_t thread;
        pthread_create(&thread, NULL, AsyncSearch, (void*)taskCopy);
        pthread_detach(thread);  // Detach the thread to free resources automatically
    }
    return NULL;
}
