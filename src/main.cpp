#include "SystemMonitor.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    SystemMonitor monitor;

    if (!monitor.initialize()) {
        return EXIT_FAILURE;
    }

    while (monitor.isRunning()) {
        monitor.refresh();
        monitor.display();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n Exiting System Resource Monitor..." << std::endl;
    return EXIT_SUCCESS;
}