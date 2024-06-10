#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

std::string readCPUInfo() 
{
  std::ifstream cpuinfo("/proc/cpuinfo");
  std::string line, processor_id;
  while (std::getline(cpuinfo, line)) 
  {
    if (line.find("processor") != std::string::npos) 
    {
      processor_id = line.substr(line.find(":") + 2);
      break;
    }
  }
  cpuinfo.close();
  return processor_id;
}

std::string readMemInfo() 
{
  std::ifstream meminfo("/proc/meminfo");
  std::string line, ram_id;
  while (std::getline(meminfo, line)) 
  {
    if (line.find("MemTotal") != std::string::npos) 
    {
      ram_id = line.substr(line.find(":") + 2, line.find("kB") - line.find(":") - 2);
      break;
    }
  }
  meminfo.close();
  return ram_id;
}

int generateMagicNumber(const std::string & processor_id, const std::string & ram_id) 
{
  int magic_number = std::stoi(processor_id) * std::stoi(ram_id);
  return magic_number;
}

int main() 
{
  std::string processor_id = readCPUInfo();
  std::string ram_id = readMemInfo();

  if (processor_id.empty() || ram_id.empty()) 
  {
    std::cerr << "Failed to read processor or RAM information." << std::endl;
    return 1;
  }

  int magic_number = generateMagicNumber(processor_id, ram_id);
  std::cout << "Magic Number: " << magic_number << std::endl;

  return 0;
}
