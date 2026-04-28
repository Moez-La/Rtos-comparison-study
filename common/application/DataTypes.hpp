#pragma once
#include <cstdint>

// Sensor data structure
struct SensorData {
    float temperature;      // Celsius
    float pressure;         // Bar
    uint32_t timestamp;     // Milliseconds since start
    
    SensorData() : temperature(0.0f), pressure(0.0f), timestamp(0) {}
};

// Control command structure
struct ControlCommand {
    enum class Action {
        NONE,
        ACTIVATE_COOLING,
        ACTIVATE_HEATING,
        EMERGENCY_SHUTDOWN
    };
    
    Action action;
    float target_value;
    uint32_t timestamp;
    
    ControlCommand() 
        : action(Action::NONE), target_value(0.0f), timestamp(0) {}
};

// CAN message structure
struct CANMessage {
    uint32_t id;
    uint8_t data[8];
    uint8_t length;
    
    CANMessage() : id(0), length(0) {
        for (int i = 0; i < 8; i++) data[i] = 0;
    }
};

// System state enum
enum class SystemState {
    NORMAL,
    WARNING,
    CRITICAL,
    EMERGENCY
};
