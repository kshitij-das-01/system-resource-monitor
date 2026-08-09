#ifdef __linux__
#include <SystemInfo.h>
#include <sys/sysinfo.h>
#include <unistd.h>

class LinuxSystemInfo : public ISystemInfo {
	~LinuxSystemInfo() override = default;
	
	uint64_t getTotalMemory() const override 
	{
		struct sysinfo info;
		if (sysinfo(&info) == 0) {
			return info.totalram * info.mem_unit;
		}

		return 0;
	}

	uint64_t getAvailableMemory() const override 
	{
		struct sysinfo info;
		if (sysinfo(&info) == 0) {
			return info.freeram * info.mem_unit;
		}

		return 0;
	}
};

std::unique_ptr<ISystemInfo> createSystemInfo() {
	return std::make_unique<LinuxSystemInfo>();
}
#endif // LINUX