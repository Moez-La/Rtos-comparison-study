#pragma once
#include <mutex>
#include <cstdint>

class Mutex {
private:
    std::mutex mutex_;
    
public:
    Mutex() = default;
    ~Mutex() = default;
    
    void lock() {
        mutex_.lock();
    }
    
    bool tryLock(uint32_t timeout_ms = 0) {
        (void)timeout_ms;  // Suppress warning
        return mutex_.try_lock();
    }
    
    void unlock() {
        mutex_.unlock();
    }
    
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
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;
    };
};
