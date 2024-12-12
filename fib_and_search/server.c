#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <sys/resource.h> 
#include <sys/time.h> 
#include "asyn.h"
#include "asyn_find.h"
#include "multi_thread_find.h"
#include "single_thread_find.h"

#define MAX 200
#define PORT 8080
#define SA struct sockaddr

long calculate_fibonacci(int n);                              
long calculate_fibonacci_multithreaded(int n);                
void add_fibonacci_event(EventQueue* q, int id, int n);                               
void event_loop(EventQueue* q);
void initialize_queue(EventQueue* q);

EventQueue event_queue;

int server_mode = 0; // 0 = single-threaded, 1 = multi-threaded, 2 = event-driven
int total_requests = 0;
int requests_processed = 0;
int active_connections = 0;
pthread_mutex_t metrics_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t throughput_lock = PTHREAD_MUTEX_INITIALIZER;
struct rusage cpu_usage_baseline;


long long get_time_in_nanoseconds() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// CPU
void log_cpu_usage() {
  struct rusage current_usage;
  getrusage(RUSAGE_SELF, &current_usage);
  
  long user_time_sec = current_usage.ru_utime.tv_sec - cpu_usage_baseline.ru_utime.tv_sec;
  long user_time_usec = current_usage.ru_utime.tv_usec - cpu_usage_baseline.ru_utime.tv_usec;
  long system_time_sec = current_usage.ru_stime.tv_sec - cpu_usage_baseline.ru_stime.tv_sec;
  long system_time_usec = current_usage.ru_stime.tv_usec - cpu_usage_baseline.ru_stime.tv_usec;
  
  if (user_time_usec < 0) {
    user_time_usec += 1000000;
    user_time_sec -= 1;
  }
  if (system_time_usec < 0) {
    system_time_usec += 1000000;
    system_time_sec -= 1;
  }
  
  printf("CPU Time Used (since reset): User = %ld.%06ld sec, System = %ld.%06ld sec\n",
         user_time_sec, user_time_usec, system_time_sec, system_time_usec);
}


void reset_cpu_usage() {
  getrusage(RUSAGE_SELF, &cpu_usage_baseline);
  printf("CPU usage baseline reset.\n");
}

// Throughput
void *log_throughput(void *arg) {
  while (1) {
    sleep(1); 
    pthread_mutex_lock(&throughput_lock);
    printf("Throughput: %d requests/second\n", requests_processed);
    requests_processed = 0; // Reset
    pthread_mutex_unlock(&throughput_lock);
  }
  return NULL;
}

// Function designed for chat between client and server. 
void func(int connfd) {
  char buff[MAX];
  int n;
  static int event_id = 0;
  
  for (;;) {
    bzero(buff, MAX);
    
    // Read the message from client
    read(connfd, buff, sizeof(buff));
    printf("From client: %s\n", buff);
    
    // Handle client requests
    
    if (strncmp("fibonacci", buff, 9) == 0) {
      sscanf(buff + 10, "%d", &n); // Extract the number for Fibonacci
      long long start_ns = get_time_in_nanoseconds();
      
      if (server_mode == 0) { // Single-threaded
        long result = calculate_fibonacci(n);
        long long end_ns = get_time_in_nanoseconds(); 
        long long exec_time_ns = end_ns - start_ns;
        
        sprintf(buff, "Fibonacci(%d) = %ld (Execution time: %lld ns, Single-threaded)", n, result, exec_time_ns);
      } else if (server_mode == 1) { // Multi-threaded
        long result = calculate_fibonacci_multithreaded(n);
        long long end_ns = get_time_in_nanoseconds(); 
        long long exec_time_ns = end_ns - start_ns;
        
        sprintf(buff, "Fibonacci(%d) = %ld (Execution time: %lld ns, Multi-threaded)", n, result, exec_time_ns);
      } else if (server_mode == 2) { // Event-driven
        add_fibonacci_event(&event_queue, event_id++, n);
        snprintf(buff, sizeof(buff), "Fibonacci request for %d queued as event %d (Event-driven)", n, event_id - 1);
      }
      
      pthread_mutex_lock(&metrics_lock);
      total_requests++;
      pthread_mutex_unlock(&metrics_lock);
      
      pthread_mutex_lock(&throughput_lock);
      requests_processed++;
      pthread_mutex_unlock(&throughput_lock);
      
    } else if (strncmp("search", buff, 6) == 0) {
      int size;
      int array[MAX];
      
      // Read array size and elements
      read(connfd, &size, sizeof(int));
      read(connfd, array, sizeof(int) * size);
      
      // Extract key to search
      sscanf(buff + 7, "%d", &n);
      
      if (server_mode == 0) { // Single-threaded search
        AcceptInput(array, size, n);
        SingleThreadSearch();
        snprintf(buff, sizeof(buff), "Search completed (Single-threaded). Check server logs for results.");
      } else if (server_mode == 1) { // Multi-threaded search
        AcceptInput(array, size, n);
        pthread_t threads[thread_max];
        for (int i = 0; i < thread_max; i++) {
          pthread_create(&threads[i], NULL, ThreadSearch, NULL);
        }
        for (int i = 0; i < thread_max; i++) {
          pthread_join(threads[i], NULL);
        }
        OutputResult();
        snprintf(buff, sizeof(buff), "Search completed (Multi-threaded). Check server logs for results.");
      } else if (server_mode == 2) { // Event-driven search
        SearchInput task = {array, size, n};
        Enqueue(task);
        snprintf(buff, sizeof(buff), "Search request for %d queued (Event-driven).", n);
      }
    } else if (strncmp("set mode single", buff, 15) == 0) {
      server_mode = 0;
      pthread_mutex_lock(&metrics_lock);
      total_requests = 0;
      pthread_mutex_unlock(&metrics_lock);
      
      pthread_mutex_lock(&throughput_lock);
      requests_processed = 0;
      pthread_mutex_unlock(&throughput_lock);
      reset_cpu_usage();
      
      sprintf(buff, "Server mode switched to SINGLE-THREADED.");
    } else if (strncmp("set mode multi", buff, 14) == 0) {
      server_mode = 1;
      pthread_mutex_lock(&metrics_lock);
      total_requests = 0;
      pthread_mutex_unlock(&metrics_lock);
      
      pthread_mutex_lock(&throughput_lock);
      requests_processed = 0;
      pthread_mutex_unlock(&throughput_lock);
      reset_cpu_usage();
      
      sprintf(buff, "Server mode switched to MULTI-THREADED.");
    } else if (strncmp("set mode event", buff, 14) == 0) {
      server_mode = 2;
      pthread_mutex_lock(&metrics_lock);
      total_requests = 0;
      pthread_mutex_unlock(&metrics_lock);
      
      pthread_mutex_lock(&throughput_lock);
      requests_processed = 0;
      pthread_mutex_unlock(&throughput_lock);
      reset_cpu_usage();
      
      sprintf(buff, "Server mode switched to EVENT-DRIVEN.");
    } else if (strncmp("metrics", buff, 7) == 0) {
      pthread_mutex_lock(&metrics_lock);
      pthread_mutex_lock(&throughput_lock);
      sprintf(buff, "Total Requests: %d, Active Connections: %d, Throughput: %d requests/sec",
              total_requests, active_connections, requests_processed);
      pthread_mutex_unlock(&throughput_lock);
      pthread_mutex_unlock(&metrics_lock);
      log_cpu_usage(); 
    } else if (strncmp("exit", buff, 4) == 0) {
      printf("Server Exit...\n");
      break;
    } else {
      snprintf(buff, sizeof(buff), "Invalid command. Use 'fibonacci <number>', 'search <number>', 'set mode single', 'set mode multi', 'set mode event', or 'exit'.");
    }
    
    // and send that buffer to client 
    write(connfd, buff, sizeof(buff));
  }
  
  pthread_mutex_lock(&metrics_lock);
  active_connections--;
  pthread_mutex_unlock(&metrics_lock);
  
  close(connfd);
}

int main() {
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;
  
  // socket creation and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Socket creation failed...\n");
    exit(0);
  }
  printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));
  
  // assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);
  
  // Binding socket and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("Socket bind failed...\n");
    exit(0);
  }
  printf("Socket successfully binded..\n");
  
  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }
  printf("Server listening..\n");
  
  // Initialize the shared event queue
  initialize_queue(&event_queue);
  
  // Start the event loop thread
  pthread_t event_thread;
  pthread_create(&event_thread, NULL, (void*)event_loop, (void*)&event_queue);
  pthread_detach(event_thread);
  
  // Start the throughput thread
  pthread_t throughput_thread;
  pthread_create(&throughput_thread, NULL, log_throughput, NULL);
  pthread_detach(throughput_thread);
  
  len = sizeof(cli);
  
  // Accept clients and handle requests
  while (1) {
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
      printf("server accept failed...\n");
      exit(0);
    }
    printf("server accepted the client...\n");
    
    pthread_mutex_lock(&metrics_lock);
    active_connections++;
    pthread_mutex_unlock(&metrics_lock);
    
    func(connfd);
  }
  
  close(sockfd);
  return 0;
}
