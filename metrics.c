#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "asyn.h"
#include <sys/resource.h> 
#include <sys/time.h>

#define MAX 1000 


long calculate_fibonacci(int n);
long calculate_fibonacci_multithreaded(int n);                                             
long fibonacci_handler(int data) ;
struct rusage cpu_usage_baseline;
int total_requests = 0;
int requests_processed = 0;
int active_connections = 0;
pthread_mutex_t metrics_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t throughput_lock = PTHREAD_MUTEX_INITIALIZER;




long long get_time_in_nanoseconds() {
    struct timespec ts;
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

//get time of each function 
char *time_calculate(int connfd, long (*f)(int) ) {
    char* buff = malloc(MAX);
    bzero(buff, MAX);
    //int index;

    long long start_time = get_time_in_nanoseconds();
  
    long res = (*f)(10);
                       
    long long end_time = get_time_in_nanoseconds();
        
    // Calculate execution time
    long long execution_time = end_time - start_time;
    sprintf( buff, "Time:  %lld, Answer: %ld", execution_time, res);
    return buff;
   }

// CPU
char *log_cpu_usage() {
  char* buff = malloc(MAX);
  bzero(buff, MAX);
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

  sprintf(buff, "CPU Time Used (since reset): User = %ld.%ld sec, System = %ld.%ld sec\n",
         user_time_sec, user_time_usec, system_time_sec, system_time_usec);
 return buff;

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
    return NULL;}
    
    
char *throughput_calculate(){
	    char* buff = malloc(MAX);
   	    bzero(buff, MAX);  

            char* CPU_usage = log_cpu_usage();
	    
	    pthread_mutex_lock(&metrics_lock);
      	    total_requests++;
     	    pthread_mutex_unlock(&metrics_lock);

     	    pthread_mutex_lock(&throughput_lock);
     	    requests_processed++;
      	    pthread_mutex_unlock(&throughput_lock);
            
	    pthread_mutex_lock(&metrics_lock);
            pthread_mutex_lock(&throughput_lock);
            sprintf(buff, "%s \nTotal Requests: %d, Active Connections: %d, Throughput: %d requests/sec",
                    CPU_usage, total_requests, active_connections, requests_processed);
            pthread_mutex_unlock(&throughput_lock);
            pthread_mutex_unlock(&metrics_lock);
            

  pthread_mutex_lock(&metrics_lock);
  active_connections--;
  pthread_mutex_unlock(&metrics_lock);

 return buff;
}



char* run_metrics(){
 int meo;
 char* buff = malloc(MAX);
 bzero(buff, MAX);

 char* event = time_calculate(meo, fibonacci_handler);
 char* event_CPU = log_cpu_usage();
 reset_cpu_usage();

 char* single  = time_calculate(meo, calculate_fibonacci);
 char* single_CPU  = log_cpu_usage();
 reset_cpu_usage();

 char* multi = time_calculate(meo, calculate_fibonacci_multithreaded);
 char* multi_CPU = log_cpu_usage();
 reset_cpu_usage();
// char* event_CPU = CPU_calculate(meo, event_loop);
// char* single_CPU = CPU_calculate(meo, calculate_fibonacci);
 //char* multi_CPU = CPU_calculate(meo, calculate_fibonacci_multithreaded);

 
 sprintf(buff, "Single: %s\n %s\nMulti: %s\n %s\nAsync: %s\n %s\n ", single, single_CPU, multi, multi_CPU, event, event_CPU);
 close(meo);
 return buff;
}

//void main(){
// char* metrics = run_metrics();
// printf("%s", metrics);}
