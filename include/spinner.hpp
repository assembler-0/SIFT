#ifndef SPINNER_HPP
#define SPINNER_HPP

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

class Spinner {
public:
    Spinner(const std::string& message) : message_(message), running_(true) {
        spinner_thread_ = std::thread(&Spinner::spin, this);
    }
    
    ~Spinner() {
        stop();
    }
    
    void stop() {
        if (running_) {
            running_ = false;
            if (spinner_thread_.joinable()) {
                spinner_thread_.join();
            }
            std::cout << "\r" << message_ << " ✅ Done!\n";
        }
    }

private:
    void spin() {
        const char* frames[] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};
        int frame = 0;
        
        while (running_) {
            std::cout << "\r" << message_ << " " << frames[frame] << std::flush;
            frame = (frame + 1) % 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::string message_;
    std::atomic<bool> running_;
    std::thread spinner_thread_;
};

#endif // SPINNER_HPP