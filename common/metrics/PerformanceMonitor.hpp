#pragma once
#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

class PerformanceMonitor {
private:
    struct TaskMetrics {
        std::string name;
        uint64_t total_execution_time_ns{0};
        uint32_t execution_count{0};
        uint64_t min_execution_ns{UINT64_MAX};
        uint64_t max_execution_ns{0};
        std::vector<uint64_t> execution_samples;
    };
    
    std::map<std::string, TaskMetrics> task_metrics_;
    uint32_t total_context_switches_{0};
    std::vector<uint64_t> context_switch_latencies_;
    std::chrono::steady_clock::time_point start_time_;
    
public:
    PerformanceMonitor() {
        start_time_ = std::chrono::steady_clock::now();
    }
    
    // Start measurement
    uint64_t startMeasurement(const std::string& task_name) {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()
        ).count();
    }
    
    // End measurement
    void endMeasurement(const std::string& task_name, uint64_t start_ns) {
        auto now = std::chrono::high_resolution_clock::now();
        uint64_t end_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()
        ).count();
        
        uint64_t duration_ns = end_ns - start_ns;
        
        auto& metrics = task_metrics_[task_name];
        metrics.name = task_name;
        metrics.total_execution_time_ns += duration_ns;
        metrics.execution_count++;
        
        if (duration_ns < metrics.min_execution_ns) {
            metrics.min_execution_ns = duration_ns;
        }
        if (duration_ns > metrics.max_execution_ns) {
            metrics.max_execution_ns = duration_ns;
        }
        
        metrics.execution_samples.push_back(duration_ns);
    }
    
    // Record context switch
    void recordContextSwitch(uint64_t latency_ns) {
        total_context_switches_++;
        context_switch_latencies_.push_back(latency_ns);
    }
    
    // Get uptime in milliseconds
    uint32_t getUptimeMs() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time_
        ).count();
    }
    
    // Print summary
    void printSummary() const {
        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║        PERFORMANCE SUMMARY                      ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝\n" << std::endl;
        
        std::cout << "Total Runtime: " << getUptimeMs() << " ms\n" << std::endl;
        
        std::cout << "Task Metrics:" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        std::cout << std::left << std::setw(15) << "Task"
                  << std::setw(10) << "Count"
                  << std::setw(12) << "Avg (μs)"
                  << std::setw(12) << "Min (μs)"
                  << std::setw(12) << "Max (μs)"
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        for (const auto& [name, metrics] : task_metrics_) {
            if (metrics.execution_count > 0) {
                double avg_us = (metrics.total_execution_time_ns / metrics.execution_count) / 1000.0;
                double min_us = metrics.min_execution_ns / 1000.0;
                double max_us = metrics.max_execution_ns / 1000.0;
                
                std::cout << std::left << std::setw(15) << name
                          << std::setw(10) << metrics.execution_count
                          << std::setw(12) << std::fixed << std::setprecision(2) << avg_us
                          << std::setw(12) << min_us
                          << std::setw(12) << max_us
                          << std::endl;
            }
        }
        
        std::cout << "\nContext Switches: " << total_context_switches_ << std::endl;
        
        if (!context_switch_latencies_.empty()) {
            uint64_t total = 0;
            uint64_t min_val = UINT64_MAX;
            uint64_t max_val = 0;
            
            for (auto latency : context_switch_latencies_) {
                total += latency;
                if (latency < min_val) min_val = latency;
                if (latency > max_val) max_val = latency;
            }
            
            double avg = total / static_cast<double>(context_switch_latencies_.size());
            
            std::cout << "  Avg Latency: " << std::fixed << std::setprecision(0) 
                      << avg << " ns" << std::endl;
            std::cout << "  Min Latency: " << min_val << " ns" << std::endl;
            std::cout << "  Max Latency: " << max_val << " ns" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Save to JSON
    void saveToJson(const std::string& filename) const {
        std::ofstream file(filename);
        
        file << "{\n";
        file << "  \"runtime_ms\": " << getUptimeMs() << ",\n";
        file << "  \"context_switches\": " << total_context_switches_ << ",\n";
        
        // Calculate avg context switch latency
        if (!context_switch_latencies_.empty()) {
            uint64_t total = 0;
            for (auto l : context_switch_latencies_) total += l;
            double avg = total / static_cast<double>(context_switch_latencies_.size());
            file << "  \"avg_context_switch_ns\": " << std::fixed 
                 << std::setprecision(0) << avg << ",\n";
        }
        
        file << "  \"tasks\": {\n";
        
        bool first = true;
        for (const auto& [name, metrics] : task_metrics_) {
            if (!first) file << ",\n";
            first = false;
            
            double avg_ns = metrics.execution_count > 0 
                ? metrics.total_execution_time_ns / static_cast<double>(metrics.execution_count)
                : 0;
            
            file << "    \"" << name << "\": {\n";
            file << "      \"count\": " << metrics.execution_count << ",\n";
            file << "      \"avg_ns\": " << std::fixed << std::setprecision(0) << avg_ns << ",\n";
            file << "      \"min_ns\": " << metrics.min_execution_ns << ",\n";
            file << "      \"max_ns\": " << metrics.max_execution_ns << "\n";
            file << "    }";
        }
        
        file << "\n  }\n";
        file << "}\n";
        
        file.close();
        std::cout << "✅ Metrics saved to: " << filename << std::endl;
    }
};
