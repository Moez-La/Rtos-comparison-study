# 🔬 RTOS Comparison Study: Custom Implementation vs FreeRTOS

> **Comparative analysis of a custom-built RTOS (C++) versus industry-standard FreeRTOS (C) using identical industrial control system workload**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-10.6.2-orange.svg)](https://www.freertos.org/)

---

## 🎯 Project Overview

This project implements the **same real-time industrial controller** using two different RTOS approaches:

1. **Custom RTOS** - Built from scratch in modern C++17
2. **FreeRTOS** - Industry-standard RTOS in C

Both implementations control an identical 5-task system and are benchmarked across multiple performance metrics.

---

## 🏭 Industrial Control System

**Application:** Multi-sensor industrial monitoring and control

**Tasks (identical in both implementations):**

| Task | Priority | Period | Function |
|------|----------|--------|----------|
| **SensorTask** | 3 | 100ms | Acquire temperature & pressure data |
| **ControlTask** | 4 | 50ms | Process data, make control decisions |
| **CANTask** | 5 (highest) | Event-driven | Emergency CAN bus communication |
| **DisplayTask** | 2 | 200ms | Update system status display |
| **LoggingTask** | 1 (lowest) | 1000ms | Log metrics to file |

**Inter-task Communication:**
- Queue: Sensor → Control
- Mutex: Shared UART/Display
- Semaphore: CAN ready signal

---

## 📊 Comparison Metrics

### Performance
- Context switch latency
- Interrupt response time
- Task activation latency
- Jitter (timing variance)

### Resource Usage
- Code size (Flash)
- RAM usage
- Stack usage per task

### CPU Load
- Per-task CPU utilization
- Scheduler overhead
- Idle time percentage

### Code Complexity
- Lines of code
- API ergonomics
- Feature richness

---


cat > README.md << 'EOF'
# 🔬 RTOS Comparison Study: Custom Implementation vs FreeRTOS

> **Comparative analysis of a custom-built RTOS (C++) versus industry-standard FreeRTOS (C) using identical industrial control system workload**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-10.6.2-orange.svg)](https://www.freertos.org/)

---

## 🎯 Project Overview

This project implements the **same real-time industrial controller** using two different RTOS approaches:

1. **Custom RTOS** - Built from scratch in modern C++17
2. **FreeRTOS** - Industry-standard RTOS in C

Both implementations control an identical 5-task system and are benchmarked across multiple performance metrics.

---

## 🏭 Industrial Control System

**Application:** Multi-sensor industrial monitoring and control

**Tasks (identical in both implementations):**

| Task | Priority | Period | Function |
|------|----------|--------|----------|
| **SensorTask** | 3 | 100ms | Acquire temperature & pressure data |
| **ControlTask** | 4 | 50ms | Process data, make control decisions |
| **CANTask** | 5 (highest) | Event-driven | Emergency CAN bus communication |
| **DisplayTask** | 2 | 200ms | Update system status display |
| **LoggingTask** | 1 (lowest) | 1000ms | Log metrics to file |

**Inter-task Communication:**
- Queue: Sensor → Control
- Mutex: Shared UART/Display
- Semaphore: CAN ready signal

---

## 📊 Comparison Metrics

### Performance
- Context switch latency
- Interrupt response time
- Task activation latency
- Jitter (timing variance)

### Resource Usage
- Code size (Flash)
- RAM usage
- Stack usage per task

### CPU Load
- Per-task CPU utilization
- Scheduler overhead
- Idle time percentage

### Code Complexity
- Lines of code
- API ergonomics
- Feature richness

---

## 🛠️ Project Structure
- Code size (Flash)
- RAM usage
- Stack usage per task

### CPU Load
- Per-task CPU utilization
- Scheduler overhead
- Idle time percentage

### Code Complexity
- Lines of code
- API ergonomics
- Feature richness

---
---

## 🚀 Getting Started

### Prerequisites
- CMake 3.15+
- C++17 compiler (GCC 9+ / Clang 10+ / MSVC 2019+)
- Python 3.8+ (for analysis scripts)

### Build Custom RTOS
```bash
cd custom-rtos
mkdir build && cd build
cmake ..
make
./industrial_controller
```

### Build FreeRTOS Implementation
```bash
cd freertos-impl
mkdir build && cd build
cmake ..
make
./industrial_controller_freertos
```

### Run Comparison
```bash
cd comparison/scripts
python3 generate_report.py
```

---

## 📈 Results Preview

*Results will be populated after benchmark runs...*

---

## 🎓 Learning Objectives

This project demonstrates:
- ✅ RTOS internals (scheduler, context switching, synchronization)
- ✅ Modern C++ for embedded systems
- ✅ FreeRTOS API and best practices
- ✅ Performance benchmarking methodology
- ✅ Real-time system design patterns

---

## 🔮 Project Status

- [x] Phase 0: Project setup
- [ ] Phase 1: Custom RTOS implementation (C++)
- [ ] Phase 2: FreeRTOS implementation (C)
- [ ] Phase 3: Comparative analysis

---

**🚧 Under Development** - Check back for updates!

---

## 👤 Author

**Moez Chagraoui**
- 🎓 Double Degree: INP-ENSEEIHT Toulouse (ACISE) & ENIT Tunis (Electrical Engineering)
- 💼 Embedded Systems Engineer | Real-Time Software Specialist
- 📧 moezchagraoui@gmail.com
- 🔗 [LinkedIn](https://www.linkedin.com/in/moez-chagraoui) | [GitHub](https://github.com/Moez-La)

---

## 📜 License

MIT License - see LICENSE file for details
