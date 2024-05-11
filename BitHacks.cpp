
#include <stdio.h>
#include <stdint.h>

// Function to swap two integers without using a temporary variable
void swap(int *a, int *b) 
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

// Function to check if a number is a power of 2
int isPowerOfTwo(uint32_t n) 
{
    return (n != 0) && ((n & (n - 1)) == 0);
}

// Function to count the number of set bits in an integer
int countSetBits(uint32_t n) 
{
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}
