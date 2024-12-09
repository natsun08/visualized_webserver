#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 

#define MAX_EVENTS 10

typedef struct {
    int id;           
    int n;            
    long long result; 
    long long a, b;   
    int current_step; 
    bool done;        
} FibonacciEvent;

FibonacciEvent events[MAX_EVENTS];
int num_events = 0;

void add_event(int id, int n) {
    if (num_events >= MAX_EVENTS) {
        printf("Error: Maximum number of events reached.\n");
        return;
    }

    events[num_events].id = id;
    events[num_events].n = n;
    events[num_events].result = 0;
    events[num_events].a = 0;
    events[num_events].b = 1;
    events[num_events].current_step = 2; 
    events[num_events].done = false;
    num_events++;
}

void process_event(FibonacciEvent *event) {
    if (event->done) return; 

    if (event->n <= 1) {
        // Base case
        event->result = event->n;
        event->done = true;
        printf("Event %d: Finished Fibonacci(%d) = %lld\n", event->id, event->n, event->result);
        return;
    }

    // Iterative Fibonacci
    if (event->current_step <= event->n) {
        long long temp = event->a + event->b;
        event->a = event->b;
        event->b = temp;
        event->result = event->b;
        event->current_step++;
    }

    if (event->current_step > event->n) {
        event->done = true;
        printf("Event %d: Finished Fibonacci(%d) = %lld\n", event->id, event->n, event->result);
    }
}

void event_loop() {
    bool all_done;
    do {
        all_done = true;

        for (int i = 0; i < num_events; i++) {
            if (!events[i].done) {
                process_event(&events[i]);
                all_done = false; 
            }
        }

        usleep(1000); 
    } while (!all_done);
}

int main() {
    int numbers[MAX_EVENTS];
    int size;

    // Input numbers
    printf("Enter the number of Fibonacci calculations (max %d): ", MAX_EVENTS);
    scanf("%d", &size);

    if (size > MAX_EVENTS) {
        printf("Error: Maximum number of events is %d.\n", MAX_EVENTS);
        return 1;
    }

    printf("Enter the numbers to calculate Fibonacci (space-separated): ");
    for (int i = 0; i < size; i++) {
        scanf("%d", &numbers[i]);
    }

    for (int i = 0; i < size; i++) {
        add_event(i, numbers[i]);
    }

    event_loop();

    printf("Final Results:\n");
    for (int i = 0; i < num_events; i++) {
        printf("Fibonacci(%d) = %lld (Event %d)\n", events[i].n, events[i].result, events[i].id);
    }

    return 0;
}
