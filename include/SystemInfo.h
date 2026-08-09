#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <cstdint>
#include <memory>

class ISystemInfo {
	public:
	virtual ~ISystemInfo() = default;
	
	// Total available physical memory in bytes
	virtual uint64_t getTotalMemory() const = 0;
	
	// Memory currently free/available for allocation in bytes
	virtual uint64_t getAvailableMemory() const = 0;
	
	// More methods if required
};

// Function to ensure consistent 
std::unique_ptr<ISystemInfo> createSystemInfo();
#endif // SYSTEM_INFO_H