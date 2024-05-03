
#include <ctime>
#include <iostream>

std::time_t ntpToGmtTime(uint64_t ntpTime) 
{
    const uint64_t ntpFracPerSec = 1LL << 32;
    const uint64_t ntpEpochOffset = 2208988800ULL;

    uint64_t ntpSec = ntpTime / ntpFracPerSec;
    uint64_t ntpFrac = ntpTime % ntpFracPerSec;

    std::time_t gmtTime = static_cast<std::time_t>(ntpSec - ntpEpochOffset);
    return gmtTime;
}

int main() 
{
    uint64_t ntpTime = 3785936769ULL; // Example NTP time
    std::time_t gmtTime = ntpToGmtTime(ntpTime);

    std::cout << "NTP Time: " << ntpTime << std::endl;
    std::cout << "GMT Time: " << std::ctime(&gmtTime);

    return 0;
}
