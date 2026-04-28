#pragma once
#include "Task.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>

enum class SchedulingPolicy {
    PRIORITY_BASED,      // Highest priority first
    ROUND_ROBIN,         // Equal time slices
    RATE_MONOTONIC       // Shorter period = higher priority
};

class Scheduler {
private:
    std::vector<std::shared_ptr<Task>> task_list_;
    std::shared_ptr<Task> current_task_;
    SchedulingPolicy policy_;
    uint32_t system_tick_;           // Milliseconds since start
    uint32_t context_switch_count_;
    bool running_;
    
    std::chrono::steady_clock::time_point start_time_;
    
public:
    explicit Scheduler(SchedulingPolicy policy = SchedulingPolicy::PRIORITY_BASED)
        : policy_(policy)
        , system_tick_(0)
        , context_switch_count_(0)
        , running_(false)
        , current_task_(nullptr) {}
    
    // Add task to scheduler
    void addTask(std::shared_ptr<Task> task) {
        task_list_.push_back(task);
        sortTasks();
    }
    
    // Start scheduler (runs for duration_ms, 0 = infinite)
    void start(uint32_t duration_ms = 0) {
        running_ = true;
        start_time_ = std::chrono::steady_clock::now();
        
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║       CUSTOM RTOS SCHEDULER STARTED        ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
        std::cout << "Policy: " << getPolicyName() << std::endl;
        std::cout << "Tasks: " << task_list_.size() << "\n" << std::endl;
        
        // Print task list
        for (const auto& task : task_list_) {
            std::cout << "  - " << task->getName() 
                      << " (Priority: " << static_cast<int>(task->getPriority())
                      << ", Period: " << task->getPeriod() << "ms)" << std::endl;
        }
        std::cout << std::endl;
        
        uint32_t end_time = duration_ms;
        
        while (running_) {
            tick();
            
            // Check if duration expired
            if (duration_ms > 0 && system_tick_ >= end_time) {
                running_ = false;
            }
            
            // Sleep 1ms (simulates 1ms tick)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║       CUSTOM RTOS SCHEDULER STOPPED        ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
        std::cout << "Total Runtime: " << system_tick_ << " ms" << std::endl;
        std::cout << "Context Switches: " << context_switch_count_ << "\n" << std::endl;
    }
    
    // Stop scheduler
    void stop() {
        running_ = false;
    }
    
    // Get system tick (milliseconds)
    uint32_t getSystemTick() const {
        return system_tick_;
    }
    
    // Get context switch count
    uint32_t getContextSwitchCount() const {
        return context_switch_count_;
    }
    
private:
    // Main scheduler tick
    void tick() {
        system_tick_++;
        
        // Select next task to run
        auto next_task = selectNextTask();
        
        if (next_task) {
            // Context switch if different task
            if (next_task != current_task_) {
                contextSwitch(next_task);
            }
            
            // Execute task
            if (current_task_->shouldRun(system_tick_)) {
                current_task_->setState(TaskState::RUNNING);
                current_task_->execute();
                current_task_->updateWakeTime(system_tick_);
                current_task_->setState(TaskState::READY);
            }
        }
    }
    
    // Context switch
    void contextSwitch(std::shared_ptr<Task> new_task) {
        auto start = std::chrono::high_resolution_clock::now();
        
        if (current_task_) {
            current_task_->setState(TaskState::READY);
        }
        
        current_task_ = new_task;
        context_switch_count_++;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        
        // Log context switch (optional, can be verbose)
        // std::cout << "[" << system_tick_ << "ms] Context Switch -> " 
        //           << new_task->getName() << " (" << latency_ns << "ns)" << std::endl;
    }
    
    // Select next task based on policy
    std::shared_ptr<Task> selectNextTask() {
        switch (policy_) {
            case SchedulingPolicy::PRIORITY_BASED:
                return selectHighestPriority();
            
            case SchedulingPolicy::ROUND_ROBIN:
                return selectRoundRobin();
            
            case SchedulingPolicy::RATE_MONOTONIC:
                return selectRateMonotonic();
            
            default:
                return nullptr;
        }
    }
    
    // Priority-based selection
    std::shared_ptr<Task> selectHighestPriority() {
        for (auto& task : task_list_) {
            if (task->shouldRun(system_tick_)) {
                return task;
            }
        }
        return nullptr;
    }
    
    // Round-robin selection
    std::shared_ptr<Task> selectRoundRobin() {
        // Simple implementation: iterate circularly
        static size_t last_index = 0;
        
        for (size_t i = 0; i < task_list_.size(); i++) {
            size_t index = (last_index + i + 1) % task_list_.size();
            if (task_list_[index]->shouldRun(system_tick_)) {
                last_index = index;
                return task_list_[index];
            }
        }
        return nullptr;
    }
    
    // Rate-monotonic selection (shorter period = higher priority)
    std::shared_ptr<Task> selectRateMonotonic() {
        std::shared_ptr<Task> selected = nullptr;
        uint32_t shortest_period = UINT32_MAX;
        
        for (auto& task : task_list_) {
            if (task->shouldRun(system_tick_)) {
                if (task->getPeriod() < shortest_period) {
                    shortest_period = task->getPeriod();
                    selected = task;
                }
            }
        }
        return selected;
    }
    
    // Sort tasks by priority (highest first)
    void sortTasks() {
        std::sort(task_list_.begin(), task_list_.end(),
            [](const auto& a, const auto& b) {
                return a->getPriority() > b->getPriority();
            });
    }
    
    // Get policy name
    std::string getPolicyName() const {
        switch (policy_) {
            case SchedulingPolicy::PRIORITY_BASED: return "Priority-Based";
            case SchedulingPolicy::ROUND_ROBIN: return "Round-Robin";
            case SchedulingPolicy::RATE_MONOTONIC: return "Rate-Monotonic";
            default: return "Unknown";
        }
    }
};
