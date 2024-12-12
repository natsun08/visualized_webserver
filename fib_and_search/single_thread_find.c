#include <stdio.h>
#include "single_thread_find.h"
#include "common.h"

// Linear search function for a single thread
void SingleThreadSearch() {
  for (int i = 0; i < searchInput.size; i++) {
    if (searchInput.array[i] == searchInput.key) {
      printf("Key element found\n");
      return;
    }
  }
  printf("Key not present\n");
}
