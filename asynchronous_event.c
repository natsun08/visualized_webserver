#include <stdio.h>
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

static FibonacciEvent events[MAX_EVENTS];
static int num_events = 0;

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

// Process a single event
void process_event(FibonacciEvent *event) {
    if (event->done) return;

    if (event->n <= 1) {
        event->result = event->n;
        event->done = true;
        return;
    }

    if (event->current_step <= event->n) {
        long long temp = event->a + event->b;
        event->a = event->b;
        event->b = temp;
        event->result = event->b;
        event->current_step++;
    }

    if (event->current_step > event->n) {
        event->done = true;
    }
}

// Event loop to process all events
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
