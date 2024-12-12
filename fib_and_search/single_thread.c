#include <stdio.h>

// Function to calculate Fibonacci 
long calculate_fibonacci(int n) {
    if (n <= 1) return n;
    long a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}
