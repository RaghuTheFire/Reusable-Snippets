
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>

bool isEthernetUp() 
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) 
    {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) 
    {
        std::cerr << "Error getting interface flags: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }

    close(fd);
    return (ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING);
}

void monitorEthernetInterface() 
{
    while (true) 
    {
        if (isEthernetUp()) 
        {
            std::cout << "Ethernet interface is up and running." << std::endl;
        } 
        else 
        {
            std::cerr << "Ethernet interface is down." << std::endl;
        }
        sleep(5); // Check every 5 seconds
    }
}

int main() 
{
    monitorEthernetInterface();
    return 0;
}
