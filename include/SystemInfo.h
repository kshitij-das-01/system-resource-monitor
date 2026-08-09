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

	// CPU usage as a percentage
	// the first call will return 0
	// subsequent call return the utilization since the last call
	virtual float getCpuUsage() const = 0;

	// Disk usage as a percentage for the root filesystem
	// Defined as (total - available) / total * 100
	virtual float getDiskUsage() const = 0;
};

// Function to ensure consistent 
std::unique_ptr<ISystemInfo> createSystemInfo();
#endif // SYSTEM_INFO_H