
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>

std::string processName = "your_process_name"; // Replace with the actual process name

bool isProcessRunning(const std::string& name) 
{
    std::string command = "pgrep -x " + name;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) 
    {
        return false;
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) 
    {
        result += buffer;
    }

    pclose(pipe);
    return !result.empty();
}

void restartProcess() 
{
    std::string command = "./" + processName; // Assuming the process is in the current directory
    system(command.c_str());
}

int main() 
{
    while (true) 
    {
        if (!isProcessRunning(processName)) 
        {
            std::cout << "Process " << processName << " is not running. Restarting..." << std::endl;
            restartProcess();
        } 
        else 
        {
            std::cout << "Process " << processName << " is running." << std::endl;
        }

        sleep(5); // Check every 5 seconds
    }

    return 0;
}
