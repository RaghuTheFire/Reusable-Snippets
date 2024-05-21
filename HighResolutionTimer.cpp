
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <iostream>

// Generic high resolution timer
template <typename Duration = std::chrono::milliseconds, typename Callback>
class Timer 
{
public:
    Timer(Callback&& callback, Duration interval) : callback_(std::forward<Callback>(callback)), interval_(interval), running_(false) 
    {
      
    }

    ~Timer() 
    {
        stop();
    }

    void start() 
{
        running_ = true;
        worker_ = std::thread([this]() 
        {
            while (running_) 
            {
                auto start = std::chrono::high_resolution_clock::now();
                callback_();
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<Duration>(end - start);
                std::this_thread::sleep_for(interval_ > elapsed ? interval_ - elapsed : Duration::zero());
            }
        });
    }

    void stop() 
    {
        running_ = false;
        if (worker_.joinable()) 
        {
            worker_.join();
        }
    }

private:
    Callback callback_;
    Duration interval_;
    std::atomic<bool> running_;
    std::thread worker_;
};

// Example usage
void printTime() 
{
    static int count = 0;
    std::cout << "Timer fired: " << count++ << std::endl;
}

int main() 
{
    Timer<std::chrono::seconds> timer(printTime, std::chrono::seconds(2));
    timer.start();

    // Keep the program running for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}
