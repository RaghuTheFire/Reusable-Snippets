#include <ctime>
#include <iostream>

std::time_t ntpToUtc(uint64_t ntpTime) 
{
    const uint64_t ntpFracPerSec = 0x100000000; // 2^32
    const uint64_t ntpEpochOffset = 0x83AA7E80; // 2208988800 (seconds between 01/01/1900 and 01/01/1970)

    uint64_t ntpSecs = ntpTime / ntpFracPerSec;
    uint64_t ntpFrac = ntpTime % ntpFracPerSec;

    std::time_t utcTime = static_cast<std::time_t>(ntpSecs - ntpEpochOffset);
    return utcTime;
}

int main() 
{
    uint64_t ntpTime = 0x83AA7F7F0145C000; // Example NTP timestamp
    std::time_t utcTime = ntpToUtc(ntpTime);

    std::cout << "NTP Time: " << ntpTime << std::endl;
    std::cout << "UTC Time: " << utcTime << " (" << std::ctime(&utcTime) << ")" << std::endl;

    return 0;
}
