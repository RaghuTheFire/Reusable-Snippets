#include <iostream>
#include <regex>
#include <string>

bool isValidRTSPAddress(const std::string& address) 
{
    const std::regex rtspRegex("^rtsp://[a-zA-Z0-9.-]+(?::\\d+)?(/[a-zA-Z0-9-._~%!$&'()*+,;=:@/]*)?(\\?[a-zA-Z0-9-._~%!$&'()*+,;=:@/?]*)?$");
    return std::regex_match(address, rtspRegex);
}

int main() 
{
    std::string address1 = "rtsp://example.com:8554/stream1";
    std::string address2 = "invalid_address";

    std::cout << "Address 1 is valid: " << std::boolalpha << isValidRTSPAddress(address1) << std::endl;
    std::cout << "Address 2 is valid: " << std::boolalpha << isValidRTSPAddress(address2) << std::endl;

    return 0;
}
