#include <iostream>
#include <string>
#include <vector>
#include <bitset>

std::string ipv6_to_class(const std::string & ipv6_address) 
{
  std::bitset < 128 > address_bits;
  std::vector < std::string > tokens;
  std::string token;

  // Convert IPv6 address to binary representation
  for (char c: ipv6_address) 
  {
    if (c == ':') 
    {
      tokens.push_back(token);
      token.clear();
    } 
    else 
    {
      token += c;
    }
  }
  tokens.push_back(token);

  for (const auto & token: tokens)
  {
    std::bitset < 16 > word(token, 16);
    address_bits <<= 16;
    address_bits |= word;
  }

  // Determine the address class
  if (address_bits.test(0)) 
  {
    return "Unspecified";
  } 
  else 
  if (address_bits.test(1)) 
  {
    return "Loopback";
  } 
  else
  if (!address_bits.test(3) && !address_bits.test(4)) 
  {
    return "Link-local";
  } 
  else
  if (!address_bits.test(3) && address_bits.test(4)) 
  {
    return "Site-local";
  } 
  else 
  {
    return "Global";
  }
}

int main() 
{
  std::string ipv6_address;
  std::cout << "Enter an IPv6 address: ";
  std::cin >> ipv6_address;

  std::string address_class = ipv6_to_class(ipv6_address);
  std::cout << "The IPv6 address " << ipv6_address << " belongs to the " << address_class << " class." << std::endl;

  return 0;
}
