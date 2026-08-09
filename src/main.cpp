#include "SystemInfo.h"
#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

// -- Signal handling
namespace {
    std::atomic<bool> g_running(true);

    void signalHandler(int) {
        g_running.store(false);
    }
} // anonymous namespace

// -- Helpers
namespace {
    double bytesToGB(uint64_t bytes) {
        return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
    }

    void clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    }
} // anonymous namespace

// -- Main
int main()
{
    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Create the platform-specific monitor
    std::unique_ptr<ISystemInfo> sysInfo = createSystemInfo();

    if (!sysInfo) {
        std::cerr << "Error: Could not create system info implementation for this platform." << std::endl;
        return EXIT_FAILURE;
    }

    bool firstFrame = true;
    
    while (g_running) 
    {
        if (!firstFrame) {
            clearScreen();
        }
        
        firstFrame = false;

        // Capture timestamp
        auto now = std::chrono::system_clock::now();
        const std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

        // Query all metrics
        uint64_t totalMemory = sysInfo->getTotalMemory();
        uint64_t availMemory = sysInfo->getAvailableMemory();
        float cpuUsage = sysInfo->getCpuUsage();
        float diskUage = sysInfo->getDiskUsage();

        // Display
        std::cout << "System Resource Monitor - Press Ctrl+C to exit" << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "[" << std::put_time(std::localtime(&time_t_now), "%H:%M:%S") << "]" << std::endl;

        std::cout << "CPU Usage: " << std::fixed << std::setprecision(2)
                << cpuUsage << "%" << std::endl;

        std::cout << "Memory: " << std::fixed << std::setprecision(2)
                << bytesToGB(availMemory) << " GB / "
                << bytesToGB(totalMemory) << " GB total" << std::endl;

        std::cout << "Disk Usage: " << std::fixed << std::setprecision(2) 
                << diskUage << "%" << std::endl;
        std::cout << std::endl;

        // Wait 2 seconds before next update
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "\nExiting System Resource Monitor..." << std::endl;
    return EXIT_SUCCESS;
}