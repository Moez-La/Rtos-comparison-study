cmake_minimum_required(VERSION 3.15)
project(FreeRTOS_IndustrialController)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# FreeRTOS Kernel path
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../freertos-kernel)

# FreeRTOS source files
set(FREERTOS_SOURCES
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/event_groups.c
    ${FREERTOS_KERNEL_PATH}/stream_buffer.c
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix/port.c
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_3.c
)

# Executable
add_executable(industrial_controller
    application/industrial_controller.cpp
    ${FREERTOS_SOURCES}
)

# Include directories
target_include_directories(industrial_controller PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

# Link libraries
find_package(Threads REQUIRED)
target_link_libraries(industrial_controller PRIVATE 
    Threads::Threads
    rt
)

# Compiler options
if(NOT MSVC)
    target_compile_options(industrial_controller PRIVATE 
        -Wall -Wextra -pedantic
        -DprojCOVERAGE_TEST=0
    )
endif()
