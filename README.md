# visualized_webserver

Compile server side:

To run the server file, compile by:

` gcc server.c asyn.c common.c multi_thread_find.c event_based_io.c asyn_find.c io_operation.c single_thread.c single_thread_find.c multi_thread.c thread_based_io.c -o server.out`


Compile client side:

` gcc client.c -o client.out `

In our program, we use three problems to measure and contrast the performance of thread-based programming versus event-based programming.

- Calculating the nth element of a Fibonacci sequence.
- Searching an array for an element.
- Performing an I/O-bound task: simulating a server handling 10 simultaneous requests by loading a 2MB file 10 times, using threads and event queue.

To run an algorithm: 
1. Set the desired mode for the server: 
- Enter "3" for single-threaded mode.
- Enter "4" for multi-threaded mode.
- Enter "5" for event-based mode.
2. Choose the desired algorithm based on the menu displayed on the terminal.

To view the CPU time of the current mode (i.e., single-threaded, multi-threaded, and event-based), enter "7". 

