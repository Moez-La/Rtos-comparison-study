#pragma once
#include <iostream>
#include <iomanip>
#include "DataTypes.hpp"

class CANSimulator {
public:
    static void sendMessage(const CANMessage& msg) {
        std::cout << "[CAN] TX: ID=0x" << std::hex << std::setfill('0') 
                  << std::setw(3) << msg.id << std::dec << " Data=[";
        
        for (uint8_t i = 0; i < msg.length; i++) {
            std::cout << "0x" << std::hex << std::setfill('0') 
                      << std::setw(2) << static_cast<int>(msg.data[i]);
            if (i < msg.length - 1) std::cout << ", ";
        }
        std::cout << std::dec << "]" << std::endl;
    }
    
    static CANMessage createEmergencyMessage() {
        CANMessage msg;
        msg.id = 0x700;  // Emergency ID
        msg.data[0] = 0xFF;  // Emergency code
        msg.data[1] = 0xEE;
        msg.length = 2;
        return msg;
    }
    
    static CANMessage createStatusMessage(uint8_t status_code) {
        CANMessage msg;
        msg.id = 0x123;  // Status ID
        msg.data[0] = status_code;
        msg.length = 1;
        return msg;
    }
};
