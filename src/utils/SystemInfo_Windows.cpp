#ifdef _WIN32
#include <SystemInfo.h>
#include <windows.h>
#include <algorithm>
#include "Utils.h"

class WindowsSystemInfo : public ISystemInfo {
private:
	mutable uint64_t m_prev_idle = 0;
	mutable uint64_t m_prev_total = 0;
public:
	~WindowsSystemInfo() override = default;

	uint64_t getTotalMemory() const override {
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);

		if (GlobalMemoryStatusEx(&status)) {
			return static_cast<uint64_t>(status.ullTotalPhys);
		}

		return 0;
	}

	uint64_t getAvailableMemory() const override {
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);

		if (GlobalMemoryStatusEx(&status)) {
			return static_cast<uint64_t>(status.ullAvailPhys);
		}

		return 0;
	}

	float getCpuUsage() const override 
	{
		FILETIME idle_time, kernel_time, user_time;

		if (!GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
			return 0.0f;
		}

		auto toUint64 = [](const FILETIME& ft) -> uint64_t {
			ULARGE_INTEGER li;
			li.LowPart = ft.dwLowDateTime;
			li.HighPart = ft.dwHighDateTime;
			return li.QuadPart;
		};

		uint64_t idle = toUint64(idle_time);
		uint64_t kernel = toUint64(kernel_time);
		uint64_t user = toUint64(user_time);

		uint64_t total = kernel + user;
		uint64_t idle_diff = idle - m_prev_idle;
		uint64_t total_diff = total - m_prev_total;

		m_prev_idle = idle;
		m_prev_total = total;

		if (total_diff == 0)
			return 0.0f;

		float usage = (1.0f - static_cast<float>(idle_diff) / total_diff) * 100.0f;
		return utils::clamp(usage, 0.0f, 100.0f);
	}

	float getDiskUsage() const override 
	{
		ULARGE_INTEGER free_bytes_available, total_bytes, total_free_bytes;

		if (!GetDiskFreeSpaceEx(
				"C:\\", &free_bytes_available, &total_bytes, &total_free_bytes
		)) {
			return 0.0f;
		}

		if (total_bytes.QuadPart == 0)
			return 0.0f;
		
		float usage = (1.0f - static_cast<float>(total_free_bytes.QuadPart) / total_bytes.QuadPart) * 100.0f;
	
		return utils::clamp(usage, 0.0f, 100.0f);
	}
};
#endif // _WIN32