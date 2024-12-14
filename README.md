# visualized_webserver

Compile server side:

If you want to include any fucntion in another file, compile by:


` gcc server.c asyn.c common.c multi_thread_find.c event_based_io.c asyn_find.c io_operation.c single_thread.c single_thread_find.c multi_thread.c thread_based_io.c -o server.out`

Example:

` gcc server.c hw4_4.c -o server.out `


Compile client side:

` gcc client.c -o client.out `
