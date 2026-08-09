cmake_minimum_required(VERSION 3.10.0)
project(SystemMonitor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Main executable
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(include)

# Executable
add_executable(system_monitor ${SOURCES})

# Threading
find_package(Threads REQUIRED)
target_link_libraries(system_monitor PRIVATE Threads::Threads)

# Installation
install(TARGETS system_monitor DESTINATION bin)

# Tests
enable_testing()
add_executable(system_monitor_tests
	tests/test_system_info.cpp
	src/SystemInfo_Utils.cpp
)
target_include_directories(system_monitor_tests PRIVATE include)
target_link_libraries(system_monitor_tests PRIVATE Threads::Threads)
add_test(NAME system_monitor_tests COMMAND system_monitor_tests)
