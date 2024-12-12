#ifndef ASYN_H
#define ASYN_H

#include <stdbool.h>

#define MAX_EVENTS_DEFAULT 10
#define EVENT_TYPE_FIBONACCI 1

typedef struct {
    int id;
    int event_type; // Type of event
    void* event_data; // Pointer to event-specific data
    bool done;
    bool result_reported;
    void (*handler)(void*); // Event handler function
} Event;

typedef struct {
    Event events[MAX_EVENTS_DEFAULT];
    int front;
    int rear;
    int size;
} EventQueue;

void initialize_queue(EventQueue* q);
bool is_empty(EventQueue* q);
bool is_full(EventQueue* q);
void enqueue(EventQueue* q, Event event);
Event dequeue(EventQueue* q);
void event_loop(EventQueue* q);
void add_fibonacci_event(EventQueue* q, int id, int n);

#endif // ASYN_H
