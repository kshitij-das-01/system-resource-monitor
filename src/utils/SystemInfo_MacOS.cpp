#ifdef __APPLE__
#include "SystemInfo.h"
#include <sys/sysctl.h>
#include <sys/statvfs.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <algorithm>
#include "Utils.h"
#include <iostream>

// @brief Concrete Implementation of ISystemInfo using macOS APIs (sysctl).
class MacOSSystemInfo : public ISystemInfo {
private:
	mutable uint64_t m_prev_total = 0;
	mutable uint64_t m_prev_idle = 0;

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
		mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
		mach_port_t host = mach_host_self();

		if (host_statistics64(host, HOST_VM_INFO64, (host_info_t)&vm_stats, &count) == KERN_SUCCESS) {
			uint64_t page_size = 0;
			size_t page_size_len = sizeof(page_size);

			if (sysctlbyname("hw.pagesize", &page_size, &page_size_len, NULL, 0) == 0) {
				return static_cast<uint64_t>(
					vm_stats.free_count + vm_stats.inactive_count + vm_stats.speculative_count
				) * page_size;
			}
		}

		std::cerr << "MacOS Error: Failed to retrieve available memory." << std::endl;
		return 0;
	}	

	float getCpuUsage() const override 
	{
		natural_t num_cpus;
		processor_info_array_t cpu_info;
		mach_msg_type_number_t num_info;
		mach_port_t host = mach_host_self();

		kern_return_t kr = host_processor_info(
			host, PROCESSOR_CPU_LOAD_INFO, &num_cpus, &cpu_info, &num_info
		);
		mach_port_deallocate(mach_task_self(), host);

		if (kr != KERN_SUCCESS)
			return 0.0f;

		uint64_t total_ticks = 0;
		uint64_t idle_ticks = 0;

		for (natural_t i = 0; i < num_cpus; i++)
		{
			natural_t* cpu_ticks = reinterpret_cast<natural_t*>(cpu_info) + (i * CPU_STATE_MAX);

			total_ticks += cpu_ticks[CPU_STATE_USER]
						+ cpu_ticks[CPU_STATE_SYSTEM]
						+ cpu_ticks[CPU_STATE_IDLE]
						+ cpu_ticks[CPU_STATE_NICE];
			idle_ticks += cpu_ticks[CPU_STATE_IDLE];
		}

		vm_size_t buf_size = static_cast<vm_size_t>(num_info) * sizeof(natural_t);
		vm_deallocate(mach_task_self(), reinterpret_cast<vm_address_t>(cpu_info), buf_size);

		uint64_t total_diff = total_ticks - m_prev_total;
		uint64_t idle_diff = idle_ticks - m_prev_idle;

		m_prev_total = total_ticks;
		m_prev_idle = idle_ticks;

		if (total_diff == 0)
			return 0.0f;
		
		float usage = (1.0f - static_cast<float>(idle_diff) / total_diff) * 100.0f;
		return utils::clamp(usage, 0.0f, 100.0f);
	}

	float getDiskUsage() const override
	{
		struct statvfs stat;
		if (statvfs("/", &stat) != 0)
			return 0.0f;

		unsigned long long total = static_cast<unsigned long long>(stat.f_blocks) * stat.f_frsize;
		unsigned long long available = static_cast<unsigned long long>(stat.f_bavail) * stat.f_frsize;

		if (total == 0)
			return 0.0f;
		
		float usage = (1.0f - static_cast<float>(available) / total) * 100.0f;
		return utils::clamp(usage, 0.0f, 100.0f);
	}
};

// std::unique_ptr<ISystemInfo> createSystemInfo() {
// 	return std::make_unique<MacOSSystemInfo>();
// }
#endif // APPLE