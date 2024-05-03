
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

using namespace std;

void periodicTimer(int interval, const function<void()>& callback) 
{
    auto start = chrono::steady_clock::now();
    while (true) 
    {
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start).count();
        if (elapsed >= interval) 
        {
            callback();
            start = now;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

int main() 
{
    int interval = 1000; // 1 second
    auto callback = []() 
    {
        cout << "Timer event triggered!" << endl;
        // Dispatch event or signal here
    };

    cout << "Starting periodic timer..." << endl;
    periodicTimer(interval, callback);

    return 0;
}
