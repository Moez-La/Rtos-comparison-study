#pragma once
#include <functional>
#include <string>
#include <cstdint>

enum class TaskState {
    READY,
    RUNNING,
    BLOCKED,
    SUSPENDED
};

class Task {
private:
    std::string name_;
    uint8_t priority_;
    TaskState state_;
    std::function<void()> task_function_;
    uint32_t period_ms_;
    uint32_t next_wake_time_;
    uint32_t execution_count_;
    
public:
    Task(const std::string& name, 
         uint8_t priority,
         std::function<void()> func,
         uint32_t period_ms = 0)
        : name_(name)
        , priority_(priority)
        , state_(TaskState::READY)
        , task_function_(std::move(func))
        , period_ms_(period_ms)
        , next_wake_time_(0)
        , execution_count_(0) {}
    
    void execute() {
        task_function_();
        execution_count_++;
    }
    
    bool shouldRun(uint32_t current_time) const {
        // Must be in READY state
        if (state_ != TaskState::READY) {
            return false;
        }
        
        // Event-driven task (period=0)
        // Only run once then go to BLOCKED until explicitly woken up
        if (period_ms_ == 0) {
            // If next_wake_time is in the future, task is blocked
            return (current_time >= next_wake_time_);
        }
        
        // Periodic task
        return (current_time >= next_wake_time_);
    }
    
    void updateWakeTime(uint32_t current_time) {
        if (period_ms_ > 0) {
            // Periodic: schedule next execution
            next_wake_time_ = current_time + period_ms_;
        } else {
            // Event-driven: block until explicitly woken
            // Set to maximum value so shouldRun() returns false
            next_wake_time_ = UINT32_MAX;
        }
    }
    
    // Wake up an event-driven task
    void wakeup() {
        next_wake_time_ = 0;  // Ready to run immediately
    }
    
    // Getters
    const std::string& getName() const { return name_; }
    uint8_t getPriority() const { return priority_; }
    TaskState getState() const { return state_; }
    uint32_t getPeriod() const { return period_ms_; }
    uint32_t getNextWakeTime() const { return next_wake_time_; }
    uint32_t getExecutionCount() const { return execution_count_; }
    
    // Setters
    void setState(TaskState state) { state_ = state; }
    void setNextWakeTime(uint32_t time) { next_wake_time_ = time; }
};
