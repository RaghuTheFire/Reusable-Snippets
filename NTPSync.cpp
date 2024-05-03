
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <cstdlib>

using namespace std;

int main() 
{
    while (true) 
    {
        // Get current time
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);

        // Format time
        char timeStr[26];
        ctime_r(&now_c, timeStr);
        timeStr[24] = '\0'; // Remove newline character

        // Sync with NTP server
        system("ntpdate -u pool.ntp.org");

        // Print synced time
        cout << "Time synced: " << timeStr << endl;

        // Wait for 1 minute before syncing again
        this_thread::sleep_for(chrono::minutes(1));
    }
    return 0;
}
