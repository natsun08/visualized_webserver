#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS 10  

typedef struct {
    int id;
    int n;
    long long result;
} ThreadData;

void *fibonacci_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Base case
    if (data->n <= 1) {
        data->result = data->n;
    } else {
        // Iterative Fibonacci
        long long a = 0, b = 1;
        for (int i = 2; i <= data->n; i++) {
            long long temp = a + b;
            a = b;
            b = temp;
        }
        data->result = b;
    }

    printf("Thread %d: Finished Fibonacci(%d) = %lld\n", data->id, data->n, data->result);
    return NULL;
}

int main() {
    int numbers[MAX_THREADS];
    int size;

    // Input array
    printf("Enter the number of Fibonacci calculations (max %d): ", MAX_THREADS);
    scanf("%d", &size);

    if (size > MAX_THREADS) {
        printf("Error: Maximum number of threads is %d.\n", MAX_THREADS);
        return 1;
    }

    printf("Enter the numbers to calculate Fibonacci (space-separated): ");
    for (int i = 0; i < size; i++) {
        scanf("%d", &numbers[i]);
    }

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    // Create threads 
    for (int i = 0; i < size; i++) {
        thread_data[i].id = i;
        thread_data[i].n = numbers[i];
        thread_data[i].result = 0;

        if (pthread_create(&threads[i], NULL, fibonacci_thread, &thread_data[i]) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    for (int i = 0; i < size; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            return 1;
        }
    }

    printf("Final Results:\n");
    for (int i = 0; i < size; i++) {
        printf("Fibonacci(%d) = %lld (Thread %d)\n", thread_data[i].n, thread_data[i].result, thread_data[i].id);
    }

    return 0;
}
