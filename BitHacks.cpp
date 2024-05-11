
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <algorithm>

//Compute the sign of an integer
int sign(int x) 
{
    return (x > 0) - (x < 0);
}

//Compute the integer absolute value (abs) without branching 
int abs_no_branching(int x) 
{
    int const mask = x >> sizeof(int) * CHAR_BIT - 1;
    return (x + mask) ^ mask;
}

//Detect if two integers have opposite signs
bool haveOppositeSign(int a, int b) 
{
    return (a * b) < 0;
}

//Compute the minimum (min) or maximum (max) of two integers without branching 
int min_no_branch(int a, int b) {
    return a + (((b - a) >> 31) & (b - a));
}

int max_no_branch(int a, int b) {
    return b - (((b - a) >> 31) & (b - a));
}



























// Swapping values with subtraction and addition 
void swap(int& a, int& b) 
{
    a = a + b;
    b = a - b;
    a = a - b;
}

//Swapping values with XOR 
void swap(int& a, int& b) 
{
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
}

// Swapping individual bits with XOR 
#include <bitset>
using namespace std;
int swapBits(int n, int i, int j) 
{
    // Create a bit mask with 1s at positions i and j
    int mask = (1 << i) | (1 << j);
    // XOR the number with the mask to toggle the bits at positions i and j
    n = n ^ mask;
    return n;
}

// Reverse bits the obvious way 
#include <iostream>
#include <bitset>
uint32_t reverseBits(uint32_t n) 
{
    uint32_t reversed = 0;
    for (int i = 0; i < 32; i++) 
    {
        reversed <<= 1;
        reversed |= (n & 1);
        n >>= 1;
    }
    return reversed;
}

// Reverse bits in word by lookup table 
#include <iostream>
#include <bitset>

uint32_t reverseBits(uint32_t n) 
{
    static const uint32_t lookupTable[16] = 
   {
        0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
        0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
    };

    uint32_t result = 0;
    for (int i = 0; i < 32; i += 4) 
    {
        result <<= 4;
        result |= lookupTable[(n >> (28 - i)) & 0xf];
    }

    return result;
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
