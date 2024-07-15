#include <iostream>
#include <string>
#include <regex>
#include <cstdlib>
#include <array>

#ifdef _WIN32
#include <Windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

std::string getMacAddress() 
{
    std::string macAddress;

#ifdef _WIN32
    IP_ADAPTER_INFO* pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    ULONG bufLen = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &bufLen) == ERROR_BUFFER_OVERFLOW) 
	  {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(bufLen);
    }

    if (GetAdaptersInfo(pAdapterInfo, &bufLen) == NO_ERROR) 
	  {
        for (IP_ADAPTER_INFO* pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) 
		    {
            if (pAdapter->Type == MIB_IF_TYPE_ETHERNET) 
			      {
                for (UINT i = 0; i < pAdapter->AddressLength; i++) 
				        {
                    char hex[3];
                    sprintf_s(hex, sizeof(hex), "%02X", pAdapter->Address[i]);
                    macAddress += hex;
                    if (i < pAdapter->AddressLength - 1) 
					          {
                        macAddress += ":";
                    }
                }
                break;
            }
        }
    }

    free(pAdapterInfo);
#else
    struct ifaddrs* ifaddr, * ifa;
    if (getifaddrs(&ifaddr) == -1) 
	  {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
	{
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_PACKET) 
		    {
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            struct ifreq ifr;
            strcpy(ifr.ifr_name, ifa->ifa_name);
            if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) 
			      {
                unsigned char* mac = reinterpret_cast<unsigned char*>(ifr.ifr_hwaddr.sa_data);
                char macStr[18];
                snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                macAddress = macStr;
                close(fd);
                break;
            }
            close(fd);
        }
    }

    freeifaddrs(ifaddr);
#endif

    return macAddress;
}

bool isValidMacAddress(const std::string& macAddress) 
{
    std::regex macRegex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    return std::regex_match(macAddress, macRegex);
}

bool isMacAddressTampered(const std::string& macAddress) 
{
    std::array<std::string, 8> commonPrefixes = 
	  {
        "00:50:56", "00:0C:29", "00:05:69", "00:1C:14",
        "00:15:5D", "00:03:FF", "00:1C:42", "00:0F:4B"
    };

    for (const auto& prefix : commonPrefixes) 
	  {
        if (macAddress.substr(0, 8) == prefix) 
		    {
            return true;
        }
    }

    return false;
}

int main() 
{
    std::string macAddress = getMacAddress();

    if (macAddress.empty()) 
	  {
        std::cout << "Failed to retrieve MAC address." << std::endl;
        return 1;
    }

    std::cout << "MAC Address: " << macAddress << std::endl;

    if (!isValidMacAddress(macAddress)) 
	  {
        std::cout << "Invalid MAC address format." << std::endl;
        return 1;
    }

    if (isMacAddressTampered(macAddress)) 
	  {
        std::cout << "The MAC address appears to be tampered or virtualized." << std::endl;
    } 
	  else 
	  {
        std::cout << "The MAC address does not appear to be tampered." << std::endl;
    }

    return 0;
}
