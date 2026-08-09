#ifdef __APPLE__
#include "SystemInfo.h"
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <iostream>

// @brief Concrete Implementation of ISystemInfo using macOS APIs (sysctl).
class MacOSSystemInfo : public ISystemInfo {
public:
	~MacOSSystemInfo() override = default;

	/**
	 * Gets the total physical RAM install on the machine
	 * Uses sysctl("hw.memsize") which expects a pointer to host memory size information
	 * @return Total memory in bytes, or 0 if reading fails.
	*/
	uint64_t getTotalMemory() const override {
		// Buffer for the call result. A void* is better used here
		void* memptr = nullptr;
		// The size of the desired data format
		size_t size = sizeof(uint64_t);

		// Pass the address of pointer to hold the result
		if (sysctlbyname("hw.memsize", &memptr, &size, NULL, 0) == 0) {
			// returned value is typically castable from void* back to uint64_t*
			uint64_t total_mem = *static_cast<uint64_t*>(memptr);
			return total_mem;
		}

		std::cerr << "MacOS Error : Failed to retrieve total memory size via sysctl." << std::endl;
		return 0;
	}
	
	/**
	 * Gets the available physical RAM on the machine.
	 * Uses sysctl for approximation of system capacity reported by hardware.
	 * @return Available memory in bytes, or 0 if reading fails.
	 */
	uint64_t getAvailableMemory() const override {
		// Re-using the same mechanism as total size query.
		void* memptr = nullptr;
		size_t size = sizeof(uint64_t);

		if (sysctlbyname("hw.user_memory", &memptr, &size, NULL, 0) == 0) {
			uint64_t available_mem = *static_cast<uint64_t*>(memptr);
			return available_mem;
		}

		std::cerr << "MacOS Error: Failed to retrieve available memory size via sysctl." << std::endl;
		return 0;
	}	
};

std::unique_ptr<ISystemInfo> createSystemInfo() {
	return std::make_unique<MacOSSystemInfo>();
}
#endif // APPLE