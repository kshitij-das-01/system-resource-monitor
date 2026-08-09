#ifdef __linux__
#include <SystemInfo.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

class LinuxSystemInfo : public ISystemInfo {
private:
	mutable unsigned long long m_prev_total = 0;
	mutable unsigned long long m_prev_idle = 0;

public:
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

	float getCpuUsage() const override 
	{
		std::ifstream file("/proc/stat");

		if (!file.is_open()) return 0.0f;

		std::string line;
		std::getline(file, line);
		std::istringstream iss(line);
		std::string cpu_label;

		unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

		iss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

		unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
		unsigned long long idle_total = idle + iowait;

		unsigned long long total_diff = total - m_prev_total;
		unsigned long long idle_diff = idle_total - m_prev_idle;

		m_prev_total = total;
		m_prev_idle = idle_total;

		if (total_diff == 0) 
			return 0.0f;

		float usage = (1.0f - static_cast<float>(idle_diff) / total_diff) * 100.0f;
		return std::clamp(usage, 0.0f, 100.0f);
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
		return std::clamp(usage, 0.0f, 100.0f);
	}
};
#endif // LINUX