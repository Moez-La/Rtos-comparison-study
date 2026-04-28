#pragma once
#include "Task.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>

enum class SchedulingPolicy {
    PRIORITY_BASED,
    ROUND_ROBIN,
    RATE_MONOTONIC
};

class Scheduler {
private:
    std::vector<std::shared_ptr<Task>> task_list_;
    std::shared_ptr<Task> current_task_;
    SchedulingPolicy policy_;
    uint32_t system_tick_;
    uint32_t context_switch_count_;
    bool running_;
    
public:
    explicit Scheduler(SchedulingPolicy policy = SchedulingPolicy::PRIORITY_BASED)
        : current_task_(nullptr)
        , policy_(policy)
        , system_tick_(0)
        , context_switch_count_(0)
        , running_(false) {}
    
    void addTask(std::shared_ptr<Task> task) {
        task_list_.push_back(task);
        // IMPORTANT: Initialize to 0 so tasks run immediately!
        task->setNextWakeTime(0);
        sortTasks();
    }
    
    void start(uint32_t duration_ms = 0) {
        running_ = true;
        
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║       CUSTOM RTOS SCHEDULER STARTED        ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
        std::cout << "Policy: " << getPolicyName() << std::endl;
        std::cout << "Tasks: " << task_list_.size() << "\n" << std::endl;
        
        for (const auto& task : task_list_) {
            std::cout << "  - " << task->getName() 
                      << " (Priority: " << static_cast<int>(task->getPriority())
                      << ", Period: " << task->getPeriod() << "ms)" << std::endl;
        }
        std::cout << std::endl;
        
        uint32_t end_time = duration_ms;
        
        while (running_ && system_tick_ < end_time) {
            tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║       SCHEDULER STOPPED                    ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
        std::cout << "Total Runtime: " << system_tick_ << " ms" << std::endl;
        std::cout << "Context Switches: " << context_switch_count_ << "\n" << std::endl;
    }
    
    void stop() { running_ = false; }
    
private:
    void tick() {
        system_tick_++;
        
        // Find next ready task
        for (auto& task : task_list_) {
            if (task->shouldRun(system_tick_)) {
                // Context switch if needed
                if (task != current_task_) {
                    if (current_task_) {
                        current_task_->setState(TaskState::READY);
                    }
                    current_task_ = task;
                    context_switch_count_++;
                }
                
                // Execute task
                current_task_->setState(TaskState::RUNNING);
                current_task_->execute();
                current_task_->updateWakeTime(system_tick_);
                current_task_->setState(TaskState::READY);
                
                break;  // Only execute highest priority ready task per tick
            }
        }
    }
    
    void sortTasks() {
        std::sort(task_list_.begin(), task_list_.end(),
            [](const auto& a, const auto& b) {
                return a->getPriority() > b->getPriority();
            });
    }
    
    std::string getPolicyName() const {
        switch (policy_) {
            case SchedulingPolicy::PRIORITY_BASED: return "Priority-Based";
            case SchedulingPolicy::ROUND_ROBIN: return "Round-Robin";
            case SchedulingPolicy::RATE_MONOTONIC: return "Rate-Monotonic";
            default: return "Unknown";
        }
    }
};
