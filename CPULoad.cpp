
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> split(const std::string& str, char delim) 
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) 
    {
        tokens.push_back(token);
    }
    return tokens;
}

double getCPULoad(const std::string& processName) 
{
    std::ifstream stat("/proc/stat");
    std::string line;
    std::getline(stat, line);
    std::vector<std::string> tokens = split(line, ' ');

    unsigned long long user = std::stoull(tokens[1]);
    unsigned long long nice = std::stoull(tokens[2]);
    unsigned long long system = std::stoull(tokens[3]);
    unsigned long long idle = std::stoull(tokens[4]);

    std::ifstream proc("/proc/stat");
    std::string procLine;
    while (std::getline(proc, procLine)) 
    {
        if (procLine.find(processName) != std::string::npos) 
        {
            std::vector<std::string> procTokens = split(procLine, ' ');
            unsigned long long procUser = std::stoull(procTokens[13]);
            unsigned long long procSystem = std::stoull(procTokens[14]);

            unsigned long long totalCPU = user + nice + system + idle;
            unsigned long long procCPU = procUser + procSystem;

            double cpuLoad = static_cast<double>(procCPU) / totalCPU * 100.0;
            return cpuLoad;
        }
    }

    return 0.0;
}

int main() 
{
    std::string processName = "your_process_name";
    double cpuLoad = getCPULoad(processName);
    std::cout << "CPU Load for " << processName << ": " << cpuLoad << "%" << std::endl;
    return 0;
}
