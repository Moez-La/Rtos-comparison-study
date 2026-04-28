cmake_minimum_required(VERSION 3.15)
project(CustomRTOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(industrial_controller
    application/industrial_controller.cpp
)

target_include_directories(industrial_controller PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

find_package(Threads REQUIRED)
target_link_libraries(industrial_controller PRIVATE Threads::Threads)

if(NOT MSVC)
    target_compile_options(industrial_controller PRIVATE -Wall -Wextra -pedantic)
endif()
