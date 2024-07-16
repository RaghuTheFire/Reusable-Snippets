// 5 common ways to manipulate hardware,using software

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>

// 1. Controlling CPU fan speed
void controlCPUFan(int speed) 
{
    std::ofstream fanFile("/sys/class/hwmon/hwmon0/pwm1");
    if (fanFile.is_open()) 
    {
        fanFile << speed;
        fanFile.close();
    }
}

// 2. Adjusting screen brightness
void adjustScreenBrightness(int brightness) 
{
    std::ofstream brightnessFile("/sys/class/backlight/intel_backlight/brightness");
    if (brightnessFile.is_open()) 
    {
        brightnessFile << brightness;
        brightnessFile.close();
    }
}

// 3. Controlling LED lights
void controlLED(bool state) 
{
    std::ofstream ledFile("/sys/class/leds/led0/brightness");
    if (ledFile.is_open()) 
    {
        ledFile << (state ? "1" : "0");
        ledFile.close();
    }
}

// 4. Changing CPU clock speed
void changeCPUClockSpeed(int frequency) 
{
    std::ofstream cpuFreqFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
    if (cpuFreqFile.is_open()) 
    {
        cpuFreqFile << frequency;
        cpuFreqFile.close();
    }
}

// 5. Controlling power management
void setPowerManagement(const std::string& mode) 
{
    std::string command = "echo " + mode + " | tee /sys/power/state";
    system(command.c_str());
}

int main() 
{
    // Example usage
    controlCPUFan(200);
    adjustScreenBrightness(50);
    controlLED(true);
    changeCPUClockSpeed(2000000);
    setPowerManagement("mem");

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
