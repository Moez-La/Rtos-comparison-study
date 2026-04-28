#pragma once
#include <mutex>
#include <chrono>

class Mutex {
private:
    std::mutex mutex_;
    
public:
    Mutex() = default;
    
    // Lock mutex
    void lock() {
        mutex_.lock();
    }
    
    // Try to lock with timeout
    bool tryLock(uint32_t timeout_ms) {
        return mutex_.try_lock_for(std::chrono::milliseconds(timeout_ms));
    }
    
    // Unlock mutex
    void unlock() {
        mutex_.unlock();
    }
    
    // RAII lock guard helper
    class Guard {
    private:
        Mutex& mutex_;
    public:
        explicit Guard(Mutex& m) : mutex_(m) {
            mutex_.lock();
        }
        ~Guard() {
            mutex_.unlock();
        }
        
        // Non-copyable
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;
    };
};
