#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

std::vector < std::string > getProcessList() 
{
  std::vector < std::string > processList;
  DIR * dir = opendir("/proc");
  if (dir == nullptr) 
  {
    std::cerr << "Failed to open /proc directory" << std::endl;
    return processList;
  }

  struct dirent * entry;
  while ((entry = readdir(dir)) != nullptr) 
  {
    if (entry -> d_type == DT_DIR && std::string(entry -> d_name).find_first_not_of("0123456789") == std::string::npos) 
    {
      processList.push_back(entry -> d_name);
    }
  }

  closedir(dir);
  return processList;
}

std::string getProcessStack(const std::string & pid) 
{
  std::string stackTrace;
  std::ifstream stackFile("/proc/" + pid + "/stack");
  if (stackFile.is_open()) 
  {
    std::stringstream buffer;
    buffer << stackFile.rdbuf();
    stackTrace = buffer.str();
    stackFile.close();
  } 
  else 
  {
    stackTrace = "Failed to read stack for process " + pid;
  }
  return stackTrace;
}

int main() 
{
  std::vector < std::string > processList = getProcessList();
  for (const auto & pid: processList) 
  {
    std::cout << "Process ID: " << pid << std::endl;
    std::cout << "Stack Trace:" << std::endl;
    std::cout << getProcessStack(pid) << std::endl;
    std::cout << std::endl;
  }
  return 0;
}
