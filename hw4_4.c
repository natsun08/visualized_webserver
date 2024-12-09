#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>


#define MAX 100000
#define FREE 0


// Shared variable

int count = 0;                                       

int lock = FREE;

void acquire_lock(int* lock){
      // Atomic load
      int wait = 0;
      while (atomic_flag_test_and_set(lock))
      {
        // busy wait
        wait++;
      }
}

void release(int* lock){
	
      *lock = FREE;
}


void* increasing(void* args) {
	acquire_lock(&lock);
	for (int i = 0; i < MAX; i++)

	count++;

	release(&lock);

	return NULL;

}

int yippee() {

		pthread_t even_thread, odd_thread;

		pthread_create(&even_thread, NULL, increasing, NULL);
		pthread_create(&odd_thread, NULL, increasing, NULL);

		pthread_join(even_thread, NULL);
		
		pthread_join(odd_thread, NULL);
		
	return count;

}

