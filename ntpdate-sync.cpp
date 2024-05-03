#include <iostream>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() 
{
  int sockfd;
  struct sockaddr_in servaddr;
  uint8_t packet[48] = {
    0x1B
  };
  ssize_t n;

  // Create a UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
  {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Set up the server address
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(123); // NTP port
  servaddr.sin_addr.s_addr = inet_addr("pool.ntp.org"); // NTP server address

  // Send the NTP query packet
  n = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr * ) & servaddr, sizeof(servaddr));
  if (n < 0) 
  {
    std::cerr << "Error sending packet" << std::endl;
    close(sockfd);
    return 1;
  }

  // Receive the NTP response packet
  n = recvfrom(sockfd, packet, sizeof(packet), 0, NULL, NULL);
  if (n < 0) 
  {
    std::cerr << "Error receiving packet" << std::endl;
    close(sockfd);
    return 1;
  }

  // Extract the timestamp from the packet
  uint32_t seconds = (packet[40] << 24) | (packet[41] << 16) | (packet[42] << 8) | packet[43];
  uint32_t fraction = (packet[44] << 24) | (packet[45] << 16) | (packet[46] << 8) | packet[47];
  double timestamp = seconds + fraction / 4294967296.0; // Convert fraction to double

  // Convert timestamp to human-readable format
  time_t time = timestamp - 2208988800 U; // Convert NTP time to Unix time
  char buffer[26];
  ctime_r( & time, buffer);
  std::cout << "Current time: " << buffer << std::endl;

  close(sockfd);
  return 0;
}
