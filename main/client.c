#include <arpa/inet.h>  // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // bzero()
#include <sys/socket.h>
#include <unistd.h>  // read(), write(), close()
#define MAX 1000
#define PORT 8080
#define SA struct sockaddr

// Function to handle communication with the server
void func(int sockfd) {
  char buff[MAX];
  int choice;
  
  for (;;) {
    printf("\n--- Menu ---\n");
    printf("1. Calculate Fibonacci\n");
    printf("2. Perform Search\n");
    printf("3. Set Mode (Single-Threaded)\n");
    printf("4. Set Mode (Multi-Threaded)\n");
    printf("5. Set Mode (Event-based)\n");
    printf("6. IO test\n");
    printf("7. View Metrics\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();  // Consume newline character
    
    switch (choice) {
    case 1: {  // Calculate Fibonacci
      int n;
      printf("Enter the number to calculate Fibonacci: ");
      scanf("%d", &n);
      getchar();  // Consume newline character
      
      // Prepare the command
      snprintf(buff, sizeof(buff), "fibonacci %d", n);
      write(sockfd, buff, sizeof(buff));
      break;
    }
    case 2: {  // Perform Search
      int size, key;
      printf("Enter the size of the array: ");
      scanf("%d", &size);
      int array[size];
      printf("Enter the array elements: ");
      for (int i = 0; i < size; i++) {
        scanf("%d", &array[i]);
      }
      printf("Enter the key to search: ");
      scanf("%d", &key);
      getchar();  // Consume newline character
      
      // Send the command
      snprintf(buff, sizeof(buff), "search %d", key);
      write(sockfd, buff, sizeof(buff));
      
      // Send the array size and elements
      write(sockfd, &size, sizeof(int));
      write(sockfd, array, sizeof(int) * size);
      break;
    }
    case 3:  // Set Mode (Single-Threaded)
      snprintf(buff, sizeof(buff), "set mode single");
      write(sockfd, buff, sizeof(buff));
      break;
    case 4:  // Set Mode (Multi-Threaded)
      snprintf(buff, sizeof(buff), "set mode multi");
      write(sockfd, buff, sizeof(buff));
      break;
    case 5:  // Set Mode (Event-Driven)
      snprintf(buff, sizeof(buff), "set mode event");
      write(sockfd, buff, sizeof(buff));
      break;
    case 6: 
      snprintf(buff, sizeof(buff), "I/O") ;
      write (sockfd, buff, sizeof(buff));
      break;
    case 7:  // View Metrics
      snprintf(buff, sizeof(buff), "metrics");
      write(sockfd, buff, sizeof(buff));
      break;
    case 8:  // Exit
      snprintf(buff, sizeof(buff), "exit");
      write(sockfd, buff, sizeof(buff));
      printf("Client Exit...\n");
      return;
    default:
      printf("Invalid choice. Please try again.\n");
    continue;
    }
    // Read the server's response
    bzero(buff, sizeof(buff));
    for (;;){
    read(sockfd, buff, sizeof(buff));
if (strncmp("", buff, 1)){
     break;
    } 
    }
    printf("From Server: %s\n", buff);
  }
}

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  
  // Socket creation and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Socket creation failed...\n");
    exit(0);
  } else {
    printf("Socket successfully created...\n");
  }
  bzero(&servaddr, sizeof(servaddr));
  
  // Assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);
  
  // Connect the client socket to the server socket
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("Connection with the server failed...\n");
    exit(0);
  } else {
    printf("Connected to the server...\n");
  }
  
  // Start interaction with the server
  func(sockfd);
  
  // Close the socket
  close(sockfd);
  return 0;
}
