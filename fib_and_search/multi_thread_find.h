#ifndef MULTI_THREAD_FIND_H
#define MULTI_THREAD_FIND_H

#include <pthread.h>
#include <stdatomic.h>
#include "common.h" // Shared structures

#define thread_max 4 // Max number of threads

// Multi-threaded search functions
void AcceptInput(int* array, int size, int key);
int MultiThreadSearch(); // Updated function
void* ThreadSearch(void* args);

#endif // MULTI_THREAD_FIND_H
