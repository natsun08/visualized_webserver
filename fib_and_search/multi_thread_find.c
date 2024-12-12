#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include "multi_thread_find.h"
#include "common.h"
// Max number of threads to create
#define thread_max 4

// Flag to indicate if key is found in a[]
// or not.
atomic_int f = 0;
int current_thread = 0;

// Linear search function which will
// run for all the threads
void* ThreadSearch(void* args)
{
  int num = current_thread++;
  int size_per_thread = searchInput.size / thread_max;
  
  for (int i = num * size_per_thread;
       i < ((num + 1) * size_per_thread) && i < searchInput.size; i++) {
    if (f == 1) {
      return NULL; // Early exit if key is already found
    }
    if (searchInput.array[i] == searchInput.key) {
      f = 1;
      return NULL;
    }
  }
  return NULL;
}

// Function to output the result of the search
void OutputResult() {
  if (f == 1)
    printf("Key element found\n");
  else
    printf("Key not present\n");
}
