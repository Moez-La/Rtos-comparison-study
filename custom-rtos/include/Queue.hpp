#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

template<typename T>
class Queue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t max_size_;
    
public:
    explicit Queue(size_t max_size = 10) : max_size_(max_size) {}
    
    // Send item to queue (blocking if full)
    bool send(const T& item, uint32_t timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (timeout_ms > 0) {
            // Wait with timeout
            if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [this] { return queue_.size() < max_size_; })) {
                return false;  // Timeout
            }
        } else {
            // Wait indefinitely
            cv_.wait(lock, [this] { return queue_.size() < max_size_; });
        }
        
        queue_.push(item);
        cv_.notify_one();
        return true;
    }
    
    // Receive item from queue (blocking if empty)
    bool receive(T& item, uint32_t timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (timeout_ms > 0) {
            // Wait with timeout
            if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [this] { return !queue_.empty(); })) {
                return false;  // Timeout
            }
        } else {
            // Wait indefinitely
            cv_.wait(lock, [this] { return !queue_.empty(); });
        }
        
        item = queue_.front();
        queue_.pop();
        cv_.notify_one();
        return true;
    }
    
    // Check if empty
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    // Get current size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};
