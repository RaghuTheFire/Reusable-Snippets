
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main() 
{
    int fd = open("/dev/watchdog", O_WRONLY);
    if (fd == -1) 
    {
        std::cerr << "Failed to open /dev/watchdog" << std::endl;
        return 1;
    }

    // Keep the watchdog timer alive by writing to it periodically
    while (true) 
    {
        if (write(fd, "\0", 1) != 1) 
        {
            std::cerr << "Failed to write to /dev/watchdog" << std::endl;
            break;
        }

        // Sleep for a certain amount of time before writing again
        sleep(10); // Example: Sleep for 10 seconds
    }

    close(fd);
    return 0;
}
