#include <stdio.h>

#define MAX_EVENTS 100  

// Function to calculate Fibonacci numbers 
long long calculate_fibonacci(int n) {
    if (n <= 1) return n;

    long long a = 0, b = 1, result;
    for (int i = 2; i <= n; i++) {
        result = a + b;
        a = b;
        b = result;
    }
    return result;
}

int main() {
    int size;
    int numbers[MAX_EVENTS];

    // Input the number
    printf("Enter the number of Fibonacci calculations (max %d): ", MAX_EVENTS);
    scanf("%d", &size);

    if (size > MAX_EVENTS) {
        printf("Error: Maximum number of calculations is %d.\n", MAX_EVENTS);
        return 1;
    }

    // Input the numbers for Fibonacci
    printf("Enter the numbers to calculate Fibonacci (space-separated): ");
    for (int i = 0; i < size; i++) {
        scanf("%d", &numbers[i]);
    }

    // Perform and print Fibonacci calculations
    for (int i = 0; i < size; i++) {
        long long result = calculate_fibonacci(numbers[i]);
    }

    // Print final results summary
    printf("\nFinal Results:\n");
    for (int i = 0; i < size; i++) {
        long long result = calculate_fibonacci(numbers[i]);
        printf("Fibonacci(%d) = %lld\n", numbers[i], result);
    }

    return 0;
}
