#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Function to get the number of CPU cores
int getNumCPUCores() 
{
  ifstream cpuinfo("/proc/cpuinfo");
  string line;
  int numCores = 0;

  while (getline(cpuinfo, line)) 
  {
    if (line.find("processor") != string::npos) 
    {
      numCores++;
    }
  }

  return numCores;
}

// Function to enable or disable a CPU core
void enableDisableCPUCore(int coreNum, bool enable) 
{
  string path = "/sys/devices/system/cpu/cpu" + to_string(coreNum) + "/online";
  ofstream cpuOnline(path);

  if (cpuOnline.is_open()) 
  {
    cpuOnline << (enable ? "1" : "0");
    cpuOnline.close();
  } 
  else 
  {
    cerr << "Error: Unable to open " << path << endl;
  }
}

int main() 
{
  int numCores = getNumCPUCores();
  cout << "Number of CPU cores: " << numCores << endl;

  string input;
  int coreNum;
  bool enable;

  while (true) 
  {
    cout << "Enter 'enable <core_num>' to enable a CPU core or 'disable <core_num>' to disable a CPU core (or 'exit' to quit): ";
    getline(cin, input);

    if (input == "exit") 
    {
      break;
    }

    istringstream iss(input);
    string command;
    iss >> command;

    if (command == "enable") 
    {
      iss >> coreNum;
      if (coreNum >= 0 && coreNum < numCores) 
      {
        enableDisableCPUCore(coreNum, true);
        cout << "CPU core " << coreNum << " enabled." << endl;
      } 
      else 
      {
        cout << "Invalid core number." << endl;
      }
    } 
    else 
    if (command == "disable") 
    {
      iss >> coreNum;
      if (coreNum >= 0 && coreNum < numCores) 
      {
        enableDisableCPUCore(coreNum, false);
        cout << "CPU core " << coreNum << " disabled." << endl;
      } 
      else 
      {
        cout << "Invalid core number." << endl;
      }
    } 
    else 
    {
      cout << "Invalid command." << endl;
    }
  }

  return 0;
}
