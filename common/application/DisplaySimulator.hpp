#pragma once
#include <iostream>
#include <iomanip>
#include <string>

class DisplaySimulator {
public:
    static void showStatus(const std::string& rtos_name, 
                          uint32_t uptime_ms,
                          float temperature,
                          float pressure,
                          const std::string& state) {
        
        std::cout << "\n┌────────────────────────────────────────────┐" << std::endl;
        std::cout << "│  System Status (" << std::left << std::setw(18) 
                  << rtos_name << ")  │" << std::endl;
        std::cout << "├────────────────────────────────────────────┤" << std::endl;
        std::cout << "│ Uptime:      " << std::setw(20) << uptime_ms << " ms  │" << std::endl;
        std::cout << "│ Temperature: " << std::setw(20) << std::fixed 
                  << std::setprecision(1) << temperature << " °C  │" << std::endl;
        std::cout << "│ Pressure:    " << std::setw(20) << std::fixed 
                  << std::setprecision(2) << pressure << " bar │" << std::endl;
        std::cout << "│ State:       " << std::setw(27) << state << "│" << std::endl;
        std::cout << "└────────────────────────────────────────────┘\n" << std::endl;
    }
    
    static void showError(const std::string& message) {
        std::cout << "\n🚨 ERROR: " << message << std::endl;
    }
    
    static void showWarning(const std::string& message) {
        std::cout << "\n⚠️  WARNING: " << message << std::endl;
    }
};
