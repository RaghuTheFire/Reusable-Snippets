#include <iostream>

int main() 
{
    #if defined(__x86_64__) || defined(_M_X64)
        std::cout << "64-bit processor detected." << std::endl;
    #elif defined(__i386__) || defined(_M_IX86)
        std::cout << "32-bit processor detected." << std::endl;
    #else
        std::cout << "Unknown processor architecture." << std::endl;
    #endif
    return 0;
}
