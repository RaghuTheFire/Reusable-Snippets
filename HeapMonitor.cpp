#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

std::vector < int > getProcessIds() 
{
  std::vector < int > pids;
  DIR * dir = opendir("/proc");
  if (dir == nullptr) 
  {
    std::cerr << "Failed to open /proc directory" << std::endl;
    return pids;
  }

  struct dirent * entry;
  while ((entry = readdir(dir)) != nullptr) 
  {
    if (entry -> d_type == DT_DIR) 
    {
      std::string name(entry -> d_name);
      if (std::all_of(name.begin(), name.end(), ::isdigit)) 
      {
        pids.push_back(std::stoi(name));
      }
    }
  }

  closedir(dir);
  return pids;
}

long getProcessMemoryUsage(int pid) 
{
  std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
  if (!statusFile) {
    std::cerr << "Failed to open status file for process " << pid << std::endl;
    return -1;
  }

  std::string line;
  long vmSize = 0;
  while (std::getline(statusFile, line)) 
  {
    if (line.find("VmSize:") != std::string::npos
    {
      vmSize = std::stol(line.substr(line.find_last_of(" ") + 1));
      break;
    }
  }

  statusFile.close();
  return vmSize;
}

int main() 
{
  std::vector < int > pids = getProcessIds();

  for (int pid: pids) 
  {
    long memoryUsage = getProcessMemoryUsage(pid);
    if (memoryUsage != -1) 
    {
      std::cout << "Process ID: " << pid << ", Memory Usage: " << memoryUsage << " kB" << std::endl;
    }
  }

  return 0;
}
