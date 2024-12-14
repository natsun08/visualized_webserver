#include "common.h"

// Define the global variable
SearchInput searchInput;

// Implement the shared AcceptInput function
void AcceptInput(int* array, int size, int key) {
    searchInput.array = array;
    searchInput.size = size;
    searchInput.key = key;
}
