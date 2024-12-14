#ifndef COMMON_H
#define COMMON_H

#include <pthread.h> // Include for pthread_mutex_t and pthread_cond_t

// Common structure for search input
typedef struct {
  int* array;
  int size;
  int key;
} SearchInput;
extern SearchInput searchInput;

// TaskNode and TaskQueue (only for async event-based implementation)
typedef struct TaskNode {
  SearchInput task;
  struct TaskNode* next;
} TaskNode;

typedef struct {
  TaskNode* front;
  TaskNode* rear;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} TaskQueue;
void AcceptInput(int* array, int size, int key);
#endif // COMMON_H
