#ifndef ASYNCHRONOUS_EVENT_FIND_H
#define ASYNCHRONOUS_EVENT_FIND_H
#include <pthread.h>
#include "common.h" // Shared structures

// Async event-based functions
void Enqueue(SearchInput task);
SearchInput Dequeue();
void* AsyncSearch(void* arg);
void* WorkerThread(void* arg);

#endif // ASYNCHRONOUS_EVENT_FIND_H
