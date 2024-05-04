
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>

#define PACKET_SIZE 64
#define TIMEOUT_SEC 1
#define TIMEOUT_USEC 0

void ping(const char* ip_addr) 
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ip_addr);

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    char packet[PACKET_SIZE];
    memset(packet, 0, PACKET_SIZE);

    struct icmp* icmp_packet = (struct icmp*) packet;
    icmp_packet->icmp_type = ICMP_ECHO;
    icmp_packet->icmp_code = 0;
    icmp_packet->icmp_cksum = 0;
    icmp_packet->icmp_id = getpid();
    icmp_packet->icmp_seq = 0;

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int bytes_sent = sendto(sock, packet, PACKET_SIZE, 0, (struct sockaddr*) &dest_addr, sizeof(dest_addr));
    if (bytes_sent == -1) 
    {
        std::cerr << "Failed to send packet" << std::endl;
        close(sock);
        return;
    }

    char recv_packet[PACKET_SIZE];
    sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);

    int bytes_received = recvfrom(sock, recv_packet, PACKET_SIZE, 0, (struct sockaddr*) &recv_addr, &recv_addr_len);
    if (bytes_received == -1) 
    {
        std::cerr << "Timeout or error occurred" << std::endl;
    } 
    else 
    {
        std::cout << "Received packet from " << inet_ntoa(recv_addr.sin_addr) << std::endl;
    }

    close(sock);
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <ip_address>" << std::endl;
        return 1;
    }

    while (true) 
    {
        ping(argv[1]);
        sleep(1);
    }

    return 0;
}
