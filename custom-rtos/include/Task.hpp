#pragma once
#include <functional>
#include <string>
#include <cstdint>
#include <memory>

enum class TaskState {
    READY,      // Ready to run
    RUNNING,    // Currently executing
    BLOCKED,    // Waiting for resource
    SUSPENDED   // Temporarily suspended
};

class Task {
private:
    std::string name_;
    uint8_t priority_;              // 0 = lowest, 255 = highest
    TaskState state_;
    std::function<void()> task_function_;
    uint32_t period_ms_;            // Task period (0 = no periodic execution)
    uint32_t next_wake_time_;       // When task should wake up
    uint32_t stack_size_;
    
    // Statistics
    uint32_t execution_count_;
    
public:
    Task(const std::string& name, 
         uint8_t priority,
         std::function<void()> func,
         uint32_t period_ms = 0,
         uint32_t stack_size = 1024)
        : name_(name)
        , priority_(priority)
        , state_(TaskState::READY)
        , task_function_(std::move(func))
        , period_ms_(period_ms)
        , next_wake_time_(0)
        , stack_size_(stack_size)
        , execution_count_(0) {}
    
    // Execute task function
    void execute() {
        if (state_ == TaskState::READY || state_ == TaskState::RUNNING) {
            task_function_();
            execution_count_++;
        }
    }
    
    // Check if task should run (for periodic tasks)
    bool shouldRun(uint32_t current_time) const {
        if (period_ms_ == 0) {
            // Event-driven task, runs when READY
            return state_ == TaskState::READY;
        }
        // Periodic task
        return (state_ == TaskState::READY) && (current_time >= next_wake_time_);
    }
    
    // Update next wake time (for periodic tasks)
    void updateWakeTime(uint32_t current_time) {
        if (period_ms_ > 0) {
            next_wake_time_ = current_time + period_ms_;
        }
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
