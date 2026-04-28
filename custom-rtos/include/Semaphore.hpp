#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>

class Semaphore {
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
    int max_count_;
    
public:
    explicit Semaphore(int initial_count = 0, int max_count = 1)
        : count_(initial_count), max_count_(max_count) {}
    
    // Give semaphore (increment counter)
    void give() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ < max_count_) {
            count_++;
            cv_.notify_one();
        }
    }
    
    // Take semaphore (decrement counter, block if zero)
    bool take(uint32_t timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (timeout_ms > 0) {
            // Wait with timeout
            if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [this] { return count_ > 0; })) {
                return false;  // Timeout
            }
        } else {
            // Wait indefinitely
            cv_.wait(lock, [this] { return count_ > 0; });
        }
        
        count_--;
        return true;
    }
    
    // Try to take without blocking
    bool tryTake() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ > 0) {
            count_--;
            return true;
        }
        return false;
    }
    
    // Get current count
    int getCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
};
