extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "semphr.h"
}

#include "../../common/application/DataTypes.hpp"
#include "../../common/application/SensorSimulator.hpp"
#include "../../common/application/CANSimulator.hpp"
#include "../../common/application/DisplaySimulator.hpp"
#include "../../common/metrics/PerformanceMonitor.hpp"

#include <iostream>
#include <atomic>

// ============================================================================
// SHARED RESOURCES
// ============================================================================

QueueHandle_t sensor_queue;
SemaphoreHandle_t uart_mutex;
SemaphoreHandle_t can_semaphore;
PerformanceMonitor perf_monitor;

std::atomic<float> last_temperature{25.0f};
std::atomic<float> last_pressure{1.0f};
std::atomic<SystemState> system_state{SystemState::NORMAL};

// ============================================================================
// TASK 1: SENSOR ACQUISITION (Priority 3, Period 100ms)
// ============================================================================

void sensorTask(void* pvParameters) {
    (void)pvParameters;
    uint32_t iteration = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    
    while(1) {
        auto start = perf_monitor.startMeasurement();
        
        SensorData data;
        data.temperature = SensorSimulator::readTemperature();
        data.pressure = SensorSimulator::readPressure();
        data.timestamp = perf_monitor.getUptimeMs();
        
        last_temperature.store(data.temperature);
        last_pressure.store(data.pressure);
        
        xQueueSend(sensor_queue, &data, pdMS_TO_TICKS(50));
        
        perf_monitor.endMeasurement("SensorTask", start);
        
        iteration++;
        if (iteration % 10 == 0) {
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            std::cout << "[SENSOR] Iter " << iteration 
                      << " - T:" << data.temperature << "°C P:" << data.pressure << "bar" 
                      << std::endl;
            xSemaphoreGive(uart_mutex);
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// TASK 2: CONTROL LOGIC (Priority 4, Period 50ms)
// ============================================================================

void controlTask(void* pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    
    while(1) {
        auto start = perf_monitor.startMeasurement();
        
        SensorData data;
        if (xQueueReceive(sensor_queue, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (data.temperature > 80.0f) {
                system_state.store(SystemState::EMERGENCY);
                
                xSemaphoreTake(uart_mutex, portMAX_DELAY);
                std::cout << "🚨 EMERGENCY! T:" << data.temperature << "°C" << std::endl;
                xSemaphoreGive(uart_mutex);
                
                xSemaphoreGive(can_semaphore);
                
            } else if (data.temperature > 60.0f) {
                system_state.store(SystemState::WARNING);
            } else {
                system_state.store(SystemState::NORMAL);
            }
        }
        
        perf_monitor.endMeasurement("ControlTask", start);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// TASK 3: CAN COMMUNICATION (Priority 5 - HIGHEST, Event-driven)
// ============================================================================

void canTask(void* pvParameters) {
    (void)pvParameters;
    
    while(1) {
        auto start = perf_monitor.startMeasurement();
        
        if (xSemaphoreTake(can_semaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            CANMessage msg = CANSimulator::createEmergencyMessage();
            
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            std::cout << "📡 CAN sent!" << std::endl;
            CANSimulator::sendMessage(msg);
            xSemaphoreGive(uart_mutex);
        }
        
        perf_monitor.endMeasurement("CANTask", start);
    }
}

// ============================================================================
// TASK 4: DISPLAY UPDATE (Priority 2, Period 200ms)
// ============================================================================

void displayTask(void* pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    
    while(1) {
        auto start = perf_monitor.startMeasurement();
        
        xSemaphoreTake(uart_mutex, portMAX_DELAY);
        
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
        
        xSemaphoreGive(uart_mutex);
        
        perf_monitor.endMeasurement("DisplayTask", start);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// TASK 5: LOGGING (Priority 1 - LOWEST, Period 1000ms)
// ============================================================================

void loggingTask(void* pvParameters) {
    (void)pvParameters;
    uint32_t log_count = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    
    while(1) {
        auto start = perf_monitor.startMeasurement();
        log_count++;
        
        if (log_count % 5 == 0) {
            perf_monitor.saveToJson("../../comparison/results/freertos_metrics.json");
            
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            std::cout << "[LOG] Saved (" << log_count << ")" << std::endl;
            xSemaphoreGive(uart_mutex);
        }
        
        perf_monitor.endMeasurement("LoggingTask", start);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
    std::cout.setf(std::ios::unitbuf);
    
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║     FreeRTOS - INDUSTRIAL CONTROL SYSTEM             ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
    
    SensorSimulator::initialize();
    
    // Create resources
    sensor_queue = xQueueCreate(10, sizeof(SensorData));
    uart_mutex = xSemaphoreCreateMutex();
    can_semaphore = xSemaphoreCreateBinary();
    
    if (sensor_queue == NULL || uart_mutex == NULL || can_semaphore == NULL) {
        std::cerr << "❌ Failed to create FreeRTOS resources!" << std::endl;
        return -1;
    }
    
    std::cout << "Creating tasks...\n";
    
    // Create tasks (Priority: 1=lowest, 5=highest)
    xTaskCreate(sensorTask,  "SensorTask",  1024, NULL, 3, NULL);
    xTaskCreate(controlTask, "ControlTask", 1024, NULL, 4, NULL);
    xTaskCreate(canTask,     "CANTask",     1024, NULL, 5, NULL);
    xTaskCreate(displayTask, "DisplayTask", 1024, NULL, 2, NULL);
    xTaskCreate(loggingTask, "LoggingTask", 1024, NULL, 1, NULL);
    
    std::cout << "✅ All tasks created!\n";
    std::cout << "Starting FreeRTOS scheduler...\n\n";
    
    // Start scheduler - runs forever
    vTaskStartScheduler();
    
    // Should never reach here
    std::cerr << "❌ Scheduler failed to start!" << std::endl;
    return -1;
}
