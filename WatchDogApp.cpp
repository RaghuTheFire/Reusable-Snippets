
#include <iostream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

int main() 
{
    const char* watchdog_dev = "/dev/watchdog";
    int watchdog_fd = open(watchdog_dev, O_WRONLY);

    if (watchdog_fd == -1) 
    {
        std::cerr << "Failed to open watchdog device" << std::endl;
        return 1;
    }

    std::cout << "Watchdog device opened successfully" << std::endl;

    const int timeout = 10; // Watchdog timeout in seconds
    const char* magic_str = "V";

    while (true) 
    {
        if (write(watchdog_fd, magic_str, 1) != 1) 
        {
            std::cerr << "Failed to write to watchdog device" << std::endl;
            break;
        }

        std::cout << "Watchdog timer reset" << std::endl;
        sleep(timeout / 2); // Reset watchdog timer every half of the timeout
    }

    close(watchdog_fd);
    return 0;
}
