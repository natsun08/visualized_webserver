#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_EVENTS_DEFAULT 10
#define EVENT_TYPE_FIBONACCI 1

typedef struct {
  int id;
  int event_type; // Type of event
  void* event_data; // Pointer to event-specific data
  bool done;
  bool result_reported;
  long long (*handler)(void*); // Event handler function with a return type
} Event;


typedef struct {
  int n;
  long long result;
  long long a, b;
  int current_step;
} FibonacciData;

// Event Queue
typedef struct {
  Event events[MAX_EVENTS_DEFAULT];
  int front;
  int rear;
  int size;
} EventQueue;

// Initialize the event queue
void initialize_queue(EventQueue* q) {
  q->front = 0;
  q->rear = 0;
  q->size = 0;
}

// Check if the queue is empty
bool is_empty(EventQueue* q) {
  return q->size == 0;
}

// Check if the queue is full
bool is_full(EventQueue* q) {
  return q->size == MAX_EVENTS_DEFAULT;
}

// Add an event to the queue
void enqueue(EventQueue* q, Event event) {
  if (is_full(q)) {
    printf("Error: Event queue is full.\n");
    return;
  }
  q->events[q->rear] = event;
  q->rear = (q->rear + 1) % MAX_EVENTS_DEFAULT;
  q->size++;
}

// Remove an event from the queue
Event dequeue(EventQueue* q) {
  if (is_empty(q)) {
    printf("Error: Event queue is empty.\n");
    Event empty_event = {0};
    return empty_event;
  }
  Event event = q->events[q->front];
  q->front = (q->front + 1) % MAX_EVENTS_DEFAULT;
  q->size--;
  return event;
}

// Fibonacci handler
long long fibonacci_handler(void* data) {
  FibonacciData* fib_data = (FibonacciData*)data;
  
  if (fib_data->n <= 1) {
    fib_data->result = fib_data->n;
    return fib_data->result; // Return result directly for n <= 1
  }
  
  // Perform Fibonacci calculations iteratively
  while (fib_data->current_step <= fib_data->n) {
    long long temp = fib_data->a + fib_data->b;
    fib_data->a = fib_data->b;
    fib_data->b = temp;
    fib_data->current_step++;
  }
  
  fib_data->result = fib_data->b;
  return fib_data->result; // Return the result
}


void event_loop(EventQueue* q) {
  printf("Event loop started. Processing events...\n");
  fflush(stdout);
  
  while (1) {
    if (!is_empty(q)) {
      printf("Event detected in queue. Attempting to dequeue...\n");
      fflush(stdout);
      
      Event event = dequeue(q);
      
      printf("Dequeued Event ID %d. Processing...\n", event.id);
      fflush(stdout);
      
      if (!event.done && event.handler) {
        event.handler(event.event_data);
        event.done = true;
        
        if (!event.result_reported) {
          if (event.event_type == EVENT_TYPE_FIBONACCI) {
            FibonacciData* fib_data = (FibonacciData*)event.event_data;
            printf("Event ID %d: Fibonacci(%d) = %lld\n", event.id, fib_data->n, fib_data->result);
            fflush(stdout);
            free(fib_data); // Free allocated memory
          }
          event.result_reported = true;
        }
      }
    } else {
      usleep(1000); // Sleep briefly
    }
  }
}



// Add Fibonacci event
void add_fibonacci_event(EventQueue* q, int id, int n) {
  if (is_full(q)) {
    printf("Error: Cannot add event. Queue is full.\n");
    return;
  }
  
  FibonacciData* fib_data = (FibonacciData*)malloc(sizeof(FibonacciData));
  fib_data->n = n;
  fib_data->a = 0;
  fib_data->b = 1;
  fib_data->current_step = 2;
  fib_data->result = 0;
  
  Event event = {
    .id = id,
    .event_type = EVENT_TYPE_FIBONACCI,
    .event_data = fib_data,
    .done = false,
    .result_reported = false,
    .handler = fibonacci_handler
  };
  
  enqueue(q, event);
}

