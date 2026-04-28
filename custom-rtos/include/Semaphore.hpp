#pragma once
#include <mutex>
#include <condition_variable>
#include <cstdint>

class Semaphore {
private:
    int count_;
    int max_count_;
    mutable std::mutex mutex_;  // MUTABLE for const methods
    std::condition_variable cv_;
    
public:
    Semaphore(int initial_count = 0, int max_count = 1)
        : count_(initial_count)
        , max_count_(max_count) {}
    
    bool take(uint32_t timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (timeout_ms == 0) {
            cv_.wait(lock, [this] { return count_ > 0; });
            count_--;
            return true;
        } else {
            if (cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                            [this] { return count_ > 0; })) {
                count_--;
                return true;
            }
            return false;
        }
    }
    
    void give() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ < max_count_) {
            count_++;
            cv_.notify_one();
        }
    }
    
    bool tryTake() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ > 0) {
            count_--;
            return true;
        }
        return false;
    }
    
    int getCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
};
