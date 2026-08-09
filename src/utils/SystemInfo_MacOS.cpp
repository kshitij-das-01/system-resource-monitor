#ifdef __APPLE__
#include "SystemInfo.h"
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <iostream>

// @brief Concrete Implementation of ISystemInfo using macOS APIs (sysctl).
class MacOSSystemInfo : public ISystemInfo {
public:
	~MacOSSystemInfo() override = default;

	uint64_t getTotalMemory() const override {
		uint64_t total_mem = 0;
		size_t size = sizeof(total_mem);

		// Pass the address of pointer to hold the result
		if (sysctlbyname("hw.memsize", &total_mem, &size, NULL, 0) == 0) {
			return total_mem;
		}

		std::cerr << "MacOS Error : Failed to retrieve total memory size via sysctl." << std::endl;
		return 0;
	}
	
	uint64_t getAvailableMemory() const override {
		vm_statistics64 vm_stats;
		size_t size = sizeof(vm_stats);

		if (sysctlbyname("vm.statistics64", &vm_stats, &size, NULL, 0) == 0) {
			uint64_t page_size = 0;
			size_t page_size_len = sizeof(page_size);

			if (sysctlbyname("vm.page_size", &page_size, &page_size_len, NULL, 0) == 0) {
				return (uint64_t) (vm_stats.free_count) * page_size;
			}
		}

		std::cerr << "MacOS Error: Failed to retrieve available memory." << std::endl;
		return 0;
	}	
};

// std::unique_ptr<ISystemInfo> createSystemInfo() {
// 	return std::make_unique<MacOSSystemInfo>();
// }
#endif // APPLE