#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int n;
    long long result;
} FibonacciThreadData;

// Thread function to calculate Fibonacci
void *fibonacci_thread(void *arg) {
    FibonacciThreadData *data = (FibonacciThreadData *)arg;
    if (data->n <= 1) {
        data->result = data->n;
    } else {
        long long a = 0, b = 1, c;
        for (int i = 2; i <= data->n; i++) {
            c = a + b;
            a = b;
            b = c;
        }
        data->result = b;
    }
    return NULL;
}

// Function to calculate Fibonacci using a thread
long calculate_fibonacci_multithreaded(int n) {
    pthread_t thread;
    FibonacciThreadData data = {n, 0};
    pthread_create(&thread, NULL, fibonacci_thread, &data);
    pthread_join(thread, NULL);
    return data.result;
}

