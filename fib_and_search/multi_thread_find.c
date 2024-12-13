#include <pthread.h>
#include <stdatomic.h>
#include "multi_thread_find.h"
#include "common.h"
#include <stdio.h>

#define thread_max 4
atomic_int f = 0;
int current_thread = 0;

void* ThreadSearch(void* args) {
  int num = current_thread++;
  int size_per_thread = searchInput.size / thread_max;
  
  for (int i = num * size_per_thread; i < ((num + 1) * size_per_thread) && i < searchInput.size; i++) {
    if (f == 1) {
      return NULL; // Early exit
    }
    if (searchInput.array[i] == searchInput.key) {
      f = 1; // Key found
      return NULL;
    }
  }
  return NULL;
}

int MultiThreadSearch(char* result) {
  pthread_t threads[thread_max];
  current_thread = 0;
  f = 0; // Reset flag before starting the search
  
  // Create threads
  for (int i = 0; i < thread_max; i++) {
    pthread_create(&threads[i], NULL, ThreadSearch, NULL);
  }
  
  // Wait for threads to finish
  for (int i = 0; i < thread_max; i++) {
    pthread_join(threads[i], NULL);
  }
  
  // Log and populate the result
  if (f == 1) {
    sprintf(result, "Key %d found (Multi-threaded)", searchInput.key);
    printf("%s\n", result); // Log result to server
    return 1;
  } else {
    sprintf(result, "Key %d not found (Multi-threaded)", searchInput.key);
    printf("%s\n", result); // Log result to server
    return 0;
  }
}
