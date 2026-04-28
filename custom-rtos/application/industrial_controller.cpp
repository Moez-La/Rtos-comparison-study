#include "../include/Task.hpp"
#include "../include/Scheduler.hpp"
#include "../include/Queue.hpp"
#include "../include/Mutex.hpp"
#include "../include/Semaphore.hpp"
#include "../../common/application/DataTypes.hpp"
#include "../../common/application/SensorSimulator.hpp"
#include "../../common/application/CANSimulator.hpp"
#include "../../common/application/DisplaySimulator.hpp"
#include "../../common/metrics/PerformanceMonitor.hpp"

#include <iostream>
#include <memory>
#include <atomic>

Queue<SensorData> sensor_queue(10);
Mutex uart_mutex;
Semaphore can_semaphore(0, 1);
PerformanceMonitor perf_monitor;

std::atomic<float> last_temperature{25.0f};
std::atomic<float> last_pressure{1.0f};
std::atomic<SystemState> system_state{SystemState::NORMAL};

void sensorTask() {
    static uint32_t iteration = 0;
    auto start = perf_monitor.startMeasurement();
    
    SensorData data;
    data.temperature = SensorSimulator::readTemperature();
    data.pressure = SensorSimulator::readPressure();
    data.timestamp = perf_monitor.getUptimeMs();
    
    last_temperature.store(data.temperature);
    last_pressure.store(data.pressure);
    sensor_queue.send(data, 50);
    
    perf_monitor.endMeasurement("SensorTask", start);
    
    iteration++;
    if (iteration % 10 == 0) {
        std::cout << "[SENSOR] Iter " << iteration 
                  << " - T:" << data.temperature << "°C P:" << data.pressure << "bar" 
                  << std::endl;
    }
}

void controlTask() {
    auto start = perf_monitor.startMeasurement();
    SensorData data;
    
    if (sensor_queue.receive(data, 100)) {
        if (data.temperature > 80.0f) {
            system_state.store(SystemState::EMERGENCY);
            std::cout << "🚨 EMERGENCY! T:" << data.temperature << "°C" << std::endl;
            can_semaphore.give();
        } else if (data.temperature > 60.0f) {
            system_state.store(SystemState::WARNING);
        } else {
            system_state.store(SystemState::NORMAL);
        }
    }
    
    perf_monitor.endMeasurement("ControlTask", start);
}

void canTask() {
    auto start = perf_monitor.startMeasurement();
    
    if (can_semaphore.take(50)) {
        CANMessage msg = CANSimulator::createEmergencyMessage();
        std::cout << "📡 CAN sent!" << std::endl;
        CANSimulator::sendMessage(msg);
    }
    
    perf_monitor.endMeasurement("CANTask", start);
}

void displayTask() {
    auto start = perf_monitor.startMeasurement();
    
    float temp = last_temperature.load();
    float pressure = last_pressure.load();
    SystemState state = system_state.load();
    
    std::string state_str;
    switch (state) {
        case SystemState::NORMAL:    state_str = "NORMAL"; break;
        case SystemState::WARNING:   state_str = "WARNING"; break;
        case SystemState::CRITICAL:  state_str = "CRITICAL"; break;
        case SystemState::EMERGENCY: state_str = "EMERGENCY"; break;
    }
    
    std::cout << "[DISPLAY] T:" << temp << "°C P:" << pressure << " " << state_str << std::endl;
    
    perf_monitor.endMeasurement("DisplayTask", start);
}

void loggingTask() {
    static uint32_t log_count = 0;
    auto start = perf_monitor.startMeasurement();
    log_count++;
    
    if (log_count % 5 == 0) {
        perf_monitor.saveToJson("../../comparison/results/custom_rtos_metrics.json");
        std::cout << "[LOG] Saved (" << log_count << ")" << std::endl;
    }
    
    perf_monitor.endMeasurement("LoggingTask", start);
}

int main() {
    std::cout.setf(std::ios::unitbuf);
    
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║     CUSTOM RTOS - INDUSTRIAL CONTROL SYSTEM          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
    
    SensorSimulator::initialize();
    Scheduler scheduler(SchedulingPolicy::PRIORITY_BASED);
    
    std::cout << "Creating tasks..." << std::endl;
    
    auto sensor_task = std::make_shared<Task>("SensorTask", 3, sensorTask, 100);
    auto control_task = std::make_shared<Task>("ControlTask", 4, controlTask, 50);
    auto can_task = std::make_shared<Task>("CANTask", 5, canTask, 0);
    auto display_task = std::make_shared<Task>("DisplayTask", 2, displayTask, 200);
    auto logging_task = std::make_shared<Task>("LoggingTask", 1, loggingTask, 1000);
    
    scheduler.addTask(sensor_task);
    scheduler.addTask(control_task);
    scheduler.addTask(can_task);
    scheduler.addTask(display_task);
    scheduler.addTask(logging_task);
    
    std::cout << "✅ All tasks created!" << std::endl;
    
    // DEBUG: Print task states
    std::cout << "\n[DEBUG] Task states after creation:" << std::endl;
    std::cout << "  Sensor - NextWake: " << sensor_task->getNextWakeTime() << " Period: " << sensor_task->getPeriod() << std::endl;
    std::cout << "  Control - NextWake: " << control_task->getNextWakeTime() << " Period: " << control_task->getPeriod() << std::endl;
    std::cout << "  CAN - NextWake: " << can_task->getNextWakeTime() << " Period: " << can_task->getPeriod() << std::endl;
    
    std::cout << "\nStarting scheduler for 10 seconds...\n" << std::endl;
    
    scheduler.start(10000);
    
    std::cout << "\n";
    perf_monitor.printSummary();
    perf_monitor.saveToJson("../../comparison/results/custom_rtos_metrics.json");
    
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║           CUSTOM RTOS EXECUTION COMPLETE             ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}
