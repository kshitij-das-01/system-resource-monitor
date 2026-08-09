cmake_minimum_required(VERSION 3.10.0)
project(SystemMonitor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Crucial for debugging: Enable debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/utils/*.cpp")

# Include directories
include_directories(include)

# Executable
add_executable(system_monitor ${SOURCES})

# Installation
install(TARGETS system_monitor DESTINATION bin)
