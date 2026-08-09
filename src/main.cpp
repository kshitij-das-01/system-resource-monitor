#include "SystemInfo.h"
#include <iostream>

void displayMemoryInfo() {
    std::unique_ptr<ISystemInfo> sysInfo = createSystemInfo();

    if (!sysInfo) {
        std::cerr << "Error: Could not find system information implementation for this platform." << std::endl;
        return;
    }

    uint64_t totalMem = sysInfo->getTotalMemory();
    uint64_t availableMem = sysInfo->getAvailableMemory();

    // Convert bytes to GB for readability
    auto bytesToGB = [](uint64_t bytes) {
        return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
    };

    std::cout << "---- System Resource Monitor ----" << std::endl;
    std::cout << "Total Memory: " << bytesToGB(totalMem) << " GB" << std::endl;
    std::cout << "Available Memory: " << bytesToGB(availableMem) << " GB" << std::endl;
}

int main() {
    displayMemoryInfo();
    return 0;
}