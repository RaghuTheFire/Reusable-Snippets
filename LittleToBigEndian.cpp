
#include <iostream>
#include <cstdint>

// Function to swap bytes for int
uint32_t swapBytesInt(uint32_t value) 
{
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

// Function to swap bytes for short
uint16_t swapBytesShort(uint16_t value) 
{
    return ((value & 0x00FF) << 8) |
           ((value & 0xFF00) >> 8);
}

// Function to swap bytes for float
float swapBytesFloat(float value) 
{
    uint32_t temp = swapBytesInt(*(uint32_t*)&value);
    return *(float*)&temp;
}

// Function to swap bytes for double
double swapBytesDouble(double value) 
{
    uint64_t temp = 0;
    uint32_t* ptr = (uint32_t*)&value;
    uint32_t* tempPtr = (uint32_t*)&temp;

    tempPtr[0] = swapBytesInt(ptr[1]);
    tempPtr[1] = swapBytesInt(ptr[0]);

    return *(double*)&temp;
}

// Function to swap for 64 bit data
uint64_t swapBytesInt64(uint64_t value) 
{
    uint64_t result = 0;
    result |= (value & 0x00000000000000FFULL) << 56;
    result |= (value & 0x000000000000FF00ULL) << 40;
    result |= (value & 0x0000000000FF0000ULL) << 24;
    result |= (value & 0x00000000FF000000ULL) << 8;
    result |= (value & 0x000000FF00000000ULL) >> 8;
    result |= (value & 0x0000FF0000000000ULL) >> 24;
    result |= (value & 0x00FF000000000000ULL) >> 40;
    result |= (value & 0xFF00000000000000ULL) >> 56;
    return result;
}

int main() 
{
    int intValue = 0x01020304;
    short shortValue = 0x0102;
    float floatValue = 3.14f;
    double doubleValue = 3.14159265358979;
    uint64_t longlongvalue = 0x0123456789ABCDEF; 

    std::cout << "Original values:" << std::endl;
    std::cout << "int: " << intValue << std::endl;
    std::cout << "short: " << shortValue << std::endl;
    std::cout << "float: " << floatValue << std::endl;
    std::cout << "double: " << doubleValue << std::endl;
    std::cout << "longlongvalue: " << longlongvalue << std::endl;

    uint32_t swappedInt = swapBytesInt(*(uint32_t*)&intValue);
    uint16_t swappedShort = swapBytesShort(*(uint16_t*)&shortValue);
    float swappedFloat = swapBytesFloat(floatValue);
    double swappedDouble = swapBytesDouble(doubleValue);
    uint64_t swap64Value = swapBytesInt64(longlongvalue);

    std::cout << "\nSwapped values:" << std::endl;
    std::cout << "int: " << *(int*)&swappedInt << std::endl;
    std::cout << "short: " << *(short*)&swappedShort << std::endl;
    std::cout << "float: " << swappedFloat << std::endl;
    std::cout << "double: " << swappedDouble << std::endl;
    std::cout << "longlongvalue: " << swap64Value << std::endl;

    return 0;
}
