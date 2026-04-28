#pragma once
#include <cstdlib>
#include <ctime>
#include "DataTypes.hpp"

class SensorSimulator {
private:
    static bool initialized_;
    static float base_temperature_;
    static float base_pressure_;
    
public:
    static void initialize() {
        if (!initialized_) {
            srand(static_cast<unsigned>(time(nullptr)));
            base_temperature_ = 25.0f;  // Starting at 25°C
            base_pressure_ = 1.0f;      // Starting at 1.0 bar
            initialized_ = true;
        }
    }
    
    static float readTemperature() {
        initialize();
        
        // Simulate gradual temperature increase + random noise
        static int call_count = 0;
        call_count++;
        
        // Temperature increases slowly over time (simulates heating)
        base_temperature_ += 0.01f;  // +0.01°C per call
        
        // Add random noise ±2°C
        float noise = (rand() % 400 - 200) / 100.0f;
        
        // Occasionally spike to simulate critical condition
        if (call_count % 100 == 0) {
            return 85.0f + noise;  // Critical temperature spike!
        }
        
        return base_temperature_ + noise;
    }
    
    static float readPressure() {
        initialize();
        
        // Simulate pressure variation ±0.1 bar
        float noise = (rand() % 200 - 100) / 1000.0f;
        return base_pressure_ + noise;
    }
    
    static void reset() {
        base_temperature_ = 25.0f;
        base_pressure_ = 1.0f;
    }
};

// Static member initialization
bool SensorSimulator::initialized_ = false;
float SensorSimulator::base_temperature_ = 25.0f;
float SensorSimulator::base_pressure_ = 1.0f;
